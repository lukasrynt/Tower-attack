/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CTower.hpp"

#include <utility>

using namespace std;

/**********************************************************************************************************************/
// INIT
CTower::CTower(int attackDamage, int attackSpeed, pos_t position, CTile  tile)
	: m_Tile(move(tile)),
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
	return in >> m_Tile.m_Char >> m_AttackDamage  >>  m_Frames;
}

std::istream & CTower::LoadOnMap(std::istream & in)
{
	int current;
	if (!(in >> m_Pos >> current))
		return in;
	m_Frames.SetCurrent(current);
	return in;
}

/**********************************************************************************************************************/
// SAVING
ostream & CTower::SaveTemplate(ostream & out) const
{
	return out << GetType() << ' ' << GetChar() << ' ' << m_AttackDamage << ' ' << m_Frames;
}

ostream & CTower::SaveOnMap(ostream & out) const
{
	return out << GetChar() << ' ' << m_Pos << ' ' << m_Frames.GetCurrent();
}

/**********************************************************************************************************************/
// ATTACK
bool CTower::Attack(unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper*> & troops)
{
	// if there is currently no arrow - make one and return
	if (m_ArrowPath.empty())
	{
		if (!m_Frames.ActionAllowed())
			return false;
		if (m_ArrowPos != pos::npos)
			ArrowClear(map);
		if (!AssignArrow(map, rows, cols, troops))
			return false;
	}
	
	// move arrow on its path
	if (ArrowMove(map))
	{
		DamageTrooper(map, troops);
		return true;
	}
	map.insert({m_ArrowPos, CTile(' ', ETileType::BULLET, Colors::bg_red)});
	return false;
}

bool CTower::AssignArrow(unordered_map<pos_t, CTile> & map, int rows, int cols, const unordered_map<pos_t, CTrooper*> & troops)
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
	m_ArrowPath = deque<pos_t>();
	return false;
}

void CTower::DamageTrooper(unordered_map<pos_t, CTile> & map, unordered_map<pos_t, CTrooper*> & troops)
{
	if (!troops.count(m_ArrowPos))
		return;
	auto troop = troops.at(m_ArrowPos);
	cout << troop->GetPosition() << " received " << m_AttackDamage;
	if (troop->Died())
		cout << "and died" << endl;
	cout << endl;
	troop->ReceiveDamage(m_AttackDamage);
	map.at(troop->GetPosition()).m_Color = troop->GetTile().m_Color + Colors::bg_red;
}

bool CTower::ArrowMove(unordered_map<pos_t,CTile> & map)
{
	if (map.count(m_ArrowPos) && m_ArrowPos != pos::npos)
		map.erase(m_ArrowPos);
	m_ArrowPos = m_ArrowPath.front();
	m_ArrowPath.pop_front();
	return map.count(m_ArrowPos) && map.at(m_ArrowPos).IsTroop();
}

void CTower::ArrowClear(unordered_map<pos_t,CTile> & map)
{
	if (map.count(m_ArrowPos))
		map.erase(m_ArrowPos);
	m_ArrowPath.clear();
	m_ArrowPos = pos::npos;
}