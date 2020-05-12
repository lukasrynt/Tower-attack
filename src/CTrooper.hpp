/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <iostream>
#include <queue>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <memory>
#include "escape_sequences.h"
#include "CTile.hpp"
#include "CPath.hpp"
#include "CFrames.hpp"
#include "CBuffer.hpp"

/**
 * (@) Basic trooper with limited powers
 */
class CTrooper : public CTile
{
public:
	// INIT
	explicit CTrooper(char ch  = '@', ETileType type = ETileType::BASIC_TROOP, std::string color = Colors::FG_YELLOW) noexcept
		: CTile(ch, type, move(color)),
		  m_Hp(0),
		  m_Attack(0),
		  m_Price(0),
		  m_SpawnIdx(0)
	{}
	
	virtual CTrooper * Clone() const noexcept
	{return new CTrooper(*this);}
	
	// ACTIONS
	void Spawn(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map);
	bool Move(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map);
	int Attack() const
	{return m_Attack;}
	
	virtual void ReceiveDamage(int damage);
	
	// LOADING
	virtual std::istream & LoadTemplate(std::istream & in);
	virtual std::istream & LoadOnMap(std::istream & in);
	
	// SAVING
	virtual std::ostream & SaveTemplate(std::ostream & out) const;
	virtual std::ostream & SaveOnMap(std::ostream & out) const;
	
	// RENDERING
	virtual CBuffer CreateInfoBuffer(int windowWidth) const;
	
	// GETTERS/ SETTERS
	bool Died() const
	{return m_Hp < 0;}
	
	int DistanceToGoal() const
	{return m_Path.size();}
	
	void SetSpawn(size_t idx)
	{m_SpawnIdx = idx;}
	
	int GetSpawn() const
	{return m_SpawnIdx;}
	
	pos_t GetPosition() const
	{return m_Pos;}
	
	void SetPath(std::deque<pos_t> path)
	{m_Path = std::move(path);}
	
	int GetPrice() const
	{return m_Price;}
protected:
	pos_t m_Pos;
	int m_Hp;					//!< Number of health points of the unit
	int m_Attack;				//!< Attack damage of the trooper
	int m_Price;
	CFrames m_Frames;
	size_t m_SpawnIdx;			//!< Index of the spawn where the trooper should appear
	std::deque<pos_t> m_Path;	//!< Path to goal
};