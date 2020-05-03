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
	map<char,CTrooper*> origTroops;
	map<char,CTower*> origTowers;
	CUnitStack::CreateOriginals(origTroops, origTowers);
	while (true)
	{
		// load first character - type of unit
		char ch;
		if (!(in >> ch))
			return in;
		
		// load coresponding unit
		if(!stack.LoadUnit(in, ch, origTroops, origTowers))
			break;
	}
	CUnitStack::DeleteOriginals(origTroops, origTowers);
	return in;
}

void CUnitStack::CreateOriginals (map<char,CTrooper*> & origTroops, map<char,CTower*> & origTowers)
{
	origTroops.insert({'T', new CTrooper()});
	origTroops.insert({'A', new CArmoredTrooper()});
	origTowers.insert({'R', new CTower()});
	origTowers.insert({'M', new CMageTower()});
}

void CUnitStack::DeleteOriginals(std::map<char, CTrooper*> & origTroops, std::map<char, CTower*> & origTowers)
{
	for (auto & trooper : origTroops)
		delete trooper.second;
	for (auto & tower : origTowers)
		delete tower.second;
}

bool CUnitStack::LoadUnit(istream & in, char ch, const map<char,CTrooper*> & origTroops, const map<char,CTower*> & origTowers)
{
	if (origTroops.count(ch))
	{
		CTrooper * trooper = origTroops.at(ch)->Clone();
		if (!trooper->LoadTemplate(in))
			return false;
		m_Troops.insert({trooper->GetChar(), trooper});
		if (!CharIsValid(trooper->GetChar()))
			return false;
	}
	else if (origTowers.count(ch))
	{
		CTower * tower = origTowers.at(ch)->Clone();
		if (!tower->LoadTemplate(in))
			return false;
		m_Towers.insert({tower->GetChar(), tower});
		if (!CharIsValid(tower->GetChar()))
			return false;
	}
	else
	{
		in.putback(ch);
		return false;
	}
	return true;
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

bool CUnitStack::Check() const
{
	return !m_Towers.empty()
		&& !m_Troops.empty();
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
	char ch = FindSelected();
	if (m_Troops.count(ch))
		return m_Troops.at(ch)->Clone();
	return nullptr;
}

ostream & CUnitStack::Render(ostream & out) const
{
	if (!(out << endl << Colors::fg_cyan << string(4 * m_Troops.size(), '-') << Colors::color_reset << endl))
		return out;
	
	size_t idx = 0;
	for (const auto & troop : m_Troops)
	{
		if (!(out << ' '))
			return out;
		if (idx++ == m_Selected)
			if (!(out << Colors::bg_cyan))
				return out;
		if (!(out << *troop.second << Colors::color_reset << string(2, ' ')))
			return out;
	}
	
	if (!(out << endl << Colors::fg_cyan << string(4 * m_Troops.size(), '-') << Colors::color_reset << endl))
		return out;
	return m_Troops.at(FindSelected())->RenderInfo(out) << endl;
}

char CUnitStack::FindSelected() const
{
	size_t idx = 0;
	for (const auto & troop : m_Troops)
		if (idx++ == m_Selected)
			return troop.first;
	return 0;
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
		if (!(troop.second->SaveTemplate(out) << endl))
			return out;
	
	for (auto & tower : stack.m_Towers)
		if (!(tower.second->SaveTemplate(out) << endl))
			return out;
	return out << endl;
}
