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
	SetColors();
}

CTile::CTile(char ch, ETileType type)
	: m_Char(ch),
	  m_Type(type)
{
	SetColors();
}

void CTile::SetColors()
{
	switch(m_Char)
	{
		case '#':
			m_Color = Colors::fg_magenta;
		case '@':
			m_Color = Colors::fg_yellow;
		case '$':
			m_Color = Colors::fg_cyan;
		case '*':
			m_Color = Colors::bg_red;
		case '%':
			m_Color = Colors::bg_blue;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			m_Color = Colors::bg_cyan;
		case 'O':
			m_Color = Colors::bg_magenta;
		case ' ':
			m_Color = Colors::bg_red;
		default:
			m_Color = "";
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