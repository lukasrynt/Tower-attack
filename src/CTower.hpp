/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <iostream>
#include "CTrooper.hpp"
#include "CTile.hpp"

/**
 * Represents basic tower with archer attacks
 */
class CTower
{
public:
	// INIT
	explicit CTower(int attackDamage = 0, int attackSpeed = 0, pos_t position = pos_t::npos, char ch = '*');
	virtual ~CTower() = default;
	virtual CTower * Clone();
	
	// LOAD
	static CTower * Load(std::istream & in);
	
	// SAVE
	virtual std::ostream & Save(std::ostream & out) const;
	
	// ATTACK
	virtual void Attack(std::unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper *> & troops);
	pos_t PerimeterBreached(std::unordered_map<pos_t, CTile> & map);
	
protected:
	char m_Char;
	pos_t m_Pos;
	int m_AttackDamage;	//!< attack damage of the tower
	CFrames m_Frames;	//!< attack speed of the tower
	
private:
	std::deque<pos_t> m_ArrowPath;
	pos_t m_ArrowPos;
	std::deque<std::vector<pos_t>> m_Perimeter;
	
	void ArrowMove(std::unordered_map<pos_t, CTile> & map);
	void ArrowClear(std::unordered_map<pos_t, CTile> & map);
};