/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <queue>
#include <memory>
#include "CTrooper.hpp"
#include "CTower.hpp"
#include "CUnitStack.hpp"

/**
 * Represents the stack from which the user can choose troops, add them to certain waves and launch these waves to map
 */
class CWaves
{
public:
	// INIT
	CWaves();
	~CWaves();
	CWaves(const CWaves & src) = default;
	CWaves & operator=(const CWaves & src) = default;
	void AssignUnitStack(std::shared_ptr<CUnitStack> unitStack);
	
	// LOADING
	/**
	 * Loads waves from input file stream
	 * @param in Input stream
	 * @param self Waves to be loaded
	 * @return in
	 */
	friend std::istream & operator>>(std::istream & in, CWaves & waves);
	bool CheckNew() const;
	int GetWaveSize() const;
	int GetWaveCnt() const
	{return m_Waves.size();}
	
	// SAVING
	/**
	 * Saves the waves on the given output stream
	 * @param out Output stream
	 * @param self Waves to be saved
	 * @return out
	 */
	friend std::ostream & operator<<(std::ostream & out, const CWaves & waves);

	// RENDER
	/**
	 * Create buffer for further renderings
	 * @param windowWidth Size of the window to be rendered into
	 * @return Created buffer
	 */
	CBuffer CreateBuffer(int windowWidth) const;
	
	// INPUT PROCESSING
	/**
	 * Moves the cursor to next wave
	 */
	void Cycle();
	/**
	 * Adds trooper to current wave
	 * @throws invalid_input if the current wave is full or if we can't afford the unit
	 */
	void AddTroop();
	/**
	 * Deletes trooper from current wave
	 * @throws invalid_input if the current wave is empty
	 */
	void DeleteTroop();
	/**
	 * Releases all waves if possible
	 * @param waveOn Signals whether the wave is on. Will be set to true if it is on
	 * @throws invalid_input if there are no troops to be released or if the wave is already being released
	 */
	void Release(bool & waveOn);
	
	// UPDATE
	/**
	 * Updates the waves, if the waves are being released then releases one trooper from each wave
	 * @param spawnersBlocked map of spawners and their occupancy status
	 * @return vector of troops that were spawned
	 */
	std::vector<CTrooper*> Update(const std::map<int,bool> & spawnersBlocked);
	/**
	 * Checks whether we have not lost, this occurs if all waves are empty and we can't afford any troop.
	 * @return true if we have lost
	 */
	bool Lost() const;
	

private:
	std::vector<std::deque<CTrooper*>> m_Waves;	//!< vector with waves (deque) initialized to the size m_WaveCnt
	std::shared_ptr<CUnitStack> m_UnitStack;
	size_t m_Selected;						//!< index of currently selected wave
	size_t m_MaxSize;						//!< maximum size of one wave, specified in constructor
	CFrames m_Frames;
	bool m_ReleasingWave;
	int m_Resources;
	
	// LOAD
	void LoadWaves(std::istream & in);
	bool CheckCounter(size_t counter);
	
	bool Empty() const;
};
