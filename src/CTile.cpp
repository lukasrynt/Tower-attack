/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTile.hpp"
#include "Colors.hpp"

#include <utility>

using namespace std;

/**********************************************************************************************************************/
// INIT
CTile::CTile(char ch)
	: m_Char(ch),
	  m_Type(TileType::CreateTile(ch))
{
	InitColors();
}

CTile::CTile(char ch, ETileType type)
	: m_Char(ch),
	  m_Type(type)
{
	InitColors();
}

CTile::CTile(char ch, ETileType type, string color)
	: m_Char(ch),
	  m_Type(type),
	  m_Color(move(color))
{}

void CTile::InitColors()
{
	switch(m_Type)
	{
		case ETileType::WALL:
			m_Color = Colors::fg_magenta;
			break;
		case ETileType::TROOP:
			m_Color = Colors::fg_yellow;
			break;
		case ETileType::TOWER:
			m_Color = Colors::bg_red;
			break;
		case ETileType::SPAWN:
			m_Color = Colors::bg_cyan;
			break;
		case ETileType::GATE:
			m_Color = Colors::bg_magenta;
			break;
		case ETileType::BULLET:
			m_Color = Colors::bg_red;
			break;
		default:
			m_Color = "";
			break;
	}
}

/**********************************************************************************************************************/
// TYPES FUNCTIONS
bool CTile::IsTroop() const
{
	return m_Type == ETileType::TROOP;
}

bool CTile::IsTower() const
{
	return m_Type == ETileType::TOWER;
}

bool CTile::IsWall() const
{
	return m_Type == ETileType::WALL;
}

bool CTile::IsSpawn() const
{
	return m_Type == ETileType::SPAWN;
}

bool CTile::IsGate() const
{
	return m_Type == ETileType::GATE;
}

bool CTile::IsValid() const
{
	return m_Type != ETileType::INVALID;
}

/**********************************************************************************************************************/
// PRINTING
char CTile::GetRawChar() const
{
	return m_Char;
}

string CTile::PrintChar() const
{
	string res = m_Color + m_Char;
	if (!m_Color.empty())
		res += Colors::color_reset;
	return res;
}