/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <string>
#include <iostream>
#include <queue>
#include <map>
#include <unordered_map>
#include "CPosition.hpp"
#include "ETileType.hpp"

/**
 * Class representing a single tile on the map
 */
class CTile{
public:
	
	explicit CTile(char ch = ' ');
	CTile(char ch, ETileType type);
	CTile(char ch, ETileType type, std::string color);
	
	
	std::string PrintChar() const;
	
	char GetChar() const
	{return m_Char;}
	
	// TYPES FUNCTIONS
	bool IsTroop() const
	{return m_Type == ETileType::BASIC_TROOP || m_Type == ETileType::ARMORED_TROOP;}
	
	bool IsTower() const
	{return m_Type == ETileType::ARCHER_TOWER || m_Type == ETileType::MAGE_TOWER;}
	
	bool IsWall() const
	{return m_Type == ETileType::WALL;}
	
	bool IsSpawn() const
	{return m_Type == ETileType::SPAWN;}
	
	bool IsGate() const
	{return m_Type == ETileType::GATE;}
	
	bool IsValid() const
	{return m_Type != ETileType::INVALID;}
private:
	friend class CTower;
	friend class CTrooper;
	friend class CArmoredTrooper;
	
	std::string InitColor() const;
	ETileType InitType() const;
	
	char m_Char;			//!< Character representation of the tile
	ETileType m_Type;		//!< Type of the tile
	std::string m_Color;
};
