/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CMageTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// ATTACK
bool CMageTower::Attack(unordered_map<pos_t, shared_ptr<CTile>> & map, unordered_map<pos_t, shared_ptr<CTrooper>> & troops, int rows, int cols)
{
	// if the wave hasn't started yet - either create it or return if the perimeter wasn't breached
	// or the action is not allowed
	(void) rows;	// not necessary because we don't need to know the path from one point to another
	(void) cols;
	if (!m_WaveLevel)
	{
		if (!m_AttackSpeed.ActionAllowed() || !PerimeterBreached(troops))
		{
			ClearLastWave(map);
			m_LastWave = deque<pos_t>();
			return false;
		}
		else
			m_WaveLevel++;
	}
	
	ClearLastWave(map);
	SendNewWave(map, troops);
	
	if (m_WaveLevel++ > m_Range)
		m_WaveLevel = 0;
	return true;
}

void CMageTower::ClearLastWave(unordered_map<pos_t, shared_ptr<CTile>> & map) const
{
	for (const auto & pos : m_LastWave)
		if (map.count(pos) && map.at(pos)->IsBullet())
			map.erase(pos);
}

bool CMageTower::PerimeterBreached(const unordered_map<pos_t, shared_ptr<CTrooper>> & troops) const
{
	for (const auto & troop : troops)
		if (m_Pos.Distance(troop.second->GetPosition()) <= m_Range)
			return true;
	return false;
}

void CMageTower::SendNewWave(unordered_map<pos_t, shared_ptr<CTile>> & map, unordered_map<pos_t, shared_ptr<CTrooper>> & troops)
{
	m_LastWave = m_Pos.GetRadius(m_WaveLevel);
	for (const auto & pos : m_LastWave)
	{
		if (!map.count(pos))
			map.emplace(pos, new CTile{' ', ETileType::BULLET, Colors::BG_BLUE});
		else if (map.at(pos)->IsTroop())
			troops.at(pos)->ReceiveDamage(m_AttackDamage, m_BackColor);
	}
}

CBuffer CMageTower::DrawHelpInfo(size_t width) const
{
	return move(CBuffer{width}
						.Append("   ").Append("("s + m_Char + ")", string(Colors::BG_BLUE) + Colors::FG_BLACK)
						.Append("\tAttack damage: " + to_string(m_AttackDamage), Colors::FG_BLUE)
						.Append("\tAttack speed: " + to_string(m_AttackSpeed.GetSpeed()), Colors::FG_BLUE)
						.Append("\tRange: " + to_string(m_Range), Colors::FG_BLUE));
}
