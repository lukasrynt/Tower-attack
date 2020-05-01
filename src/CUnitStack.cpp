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
istream & operator>>(istream & in, CUnitStack & stack)
{
	while (true)
	{
		// load first character - type of unit
		char ch;
		if (!(in >> ch))
			return in;
		
		// load coresponding unit
		if(!stack.LoadUnit(in, ch))
			break;
	}
	return in;
}

bool CUnitStack::LoadUnit(istream & in, char ch)
{
	switch (ch)
	{
		case 'T':
			if (!LoadBasicTroop(in))
				return false;
			break;
		case 'A':
			if (!LoadArmoredTroop(in))
				return false;
			break;
		case 'R':
			if (!LoadArcherTower(in))
				return false;
			break;
		case 'M':
			if (!LoadMageTower(in))
				return false;
			break;
		default:
			in.putback(ch);
			return false;
	}
	return true;
}

istream & CUnitStack::LoadBasicTroop(istream & in)
{
	CTrooper * trooper = CTrooper::LoadTemplate(in);
	if (!trooper || !CharIsValid(trooper->GetChar()))
	{
		in.setstate(ios::failbit);
		return in;
	}
	m_Troops.insert({trooper->GetChar(), trooper});
	return in;
}

istream & CUnitStack::LoadArmoredTroop(istream & in)
{
	CArmoredTrooper * trooper = CArmoredTrooper::LoadTemplate(in);
	if (!trooper || !CharIsValid(trooper->GetChar()))
	{
		in.setstate(ios::failbit);
		return in;
	}
	m_Troops.insert({trooper->GetChar(), trooper});
	return in;
}

istream & CUnitStack::LoadArcherTower(istream & in)
{
	CTower * tower = CTower::LoadTemplate(in);
	if (!tower || !CharIsValid(tower->GetChar()))
	{
		in.setstate(ios::failbit);
		return in;
	}
	m_Towers.insert({tower->GetChar(), tower});
	return in;
}

istream & CUnitStack::LoadMageTower(istream & in)
{
	CMageTower * tower = CMageTower::LoadTemplate(in);
	if (!tower || !CharIsValid(tower->GetChar()))
	{
		in.setstate(ios::failbit);
		return in;
	}
	m_Towers.insert({tower->GetChar(), tower});
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

bool CUnitStack::CharIsValid(char ch) const
{
	if (m_Troops.count(ch) || m_Towers.count(ch))
		return false;
	string forbidden(FORBIDDEN_CHARS);
	return forbidden.find(ch) == string::npos;
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
	size_t idx = 0;
	for (const auto & troop : m_Troops)
	{
		if (idx++ == m_Selected)
			return troop.second->Clone();
	}
	return nullptr;
}

void CUnitStack::Render() const
{
	cout << endl << Colors::fg_cyan << string(4 * m_Troops.size(), '-') << Colors::color_reset << endl;
	
	size_t idx = 0;
	for (const auto & troop : m_Troops)
	{
		cout << ' ';
		if (idx++ == m_Selected)
			cout << Colors::bg_cyan;
		cout << *troop.second << Colors::color_reset << string(2, ' ');
	}
	
	cout << endl << Colors::fg_cyan << string(4 * m_Troops.size(), '-') << Colors::color_reset << endl << endl;
}

void CUnitStack::Cycle() const
{
	if (++m_Selected == m_Troops.size())
		m_Selected = 0;
}

/**********************************************************************************************************************/
// SAVING
ostream & operator<<(ostream & out, const CUnitStack & stack)
{
	if (!(out << "(U)" << endl))
		return out;
	for (auto & troop : stack.m_Troops)
		if (!troop.second->SaveTemplate(out))
			return out;
	
	for (auto & tower : stack.m_Towers)
		if (!tower.second->SaveTemplate(out))
			return out;
	return out << endl;
}
