#pragma once

#include <iostream>
#include <list>
#include <vector>
#include <deque>

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
	std::list<CPosition> GetNeighbours() const;
	bool LiesInRange(int rows, int cols) const;
	std::deque<std::vector<CPosition>> GetRadius(int level) const;
	friend bool operator<(const CPosition & self, const CPosition & other);
	friend bool operator==(const CPosition & self, const CPosition & other);
	friend bool operator!=(const CPosition & self, const CPosition & other);
	static const CPosition npos;
	
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