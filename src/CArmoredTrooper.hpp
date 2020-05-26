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
	 * Constructor - initializes values
	 */
	explicit CArmoredTrooper() noexcept
		: CTrooper('$', ETileType::ARMORED_TROOP, Colors::FG_CYAN),
		  m_ArmorOn(false)
	{}
	/**
	 * @return cloned unit
	 */
	std::unique_ptr<CTrooper> Clone() const noexcept final
	{return std::make_unique<CArmoredTrooper>(*this);}
	
	// ACTIONS
	/**
	 * Adds armor which can prevent a certain amount of damage
	 * @param damage Damage received
	 * @param bulletColor Background color the troop should accept when hit
	 */
	void ReceiveDamage(int damage, std::string bulletColor) final;
	/**
	 * Creates buffer with information about the unit
	 * @param width Width of the screen
	 * @return created buffer
	 */
	CBuffer CreateInfoBuffer(size_t width) const final;
	/**
	 * Updates the state of the unit - takes care of armor
	 * @return True if the unit can do something
	 */
	bool Update() override;
	
	// LOAD
	/**
	 * Loads template unit from input stream
	 * @param in Input stream
	 * @return in
	 */
	std::istream & LoadTemplate(std::istream & in) final
	{return CTrooper::LoadTemplate(in) >> m_Armor;}
	/**
	 * Loads unit on map - with its armor state
	 * @param in Input stream
	 * @return in
	 */
	std::istream & LoadOnMap(std::istream & in) final;
	
	// SAVE
	/**
	 * Saves unit template to output stream
	 * @param out Output stream
	 * @return out
	 */
	std::ostream & SaveTemplate(std::ostream & out) const final
	{return CTrooper::SaveTemplate(out) << ' ' << m_Armor;}
	/**
	 * Saves unit on map to output stream
	 * @param out Output stream
	 * @return out
	 */
	std::ostream & SaveOnMap(std::ostream & out) const final
	{return CTrooper::SaveOnMap(out) << ' ' << m_Armor.GetCurrent() << ' ' << m_ArmorOn;}

private:
	CCounter m_Armor;	//!< Counter for armor state - unit can ignore one attack if armor is up
	bool m_ArmorOn;		//!< Turned true if the armor is currently set on
};
