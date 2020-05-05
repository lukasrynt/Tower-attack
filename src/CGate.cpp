/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 29.4.2020
 */

#include "CGate.hpp"
#include "NColors.hpp"

using namespace std;

/**********************************************************************************************************************/
// GATE
istream & operator>>(istream & in, CGate & self)
{
	char brOp, brCl;
	if (!(in >> brOp >> self.m_Hp >> self.m_MaxHp >> brCl)
		|| brOp != '('
		|| brCl != ')')
		in.setstate(std::ios::failbit);
	return in;
}

ostream & CGate::Render(ostream & out) const
{
	int part = 0;
	if (m_Hp > 0)
		part = round<int>(m_Hp / static_cast<double> (m_MaxHp) * 10);
	return out << "Gate: ["
			   << Colors::BG_RED << string(part, ' ') << Colors::RESET
		 << string(10 - part, ' ') << ']' << endl << endl;
}