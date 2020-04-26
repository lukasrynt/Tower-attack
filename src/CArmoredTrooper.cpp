/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CArmoredTrooper.hpp"

CArmoredTrooper::CArmoredTrooper(int hp, int speed, int attack, int armor)
	: CTrooper(hp, speed, attack, '$'),
	  m_Armor(armor),
	  m_MaxArmor(armor)
{}

void CArmoredTrooper::ReceiveDamage(int damage)
{
	// TODO Should be calculated in some other fashion- logarithm or so... this is just stupid
	// If armor is up it can defend some damage
	if (m_Armor > 0)
		m_Armor -= damage;
	if (m_Armor < 0)
		CTrooper::ReceiveDamage(abs(m_Armor));
	m_Armor = 0;
}

void CArmoredTrooper::FindPath(pos_t goal, std::unordered_map<pos_t,CTile> & map)
{

}

CArmoredTrooper * CArmoredTrooper::Clone() const
{
	return new CArmoredTrooper(*this);
}