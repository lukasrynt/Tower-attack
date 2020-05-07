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
	explicit CInterface(std::ostream & out);
	
	~CInterface() noexcept(false);
	
	// SCREENS
	void Render(CBuffer buffer) const;
	bool Save(const CGame & game);
	bool LoadNewGame(CGame & game);
	bool LoadSavedGame(CGame & game);
	void GameScreen(const CGame & game) const;
	void Menu(const std::map<char, CCommand> & commands) const;
	void GameOver() const;
	void Winner() const;
	void HelpScreen(const std::map<char, CCommand> & commands, const CUnitStack & stack) const;
	
	// COMMON
	void InvalidInput(const char * message) const;
	static void ResetTimeout();
	static char GetChar();
	
	// TIME
	static std::chrono::milliseconds GetCurrentTime();
	static void Sleep(std::chrono::milliseconds duration);
	
	
private:
	std::ostream & m_Out;
	termios m_Term;
	static const int WINDOW_WIDTH = 150;
	static const int WINDOW_HEIGHT = 50;
	
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
	static CBuffer CreateLegendScreen(const CUnitStack & stack);
	static CBuffer CreateCommandsHelpScreen(const std::map<char, CCommand> & commands);
	static CBuffer CreateCommonLegend();
	static CBuffer CreateTrooperLegend(const CUnitStack & stack);
	static CBuffer CreateTowerLegend(const CUnitStack & stack);
	static CBuffer CreateHelpOptions();
	
	// HEADERS
	static CBuffer CreateGameHeader();
	static CBuffer CreateLegendHeader();
	static CBuffer CreateTowerHeader();
	static CBuffer CreateTrooperHeader();
	static CBuffer CreateCommonHeader();
	static CBuffer CreateCommandsHeader();
	static CBuffer CreateWinnerHeader(std::string color);
	static CBuffer CreateGameOverHeader(std::string color);
	
	
	// COMMON
	std::string PromptFileName(const std::string & message) const;
	static void NullTimeout();
	void ChangeWindowSize(int width, int height) const;
	void ResetScreen() const;
};


