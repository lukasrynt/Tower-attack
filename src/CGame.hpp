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

/**
 * A single game with map, waves and unit stack
 */
class CGame
{
public:
	CGame();
	
	// LOADING
	std::istream & LoadNew(std::istream & in);
	std::istream & LoadSaved(std::istream & in);
	
	// SAVING
	std::ostream & Save(std::ostream & out) const;
	std::ostream & SaveState(std::ostream & out) const;
	
	// INGAME
	void End();
	bool IsOn() const;
	void Update();
	void Render() const;
	
	// INPUT PROCESSING
	void ProcessInput(char ch);

private:
	// VARIABLES
	std::shared_ptr<CUnitStack> m_UnitStack;
	CWaves m_Waves;								//!< Troops waiting in waves and user's selection of troops and waves
	CMap m_Map;									//!< Map with tiles occupied by troops, walls or towers
	bool m_GameOn;								//!< Signalizes if the game has ended or not
	bool m_WaveOn;								//!< Signalizes start and end of wave
	
	// LOADING
	std::istream & LoadCommon(std::istream & in, char ch, bool saved);
	std::istream & LoadState(std::istream & in);
	static char LoadSignatureChar(std::istream & in);
	bool CheckLoaded(const std::set<char> & signs);
	
	// INGAME
	void AddTroopToWave();
	void StartWave();
};
