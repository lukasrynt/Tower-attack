#pragma once

#include <chrono>
#include <termios.h>
#include <iomanip>
#include "CMap.hpp"
#include "Colors.hpp"
#include "CWaves.hpp"

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
	CWaves m_Waves;	//!< Troops waiting in waves and user's selection of troops and waves
	CMap m_Map;		//!< Map with tiles occupied by troops, walls or towers
	termios m_Term;	//!< Terminal settings are stored here
	bool m_WaveOn;	//!< Boolean signalizing start and end of wave
	bool m_GameOn;
	static const int DISPLAY_WIDTH = 150;
	static const int DISPLAY_HEIGHT = 50;
	
	// MAIN LOOP
	void MainLoop();
	static std::chrono::milliseconds GetCurrentTime();
	static void Sleep(std::chrono::milliseconds duration);
	void Update();
	
	// INPUT PROCESSING
	void ProcessInput();
	void StartWave();
	void AddTroopToWave();
	void EndGame();
	
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
	void SavedGameLoadingScreen();
	void LoadSavedGame(const char * filename);
	void NewGameLoadingScreen();
	void LoadNewGame(const char * filename);
	static char LoadSignatureChar(std::istream & in);
	static std::vector<int> LoadSpecifications(std::istream & in);
	static void InvalidInput(const char * message);
	
	// HELP SCREEN
	static void HelpScreen();
	static void RenderHelpScreen();
	static void RenderLegendHeader();
	static void RenderCommonLegend();
	static void RenderTrooperLegend();
	static void RenderTowerLegend();
	
	// SAVE SCREEN
	void SaveScreen();
	void SaveMap();
	std::string PromptFileName();
	static void RenderGameSavedScreen();

	// TERMIOS
	static char GetChar();
	/**
	 * Resets the timeout for read to 0, read can return instantly
	 */
	static void ResetTimeout();
	/**
	 * Exits the program when serious error has occurred
	 * @param [in] s string message with what happened
	 */
	static void Die(const char * s);
};
