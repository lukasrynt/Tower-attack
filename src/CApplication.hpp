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
	explicit CApplication(const CInterface & interface) noexcept
			: m_Interface(interface),
			  m_AppOn(true)
	{
		InitCommands();
	}
	/**
	 * Runs the application menu screen
	 */
	void Run();
	
	// RULE OF FIVE
	~CApplication() = default;
	/**
	 * There can only be one instance of application so it can't be copied
	 */
	CApplication(const CApplication & src) = delete;
	/**
	 * There can only be one instance of application so it can't be copied
	 */
	CApplication & operator=(const CApplication & src) = delete;
	/**
	 * There can only be one instance of application so it can't be moved
	 */
	CApplication(CApplication && src) = delete;
	/**
	 * There can only be one instance of application so it can't be moved
	 */
	CApplication & operator=(CApplication && src) = delete;
private:
	CInterface m_Interface;						//!< Interface, handles I/O operations
	CGame m_Game;								//!< Game, comprises individual game objects and handles loading and saving
	std::map<char, CCommand> m_MenuCommands;	//!< Commands mapped to character input, while in menu
	std::map<char, CCommand> m_GameCommands;	//!< Commands mapped to character input, while in game
	bool m_AppOn;								//!< Runs the application menu loop
	
	// MAIN LOOP
	/**
	 * Main loop of the game, consists of update, drawing and input processing
	 */
	void MainLoop();
	/**
	 * Handles winner / game over screens, ends the game
	 */
	void EndGame();
	
	// INPUT PROCESSING
	/**
	 * Creates individual commands used later in the game
	 */
	void InitCommands();
	/**
	 * Processes the input from user and launches the corresponding command
	 */
	void ProcessInput();
	
	// COMMANDS
	/**
	 * Quits the application
	 * @return Created command
	 */
	CCommand QuitCommand();
	/**
	 * Loads new game
	 * @return Created command
	 */
	CCommand LoadNewCommand();
	/**
	 * Loads saved game
	 * @return Created command
	 */
	CCommand LoadSavedCommand();
	/**
	 * Adds troop to current wave
	 * @return Created command
	 */
	CCommand AddTroopCommand();
	/**
	 * Deletes troop from current wave
	 * @return Created command
	 */
	CCommand DeleteTroopCommand();
	/**
	 * Moves the selection in unit stack to next trooper
	 * @return Created command
	 */
	CCommand CycleTroopsCommand();
	/**
	 * Moves the selection in waves to next wave
	 * @return Created command
	 */
	CCommand CycleWavesCommand();
	/**
	 * Releases all waves
	 * @return Created command
	 */
	CCommand ReleaseWavesCommand();
	/**
	 * Opens the help screen
	 * @return Created command
	 */
	CCommand HelpCommand();
	/**
	 * Saves the game
	 * @return Created command
	 */
	CCommand SaveCommand();
	/**
	 * Quits the game and returns to original menu
	 * @return Created command
	 */
	CCommand QuitGameCommand();
};
