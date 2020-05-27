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
	static const int m_WindowWidth = 150;	//!< width of the window
	static const int m_WindowHeight = 55;	//!< height of the window
	
	// MENU SCREEN
	/**
	 * Draws options in the main menu
	 * @param commands Commands which we want to draw
	 */
	static CBuffer DrawMenuOptions(const std::map<char, CCommand> & commands);
	
	// SAVING GAME
	/**
	 * Screen asking user to return to game after saving it
	 * @return True if user wants to return to game
	 */
	void SavedScreen() const;
	/**
	 * Screen prompting user to save the game
	 * @param game Game to be saved
	 * @return True if saving was successful, false if user wants to return
	 */
	bool PromptSave(const CGame & game);
	
	// LOADING
	/**
	 * Tries to load the game with the given filename
	 * @param filename Filename of the input file
	 * @param game Game to be loaded
	 * @return True if game was successfully loaded
	 */
	bool TryLoading(const char * filename, CGame & game) const;
	
	// HELP SCREEN
	/**
	 * Draws in game options for help screen
	 * @param commands which we want to draw
	 * @return Created buffer
	 */
	static CBuffer DrawGameOptions(const std::map<char, CCommand> & commands);
	/**
	 * Draws help screen with information about command
	 * @param commands which we want to draw
	 * @return Created buffer
	 */
	static CBuffer DrawCommandsHelp(const std::map<char, CCommand> & commands);
	/**
	 * Draws help screen with information about common game objects
	 * @return Created buffer
	 */
	static CBuffer DrawCommonLegend();
	/**
	 * Draws help screen with information about individual troops present in the game
	 * @param stack Stack through which we can access the troops information
	 * @return Created buffer
	 */
	static CBuffer DrawTrooperLegend(const CUnitStack & stack);
	/**
	 * Draws help screen with information about individual towers present in the game
	 * @param stack Stack through which we can access the towers information
	 * @return Created buffer
	 */
	static CBuffer DrawTowerLegend(const CUnitStack & stack);
	/**
	 * Draws options available in all help screens
	 * @return Created buffer
	 */
	static CBuffer DrawHelpOptions();
	
	// HEADERS
	/**
	 * Create game header (INCURSION title)
	 * @return Created buffer
	 */
	static CBuffer DrawGameHeader();
	/**
	 * Create tower legend header (TOWERS)
	 * @return Created buffer
	 */
	static CBuffer DrawTowerHeader();
	/**
	 * Create trooper legend header (TROOPERS)
	 * @return Created buffer
	 */
	static CBuffer DrawTrooperHeader();
	/**
	 * Create common legend header (COMMON)
	 * @return Created buffer
	 */
	static CBuffer DrawCommonHeader();
	/**
	 * Create commands legend header (COMMANDS)
	 * @return Created buffer
	 */
	static CBuffer DrawCommandsHeader();
	/**
	 * Create winner header (WINNER)
	 * @param color Color of the text
	 * @return Created buffer
	 */
	static CBuffer DrawWinnerHeader(std::string color);
	/**
	 * Create game over header (GAME OVER)
	 * @param color Color of the text
	 * @return Created buffer
	 */
	static CBuffer DrawGameOverHeader(std::string color);
	
	
	// COMMON
	/**
	 * Draws the screen in which we prompt the user to enter filename
	 * @param message Message we want to show the user in the prompt screen
	 * @return Filename
	 */
	std::string PromptFileName(const std::string & message) const;
	/**
	 * Changes read attributes so that it will return instantly without waiting for input
	 */
	static void NullTimeout();
	/**
	 * Deletes the screen and moves cursor up
	 */
	void ResetScreen() const;
};


