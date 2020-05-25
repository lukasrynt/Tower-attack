/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <iostream>

/**
 * Type of the tile
 */
enum class ETileType : char
{
	WALL = 'W',
	BASIC_TROOP = 'T',
	ARMORED_TROOP = 'A',
	ARCHER_TOWER = 'R',
	MAGE_TOWER = 'M',
	SPAWN = 'S',
	GATE = 'G',
	BULLET = 'B',
	INVALID = 'I'
};

inline std::ostream & operator<<(std::ostream & out, const ETileType & type)
{return out << static_cast<std::underlying_type<ETileType>::type>(type);}