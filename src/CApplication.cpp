/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CApplication.hpp"

#include <memory>
#include "ExInvalidInput.hpp"


using namespace std;
/**********************************************************************************************************************/
// TERMIOS
CApplication::CApplication(const CInterface & interface)
	: m_Interface(interface),
	  m_Game(make_unique<CGame>())
{}

/**********************************************************************************************************************/
// MAIN LOOP
void CApplication::Run()
{
	bool end = false;
	do
	{
		m_Interface.Menu();
		
		// validate input
		switch (CInterface::GetInput())
		{
			case '1':
				if (m_Interface.LoadNewGame(m_Game))
					MainLoop();
				break;
			case '2':
				if (m_Interface.LoadSavedGame(m_Game))
					MainLoop();
				break;
			case 'q':
				end = true;
				break;
			default:
				m_Interface.InvalidInput("Invalid input.");
				break;
		}
		
	} while (!end);
}

void CApplication::MainLoop()
{
	//	http://gameprogrammingpatterns.com/game-loop.html
	while (m_Game->Running())
	{
		// get time stamp
		auto start = CInterface::GetCurrentTime();
		ProcessInput();
		m_Game->Update();
		m_Interface.GameScreen(m_Game);
		CInterface::Sleep(start - CInterface::GetCurrentTime() + 30ms);
	}
	EndGame();
}

void CApplication::EndGame() const
{
	if (m_Game->Won())
		m_Interface.Winner();
	else if (m_Game->Lost())
		m_Interface.GameOver();
	CInterface::ResetTimeout();
}

/**********************************************************************************************************************/
// INPUT PROCESSING
void CApplication::ProcessInput()
{
	char ch = CInterface::GetInput();
	switch (ch)
	{
		case 0:
			break;
		case 'h':
			m_Interface.HelpScreen();
			break;
		case 's':
			m_Interface.Save(m_Game);
			break;
		default:
			GameInput(ch);
			break;
	}
}

void CApplication::GameInput(char ch)
{
	try
	{
		m_Game->ProcessInput(ch);
	}
	catch (invalid_input & e)
	{
		m_Interface.InvalidInput(e.what());
	}
}