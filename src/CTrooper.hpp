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
	explicit CTrooper(char ch  = ' ', ETileType type = ETileType::INVALID, std::string color = "") noexcept
		: CTile(ch, type, move(color)),
		  m_Hp(0),
		  m_Attack(0),
		  m_Price(0),
		  m_SpawnIdx(0)
	{}
	~CTrooper() override = default;
	
	virtual std::unique_ptr<CTrooper> Clone() const noexcept = 0;
	
	// ACTIONS
	void Spawn();
	
	virtual bool Update()
	{return m_Frames.ActionAllowed();}
	
	bool Move(const std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, bool & emplace, bool & erase);
	
	int Attack() const
	{return m_Attack;}
	
	virtual void ReceiveDamage(int damage, std::string bulletColor)
	{m_Hp -= damage;}
	
	// LOADING
	virtual std::istream & LoadTemplate(std::istream & in)
	{return in >> m_Char >> m_Price >> m_Hp >> m_Frames >> m_Attack;}
	
	virtual std::istream & LoadOnMap(std::istream & in);
	
	// SAVING
	virtual std::ostream & SaveTemplate(std::ostream & out) const
	{return out << m_Type << ' ' << m_Char << ' ' << m_Price << ' ' << m_Hp << ' ' << m_Frames << ' ' << m_Attack;};
	
	virtual std::ostream & SaveOnMap(std::ostream & out) const
	{return out << m_Char << ' ' << m_Pos << ' ' << m_Frames.GetCurrent();}
	
	// RENDERING
	virtual CBuffer CreateInfoBuffer(size_t width) const = 0;
	
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