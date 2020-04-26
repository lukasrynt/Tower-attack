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
#include "ExInvalidFile.hpp"

using namespace std;
/**********************************************************************************************************************/
// INIT
CMap::CMap()
	: m_Cols(0),
	  m_Rows(0),
	  m_GateMaxHp(0),
	  m_GateHp(0),
	  m_Gate(pos_t::npos)
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
void CMap::LoadMap(std::istream & in, bool saved)
{
	char ch = 0;
	int row = 0;
	// load upper wall line
	for (int col = 0; col < m_Cols; ++col)
		AddToMap({col, row}, in.get(), false);
	row++;
	
	for (; row < m_Rows - 1; ++row)
	{
		// read left wall boundary - skip enter
		in.get();
		AddToMap({0, row}, in.get(), false);
		
		// load characters inside map
		for (int col = 1; col < m_Cols - 1; ++col)
		{
			ch = in.get();
			if (ch == ' ')
				continue;
			AddToMap({col, row}, ch, saved);
		}
		
		// load right wall boundary
		AddToMap({m_Cols - 1, row}, in.get(), false);
	}
	
	// load lower wall line
	in.get();
	for (int col = 0; col < m_Cols; ++col)
		AddToMap({col, row}, in.get(), false);
	
	// check if the file has ended
	in >> ch;
	if (!in.eof())
		throw invalid_file("file not properly ended.");
	
	// find paths for troops
	FindPaths();
}

void CMap::AddToMap(pos_t position, char ch, bool saved)
{
	CTile tile{ch};
	if (tile.IsWall())
	{
		m_Map.insert({position, tile});
		return;
	}
	if (tile.IsGate())
		InitGatePosition(position);
	else if (tile.IsSpawn())
		InitSpawner(position, ch);
	else if (saved)
		AddFromSaved(tile);
	else
		throw invalid_file("invalid map character.");
	m_Map.insert({position, tile});
}

void CMap::InitGatePosition(pos_t position)
{
	if (m_Gate != pos_t::npos)
		throw invalid_file("there can't be two gates");
	m_Gate = position;
}

void CMap::AddFromSaved(const CTile & tile)
{
	if (!m_UnitStack)
		throw invalid_file("unit stack not initialized.");
	
	if (tile.IsTower())
	{
		CTower * tower = m_UnitStack->CreateTowerAt(tile.GetRawChar());
		if (!tower)
			throw invalid_file((string("no tower named '") + tile.GetRawChar() + "' has been defined earlier").c_str());
		m_Towers.emplace_back(tower);
	}
	else if (tile.IsTroop())
	{
		CTrooper * trooper = m_UnitStack->CreateTrooperAt(tile.GetRawChar());
		if (!trooper)
			throw invalid_file((string("no trooper named '") + tile.GetRawChar() + "' has been defined earlier").c_str());
		m_Troops.emplace_back(trooper);
	}
	else
		throw invalid_file("invalid map character.");
}

void CMap::InitSpawner(pos_t position, char ch)
{
	if (m_Spawns.count(ch - '0'))
		throw invalid_file("spawner redefinition.");
	m_Spawns.insert({{ch - '0', position}});
}

void CMap::CheckSpawnCount(int count) const
{
	int max = 0;
	for (const auto & it : m_Spawns)
		if (max < it.first)
			max = it.first;
	if (max != count)
		throw invalid_file("The number of spawns and waves are not the same.");
}

void CMap::SetGateHealth(int hp)
{
	m_GateHp = m_GateMaxHp =  hp;
}

void CMap::SetMapDimensions(int rows, int cols)
{
	m_Rows = rows;
	m_Cols = cols;
}

/**********************************************************************************************************************/
// SAVING
ostream & operator<<(ostream & out, const CMap & map)
{
	for (int i = 0; i < map.m_Rows; ++i)
	{
		string line;
		for (int j = 0; j < map.m_Cols; ++j)
		{
			if (!map.m_Map.count({j,i}))
				line += ' ';
			else
				line += map.m_Map.at({j, i}).GetRawChar();
		}
		if (!(out << line << endl))
			return out;
	}
	return out;
}

bool CMap::Save(const char * filename) const
{
	ofstream outFile(filename);
	
	// Check output stream
	if (!outFile)
		throw runtime_error("File not found");
	
	// Write the dimension
	if (!(outFile << m_Rows << ','<< m_Cols << endl))
		throw runtime_error("Error during saving has occured");
	
	// Write the characters based on the map
	return !!(outFile << *this);
}

/**********************************************************************************************************************/
// RENDER
void CMap::Render() const
{
	RenderGate();
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

void CMap::RenderGate() const
{
	int part = 0;
	if (m_GateHp > 0)
		part = (m_GateHp / static_cast<double> (m_GateMaxHp)) * 10;
	cout << "Gate: ["
		<< Colors::bg_red << string(part, ' ') << Colors::color_reset
		<< string(10 - part, ' ') << ']' << endl << endl;
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
	if (GateDefeated())
		return false;
	TowerAttack();
	return true;
}

void CMap::FindPaths()
{
	for (const auto & position : m_Spawns)
	{
		CPath path{m_Map, m_Rows, m_Cols, position.second, m_Gate};
		m_Paths.insert({position.second, path.FindPath()});
	}
}

void CMap::MoveTroops(bool & waveOn)
{
	for (auto troop : m_Troops)
	{
		if (!troop->Move(m_Map))
			continue;
		DamageGate(troop->Attack());
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

void CMap::DamageGate(int damage)
{
	m_GateHp -= damage;
}

bool CMap::GateDefeated() const
{
	return m_GateHp < 0;
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
