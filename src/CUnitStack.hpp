#pragma once

#include <map>
#include "CTrooper.hpp"
#include "CTower.hpp"

class CUnitStack
{
public:
	void LoadUnitSpecifications(const std::vector<int> & specifications, char ch);
	CTrooper * CreateTrooperAt(char ch) const;
	CTower * CreateTowerAt(char ch) const;
	void RenderTroops() const;
private:
	// VARIABLES
	std::map<char, CTrooper*> m_Troops;			//!< vector with all troops the user can choose
	std::map<char, CTower*> m_Towers;
	
	
	void CreateBasicTroop(const std::vector<int> & specifications);
	void CreateArmoredTroop(const std::vector<int> & specifications);
	void CreateArcherTower(const std::vector<int> & specifications);
	void CreateMageTower(const std::vector<int> & specifications);
};