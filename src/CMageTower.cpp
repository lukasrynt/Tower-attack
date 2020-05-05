/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CMageTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CMageTower::CMageTower(int attackDamage, int attackSpeed, int range, int waveSpeed, pos_t position)
	: CTower(attackDamage, attackSpeed, range, position, {'%', ETileType::MAGE_TOWER, Colors::BG_BLUE}),
	  m_WaveStatus(waveSpeed),
	  m_WaveLevel(0)
{}

/**********************************************************************************************************************/
// LOADING
istream & CMageTower::LoadTemplate(istream & in)
{
	return CTower::LoadTemplate(in) >> m_WaveStatus;
}

istream & CMageTower::LoadOnMap(istream & in)
{
	int curr;
	if (!(CTower::LoadOnMap(in) >> curr))
		return in;
	m_Frames.SetCurrent(curr);
	return in;
}

/**********************************************************************************************************************/
// SAVING
ostream & CMageTower::SaveTemplate(ostream & out) const
{
	return CTower::SaveTemplate(out) << ' ' << m_WaveStatus;
}

ostream & CMageTower::SaveOnMap(ostream & out) const
{
	return CTower::SaveOnMap(out) << ' ' << m_WaveStatus.GetCurrent();
}

/**********************************************************************************************************************/
// ATTACK
bool CMageTower::Attack(unordered_map<pos_t, CTile> & map, int, int, unordered_map<pos_t, CTrooper*> & troops)
{
	// if the wave hasn't started yet - either create it or return if the perimeter wasn't breached
	// or the action is not allowed
	if (!m_WaveLevel)
	{
		if (!m_Frames.ActionAllowed() || !PerimeterBreached(troops))
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
	
	if (m_WaveLevel++ == m_Range + 1)
		m_WaveLevel = 0;
	return true;
}

void CMageTower::ClearLastWave(unordered_map<pos_t, CTile> & map) const
{
	for (const auto & pos : m_LastWave)
		if (map.count(pos) && map.at(pos).IsBullet())
			map.erase(pos);
}

bool CMageTower::PerimeterBreached(const unordered_map<pos_t, CTrooper*> & troops) const
{
	for (const auto & troop : troops)
		if (m_Pos.Distance(troop.second->GetPosition()) < m_Range)
			return true;
	return false;
}

void CMageTower::SendNewWave(std::unordered_map<pos_t, CTile> & map, std::unordered_map<pos_t, CTrooper*> & troops)
{
	m_LastWave = m_Pos.GetRadius(m_WaveLevel);
	for (const auto & pos : m_LastWave)
	{
		if (!map.count(pos))
			map.insert({pos, {' ', ETileType::BULLET, Colors::BG_BLUE}});
		else if (map.at(pos).IsTroop())
		{
			troops.at(pos)->ReceiveDamage(m_AttackDamage);
			map.at(pos).SetColor(map.at(pos).GetColor() + Colors::BG_BLUE);
		}
	}
}
