/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CArmoredTrooper.hpp"

using namespace std;

/**********************************************************************************************************************/
// ACTIONS
void CArmoredTrooper::ReceiveDamage(int damage)
{
	// if armor is up we can ignore an attack and remove armor
	if (!m_Armor)
		CTrooper::ReceiveDamage(damage);
	else
		m_Armor -= damage;
	
	if (m_Armor < 0)
		m_Armor = 0;
}

CBuffer CArmoredTrooper::CreateInfoBuffer(int windowWidth) const
{
	return move(CBuffer{windowWidth}
		.Append("   ").Append("("s + m_Char + ")", string(Colors::BG_CYAN) + Colors::FG_BLACK)
		.Append("\tHP: " + to_string(m_Hp), Colors::FG_CYAN)
		.Append("\tSpeed: " + to_string(m_Frames.GetSpeed()), Colors::FG_CYAN)
		.Append("\tAttack: " + to_string(m_Attack), Colors::FG_CYAN)
		.Append("\tArmor: " + to_string(m_Armor), Colors::FG_CYAN)
		.Append("\tCost: " + to_string(m_Price) + " ©", Colors::FG_CYAN));
}