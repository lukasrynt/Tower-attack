#include <iostream>
#include <fstream>
#include <CGame.hpp>
#include "CWaves.hpp"
#include "CMap.hpp"
#include "CPath.hpp"

using namespace std;

int main ()
{
	ifstream inFile("/home/lukas/School/PA2/semestralka/saves/test1.sav");
	if (!inFile)
		return 1;
	CGame game{};
	if (!game.LoadSaved(inFile))
		return 2;
//		game.Render();
//		game.ProcessInput('a');
//		game.Render();
//		game.Update();
//		game.Render();
//		game.ProcessInput('p');
//		game.Update();
//		game.Update();
//		game.Update();
//		game.Update();
//		game.Update();
//		game.Update();
//		game.Update();
//		game.Update();
//		game.Update();
//		game.Render();
	
	return 0;
}

