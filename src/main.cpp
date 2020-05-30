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
	// won't happen - it is here only in case something goes wrong,
	// because if that happens we at least want to return terminal to original settings
	catch (const exception & e)
	{
		cerr << e.what() << endl;
		return 89;
	}
	return 0;
}