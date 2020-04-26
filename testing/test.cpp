#include <iostream>
#include <fstream>
#include <ExInvalidFile.hpp>
#include <CGame.hpp>
#include "CWaves.hpp"
#include "CMap.hpp"
#include "CPath.hpp"

using namespace std;

int main ()
{
	CGame game{};
	try
	{
		game.Load("/home/lukas/School/PA2/semestralka/maps/new1.map");
		game.Render();
		game.ProcessInput('a');
		game.Render();
		game.Update();
		game.Render();
		game.ProcessInput('p');
		game.Update();
		game.Update();
		game.Update();
		game.Update();
		game.Update();
		game.Update();
		game.Update();
		game.Update();
		game.Update();
		game.Render();
	}
	catch (runtime_error & e)
	{
		cout << e.what();
	}
	
	
	return 0;
}

