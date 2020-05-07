/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */
 
#pragma once

#include <memory>
#include <fstream>
#include <set>
#include "CUnitStack.hpp"
#include "CMap.hpp"
#include "CWaves.hpp"
#include "EGameState.hpp"
#include "CBuffer.hpp"

/**
 * A single game with map, waves and unit stack
 */
class CGame
{
public:
	CGame();
	
	// TESTING
	void Visualize(const std::deque<pos_t> & positions);
	
	// LOADING
	friend std::istream & operator>>(std::istream & in, CGame & self);
	void SetUnitStack();
	bool CheckNew() const;
	bool CheckSaved() const;
	
	// SAVING
	friend std::ostream & operator<<(std::ostream & in, const CGame & self);
	
	// INGAME
	void Update();
	CBuffer Render(int windowWidth) const;
	
	bool Won() const
	{return m_GameState == EGameState::GAME_WON;}
	
	bool Lost() const
	{return m_GameState == EGameState::GAME_OVER;}
	
	bool Running() const
	{return m_GameState == EGameState::GAME_RUNNING;}
	
	void Quit()
	{m_GameState = EGameState::GAME_QUIT;}
	
	void CycleWaves()
	{m_Waves.Cycle();}
	
	void CycleTroops()
	{m_UnitStack->Cycle();}
	
	void DeleteTroop()
	{m_Waves.DeleteTroop();}
	
	void AddTroop()
	{m_Waves.AddTroop();}
	
	void ReleaseWaves()
	{m_Waves.Release(m_WaveOn);}
	
	const CUnitStack & GetStack() const
	{return *m_UnitStack;}
private:
	// VARIABLES
	std::shared_ptr<CUnitStack> m_UnitStack;
	CWaves m_Waves;								//!< Troops waiting in waves and user's selection of troops and waves
	CMap m_Map;									//!< Map with tiles occupied by troops, walls or towers
	bool m_WaveOn;								//!< Signalizes start and end of wave
	EGameState m_GameState;
	int m_Resources;
	
	// LOADING
	void LoadObjects(std::istream & in, std::set<char> & signs, bool & end);
	static char LoadSignatureChar(std::istream & in);
	bool CheckDefined(const std::set<char> & signs);
};
