/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include "escape_sequences.h"
#include "CGame.hpp"
#include "CInterface.hpp"
#include "CCommand.hpp"
#include <functional>

/**
 * The main part of the program, launches the game and handles users commands
 */
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
	CCommand QuitCommand();
	CCommand LoadNewCommand();
	CCommand LoadSavedCommand();
	CCommand AddTroopCommand();
	CCommand DeleteTroopCommand();
	CCommand CycleTroopsCommand();
	CCommand CycleWavesCommand();
	CCommand ReleaseWavesCommand();
	CCommand HelpCommand();
	CCommand SaveCommand();
	CCommand QuitGameCommand();
};
