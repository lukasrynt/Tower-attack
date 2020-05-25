#include <iostream>
#include <fstream>
#include <CGame.hpp>
#include <cstring>
#include <iomanip>
#include <memory>
#include <sstream>
#include <CInterface.hpp>
#include "CWaves.hpp"
#include "CMap.hpp"
#include "CPath.hpp"

using namespace std;

int main ()
{

	ifstream inFile("/home/lukas/School/PA2/semestralka/maps/test2.map");
	inFile.exceptions(ios::failbit | ios::badbit | ios::eofbit);

	CGame game;
	try
	{
		inFile >> game;
	}
	catch(const ios::failure &)
	{
		if (!inFile.eof())
			return 2;
	}
	if (!game.CheckNew())
		return 3;
	cout << game.CreateBuffer(150);
	game.AddTroop();
	game.ReleaseWaves();
//	game.Update();
//	for (int i = 0; i < 5; ++i)
//		game.AddTroop();
//	game.ReleaseWaves();
	cout << game.CreateBuffer(150);
	for (int i = 0; i < 60; ++i)
	{
		cout << game.CreateBuffer(150);
		game.Update();
	}

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

