/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include <cstring>
#include <iostream>
#include <fstream>
#include <zconf.h>
#include "CMap.hpp"
#include "CMageTower.hpp"

using namespace std;
/**********************************************************************************************************************/
// INIT
CMap::CMap()
	: m_Cols(0),
	  m_Rows(0)
{}

CMap::~CMap()
{
	for (auto & troop : m_Troops)
		delete troop;
	for (auto & tower : m_Towers)
		delete tower;
}

void CMap::AssignUnitStack(shared_ptr<CUnitStack> unitStack)
{
	m_UnitStack = move(unitStack);
}

/**********************************************************************************************************************/
// LOADING

istream & operator>>(istream & in, CMap & self)
{
	if (!self.LoadMapInfo(in))
		return in;
	if (!self.LoadMap(in))
		return in;
	return self.LoadEntities(in);
}

istream & CMap::LoadMapInfo(istream & in)
{
	char del1 = 0, del2 = 0, del3 = 0;
	if (!(in >> m_Rows >> del1 >> m_Cols >> del2 >> m_Gate >> del3)
		|| del1 != ','
		|| del2 != ','
		|| del3 != ';')
		in.setstate(ios::failbit);
	return in;
}

istream & CMap::LoadMap(istream & in)
{
	if (!LoadWallLine(in, 0))
		return in;
		
	for (int row = 1; row < m_Rows - 1; ++row)
		if (!LoadMapCenter(in, row))
			return in;
	
	if(!LoadWallLine(in, m_Rows - 1))
		return in;

	return in;
}

istream & CMap::LoadEntities(istream & in)
{
	while (true)
	{
		// check first character
		char ch;
		if (!(in >> ch))
			return in;
		
		if (m_UnitStack->IsTrooperChar(ch))
		{
			if (!LoadTroops(in))
				return in;
		}
		else if (m_UnitStack->IsTowerChar(ch))
		{
			if (!LoadTowers(in))
				return in;
		}
		else
		{
			in.putback(ch);
			return in;
		}
	}
}

istream & CMap::LoadTroops(istream & in, char ch)
{
	// skip comma
	char comma;
	if (!(in >> comma)
		|| comma != ',')
	{
		in.setstate(ios::failbit);
		return in;
	}
	
	CTrooper * trooper = m_UnitStack->CreateTrooperAt(ch);
	if (!trooper->LoadOnMap(in))
		return in;
	m_Troops.push_back(trooper);
	m_Map.insert({trooper->GetPosition(), trooper->GetTile()});
	return in;
}

istream & CMap::LoadTowers(istream & in, char ch)
{
	// skip comma
	char comma;
	if (!(in >> comma)
		|| comma != ',')
	{
		in.setstate(ios::failbit);
		return in;
	}
	
	CTower * tower = m_UnitStack->CreateTowerAt(ch);
	if (!tower->LoadOnMap(in))
		return in;
	m_Towers.push_back(tower);
	m_Map.insert({tower->GetPosition(), tower->GetTile()});
	return in;
}

istream & CMap::LoadWallLine(istream & in, int row)
{
	// skip whitespaces until first character and return the character itself back
	char ch = 0;
	if (!DeleteWs(in))
		return in;
	
	// load the wall line
	for (int col = 0; col < m_Cols; ++col)
	{
		if (!(in.get(ch)))
			return in;
		if (!LoadWallChar(ch, {col, row}))
		{
			in.setstate(ios::failbit);
			return in;
		}
	}
	return in;
}

istream & CMap::LoadMapCenter(istream & in, int row)
{
	// read left wall boundary - skip enter
	DeleteWs(in);
	char ch = 0;
	if (!in.get(ch))
		return in;
	if (!LoadWallChar(ch, {0, row}))
	{
		in.setstate(ios::failbit);
		return in;
	}
	
	// load characters inside map
	for (int col = 1; col < m_Cols - 1; ++col)
	{
		if (!in.get(ch))
			return in;
		if (!LoadCenterChar(ch, {col, row}))
		{
			in.setstate(ios::failbit);
			return in;
		}
	}
	
	// load right wall boundary
	if (!in.get(ch))
		return in;
	if (!LoadWallChar(ch, {m_Cols - 1, row}))
	{
		in.setstate(ios::failbit);
		return in;
	}
	return in;
}

bool CMap::LoadWallChar(char ch, pos_t position)
{
	CTile tile{ch};
	if (tile.IsSpawn())
	{
		if (!InitSpawner(position, ch))
			return false;
	}
	else if (tile.IsGate())
	{
		if (!InitGatePosition(position))
			return false;
	}
	else if (!tile.IsWall())
		return false;
	m_Map.insert({position, tile});
	return true;
}

bool CMap::LoadCenterChar(char ch, pos_t position)
{
	// check for empty character and skip troops
	if (ch == ' ')
		return true;
	
	// try wall characters first
	if (LoadWallChar(ch, position))
		return true;
	
	// if it's not load entity
	else
		return LoadEntity(position, ch);
	
	// insert to map
	m_Map.insert({position, CTile{ch}});
	return true;
}

istream & CMap::DeleteWs(istream & in)
{
	char ch = 0;
	if (!(in >> ch))
		return in;
	in.putback(ch);
	return in;
}

void CMap::InitTroops()
{
	for (auto & troop : m_Troops)
		troop->SetPath(m_Paths.at(troop->GetSpawn()));
}

bool CMap::InitGatePosition(pos_t position)
{
	if (m_Gate.Position() != pos_t::npos)
		return false;
	m_Gate.Position() = position;
	return true;
}

bool CMap::LoadEntity(pos_t position, char ch)
{
	if (!m_UnitStack)
		throw runtime_error("Unit stack not defined.");
	
	// replace tower with walls
	if (m_UnitStack->IsTowerChar(ch))
		m_Map.insert({position, CTile{'#'}});
	// skip troops in the loading phase
	else if (!m_UnitStack->IsTrooperChar(ch))
		return false;
	return true;
}

bool CMap::InitSpawner(pos_t position, char ch)
{
	// redefinition is not allowed
	if (m_Spawns.count(ch - '0'))
		return false;
	m_Spawns.insert({{ch - '0', position}});
	return true;
}

bool CMap::CheckSpawnCount(int count) const
{
	int max = 0;
	for (const auto & it : m_Spawns)
		if (max < it.first)
			max = it.first;
	return max == count;
}

/**********************************************************************************************************************/
// SAVING
ostream & operator<<(ostream & out, const CMap & self)
{
	
	if (!(self.SaveMapInfo(out) << self.m_Gate))
		return out;
	if (!self.SaveMap(out))
		return out;
	if (!self.SaveTroops(out))
		return out;
	return out;
}

ostream & CMap::SaveMapInfo(ostream & out) const
{
	return out << "(M): " << m_Rows << ", " << m_Cols << ", " << m_Gate << ';' << endl;
}

ostream & CMap::SaveMap(ostream & out) const
{
	for (int i = 0; i < m_Rows; ++i)
	{
		string line;
		for (int j = 0; j < m_Cols; ++j)
		{
			if (!m_Map.count({j,i}) || m_Map.at({j,i}).IsTroop())	// skip troops while saving the map
				line += ' ';
			else
				line += m_Map.at({j, i}).GetRawChar();
		}
		if (!(out << line << endl))
			return out;
	}
	return out;
}

ostream & CMap::SaveTroops(ostream & out) const
{
	// save troops
	for (const auto & troop : m_Troops)
		if (!(troop->SaveOnMap(out) << ';' << endl))
			return out;
	
	return out;
}

/**********************************************************************************************************************/
// RENDER
void CMap::Render() const
{
	m_Gate.Render();
	RenderMap();
}

void CMap::RenderMap() const
{
	for (int i = 0; i < m_Rows; ++i)
	{
		string line;
		for (int j = 0; j < m_Cols; ++j)
		{
			if (!m_Map.count({j,i}))
				line += ' ';
			else
				line += m_Map.at({j, i}).PrintChar();
		}
		cout << line << endl;
	}
}

/**********************************************************************************************************************/
// UPDATE
void CMap::Spawn(CTrooper * trooper)
{
	trooper->SetPath(m_Paths.at(m_Spawns.at(trooper->GetSpawn())));
	m_Troops.emplace_back(trooper);
}

bool CMap::Update(bool & waveOn)
{
	MoveTroops(waveOn);
	if (m_Gate.Fallen())
		return false;
	TowerAttack();
	return true;
}

void CMap::FindPaths()
{
	for (const auto & position : m_Spawns)
	{
		CPath path{m_Map, m_Rows, m_Cols, position.second, m_Gate.Position()};
		m_Paths.insert({position.second, path.FindPath()});
	}
}

void CMap::MoveTroops(bool & waveOn)
{
	for (auto troop : m_Troops)
	{
		if (!troop->Move(m_Map))
			continue;
		m_Gate.ReceiveDamage(troop->Attack());
		delete m_Troops.front();
		m_Troops.pop_front();
		if (m_Troops.empty())
			waveOn = false;
	}
}

void CMap::TowerAttack()
{
	// map troopers to their positions
	unordered_map<pos_t,CTrooper*> troops;
	for (auto & troop : m_Troops)
		troops.insert({troop->GetPosition(), troop});
	
	// attack troops by towers
	for (auto tower : m_Towers)
		tower->Attack(m_Map, m_Rows, m_Cols, troops);
}

/**********************************************************************************************************************/
// TESTING
void CMap::VisualizePath(std::queue<pos_t> path)
{
	while (!path.empty())
	{
		CTile tile = CTile(' ');
		m_Map.insert({path.front(), tile});
		path.pop();
	}
}
