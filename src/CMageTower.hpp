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
		: CTower('%', ETileType::MAGE_TOWER, Colors::BG_BLUE)
	{}
	
	std::unique_ptr<CTower> Clone() const override
	{return std::make_unique<CMageTower>(*this);}
	
	// LOADING
	std::istream & LoadOnMap(std::istream & in) override
	{return CTower::LoadOnMap(in) >> m_WaveLevel;}
	
	// SAVING
	std::ostream & SaveOnMap(std::ostream & out) const override
	{return CTower::SaveOnMap(out) << ' ' << m_WaveLevel;}
	
	// ACTIONS
	/**
	 * Creates buffer with information about the tower
	 * @param width Width of the screen
	 * @return Created buffer
	 */
	CBuffer DrawHelpInfo(size_t width) const override;
	/**
	 * Attacks troopers with waves of electricity that can even pass through walls
	 * @param map Map on which the tower is
	 * @param troops Troopers on the map
	 * @param rows Rows of the map
	 * @param cols Columns of the map
	 * @return True if some unit was hit
	 */
	bool Attack(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops, int rows, int cols) override;
private:
	int m_WaveLevel = 0;			// Status of current wave
	std::deque<pos_t> m_LastWave;	// Positions of last wave
	
	/**
	 * Checks if some trooper is closer than range
	 * @param troops Troops on the map
	 * @return If trooper has entered perimeter
	 */
	bool PerimeterBreached(const std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops) const;
	/**
	 * Clears all positions on the map on which the last wave was
	 * @param map Map on which the tower is
	 */
	void ClearLastWave(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map) const;
	/**
	 * Moves the wave on next level
	 * @param map Map on which the tower is
	 * @param troops Troopers on the map
	 */
	void SendNewWave(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops);
};