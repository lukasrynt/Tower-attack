/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */
#include "CUnitStack.hpp"

using namespace std;

CUnitStack::CUnitStack()
	: m_Selected(0)
{}

CUnitStack::~CUnitStack()
{
	for (auto & troop : m_Troops)
		delete troop.second;
	for (auto & tower : m_Towers)
		delete tower.second;
}

/**********************************************************************************************************************/
// LOADING
std::istream & CUnitStack::Load(char ch, std::istream &in)
{
	// prevent redefinition of units
	if (m_Troops.count(ch) || m_Towers.count(ch))
	{
		in.setstate(ios::failbit);
		return in;
	}
	
	// load coresponding unit
	switch (ch)
	{
		case '@':
			if (!LoadBasicTroop(in))
				return in;
			break;
		case '$':
			if (!LoadArmoredTroop(in))
				return in;
			break;
		case '*':
			if (!LoadArcherTower(in))
				return in;
			break;
		case '%':
			if (!LoadMageTower(in))
				return in;
			break;
		default:
			break;
	}
	return in;
}

istream & CUnitStack::LoadBasicTroop(istream & in)
{
	CTrooper * trooper = CTrooper::Load(in);
	if (!trooper)
	{
		in.setstate(ios::failbit);
		return in;
	}
	m_Troops.insert({'@', trooper});
	return in;
}

istream & CUnitStack::LoadArmoredTroop(istream & in)
{
	CArmoredTrooper * trooper = CArmoredTrooper::Load(in);
	if (!trooper)
	{
		in.setstate(ios::failbit);
		return in;
	}
	m_Troops.insert({'$', trooper});
	return in;
}

istream & CUnitStack::LoadArcherTower(istream & in)
{
	CTower * tower = CTower::Load(in);
	if (!tower)
	{
		in.setstate(ios::failbit);
		return in;
	}
	m_Towers.insert({'*', tower});
	return in;
}

istream & CUnitStack::LoadMageTower(istream & in)
{
	CMageTower * tower = CMageTower::Load(in);
	if (!tower)
	{
		in.setstate(ios::failbit);
		return in;
	}
	m_Towers.insert({'%', tower});
	return in;
}

bool CUnitStack::IsTowerChar(char ch) const
{
	for (const auto & tower : m_Towers)
		if (ch == tower.first)
			return true;
	return false;
}

bool CUnitStack::IsTrooperChar(char ch) const
{
	for (const auto & trooper : m_Troops)
		if (ch == trooper.first)
			return true;
	return false;
}

/**********************************************************************************************************************/
// INTERFACE
CTower * CUnitStack::CreateTowerAt(char ch) const
{
	if (m_Towers.count(ch))
		return m_Towers.at(ch)->Clone();
	return nullptr;
}

CTrooper * CUnitStack::CreateTrooperAt(char ch) const
{
	if (m_Troops.count(ch))
		return m_Troops.at(ch)->Clone();
	return nullptr;
}

CTrooper * CUnitStack::CreateSelected() const
{
	int idx = 0;
	for (const auto & troop : m_Troops)
	{
		if (idx++ == m_Selected)
			return troop.second->Clone();
	}
	return nullptr;
}

void CUnitStack::Render() const
{
	cout << endl << Colors::fg_cyan << string(3 * + m_Troops.size(), '-') << Colors::color_reset << endl;
	
	size_t idx = 0;
	for (const auto & troop : m_Troops)
	{
		if (idx++ == m_Selected)
			cout << Colors::bg_cyan;
		cout << *troop.second << Colors::color_reset << string(3, ' ');
	}
	
	cout << endl << Colors::fg_cyan << string(3 * + m_Troops.size(), '-') << Colors::color_reset << endl << endl;
}

void CUnitStack::Cycle() const
{
	if (++m_Selected == m_Troops.size())
		m_Selected = 0;
}

/**********************************************************************************************************************/
// SAVING
ostream & CUnitStack::Save(ostream & out) const
{
	for (auto & troop : m_Troops)
		if (!(troop.second->Save(out) << ';' << endl))
			return out;
	
	for (auto & tower : m_Towers)
		if (!(tower.second->Save(out) << ';' << endl))
			return out;
	return out;
}
