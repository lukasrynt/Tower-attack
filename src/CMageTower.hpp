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
	// INIT
	/**
	 * Constructor
	 * @param attackDamage
	 * @param attackSpeed
	 * @param maxMana
	 */
	explicit CMageTower(int attackDamage = 0, int attackSpeed = 0, int mana = 0, int magicAttack = 0, pos_t position = pos_t::npos);
	
	// LOADING
	static CMageTower * Load(std::istream & in);
	
	// SAVING
	std::ostream & Save(std::ostream & out) const final;
	
	// ACTIONS
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