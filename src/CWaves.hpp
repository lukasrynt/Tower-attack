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
	void SetWavesSpecifications(int waveCnt, int maxWaveSize);
	int GetWaveSize() const;
	
	// SAVING
	std::ostream & Save(std::ostream & out) const;

	// RENDER
	void Render() const;
	
	// INPUT PROCESSING
	void Cycle();
	bool AddTroop();
	bool Release();
	
	// UPDATE
	CTrooper * Update(bool & waveOn);
	

private:
	std::vector<std::deque<CTrooper*>> m_Waves;	//!< vector with waves (deque) initialized to the size m_WaveCnt
	std::shared_ptr<CUnitStack> m_UnitStack;
	size_t m_Selected;						//!< index of currently selected wave
	size_t m_WaveCnt;							//!< number of waves specified in constructor
	size_t m_MaxSize;						//!< maximum size of one wave, specified in constructor
	CFrames m_Frames;
	bool m_ReleasingWave;
	
	// LOAD
	void InitWaves();
};
