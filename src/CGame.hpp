/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include <memory>
#include <fstream>
#include <set>
#include "CUnitStack.hpp"
#include "CMap.hpp"
#include "CWaves.hpp"
#include "EGameState.hpp"

/**
 * A single game with map, waves and unit stack
 */
class CGame
{
public:
	CGame();
	
	// LOADING
	friend std::istream & operator>>(std::istream & in, CGame & self);
	bool CheckNew() const;
	bool CheckSaved() const;
	
	// SAVING
	friend std::ostream & operator<<(std::ostream & in, const CGame & self);
	
	// INGAME
	void Update();
	void Render() const;
	bool Won() const
	{return m_GameState == EGameState::GAME_WON;}
	
	bool Lost() const
	{return m_GameState == EGameState::GAME_OVER;}
	
	bool Running() const
	{return m_GameState == EGameState::GAME_RUNNING;}
	
	bool Ended() const
	{return m_GameState == EGameState::GAME_QUIT;}
	
	void Quit()
	{m_GameState = EGameState::GAME_QUIT;}
	
	// INPUT PROCESSING
	void ProcessInput(char ch);

private:
	// VARIABLES
	std::shared_ptr<CUnitStack> m_UnitStack;
	CWaves m_Waves;								//!< Troops waiting in waves and user's selection of troops and waves
	CMap m_Map;									//!< Map with tiles occupied by troops, walls or towers
	bool m_WaveOn;								//!< Signalizes start and end of wave
	EGameState m_GameState;
	
	// LOADING
	static char LoadSignatureChar(std::istream & in);
	bool CheckDefined(const std::set<char> & signs);
	
	// INGAME
	void AddTroopToWave();
	void StartWave();
};
