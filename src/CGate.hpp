/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 29.4.2020
 */

#pragma once

#include "CPosition.hpp"

class CGate
{
public:
	bool Fallen() const
	{return m_Hp <= 0;}
	
	void ReceiveDamage(int damage)
	{m_Hp -= damage;}
	
	void Render() const;
	
	pos_t & Position()
	{return m_Pos;}
	
	const pos_t & Position() const
	{return m_Pos;}
	
	bool Full() const
	{return m_Hp == m_MaxHp;}
	
	friend std::istream & operator>>(std::istream & in, CGate & self);
	
	friend std::ostream & operator<<(std::ostream & out, const CGate & self)
	{return out << '(' << self.m_Hp << ' ' << self.m_MaxHp << ')';}
private:
	pos_t m_Pos = pos_t::npos;		//!< position of the gate
	int m_Hp = 0;					//!< number of current gate's health
	int m_MaxHp = 0;				//!< number of gate's max health
};