#pragma once

#include "CTrooper.hpp"

/**
 * Tougher version of normal trooper with armor on him
 */
class CArmoredTrooper : public CTrooper
{
public:
	/**
	 * Constructor
	 * @param hp
	 * @param speed
	 * @param attack
	 * @param armor
	 */
	explicit CArmoredTrooper(int hp = 0, int speed = 0, int attack = 0, int armor = 0);
	/**
	 * Adds armor which can prevent a certain amount of damage
	 * @param damage
	 */
	void ReceiveDamage(int damage) final;
	/**
	 * Implements more direct approach to goal
	 * @param goal
	 */
	void FindPath(pos_t goal, std::unordered_map<pos_t,CTile> & map);
	
	CArmoredTrooper * Clone() final;

private:
	int m_Armor;	//!< Current armor
	int m_MaxArmor;	//!< Maximum armor
};
