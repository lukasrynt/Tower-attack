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
	friend std::istream & operator>>(std::istream & in, CWaves & waves);
	bool CheckNew() const;
	int GetWaveSize() const;
	int GetWaveCnt() const
	{return m_Waves.size();}
	
	// SAVING
	friend std::ostream & operator<<(std::ostream & out, const CWaves & waves);

	// RENDER
	void Render() const;
	
	// INPUT PROCESSING
	void Cycle();
	bool AddTroop();
	bool Release();
	
	// UPDATE
	std::vector<CTrooper*> Update(const std::map<int,bool> & spawnersBlocked);
	

private:
	std::vector<std::deque<CTrooper*>> m_Waves;	//!< vector with waves (deque) initialized to the size m_WaveCnt
	std::shared_ptr<CUnitStack> m_UnitStack;
	size_t m_Selected;						//!< index of currently selected wave
	size_t m_MaxSize;						//!< maximum size of one wave, specified in constructor
	CFrames m_Frames;
	bool m_ReleasingWave;
	
	// LOAD
	std::istream & LoadWaves(std::istream & in);
	bool CheckCounter(size_t counter);
};
