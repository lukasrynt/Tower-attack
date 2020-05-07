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
	explicit CTower(int attackDamage = 0, int attackSpeed = 0, int range = 0, pos_t position = pos::npos, CTile  tile = {'*', ETileType::ARCHER_TOWER, Colors::BG_RED});
	virtual ~CTower() = default;
	virtual CTower * Clone() const;
	
	// LOAD
	virtual std::istream & LoadTemplate(std::istream & in);
	virtual std::istream & LoadOnMap(std::istream & in);
	
	// SAVE
	virtual std::ostream & SaveTemplate(std::ostream & out) const;
	virtual std::ostream & SaveOnMap(std::ostream & out) const;
	
	// ACTIONS
	virtual CBuffer CreateInfoBuffer(int windowWidth) const;
	virtual bool Attack(std::unordered_map<pos_t, CTile> & map, int rows, int cols, std::unordered_map<pos_t, CTrooper*> & troops);
	
	// GETTER / SETTERS
	virtual CTile GetTile() const
	{return m_Tile;}
	
	char GetChar() const
	{return m_Tile.GetChar();}
	
	pos_t GetPosition() const
	{return m_Pos;}
	
	ETileType GetType() const
	{return m_Tile.m_Type;}
	
protected:
	CTile m_Tile;
	pos_t m_Pos;
	double m_Range;
	int m_AttackDamage;	//!< attack damage of the tower
	CFrames m_Frames;	//!< attack speed of the tower
	
private:
	pos_t m_ArrowPos;
	std::deque<pos_t> m_ArrowPath;
	
	bool ArrowMove(std::unordered_map<pos_t, CTile> & map);
	void ArrowClear(std::unordered_map<pos_t, CTile> & map);
	bool AssignArrow(std::unordered_map<pos_t, CTile> & map, int rows, int cols, const std::unordered_map<pos_t, CTrooper*> & troops);
	void DamageTrooper(std::unordered_map<pos_t, CTile> & map, std::unordered_map<pos_t, CTrooper*> & troops);
};