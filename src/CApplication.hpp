/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include "Colors.hpp"
#include "CGame.hpp"
#include "CInterface.hpp"

class CApplication
{
public:
	explicit CApplication(const CInterface & interface);
	void Run();
private:
	CInterface m_Interface;
	std::unique_ptr<CGame> m_Game;
	
	// MAIN LOOP
	void MainLoop();
	void EndGame();
	
	// INPUT PROCESSING
	void ProcessInput();
	void GameInput(char ch);
};
