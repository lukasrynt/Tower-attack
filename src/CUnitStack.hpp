/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include "CTrooper.hpp"
#include "CTower.hpp"
#include "CArmoredTrooper.hpp"
#include "CMageTower.hpp"
#include "ExInvalidFile.hpp"

#include <map>

/**
 * Unit stack with templates to both towers and troopers
 * Handles rendering of trooper selection and cloning from templates
 */
class CUnitStack
{
public:
	CUnitStack();
	~CUnitStack();
	/**
	 * Load unit based on character and it's specification. Add it to template vectors.
	 * @param specifications Specifications for the unit
	 * @param ch Character defining unit
	 */
	void LoadUnitSpecifications(const std::vector<int> & specifications, char ch);
	/**
	 * Create trooper from template defined by character
	 * @param ch Character defining the character
	 * @return Clone of the created trooper
	 */
	CTrooper * CreateTrooperAt(char ch) const;
	/**
	 * Create tower from template defined by character
	 * @param ch Character defining the character
	 * @return Clone of the created tower
	 */
	CTower * CreateTowerAt(char ch) const;
	/**
	 * Create trooper at currently selected position
	 * @return Clone of the created trooper
	 */
	CTrooper * CreateSelected() const;
	/**
	 * Render trooper selection
	 */
	void Render() const;
	/**
	 * Cycle through trooper selection
	 */
	void Cycle() const;
	
private:
	// VARIABLES
	std::map<char, CTrooper*> m_Troops;	//!< vector with all troops templates
	std::map<char, CTower*> m_Towers;	//!< vector with all towers templates
	mutable int m_Selected;				//!< currently selected trooper
	
	/**
	 * Create basic trooper with given specifications
	 * @param specifications Specifications with which we create the trooper
	 * @throws invalid_file when specification doesn't match
	 */
	void CreateBasicTroop(const std::vector<int> & specifications);
	/**
	 * Create armored trooper with given specifications
	 * @param specifications Specifications with which we create the trooper
	 * @throws invalid_file when specification doesn't match
	 */
	void CreateArmoredTroop(const std::vector<int> & specifications);
	/**
	 * Create archer tower with given specifications
	 * @param specifications Specifications with which we create the trooper
	 * @throws invalid_file when specification doesn't match
	 */
	void CreateArcherTower(const std::vector<int> & specifications);
	/**
	 * Create mage tower with given specifications
	 * @param specifications Specifications with which we create the trooper
	 * @throws invalid_file when specification doesn't match
	 */
	void CreateMageTower(const std::vector<int> & specifications);
};