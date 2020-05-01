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
	CMageTower * Clone() const final
	{return new CMageTower(*this);}
	
	// LOADING
	static CMageTower * LoadTemplate(std::istream & in);
	std::istream & LoadOnMapTower(std::istream & in) final;
	CTile GetTile() const final
	{return CTile{m_Char, ETileType::TOWER, Colors::bg_blue};}
	
	// SAVING
	std::ostream & SaveTemplateTower(std::ostream & out) const final;
	std::ostream & SaveOnMapTower(std::ostream & out) const final;
	
	// ACTIONS
	/**
	 * Adds magical attack to the base damage
	 */
	void SpecialAttack(std::unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper *> & troops) final;
private:
	CFrames m_ManaFrames;
	int m_MagicAttack;	//!< Damage of magic attacks
	
	void MagicAttack();
};