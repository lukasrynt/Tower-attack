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
	explicit CTower(int attackDamage = 0, int attackSpeed = 0, pos_t position = pos::npos, char ch = '*', char type = 'R');
	
	virtual ~CTower() = default;
	
	virtual CTower * Clone() const;
	
	// LOAD
	virtual std::istream & LoadTemplate(std::istream & in);
	virtual std::istream & LoadOnMap(std::istream & in);
	
	// SAVE
	virtual std::ostream & SaveTemplate(std::ostream & out) const;
	virtual std::ostream & SaveOnMap(std::ostream & out) const;
	
	// ATTACK
	void Attack(std::unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper*> & troops);
	
	// GETTER / SETTERS
	virtual CTile GetTile() const
	{return CTile{m_Char, ETileType::TOWER, Colors::bg_red};}
	
	char GetChar() const
	{return m_Char;}
	
	void SetPosition(pos_t position)
	{m_Pos = position;}
	
	pos_t GetPosition() const
	{return m_Pos;}
	
protected:
	CTile m_Tile;
	char m_Char;
	char m_Type;
	pos_t m_Pos;
	double m_Range;
	int m_AttackDamage;	//!< attack damage of the tower
	CFrames m_Frames;	//!< attack speed of the tower
	
	virtual void SpecialAttack(std::unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper*> & troops);
private:
	pos_t m_ArrowPos;
	std::deque<pos_t> m_ArrowPath;
	
	void ArrowMove(std::unordered_map<pos_t, CTile> & map);
	void ArrowClear(std::unordered_map<pos_t, CTile> & map);
	void AssignArrow(std::unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper*> & troops);
	bool TrooperDamaged(std::unordered_map<pos_t, CTile> & map, CTrooper * trooper);
};