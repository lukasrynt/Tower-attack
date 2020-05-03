/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTrooper.hpp"

#include <utility>

#include "Colors.hpp"

using namespace std;
/**********************************************************************************************************************/
// INIT
CTrooper::CTrooper(int hp, int speed, int attack, CTile tile)
	: m_Tile(std::move(tile)),
	  m_Pos(pos::npos),
	  m_Hp(hp),
	  m_Attack(attack),
	  m_Frames(speed),
	  m_SpawnIdx(0)

{}

CTrooper * CTrooper::Clone() const
{
	return new CTrooper(*this);
}

/**********************************************************************************************************************/
// ACTIONS
void CTrooper::Spawn(unordered_map<pos_t,CTile> & map)
{
	pos_t target = m_Path.front();
	if (!map.count(target))
		map.insert({target, GetTile()});
	m_Pos = target;
	m_Path.pop_front();
}

void CTrooper::ReceiveDamage(int damage)
{
	m_Hp -= damage;
}

bool CTrooper::Move(unordered_map<pos_t,CTile> & map)
{
	// move according to current frame
	if (!m_Frames.ActionAllowed() || m_Path.empty())
		return false;
	
	// move the trooper on the next tile
	pos_t target = m_Path.front();
	
	// if there is trooper in the way, wait till he's gone
	if (map.count(target) && map.at(target).IsTroop())
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
	map.insert({m_Pos, GetTile()});
	return false;
}

int CTrooper::Attack() const
{
	return m_Attack;
}

std::ostream & CTrooper::RenderInfo(std::ostream & out) const
{
	return out << m_Tile.m_Color
		<< "Basic trooper, hp: " << m_Hp
		<< ", speed: " << m_Frames
		<< ", attack: " << m_Attack << "."
		<< Colors::color_reset << endl;
}

/**********************************************************************************************************************/
// LOADING
istream & CTrooper::LoadTemplate(istream & in)
{
	return in >> m_Tile.m_Char >> m_Hp >> m_Frames >> m_Attack;
}

istream & CTrooper::LoadOnMap(istream & in)
{
	int current;
	if (!(in >> m_Pos >> current))
		return in;
	m_Frames.SetCurrent(current);
	return in;
}

/**********************************************************************************************************************/
// SAVING
ostream & CTrooper::SaveTemplate(ostream & out) const
{
	return out << m_Tile.m_Type << ' ' << m_Tile.m_Char << ' ' << m_Hp << ' ' << m_Frames << ' ' << m_Attack;
}

ostream & CTrooper::SaveOnMap(ostream & out) const
{
	return out << m_Tile.m_Char << ' ' << m_Pos << ' ' << m_Frames.GetCurrent();
}
