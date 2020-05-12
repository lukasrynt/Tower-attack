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
	explicit CMageTower() noexcept
		: CTower('%', ETileType::MAGE_TOWER, Colors::BG_BLUE),
		  m_WaveStatus({}),
		  m_WaveLevel(0)
	{}
	
	CMageTower * Clone() const override
	{return new CMageTower(*this);}
	
	// LOADING
	std::istream & LoadTemplate(std::istream & in) override
	{return CTower::LoadTemplate(in) >> m_WaveStatus;}
	
	std::istream & LoadOnMap(std::istream & in) override;
	
	// SAVING
	std::ostream & SaveTemplate(std::ostream & out) const override
	{return CTower::SaveTemplate(out) << ' ' << m_WaveStatus;}
	
	std::ostream & SaveOnMap(std::ostream & out) const override
	{return CTower::SaveOnMap(out) << ' ' << m_WaveStatus.GetCurrent();}
	
	// ACTIONS
	CBuffer CreateInfoBuffer(int windowWidth) const override;
	bool Attack(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops, int rows, int cols) override;
private:
	CFrames m_WaveStatus;
	int m_WaveLevel;
	std::deque<pos_t> m_LastWave;
	
	bool PerimeterBreached(const std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops) const;
	void ClearLastWave(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map) const;
	void SendNewWave(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops);
};