/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CApplication.hpp"

using namespace std;

int main()
{
	// run the game
	try
	{
		CApplication{CInterface{cout}}.Run();
	}
	// catch exceptions related to I/O
	catch (const ios::failure & e)
	{
		cerr << e.what() << endl;
		return 66;
	}
	return 0;
}