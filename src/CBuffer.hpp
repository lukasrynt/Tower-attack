/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 05/05/2020.
*/

#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <iomanip>
#include "NColors.hpp"

class CBuffer
{
public:
	CBuffer(const int WINDOW_WIDTH)
		: m_WindowWidth(WINDOW_WIDTH)
	{}
	
	CBuffer(const int WINDOW_WIDTH, std::vector<std::string> buffer)
		: m_WindowWidth(WINDOW_WIDTH),
		  m_Buffer(move(buffer))
	{}
	
	void Clear()
	{m_Buffer.clear();}
	
	int GetWindowHeight() const
	{return m_WindowWidth;}
	
	CBuffer & AddLine(std::string line = "")
	{
		m_Buffer.emplace_back(std::move(line));
		return *this;
	}
	
	CBuffer & AddLine(const std::string & line, const std::string & color)
	{
		m_Buffer.emplace_back(color + line + Colors::RESET);
		return *this;
	}
	
	CBuffer & operator<<(const std::string & line);
	
	CBuffer operator+(const CBuffer & other);
	
	CBuffer & operator+=(CBuffer other);
	
	friend std::ostream & operator<<(std::ostream & out, const CBuffer & self);
	
	CBuffer & AddCenteredLine(const std::string & line, const std::string & color = "");
	
	CBuffer & AddCenteredLine(const std::string & line, int size, const std::string & color = "");
	
	CBuffer & AddHeaderLine(const std::string & line, const std::string & color = "");
	
	CBuffer & AddToCurrentLine(const std::string & text);
	
	CBuffer & AddToCurrentLine(const std::string & text, const std::string & color);
	
	int LongestSize() const;
	
	void Flush()
	{m_Buffer.clear();}
	
	size_t Size() const
	{return m_Buffer.size();}
	
private:
	int m_WindowWidth;
	std::vector<std::string> m_Buffer;
};