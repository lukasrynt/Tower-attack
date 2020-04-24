#include <iostream>
#include <fstream>
#include "CWaves.hpp"
#include "CMap.hpp"
#include "CPath.hpp"

using namespace std;

int main ()
{
	CMap map;
	ifstream in("maps/test5.map");
	in >> map;
	CWaves stack = CWaves(map.GetSpawnCnt());
	stack.AddTroop();
	stack.AddTroop();
	stack.AddTroop();
	
	
	stack.ReleaseWave();
	bool waveon = true;
	auto trooper = stack.Update(waveon);
	map.Update(trooper, waveon);
	cout << map;
	trooper = stack.Update(waveon);
	map.Update(trooper, waveon);
	cout << map;
	trooper = stack.Update(waveon);
	map.Update(trooper, waveon);
	cout << map;
	
	
	return 0;
}

