/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 08/05/2020.
*/

#include "CArcherTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// ATTACK
bool CArcherTower::Attack(unordered_map<pos_t, shared_ptr<CTile>> & map, unordered_map<pos_t, shared_ptr<CTrooper>> & troops, int rows, int cols)
{
	// if there is currently no arrow - make one and return
	if (m_ArrowPath.empty())
	{
		if (!m_AttackSpeed.ActionAllowed())
			return false;
		if (m_ArrowPos != pos::npos)
			ArrowClear(map);
		if (!AssignArrow(map, troops, rows, cols))
			return false;
	}
	
	// move arrow on its path
	if (ArrowMove(map))
	{
		DamageTrooper(troops);
		return true;
	}
	map.emplace(m_ArrowPos, new CTile(' ', ETileType::BULLET, Colors::BG_RED));
	return false;
}

bool CArcherTower::AssignArrow(unordered_map<pos_t, shared_ptr<CTile>> & map, const unordered_map<pos_t, shared_ptr<CTrooper>> & troops, int rows, int cols)
{
	size_t closest = m_Range;
	deque<pos_t> path;
	for (const auto & troop : troops)
	{
		// check that the trooper is within range
		double distance = m_Pos.Distance(troop.second->GetPosition());
		if (distance > closest)
			continue;
		
		// find path from tower to trooper
		path = CPath{map, rows, cols, m_Pos, troop.second->GetPosition()}
				.FindDiagonalPath();
		if (path.size() > closest)
			continue;
		
		// if the path to target is closer than it was - it will be our next candidate
		if (m_ArrowPath.size() <= closest)
		{
			closest = distance;
			m_ArrowPath = path;
		}
	}
	
	// check that there is some path found - it can happen that the closest trooper cannot be reached
	return !m_ArrowPath.empty();
}

void CArcherTower::DamageTrooper(unordered_map<pos_t, shared_ptr<CTrooper>> & troops)
{
	auto troop = troops.at(m_ArrowPos);
	m_ArrowPath.clear();
	m_ArrowPos = pos::npos;
	troop->ReceiveDamage(m_AttackDamage, m_BackColor);
}

bool CArcherTower::ArrowMove(unordered_map<pos_t,shared_ptr<CTile>> & map)
{
	if (map.count(m_ArrowPos) && m_ArrowPos != pos::npos)
		map.erase(m_ArrowPos);
	m_ArrowPos = m_ArrowPath.front();
	m_ArrowPath.pop_front();
	return map.count(m_ArrowPos) && map.at(m_ArrowPos)->IsTroop();
}

void CArcherTower::ArrowClear(unordered_map<pos_t,shared_ptr<CTile>> & map)
{
	if (map.count(m_ArrowPos))
		map.erase(m_ArrowPos);
	m_ArrowPath.clear();
	m_ArrowPos = pos::npos;
}

CBuffer CArcherTower::DrawHelpInfo(size_t width) const
{
	return move(CBuffer{width}
		.Append("   ").Append("("s + m_Char + ")", string(Colors::BG_RED) + Colors::FG_BLACK)
		.Append("\tAttack damage: " + to_string(m_AttackDamage), Colors::FG_RED)
		.Append("\tAttack speed: " + to_string(m_AttackSpeed.GetSpeed()), Colors::FG_RED)
		.Append("\tRange: " + to_string(m_Range), Colors::FG_RED));
}

