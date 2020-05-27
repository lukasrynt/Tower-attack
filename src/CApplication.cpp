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
	while (m_Game.Running())
	{
		// get time stamp
		auto start = CInterface::GetCurrentTime();
		ProcessInput();
		m_Game.Update();
		m_Interface.GameScreen(m_Game);
		CInterface::Sleep(start - CInterface::GetCurrentTime() + 16ms);
	}
	EndGame();
}

void CApplication::EndGame()
{
	if (m_Game.Won())
		m_Interface.Winner();
	else if (m_Game.Lost())
		m_Interface.GameOver();
	CInterface::ResetTimeout();
	m_Game = {};
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
	m_MenuCommands.emplace('1',LoadNewCommand());
	m_MenuCommands.emplace('2',LoadSavedCommand());
	m_MenuCommands.emplace('q', QuitCommand());
	
	m_GameCommands.emplace(0, [](){});
	m_GameCommands.emplace('d', DeleteTroopCommand());
	m_GameCommands.emplace('a', AddTroopCommand());
	m_GameCommands.emplace('2', CycleTroopsCommand());
	m_GameCommands.emplace('1', CycleWavesCommand());
	m_GameCommands.emplace('r', ReleaseWavesCommand());
	m_GameCommands.emplace('h', HelpCommand());
	m_GameCommands.emplace('s', SaveCommand());
	m_GameCommands.emplace('q', QuitGameCommand());
}

CCommand CApplication::QuitCommand()
{
	return CCommand{[this](){m_AppOn = false;},
		 "Quit game",
		 Colors::FG_RED};
}

CCommand CApplication::LoadNewCommand()
{
	return CCommand{[this]()
		{
			if (m_Interface.LoadNewGame(m_Game))
				MainLoop();
		},
		"New game",
		Colors::FG_CYAN};
}

CCommand CApplication::LoadSavedCommand()
{
	return CCommand{[this]()
		{
			if (m_Interface.LoadSavedGame(m_Game))
				MainLoop();
		},
		"Load game",
		Colors::FG_GREEN};
}

CCommand CApplication::AddTroopCommand()
{
	return CCommand{[this](){m_Game.AddTroop();},
		 "Add trooper to current wave."};
}

CCommand CApplication::DeleteTroopCommand()
{
	return CCommand{[this](){m_Game.DeleteTroop();},
		 "Delete trooper from current wave"};
}

CCommand CApplication::CycleTroopsCommand()
{
	return CCommand{[this](){m_Game.CycleTroops();},
		 "Move cursor to next trooper."};
}

CCommand CApplication::CycleWavesCommand()
{
	return CCommand{[this](){m_Game.CycleWaves();},
			"Move cursor to next wave"};
}

CCommand CApplication::ReleaseWavesCommand()
{
	return CCommand{[this](){m_Game.ReleaseWaves();},
		 "Release all waves. Careful once this is done no more troops can be released until the map is empty."};
}

CCommand CApplication::HelpCommand()
{
	return CCommand{[this](){m_Interface.HelpScreen(m_GameCommands, m_Game.GetStack());},
		 "Show this screen"};
}

CCommand CApplication::SaveCommand()
{
	return CCommand{[this](){if (!m_Interface.Save(m_Game)) m_Game.Quit();},
		"Save current game"};
}

CCommand CApplication::QuitGameCommand()
{
	return CCommand{[this](){m_Game.Quit();},
			"Quit game"};
}