/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CMageTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CMageTower::CMageTower(int attackDamage, int attackSpeed, int mana, int magicAttack, pos_t position)
	: CTower(attackDamage, attackSpeed, position, '%', 'M'),
	  m_ManaFrames(mana),
	  m_MagicAttack(magicAttack)
{}

/**********************************************************************************************************************/
// LOADING
CMageTower * CMageTower::LoadTemplate(istream & in)
{
	CMageTower * tower = new CMageTower();
	char del;
	if (!(in >> tower->m_Char >> tower->m_AttackDamage >> tower->m_Frames >> tower->m_ManaFrames >> tower->m_MagicAttack >> del)
		|| del != ';')
		return nullptr;
	return tower;
}

istream & CMageTower::LoadOnMapTower(istream & in)
{
	int curr;
	if (!(CTower::LoadOnMapTower(in) >> curr))
		return in;
	m_Frames.SetCurrent(curr);
	return in;
}

/**********************************************************************************************************************/
// SAVING
ostream & CMageTower::SaveTemplateTower(ostream & out) const
{
	return CTower::SaveTemplateTower(out) << ' ' << m_ManaFrames << ' ' << m_MagicAttack;
}

ostream & CMageTower::SaveOnMapTower(ostream & out) const
{
	return CTower::SaveOnMapTower(out) << ' ' << m_ManaFrames.GetCurrent();
}

/**********************************************************************************************************************/
// ATTACK
void CMageTower::SpecialAttack(unordered_map<pos_t, CTile> & map, int rows, int cols, unordered_map<pos_t, CTrooper *> & troops)
{
	if (m_ManaFrames.ActionAllowed())
		MagicAttack();
	else
		CTower::SpecialAttack(map, rows, cols, troops);
}

void CMageTower::MagicAttack() {}
