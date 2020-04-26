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
	
	std::istream & Load(char ch, std::istream & in);
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
	/**
	 * Saves units specifications to output stream
	 * @param out Output stream
	 * @return Output stream
	 */
	std::ostream & Save(std::ostream & out) const;
	
private:
	// VARIABLES
	std::map<char, CTrooper*> m_Troops;	//!< vector with all troops templates
	std::map<char, CTower*> m_Towers;	//!< vector with all towers templates
	mutable int m_Selected;				//!< currently selected trooper
	
	std::istream & CreateBasicTroop(std::istream & in);
	std::istream & CreateArmoredTroop(std::istream & in);
	std::istream & CreateArcherTower(std::istream & in);
	std::istream & CreateMageTower(std::istream & in);
};