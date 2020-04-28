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

istream & CMap::LoadDimensions(istream &in)
{
	char del1 = 0, del2 = 0;
	if (!(in >> m_Rows >> del1 >> m_Cols >> del2)
		|| del1 != ','
		|| del2 != ';')
		in.setstate(ios::failbit);
	return in;
}

istream & CMap::LoadGate(istream &in)
{
	return in >> m_Gate;
}

istream & CMap::LoadMap(istream & in, bool saved)
{
	if (!LoadWallLine(in, 0))
		return in;
		
	if (!LoadMapCenter(in, saved))
		return in;
	
	if(!LoadWallLine(in, m_Rows - 1))
		return in;

	FindPaths();
	return in;
}

istream & CMap::DeleteWs(istream & in)
{
	char ch = 0;
	if (!(in >> ch))
		return in;
	in.putback(ch);
	return in;
}

istream & CMap::LoadWallLine(istream & in, int row)
{
	// skip whitespaces until first character and return the character itself back
	DeleteWs(in);
	
	// load the wall line
	for (int col = 0; col < m_Cols; ++col)
		if (!LoadCharToMap(in, {col, row}, false))
			return in;
	return in;
}

istream & CMap::LoadMapCenter(istream & in, bool saved)
{
	for (int row = 1; row < m_Rows - 1; ++row)
	{
		// read left wall boundary - skip enter
		DeleteWs(in);
		if (!LoadCharToMap(in, {0, row}, false))
			return in;
		
		// load characters inside map
		for (int col = 1; col < m_Cols - 1; ++col)
			if (!LoadCharToMap(in, {col, row}, saved))
				return in;
		
		// load right wall boundary
		if (!LoadCharToMap(in, {m_Cols - 1, row}, false))
			return in;
	}
	return in;
}

istream & CMap::LoadCharToMap(istream & in, pos_t position, bool saved)
{
	// check for empty character
	char ch = in.get();
	if (ch == ' ')
		return in;
	
	// check for eof or failbit
	if (!in.good())
		return in;
	
	// check it's type
	CTile tile{ch};
	if (tile.IsWall())
	{
		m_Map.insert({position, tile});
		return in;
	}
	if (tile.IsGate())
	{
		if (!InitGatePosition(position))
		{
			in.setstate(ios::failbit);
			return in;
		}
	}
	else if (tile.IsSpawn())
	{
		if (!InitSpawner(position, ch))
		{
			in.setstate(ios::failbit);
			return in;
		}
	}
	else if (saved)
	{
		if (!AddFromSaved(tile))
		{
			in.setstate(ios::failbit);
			return in;
		}
	}
	else
	{
		in.setstate(ios::failbit);
		return in;
	}
	m_Map.insert({position, tile});
	return in;
}

bool CMap::InitGatePosition(pos_t position)
{
	if (m_Gate.Position() != pos_t::npos)
		return false;
	m_Gate.Position() = position;
	return true;
}

bool CMap::AddFromSaved(const CTile & tile)
{
	if (!m_UnitStack)
		return false;
	
	if (tile.IsTower())
	{
		CTower * tower = m_UnitStack->CreateTowerAt(tile.GetRawChar());
		if (!tower)
			return false;
		m_Towers.emplace_back(tower);
	}
	else if (tile.IsTroop())
	{
		CTrooper * trooper = m_UnitStack->CreateTrooperAt(tile.GetRawChar());
		if (!trooper)
			return false;
		m_Troops.emplace_back(trooper);
	} else
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
ostream & CMap::Save(std::ostream & out) const
{
	if (!(SaveDimensions(out))
		|| !(SaveGate(out))
		|| !(SaveMap(out)))
		return out;
	return out;
}

std::ostream & CMap::SaveDimensions(std::ostream &out) const
{
	return out << "(D): " << m_Rows << ',' << m_Cols << ';' << endl;
}

std::ostream & CMap::SaveGate(std::ostream &out) const
{
	return out << "(G): " << m_Gate << ';' << endl;
}

std::ostream & CMap::SaveMap(std::ostream &out) const
{
	if (!(out << "(#): " << endl))
		return out;
	for (int i = 0; i < m_Rows; ++i)
	{
		string line;
		for (int j = 0; j < m_Cols; ++j)
		{
			if (!m_Map.count({j,i}))
				line += ' ';
			else
				line += m_Map.at({j, i}).GetRawChar();
		}
		if (!(out << line << endl))
			return out;
	}
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
