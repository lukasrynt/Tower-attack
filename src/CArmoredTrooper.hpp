/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include "CTrooper.hpp"

/**
 * Tougher version of normal trooper with armor on him
 */
class CArmoredTrooper : public CTrooper
{
public:
	// INIT
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
	std::ostream & RenderInfo(std::ostream & out) const final;
	CArmoredTrooper * Clone() const final;
	
	// LOAD
	std::istream & LoadTemplate(std::istream & in) final;
	std::istream & LoadOnMap(std::istream & in) final;
	
	
	// SAVE
	std::ostream & SaveTemplate(std::ostream & out) const final;
	std::ostream & SaveOnMap(std::ostream & out) const final;

private:
	int m_Armor;	//!< Current armor
	int m_MaxArmor;	//!< Maximum armor
};
