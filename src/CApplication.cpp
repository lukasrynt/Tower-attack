/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CApplication.hpp"

#include <memory>
#include "ExInvalidInput.hpp"
#include "CCommand.hpp"


using namespace std;
/**********************************************************************************************************************/
// TERMIOS
CApplication::CApplication(const CInterface & interface) noexcept
	: m_Interface(interface),
	  m_Game(make_unique<CGame>()),
	  m_AppOn(true)
{
	InitCommands();
}

/**********************************************************************************************************************/
// MAIN LOOP
void CApplication::Run()
{
	while (m_AppOn)
	{
		m_Interface.Menu(m_MenuCommands);
		char input = CInterface::GetChar();
		if (m_MenuCommands.count(input))
			m_MenuCommands.at(input).Execute();
		else
			m_Interface.InvalidInput("Invalid input.");
	}
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
		m_Interface.GameScreen(*m_Game);
		CInterface::Sleep(start - CInterface::GetCurrentTime() + 30ms);
	}
	EndGame();
}

void CApplication::EndGame()
{
	if (m_Game->Won())
		m_Interface.Winner();
	else if (m_Game->Lost())
		m_Interface.GameOver();
	CInterface::ResetTimeout();
	m_Game = nullptr;
}

/**********************************************************************************************************************/
// INPUT PROCESSING

void CApplication::ProcessInput()
{
	char input = CInterface::GetChar();
	try {
		auto it = m_GameCommands.find(input);
		if (it == m_GameCommands.end())
			throw invalid_input("Unknown command, press 'h' for more info.");
		it->second.Execute();
	} catch (invalid_input & e) {
		m_Interface.InvalidInput(e.what());
	}
}

void CApplication::InitCommands()
{
	m_MenuCommands.emplace('1',LoadNew());
	m_MenuCommands.emplace('2',LoadSaved());
	m_MenuCommands.emplace('q', Quit());
	
	m_GameCommands.emplace(0, [](){});
	m_GameCommands.emplace('d', DeleteTroop());
	m_GameCommands.emplace('a', AddTroop());
	m_GameCommands.emplace('2', CycleTroops());
	m_GameCommands.emplace('1', CycleWaves());
	m_GameCommands.emplace('r', ReleaseWaves());
	m_GameCommands.emplace('h', Help());
	m_GameCommands.emplace('s', Save());
	m_GameCommands.emplace('q', QuitGame());
}

CCommand CApplication::Quit()
{
	return {[this](){m_AppOn = false;},
		 "Quit game",
		 Colors::FG_RED};
}

CCommand CApplication::LoadNew()
{
	return {[this]()
		{
			m_Game = make_unique<CGame>();
			if (m_Interface.LoadNewGame(*m_Game))
				MainLoop();
		},
		"New game",
		Colors::FG_CYAN};
}

CCommand CApplication::LoadSaved()
{
	return {[this]()
		{
			m_Game = make_unique<CGame>();
			if (m_Interface.LoadSavedGame(*m_Game))
				MainLoop();
		},
		"Load game",
		Colors::FG_GREEN};
}

CCommand CApplication::AddTroop()
{
	return {[this](){m_Game->AddTroop();},
		 "Add troop",
		 Colors::FG_RED};
}

CCommand CApplication::DeleteTroop()
{
	return {[this](){m_Game->DeleteTroop();},
		 "Delete troop",
		 Colors::FG_YELLOW};
}

CCommand CApplication::CycleTroops()
{
	return {[this](){m_Game->CycleTroops();},
		 "Cycle waves",
		 Colors::FG_GREEN};
}

CCommand CApplication::CycleWaves()
{
	return {[this](){m_Game->CycleWaves();},
			"Cycle troops",
			Colors::FG_CYAN};
}

CCommand CApplication::ReleaseWaves()
{
	return {[this](){m_Game->ReleaseWaves();},
		 "Release waves",
		 Colors::FG_MAGENTA};
}

CCommand CApplication::Help()
{
	return {[this](){m_Interface.HelpScreen(m_GameCommands);},
		 "Help screen",
		 Colors::FG_BLACK};
}

CCommand CApplication::Save()
{
	return {[this](){m_Interface.Save(*m_Game);},
		"Save game",
		Colors::FG_BLUE};
}

CCommand CApplication::QuitGame()
{
	return {[this](){m_Game->Quit();},
			"Quit game",
			Colors::FG_RED};
}