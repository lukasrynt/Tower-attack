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
#include "escape_sequences.h"
#include "CTile.hpp"

class CBuffer
{
public:
	~CBuffer() = default;
	CBuffer(const CBuffer & other) = delete;
	CBuffer(CBuffer && other) = default;
	CBuffer & operator=(const CBuffer & other) = delete;
	CBuffer & operator=(CBuffer && other) = default;
	
	explicit CBuffer(const int WINDOW_WIDTH) noexcept
		: m_WindowWidth(WINDOW_WIDTH)
	{}
	
	CBuffer(const int WINDOW_WIDTH, std::vector<std::string> buffer)
		: m_WindowWidth(WINDOW_WIDTH),
		  m_Buffer(move(buffer))
	{}
	
	CBuffer & AddLine(std::string line = "", std::string color = "");
	CBuffer & AddText(std::string text, std::string color = "");
	CBuffer & AddLines(const std::string & lines, std::string color = "");
	CBuffer & AddEscapeSequence(std::string sequence);
	CBuffer & operator<<(std::string line);
	CBuffer & operator<<(const CTile & tile);
	CBuffer & operator+=(std::string line);
	CBuffer & Append(CBuffer other);
	CBuffer & operator+=(CBuffer && other);
	CBuffer & Concat(CBuffer && other);
	CBuffer & Center();
	CBuffer & AddTextAt(size_t idx, std::string line, std::string color = "");
	std::string & At(size_t idx);
	friend std::ostream & operator<<(std::ostream & out, const CBuffer & self);
	size_t LongestSize() const;
	
	void Flush()
	{m_Buffer.clear();}
	
	int Height() const
	{return m_Buffer.size();}
	
	int Width() const
	{return LongestSize();}
	
private:
	size_t m_WindowWidth;
	std::vector<size_t> m_Sizes;
	std::vector<std::string> m_Buffer;
};