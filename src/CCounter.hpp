/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <iostream>
#include "ExInvalidFormat.hpp"

/**
 * Serves as counter for actions.
 * Handles the frames of one entity.
 */
class CCounter
{
public:
	/**
	 * Checks whether the counter has reached the end and increases currFrame
	 * @return True if the counter has reached end
	 */
	bool ActionAllowed()
	{
		if (m_CurrFrame++ != MAX_CNT / m_Speed)
			return false;
		m_CurrFrame = 0;
		return true;
	}
	
	int GetSpeed() const
	{return m_Speed;}
	
	const int & GetCurrent() const
	{return m_CurrFrame;}
	
	void SetCurrent(int current)
	{m_CurrFrame = current > MAX_CNT / m_Speed ? 0 : current;}
	/**
	 * Loads the speed of the counter from input stream
	 * @param in Input stream
	 * @param self Counter
	 * @return in
	 * @throws invalid_format if speed isn't withing interval of 0 to MAX_CNT
	 */
	friend std::istream & operator>>(std::istream & in, CCounter & self)
	{
		in >> self.m_Speed;
		if (self.m_Speed > MAX_CNT || self.m_Speed <= 0)
			throw invalid_format(("Provided speed is not within interval 1 - " + std::to_string(MAX_CNT) + '.').c_str());
		return in;
	}
	/**
	 * Saves the speed of the counter to output stream
	 * @param out Output stream
	 * @param self Counter
	 * @return in
	 */
	friend std::ostream & operator<<(std::ostream & out, const CCounter & self)
	{return out << self.m_Speed;}
private:
	int m_Speed = 0;				//!< Speed of the counter
	int m_CurrFrame = 0;			//!< Current frame
	static const int MAX_CNT = 100;	//!< Maximum number of frames - MAX_CNT/speed is the peak of the counter in which action is allowed
};