/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <iostream>
#include <queue>
#include <utility>
#include <unordered_map>
#include "Colors.hpp"
#include "CTile.hpp"
#include "CPath.hpp"
#include "CFrames.hpp"

/**
 * (@) Basic trooper with limited powers
 */
class CTrooper
{
public:
	// INIT
	virtual ~CTrooper() = default;
	explicit CTrooper(int hp = 0, int speed = 0, int attack = 0, char ch = '@', char type = 'T', std::string color = Colors::fg_yellow);
	virtual CTrooper * Clone() const;
	
	// ACTIONS
	void Spawn(std::unordered_map<pos_t, CTile> & map);
	bool Move(std::unordered_map<pos_t, CTile> & map);
	int Attack() const;
	virtual void ReceiveDamage(int damage);
	
	// LOADING
	virtual std::istream & LoadTemplate(std::istream & in);
	virtual std::istream & LoadOnMap(std::istream & in);
	
	// SAVING
	virtual std::ostream & SaveTemplate(std::ostream & out) const;
	virtual std::ostream & SaveOnMap(std::ostream & out) const;
	
	// RENDERING
	virtual std::ostream & RenderInfo(std::ostream & out) const;
	
	// GETTERS/ SETTERS
	bool Died() const
	{return m_Hp < 0;}
	
	int DistanceToGoal() const
	{return m_Path.size();}
	
	std::string GetColor() const
	{return m_Color;}
	
	CTile GetTile() const
	{return CTile{m_Char, ETileType::TROOP, m_Color};}
	
	char GetChar() const
	{return m_Char;}
	
	void SetSpawn(size_t idx)
	{m_SpawnIdx = idx;}
	
	int GetSpawn() const
	{return m_SpawnIdx;}
	
	void SetPosition(pos_t pos)
	{m_Pos = pos;}
	
	pos_t GetPosition() const
	{return m_Pos;}
	
	void SetPath(std::deque<pos_t> path)
	{m_Path = std::move(path);}
	
	friend std::ostream & operator<<(std::ostream & out, const CTrooper & troop)
	{return out << troop.m_Char;}

protected:
	char m_Char;
	char m_Type;
	std::string m_Color;
	pos_t m_Pos;
	int m_Hp;					//!< Number of health points of the unit
	int m_Attack;				//!< Attack damage of the trooper
	CFrames m_Frames;
	size_t m_SpawnIdx;			//!< Index of the spawn where the trooper should appear
	std::deque<pos_t> m_Path;	//!< Path to goal
};