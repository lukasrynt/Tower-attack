/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 02/05/2020.
*/

#include <iomanip>
#include <zconf.h>
#include <thread>
#include "CInterface.hpp"
#include "Colors.hpp"

using namespace std;

CInterface::CInterface(ostream & out)
	: m_Out(out),
	  m_Term({})
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
	ChangeWindowSize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
}

CInterface::~CInterface() noexcept(false)
{
	// set the attributes back
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_Term) == -1)
		throw runtime_error("Error during tcsetattr");
	
	ShowCursor();
	ResetScreen();
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
			m_Out << Colors::color_reset;
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
		
		m_Out << Colors::color_reset;
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
			m_Out << Colors::color_reset;
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
		m_Out << Colors::color_reset;
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
		ch = GetInput();
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
			m_Out << Colors::color_reset;
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
		m_Out << Colors::color_reset;
		outFile.close();
		return true;
	}
}

void CInterface::SavedScreen() const
{
	ResetScreen();
	m_Out << Colors::fg_white	<< "Game saved. Do you wish to continue?" << Colors::color_reset << endl << endl;
	m_Out << Colors::fg_red 	<< "		q - to quit					" << Colors::color_reset << endl;
	m_Out << Colors::fg_blue 	<< "		r - to return to game       " << Colors::color_reset << endl;
}

/**********************************************************************************************************************/
// MENU SCREEN
void CInterface::Menu() const
{
	ResetScreen();
	RenderHeader();
	RenderMenuOptions();
}

void CInterface::RenderMenuOptions() const
{
	m_Out	<< endl;
	m_Out	<< endl;
	m_Out 	<< setw(DISPLAY_WIDTH / 2) << Colors::fg_white
			 << "Choose one option"
			 << Colors::color_reset <<	endl;
	m_Out 	<<	endl;
	
	m_Out 	<< setw(DISPLAY_WIDTH / 2) << Colors::fg_cyan
			 << "1 - new game"
			 << Colors::color_reset << endl;
	m_Out 	<<	endl;
	
	m_Out 	<< setw(DISPLAY_WIDTH / 2) << Colors::fg_green
			 << "2 - load game"
			 << Colors::color_reset <<	endl;
	m_Out 	<< endl;
	m_Out 	<< setw(DISPLAY_WIDTH / 2) << Colors::fg_red
			 << "q - quit game"
			 << Colors::color_reset << endl;
	m_Out 	<<	endl;
}

void CInterface::RenderHeader() const
{
	// http://patorjk.com/software/taag/#p=display&f=Graffiti&t=Type%20Something%20
	m_Out << Colors::fg_yellow;
	PrintCenteredLine(R"(_________ _        _______           _______  _______ _________ _______  _       )");
	PrintCenteredLine(R"(\__   __/( (    /|(  ____ \|\     /|(  ____ )(  ____ \\__   __/(  ___  )( (    /|)");
	PrintCenteredLine(R"(   ) (   |  \  ( || (    \/| )   ( || (    )|| (    \/   ) (   | (   ) ||  \  ( |)");
	PrintCenteredLine(R"(   | |   |   \ | || |      | |   | || (____)|| (_____    | |   | |   | ||   \ | |)");
	PrintCenteredLine(R"(   | |   | (\ \) || |      | |   | ||     __)(_____  )   | |   | |   | || (\ \) |)");
	PrintCenteredLine(R"(   | |   | | \   || |      | |   | || (\ (         ) |   | |   | |   | || | \   |)");
	PrintCenteredLine(R"(___) (___| )  \  || (____/\| (___) || ) \ \__/\____) |___) (___| (___) || )  \  |)");
	PrintCenteredLine(R"(\_______/|/    )_)(_______/(_______)|/   \__/\_______)\_______/(_______)|/    )_))");
	m_Out << Colors::color_reset << endl;
}

/**********************************************************************************************************************/
// GAME END
void CInterface::Winner() const
{
	bool col = false;
	while (true)
	{
		if (col)
			m_Out << Colors::fg_green;
		else
			m_Out << Colors::fg_blue;
		col = !col;
		ResetScreen();
		RenderWinner();
		if (GetInput() == 'q')
			break;
		Sleep(60ms);
	}
	m_Out << Colors::color_reset;
}

void CInterface::RenderWinner() const
{
	const int TEXT_HEIGHT = 8;
	for (int i = 0; i < (DISPLAY_HEIGHT - TEXT_HEIGHT) / 2; ++i)
		m_Out << endl;
	PrintCenteredLine(R"(         _________ _        _        _______  _______ )");
	PrintCenteredLine(R"(|\     /|\__   __/( (    /|( (    /|(  ____ \(  ____ ))");
	PrintCenteredLine(R"(| )   ( |   ) (   |  \  ( ||  \  ( || (    \/| (    )|)");
	PrintCenteredLine(R"(| | _ | |   | |   |   \ | ||   \ | || (__    | (____)|)");
	PrintCenteredLine(R"(| |( )| |   | |   | (\ \) || (\ \) ||  __)   |     __))");
	PrintCenteredLine(R"(| || || |   | |   | | \   || | \   || (      | (\ (   )");
	PrintCenteredLine(R"(| () () |___) (___| )  \  || )  \  || (____/\| ) \ \__)");
	PrintCenteredLine(R"((_______)\_______/|/    )_)|/    )_)(_______/|/   \__/)");
}

void CInterface::GameOver() const
{
	bool col = false;
	while (true)
	{
		if (col)
			m_Out << Colors::fg_red;
		else
			m_Out << Colors::fg_black;
		col = !col;
		ResetScreen();
		RenderGameOver();
		if (GetInput() == 'q')
			break;
		Sleep(60ms);
	}
	m_Out << Colors::color_reset;
}

void CInterface::RenderGameOver() const
{
	const int TEXT_HEIGHT = 8;
	for (int i = 0; i < (DISPLAY_HEIGHT - TEXT_HEIGHT) / 2; ++i)
		m_Out << endl;
	PrintCenteredLine(R"( _______  _______  _______  _______    _______           _______  _______ )");
	PrintCenteredLine(R"((  ____ \(  ___  )(       )(  ____ \  (  ___  )|\     /|(  ____ \(  ____ ))");
	PrintCenteredLine(R"(| (    \/| (   ) || () () || (    \/  | (   ) || )   ( || (    \/| (    )|)");
	PrintCenteredLine(R"(| |      | (___) || || || || (__      | |   | || |   | || (__    | (____)|)");
	PrintCenteredLine(R"(| | ____ |  ___  || |(_)| ||  __)     | |   | |( (   ) )|  __)   |     __))");
	PrintCenteredLine(R"(| | \_  )| (   ) || |   | || (        | |   | | \ \_/ / | (      | (\ (   )");
	PrintCenteredLine(R"(| (___) || )   ( || )   ( || (____/\  | (___) |  \   /  | (____/\| ) \ \__)");
	PrintCenteredLine(R"((_______)|/     \||/     \|(_______/  (_______)   \_/   (_______/|/   \__/)");
}

/**********************************************************************************************************************/
// GAME SCREEN
void CInterface::GameScreen(const unique_ptr<CGame> & game) const
{
	ResetScreen();
	RenderGameOptions();
	game->Render(m_Out);
}

void CInterface::RenderGameOptions() const
{
	m_Out 	<<	endl << endl;
	m_Out 	<< 	Colors::fg_white
			 <<	"Welcome to the game, pay attention to the control directions below, to quit the game anytime, just press 'q'"
			 <<  Colors::color_reset
			 << 	endl;
	m_Out 	<<	endl;
	
	m_Out 	<< 	Colors::fg_green
			 << 	"1 - cycle waves"
			 << 	Colors::color_reset << endl;
	
	m_Out 	<< Colors::fg_cyan
			 << "2 - cycle troops"
			 << Colors::color_reset << endl;
	
	m_Out 	<< Colors::fg_red
			 << "a - add troop"
			 << Colors::color_reset << endl;
	
	m_Out 	<< Colors::fg_yellow
			 << "d - delete troop"
			 << Colors::color_reset << endl;
	
	m_Out 	<< Colors::fg_magenta
			 << "p - start round"
			 << Colors::color_reset << endl;
	
	m_Out 	<< Colors::fg_blue
			 << "s - save game"
			 << Colors::color_reset << endl;
	
	m_Out 	<< Colors::fg_white
			 << "h - help screen"
			 << Colors::color_reset << endl;
}

/**********************************************************************************************************************/
// HELP SCREEN
void CInterface::HelpScreen() const
{
	ResetScreen();
	RenderHelpScreen();
	char ch = 0;
	while (ch != 'r')
		ch = GetInput();
}

void CInterface::RenderHelpScreen() const
{
	// print header of help screen
	ResetScreen();
	RenderLegendHeader();
	
	// print legend
	RenderCommonLegend();
	RenderTrooperLegend();
	RenderTowerLegend();
	
	// return option
	m_Out << Colors::fg_blue 	<< "r - to return to game" << Colors::color_reset << endl;
	
}

void CInterface::RenderLegendHeader() const
{
	m_Out << Colors::fg_green;
	PrintCenteredLine(R"( _                      _ )");
	PrintCenteredLine(R"(| |___ __ _ ___ _ _  __| |)");
	PrintCenteredLine(R"(| / -_) _` / -_) ' \/ _` |)");
	PrintCenteredLine(R"(|_\___\__, \___|_||_\__,_|)");
	PrintCenteredLine(R"(      |___/               )");
	m_Out << Colors::color_reset;
}

void CInterface::RenderCommonLegend() const
{
	m_Out << Colors::fg_white <<
		 R"(  __ ___ _ __  _ __  ___ _ _
 / _/ _ \ '  \| '  \/ _ \ ' \
 \__\___/_|_|_|_|_|_\___/_||_|
                              )" << endl;
	m_Out 	<< Colors::bg_white << Colors::fg_black <<  "(#) Wall" << Colors::color_reset << endl
			<< Colors::fg_white
			<< 	"	● Obstacle through which neither troops, nor bullets can pass.\n";
	
	m_Out 	<< Colors::bg_cyan << Colors::fg_black <<  "(1) Spawn point" << Colors::color_reset << endl
			<< Colors::fg_cyan
			<< 	"	● Point from which troopers will be spawned.\n"
				  "	● Numbered from 1 - each point point is tied to one wave in order.\n";
	
	m_Out 	<< Colors::bg_magenta << Colors::fg_black <<  "(O) Gate" << Colors::color_reset << endl
			<< Colors::fg_magenta
			<< 	"	● Gate to which the troopers need to get.\n"
				  "	● Health points of the gate are displayed in the top right corner.\n"
				  "	● The goal of the player is to destroy the gate.\n";
}	// TODO: let unit stack handle writing info

void CInterface::RenderTrooperLegend() const
{
	m_Out << Colors::fg_white <<
		 R"(  _
 | |_ _ _ ___  ___ _ __  ___ _ _ ___
 |  _| '_/ _ \/ _ \ '_ \/ -_) '_(_-<
  \__|_| \___/\___/ .__/\___|_| /__/
                  |_|               )" << endl;
	m_Out 	<< Colors::bg_yellow << Colors::fg_black <<  "(@) Basic trooper" << Colors::color_reset << endl
			<< Colors::fg_yellow
			<< 	"	● Basic unit with limited health and options.\n"
				  "	● Will always take the shortest route to finish.\n";
	
	m_Out 	<< Colors::bg_cyan << Colors::fg_black <<  "($) Armored trooper" << Colors::color_reset << endl
			<< Colors::fg_cyan
			<< 	"	● Protects fellow soldiers.\n"
				  "	● Will take aggro of the turrets.\n"
				  "	● Can take more damage, unless his shields are depleted.\n";
}

void CInterface::RenderTowerLegend() const
{
	m_Out << Colors::fg_white << R"(  _
 | |_ _____ __ _____ _ _ ___
 |  _/ _ \ V  V / -_) '_(_-<
  \__\___/\_/\_/\___|_| /__/
                            )" << endl;
	m_Out 	<< Colors::bg_red << Colors::fg_black <<  "(*) Archer tower" << Colors::color_reset << endl
			<< Colors::fg_red
			<< 	"	● Basic tower with archer attacks.\n"
				  "	● Can focus only one trooper at once and it will be always the closest.\n";
	
	m_Out 	<< Colors::bg_blue << Colors::fg_black <<  "(%) Mage tower" << Colors::color_reset << endl
			<< Colors::fg_blue
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

char CInterface::GetInput()
{
	char ch = 0;
	
	// read one byte from input
	if (read(STDIN_FILENO, &ch, sizeof(ch)) == -1)
		throw runtime_error("Error during read");
	
	return ch;
}

void CInterface::ResetScreen() const
{
	// clear the screen and move cursor back to top
	m_Out <<"\x1b[2J";
	m_Out <<"\x1b[H";
}

void CInterface::InvalidInput(const char * message) const
{
	char ch = 0;
	m_Out << Colors::bg_red << Colors::fg_black << message << " Press space to continue." << Colors::color_reset << endl;
	while ((ch != ' '))
	{
		ch = GetInput();
	}
}

string CInterface::PromptFileName(const string & message) const
{
	// welcome message
	ResetScreen();
	m_Out << Colors::fg_white << message
		  << Colors::color_reset << endl << endl;
	
	
	// disable raw mode for a while
	termios orig = {};
	if (tcgetattr(STDIN_FILENO, &orig) == -1)
		throw runtime_error("Error during tcgetattr");
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_Term) == -1)
		throw runtime_error("Error during tcsetattr");
	m_Out << "\x1b[?25h";
	
	// ask for filename
	m_Out << "\x1b[2, 10H";
	m_Out << Colors::fg_black << Colors::bg_white << string(20, ' ') << '\r';
	m_Out.flush();
	string filename;
	char ch;
	while ((ch = GetInput()) != '\n')
		filename += ch;
	
	// enable raw mode back
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig) == -1)
		throw runtime_error("Error during tcsetattr");
	m_Out << "\x1b[?25l";
	return filename;
}

void CInterface::PrintCenteredLine(const string & line) const
{
	m_Out << string((DISPLAY_WIDTH - line.size()) / 2, ' ') << line << endl;
}


