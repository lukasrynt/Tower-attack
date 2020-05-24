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
	explicit CBuffer(size_t width) noexcept
		: m_Width(width)
	{}
	
	CBuffer(std::vector<std::string> buffer)
		: m_Buffer(move(buffer))
	{}
	
	/**
	 * Appends buffer on top of our buffer
	 * @param other Buffer that we want to move
	 * @return Reference to this
	 */
	CBuffer & Append(CBuffer other);
	/**
	 * Appends line on top of our buffer
	 * @param line Line to be appended
	 * @param color Color of the line - if there is some it will be reset at the end of line
	 * @return Reference to this
	 */
	CBuffer & Append(std::string line = "", std::string color = "");
	CBuffer & AddText(std::string text, std::string color = "");
	/**
	 * Appends multiple lines
	 * @param lines Lines to be appended, they will be split by '\n' or by the window width
	 * @param color Color of the line - if there is some it will be reset at the end of all lines
	 * @return Reference to this
	 */
	CBuffer & AddLines(const std::string & lines, std::string color = "");
	/**
	 * Adds escape sequence to current line - meaning it will not count toward the total length of line
	 * @param sequence Escape sequence to be added
	 * @return Reference to this
	 */
	CBuffer & AddEscapeSequence(std::string sequence);
	/**
	 * Same as AddText method
	 * @param line Line to be added
	 * @return Reference to this
	 */
	CBuffer & operator<<(std::string line);
	/**
	 * Appends tile to buffer, respecting it's colors
	 * @param tile Tile to be added
	 * @return Reference to this
	 */
	CBuffer & operator<<(const CTile & tile);
	/**
	 * Same as Append method
	 * @param line Line to be added
	 * @return Reference to this
	 */
	CBuffer & operator+=(std::string line);
	/**
	 * Same as Append method
	 * @param other Buffer to be appended
	 * @return Reference to this
	 */
	CBuffer & operator+=(CBuffer other);
	/**
	 * Moves the buffer next to our buffer - vertically
	 * @param other R-value reference to buffer that we want to merge with our current buffer
	 * @return Reference to this
	 */
	CBuffer & Concat(CBuffer && other);
	/**
	 * Centers the buffer so that it is in center of the screen and left aligned
	 * @return Reference to this
	 */
	CBuffer & CenterVertical();
	CBuffer & CenterHorizontal(size_t height);
	/**
	 * Adds text to the line at the given index
	 * @param idx Index of the line to which we want to append
	 * @param line Line we want to append
	 * @param color Color of the line
	 * @return Reference to this
	 */
	CBuffer & AddTextAt(size_t idx, std::string line, std::string color = "");
	/**
	 * Access line at the given index
	 * @param idx Index of the line we want to access
	 * @return Reference to line
	 */
	std::string & At(size_t idx);
	/**
	 * Renders the buffer on the given stream
	 * @param out Out stream
	 * @param self Buffer we want to render
	 * @return out
	 */
	friend std::ostream & operator<<(std::ostream & out, const CBuffer & self);
	
	size_t Height() const
	{return m_Buffer.size();}
	
	size_t Width() const
	{return LongestSize();}
	
private:
	size_t m_Width;
	std::vector<size_t> m_Sizes;
	std::vector<std::string> m_Buffer;
	
	size_t LongestSize() const;
};