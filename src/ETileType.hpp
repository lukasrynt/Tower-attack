/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <iostream>

enum class ETileType : char
{
	WALL = 'W',			//!< Tile with wall
	BASIC_TROOP = 'T',	//!< Tile with troop
	ARMORED_TROOP = 'A',
	ARCHER_TOWER = 'R',	//!< Tile with tower
	MAGE_TOWER = 'M',
	SPAWN = 'S',			//!< Tile with spawn point
	GATE = 'G',			//!< Target of troops
	BULLET = 'B',
	INVALID = 'I'			//!< Invalid character
};

inline std::ostream & operator<<(std::ostream & out, const ETileType & type)
{return out << static_cast<std::underlying_type<ETileType>::type>(type);}