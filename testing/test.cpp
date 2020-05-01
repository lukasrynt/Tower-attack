#include <iostream>
#include <fstream>
#include <CGame.hpp>
#include "CWaves.hpp"
#include "CMap.hpp"
#include "CPath.hpp"

using namespace std;

istream & LoadSign(istream & in)
{
	char br1, br2, ch;
	if (!(in >> br1 >> ch >> br2)
		|| br1 != '('
		|| br2 != ')')
		in.setstate(ios::failbit);
	return in;
}

int main ()
{
	ifstream inFile("/home/lukas/School/PA2/semestralka/maps/test3.map");
	if (!inFile)
		return 1;

	CGame game;
	if (!(inFile >> game))
		return 2;
	
	game.Render();
	game.ProcessInput('2');
	game.ProcessInput('a');
	game.ProcessInput('2');
	game.ProcessInput('a');
	game.ProcessInput('p');
	game.Render();
	game.Update();
	game.Render();
	game.Update();
	game.Render();
	game.Update();
	game.Render();
	game.Update();
	game.Render();
	game.Update();
	game.Render();
	game.Update();
	game.Render();
	game.Update();
	game.Render();




//	LoadSign(inFile);
//	shared_ptr<CUnitStack> stack(new CUnitStack);
//	if (!(inFile >> *stack))
//		return 2;
//
//	LoadSign(inFile);
//	CMap map;
//	map.AssignUnitStack(stack);
//	if (!(inFile >> map))
//		return 3;
//
//	LoadSign(inFile);
//	CWaves waves;
//	waves.AssignUnitStack(stack);
//	if (!(inFile >> waves))
//		return 4;
	
	return 0;
}

