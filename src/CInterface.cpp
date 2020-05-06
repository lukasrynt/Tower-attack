/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 02/05/2020.
*/

#include <iomanip>
#include <zconf.h>
#include <thread>
#include "CInterface.hpp"
#include "NColors.hpp"
#include "ExInvalidInput.hpp"

using namespace std;

CInterface::CInterface(ostream & out)
	: m_Out(out),
	  m_Term({}),
	  m_Buffer(WINDOW_WIDTH)
{
	// https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
	// get attributes to termios struct
	if (tcgetattr(STDIN_FILENO, &m_Term) == -1)
		throw runtime_error("Error during tcgetattr");
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
		throw runtime_error("Error during tcsetattr");
	
	HideCursor();
	ChangeWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

CInterface::~CInterface() noexcept(false)
{
	// set the attributes back
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_Term) == -1)
		throw runtime_error("Error during tcsetattr");
	
	ShowCursor();
	ResetScreen();
}

void CInterface::Render()
{
	ResetScreen();
	
	// align horizontally
	if (m_Buffer.Size() < WINDOW_HEIGHT)
		for (size_t i = 0; i < (WINDOW_HEIGHT - m_Buffer.Size()) / 2; ++i)
			if (!(m_Out << endl))
				throw runtime_error("Failure during write");
			
	// render buffer
	if (!(m_Out << m_Buffer))
		throw runtime_error("Failure during write");
	m_Buffer.Flush();
}

/**********************************************************************************************************************/
// LOAD
bool CInterface::LoadNewGame(unique_ptr<CGame> & game) const
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
			m_Out << Colors::RESET;
			return false;
		}
		
		// check input stream
		string filename = "maps/" + response + ".map";
		ifstream inFile(filename);
		
		if (!inFile)
		{
			InvalidInput("File not found.");
			continue;
		}
		
		// load game
		game = make_unique<CGame>();
		
		if (!(inFile >> *game)
			|| !(game->CheckNew()))
		{
			InvalidInput("Incorrect file format.");
			continue;
		}
		inFile.close();
		
		m_Out << Colors::RESET;
		NullTimeout();
		inFile.close();
		return true;
	}
}

bool CInterface::LoadSavedGame(unique_ptr<CGame> & game) const
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
			m_Out << Colors::RESET;
			return false;
		}
		
		// check input stream
		string filename = "saves/" + response + ".sav";
		ifstream inFile(filename);
		if (!inFile)
		{
			InvalidInput("File not found.");
			continue;
		}
		
		// load game
		game = make_unique<CGame>();
		if (!(inFile >> *game)
			|| !(game->CheckSaved()))
		{
			InvalidInput("Incorrect file format.");
			continue;
		}
		m_Out << Colors::RESET;
		NullTimeout();
		inFile.close();
		return true;
	}
}

/**********************************************************************************************************************/
// SAVE
bool CInterface::Save(const unique_ptr<CGame> & game) const
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

bool CInterface::PromptSave(const unique_ptr<CGame> & game) const
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
		if (!outFile)
		{
			InvalidInput("File not found.");
			continue;
		}
		
		
		// save game
		if (!(outFile << *game))
		{
			InvalidInput("Error during writing to file.");
			continue;
		}
		m_Out << Colors::RESET;
		outFile.close();
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
void CInterface::Menu(const map<char, CCommand> & commands)
{
	AppendHeader();
	AppendMenuOptions(commands);
	Render();
}

void CInterface::AppendMenuOptions(const map<char, CCommand> & commands)
{
	m_Buffer.AddLine();
	m_Buffer.AddLine();
	for (const auto & command : commands)
	{
		m_Buffer.AddCenteredLine(command.second.Help(command.first), command.second.Color());
		m_Buffer.AddLine();
	}
}

void CInterface::AppendHeader()
{
	// http://patorjk.com/software/taag/#p=display&f=Graffiti&t=Type%20Something%20
	m_Buffer << Colors::FG_YELLOW;
	m_Buffer.AddHeaderLine(R"(_________ _        _______           _______  _______ _________ _______  _       )")
			.AddHeaderLine(R"(\__   __/( (    /|(  ____ \|\     /|(  ____ )(  ____ \\__   __/(  ___  )( (    /|)")
			.AddHeaderLine(R"(   ) (   |  \  ( || (    \/| )   ( || (    )|| (    \/   ) (   | (   ) ||  \  ( |)")
			.AddHeaderLine(R"(   | |   |   \ | || |      | |   | || (____)|| (_____    | |   | |   | ||   \ | |)")
			.AddHeaderLine(R"(   | |   | (\ \) || |      | |   | ||     __)(_____  )   | |   | |   | || (\ \) |)")
			.AddHeaderLine(R"(   | |   | | \   || |      | |   | || (\ (         ) |   | |   | |   | || | \   |)")
			.AddHeaderLine(R"(___) (___| )  \  || (____/\| (___) || ) \ \__/\____) |___) (___| (___) || )  \  |)")
			.AddHeaderLine(R"(\_______/|/    )_)(_______/(_______)|/   \__/\_______)\_______/(_______)|/    )_))");
	m_Buffer << Colors::RESET;
	m_Buffer.AddLine();
	m_Buffer.AddLine();
	m_Buffer.AddLine();
}

/**********************************************************************************************************************/
// GAME END
void CInterface::Winner()
{
	bool col = false;
	while (true)
	{
		if (col)
			m_Buffer << Colors::FG_GREEN;
		else
			m_Buffer << Colors::FG_BLUE;
		col = !col;
		AppendWinner();
		Render();
		if (GetChar() == 'q')
			break;
		Sleep(60ms);
	}
	m_Buffer << Colors::RESET;
}

void CInterface::AppendWinner()
{
	m_Buffer.AddHeaderLine(R"(         _________ _        _        _______  _______ )")
			.AddHeaderLine(R"(|\     /|\__   __/( (    /|( (    /|(  ____ \(  ____ ))")
			.AddHeaderLine(R"(| )   ( |   ) (   |  \  ( ||  \  ( || (    \/| (    )|)")
			.AddHeaderLine(R"(| | _ | |   | |   |   \ | ||   \ | || (__    | (____)|)")
			.AddHeaderLine(R"(| |( )| |   | |   | (\ \) || (\ \) ||  __)   |     __))")
			.AddHeaderLine(R"(| || || |   | |   | | \   || | \   || (      | (\ (   )")
			.AddHeaderLine(R"(| () () |___) (___| )  \  || )  \  || (____/\| ) \ \__)")
			.AddHeaderLine(R"((_______)\_______/|/    )_)|/    )_)(_______/|/   \__/)");
}

void CInterface::GameOver()
{
	bool col = false;
	while (true)
	{
		if (col)
			m_Buffer << Colors::FG_RED;
		else
			m_Buffer << Colors::FG_BLACK;
		col = !col;
		AppendGameOver();
		Render();
		if (GetChar() == 'q')
			break;
		Sleep(60ms);
	}
	m_Buffer << Colors::RESET;
}

void CInterface::AppendGameOver()
{
	m_Buffer.AddHeaderLine(R"( _______  _______  _______  _______    _______           _______  _______ )")
			.AddHeaderLine(R"((  ____ \(  ___  )(       )(  ____ \  (  ___  )|\     /|(  ____ \(  ____ ))")
			.AddHeaderLine(R"(| (    \/| (   ) || () () || (    \/  | (   ) || )   ( || (    \/| (    )|)")
			.AddHeaderLine(R"(| |      | (___) || || || || (__      | |   | || |   | || (__    | (____)|)")
			.AddHeaderLine(R"(| | ____ |  ___  || |(_)| ||  __)     | |   | |( (   ) )|  __)   |     __))")
			.AddHeaderLine(R"(| | \_  )| (   ) || |   | || (        | |   | | \ \_/ / | (      | (\ (   )")
			.AddHeaderLine(R"(| (___) || )   ( || )   ( || (____/\  | (___) |  \   /  | (____/\| ) \ \__)")
			.AddHeaderLine(R"((_______)|/     \||/     \|(_______/  (_______)   \_/   (_______/|/   \__/)");
}

/**********************************************************************************************************************/
// GAME SCREEN
void CInterface::GameScreen(const unique_ptr<CGame> & game)
{
	ResetScreen();
	m_Buffer = game->Render(WINDOW_WIDTH);
	Render();
}

void CInterface::AppendGameOptions(const map<char, CCommand> & commands)
{
	AppendHeader();
	for (const auto & command : commands)
	{
		string line = command.second.Help(command.first);
		if (!line.empty())
			m_Buffer.AddCenteredLine(line, command.second.Color());
	}
}

/**********************************************************************************************************************/
// HELP SCREEN
void CInterface::HelpScreen(const map<char, CCommand> & commands)
{
	AppendGameOptions(commands);
	Render();
	RenderHelpScreen();
	char ch = 0;
	while (ch != 'r')
		ch = GetChar();
}

void CInterface::RenderHelpScreen()
{
	// print header of help screen
	ResetScreen();
	RenderLegendHeader();
	
	// print legend
	RenderCommonLegend();
	RenderTrooperLegend();
	RenderTowerLegend();
	
	// return option
	m_Out << Colors::FG_BLUE << "r - to return to game" << Colors::RESET << endl;
	
}

void CInterface::RenderLegendHeader()
{
	m_Buffer << Colors::FG_GREEN;
	m_Buffer.AddCenteredLine(R"( _                      _ )")
			.AddCenteredLine(R"(| |___ __ _ ___ _ _  __| |)")
			.AddCenteredLine(R"(| / -_) _` / -_) ' \/ _` |)")
			.AddCenteredLine(R"(|_\___\__, \___|_||_\__,_|)")
			.AddCenteredLine(R"(      |___/               )");
	m_Buffer << Colors::RESET;
}

void CInterface::RenderCommonLegend() const
{
	m_Out << Colors::FG_WHITE <<
		  R"(  __ ___ _ __  _ __  ___ _ _
 / _/ _ \ '  \| '  \/ _ \ ' \
 \__\___/_|_|_|_|_|_\___/_||_|
                              )" << endl;
	m_Out << Colors::BG_WHITE << Colors::FG_BLACK << "(#) Wall" << Colors::RESET << endl
		  << Colors::FG_WHITE
		  << "	● Obstacle through which neither troops, nor bullets can pass.\n";
	
	m_Out << Colors::BG_CYAN << Colors::FG_BLACK << "(1) Spawn point" << Colors::RESET << endl
		  << Colors::FG_CYAN
		  << 	"	● Point from which troopers will be spawned.\n"
				  "	● Numbered from 1 - each point point is tied to one wave in order.\n";
	
	m_Out << Colors::BG_MAGENTA << Colors::FG_BLACK << "(O) Gate" << Colors::RESET << endl
		  << Colors::FG_MAGENTA
		  << 	"	● Gate to which the troopers need to get.\n"
				  "	● Health points of the gate are displayed in the top right corner.\n"
				  "	● The goal of the player is to destroy the gate.\n";
}	// TODO: let unit stack handle writing info

void CInterface::RenderTrooperLegend() const
{
	m_Out << Colors::FG_WHITE <<
		  R"(  _
 | |_ _ _ ___  ___ _ __  ___ _ _ ___
 |  _| '_/ _ \/ _ \ '_ \/ -_) '_(_-<
  \__|_| \___/\___/ .__/\___|_| /__/
                  |_|               )" << endl;
	m_Out << Colors::BG_YELLOW << Colors::FG_BLACK << "(@) Basic trooper" << Colors::RESET << endl
		  << Colors::FG_YELLOW
		  << 	"	● Basic unit with limited health and options.\n"
				  "	● Will always take the shortest route to finish.\n";
	
	m_Out << Colors::BG_CYAN << Colors::FG_BLACK << "($) Armored trooper" << Colors::RESET << endl
		  << Colors::FG_CYAN
		  << 	"	● Protects fellow soldiers.\n"
				  "	● Will take aggro of the turrets.\n"
				  "	● Can take more damage, unless his shields are depleted.\n";
}

void CInterface::RenderTowerLegend() const
{
	m_Out << Colors::FG_WHITE << R"(  _
 | |_ _____ __ _____ _ _ ___
 |  _/ _ \ V  V / -_) '_(_-<
  \__\___/\_/\_/\___|_| /__/
                            )" << endl;
	m_Out << Colors::BG_RED << Colors::FG_BLACK << "(*) Archer tower" << Colors::RESET << endl
		  << Colors::FG_RED
		  << 	"	● Basic tower with archer attacks.\n"
				  "	● Can focus only one trooper at once and it will be always the closest.\n";
	
	m_Out << Colors::BG_BLUE << Colors::FG_BLACK << "(%) Mage tower" << Colors::RESET << endl
		  << Colors::FG_BLUE
		  << 	"	● Mage tower which can cast magic attacks, it needs full mana in order to cast them.\n"
				  "	● Magic wave is the main attack the tower can cast. It will damage troopers in radius around the tower.\n";
}

/**********************************************************************************************************************/
// TIME
chrono::milliseconds CInterface::GetCurrentTime()
{
	using namespace chrono;
	return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
}

void CInterface::Sleep(std::chrono::milliseconds duration)
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
		throw runtime_error("Error during tcgetattr");
	
	// set timeout
	tmp.c_cc[VMIN] = 1;									// minimimum number of bytes that read needs before it can return
	tmp.c_cc[VTIME] = 0;								// time before read can return
	
	// set the attributes back to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp) == -1)
		throw runtime_error("Error during tcsetattr");
}

void CInterface::NullTimeout()
{
	// get the attributes
	termios tmp = {};
	if (tcgetattr(STDIN_FILENO, & tmp) == -1)
		throw runtime_error("Error during tcgetattr");
	
	// set timeout
	tmp.c_cc[VMIN] = 0;									// minimimum number of bytes that read needs before it can return
	tmp.c_cc[VTIME] = 1;								// time before read can return
	
	// set the attributes back to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, & tmp) == -1)
		throw runtime_error("Error during tcsetattr");
}

void CInterface::HideCursor() const
{
	m_Out << "\x1b[?25l";
}

void CInterface::ShowCursor() const
{
	m_Out << "\x1b[?25h";
}

void CInterface::ChangeWindowSize(int width, int height) const
{
	m_Out << "\x1b[8;" << height << ';' << width << 't';
}

char CInterface::GetChar()
{
	char ch = 0;
	
	// read one byte from input
	if (read(STDIN_FILENO, &ch, sizeof(ch)) == -1)
		throw runtime_error("Error during read");
	
	return ch;
}

void CInterface::InvalidInput(const char * message) const
{
	char ch = 0;
	m_Out << Colors::BG_RED << Colors::FG_BLACK << message << " Press space to continue." << Colors::RESET << endl;
	while ((ch != ' '))
	{
		ch = GetChar();
	}
}

string CInterface::PromptFileName(const string & message) const
{
	// welcome message
	ResetScreen();
	m_Out << Colors::FG_WHITE << message
		  << Colors::RESET << endl << endl;
	
	
	// disable raw mode for a while
	termios orig = {};
	if (tcgetattr(STDIN_FILENO, &orig) == -1)
		throw runtime_error("Error during tcgetattr");
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_Term) == -1)
		throw runtime_error("Error during tcsetattr");
	m_Out << "\x1b[?25h";
	
	// ask for filename
	m_Out << "\x1b[2, 10H";
	m_Out << Colors::FG_BLACK << Colors::BG_WHITE << string(20, ' ') << '\r';
	m_Out.flush();
	string filename;
	char ch;
	while ((ch = GetChar()) != '\n')
		filename += ch;
	
	// enable raw mode back
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig) == -1)
		throw runtime_error("Error during tcsetattr");
	m_Out << "\x1b[?25l";
	return filename;
}

void CInterface::ResetScreen() const
{
	// clear the screen and move cursor back to top
	m_Out << "\x1b[2J";
	m_Out << "\x1b[H";
}

