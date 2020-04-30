/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <iostream>

class CFrames
{
public:
	explicit CFrames(int speed);
	bool ActionAllowed();
	
	int & Current()
	{return m_CurrFrame;}
	
	const int & Current() const
	{return m_CurrFrame;}
	
	friend std::istream & operator>>(std::istream & in, CFrames & self)
	{return in >> self.m_Speed;}
	
	friend std::ostream & operator<<(std::ostream & out, const CFrames & self)
	{return out << self.m_Speed;}
private:
	int m_Speed;
	int m_CurrFrame;
	static const int MAX_FRAME_CNT = 60;
};