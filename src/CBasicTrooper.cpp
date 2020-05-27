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

CBuffer CBasicTrooper::DrawHelpInfo(size_t width) const
{
	return move(CBuffer{width}
		.Append("   ").Append("("s + m_Char + ")", string(Colors::BG_YELLOW) + Colors::FG_BLACK)
		.Append("\tHP: " + to_string(m_Hp), Colors::FG_YELLOW)
		.Append("\tSpeed: " + to_string(m_Frames.GetSpeed()), Colors::FG_YELLOW)
		.Append("\tAttack: " + to_string(m_Attack), Colors::FG_YELLOW)
		.Append("\tCost: " + to_string(m_Price) + " ©", Colors::FG_YELLOW));
}