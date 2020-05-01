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
class CTile
{
public:
	explicit CTile(char ch = ' ');
	CTile(char ch, ETileType type);
	CTile(char ch, ETileType type, std::string color);
	bool IsTroop() const;
	bool IsTower() const;
	bool IsWall() const;
	bool IsSpawn() const;
	bool IsGate() const;
	bool IsValid() const;
	void SetColors();
	char GetRawChar() const;
	std::string PrintChar() const;
protected:
	
	char m_Char;			//!< Character representation of the tile
	ETileType m_Type;		//!< Type of the tile
	std::string m_Color;
};
