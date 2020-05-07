/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 02/05/2020.
*/

#include <iomanip>
#include <zconf.h>
#include <thread>
#include "CInterface.hpp"
#include "escape_sequences.h"
#include "ExInvalidInput.hpp"

using namespace std;
CInterface::CInterface(ostream & out)
	: m_Out(out),
	  m_Term({})
{
	// https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
	// get attributes to termios struct
	if (tcgetattr(STDIN_FILENO, &m_Term) == -1)
		throw ios::failure("Error during tcgetattr");
	out.exceptions(ios::failbit | ios::badbit | ios::eofbit);	//TODO catch those exceptions somewhere
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
	
	m_Out << Escapes::HIDE_CURSOR;
	ChangeWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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

void CInterface::Render(CBuffer buffer) const
{
	ResetScreen();
	
	// align horizontally
	CBuffer tmp{WINDOW_WIDTH};
	if (buffer.Height() < WINDOW_HEIGHT)
		for (int i = 0; i < (WINDOW_HEIGHT - buffer.Height()) / 2; ++i)
			tmp.AddLine();
	
	// render buffer
	tmp.Append(move(buffer));
	m_Out << tmp;
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
		string filename = "maps/" + response + ".map";
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
		string filename = "saves/" + response + ".sav";
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
	
	inFile.exceptions(ios::failbit | ios::eofbit | ios::badbit);
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
		return false;
	}
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
		string filename = "saves/" + response + ".sav";
		ofstream outFile(filename);
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
	Render(move(CBuffer{WINDOW_WIDTH}
		.Append(CreateGameHeader())
		.Append(CreateMenuOptions(commands))));
}

CBuffer CInterface::CreateMenuOptions(const map<char, CCommand> & commands)
{
	CBuffer tmp{WINDOW_WIDTH};
	tmp.AddLine()
		.AddLine();
	for (const auto & command : commands)
	{
		tmp.AddLine(command.first + " - "s + command.second.Help(), command.second.Color())
			.AddLine();
	}
	tmp.Center();
	return tmp;
}

CBuffer CInterface::CreateGameHeader()
{
	// http://patorjk.com/software/taag/#p=display&f=Graffiti&t=Type%20Something%20
	CBuffer tmp{WINDOW_WIDTH};
	tmp.AddEscapeSequence(Colors::FG_YELLOW)
			.AddLine(R"(_________ _        _______           _______  _______ _________ _______  _       )")
			.AddLine(R"(\__   __/( (    /|(  ____ \|\     /|(  ____ )(  ____ \\__   __/(  ___  )( (    /|)")
			.AddLine(R"(   ) (   |  \  ( || (    \/| )   ( || (    )|| (    \/   ) (   | (   ) ||  \  ( |)")
			.AddLine(R"(   | |   |   \ | || |      | |   | || (____)|| (_____    | |   | |   | ||   \ | |)")
			.AddLine(R"(   | |   | (\ \) || |      | |   | ||     __)(_____  )   | |   | |   | || (\ \) |)")
			.AddLine(R"(   | |   | | \   || |      | |   | || (\ (         ) |   | |   | |   | || | \   |)")
			.AddLine(R"(___) (___| )  \  || (____/\| (___) || ) \ \__/\____) |___) (___| (___) || )  \  |)")
			.AddLine(R"(\_______/|/    )_)(_______/(_______)|/   \__/\_______)\_______/(_______)|/    )_))")
			.Center()
			.AddEscapeSequence(Colors::RESET)
			.AddLine()
			.AddLine()
			.AddLine();
	return tmp;
}

/**********************************************************************************************************************/
// GAME END
void CInterface::Winner() const
{
	bool col = false;
	while (true)
	{
		Render(move(CBuffer{WINDOW_WIDTH}
			.Append(CreateWinnerHeader(col ? Colors::FG_GREEN : Colors::FG_BLUE))));
		col = !col;
		if (GetChar() == 'q')
			break;
		Sleep(60ms);
	}
}

CBuffer CInterface::CreateWinnerHeader(string color)
{
	CBuffer tmp{WINDOW_WIDTH};
	tmp.AddEscapeSequence(move(color))
			.AddLine(R"(         _________ _        _        _______  _______ )")
			.AddLine(R"(|\     /|\__   __/( (    /|( (    /|(  ____ \(  ____ ))")
			.AddLine(R"(| )   ( |   ) (   |  \  ( ||  \  ( || (    \/| (    )|)")
			.AddLine(R"(| | _ | |   | |   |   \ | ||   \ | || (__    | (____)|)")
			.AddLine(R"(| |( )| |   | |   | (\ \) || (\ \) ||  __)   |     __))")
			.AddLine(R"(| || || |   | |   | | \   || | \   || (      | (\ (   )")
			.AddLine(R"(| () () |___) (___| )  \  || )  \  || (____/\| ) \ \__)")
			.AddLine(R"((_______)\_______/|/    )_)|/    )_)(_______/|/   \__/)")
			.Center()
			.AddEscapeSequence(Colors::RESET);
	return tmp;
}

void CInterface::GameOver() const
{
	bool col = false;
	while (true)
	{
		Render(move(CBuffer{WINDOW_WIDTH}
			.Append(CreateGameOverHeader(col ? Colors::FG_RED : Colors::FG_BLACK))));
		col = !col;
		if (GetChar() == 'q')
			break;
		Sleep(60ms);
	}
}

CBuffer CInterface::CreateGameOverHeader(string color)
{
	CBuffer tmp{WINDOW_WIDTH};
	tmp.AddEscapeSequence(move(color))
			.AddLine(R"( _______  _______  _______  _______    _______           _______  _______ )")
			.AddLine(R"((  ____ \(  ___  )(       )(  ____ \  (  ___  )|\     /|(  ____ \(  ____ ))")
			.AddLine(R"(| (    \/| (   ) || () () || (    \/  | (   ) || )   ( || (    \/| (    )|)")
			.AddLine(R"(| |      | (___) || || || || (__      | |   | || |   | || (__    | (____)|)")
			.AddLine(R"(| | ____ |  ___  || |(_)| ||  __)     | |   | |( (   ) )|  __)   |     __))")
			.AddLine(R"(| | \_  )| (   ) || |   | || (        | |   | | \ \_/ / | (      | (\ (   )")
			.AddLine(R"(| (___) || )   ( || )   ( || (____/\  | (___) |  \   /  | (____/\| ) \ \__)")
			.AddLine(R"((_______)|/     \||/     \|(_______/  (_______)   \_/   (_______/|/   \__/)")
			.Center()
			.AddEscapeSequence(Colors::RESET);
	return tmp;
}

/**********************************************************************************************************************/
// GAME SCREEN
void CInterface::GameScreen(const CGame & game) const
{
	Render(move(CBuffer{WINDOW_WIDTH}
		.Append(CreateGameHeader())
		.Append(game.Render(WINDOW_WIDTH))));
}

/**********************************************************************************************************************/
// HELP SCREEN
void CInterface::HelpScreen(const map<char, CCommand> & commands, const CUnitStack & stack) const
{
	Render({CreateCommandsHelpScreen(commands)});
	char ch = 0;
	bool com = true;
	while (ch != 'r')
	{
		ch = GetChar();
		if (ch == 'n')
		{
			com = !com;
			if (com)
				Render({CreateCommandsHelpScreen(commands)});
			else
				Render({CreateLegendScreen(stack)});
		}
	}
}

CBuffer CInterface::CreateCommandsHelpScreen(const std::map<char, CCommand> & commands)
{
	return move(CBuffer{WINDOW_WIDTH}
		.Append(CreateCommandsHeader())
		.Append(CreateGameOptions(commands))
		.Append(CreateHelpOptions()));
}

CBuffer CInterface::CreateLegendScreen(const CUnitStack & stack)
{
	return move(CBuffer{WINDOW_WIDTH}
		.Append(CreateLegendHeader())
		.Append(CreateCommonLegend())
		.Append(CreateTrooperLegend(stack))
		.Append(CreateTowerLegend(stack))
		.Append(CreateHelpOptions()));
}

CBuffer CInterface::CreateGameOptions(const map<char, CCommand> & commands)
{
	CBuffer tmp{WINDOW_WIDTH};
	for (const auto & command : commands)
	{
		if (!command.second.Help().empty())
		{
			tmp.AddLine("( "s + command.first + ")", string(Colors::BG_WHITE) + Colors::FG_BLACK)
				.AddLine(" ● "s + command.second.Help(), Colors::FG_WHITE)
				.AddLine();
		}
	}
	return tmp;
}

CBuffer CInterface::CreateCommonLegend()
{
	return move(CreateCommonHeader()
			.AddLine()
			.AddLine("(#) Wall", string(Colors::BG_WHITE) + Colors::FG_BLACK)
				.AddLine("● Obstacle through which neither troops, nor bullets can pass.", Colors::FG_WHITE)
				.AddLine()
			.AddLine("(#) Wall", string(Colors::BG_CYAN) + Colors::FG_BLACK)
				.AddLine(" ● Point from which troopers will be spawned.", Colors::FG_CYAN)
				.AddLine(" ● Numbered from 1 to 5 - each point point is tied to one wave in order.", Colors::FG_CYAN)
				.AddLine()
			.AddLine("(O) Gate", string(Colors::BG_MAGENTA) + Colors::FG_BLACK)
				.AddLine(" ● Gate to which the troopers need to get.", Colors::FG_MAGENTA)
				.AddLine(" ● Health points of the gate are displayed above the map.", Colors::FG_MAGENTA)
				.AddLine(" ● The goal of the player is to destroy the gate.", Colors::FG_MAGENTA)
				.AddLine());
}

CBuffer CInterface::CreateTrooperLegend(const CUnitStack & stack)
{
	CBuffer buffer{CreateTrooperHeader()};
	buffer.AddLine()
		.AddLine("Basic trooper", string(Colors::BG_YELLOW) + Colors::FG_BLACK)
			.AddLine(" ● Basic unit with limited health and options.", Colors::FG_YELLOW)
			.AddLine(" ● Will always take the shortest route to finish.", Colors::FG_YELLOW)
		.AddLine("Basic trooper", string(Colors::BG_CYAN) + Colors::FG_BLACK)
			.AddLine(" ● Armored trooper can take more damage than normal troops.", Colors::FG_CYAN)
			.AddLine(" ● He can wall up to prevent incoming damage, before his shields deplete.", Colors::FG_CYAN);
	return buffer;
}

CBuffer CInterface::CreateTowerLegend(const CUnitStack & stack)
{
	CBuffer buffer{CreateTowerHeader()};
	buffer.AddLine()
			.AddLine("Archer tower", string(Colors::BG_RED) + Colors::FG_BLACK)
				.AddLine(" ● Basic tower with archer attacks.", Colors::FG_RED)
				.AddLine(" ● Can focus only one trooper at once and it will be always the closest.", Colors::FG_RED)
			.AddLine("Mage tower", string(Colors::BG_BLUE) + Colors::FG_BLACK)
				.AddLine(" ● Mage tower which can cast magic attacks", Colors::FG_BLUE)
				.AddLine(" ● Magic wave is the main attack the tower can cast. It will damage troopers in radius around the tower.", Colors::FG_BLUE);
	return buffer;
}

CBuffer CInterface::CreateHelpOptions()
{
	return move(CBuffer{WINDOW_WIDTH}
		.AddLine()
		.AddLine("r - to return to game", Colors::FG_BLUE)
		.AddLine("n - move to next screen", Colors::FG_GREEN));
}

/**********************************************************************************************************************/
// HEADERS
CBuffer CInterface::CreateLegendHeader()
{
	
	return move(CBuffer{WINDOW_WIDTH}
		.AddEscapeSequence(Colors::FG_GREEN)
		.AddLine(R"( _                      _ )")
		.AddLine(R"(| |___ __ _ ___ _ _  __| |)")
		.AddLine(R"(| / -_) _` / -_) ' \/ _` |)")
		.AddLine(R"(|_\___\__, \___|_||_\__,_|)")
		.AddLine(R"(      |___/               )")
		.Center()
		.AddEscapeSequence(Colors::RESET));
}

CBuffer CInterface::CreateCommonHeader()
{
	return move(CBuffer{WINDOW_WIDTH}
						.AddLines(R"(  __ ___ _ __  _ __  ___ _ _
 / _/ _ \ '  \| '  \/ _ \ ' \
 \__\___/_|_|_|_|_|_\___/_||_|
                              )", Colors::FG_WHITE));	// TODO whether i want to use this or line by line
}

CBuffer CInterface::CreateTowerHeader()
{
	return move(CBuffer{WINDOW_WIDTH}
						.AddLines(R"(  _
 | |_ _____ __ _____ _ _ ___
 |  _/ _ \ V  V / -_) '_(_-<
  \__\___/\_/\_/\___|_| /__/
                            )", Colors::FG_RED));
}

CBuffer CInterface::CreateTrooperHeader()
{
	return move(CBuffer{WINDOW_WIDTH}
						.AddLines(R"(  _
 | |_ _ _ ___  ___ _ __  ___ _ _ ___
 |  _| '_/ _ \/ _ \ '_ \/ -_) '_(_-<
  \__|_| \___/\___/ .__/\___|_| /__/
                  |_|               )", Colors::FG_YELLOW));
}

CBuffer CInterface::CreateCommandsHeader()
{
	return move(CBuffer{WINDOW_WIDTH}
						.AddLines(R"(  _
 | |_ _____ __ _____ _ _ ___
 |  _/ _ \ V  V / -_) '_(_-<
  \__\___/\_/\_/\___|_| /__/
                            )", Colors::FG_WHITE));
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

void CInterface::ChangeWindowSize(int width, int height) const
{
	m_Out << "\x1b[8;" << height << ';' << width << 't';
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
	m_Out << CBuffer{WINDOW_WIDTH}
		.AddLine(string(message) + " Press space to continue.", string(Colors::BG_RED) + Colors::FG_BLACK)
		.Center();
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
	CBuffer tmp{WINDOW_WIDTH};
	tmp.AddEscapeSequence(Colors::FG_WHITE)
		.AddLines(message)
		.AddEscapeSequence(Colors::RESET)
		.AddLine(string(20, ' '), Colors::BG_WHITE)
		.Center();
	int height = tmp.Height();
	int width = tmp.Width();
	Render(move(tmp));
	
	// reposition cursor and set color to white
	(m_Out << Escapes::SHOW_CURSOR
		<< Escapes::REPOSITION_CURSOR((WINDOW_HEIGHT) / 2 + height / 2,WINDOW_WIDTH / 2 - width / 2)
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


