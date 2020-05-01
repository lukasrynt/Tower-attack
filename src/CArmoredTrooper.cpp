/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CArmoredTrooper.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CArmoredTrooper::CArmoredTrooper(int hp, int speed, int attack, int armor)
	: CTrooper(hp, speed, attack, '$', 'A'),
	  m_Armor(armor),
	  m_MaxArmor(armor)
{}

CArmoredTrooper * CArmoredTrooper::Clone() const
{
	return new CArmoredTrooper(*this);
}

/**********************************************************************************************************************/
// ACTIONS
void CArmoredTrooper::ReceiveDamage(int damage)
{
	// TODO Should be calculated in some other fashion- logarithm or so...
	// If armor is up it can defend some damage
	if (m_Armor > 0)
		m_Armor -= damage;
	if (m_Armor < 0)
		CTrooper::ReceiveDamage(abs(m_Armor));
	m_Armor = 0;
}

/**********************************************************************************************************************/
// LOADING
CArmoredTrooper * CArmoredTrooper::LoadTemplate(std::istream & in)
{
	CArmoredTrooper * trooper = new CArmoredTrooper();
	char del;
	if (!(in >> trooper->m_Char >> trooper->m_Hp >> trooper->m_Frames >> trooper->m_Attack >> trooper->m_MaxArmor >> del)
		|| del != ';')
		return nullptr;
	trooper->m_Armor = trooper->m_MaxArmor;
	return trooper;
}

istream & CArmoredTrooper::LoadOnMapTroop(istream & in)
{
	return CTrooper::LoadOnMapTroop(in) >> m_Armor;
}

/**********************************************************************************************************************/
// SAVING
ostream & CArmoredTrooper::SaveTemplateTroop(ostream & out) const
{
	return CTrooper::SaveTemplateTroop(out) << ' ' << m_MaxArmor;
}

ostream & CArmoredTrooper::SaveOnMapTroop(std::ostream &out) const
{
	return CTrooper::SaveOnMapTroop(out) << ' ' << m_Armor;
}
