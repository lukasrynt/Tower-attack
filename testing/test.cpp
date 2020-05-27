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
#include "ExInvalidFormat.hpp"

using namespace std;

int main ()
{

	ifstream inFile("examples/maps/test5.map");
	inFile.exceptions(ios::failbit | ios::badbit);

	CGame game;
	try
	{
		inFile >> game;
	}
	catch(const ios_base::failure &)
	{
		if (!inFile.eof())
			return 2;
	}
	catch(const invalid_format & e)
	{
		cerr << e.what();
		return 5;
	}
	if (!game.CheckSaved())
		return 3;
	cout << game.Draw(150);
//	game.AddTroop();
//	game.ReleaseWaves();
//	game.Update();
//	for (int i = 0; i < 5; ++i)
//		game.AddTroop();
//	game.ReleaseWaves();
//	cout << game.Draw(150);
	for (int i = 0; i < 60; ++i)
	{
		cout << game.Draw(150);
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

