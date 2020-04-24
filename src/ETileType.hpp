#pragma once


#include <iostream>

enum class ETileType
{
	WALL,	//!< Tile with wall
	TROOP,	//!< Tile with troop
	TOWER,	//!< Tile with tower
	SPAWN,	//!< Tile with spawn point
	GATE,	//!< Target of troops
	BULLET,
	INVALID	//!< Invalid character
};

namespace TileType
{
	ETileType CreateTile(char ch);
}