/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24/05/2020.
*/

#pragma once

#include "CTrooper.hpp"

/**
 * Basic trooper, has no special powers
 */
class CBasicTrooper : public CTrooper
{
public:
	// INIT
	/**
	 * Constructor - initializes values
	 */
	explicit CBasicTrooper() noexcept
		: CTrooper('@', ETileType::BASIC_TROOP, Colors::FG_YELLOW)
	{}
	/**
	 * @return Cloned unit
	 */
	std::unique_ptr<CTrooper> Clone() const noexcept override
	{return std::make_unique<CBasicTrooper>(*this);}
	
	// LOAD
	/**
	 * Loads template unit from input stream
	 * @param in Input stream
	 * @return in
	 */
	std::istream & LoadTemplate(std::istream & in) override
	{return CTrooper::LoadTemplate(in);}
	/**
	 * Loads unit on map - with its armor state
	 * @param in Input stream
	 * @return in
	 */
	std::istream & LoadOnMap(std::istream & in) override
	{return CTrooper::LoadOnMap(in);}
	
	// SAVE
	/**
	 * Saves unit template to output stream
	 * @param out Output stream
	 * @return out
	 */
	std::ostream & SaveTemplate(std::ostream & out) const override
	{return CTrooper::SaveTemplate(out);}
	/**
	 * Saves unit on map to output stream
	 * @param out Output stream
	 * @return out
	 */
	std::ostream & SaveOnMap(std::ostream & out) const override
	{return CTrooper::SaveOnMap(out);}
	
	// ACTIONS
	/**
	 * Updates trooper frames
	 * @return True if the trooper can perform action
	 */
	bool Update() override;
	/**
	 * Creates buffer with information about the unit
	 * @param width Width of the screen
	 * @return created buffer
	 */
	CBuffer CreateInfoBuffer(size_t width) const override;
	/**
	 * Receive damage
	 * @param damage Damage received
	 * @param bulletColor Background color the troop should accept when hit
	 */
	void ReceiveDamage(int damage, std::string bulletColor) override
	{CTrooper::ReceiveDamage(damage, bulletColor);}
};


