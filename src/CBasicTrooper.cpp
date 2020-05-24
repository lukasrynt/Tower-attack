/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24/05/2020.
*/

#include "CBasicTrooper.hpp"

using namespace std;

bool CBasicTrooper::Update()
{
	if (!CTrooper::Update())
		return false;
	m_BackColor = "";
	return true;
}

CBuffer CBasicTrooper::CreateInfoBuffer(size_t width) const
{
	return move(CBuffer{width}
		.Append("   ").Append("("s + m_Char + ")", string(Colors::BG_YELLOW) + Colors::FG_BLACK)
		.AddEscapeSequence(Colors::FG_YELLOW)
		.Append("\tHP: " + to_string(m_Hp))
		.Append("\tSpeed: " + to_string(m_Frames.GetSpeed()))
		.Append("\tAttack: " + to_string(m_Attack))
		.Append("\tCost: " + to_string(m_Price) + " ©")
		.AddEscapeSequence(Colors::RESET));
}

void CBasicTrooper::ReceiveDamage(int damage, string bulletColor)
{
	CTrooper::ReceiveDamage(damage, bulletColor);
	m_BackColor = bulletColor;
}
