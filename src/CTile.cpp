/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTile.hpp"
#include "escape_sequences.h"

#include <utility>

using namespace std;

/**********************************************************************************************************************/
CTile::CTile(char ch)
	: m_Char(ch)
{
	m_Type = InitType();
	InitColor();
}

CTile::CTile(char ch, ETileType type)
	: m_Char(ch),
	  m_Type(type)
{
	InitColor();
}

CTile::CTile(char ch, ETileType type, string foreColor, string backColor)
	: m_Char(ch),
	  m_Type(type),
	  m_ForeColor(move(foreColor)),
	  m_BackColor(move(backColor))
{}

void CTile::InitColor()
{
	switch(m_Type)
	{
		case ETileType::WALL:
			m_ForeColor = Colors::FG_MAGENTA;
			break;
		case ETileType::SPAWN:
			m_BackColor = Colors::BG_CYAN;
			break;
		case ETileType::GATE:
			m_BackColor = Colors::BG_MAGENTA;
			break;
		default:
			break;
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