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
	explicit CMageTower(int attackDamage = 0, int attackSpeed = 0, int mana = 0, int magicAttack = 0, pos_t position = pos::npos);
	CMageTower * Clone() const final
	{return new CMageTower(*this);}
	
	// LOADING
	std::istream & LoadTemplate(std::istream & in) final;
	std::istream & LoadOnMap(std::istream & in) final;
	CTile GetTile() const final
	{return CTile{m_Char, ETileType::TOWER, Colors::bg_blue};}
	
	// SAVING
	std::ostream & SaveTemplate(std::ostream & out) const final;
	std::ostream & SaveOnMap(std::ostream & out) const final;
	
	// ACTIONS
	void SpecialAttack(std::unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper *> & troops) final;
private:
	CFrames m_ManaFrames;
	int m_MagicAttack;	//!< Damage of magic attacks
	
	void MagicAttack();
};