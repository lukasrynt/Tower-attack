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
CApplication::CApplication()
	: m_Game(make_unique<CGame>()),
	  m_Term({})
{
	// https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
	// get attributes to termios struct
	if (tcgetattr(STDIN_FILENO, &m_Term) == -1)
		Die("Error during tcgetattr");
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
		Die("Error during tcsetattr");
	
	cout << "\x1b[?25l";
	cout << "\x1b[8;" << DISPLAY_HEIGHT << ';' << DISPLAY_WIDTH << 't';
}

CApplication::~CApplication()
{
	// set the attributes back
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_Term) == -1)
		Die("Error during tcsetattr");
	
	cout << "\x1b[?25h";
	ResetScreen();
}

void CApplication::ResetTimeout()
{
	// get the attributes
	termios tmp = {};
	if (tcgetattr(STDIN_FILENO, &tmp) == -1)
		Die("Error during tcgetattr");
	
	// set timeout
	tmp.c_cc[VMIN] = 1;									// minimimum number of bytes that read needs before it can return
	tmp.c_cc[VTIME] = 0;								// time before read can return
	
	// set the attributes back to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp) == -1)
		Die("Error during tcsetattr");
}

void CApplication::NullTimeout()
{
	// get the attributes
	termios tmp = {};
	if (tcgetattr(STDIN_FILENO, &tmp) == -1)
		Die("Error during tcgetattr");
	
	// set timeout
	tmp.c_cc[VMIN] = 0;									// minimimum number of bytes that read needs before it can return
	tmp.c_cc[VTIME] = 1;								// time before read can return
	
	// set the attributes back to terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp) == -1)
		Die("Error during tcsetattr");
}

char CApplication::GetChar()
{
	char ch = 0;
	
	// read one byte from input
	if (read(STDIN_FILENO, &ch, sizeof(ch)) == -1)
		Die("Error during read");
	
	return ch;
}

void CApplication::Die(const char * s)
{
	perror(s);
	exit(1);
}

/**********************************************************************************************************************/
// MAIN LOOP
void CApplication::Run()
{
	MenuScreen();
	MainLoop();
}

void CApplication::MainLoop()
{
	//	http://gameprogrammingpatterns.com/game-loop.html
	while (m_Game->IsOn())
	{
		// get time stamp
		auto start = GetCurrentTime();
		ProcessInput();
		m_Game->Update();
		RenderGameScreen();
		Sleep(start - GetCurrentTime() + 30ms);
	}
}

chrono::milliseconds CApplication::GetCurrentTime()
{
	using namespace chrono;
	return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
}

void CApplication::Sleep(std::chrono::milliseconds duration)
{
	// https://en.cppreference.com/w/cpp/chrono/duration
	this_thread::sleep_for(duration);
}

/**********************************************************************************************************************/
// INPUT PROCESSING
void CApplication::ProcessInput()
{
	char ch = GetChar();
	switch (ch)
	{
		case 0:
			break;
		case 'h':
			HelpScreen();
			break;
		case 's':
			SaveScreen();
			break;
		case 'm':
			ResetTimeout();
			MenuScreen();
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
		InvalidInput(e.what());
	}
}

/**********************************************************************************************************************/
// GAME SCREEN RENDERING
void CApplication::RenderGameScreen() const
{
	ResetScreen();
	RenderGameOptions();
	m_Game->Render();
}

void CApplication::RenderGameOptions()
{
	cout 	<<	endl;
	cout 	<<	endl;
	cout 	<< 	Colors::fg_white
			<<	"Welcome to the game, pay attention to the control directions below, to quit the game anytime, just press 'q'"
			<<  Colors::color_reset
			<< 	endl;
	cout 	<<	endl;
	
	cout 	<< 	Colors::fg_cyan
			<< 	"1 - cycle troopers"
			<< 	Colors::color_reset << endl;
	
	cout 	<< Colors::fg_green
			<< "2 - cycle waves"
			<< Colors::color_reset << endl;
	
	cout 	<< Colors::fg_red
			<< "a - add troop"
			<< Colors::color_reset << endl;
	
	cout 	<< Colors::fg_magenta
			<< "p - start round"
			<< Colors::color_reset << endl;
	
	cout 	<< Colors::fg_blue
			<< "s - save game"
			<< Colors::color_reset << endl;
	
	cout 	<< Colors::fg_white
			<< "h - help screen"
			<< Colors::color_reset << endl;
	
	cout 	<< Colors::fg_black
			<< "m - menu screen"
			<< Colors::color_reset << endl;
}

void CApplication::ResetScreen()
{
	// clear the screen and move cursor back to top
	cout <<"\x1b[2J";
	cout <<"\x1b[H";
}

/**********************************************************************************************************************/
// MENU SCREEN
void CApplication::MenuScreen()
{
	int ch;
	bool end;
	do
	{
		end = true;
		ResetScreen();
		RenderHeader();
		RenderMenuScreen();
		
		ch = GetChar();
		
		// validate input
		switch (ch)
		{
			case '1':
				end = LoadNewGame();
				break;
			case '2':
				end = LoadSavedGame();
				break;
			case 'q':
				m_Game->End();
				break;
			default:
				end = false;
				InvalidInput("Invalid input.");
				break;
		}
		
	} while (!end);
	NullTimeout();
}

void CApplication::RenderHeader()
{
	// http://patorjk.com/software/taag/#p=display&f=Graffiti&t=Type%20Something%20
	cout << Colors::fg_yellow;
	PrintCenteredLine(R"(_________ _        _______           _______  _______ _________ _______  _       )");
	PrintCenteredLine(R"(\__   __/( (    /|(  ____ \|\     /|(  ____ )(  ____ \\__   __/(  ___  )( (    /|)");
	PrintCenteredLine(R"(   ) (   |  \  ( || (    \/| )   ( || (    )|| (    \/   ) (   | (   ) ||  \  ( |)");
	PrintCenteredLine(R"(   | |   |   \ | || |      | |   | || (____)|| (_____    | |   | |   | ||   \ | |)");
	PrintCenteredLine(R"(   | |   | (\ \) || |      | |   | ||     __)(_____  )   | |   | |   | || (\ \) |)");
	PrintCenteredLine(R"(   | |   | | \   || |      | |   | || (\ (         ) |   | |   | |   | || | \   |)");
	PrintCenteredLine(R"(___) (___| )  \  || (____/\| (___) || ) \ \__/\____) |___) (___| (___) || )  \  |)");
	PrintCenteredLine(R"(\_______/|/    )_)(_______/(_______)|/   \__/\_______)\_______/(_______)|/    )_))");
	cout << Colors::color_reset << endl;
}

void CApplication::RenderMenuScreen()
{
	cout	<< endl;
	cout	<< endl;
	cout 	<< setw(DISPLAY_WIDTH / 2) << Colors::fg_white
			<< "Choose one option"
			<< Colors::color_reset <<	endl;
	cout 	<<	endl;
	
	cout 	<< setw(DISPLAY_WIDTH / 2) << Colors::fg_cyan
			<< "1 - new game"
			<< Colors::color_reset << endl;
	cout 	<<	endl;
	
	cout 	<< setw(DISPLAY_WIDTH / 2) << Colors::fg_green
			<< "2 - load game"
			<< Colors::color_reset <<	endl;
	cout 	<< endl;
	cout 	<< setw(DISPLAY_WIDTH / 2) << Colors::fg_red
			<< "q - quit game"
			<< Colors::color_reset << endl;
	cout 	<<	endl;
}

void CApplication::PrintCenteredLine(const string & line)
{
	cout << string((DISPLAY_WIDTH - line.size()) / 2, ' ') << line << endl;
}

void CApplication::InvalidInput(const char * message)
{
	char ch = 0;
	cout << Colors::bg_red << Colors::fg_black << message << " Press space to continue." << Colors::color_reset << endl;
	while ((ch != ' '))
	{
		ch = GetChar();
	}
}

/**********************************************************************************************************************/
// LOADING
bool CApplication::LoadNewGame()
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
			cout << Colors::color_reset;
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
		
		m_Game = make_unique<CGame>();
		if (!m_Game->LoadNew(inFile))
		{
			InvalidInput("Incorrect file format.");
			continue;
		}
		inFile.close();
		cout << Colors::color_reset;
		return true;
	}
}

bool CApplication::LoadSavedGame()
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
			cout << Colors::color_reset;
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
		m_Game = make_unique<CGame>();
		if (!m_Game->LoadSaved(inFile))
		{
			InvalidInput("Incorrect file format.");
			continue;
		}
		cout << Colors::color_reset;
		inFile.close();
		return true;
	}
}

/**********************************************************************************************************************/
// HELP SCREEN
void CApplication::HelpScreen()
{
	ResetScreen();
	RenderHelpScreen();
	char ch = 0;
	while (ch != 'r')
	{
		ch = GetChar();
	}
}

void CApplication::RenderHelpScreen()
{
	// print header of help screen
	ResetScreen();
	RenderLegendHeader();
	
	// print legend
	RenderCommonLegend();
	RenderTrooperLegend();
	RenderTowerLegend();
	
	// return option
	cout << Colors::fg_white	<< "" << Colors::color_reset << endl << endl;
	cout << Colors::fg_blue 	<< "r - to return to game" << Colors::color_reset << endl;
	
}

void CApplication::RenderLegendHeader()
{
	cout << Colors::fg_green;
	PrintCenteredLine(R"( _                      _ )");
	PrintCenteredLine(R"(| |___ __ _ ___ _ _  __| |)");
	PrintCenteredLine(R"(| / -_) _` / -_) ' \/ _` |)");
	PrintCenteredLine(R"(|_\___\__, \___|_||_\__,_|)");
	PrintCenteredLine(R"(      |___/               )");
	cout << Colors::color_reset;
}

void CApplication::RenderCommonLegend()
{
	cout << Colors::fg_white <<
		 R"(  __ ___ _ __  _ __  ___ _ _
 / _/ _ \ '  \| '  \/ _ \ ' \
 \__\___/_|_|_|_|_|_\___/_||_|
                              )" << endl;
	cout 	<< Colors::bg_white << Colors::fg_black <<  "(#) Wall" << Colors::color_reset << endl
			<< Colors::fg_white
			<< 	"	● Obstacle through which neither troops, nor bullets can pass.\n";
	
	cout 	<< Colors::bg_cyan << Colors::fg_black <<  "(1) Spawn point" << Colors::color_reset << endl
			<< Colors::fg_cyan
			<< 	"	● Point from which troopers will be spawned.\n"
				  "	● Numbered from 1 - each point point is tied to one wave in order.\n";
	
	cout 	<< Colors::bg_magenta << Colors::fg_black <<  "(O) Gate" << Colors::color_reset << endl
			<< Colors::fg_magenta
			<< 	"	● Gate to which the troopers need to get.\n"
				  "	● Health points of the gate are displayed in the top right corner.\n"
				  "	● The goal of the player is to destroy the gate.\n";
}

void CApplication::RenderTrooperLegend()
{
	cout << Colors::fg_white <<
		 R"(  _
 | |_ _ _ ___  ___ _ __  ___ _ _ ___
 |  _| '_/ _ \/ _ \ '_ \/ -_) '_(_-<
  \__|_| \___/\___/ .__/\___|_| /__/
                  |_|               )" << endl;
	cout 	<< Colors::bg_yellow << Colors::fg_black <<  "(@) Basic trooper" << Colors::color_reset << endl
			<< Colors::fg_yellow
			<< 	"	● Basic unit with limited health and options.\n"
				  "	● Will always take the shortest route to finish.\n";
	
	cout 	<< Colors::bg_cyan << Colors::fg_black <<  "($) Armored trooper" << Colors::color_reset << endl
			<< Colors::fg_cyan
			<< 	"	● Protects fellow soldiers.\n"
				  "	● Will take aggro of the turrets.\n"
				  "	● Can take more damage, unless his shields are depleted.\n";
}

void CApplication::RenderTowerLegend()
{
	cout << Colors::fg_white << R"(  _
 | |_ _____ __ _____ _ _ ___
 |  _/ _ \ V  V / -_) '_(_-<
  \__\___/\_/\_/\___|_| /__/
                            )" << endl;
	cout 	<< Colors::bg_red << Colors::fg_black <<  "(*) Archer tower" << Colors::color_reset << endl
			<< Colors::fg_red
			<< 	"	● Basic tower with archer attacks.\n"
				  "	● Can focus only one trooper at once and it will be always the closest.\n";
	
	cout 	<< Colors::bg_blue << Colors::fg_black <<  "(%) Mage tower" << Colors::color_reset << endl
			<< Colors::fg_blue
			<< 	"	● Mage tower which can cast magic attacks, it needs full mana in order to cast them.\n"
				  "	● Magic wave is the main attack the tower can cast. It will damage troopers in radius around the tower.\n";
}

/**********************************************************************************************************************/
// SAVE SCREEN
void CApplication::SaveScreen()
{
	if (!SaveGame())
		return;
	RenderGameSavedScreen();
	char ch;
	while (true)
	{
		ch = GetChar();
		if (ch == 'q')
		{
			m_Game->End();
			break;
		}
		else if (ch == 'r')
			break;
	}
}

bool CApplication::SaveGame()
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
			cout << Colors::color_reset;
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
		if (!m_Game->Save(outFile))
		{
			InvalidInput("Error during writing to file.");
			continue;
		}
		cout << Colors::color_reset;
		outFile.close();
		return true;
	}
}

void CApplication::RenderGameSavedScreen()
{
	ResetScreen();
	cout << Colors::fg_white	<< "Game saved. Do you wish to continue?" << Colors::color_reset << endl << endl;
	cout << Colors::fg_red 		<< "		q - to quit					" << Colors::color_reset << endl;
	cout << Colors::fg_blue 	<< "		r - to return to game       " << Colors::color_reset << endl;
}

string CApplication::PromptFileName(const string & message)
{
	// welcome message
	ResetScreen();
	cout << Colors::fg_white << message
		 << Colors::color_reset << endl << endl;
	
	
	// disable raw mode for a while
	termios orig = {};
	if (tcgetattr(STDIN_FILENO, &orig) == -1)
		Die("Error during tcgetattr");
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_Term) == -1)
		Die("Error during tcsetattr");
	cout << "\x1b[?25h";
	
	// ask for filename
	cout << "\x1b[2, 10H";
	cout << Colors::fg_black << Colors::bg_white << string(20, ' ') << '\r';
	cout.flush();
	string filename;
	char ch;
	while ((ch = GetChar()) != '\n')
		filename += ch;
	
	// enable raw mode back
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig) == -1)
		Die("Error during tcsetattr");
	cout << "\x1b[?25l";
	return filename;
}