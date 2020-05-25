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
	//INIT
	/**
	 * Constructor for common tiles like walls, spawns and gate
	 * @param ch Character representing the tile
	 */
	explicit CTile(char ch = ' ');
	/**
	 * Constructor in which we know the tile type and it is easier for us to provide
	 * @param ch Character representing the tile
	 * @param type Type of the tile
	 */
	CTile(char ch, ETileType type);
	/**
	 * Constructor in which we know everything - generally used in derived classes
	 * @param ch Character representing the tile
	 * @param type Type of the tile
	 * @param foreColor Foreground color of the tile
	 * @param backColor Background color of the tile
	 */
	CTile(char ch, ETileType type, std::string foreColor, std::string backColor = "");
	virtual ~CTile() noexcept = default;
	CTile(const CTile & src) = default;
	CTile(CTile && src) = default;
	CTile & operator=(const CTile & src) = default;
	CTile & operator=(CTile && src) = default;
	
	// GETTERS / SETTERS
	char GetChar() const
	{return m_Char;}
	
	std::string GetColor() const
	{return m_ForeColor + m_BackColor;}
	
	ETileType GetType() const
	{return m_Type;}

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
	
	bool IsBullet() const
	{return m_Type == ETileType::BULLET;}
	
protected:
	char m_Char;				//!< Character representation of the tile
	ETileType m_Type;			//!< Type of the tile
	std::string m_ForeColor;	//!< Foreground color of the tile
	std::string m_BackColor;	//!< Background color of the tile
	
private:
	void InitColor();
	ETileType InitType() const;
};
