/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CMageTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CMageTower::CMageTower(int attackDamage, int attackSpeed, int mana, int magicAttack, pos_t position)
	: CTower(attackDamage, attackSpeed, position, {'%', ETileType::MAGE_TOWER, Colors::bg_blue}),
	  m_ManaFrames(mana),
	  m_MagicAttack(magicAttack)
{}

/**********************************************************************************************************************/
// LOADING
istream & CMageTower::LoadTemplate(istream & in)
{
	return CTower::LoadTemplate(in) >> m_ManaFrames >> m_MagicAttack;
}

istream & CMageTower::LoadOnMap(istream & in)
{
	int curr;
	if (!(CTower::LoadOnMap(in) >> curr))
		return in;
	m_Frames.SetCurrent(curr);
	return in;
}

/**********************************************************************************************************************/
// SAVING
ostream & CMageTower::SaveTemplate(ostream & out) const
{
	return CTower::SaveTemplate(out) << ' ' << m_ManaFrames << ' ' << m_MagicAttack;
}

ostream & CMageTower::SaveOnMap(ostream & out) const
{
	return CTower::SaveOnMap(out) << ' ' << m_ManaFrames.GetCurrent();
}

/**********************************************************************************************************************/
// ATTACK
bool CMageTower::Attack(unordered_map<pos_t, CTile> & map, int rows, int cols, unordered_map<pos_t, CTrooper *> & troops)
{
	MagicAttack();
	return CTower::Attack(map, rows, cols, troops);
}

void CMageTower::MagicAttack() {}
