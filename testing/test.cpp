#include <iostream>
#include <fstream>
#include <ExInvalidFile.hpp>
#include "CWaves.hpp"
#include "CMap.hpp"
#include "CPath.hpp"

using namespace std;

int main ()
{
	shared_ptr <CUnitStack> unitStack(new CUnitStack);
	CMap map(unitStack);
	ifstream in("/home/lukas/School/PA2/semestralka/maps/test1.map");
	if (!in)
		return 1;
	map.SetMapDimensions(5,5);
	map.SetGateHealth(200);
	try
	{
		unitStack->LoadUnitSpecifications({12, 12}, '*');
		unitStack->LoadUnitSpecifications({12, 12, 28}, '@');
		map.LoadMap(in, true);
		unitStack->Render();
		map.Render();
	}
	catch (invalid_file & e)
	{
		cout << e.what();
	}
	
	
	return 0;
}

