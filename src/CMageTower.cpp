/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CMageTower.hpp"

using namespace std;

CMageTower::CMageTower(pos_t position, int attackDamage, int attackSpeed, int mana, int magicAttack)
	: CTower(position, attackDamage, attackSpeed),
	  m_MaxMana(mana),
	  m_Mana(mana),
	  m_MagicAttack(magicAttack)
{}

void CMageTower::Attack(unordered_map<pos_t, CTile> & map, int rows, int cols, unordered_map<pos_t, CTrooper *> & troops)
{
	if (m_Frames.ActionAllowed() && m_Mana == m_MaxMana)
		MagicAttack();
	else
		CTower::Attack(map, rows, cols, troops);
	
	// If mana is charged, add magic attack to basic attack
	if (m_Mana == m_MaxMana)
	{
		m_Mana = 0;
	}
	else
		m_Mana += 10;
}

void CMageTower::MagicAttack() {}
