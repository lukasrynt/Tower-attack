/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include "CTrooper.hpp"
#include "CTower.hpp"
#include "CArmoredTrooper.hpp"
#include "CMageTower.hpp"

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
	
	// LOADING
	std::istream & Load(std::istream & in);
	bool IsTrooperChar(char ch) const;
	bool IsTowerChar(char ch) const;
	bool IsTrooperType(char ch) const;
	bool IsTowerType(char ch) const;
	
	// SAVING
	/**
 * Saves units specifications to output stream
 * @param out Output stream
 * @return Output stream
 */
	std::ostream & Save(std::ostream & out) const;
	
	// CLONING
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
	
	// INGAME
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
	mutable size_t m_Selected;				//!< currently selected trooper
	
	// LOADING
	bool LoadUnit(std::istream & in, char ch);
	std::istream & LoadBasicTroop(std::istream & in);
	std::istream & LoadArmoredTroop(std::istream & in);
	std::istream & LoadArcherTower(std::istream & in);
	std::istream & LoadMageTower(std::istream & in);
};