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
	// TESTING
	/**
	 * Visualizes the positions on the map
	 * @param positions Positions to be visualized
	 */
	void Visualize(const std::deque<pos_t> & positions);
	
	// LOADING
	/**
	 * Loads the game from input file stream
	 * @param in Input stream
	 * @param self Game to be loaded
	 * @return in
	 */
	friend std::istream & operator>>(std::istream & in, CGame & self);
	/**
	 * Create and assign CUnitStack to elements in game
	 */
	void AssignUnitStack();
	/**
	 * Checks if new game was correctly loaded and generates + places towers on map
	 * @return true if it was loaded correctly
	 */
	bool CheckNew();
	/**
	 * Checks if saved game was correctly loaded and places towers and troops on map
	 * @return true if it was loaded correctly
	 */
	bool CheckSaved();
	
	// SAVING
	/**
	 * Saves the game
	 * @param out output file stream
	 * @param self game to be saved
	 * @return out
	 */
	friend std::ostream & operator<<(std::ostream & out, const CGame & self);
	
	// INGAME
	/**
	 * Updates all entities inside game
	 */
	void Update();
	/**
	 * Creates buffer for further renderings
	 * @param windowWidth size of the window
	 * @return Created buffer
	 */
	CBuffer CreateBuffer(size_t height) const;
	
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
	std::shared_ptr<CUnitStack> m_UnitStack;			//!< Unit stack with units templates
	CWaves m_Waves;										//!< Troops waiting in waves and user's selection of troops and waves
	CMap m_Map;											//!< Map with tiles occupied by troops, walls or towers
	bool m_WaveOn = false;								//!< Signalizes start and end of wave
	EGameState m_GameState = EGameState::GAME_RUNNING;	//!< Current state of the game
	
	// LOADING
	void LoadObjects(std::istream & in, std::set<char> & signs, bool & end);
	static char LoadSignatureChar(std::istream & in);
	bool CheckDefined(const std::set<char> & signs);
};
