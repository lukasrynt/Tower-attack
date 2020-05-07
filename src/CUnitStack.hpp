/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include "CTrooper.hpp"
#include "CTower.hpp"
#include "CArmoredTrooper.hpp"
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
	CUnitStack();
	~CUnitStack();
	
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
	CBuffer CreateTroopsInfoBuffer(int windowWidth) const;
	CBuffer CreateTowersInfoBuffer(int windowWidth) const;
	/**
	 * Creates a buffer with unit stack that we can later render
	 * @param windowWidth Width of the window we want to render in
	 * @return created buffer
	 */
	CBuffer CreateBuffer(int windowWidth) const;
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
private:
	// VARIABLES
	std::map<char, CTrooper*> m_Troops;	//!< vector with all troops templates
	std::map<char, CTower*> m_Towers;	//!< vector with all towers templates
	mutable size_t m_Selected;			//!< currently selected trooper
	constexpr static const char * const FORBIDDEN_CHARS = "#12345O";	//!< characters that are used by the map and we can't use them for troops or tower
	
	// RENDER
	std::string RenderTroops() const;
	
	// LOADING
	bool LoadUnit(std::istream & in, char ch, const std::map<char,CTrooper*> & origTroops, const std::map<char,CTower*> & origTowers);
	static void CreateOriginals(std::map<char,CTrooper*> & origTroops, std::map<char,CTower*> & origTowers);
	static void DeleteOriginals(std::map<char,CTrooper*> & origTroops, std::map<char,CTower*> & origTowers);
	static bool CharIsValid(char ch) ;
	char FindSelected() const;
};