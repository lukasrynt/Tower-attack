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
	explicit CArmoredTrooper() noexcept
		: CTrooper('$', ETileType::ARMORED_TROOP, Colors::FG_CYAN),
		  m_Armor(0),
		  m_MaxArmor(0)
	{}
	
	std::unique_ptr<CTrooper> Clone() const noexcept final
	{return std::make_unique<CArmoredTrooper>(*this);}
	
	/**
	 * Adds armor which can prevent a certain amount of damage
	 * @param damage
	 */
	void ReceiveDamage(int damage, std::string bulletColor) final;
	CBuffer CreateInfoBuffer(size_t width) const final;
	
	// LOAD
	std::istream & LoadTemplate(std::istream & in) final
	{return CTrooper::LoadTemplate(in) >> m_MaxArmor;}
	
	std::istream & LoadOnMap(std::istream & in) final
	{return CTrooper::LoadOnMap(in) >> m_Armor;}
	
	
	// SAVE
	std::ostream & SaveTemplate(std::ostream & out) const final
	{return CTrooper::SaveTemplate(out) << ' ' << m_MaxArmor;}
	
	std::ostream & SaveOnMap(std::ostream & out) const final
	{return CTrooper::SaveOnMap(out) << ' ' << m_Armor;}
	
	bool Update() override;

private:
	int m_Armor;	//!< Current armor
	int m_MaxArmor;	//!< Maximum armor
};
