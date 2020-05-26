/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CArmoredTrooper.hpp"

using namespace std;

/**********************************************************************************************************************/
// ACTIONS
void CArmoredTrooper::ReceiveDamage(int damage, string bulletColor)
{
	// if armor is up we can ignore an attack and remove armor
	if (!m_ArmorOn)
		CTrooper::ReceiveDamage(damage, bulletColor);
	else
	{
		m_ArmorOn = false;
		m_ForeColor = Colors::FG_CYAN;
		m_BackColor = "";
	}
}

CBuffer CArmoredTrooper::CreateInfoBuffer(size_t width) const
{
	return move(CBuffer{width}
		.Append("   ").Append("("s + m_Char + ")", string(Colors::BG_CYAN) + Colors::FG_BLACK)
		.Append("\tHP: " + to_string(m_Hp), Colors::FG_CYAN)
		.Append("\tSpeed: " + to_string(m_Frames.GetSpeed()), Colors::FG_CYAN)
		.Append("\tAttack: " + to_string(m_Attack), Colors::FG_CYAN)
		.Append("\tArmor: " + to_string(m_Armor.GetSpeed()), Colors::FG_CYAN)
		.Append("\tCost: " + to_string(m_Price) + " ©", Colors::FG_CYAN));
}

bool CArmoredTrooper::Update()
{
	if (!CTrooper::Update())
		return false;
	if (!m_ArmorOn)
	{
		if (!m_BackColor.empty())
			m_BackColor = "";
		if (m_Armor.ActionAllowed())
		{
			m_BackColor = Colors::BG_CYAN;
			m_ForeColor = Colors::FG_BLACK;
			m_ArmorOn = true;
		}
	}
	return true;
}

istream & CArmoredTrooper::LoadOnMap(istream & in)
{
	int tmp;
	CTrooper::LoadOnMap(in) >> tmp >> m_ArmorOn;
	m_Armor.SetCurrent(tmp);
	if (m_ArmorOn)
	{
		m_BackColor = Colors::BG_CYAN;
		m_ForeColor = Colors::FG_BLACK;
	}
	return in;
}
