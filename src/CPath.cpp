/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include <algorithm>
#include "CPath.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CPath::CPath(const unordered_map<pos_t, CTile> & map, int rows, int cols, pos_t start, pos_t goal)
	: m_TileMap(map),
	  m_Rows(rows),
	  m_Cols(cols),
	  m_Start(start),
	  m_Goal(goal)
{}

/**********************************************************************************************************************/
// PATHFINDING
deque<pos_t> CPath::FindPath()
{
	// Spread the tiles using BFS
	if (!BFS())
		return deque<pos_t>();
	
	// trace the path back, based on the data received from BFS
	return TraceBack();
}

deque<pos_t> CPath::TraceBack()
{
	deque<pos_t> path;
	path.push_back(m_Goal);
	
	for (CNode curr = m_NodeMap.at(m_Goal); curr.m_Pos != m_Start;)
	{
		int minDist = INT32_MAX;
		for (const auto & neighbour : curr.m_Pos.GetNeighbours())
		{
			if (!m_NodeMap.count(neighbour)
				|| m_NodeMap.at(neighbour).m_Dist > minDist)
				continue;
			curr = m_NodeMap.at(neighbour);
			m_NodeMap.erase(neighbour);
			minDist = curr.m_Dist;
		}
		path.push_back(curr.m_Pos);
		m_NodeMap.erase(curr.m_Pos);
	}
	reverse(path.begin(), path.end());
	// we don't want the start point to be present in the list
	path.pop_front();
	return path;
}

bool CPath::BFS()
{
	// if point is not in the map - path was not found
	if (!m_Start.LiesInRange(m_Rows, m_Cols) || !m_Goal.LiesInRange(m_Rows, m_Cols))
		return false;
	
	// visited map
	unordered_map<pos_t, bool> visited;
	
	// insert the starting point and set it to visited
	queue<CNode> cells;
	cells.push({m_Start});
	m_NodeMap.insert({m_Start, {m_Start}});
	visited.insert({m_Start, true});
	
	while (!cells.empty())
	{
		// if we reached the destination - break the cycle
		if (cells.front().m_Pos == m_Goal)
			return true;
		
		// queue neighbouring cells - return true if one of the neighbours was target
		if (IterateNeighbours(visited, cells))
			return true;
		
		// dequeue the current cell
		cells.pop();
	}
	// path was not found
	return false;
}

bool CPath::IterateNeighbours(unordered_map<pos_t, bool> & visited, queue<CNode> & cells)
{
	pos_t curr = cells.front().m_Pos;
	for (const auto & neighbour : curr.GetNeighbours())
	{
		// found finish
		if (neighbour == m_Goal)
		{
			m_NodeMap.insert({neighbour, {neighbour, cells.front().m_Dist + 1}});
			return true;
		}
		QueueNeighbours(neighbour, visited, cells);
	}
	return false;
}

void CPath::QueueNeighbours(pos_t neighbour, unordered_map<pos_t, bool> & visited, queue<CNode> & cells)
{
	if (neighbour.LiesInRange(m_Rows, m_Cols)
		&& !visited.count(neighbour)
		&& !m_TileMap.count(neighbour))
	{
		cells.push({neighbour, cells.front().m_Dist + 1});
		m_NodeMap.insert({neighbour, {neighbour, cells.front().m_Dist + 1}});
	}
	visited.insert({neighbour, true});
}
