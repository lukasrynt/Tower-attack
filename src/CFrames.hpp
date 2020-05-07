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
	
	int GetSpeed() const
	{return m_Speed;}
	
	const int & GetCurrent() const
	{return m_CurrFrame;}
	
	void SetCurrent(int current)
	{m_CurrFrame = current > MAX_FRAME_CNT / m_Speed ? 0 : current;}
	
	friend std::istream & operator>>(std::istream & in, CFrames & self)
	{return in >> self.m_Speed;}
	
	friend std::ostream & operator<<(std::ostream & out, const CFrames & self)
	{return out << self.m_Speed;}
private:
	int m_Speed;
	int m_CurrFrame;
	static const int MAX_FRAME_CNT = 60;
};