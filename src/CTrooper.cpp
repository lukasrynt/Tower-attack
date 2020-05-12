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
void CTrooper::Spawn(unordered_map<pos_t,shared_ptr<CTile>> & map)
{
	pos_t target = m_Path.front();
	if (!map.count(target))
		map.emplace(target, this);
	m_Pos = target;
	m_Path.pop_front();
}

void CTrooper::ReceiveDamage(int damage)
{
	m_Hp -= damage;
}

bool CTrooper::Move(unordered_map<pos_t,shared_ptr<CTile>> & map)
{
	// move according to current frame
	if (!m_Frames.ActionAllowed() || m_Path.empty())
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
		if (map.count(m_Pos))
			map.erase(m_Pos);
		return true;
	}
	
	// move
	m_Path.pop_front();
	if (map.count(m_Pos))
		map.erase(m_Pos);
	m_Pos = target;
	map.emplace(m_Pos, this);
	return false;
}

CBuffer CTrooper::CreateInfoBuffer(int windowWidth) const
{
	return move(CBuffer{windowWidth}
						.Append("   ").Append("("s + m_Char + ")", string(Colors::BG_YELLOW) + Colors::FG_BLACK)
						.AddEscapeSequence(Colors::FG_YELLOW)
						.Append("\tHP: " + to_string(m_Hp))
						.Append("\tSpeed: " + to_string(m_Frames.GetSpeed()))
						.Append("\tAttack: " + to_string(m_Attack))
						.Append("\tCost: " + to_string(m_Price) + " ©")
		.AddEscapeSequence(Colors::RESET));
}

/**********************************************************************************************************************/
// LOADING
istream & CTrooper::LoadTemplate(istream & in)
{
	return in >> m_Char >> m_Price >> m_Hp >> m_Frames >> m_Attack;
}

istream & CTrooper::LoadOnMap(istream & in)
{
	int current;
	in >> m_Pos >> current;
	m_Frames.SetCurrent(current);
	return in;
}

/**********************************************************************************************************************/
// SAVING
ostream & CTrooper::SaveTemplate(ostream & out) const
{
	return out << m_Type << ' ' << m_Char << ' ' << m_Price << ' ' << m_Hp << ' ' << m_Frames << ' ' << m_Attack;
}

ostream & CTrooper::SaveOnMap(ostream & out) const
{
	return out << m_Char << ' ' << m_Pos << ' ' << m_Frames.GetCurrent();
}
