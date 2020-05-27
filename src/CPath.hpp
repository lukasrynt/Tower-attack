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

/**
 * Finds path from start to goal
 */
class CPath
{
public:
	CPath(const std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, int rows, int cols, pos_t start, pos_t goal)
			: m_TileMap(map),
			  m_Rows(rows),
			  m_Cols(cols),
			  m_Start(start),
			  m_Goal(goal)
	{}
	/**
	 * Finds path from start to goal in which the points are neighbouring on sides
	 * @return Path
	 */
	std::deque<pos_t> FindStraightPath();
	/**
	 * Finds path from start to goal in which the points are neighbouring on vertices
	 * @return Path
	 */
	std::deque<pos_t> FindDiagonalPath();
private:
	/**
	 * Structure with position and distance from start
	 */
	struct CNode
	{
		pos_t m_Pos;
		int m_Dist = 0;
	};
	/**
	 * Traces the path back from goal to start calculating the shortest distance
	 * @param getNeighbours Function that returns neighbours of the point
	 * @return Path
	 */
	std::deque<pos_t> TraceBack(const std::function<std::list<pos_t>(pos_t)> & getNeighbours);
	/**
	 * Implements Breadth first search to spread through the map and find path from start to goal
	 * @param getNeighbours Function that returns neighbours of the point
	 * @return True if some path was found
	 */
	bool BFS(const std::function<std::list<pos_t>(pos_t)> & getNeighbours);
	/**
	 * Iterates the neighbours of current point
	 * @param visited Set of positions we have been to
	 * @param cells All cells left to explore
	 * @param getNeighbours Function that returns neighbours of the point
	 * @return True if goal was reached
	 */
	bool IterateNeighbours(std::set<pos_t> & visited, std::queue<CNode> & cells, const std::function<std::list<pos_t>(pos_t)> & getNeighbours);
	/**
	 * Queues the neighbouring cells if they haven't been visited and are in the range of map
	 * @param neighbour Cell we want to queue
	 * @param visited Set of positions we have been to
	 * @param cells All cells left to explore
	 */
	void QueueNeighbour(pos_t neighbour, std::set<pos_t> & visited, std::queue<CNode> & cells);
	
	
	std::unordered_map<pos_t, int> m_DistMap;								//!< Positions and their distances from start
	const std::unordered_map<pos_t, std::shared_ptr<CTile>> & m_TileMap;	//!< Reference to original map with tiles
	int m_Rows;																//!< Number of rows of the map
	int m_Cols;																//!< Number of columns of the map
	pos_t m_Start;															//!< Starting point of the path
	pos_t m_Goal;															//!< End point of the path
};