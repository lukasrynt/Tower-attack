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
void CMap::LoadNewMap(istream & in)
{
	// Check input stream
	if (!in)
		throw runtime_error("Error loading file.");
	
	// Load first two numbers defining the size of the map
	char delimit;
	if (!(in >> m_Rows >> delimit >> m_Cols) || delimit != ',')
		throw runtime_error("Wrong dimensions in map descriptions.");
	
	// Skip the enter after dimension sequence
	in.get();
	
	m_GateHp = m_MaxGateHp = 200;
	
	// Load the characters from input, based on the description of the map
	LoadNewPositions(in);
	FindPaths();
}

void CMap::AddFromNew(pos_t position, char ch)
{
	// For empty tile just return
	if (ch == ' ')
		return;
	
	// Add tile to map
	CTile tile{ch};
	if (tile.IsGate())
		m_Gate = position;
	else if (tile.IsSpawn())
		InitSpawner(position, ch);
	else if (tile.IsTower())
		InitTower(position, ch);
	else
		throw runtime_error("Invalid character in map.");
	m_Map.insert({position,tile});
}


void CMap::LoadSavedMap(istream & in)
{
	// Check input stream
	if (!in)
		throw runtime_error("Error loading file.");
	
	// Load first two numbers defining the size of the map
	char delimit;
	if (!(in >> m_Rows >> delimit >> m_Cols) || delimit != ',')
		throw runtime_error("Wrong dimensions in map descriptions.");
	
	// Skip the enter after dimension sequence
	in.get();
	
	m_GateHp = m_MaxGateHp = 200;
	
	// Load the characters from input, based on the description of the map
	LoadNewPositions(in);
	FindPaths();
}

void CMap::LoadSavedPositions(std::istream &in)
{
	char ch = 0;
	for (int i = 0; i < m_Rows; i++)
	{
		for (int j = 0; j < m_Cols; j++)
		{
			// try to load character
			if (!(ch = in.get()))
				throw runtime_error("Failure during read");
			
			// check whether the char is not invalid and add it to map
			AddFromNew({j,i}, ch);
			
		}
		if (!(ch = in.get()) || ch != '\n')
			throw runtime_error("Missing enter");
	}
}

void CMap::AddFromSaved(pos_t position, char ch)
{
	// For empty tile just return
	if (ch == ' ')
		return;
	
	// Add tile to map
	CTile tile{ch};
	if (!tile.IsValid())
		throw runtime_error("Invalid character.");
	else if (tile.IsGate())
		m_Gate = position;
	else if (tile.IsSpawn())
		InitSpawner(position, ch);
	else if (tile.IsTower())
		InitTower(position, ch);
	m_Map.insert({position,tile});
}


void CMap::InitTower(pos_t position, char ch)
{
	CTower * tower = nullptr;
	if (ch == '*')
		tower = new CTower(position, 20, 20);
	else if (ch == '%')
		tower = new CMageTower(position, 20, 20, 20, 40);
	m_Towers.emplace_back(tower);
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
		throw runtime_error("The number of spawns and waves are not the same.");
}

void CMap::SetGateHealth(int hp)
{
	m_GateHp = hp;
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
