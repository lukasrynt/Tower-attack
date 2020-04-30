/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CArmoredTrooper.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CArmoredTrooper::CArmoredTrooper(int hp, int speed, int attack, int armor)
	: CTrooper(hp, speed, attack, '$'),
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
CArmoredTrooper * CArmoredTrooper::Load(std::istream &in)
{
	CArmoredTrooper * trooper = new CArmoredTrooper();
	char del1, del2, del3, del4;
	if (!(in >> trooper->m_Hp >> del1 >> trooper->m_Frames >> del2 >> trooper->m_Attack >> del3 >> trooper->m_MaxArmor >> del4)
		|| del1 != ','
		|| del2 != ','
		|| del3 != ','
		|| del4 != ';')
		return nullptr;
	trooper->m_Char = '$';
	trooper->m_Armor = trooper->m_MaxArmor;
	return trooper;
}

/**********************************************************************************************************************/
// SAVING
ostream & CArmoredTrooper::SaveTemplate(ostream &out) const
{
	return CTrooper::SaveTemplate(out) << ", " << m_MaxArmor;
}