/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 08/05/2020.
*/

#pragma once

#include "CTower.hpp"

class CArcherTower : public CTower
{
public:
	// INIT
	explicit CArcherTower() noexcept
		: CTower('*', ETileType::ARCHER_TOWER, Colors::BG_RED)
	{}
	
	std::unique_ptr<CTower> Clone() const override
	{return std::make_unique<CArcherTower>(*this);}
	
	// LOAD
	std::istream & LoadTemplate(std::istream & in) override
	{return CTower::LoadTemplate(in);}
	
	std::istream & LoadOnMap(std::istream & in) override
	{return CTower::LoadOnMap(in);}
	
	// SAVE
	std::ostream & SaveTemplate(std::ostream & out) const override
	{return CTower::SaveTemplate(out);}
	
	std::ostream & SaveOnMap(std::ostream & out) const override
	{return CTower::SaveOnMap(out);}
	
	// ACTIONS
	CBuffer CreateInfoBuffer(size_t width) const override;
	bool Attack(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops, int rows, int cols) override;

private:
	CCounter m_ArrowSpeed;	//TODO
	pos_t m_ArrowPos;
	std::deque<pos_t> m_ArrowPath;
	
	bool ArrowMove(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map);
	void ArrowClear(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map);
	bool AssignArrow(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, const std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops, int rows, int cols);
	void DamageTrooper(std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops);
};


