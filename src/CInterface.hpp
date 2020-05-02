/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 02/05/2020.
*/

#pragma once

#include <iostream>
#include <termios.h>
#include <chrono>
#include "CGame.hpp"

class CInterface
{
public:
	explicit CInterface(std::ostream & out);
	~CInterface() noexcept(false);
	
	// SCREENS
	bool Save(const std::unique_ptr<CGame> & game) const;
	bool LoadNewGame(std::unique_ptr<CGame> & game) const;
	bool LoadSavedGame(std::unique_ptr<CGame> & game) const;
	void GameScreen(const std::unique_ptr<CGame> & game) const;
	void Menu() const;
	void GameOver() const;
	void Winner() const;
	void HelpScreen() const;
	
	// COMMON
	void InvalidInput(const char * message) const;
	static void ResetTimeout();
	static char GetInput();
	
	// TIME
	static std::chrono::milliseconds GetCurrentTime();
	static void Sleep(std::chrono::milliseconds duration);
	
	
private:
	std::ostream & m_Out;
	termios m_Term;
	static const int DISPLAY_WIDTH = 150;
	static const int DISPLAY_HEIGHT = 50;
	
	// END GAME
	void RenderWinner() const;
	void RenderGameOver() const;
	
	// MENU SCREEN
	void RenderHeader() const;
	void RenderMenuOptions() const;
	
	// GAME SCREEN
	void RenderGameOptions() const;
	
	// SAVING GAME
	void SavedScreen() const;
	bool PromptSave(const std::unique_ptr<CGame> & game) const;
	
	// HELP SCREEN
	void RenderHelpScreen() const;
	void RenderLegendHeader() const;
	void RenderCommonLegend() const;
	void RenderTrooperLegend() const;
	void RenderTowerLegend() const;
	
	// COMMON
	std::string PromptFileName(const std::string & message) const;
	void PrintCenteredLine(const std::string & line) const;
	static void NullTimeout();
	void ChangeWindowSize(int width, int height) const;
	void HideCursor() const;
	void ShowCursor() const;
	void ResetScreen() const;
};


