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
	explicit CMageTower(int attackDamage = 0, int attackSpeed = 0, int range = 0, int waveSpeed = 0, pos_t position = pos::npos);
	CMageTower * Clone() const final
	{return new CMageTower(*this);}
	
	// LOADING
	std::istream & LoadTemplate(std::istream & in) final;
	std::istream & LoadOnMap(std::istream & in) final;
	CTile GetTile() const final
	{return m_Tile;}
	
	// SAVING
	std::ostream & SaveTemplate(std::ostream & out) const final;
	std::ostream & SaveOnMap(std::ostream & out) const final;
	
	// ACTIONS
	CBuffer CreateInfoBuffer(int windowWidth) const final;
	bool Attack(std::unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper*> & troops) final;
private:
	CFrames m_WaveStatus;
	int m_WaveLevel;
	std::deque<pos_t> m_LastWave;
	
	bool PerimeterBreached(const std::unordered_map<pos_t, CTrooper*> & troops) const;
	void ClearLastWave(std::unordered_map<pos_t,CTile> & map) const;
	void SendNewWave(std::unordered_map<pos_t,CTile> & map, std::unordered_map<pos_t, CTrooper*> & troops);
};