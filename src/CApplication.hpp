/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <chrono>
#include <termios.h>
#include <iomanip>
#include <iomanip>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <sstream>

#include "ExInvalidFile.hpp"
#include "Colors.hpp"
#include "CGame.hpp"

class CApplication
{
public:
	/**
	 * Initializes terminal attributes and saves the original settings to m_Term struct
	 */
	explicit CApplication();
	/**
	 * Restores terminal back to normal settings
	 */
	~CApplication();
	CApplication(const CApplication & src) = delete;
	CApplication & operator=(const CApplication & src) = delete;
	
	void Run();
private:
	CGame m_Game;
	termios m_Term;	//!< Terminal settings are stored here
	static const int DISPLAY_WIDTH = 150;
	static const int DISPLAY_HEIGHT = 50;
	
	// MAIN LOOP
	void MainLoop();
	static std::chrono::milliseconds GetCurrentTime();
	static void Sleep(std::chrono::milliseconds duration);
	
	// INPUT PROCESSING
	void ProcessInput();
	void GameInput(char ch);
	
	// GAME SCREEN RENDERING
	void RenderGameScreen() const;
	static void RenderGameOptions();
	static void ResetScreen();
	
	// MENU SCREEN
	void MenuScreen();
	static void RenderHeader();
	static void PrintCenteredLine(const std::string & line);
	static void RenderMenuScreen();
	
	// LOADING
	bool LoadNewGame();
	bool LoadSavedGame();
	static void InvalidInput(const char * message);
	
	// HELP SCREEN
	static void HelpScreen();
	static void RenderHelpScreen();
	static void RenderLegendHeader();
	static void RenderCommonLegend();
	static void RenderTrooperLegend();
	static void RenderTowerLegend();
	
	// SAVING
	void SaveScreen();
	bool SaveGame();
	std::string PromptFileName(const std::string & message);
	static void RenderGameSavedScreen();

	// TERMIOS
	static char GetChar();
	/**
	 * Resets the timeout for read to 0, read can return instantly
	 */
	static void ResetTimeout();
	static void NullTimeout();
	/**
	 * Exits the program when serious error has occurred
	 * @param [in] s string message with what happened
	 */
	static void Die(const char * s);
};
