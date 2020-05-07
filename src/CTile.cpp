/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTile.hpp"
#include "escape_sequences.h"

#include <utility>

using namespace std;

/**********************************************************************************************************************/
// INIT
CTile::CTile(char ch)
	: m_Char(ch)
{
	m_Type = InitType();
	m_Color = InitColor();
}

CTile::CTile(char ch, ETileType type)
	: m_Char(ch),
	  m_Type(type)
{
	m_Color = InitColor();
}

CTile::CTile(char ch, ETileType type, string color)
	: m_Char(ch),
	  m_Type(type),
	  m_Color(move(color))
{}

string CTile::InitColor() const
{
	switch(m_Type)
	{
		case ETileType::WALL:
			return Colors::FG_MAGENTA;
		case ETileType::SPAWN:
			return Colors::BG_CYAN;
		case ETileType::GATE:
			return Colors::BG_MAGENTA;
		default:
			return "";
	}
}

ETileType CTile::InitType() const
{
	switch(m_Char)
	{
		case '#':
			return ETileType::WALL;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			return ETileType::SPAWN;
		case 'O':
			return ETileType::GATE;
		default:
			return ETileType::INVALID;
	}
}

/**********************************************************************************************************************/
// PRINTING
string CTile::PrintChar() const
{
	string res = m_Color + m_Char;
	if (!m_Color.empty())
		res += Colors::RESET;
	return res;
}