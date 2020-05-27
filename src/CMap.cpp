/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include <cstring>
#include <iostream>
#include <algorithm>
#include <random>
#include "CMap.hpp"
#include "CMageTower.hpp"
#include "ExInvalidFormat.hpp"

using namespace std;
/**********************************************************************************************************************/
// INIT
void CMap::AssignUnitStack(shared_ptr<CUnitStack> unitStack)
{
	m_UnitStack = move(unitStack);
}

/**********************************************************************************************************************/
// LOADING
istream & operator>>(istream & in, CMap & self)
{
	self.LoadMapInfo(in)
		.LoadMap(in)
		.LoadEntities(in);
	return in;
}

CMap & CMap::PlaceTowers()
{
	for (const auto & tower : m_Towers)
		if (m_Map.count(tower->GetPosition()) && m_Map.at(tower->GetPosition())->IsWall())
			m_Map.at(tower->GetPosition()) = tower;
	return *this;
}

CMap & CMap::GenerateTowers()
{
	string towerChars = m_UnitStack->GetTowerChars();
	
	/** @source https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution */
	random_device rd;
	mt19937 gen(rd());
	
	uniform_int_distribution<int> disRow(0, m_Rows - 1);
	uniform_int_distribution<int> disCol(0, m_Cols - 1);
	uniform_int_distribution<size_t> disChar(0, towerChars.size() - 1);
	
	while (m_Towers.size() < m_TowerCount)
	{
		// generate random numbers while their distance is smaller than 3 - that way they won't be grouped next to each other
		while (true)
		{
			size_t charIdx = disChar(gen);
			pos_t point = pos_t{disCol(gen), disRow(gen)};
			
			// check if the position is ok - towers are not somewhere on blank spaces - would block troops paths
			if (!m_Map.count(point)
				|| !m_Map.at(point)->IsWall())
				continue;
			
			unique_ptr<CTower> tmp = m_UnitStack->CreateTowerAt(towerChars[charIdx]);
			tmp->SetPosition(point);
			m_Towers.emplace_back(move(tmp));
			break;
		}
	}
	return *this;
}

CMap & CMap::PlaceTroops()
{
	for (const auto & troop : m_Troops)
		m_Map.emplace(troop->GetPosition(), troop);
	return *this;
}

CMap & CMap::LoadMapInfo(istream & in)
{
	in >> m_Rows >> m_Cols >> m_Gate >> m_TowerCount;
	return *this;
}

CMap & CMap::LoadMap(istream & in)
{
	LoadWallLine(in, 0);
	
	for (int row = 1; row < m_Rows - 1; ++row)
		LoadMapCenter(in, row);
	
	LoadWallLine(in, m_Rows - 1);
	
	
	if (!FindPathsFromSpawner())
		throw invalid_format("No path from spawner to gate has been found.");
	return *this;
}

CMap & CMap::LoadEntities(istream & in)
{
	if (!m_UnitStack)
		throw invalid_format("Unit stack not defined.");
	
	while (true)
	{
		// check first character
		char ch;
		in >> ch;
		if (m_UnitStack->IsTrooperChar(ch))
			LoadTroops(in, ch);
		else if (m_UnitStack->IsTowerChar(ch))
			LoadTowers(in, ch);
		else
		{
			in.putback(ch);
			break;
		}
	}
	return *this;
}

void CMap::LoadTroops(istream & in, char ch)
{
	unique_ptr<CTrooper> trooper(m_UnitStack->CreateTrooperAt(ch));
	trooper->LoadOnMap(in);
	if (m_Map.count(trooper->GetPosition()))
		throw invalid_format("Trooper cannot be placed on occupied spot.");
	auto path = CPath{m_Map, m_Rows, m_Cols, trooper->GetPosition(), m_Gate.Position()}.FindStraightPath();
	trooper->SetPath(path);
	m_Troops.emplace_back(move(trooper));
}

void CMap::LoadTowers(istream & in, char ch)
{
	shared_ptr<CTower> tower(m_UnitStack->CreateTowerAt(ch));
	tower->LoadOnMap(in);
	if (!m_Map.count(tower->GetPosition()) || !m_Map.at(tower->GetPosition())->IsWall())
		throw invalid_format("Tower can only be placed on position occupied by wall.");
	m_Towers.push_back(tower);
}

void CMap::LoadWallLine(istream & in, int row)
{
	// skip whitespaces until first character and return the character itself back
	char ch = 0;
	in >> ch;
	in.putback(ch);
	
	// load the wall line
	for (int col = 0; col < m_Cols; ++col)
	{
		in.get(ch);
		if (m_UnitStack->IsTowerChar(ch))
			LoadEntityChar(pos_t{col, row}, ch);
		else
			if (!LoadWallChar(ch, pos_t{col, row}))
				throw invalid_format("Invalid character detected on the edge of map.");
	}
}

void CMap::LoadMapCenter(istream & in, int row)
{
	// read left wall boundary - skip enter
	in.get();
	if (!LoadWallChar(in.get(), pos_t{0, row}))
		throw invalid_format("Invalid character detected on the edge of map.");
	
	// load characters inside map
	for (int col = 1; col < m_Cols - 1; ++col)
		if (!LoadCenterChar(in.get(), pos_t{col, row}))
			throw invalid_format("Invalid character in the center of the map.");
	
	// load right wall boundary
	if (!LoadWallChar(in.get(), pos_t{m_Cols - 1, row}))
		throw invalid_format("Invalid character detected on the edge of map.");
}

bool CMap::LoadWallChar(char ch, pos_t position)
{
	CTile tile{ch};
	if (tile.IsSpawn())
	{
		if (!InitSpawner(position, ch))
			throw invalid_format("Redefinition of spawner is not allowed.");
	}
	else if (tile.IsGate())
	{
		if (!InitGatePosition(position))
			throw invalid_format("Redefinition of gate is not allowed.");
	}
	else if (!tile.IsWall())
		return false;
	m_Map.emplace(position, new CTile{tile});
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
		return LoadEntityChar(position, ch);
}

bool CMap::InitGatePosition(pos_t position)
{
	if (m_Gate.Position() != pos::npos)
		return false;
	m_Gate.Position() = position;
	return true;
}

bool CMap::LoadEntityChar(pos_t position, char ch)
{
	if (!m_UnitStack)
		return false;
	
	// replace tower with walls
	if (m_UnitStack->IsTowerChar(ch))
		m_Map.emplace(position, new CTile{'#'});
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
	m_Spawns.emplace(ch - '0', position);
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
	
	self.SaveMapInfo(out);
	self.SaveMap(out);
	self.SaveEntities(out);
	return out << endl;
}

void CMap::SaveMapInfo(ostream & out) const
{
	out << "(M)" << endl << m_Rows << ' ' << m_Cols << ' ' << m_Gate << ' ' << m_TowerCount << endl;
}

void CMap::SaveMap(ostream & out) const
{
	for (int i = 0; i < m_Rows; ++i)
	{
		string line;
		for (int j = 0; j < m_Cols; ++j)
		{
			if (!m_Map.count(pos_t{j,i}) || m_Map.at(pos_t{j,i})->IsTroop())	// skip troops while saving the map
				line += ' ';
			else if (m_Map.at(pos_t{j,i})->IsTower())
				line += '#';
			else
				line += m_Map.at(pos_t{j, i})->GetChar();
		}
		out << line << endl;
	}
}

void CMap::SaveEntities(ostream & out) const
{
	// save troops
	for (const auto & troop : m_Troops)
		troop->SaveOnMap(out) << endl;
		
	for (const auto & tower : m_Towers)
		tower->SaveOnMap(out) << endl;
}

/**********************************************************************************************************************/
// DRAW
CBuffer CMap::Draw(size_t width) const
{
	return move(CBuffer{width}
		.Append(m_Gate.Draw(width))
		.Append(DrawMap(width)));
}

CBuffer CMap::DrawMap(size_t width) const
{
	CBuffer buffer{width};
	for (int i = 0; i < m_Rows; ++i)
	{
		buffer.Append();
		for (int j = 0; j < m_Cols; ++j)
		{
			if (!m_Map.count(pos_t{j,i}))
				buffer << " ";
			else
				buffer << *m_Map.at(pos_t{j, i});
		}
	}
	buffer.Append(DrawTroopsOnMap(width));
	buffer.CenterHorizontal();
	return buffer;
}

CBuffer CMap::DrawTroopsOnMap(size_t width) const
{
	// map troops to their spawns
	CBuffer buffer{width};
	map<int, CBuffer> lines;
	for (const auto & troop : m_Troops)
	{
		if (!lines.count(troop->GetSpawn()))
			lines.emplace(troop->GetSpawn(), CBuffer{width}.Append("Wave " + to_string(troop->GetSpawn()) + ": ", Colors::FG_GREEN));
		lines.at(troop->GetSpawn()).AddText(to_string(troop->GetHp()), troop->GetColor()).AddText(" ");
	}
	for (auto & line : lines)
		buffer.Append(move(line.second));
	return buffer;
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

void CMap::Spawn(vector<unique_ptr<CTrooper>> & spawns)
{
	for (auto & trooper : spawns)
	{
		shared_ptr<CTrooper> troopRef(move(trooper));
		troopRef->SetPath(m_Paths.at(m_Spawns.at(troopRef->GetSpawn())));
		troopRef->Spawn();
		if (!m_Map.count(troopRef->GetPosition()))
			m_Map.emplace(troopRef->GetPosition(), troopRef);
		auto it = lower_bound(m_Troops.begin(), m_Troops.end(), troopRef,
							  [](const shared_ptr<CTrooper> & a, const shared_ptr<CTrooper> & b)
							  {return a->DistanceToGoal() < b->DistanceToGoal();});
		m_Troops.emplace(it, troopRef);
	}
}

bool CMap::FindPathsFromSpawner()
{
	for (const auto & position : m_Spawns)
	{
		auto path = CPath{m_Map, m_Rows, m_Cols, position.second, m_Gate.Position()}.FindStraightPath();
		if (path.empty())
			return false;
		m_Paths.emplace(position.second, path);
	}
	return true;
}

bool CMap::MoveOnMap(shared_ptr<CTrooper> & troop)
{
	bool emplace, erase, goal;
	pos_t origPos = troop->GetPosition();
	goal = troop->Move(m_Map, emplace, erase);
	if (erase)
		m_Map.erase(origPos);
	if (emplace)
		m_Map.emplace(troop->GetPosition(), troop);
	return goal;
}

bool CMap::MoveTroops()
{
	bool ret = true;
	for (auto troop = m_Troops.begin(); troop != m_Troops.end();)
	{
		if (!(**troop).Update() || !MoveOnMap(*troop))
		{
			troop++;
			continue;
		}
		m_Gate.ReceiveDamage((*troop)->Attack());
		troop = m_Troops.erase(troop);
		if (m_Troops.empty())
			ret = false;
	}
	return ret;
}

bool CMap::TowerAttack()
{
	// map troopers to their positions
	unordered_map<pos_t, shared_ptr<CTrooper>> troops;
	for (auto & troop : m_Troops)
		troops.emplace(troop->GetPosition(), troop);
	
	// attack troops by towers
	for (auto & tower : m_Towers)
		tower->Attack(m_Map, troops, m_Rows, m_Cols);
	
	return CheckTroopsDeaths();
}

bool CMap::CheckTroopsDeaths()
{
	// check if a trooper has died - if so delete him from map
	bool ret = true;
	for (auto troop = m_Troops.begin(); troop != m_Troops.end();)
	{
		if (!(*troop)->Died())
		{
			troop++;
			continue;
		}
		m_Map.erase((*troop)->GetPosition());
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
		res.emplace(spawn.first, !m_Map.count(spawnPos));
	}
	return res;
}

/**********************************************************************************************************************/
// TESTING
void CMap::Visualize(pos_t start, pos_t goal)
{
	auto path = CPath{m_Map, m_Rows, m_Cols, start, goal}.FindStraightPath();
	while (!path.empty())
	{
		CTile tile = CTile(' ');
		m_Map.emplace(path.front(), new CTile{tile});
		path.pop_front();
	}
}

void CMap::Visualize(const deque<pos_t> & positions)
{
	for (const auto & pos : positions)
		if (!m_Map.count(pos))
			m_Map.emplace(pos, new CTile(' ', ETileType::BULLET, Colors::BG_RED));
}
