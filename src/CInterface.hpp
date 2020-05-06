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
	void Render();
	bool Save(const std::unique_ptr<CGame> & game) const;
	bool LoadNewGame(std::unique_ptr<CGame> & game) const;
	bool LoadSavedGame(std::unique_ptr<CGame> & game) const;
	void GameScreen(const std::unique_ptr<CGame> & game);
	void Menu(const std::map<char, CCommand> & commands);
	void GameOver();
	void Winner();
	void HelpScreen(const std::map<char, CCommand> & commands);
	
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
	CBuffer m_Buffer;
	
	// END GAME
	void AppendWinner();
	void AppendGameOver();
	
	// MENU SCREEN
	void AppendHeader();
	void AppendMenuOptions(const std::map<char, CCommand> & commands);
	
	// GAME SCREEN
	void AppendGameOptions(const std::map<char, CCommand> & commands);
	
	// SAVING GAME
	void SavedScreen() const;
	bool PromptSave(const std::unique_ptr<CGame> & game) const;
	
	// HELP SCREEN
	void RenderHelpScreen();
	void RenderLegendHeader();
	void RenderCommonLegend() const;
	void RenderTrooperLegend() const;
	void RenderTowerLegend() const;
	
	// COMMON
	std::string PromptFileName(const std::string & message) const;
	static void NullTimeout();
	void ChangeWindowSize(int width, int height) const;
	void HideCursor() const;
	void ShowCursor() const;
	void ResetScreen() const;
};


