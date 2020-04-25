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

void CUnitStack::CreateBasicTroop(const vector<int> & specifications)
{
	if (specifications.size() != 3)
		throw invalid_file("specifications of trooper doesn't match. For common there should be 3 arguments.");
	m_Troops.insert({'@', new CTrooper(specifications[0], specifications[1], specifications[2])});
}

void CUnitStack::CreateArmoredTroop(const vector<int> & specifications)
{
	if (specifications.size() != 4)
		throw invalid_file("specifications of trooper doesn't match. For armored there should be 3 arguments.");
	m_Troops.insert({'$',new CArmoredTrooper(specifications[0], specifications[1], specifications[2], specifications[3])});
}

void CUnitStack::CreateArcherTower(const vector<int> & specifications)
{
	if (specifications.size() != 2)
		throw invalid_file("specifications of tower doesn't match. For archer tower there should be 2 arguments.");
	m_Towers.insert({'*', new CTower(specifications[0], specifications[1])});
}

void CUnitStack::CreateMageTower(const vector<int> & specifications)
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
