/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTrooper.hpp"

#include "Colors.hpp"

using namespace std;
/**********************************************************************************************************************/
// INIT
CTrooper::CTrooper(int hp, int speed, int attack, char ch, char type)
	: m_Char(ch),
	  m_Type(type),
	  m_Pos(pos_t::npos),
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

/**********************************************************************************************************************/
// LOADING
CTrooper * CTrooper::LoadTemplate(istream & in)
{
	CTrooper * trooper = new CTrooper();
	char del;
	if (!(in >> trooper->m_Char >> trooper->m_Hp >> trooper->m_Frames >> trooper->m_Attack >> del)
		|| del != ';')
		return nullptr;
	return trooper;
}

istream & CTrooper::LoadOnMap(istream & in)
{
	char del;
	if (!(LoadOnMapTroop(in) >> del)
		|| del != ';')
		in.setstate(ios::failbit);
	return in;
}

istream & CTrooper::LoadOnMapTroop(istream & in)
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
	return SaveTemplateTroop(out) << ';' << endl;
}

ostream & CTrooper::SaveTemplateTroop(ostream & out) const
{
	return out << m_Type << ' ' << m_Char << ' ' << m_Hp << ' ' << m_Frames << ' ' << m_Attack;
}

ostream & CTrooper::SaveOnMap(std::ostream & out) const
{
	return SaveOnMapTroop(out) << ';' << endl;
}

ostream & CTrooper::SaveOnMapTroop(ostream & out) const
{
	return out << m_Char << ' ' << m_Pos << ' ' << m_Frames.GetCurrent();
}
