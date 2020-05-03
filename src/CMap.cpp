/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
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
	if (!self.LoadEntities(in))
		return in;
	self.PlaceTroops();
	return in;
}

void CMap::PlaceTroops()
{
	for (const auto & troop : m_Troops)
	{
		m_Map.insert({troop->GetPosition(), troop->GetTile()});
	}
}

istream & CMap::LoadMapInfo(istream & in)
{
	return in >> m_Rows >> m_Cols >> m_Gate;
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
	
	if (!FindPathsFromSpawn())
		in.setstate(ios::failbit);
	return in;
}

istream & CMap::LoadEntities(istream & in)
{
	if (!m_UnitStack)
		throw runtime_error("Unit stack not defined.");
	
	while (true)
	{
		// check first character
		char ch;
		if (!(in >> ch))
		{
			if (in.eof())
				in.clear(ios::goodbit);
			return in;
		}
		
		if (m_UnitStack->IsTrooperChar(ch))
		{
			if (!LoadTroops(in, ch))
				return in;
		}
		else if (m_UnitStack->IsTowerChar(ch))
		{
			if (!LoadTowers(in, ch))
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
	CTrooper * trooper = m_UnitStack->CreateTrooperAt(ch);
	if (!trooper->LoadOnMap(in))
	{
		cout << "fail loading " << ch << endl;
		return in;
	}
	m_Troops.push_back(trooper);
	if (m_Map.count(trooper->GetPosition()))
	{
		in.setstate(ios::failbit);
		return in;
	}
	auto path = CPath{m_Map, m_Rows, m_Cols, trooper->GetPosition(), m_Gate.Position()}.FindStraightPath();
	trooper->SetPath(path);
	return in;
}

istream & CMap::LoadTowers(istream & in, char ch)
{
	CTower * tower = m_UnitStack->CreateTowerAt(ch);
	if (!tower->LoadOnMap(in))
		return in;
	m_Towers.push_back(tower);
	if (!m_Map.count(tower->GetPosition()) || !m_Map.at(tower->GetPosition()).IsWall())
		in.setstate(ios::failbit);
	m_Map.at(tower->GetPosition()) = tower->GetTile();
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
		if (m_UnitStack->IsTowerChar(ch))
		{
			if (!LoadEntity({col, row}, ch))
			{
				in.setstate(ios::failbit);
				return in;
			}
		}
		else if (!LoadWallChar(ch, {col, row}))
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
}

istream & CMap::DeleteWs(istream & in)
{
	char ch = 0;
	if (!(in >> ch))
		return in;
	in.putback(ch);
	return in;
}

bool CMap::InitGatePosition(pos_t position)
{
	if (m_Gate.Position() != pos::npos)
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

bool CMap::CheckNew() const
{
	return m_Troops.empty()
		&& m_Towers.empty()
		&& m_Gate.Full()
		&& CheckSaved();
}

bool CMap::CheckSaved() const
{
	return !(m_Gate.Position() == pos::npos)
		&& !m_Spawns.empty();
}
/**********************************************************************************************************************/
// SAVING
ostream & operator<<(ostream & out, const CMap & self)
{
	
	if (!self.SaveMapInfo(out))
		return out;
	if (!self.SaveMap(out))
		return out;
	if (!self.SaveEntities(out))
		return out;
	return out << endl;
}

ostream & CMap::SaveMapInfo(ostream & out) const
{
	return out << "(M)" << endl << m_Rows << ' ' << m_Cols << ' ' << m_Gate << endl;
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
				line += m_Map.at({j, i}).GetChar();
		}
		if (!(out << line << endl))
			return out;
	}
	return out;
}

ostream & CMap::SaveEntities(ostream & out) const
{
	// save troops
	for (const auto & troop : m_Troops)
		if (!(troop->SaveOnMap(out) << endl))
			return out;
		
	for (const auto & tower : m_Towers)
		if (!(tower->SaveOnMap(out) << endl))
			return out;
	
	return out;
}

/**********************************************************************************************************************/
// RENDER
ostream & CMap::Render(ostream & out) const
{
	if (!m_Gate.Render(out))
		return out;
	if(!RenderMap(out))
		return out;
	return out;
}

ostream & CMap::RenderMap(ostream & out) const
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
		if (!(out << line << endl))
			return out;
	}
	return out;
}

/**********************************************************************************************************************/
// UPDATE
bool CMap::Update(bool & waveOn)
{
	if (!MoveTroops())
		waveOn = false;
	if (m_Gate.Fallen())
		return false;
	if (!TowerAttack())
		waveOn = false;
	return true;
}

void CMap::Spawn(const vector<CTrooper*> & spawns)
{
	for (auto trooper : spawns)
	{
		trooper->SetPath(m_Paths.at(m_Spawns.at(trooper->GetSpawn())));
		trooper->Spawn(m_Map);
		auto it = lower_bound(m_Troops.begin(), m_Troops.end(), trooper,
				[](const CTrooper * a, const CTrooper * b){return a->DistanceToGoal() < b->DistanceToGoal();});
		m_Troops.insert(it, trooper);
	}
}

bool CMap::FindPathsFromSpawn()
{
	for (const auto & position : m_Spawns)
	{
		auto path = CPath{m_Map, m_Rows, m_Cols, position.second, m_Gate.Position()}.FindStraightPath();
		if (path.empty())
			return false;
		m_Paths.insert({position.second, path});
	}
	return true;
}

bool CMap::MoveTroops()
{
	bool ret = true;
	for (auto troop = m_Troops.begin(); troop != m_Troops.end();)
	{
		if (!(*troop)->Move(m_Map))
		{
			troop++;
			continue;
		}
		m_Gate.ReceiveDamage((*troop)->Attack());
		delete *troop;
		troop = m_Troops.erase(troop);
		if (m_Troops.empty())
			ret = false;
	}
	return ret;
}

bool CMap::TowerAttack()
{
	// map troopers to their positions
	unordered_map<pos_t,CTrooper*> troops;
	for (auto & troop : m_Troops)
		troops.insert({troop->GetPosition(), troop});
	
	// attack troops by towers
	for (auto tower : m_Towers)
		tower->Attack(m_Map, m_Rows, m_Cols, troops);
	
	return CheckTroopsDeaths();
}

bool CMap::CheckTroopsDeaths()
{
	bool ret = true;
	// check if a trooper has died - if so delete him from map
	for (auto troop = m_Troops.begin(); troop != m_Troops.end();)
	{
		if (!(*troop)->Died())
		{
			troop++;
			continue;
		}
		m_Map.erase((*troop)->GetPosition());
		delete *troop;
		troop = m_Troops.erase(troop);
		if (m_Troops.empty())
			ret = false;
	}
	return ret;
}

map<int, bool> CMap::SpawnsFree() const
{
	map<int, bool> res;
	for (const auto & spawn : m_Spawns)
	{
		pos_t spawnPos = m_Paths.at(spawn.second).front();
		res.insert({spawn.first, !m_Map.count(spawnPos)});
	}
	return res;
}

/**********************************************************************************************************************/
// TESTINGc
void CMap::VisualizePath(pos_t start, pos_t goal)
{
	auto path = CPath{m_Map, m_Rows, m_Cols, start, goal}.FindStraightPath();
	while (!path.empty())
	{
		CTile tile = CTile(' ');
		m_Map.insert({path.front(), tile});
		path.pop_front();
	}
}