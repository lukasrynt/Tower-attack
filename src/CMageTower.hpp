/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include "CTower.hpp"

/**
 * (%) Mage tower with magic attacks
 */
class CMageTower : public CTower
{
public:
	/**
	 * Constructor
	 * @param attackDamage
	 * @param attackSpeed
	 * @param maxMana
	 */
	explicit CMageTower(pos_t position, int attackDamage = 0, int attackSpeed = 0, int mana = 0, int magicAttack = 0);
	/**
	 * Adds magical attack to the base damage
	 */
	void Attack(std::unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper *> & troops) final;
private:
	int m_MaxMana;		//!< Maximum amount of mana the mage can hold
	int m_Mana;			//!< Current mana level
	int m_MagicAttack;	//!< Damage of magic attacks
	
	void MagicAttack();
};