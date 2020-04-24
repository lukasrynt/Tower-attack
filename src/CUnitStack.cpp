#include "CUnitStack.hpp"
#include "CArmoredTrooper.hpp"
#include "CMageTower.hpp"

using namespace std;

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
		throw runtime_error("Specifications of trooper doesn't match. For common there should be 3 arguments.");
	m_Troops.insert({'@', new CTrooper(specifications[0], specifications[1], specifications[2])});
}

void CUnitStack::CreateArmoredTroop(const vector<int> & specifications)
{
	if (specifications.size() != 3)
		throw runtime_error("Specifications of trooper doesn't match. For armored there should be 3 arguments.");
	m_Troops.insert({'$',new CArmoredTrooper(specifications[0], specifications[1], specifications[2], specifications[3])});
}

void CUnitStack::CreateArcherTower(const vector<int> & specifications)
{
	if (specifications.size() != 2)
		throw runtime_error("Specifications of trooper doesn't match. For armored there should be 3 arguments.");
	m_Towers.insert({'*', new CTower(specifications[0], specifications[1])});
}

void CUnitStack::CreateMageTower(const vector<int> & specifications)
{
	if (specifications.size() != 4)
		throw runtime_error("Specifications of trooper doesn't match. For armored there should be 3 arguments.");
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

void CUnitStack::RenderTroops() const
{

}
