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

void CTrooper::ReceiveDamage(int damage, std::string bulletColor)
{
	m_Hp -= damage;
	// if it hasn't been hit by another turret recolor it
	if(m_BackColor.empty())
		m_BackColor = move(bulletColor);
}

CBuffer CTrooper::DrawShortInfo(size_t width) const
{
	return move(CBuffer{width})
		.Append("Attack: ", Colors::FG_CYAN).AddText(to_string(m_Attack))
		.Append("Speed: ", Colors::FG_CYAN).AddText(to_string(m_Frames.GetSpeed()))
		.Append("Cost: ", Colors::FG_CYAN).AddText(to_string(m_Price)).AddText(" C ", Colors::FG_YELLOW)
		.Append("HP: ", Colors::FG_CYAN).AddText(to_string(m_Hp));
}

/**********************************************************************************************************************/
// LOADING
istream & CTrooper::LoadOnMap(istream & in)
{
	int current = 0;
	in >> m_Pos >> current >> m_SpawnIdx >> m_Hp;
	m_Frames.SetCurrent(current);
	return in;
}

std::istream & CTrooper::LoadTemplate(istream & in)
{
	
	in >> m_Char >> m_Attack >> m_Frames >> m_Price >> m_Hp;
	if (m_Price <= 0
		|| m_Hp <= 0
		|| m_Attack <= 0)
		throw invalid_format(("Some of the trooper"s + m_Char + "template attributes are zero or negative.").c_str());
	return in;
}


