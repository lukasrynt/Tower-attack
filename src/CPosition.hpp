/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <iostream>
#include <list>
#include <vector>
#include <deque>
#include <cmath>
#include <set>

/**
 * Class representing x-y position on the map
 */
typedef struct CPosition
{
public:
	CPosition(int x = -1, int y = -1);
	CPosition left() const;
	CPosition right() const;
	CPosition up() const;
	CPosition down() const;
	/**
	 * Gets positions that are neighbouring on edges with current position
	 * @return list of neighbours
	 */
	std::list<CPosition> GetCrossNeighbours() const;
	/**
	 * Gets positions that is neighbouring on at least one vertex
	 * @return list of neighbours
	 */
	std::list<CPosition> GetDiagNeighbours() const;
	/**
	 * Checks whether the position lies in the given range
	 * @param rows Maximum number of rows
	 * @param cols Maximum numer of columns
	 * @return true if the point lies in range
	 */
	bool LiesInRange(int rows, int cols) const;
	/**
	 * Gets the radius of the current positions on given level
	 * @param level Level of radius
	 * @return positions in the radius
	 */
	std::deque<CPosition> GetRadius(int level) const;
	friend bool operator<(const CPosition & self, const CPosition & other);
	friend bool operator==(const CPosition & self, const CPosition & other);
	friend bool operator!=(const CPosition & self, const CPosition & other);
	
	double Distance(const CPosition & other) const
	{return sqrt((m_X - other.m_X)*(m_X - other.m_X) + (m_Y - other.m_Y)*(m_Y - other.m_Y));}
	
	friend std::ostream & operator<<(std::ostream & out, const CPosition & self)
	{return out << "(" << self.m_X << ' ' << self.m_Y << ")";}
	
	friend std::istream & operator>>(std::istream & in, CPosition & self);
	
	int m_X; 	//!< X coordinate
	int m_Y;	//!< Y coordinate
}pos_t;

/**
 * Custom hash function to use in map
 */
namespace std
{
	template<> struct hash<CPosition>
	{
		std::size_t operator()(const CPosition & s) const noexcept
		{
			return s.m_X ^ (s.m_Y << 1);
		}
	};
}

namespace pos
{
	const pos_t npos = pos_t{-1, -1};
}