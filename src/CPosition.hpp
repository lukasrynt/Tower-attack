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
	CPosition(int x = 0, int y = 0);
	CPosition left() const;
	CPosition right() const;
	CPosition up() const;
	CPosition down() const;
	std::list<CPosition> GetCrossNeighbours() const;
	std::list<CPosition> GetDiagNeighbours() const;
	bool LiesInRange(int rows, int cols) const;
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