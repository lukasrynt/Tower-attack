/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTower.hpp"

#include <utility>

using namespace std;

istream & CTower::LoadOnMap(istream & in) {
	int current;
	in >> m_Pos >> current;
	m_AttackSpeed.SetCurrent(current);
	return in;
}