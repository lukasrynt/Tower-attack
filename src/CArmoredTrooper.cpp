/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CArmoredTrooper.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CArmoredTrooper::CArmoredTrooper(int hp, int speed, int attack, int cost, int armor)
	: CTrooper(hp, speed, attack, cost, {'$', ETileType::ARMORED_TROOP, Colors::FG_CYAN}),
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
	// if armor is up we can ignore an attack and remove armor
	if (!m_Armor)
		CTrooper::ReceiveDamage(damage);
	else
		m_Armor -= damage;
	
	if (m_Armor < 0)
		m_Armor = 0;
}

ostream & CArmoredTrooper::RenderInfo(ostream & out) const
{
	return out << m_Tile.GetColor()
			   << "Armored trooper, hp: " << m_Hp
			   << ", speed: " << m_Frames
			   << ", attack: " << m_Attack
			   << ", armor: " << m_MaxArmor
			   << ", cost: " << m_Price << Colors::FG_YELLOW << " ©" << Colors::RESET << endl;
}

/**********************************************************************************************************************/
// LOADING
istream & CArmoredTrooper::LoadTemplate(istream & in)
{
	return CTrooper::LoadTemplate(in) >> m_MaxArmor;
}

istream & CArmoredTrooper::LoadOnMap(istream & in)
{
	return CTrooper::LoadOnMap(in) >> m_Armor;
}

/**********************************************************************************************************************/
// SAVING
ostream & CArmoredTrooper::SaveTemplate(ostream & out) const
{
	return CTrooper::SaveTemplate(out) << ' ' << m_MaxArmor;
}

ostream & CArmoredTrooper::SaveOnMap(std::ostream & out) const
{
	return CTrooper::SaveOnMap(out) << ' ' << m_Armor;
}
