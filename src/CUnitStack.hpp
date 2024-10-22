/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include "CTrooper.hpp"
#include "CTower.hpp"
#include "CArmoredTrooper.hpp"
#include "CArcherTower.hpp"
#include "CMageTower.hpp"
#include "CBuffer.hpp"

#include <map>

/**
 * Unit stack with templates to both towers and troopers
 * Handles rendering of trooper selection and cloning from templates
 */
class CUnitStack
{
public:
	// LOADING
	/**
	 * Loads unit stack from input stream
	 * @param in Input stream
	 * @param stack Unit stack to be printed
	 * @return in
	 */
	friend std::istream & operator>>(std::istream & in, CUnitStack & stack);
	/**
	 * Checks whether the given character is among the ones defined as trooper chars
	 * @param ch Character to be checked
	 * @return true if map of troops counts the given character
	 */
	bool IsTrooperChar(char ch) const;
	/**
	 * Checks whether the given character is among the ones defined as tower chars
	 * @param ch Character to be checked
	 * @return true if map of towers counts the given character
	 */
	bool IsTowerChar(char ch) const;
	/**
	 * Check whether loading has been successful. In this case, it means that we have at least one tower and trooper
	 * @return true if loading was successful
	 */
	bool Check() const;
	
	// SAVING
	/**
	 * Saves units specifications to output stream
	 * @param out Output stream
	 * @param [in] stack CUnitStack which we want to save
	 * @return Output stream
	 */
	friend std::ostream & operator<<(std::ostream & out, const CUnitStack & stack);
	
	// CLONING
	/**
	 * Create trooper from template defined by character
	 * @param ch Character defining the character
	 * @return Clone of the created trooper
	 */
	std::unique_ptr<CTrooper> CreateTrooperAt(char ch) const;
	/**
	 * Create tower from template defined by character
	 * @param ch Character defining the character
	 * @return Clone of the created tower
	 */
	std::unique_ptr<CTower> CreateTowerAt(char ch) const;
	/**
	 * Create trooper at currently selected position
	 * @return Clone of the created trooper
	 */
	std::unique_ptr<CTrooper> CreateSelected() const;
	
	// IN GAME
	/**
	 * Creates buffer with information about individual troopers
	 * @param width Width of the window
	 * @return Created buffer
	 */
	CBuffer DrawTroopsInfo(size_t width) const;
	/**
	 * Creates buffer with information about individual towers
	 * @param width Width of the window
	 * @return Created buffer
	 */
	CBuffer DrawTowersInfo(size_t width) const;
	/**
	 * Creates buffer with troopers in unit stack and current selection
	 * @param width Width of the window
	 * @return Created buffer
	 */
	CBuffer Draw(size_t width) const;
	/**
	 * Cycle through trooper selection
	 */
	void Cycle() const;
	/**
	 * Get the price of currently selected unit
	 * @return price
	 */
	int GetSelectedPrice() const;
	/**
	 * Check whether we have not lost - meaning we can no longer afford any unit present in the stack
	 * @param resources Resources that we can spend
	 * @return false when we can no longer afford anything
	 */
	bool Lost(int resources) const;
	
	/**
	 * @return All characters that are registered for towers
	 */
	std::string GetTowerChars() const;
private:
	// VARIABLES
	std::map<char, std::unique_ptr<CTrooper>> m_Troops;					//!< Vector with all troops templates
	std::map<char, std::unique_ptr<CTower>> m_Towers;					//!< Vector with all towers templates
	mutable size_t m_Selected = 0;										//!< Currently selected trooper
	constexpr static const char * const FORBIDDEN_CHARS = "#12345O";	//!< Characters that are used by the map and we can't use them for troops or tower
	
	// RENDER
	/**
	 * Creates line with all the troopers in unit stack and current selection
	 * @return Created line
	 */
	std::string DrawTroops() const;
	
	// LOADING
	/**
	 * Creates entity template based on what character was loaded
	 * @param in Input stream
	 * @param ch Character defining type of entity
	 * @param origTroops Basic versions of troopers - only hollow derived objects
	 * @param origTowers Basic versions of towers - only hollow derived objects
	 * @return True if there is still something to be loaded
	 */
	bool LoadUnit(std::istream & in, char ch, const std::map<char,std::unique_ptr<CTrooper>> & origTroops, const std::map<char,std::unique_ptr<CTower>> & origTowers);
	/**
	 * Creates vector with troopers and towers filled with hollow objects
	 * @param origTroops Basic versions of troopers - only hollow derived objects
	 * @param origTowers Basic versions of towers - only hollow derived objects
	 */
	static void CreateOriginals(std::map<char,std::unique_ptr<CTrooper>> & origTroops, std::map<char,std::unique_ptr<CTower>> & origTowers);
	/**
	 * Checks that the character is not amongst the forbidden ones
	 * @param ch Character to be checked
	 * @return True if the character is valid
	 */
	static bool CharIsValid(char ch);
	/**
	 * Converts currently selected trooper to it's character representation
	 * @return
	 */
	char FindSelected() const;
};