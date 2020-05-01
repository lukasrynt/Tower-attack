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
	  m_AttackDamage(attackDamage),
	  m_Frames(attackSpeed),
	  m_ArrowPos(pos_t::npos)
{}

CTower * CTower::Clone() const
{
	return new CTower(*this);
}


/**********************************************************************************************************************/
// LOADING
CTower * CTower::LoadTemplate(std::istream & in)
{
	CTower * tower = new CTower();
	char del;
	if (!(in >> tower->m_Char >> tower->m_AttackDamage >> tower->m_Frames >> del)
		|| del != ';')
		return nullptr;
	return tower;
}

std::istream & CTower::LoadOnMap(std::istream &in)
{
	char del;
	if (!(LoadOnMapTower(in) >> del)
		|| del != ';')
		in.setstate(ios::failbit);
	return in;
}

std::istream & CTower::LoadOnMapTower(std::istream &in)
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
	return SaveTemplateTower(out) << ';' << endl;
}

ostream & CTower::SaveTemplateTower(ostream &out) const
{
	return out << m_Type << ' ' << m_Char << ' ' << m_AttackDamage << ' ' << m_Frames;
}

ostream & CTower::SaveOnMap(ostream &out) const
{
	return SaveOnMapTower(out) << ';' << endl;
}

ostream & CTower::SaveOnMapTower(ostream &out) const
{
	return out << m_Char << ' ' << m_Pos << ' ' << m_Frames.GetCurrent();
}

/**********************************************************************************************************************/
// ATTACK

pos_t CTower::PerimeterBreached(std::unordered_map<pos_t, CTile> & map)
{
	return pos_t::npos;
}

void CTower::Attack(unordered_map<pos_t, CTile> & map, int rows, int cols, unordered_map<pos_t, CTrooper *> & troops)
{
	// return if frames are low
	if (!m_Frames.ActionAllowed())
		return;
	
	SpecialAttack(map, rows, cols, troops);
}

void CTower::SpecialAttack(std::unordered_map<pos_t, CTile> &map, int rows, int cols, std::unordered_map<pos_t, CTrooper*> & troops)
{
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
		map.insert({m_ArrowPos, CTile(' ', ETileType::BULLET)});}

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