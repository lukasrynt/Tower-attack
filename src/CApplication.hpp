/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include "NColors.hpp"
#include "CGame.hpp"
#include "CInterface.hpp"
#include "CCommand.hpp"
#include <functional>

class CApplication
{
public:
	/**
	 * Constructor for application
	 * @param interface Interface that handle I/O operations
	 */
	explicit CApplication(const CInterface& interface) noexcept;
	/**
	 * Runs the application menu screen
	 */
	void Run();
private:
	CInterface m_Interface;						//!< Interface, handles I/O operations
	std::unique_ptr<CGame> m_Game;				//!< Game, comprises individual game objects and handles loading and saving
	std::map<char, CCommand> m_MenuCommands;	//!< Commands mapped to character input, while in menu
	std::map<char, CCommand> m_GameCommands;	//!< Commands mapped to character input, while in game
	bool m_AppOn;								//!< Runs the application menu loop
	
	// MAIN LOOP
	void MainLoop();
	void EndGame();
	
	// INPUT PROCESSING
	void InitCommands();
	void ProcessInput();
	CCommand Quit();
	CCommand LoadNew();
	CCommand LoadSaved();
	CCommand AddTroop();
	CCommand DeleteTroop();
	CCommand CycleTroops();
	CCommand CycleWaves();
	CCommand ReleaseWaves();
	CCommand Help();
	CCommand Save();
	CCommand QuitGame();
};
