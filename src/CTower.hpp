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
class CTower : public CTile
{
public:
	explicit CTower(char ch = ' ', ETileType tileType = ETileType::INVALID, std::string color = "") noexcept
		: CTile(ch, tileType, "", move(color))
	{}
	
	// RULE OF FIVE
	~CTower() override = default;
	CTower(const CTower & src) = default;
	CTower & operator=(const CTower & src) = default;
	CTower(CTower && src) = default;
	CTower & operator=(CTower && src) = default;
	
	virtual std::unique_ptr<CTower> Clone() const = 0;
	
	// LOAD
	std::istream & LoadTemplate(std::istream & in)
	{return in >> m_Char >> m_AttackDamage >> m_AttackSpeed >> m_Range;}
	
	virtual std::istream & LoadOnMap(std::istream & in);
	
	// SAVE
	std::ostream & SaveTemplate(std::ostream & out) const
	{return out << m_Type << ' ' << GetChar() << ' ' << m_AttackDamage << ' ' << m_AttackSpeed << ' ' << m_Range;}
	
	virtual std::ostream & SaveOnMap(std::ostream & out) const
	{return out << GetChar() << ' ' << m_Pos << ' ' << m_AttackSpeed.GetCurrent();}
	
	// ACTIONS
	pos_t GetPosition() const
	{return m_Pos;}
	
	CTower & SetPosition(pos_t position);
	/**
	 * Creates buffer with information about the tower
	 * @param width Width of the screen
	 * @return Created buffer
	 */
	virtual CBuffer DrawHelpInfo(size_t width) const = 0;
	/**
	 * Attack on any unit in the radius
	 * @param map Map in which the tower is placed
	 * @param troops Troops on the map we can attack
	 * @param rows Number of rows of the map
	 * @param cols Number of columns of the map
	 * @return True if something was hit
	 */
	virtual bool Attack(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops, int rows, int cols) = 0;
protected:
	int m_AttackDamage = 0;	//!< attack damage of the tower
	CCounter m_AttackSpeed;	//!< attack speed of the tower
	int m_Range = 0;		//!< range of the towers
	pos_t m_Pos;			//!< position of the tower
};