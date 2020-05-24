/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 02/05/2020.
*/

#pragma once

#include <iostream>
#include <termios.h>
#include <chrono>
#include "CGame.hpp"
#include "CCommand.hpp"
#include "CBuffer.hpp"

class CInterface
{
public:
	/**
	 * Constructor, turns off terminal canonical mode and enters raw mode
	 * (and turns off some additional flag like allowing CTRL-C)
	 * @param out Output stream
	 * @throws ios::failure when there was an error during setting the terminal to raw mode
	 */
	explicit CInterface(std::ostream & out) noexcept(false);
	/**
	 * Disables raw mode and return terminal back to original state
	 * @throws ios::failure when there was an error during resetting the terminal back to original
	 */
	~CInterface() noexcept(false);
	
	// SCREENS
	/**
	 * Saves the game and handles save screen
	 * @param game Game to be saved
	 * @return false if the user wanted to return
	 */
	bool Save(const CGame & game);
	/**
	 * Loads new game from files, handles loading screen
	 * @param game Game to be loaded
	 * @return false if the user wanted to return
	 */
	bool LoadNewGame(CGame & game);
	/**
	 * Loads saved game from file, handles loading screen
	 * @param game Game to be loaded
	 * @return false if the user wanted to returns
	 */
	bool LoadSavedGame(CGame & game);
	/**
	 * Renders the main game screen - map, stack and waves
	 * @param game Game to be rendered
	 */
	void GameScreen(const CGame & game) const;
	/**
	 * Renders menu screen
	 * @param commands Available commands to be printed
	 */
	void Menu(const std::map<char, CCommand> & commands) const;
	/**
	 * Renders game over screen
	 */
	void GameOver() const;
	/**
	 * Renders winning screen
	 */
	void Winner() const;
	/**
	 * Renders help screen with info on individual commands, units and multiple screens that can be switched
	 * @param commands Commands available during game to be printed
	 * @param stack Unit stack to print units informations
	 */
	void HelpScreen(const std::map<char, CCommand> & commands, const CUnitStack & stack) const;
	
	// COMMON
	/**
	 * Signals the user that his input was not valid and waits for pressing space to continue
	 * @param message Message to be printed to user
	 */
	void InvalidInput(const char * message) const;
	/**
	 * Resets the timeout of read - read will wait for input before returning
	 */
	static void ResetTimeout();
	/**
	 * Gets character from user input
	 * @return Character loaded from input
	 */
	static char GetChar();
	
	// TIME
	/**
	 * Gets the current timestamp in milliseconds
	 * @return Timestamp in milliseconds
	 */
	static std::chrono::milliseconds GetCurrentTime();
	/**
	 * Suspends current thread for the duration specified
	 * @param duration Amount of time we want to suspend the thread
	 */
	static void Sleep(std::chrono::milliseconds duration);
	
	
private:
	std::ostream & m_Out;					//!< output stream
	termios m_Term;							//!< original terminal settings
	static const int WINDOW_WIDTH = 150;	//!< width of the window
	static const int WINDOW_HEIGHT = 50;	//!< height of the window
	
	// MENU SCREEN
	static CBuffer CreateMenuOptions(const std::map<char, CCommand> & commands);
	
	// GAME SCREEN
	static CBuffer CreateGameOptions(const std::map<char, CCommand> & commands);
	
	// SAVING GAME
	void SavedScreen() const;
	bool PromptSave(const CGame & game);
	
	// LOADING
	bool TryLoading(const char * filename, CGame & game) const;
	
	// HELP SCREEN
	static CBuffer CreateCommandsHelpScreen(const std::map<char, CCommand> & commands);
	static CBuffer CreateCommonLegendScreen();
	static CBuffer CreateTrooperLegendScreen(const CUnitStack & stack);
	static CBuffer CreateTowerLegendScreen(const CUnitStack & stack);
	static CBuffer CreateHelpOptions();
	
	// HEADERS
	static CBuffer CreateGameHeader();
	static CBuffer CreateTowerHeader();
	static CBuffer CreateTrooperHeader();
	static CBuffer CreateCommonHeader();
	static CBuffer CreateCommandsHeader();
	static CBuffer CreateWinnerHeader(std::string color);
	static CBuffer CreateGameOverHeader(std::string color);
	
	
	// COMMON
	std::string PromptFileName(const std::string & message) const;
	static void NullTimeout();
	void ResetScreen() const;
};


