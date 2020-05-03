/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CTower::CTower(int attackDamage, int attackSpeed, pos_t position, char ch, char type)
	: m_Char(ch),
	  m_Type(type),
	  m_Pos(position),
	  m_Range(3),
	  m_AttackDamage(attackDamage),
	  m_Frames(attackSpeed),
	  m_ArrowPos(pos::npos)
{}

CTower * CTower::Clone() const
{
	return new CTower(*this);
}

/**********************************************************************************************************************/
// LOADING
std::istream & CTower::LoadTemplate(std::istream & in)
{
	return in >> m_Char >> m_AttackDamage >> m_Frames;
}

std::istream & CTower::LoadOnMap(std::istream &in)
{
	int current;
	if (!(in >> m_Pos >> current))
		return in;
	m_Frames.SetCurrent(current);
	return in;
}

/**********************************************************************************************************************/
// SAVING
ostream & CTower::SaveTemplate(ostream &out) const
{
	return out << m_Type << ' ' << m_Char << ' ' << m_AttackDamage << ' ' << m_Frames;
}

ostream & CTower::SaveOnMap(ostream &out) const
{
	return out << m_Char << ' ' << m_Pos << ' ' << m_Frames.GetCurrent();
}

/**********************************************************************************************************************/
// ATTACK
void CTower::Attack(unordered_map<pos_t, CTile> & map, int rows, int cols, unordered_map<pos_t, CTrooper *> & troops)
{
	// return if frames are low
	if (!m_Frames.ActionAllowed())
		return;
	
	SpecialAttack(map, rows, cols, troops);
}

void CTower::SpecialAttack(unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper*> & troops)
{
	// if there is currently no arrow - make one and return
	if (m_ArrowPath.empty())
	{
		AssignArrow(map, rows, cols, troops);
		return;
	}
	
	// move arrow on its path
	ArrowMove(map);
	
	// if we have reached the trooper - damage it and remove the arrow (clear it's path)
	for (const auto & troop : troops)
		if (!TrooperDamaged(map, troop.second))
			map.insert({m_ArrowPos, CTile(' ', ETileType::BULLET,Colors::bg_red)});
}

void CTower::AssignArrow(unordered_map<pos_t, CTile> & map, int rows, int cols, unordered_map<pos_t, CTrooper*> & troops)
{
	size_t closest = m_Range;
	for (const auto & troop : troops)
	{
		if (m_Pos.Distance(troop.second->GetPosition()) > m_Range)
			continue;
		
		m_ArrowPath = CPath{map, rows, cols, m_Pos, troop.second->GetPosition()}
				.FindDiagonalPath();
		if (m_ArrowPath.size() <= closest)
			return;
	}
	m_ArrowPath = deque<pos_t>();
}

bool CTower::TrooperDamaged(unordered_map<pos_t, CTile> & map, CTrooper * trooper)
{
	if (m_ArrowPos != trooper->GetPosition())
		return false;
	ArrowClear(map);
	map.at(trooper->GetPosition()).SetColor(trooper->GetColor() + Colors::bg_red);
	trooper->ReceiveDamage(m_AttackDamage);
	return true;
}

void CTower::ArrowMove(unordered_map<pos_t,CTile> & map)
{
	if (map.count(m_ArrowPos))
		map.erase(m_ArrowPos);
	m_ArrowPos = m_ArrowPath.front();
	m_ArrowPath.pop_front();
}

void CTower::ArrowClear(unordered_map<pos_t,CTile> & map)
{
	if (map.count(m_ArrowPos))
		map.erase(m_ArrowPos);
	m_ArrowPath.clear();
	m_ArrowPos = pos::npos;
}