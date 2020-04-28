/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CMageTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CMageTower::CMageTower(int attackDamage, int attackSpeed, int mana, int magicAttack, pos_t position)
	: CTower(attackDamage, attackSpeed, position, '%'),
	  m_MaxMana(mana),
	  m_Mana(mana),
	  m_MagicAttack(magicAttack)
{}

/**********************************************************************************************************************/
// LOADING
CMageTower * CMageTower::Load(std::istream &in)
{
	CMageTower * tower = new CMageTower();
	char del1, del2, del3, del4;
	if (!(in >> tower->m_AttackDamage >> del1 >> tower->m_Frames >> del2 >> tower->m_MaxMana >> del3 >> tower->m_MagicAttack >> del4)
		|| del1 != ','
		|| del2 != ','
		|| del3 != ','
		|| del4 != ';')
		return nullptr;
	tower->m_Mana = tower->m_MaxMana;
	return tower;
}

/**********************************************************************************************************************/
// SAVING
std::ostream & CMageTower::Save(std::ostream &out) const
{
	return CTower::Save(out) << ", " << m_MaxMana << ", " << m_MagicAttack;
}

/**********************************************************************************************************************/
// ATTACK
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
