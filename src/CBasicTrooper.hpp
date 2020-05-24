/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24/05/2020.
*/

#pragma once

#include "CTrooper.hpp"

class CBasicTrooper : public CTrooper
{
public:
	// INIT
	explicit CBasicTrooper() noexcept
		: CTrooper('@', ETileType::BASIC_TROOP, Colors::FG_YELLOW)
	{}
	
	std::unique_ptr<CTrooper> Clone() const noexcept override
	{return std::make_unique<CBasicTrooper>(*this);}
	
	// LOAD
	std::istream & LoadTemplate(std::istream & in) override
	{return CTrooper::LoadTemplate(in);}
	
	std::istream & LoadOnMap(std::istream & in) override
	{return CTrooper::LoadOnMap(in);}
	
	// SAVE
	std::ostream & SaveTemplate(std::ostream & out) const override
	{return CTrooper::SaveTemplate(out);}
	
	std::ostream & SaveOnMap(std::ostream & out) const override
	{return CTrooper::SaveOnMap(out);}
	
	// ACTIONS
	bool Update() override;
	
	CBuffer CreateInfoBuffer(int windowWidth) const override;
	
	void ReceiveDamage(int damage, std::string bulletColor) override;
};


