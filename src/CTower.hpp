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
		: CTile(ch, tileType, "", move(color)),
		  m_AttackDamage(0),
		  m_AttackSpeed(0),
		  m_Range(0)
	{}
	
	~CTower() override = default;
	virtual std::unique_ptr<CTower> Clone() const = 0;
	
	// LOAD
	virtual std::istream & LoadTemplate(std::istream & in)
	{return in >> m_Char >> m_AttackDamage >> m_AttackSpeed >> m_Range;}
	
	virtual std::istream & LoadOnMap(std::istream & in);
	
	// SAVE
	virtual std::ostream & SaveTemplate(std::ostream & out) const
	{return out << m_Type << ' ' << GetChar() << ' ' << m_AttackDamage << ' ' << m_AttackSpeed << ' ' << m_Range;}
	
	virtual std::ostream & SaveOnMap(std::ostream & out) const
	{return out << GetChar() << ' ' << m_Pos << ' ' << m_AttackSpeed.GetCurrent();}
	
	// ACTIONS
	pos_t GetPosition() const
	{return m_Pos;}
	
	CTower & SetPosition(pos_t position);
	
	virtual CBuffer CreateInfoBuffer(int windowWidth) const = 0;
	virtual bool Attack(std::unordered_map<pos_t, std::shared_ptr<CTile>> & map, std::unordered_map<pos_t, std::shared_ptr<CTrooper>> & troops, int rows, int cols) = 0;
protected:
	int m_AttackDamage;		//!< attack damage of the tower
	CFrames m_AttackSpeed;	//!< attack speed of the tower
	int m_Range;
	pos_t m_Pos;

	
};