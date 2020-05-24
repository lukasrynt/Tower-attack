/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTrooper.hpp"

#include <utility>

#include "escape_sequences.h"

using namespace std;
/**********************************************************************************************************************/
// ACTIONS
void CTrooper::Spawn()
{
	m_Pos = m_Path.front();
	m_Path.pop_front();
}

bool CTrooper::Move(const unordered_map<pos_t,shared_ptr<CTile>> & map, bool & emplace, bool & erase)
{
	emplace = erase = false;
	
	// move according to current frame
	if (m_Path.empty())
		return false;
	
	// move the trooper on the next tile
	pos_t target = m_Path.front();
	
	// if there is trooper in the way, wait till he's gone
	if (map.count(target) && map.at(target)->IsTroop())
		return false;
	
	// if we reached gate return true
	if (target == m_Path.back())
	{
		m_Path.pop_front();
		erase = true;
		return true;
	}
	
	// move
	m_Path.pop_front();
	m_Pos = target;
	erase = true;
	emplace = true;
	return false;
}

/**********************************************************************************************************************/
// LOADING
istream & CTrooper::LoadOnMap(istream & in)
{
	int current;
	in >> m_Pos >> current;
	m_Frames.SetCurrent(current);
	return in;
}