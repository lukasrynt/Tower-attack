#pragma once

#include <unordered_map>
#include "CPosition.hpp"
#include "CTile.hpp"

class CPath
{
public:
	CPath(const std::unordered_map<pos_t, CTile> & map, int rows, int cols, pos_t start, pos_t goal);
	std::deque<pos_t> FindPath();
private:
	struct CNode
	{
		pos_t m_Pos;
		int m_Dist = 0;
	};
	
	std::deque<pos_t> TraceBack();
	bool BFS();
	bool IterateNeighbours(std::unordered_map<pos_t, bool> & visited, std::queue<CNode> & cells);
	void QueueNeighbours(pos_t neighbour, std::unordered_map<pos_t, bool> & visited, std::queue<CNode> & cells);
	
	
	std::unordered_map<pos_t, CNode> m_NodeMap;
	const std::unordered_map<pos_t, CTile> & m_TileMap;
	int m_Rows;
	int m_Cols;
	pos_t m_Start;
	pos_t m_Goal;
};