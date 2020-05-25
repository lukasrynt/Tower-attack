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
#include "CCounter.hpp"
#include "CBuffer.hpp"

/**
 * (@) Basic trooper with limited powers
 */
class CTrooper : public CTile
{
public:
	// INIT
	/**
	 * Constructor to initialize trooper
	 * @param ch Character representing trooper
	 * @param type Type of tile - set to invalid as CTrooper is just an interface
	 * @param color Foreground color of the tile
	 */
	explicit CTrooper(char ch  = ' ', ETileType type = ETileType::INVALID, std::string color = "") noexcept
		: CTile(ch, type, move(color))
	{}
	
	~CTrooper() override = default;
	
	/**
	 * @return Cloned unit
	 */
	virtual std::unique_ptr<CTrooper> Clone() const noexcept = 0;
	
	// ACTIONS
	/**
	 * Moves trooper first position of his path
	 */
	void Spawn();
	/**
	 * Updates trooper frames
	 * @return True if the trooper can perform action
	 */
	virtual bool Update()
	{return m_Frames.ActionAllowed();}
	/**
	 * Moves the trooper on his determined path
	 * @param map Map on which the trooper moves - for us to see if our path isn't temporarily blocked
	 * @param emplace Set to true if we should move the trooper on the map to his current position
	 * @param erase Set to true if we should remove troops old position from map
	 * @return True if trooper has reached goal
	 */
	bool Move(const std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, bool & emplace, bool & erase);
	/**
	 * @return Attack force of the trooper
	 */
	int Attack() const
	{return m_Attack;}
	/**
	 * Receive damage from turret and recolors the trooper if he was hit
	 * @param damage Damage to be received
	 * @param bulletColor Color to which the background will change if the trooper was damaged
	 */
	virtual void ReceiveDamage(int damage, std::string bulletColor);
	
	// LOADING
	/**
	 * Loads template unit from input stream
	 * @param in Input stream
	 * @return in
	 */
	virtual std::istream & LoadTemplate(std::istream & in)
	{return in >> m_Char >> m_Price >> m_Hp >> m_Frames >> m_Attack;}
	/**
	 * Loads unit on map - with state of his frames
	 * @param in Input stream
	 * @return in
	 */
	virtual std::istream & LoadOnMap(std::istream & in);
	
	// SAVING
	/**
	 * Saves unit template to output stream
	 * @param out Output stream
	 * @return out
	 */
	virtual std::ostream & SaveTemplate(std::ostream & out) const
	{return out << m_Type << ' ' << m_Char << ' ' << m_Price << ' ' << m_Hp << ' ' << m_Frames << ' ' << m_Attack;};
	/**
	 * Saves unit on map to output stream
	 * @param out Output stream
	 * @return out
	 */
	virtual std::ostream & SaveOnMap(std::ostream & out) const
	{return out << m_Char << ' ' << m_Pos << ' ' << m_Frames.GetCurrent();}
	
	// RENDERING
	/**
	 * Creates buffer with information about the unit
	 * @param width Width of the screen
	 * @return created buffer
	 */
	virtual CBuffer CreateInfoBuffer(size_t width) const = 0;
	
	// GETTERS/ SETTERS
	int GetHp() const
	{return m_Hp;}

	bool Died() const
	{return m_Hp <= 0;}
	
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
	int m_Hp = 0;				//!< Number of health points of the unit
	int m_Attack = 0;			//!< Attack damage of the trooper
	int m_Price = 0;			//!< Price of the unit
	CCounter m_Frames;			//!< Counter for actions - will do something only if the frames allow it
	size_t m_SpawnIdx = 0;		//!< Index of the spawn where the trooper should appear
	std::deque<pos_t> m_Path;	//!< Path to goal
};