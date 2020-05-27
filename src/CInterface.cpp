/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 02/05/2020.
*/

#include <iomanip>
#include <zconf.h>
#include <thread>
#include <functional>
#include "CInterface.hpp"
#include "escape_sequences.h"
#include "ExInvalidInput.hpp"
#include "ExInvalidFormat.hpp"

using namespace std;
CInterface::CInterface(ostream & out) noexcept(false)
	: m_Out(out),
	  m_Term({})
{
	// https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
	// get attributes to termios struct
	if (tcgetattr(STDIN_FILENO, &m_Term) == -1)
		throw ios::failure("Error during tcgetattr");
	m_Out.exceptions(ios::failbit | ios::badbit | ios::eofbit);
	termios raw = m_Term;
	/*
	c_lflag stands for "local flags":
	ECHO is bitflag which we flip binary and use AND for it - all other bits remain the same just ECHO changes in the local flags
		the standard behavior of echo - creating one new line
	ICANON is a flag that allows us to turn off canonical mode
	IEXTEN disables Ctrl-V
	ISIG is a flag that allows us to disable:
		CTRL-C (SIGINT signal causing termination of a process)
		CTRL-Z (SIGTSPT signal causing suspending a process)
	*/
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);	// various flags
	raw.c_cflag |= (CS8);
	raw.c_cc[VMIN] = 1;									// minimimum number of bytes that read needs before it can return
	raw.c_cc[VTIME] = 0;								// time before read can return
	
	// set the attributes to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		throw ios::failure("Error during tcgetattr");
	
	m_Out << Escapes::HIDE_CURSOR
		<< Escapes::CHANGE_WINDOW_SIZE(m_WindowHeight, m_WindowWidth);
}

CInterface::~CInterface() noexcept(false)
{
	// set the attributes back
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_Term) == -1)
		throw ios::failure("Error during tcsetattr");
	
	m_Out << Escapes::SHOW_CURSOR
		<< Escapes::RESET_SCREEN
		<< Escapes::RESET_CURSOR;
}

/**********************************************************************************************************************/
// LOAD
bool CInterface::LoadNewGame(CGame & game)
{
	while (true)
	{
		// get the filename from user;
		string response = PromptFileName("Enter the name of the map that you want to load.\n"
										 "The program will only search for maps in \"maps\" "
										 "folder so please keep that in mind.\n"
										 "Type 'RETURN' to go back to menu screen");
		
		// return to menu if requested
		if (response == "RETURN")
		{
			(m_Out << Colors::RESET).flush();
			return false;
		}
		
		// check input stream
		string filename = "examples/maps/" + response + ".map";
		if (!TryLoading(filename.c_str(), game))
			continue;
		
		if (!(game.CheckNew()))
		{
			InvalidInput("Incorrect file format.");
			continue;
		}
		(m_Out << Colors::RESET).flush();
		NullTimeout();
		return true;
	}
}

bool CInterface::LoadSavedGame(CGame & game)
{
	while (true)
	{
		// get the filename from user;
		string response = PromptFileName("Enter the name of saved game you wish to load.\n"
										 "The program will only search for maps in \"saved\" "
										 "folder so please keep that in mind.\n"
										 "Type 'RETURN' to go back to menu screen");
		// return to menu if requested
		if (response == "RETURN")
		{
			(m_Out << Colors::RESET).flush();
			return false;
		}
		
		// check input stream
		string filename = "examples/saves/" + response + ".sav";
		if (!TryLoading(filename.c_str(), game))
			continue;
		
		if (!game.CheckSaved())
		{
			InvalidInput("Incorrect file format.");
			continue;
		}
		(m_Out << Colors::RESET).flush();
		NullTimeout();
		return true;
	}
}

bool CInterface::TryLoading(const char * filename, CGame & game) const
{
	ifstream inFile(filename);
	if (!inFile)
	{
		(m_Out << Colors::RESET).flush();
		InvalidInput("File not found.");
		return false;
	}
	
	inFile.exceptions(ios::failbit | ios::badbit);
	// load game
	try
	{
		inFile >> game;
		return true;
	}
	catch (const ios::failure &)
	{
		if (inFile.eof())
			return true;
		InvalidInput("Incorrect file format.");
	}
	catch (const invalid_format & e)
	{
		InvalidInput(e.what());
	}
	return false;
}

/**********************************************************************************************************************/
// SAVE
bool CInterface::Save(const CGame & game)
{
	if (!PromptSave(game))
		return true;
	SavedScreen();
	char ch;
	while (true)
	{
		ch = GetChar();
		if (ch == 'q')
			return false;
		else if (ch == 'r')
			return true;
	}
}

bool CInterface::PromptSave(const CGame & game)
{
	while (true)
	{
		// load filename name and check if it exists
		string response = PromptFileName("Enter the name of your save file. Press enter to validate the name.\n"
										 "The file will be automatically appended with .sav\n"
										 "Type 'RETURN' to go back to game");
		
		// return to game if requested
		if (response == "RETURN")
		{
			m_Out << Colors::RESET;
			return false;
		}
		
		// check output stream
		string filename = "examples/saves/" + response + ".sav";
		ofstream outFile(filename);
		if (!outFile)
		{
			(m_Out << Colors::RESET).flush();
			InvalidInput("File not found.");
			return false;
		}
		outFile.exceptions(ios::failbit | ios::badbit | ios::eofbit);
		
		// save game
		try
		{
			outFile << game;
		}
		catch (const ios::failure &)
		{
			m_Out << Colors::RESET;
			InvalidInput("Error during writing to file.");
			continue;
		}
		m_Out << Colors::RESET;
		return true;
	}
}

void CInterface::SavedScreen() const
{
	ResetScreen();
	m_Out << Colors::FG_WHITE << "Game saved. Do you wish to continue?" << Colors::RESET << endl << endl;
	m_Out << Colors::FG_RED << "		q - to quit					" << Colors::RESET << endl;
	m_Out << Colors::FG_BLUE << "		r - to return to game       " << Colors::RESET << endl;
}

/**********************************************************************************************************************/
// MENU SCREEN
void CInterface::Menu(const map<char, CCommand> & commands) const
{
	ResetScreen();
	m_Out << CBuffer{m_WindowWidth}
		.Append(DrawGameHeader())
		.Append(DrawMenuOptions(commands))
		.CenterHorizontal(m_WindowHeight);
}

CBuffer CInterface::DrawMenuOptions(const map<char, CCommand> & commands)
{
	CBuffer tmp{m_WindowWidth};
	tmp.Append()
			.Append();
	for (const auto & command : commands)
	{
		tmp.Append(command.first + " - "s + command.second.Help(), command.second.Color())
				.Append();
	}
	tmp.CenterVertical();
	return tmp;
}

/**********************************************************************************************************************/
// GAME END
void CInterface::Winner() const
{
	bool col = false;
	while (true)
	{
		ResetScreen();
		m_Out << CBuffer{m_WindowWidth}
			.Append(DrawWinnerHeader(col ? Colors::FG_GREEN : Colors::FG_BLUE))
			.CenterHorizontal(m_WindowHeight);
		col = !col;
		if (GetChar() == 'q')
			break;
		Sleep(60ms);
	}
}

void CInterface::GameOver() const
{
	bool col = false;
	while (true)
	{
		ResetScreen();
		m_Out << CBuffer{m_WindowWidth}
			.Append(DrawGameOverHeader(col ? Colors::FG_RED : Colors::FG_BLACK))
			.CenterHorizontal(m_WindowHeight);
		col = !col;
		if (GetChar() == 'q')
			break;
		Sleep(60ms);
	}
}

/**********************************************************************************************************************/
// GAME SCREEN
void CInterface::GameScreen(const CGame & game) const
{
	ResetScreen();
	CBuffer buffer{m_WindowWidth};
	buffer.Append(DrawGameHeader())
		.Append(game.Draw(m_WindowWidth));
	if (buffer.Height() > m_WindowHeight)
		(m_Out << Escapes::CHANGE_WINDOW_SIZE(buffer.Height(), m_WindowWidth)).flush();
	m_Out << buffer;
}

/**********************************************************************************************************************/
// HELP SCREEN
void CInterface::HelpScreen(const map<char, CCommand> & commands, const CUnitStack & stack) const
{
	vector<CBuffer> screens;
	screens.emplace_back(DrawCommandsHelp(commands));
	screens.emplace_back(DrawCommonLegend());
	screens.emplace_back(DrawTrooperLegend(stack));
	screens.emplace_back(DrawTowerLegend(stack));
	cout << screens.size();
	
	size_t idx = 0;
	ResetScreen();
	m_Out << screens[idx];
	char ch = 0;
	while (ch != 'r')
	{
		ch = GetChar();
		if (ch == 'n')
		{
			if (++idx == screens.size())
				idx = 0;
			ResetScreen();
			m_Out << screens[idx];
		}
	}
}

CBuffer CInterface::DrawCommandsHelp(const std::map<char, CCommand> & commands)
{
	return move(CBuffer{m_WindowWidth}
		.Append(DrawCommandsHeader())
		.Append(DrawGameOptions(commands))
		.Append(DrawHelpOptions()))
		.CenterHorizontal(m_WindowHeight);
}

CBuffer CInterface::DrawGameOptions(const map<char, CCommand> & commands)
{
	CBuffer tmp{m_WindowWidth};
	for (const auto & command : commands)
		if (!command.second.Help().empty())
			tmp.Append("("s + command.first + ")", string(Colors::BG_MAGENTA) + Colors::FG_BLACK)
					.Append("\t"s + command.second.Help(), Colors::FG_MAGENTA)
					.Append();
	return tmp;
}

CBuffer CInterface::DrawCommonLegend()
{
	return move(DrawCommonHeader().CenterVertical()
		.Append()
		.Append("(#) Wall", string(Colors::BG_WHITE) + Colors::FG_BLACK)
		.Append("● Obstacle through which neither troops, nor bullets can pass.", Colors::FG_WHITE)
		.Append()
		.Append("(#) Wall", string(Colors::BG_CYAN) + Colors::FG_BLACK)
		.Append(" ● Point from which troopers will be spawned.", Colors::FG_CYAN)
		.Append(" ● Numbered from 1 to 5 - each point point is tied to one wave in order.", Colors::FG_CYAN)
		.Append()
		.Append("(O) Gate", string(Colors::BG_MAGENTA) + Colors::FG_BLACK)
		.Append(" ● Gate to which the troopers need to get.", Colors::FG_MAGENTA)
		.Append(" ● Health points of the gate are displayed above the map.", Colors::FG_MAGENTA)
		.Append(" ● The goal of the player is to destroy the gate.", Colors::FG_MAGENTA)
		.Append()
		.Append(DrawHelpOptions()))
		.CenterHorizontal(m_WindowHeight);
}

CBuffer CInterface::DrawTrooperLegend(const CUnitStack & stack)
{
	return move(DrawTrooperHeader().CenterVertical()
		.Append(stack.DrawTroopsInfo(m_WindowWidth))
		.Append(DrawHelpOptions()))
		.CenterHorizontal(m_WindowHeight);
}

CBuffer CInterface::DrawTowerLegend(const CUnitStack & stack)
{
	return move(DrawTowerHeader().CenterVertical()
		.Append(stack.DrawTowersInfo(m_WindowWidth))
		.Append(DrawHelpOptions())
		.CenterHorizontal(m_WindowHeight));
}

CBuffer CInterface::DrawHelpOptions()
{
	return move(CBuffer{m_WindowWidth}
		.Append()
		.Append("r - to return to game", Colors::FG_BLUE)
		.Append("n - move to next screen", Colors::FG_GREEN));
}

/**********************************************************************************************************************/
// HEADERS

CBuffer CInterface::DrawGameHeader()
{
	// http://patorjk.com/software/taag/#p=display&f=Graffiti&t=Type%20Something%20
	CBuffer tmp{m_WindowWidth};
	tmp.AddEscapeSequence(Colors::FG_YELLOW)
			.Append(R"(_________ _        _______           _______  _______ _________ _______  _       )")
			.Append(R"(\__   __/( (    /|(  ____ \|\     /|(  ____ )(  ____ \\__   __/(  ___  )( (    /|)")
			.Append(R"(   ) (   |  \  ( || (    \/| )   ( || (    )|| (    \/   ) (   | (   ) ||  \  ( |)")
			.Append(R"(   | |   |   \ | || |      | |   | || (____)|| (_____    | |   | |   | ||   \ | |)")
			.Append(R"(   | |   | (\ \) || |      | |   | ||     __)(_____  )   | |   | |   | || (\ \) |)")
			.Append(R"(   | |   | | \   || |      | |   | || (\ (         ) |   | |   | |   | || | \   |)")
			.Append(R"(___) (___| )  \  || (____/\| (___) || ) \ \__/\____) |___) (___| (___) || )  \  |)")
			.Append(R"(\_______/|/    )_)(_______/(_______)|/   \__/\_______)\_______/(_______)|/    )_))")
			.CenterVertical()
			.AddEscapeSequence(Colors::RESET)
			.Append()
			.Append()
			.Append();
	return tmp;
}

CBuffer CInterface::DrawCommonHeader()
{
	return move(CBuffer{m_WindowWidth}
						.AddLines(R"(  __ ___ _ __  _ __  ___ _ _
 / _/ _ \ '  \| '  \/ _ \ ' \
 \__\___/_|_|_|_|_|_\___/_||_|
                              )", Colors::FG_WHITE));
}

CBuffer CInterface::DrawTowerHeader()
{
	return move(CBuffer{m_WindowWidth}
						.AddLines(R"(  _
 | |_ _____ __ _____ _ _ ___
 |  _/ _ \ V  V / -_) '_(_-<
  \__\___/\_/\_/\___|_| /__/
                            )", Colors::FG_RED));
}

CBuffer CInterface::DrawTrooperHeader()
{
	return move(CBuffer{m_WindowWidth}
						.AddLines(R"(  _
 | |_ _ _ ___  ___ _ __  ___ _ _ ___
 |  _| '_/ _ \/ _ \ '_ \/ -_) '_(_-<
  \__|_| \___/\___/ .__/\___|_| /__/
                  |_|               )", Colors::FG_YELLOW));
}

CBuffer CInterface::DrawCommandsHeader()
{
	return move(CBuffer{m_WindowWidth}
						.AddLines(R"(   ___                              _
  / __|___ _ __  _ __  __ _ _ _  __| |___
 | (__/ _ \ '  \| '  \/ _` | ' \/ _` (_-<
  \___\___/_|_|_|_|_|_\__,_|_||_\__,_/__/
                                         )", Colors::FG_MAGENTA).CenterVertical());
}

CBuffer CInterface::DrawWinnerHeader(string color)
{
	CBuffer tmp{m_WindowWidth};
	tmp.AddEscapeSequence(move(color))
			.Append(R"(         _________ _        _        _______  _______ )")
			.Append(R"(|\     /|\__   __/( (    /|( (    /|(  ____ \(  ____ ))")
			.Append(R"(| )   ( |   ) (   |  \  ( ||  \  ( || (    \/| (    )|)")
			.Append(R"(| | _ | |   | |   |   \ | ||   \ | || (__    | (____)|)")
			.Append(R"(| |( )| |   | |   | (\ \) || (\ \) ||  __)   |     __))")
			.Append(R"(| || || |   | |   | | \   || | \   || (      | (\ (   )")
			.Append(R"(| () () |___) (___| )  \  || )  \  || (____/\| ) \ \__)")
			.Append(R"((_______)\_______/|/    )_)|/    )_)(_______/|/   \__/)")
			.CenterVertical()
			.AddEscapeSequence(Colors::RESET);
	return tmp;
}

CBuffer CInterface::DrawGameOverHeader(string color)
{
	CBuffer tmp{m_WindowWidth};
	tmp.AddEscapeSequence(move(color))
			.Append(R"( _______  _______  _______  _______    _______           _______  _______ )")
			.Append(R"((  ____ \(  ___  )(       )(  ____ \  (  ___  )|\     /|(  ____ \(  ____ ))")
			.Append(R"(| (    \/| (   ) || () () || (    \/  | (   ) || )   ( || (    \/| (    )|)")
			.Append(R"(| |      | (___) || || || || (__      | |   | || |   | || (__    | (____)|)")
			.Append(R"(| | ____ |  ___  || |(_)| ||  __)     | |   | |( (   ) )|  __)   |     __))")
			.Append(R"(| | \_  )| (   ) || |   | || (        | |   | | \ \_/ / | (      | (\ (   )")
			.Append(R"(| (___) || )   ( || )   ( || (____/\  | (___) |  \   /  | (____/\| ) \ \__)")
			.Append(R"((_______)|/     \||/     \|(_______/  (_______)   \_/   (_______/|/   \__/)")
			.CenterVertical()
			.AddEscapeSequence(Colors::RESET);
	return tmp;
}

/**********************************************************************************************************************/
// TIME
chrono::milliseconds CInterface::GetCurrentTime()
{
	using namespace chrono;
	return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
}

void CInterface::Sleep(chrono::milliseconds duration)
{
	// https://en.cppreference.com/w/cpp/chrono/duration
	this_thread::sleep_for(duration);
}

/**********************************************************************************************************************/
// UNIVERSAL
void CInterface::ResetTimeout()
{
	// get the attributes
	termios tmp = {};
	if (tcgetattr(STDIN_FILENO, &tmp) == -1)
		throw ios::failure("Error during tcgetattr");
	
	// set timeout
	tmp.c_cc[VMIN] = 1;									// minimimum number of bytes that read needs before it can return
	tmp.c_cc[VTIME] = 0;								// time before read can return
	
	// set the attributes back to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp) == -1)
		throw ios::failure("Error during tcsetattr");
}

void CInterface::NullTimeout()
{
	// get the attributes
	termios tmp = {};
	if (tcgetattr(STDIN_FILENO, & tmp) == -1)
		throw ios::failure("Error during tcgetattr");
	
	// set timeout
	tmp.c_cc[VMIN] = 0;									// minimimum number of bytes that read needs before it can return
	tmp.c_cc[VTIME] = 1;								// time before read can return
	
	// set the attributes back to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, & tmp) == -1)
		throw ios::failure("Error during tcsetattr");
}

char CInterface::GetChar()
{
	char ch = 0;
	
	// read one byte from input
	if (read(STDIN_FILENO, &ch, sizeof(ch)) == EOF)	// needs to be C style because streams don't respond to timeout
		throw ios::failure("Error during read");
	
	return ch;
}

void CInterface::InvalidInput(const char * message) const
{
	(m_Out << Colors::RESET).flush();
	m_Out << CBuffer{m_WindowWidth}
			.Append(string(message) + " Press space to continue.", string(Colors::BG_RED) + Colors::FG_BLACK)
			.CenterVertical();
	char ch = 0;
	while ((ch != ' '))
		ch = GetChar();
}

string CInterface::PromptFileName(const string & message) const
{
	// disable raw mode for a while
	termios orig = {};
	if (tcgetattr(STDIN_FILENO, &orig) == -1)
		throw ios::failure("Error during tcgetattr");
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_Term) == -1)
		throw ios::failure("Error during tcsetattr");
	
	// render message
	CBuffer tmp{m_WindowWidth};
	tmp.AddEscapeSequence(Colors::FG_WHITE)
			.AddLines(message)
			.AddEscapeSequence(Colors::RESET)
			.Append(string(20, ' '), Colors::BG_WHITE)
			.CenterVertical();
	size_t height = m_WindowHeight / 2 + tmp.Height() / 2;
	size_t width = m_WindowWidth / 2 - tmp.Width() / 2;
	ResetScreen();
	m_Out << tmp.CenterHorizontal(m_WindowHeight);
	
	// reposition cursor and set color to white
	(m_Out << Escapes::SHOW_CURSOR
		<< Escapes::REPOSITION_CURSOR(height % 2 == 0 ? height : height + 1, width)
		<< string(Colors::BG_WHITE) + Colors::FG_BLACK).flush();
	
	string filename;
	char ch;
	while ((ch = GetChar()) != '\n')
		filename += ch;
	(m_Out << Escapes::HIDE_CURSOR).flush();
	
	// enable raw mode back
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig) == -1)
		throw ios::failure("Error during tcsetattr");
	return filename;
}

void CInterface::ResetScreen() const
{
	// clear the screen and move cursor back to top
	(m_Out << Escapes::RESET_SCREEN
		<< Escapes::RESET_CURSOR).flush();
}


