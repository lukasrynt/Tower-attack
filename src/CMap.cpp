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
#include "EInvalidFile.hpp"

using namespace std;
/**********************************************************************************************************************/
// INIT
CMap::CMap()
	: m_Cols(0),
	  m_Rows(0),
	  m_GateHp(0),
	  m_MaxGateHp(0)
{}

CMap::~CMap()
{
	//TODO
}

/**********************************************************************************************************************/
// LOADING
void CMap::LoadMap(std::istream & in, bool saved)
{
	char ch;
	int row = 0;
	// load upper wall line
	for (int col = 0; col < m_Cols; ++col)
		LoadWall({row++, col}, in.get());
	
	for (; row < m_Rows - 2; ++row)
	{
		// read left wall boundary - skip enter
		in.get();
		LoadWall({row, 0}, in.get());
		
		// load characters inside map
		for (int col = 0; col < m_Cols - 2; ++col)
			AddToMap({row, col}, in.get(), saved);
		
		// load right wall boundary
		LoadWall({row, m_Cols - 1}, in.get());
	}
	
	// load lower wall line
	for (int col = 0; col < m_Cols; ++col)
		LoadWall({row, col}, in.get());
	
	// check if the file has ended
	if (cin >> ch)
		throw invalid_file("");
	
	// find paths for troops
	FindPaths();
}

void CMap::LoadWall(pos_t position, char ch)
{
	CTile tile{ch};
	if (ch != '#')
		throw invalid_file("unexpected character read in map.");
	m_Map.insert({position, tile});
}

void CMap::AddToMap(pos_t position, char ch, bool saved)
{
	// For empty tile just return
	if (ch == ' ')
		return;
	
	CTile tile{ch};
	if (tile.IsGate())
		m_Gate = position;
	else if (tile.IsSpawn())
		InitSpawner(position, ch);
	else if (saved)
		AddFromSaved(tile);
	else
		throw invalid_file("invalid map character.");
	m_Map.insert({position, tile});
}

void CMap::AddFromSaved(const CTile & tile)
{
	if (tile.IsTower())
		m_Towers.emplace_back(m_UnitStack->CreateTowerAt(tile.GetRawChar()));
	else if (tile.IsTroop())
		m_Troops.emplace_back(m_UnitStack->CreateTrooperAt(tile.GetRawChar()));
	else
		throw invalid_file("invalid map character.");
}

void CMap::InitSpawner(pos_t position, char ch)
{
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
	m_GateHp = hp;
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
		part = (m_GateHp / static_cast<double> (m_MaxGateHp)) * 10;
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
// MORE
void CMap::VisualizePath(std::queue<pos_t> path)
{
	while (!path.empty())
	{
		CTile tile = CTile(' ');
		m_Map.insert({path.front(), tile});
		path.pop();
	}
}
