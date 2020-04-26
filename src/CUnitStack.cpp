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
		if (!CreateBasicTroop(in))
			return in;
		break;
	}
	return in;
}

void CUnitStack::LoadUnitSpecifications(const vector<int> & specifications, char ch)
{
	if (m_Troops.count(ch) || m_Towers.count(ch))
		throw runtime_error("Trooper or tower is already defined.");
	switch (ch)
	{
		case '@':
			CreateBasicTroop(specifications);
			break;
		case '$':
			CreateArmoredTroop(specifications);
			break;
		case '*':
			CreateArcherTower(specifications);
			break;
		case '%':
			CreateMageTower(specifications);
			break;
		default:
			break;
	}
}

istream & CUnitStack::CreateBasicTroop(istream & in)
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

istream & CUnitStack::CreateArmoredTroop(istream & in)
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

istream & CUnitStack::CreateArcherTower(istream & in)
{
	if (specifications.size() != 2)
		throw invalid_file("specifications of tower doesn't match. For archer tower there should be 2 arguments.");
	m_Towers.insert({'*', new CTower(specifications[0], specifications[1])});
}

istream & CUnitStack::CreateMageTower(istream & in)
{
	if (specifications.size() != 4)
		throw invalid_file("Specifications of trooper doesn't match. For armored there should be 3 arguments.");
	m_Towers.insert({'%', new CMageTower(specifications[0], specifications[1], specifications[2], specifications[3])});
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
		troop.second->Save(out) << endl;
	
	for (auto & tower : m_Towers)
		tower.second->Save(out) << endl;
}
