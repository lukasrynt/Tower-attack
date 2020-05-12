/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include "CPosition.hpp"
#include "CTile.hpp"

class CPath
{
public:
	CPath(const std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, int rows, int cols, pos_t start, pos_t goal);
	std::deque<pos_t> FindStraightPath();
	std::deque<pos_t> FindDiagonalPath();
private:
	struct CNode
	{
		pos_t m_Pos;
		int m_Dist = 0;
	};
	
	std::deque<pos_t> TraceBack(const std::function<std::list<pos_t>(pos_t)> & getNeighbours);
	bool BFS(const std::function<std::list<pos_t>(pos_t)> & getNeighbours);
	bool IterateNeighbours(std::set<pos_t> & visited, std::queue<CNode> & cells, const std::function<std::list<pos_t>(pos_t)> & getNeighbours);
	void QueueNeighbours(pos_t neighbour, std::set<pos_t> & visited, std::queue<CNode> & cells);
	
	
	std::unordered_map<pos_t, CNode> m_NodeMap;
	const std::unordered_map<pos_t, std::shared_ptr<CTile>> & m_TileMap;
	int m_Rows;
	int m_Cols;
	pos_t m_Start;
	pos_t m_Goal;
};