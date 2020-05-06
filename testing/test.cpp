#include <iostream>
#include <fstream>
#include <CGame.hpp>
#include <cstring>
#include <iomanip>
#include <sstream>
#include "CWaves.hpp"
#include "CMap.hpp"
#include "CPath.hpp"

using namespace std;

int main ()
{
	ifstream inFile("/home/lukas/School/PA2/semestralka/saves/ahoj.sav");
	if (!inFile)
		return 1;

	CGame game;
	if (!(inFile >> game))
		return 2;
//	game.Render(cout);
//	for (int i = 0; i < 60; ++i)
//	{
//		game.Render(cout);
//		game.Update();
//	}

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

