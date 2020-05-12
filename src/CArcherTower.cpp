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
	for (const auto & troop : troops)
	{
		if (m_Pos.Distance(troop.second->GetPosition()) > m_Range)
			continue;
		
		m_ArrowPath = CPath{map, rows, cols, m_Pos, troop.second->GetPosition()}
				.FindDiagonalPath();
		if (m_ArrowPath.size() <= closest)
			return true;
	}
	m_ArrowPath.clear();
	return false;
}

void CArcherTower::DamageTrooper(unordered_map<pos_t, shared_ptr<CTrooper>> & troops)
{
	if (!troops.count(m_ArrowPos))
		return;
	auto troop = troops.at(m_ArrowPos);
	troop->ReceiveDamage(m_AttackDamage);	// TODO should implement recolor
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

CBuffer CArcherTower::CreateInfoBuffer(int windowWidth) const
{
	return move(CBuffer{windowWidth}
						.Append("   ").Append("("s + m_Char + ")", string(Colors::BG_RED) + Colors::FG_BLACK)
						.Append("\tAttack damage: " + to_string(m_AttackDamage), Colors::FG_RED)
						.Append("\tAttack speed: " + to_string(m_AttackSpeed.GetSpeed()), Colors::FG_RED)
						.Append("\tRange: " + to_string(m_Range), Colors::FG_RED));
}
