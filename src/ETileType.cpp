/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "ETileType.hpp"

using namespace std;

ETileType TileType::CreateTile(char ch)
{
	switch(ch)
	{
		case '#':
			return ETileType::WALL;
		case '@':
		case '$':
			return ETileType::TROOP;
		case '*':
		case '%':
			return ETileType::TOWER;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			return ETileType::SPAWN;
		case 'O':
			return ETileType::GATE;
		case ' ':
			return ETileType::BULLET;
		default:
			return ETileType::INVALID;
	}
}

