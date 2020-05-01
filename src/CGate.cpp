/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 29.4.2020
 */

#include "CGate.hpp"
#include "Colors.hpp"

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

void CGate::Render() const
{
	int part = 0;
	if (m_Hp > 0)
		part = (m_Hp / static_cast<double> (m_MaxHp)) * 10;
	cout << "Gate: ["
		 << Colors::bg_red << string(part, ' ') << Colors::color_reset
		 << string(10 - part, ' ') << ']' << endl << endl;
}