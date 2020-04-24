/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CTower::CTower(int attackDamage, int attackSpeed, pos_t position)
	: m_Pos(position),
	  m_AttackDamage(attackDamage),
	  m_Frames(attackSpeed),
	  m_ArrowPos(pos_t::npos),
	  m_Perimeter(position.GetRadius(3))
{}

CTower * CTower::Clone()
{
	return new CTower(*this);
}

/**********************************************************************************************************************/
// ATTACK

pos_t CTower::PerimeterBreached(std::unordered_map<pos_t, CTile> & map)
{
	// iterate the layers of perimeter and look for trooper
	for (const auto & layer : m_Perimeter)
	{
		for (const auto & pos : layer)
		{
			if (map.count(pos) && map.at(pos).IsTroop())
				return pos;
		}
	}
	return pos_t::npos;
}

void CTower::Attack(unordered_map<pos_t, CTile> & map, int rows, int cols, unordered_map<pos_t, CTrooper *> & troops)
{
	// return if frames are low
	if (!m_Frames.ActionAllowed())
		return;
	
	// if perimeter hasn't been breached we want to erase current arrow
	pos_t target;
	if ((target = PerimeterBreached(map)) == pos_t::npos)
	{
		ArrowClear(map);
		return;
	}
	
	// if there is currently no arrow - create one
	if (m_ArrowPath.empty() && troops.count(target))
	{
		CPath path = CPath(map, rows, cols, m_Pos, target);
		m_ArrowPath = path.FindPath();
	}
	
	// move arrow on its path
	ArrowMove(map);
	
	// if we have reached the trooper - damage it and remove the arrow (clear it's path)
	if (m_ArrowPos == target)
	{
		m_ArrowPath.clear();
		troops.at(target)->ReceiveDamage(m_AttackDamage);
	}
	else
		map.insert({m_ArrowPos, CTile(' ', ETileType::BULLET)});
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
}