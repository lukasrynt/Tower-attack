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
 * Represents the waves to which the user can add or delete troops and release those waves
 */
class CWaves
{
public:
	// INIT
	/**
	 * Assigns unit stack to waves
	 * @param unitStack Pointer to CUnitStack that we want to assign
	 */
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
	 * @param width Size of the window to be rendered into
	 * @return Created buffer
	 */
	CBuffer Draw(size_t width) const;
	
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
	std::vector<std::unique_ptr<CTrooper>> Update(const std::map<int,bool> & spawnersBlocked);
	/**
	 * Checks whether we have not lost, this occurs if all waves are empty and we can't afford any troop.
	 * @return true if we have lost
	 */
	bool Lost() const;
	

private:
	std::vector<std::deque<std::unique_ptr<CTrooper>>> m_Waves;	//!< Vector with waves (deque) initialized to the size m_WaveCnt
	std::shared_ptr<CUnitStack> m_UnitStack;					//!< Unit stack with units templates
	size_t m_Selected = 0;										//!< Index of currently selected wave
	size_t m_MaxSize = 0;										//!< Maximum size of one wave, specified in constructor
	CCounter m_Frames;											//!< Speed of the spawner
	bool m_ReleasingWave = false;								//!< True if waves are currently being released
	int m_Resources = 0;										//!< Resources given for the current game
	
	// LOAD
	std::deque<std::unique_ptr<CTrooper>> LoadWaves(std::istream & in);
	bool CheckCounter(size_t counter);
	
	bool Empty() const;
};
