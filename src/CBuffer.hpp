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
	
	explicit CBuffer(std::vector<std::string> buffer)
		: m_Width(0),
		  m_Buffer(move(buffer))
	{
		m_Width = Width();
	}
	
	/**
	 * Appends buffer on top of our buffer
	 * @param other Buffer that we want to move
	 * @return Reference to this
	 */
	CBuffer & Append(CBuffer && other);
	/**
	 * Appends line on top of our buffer
	 * @param line Line to be appended
	 * @param color Color of the line - if there is some it will be reset at the end of line
	 * @return Reference to this
	 */
	CBuffer & Append(const std::string & line = "", const std::string & color = "");
	/**
	 * Adds text to current line
	 * @param text Text to be added to current line
	 * @param color Color the text should have
	 * @return Reference to this
	 */
	CBuffer & AddText(const std::string & text, const std::string & color = "");
	/**
	 * Appends multiple lines
	 * @param lines Lines to be appended, they will be split by '\n' or by the window width
	 * @param color Color of the line - if there is some it will be reset at the end of all lines
	 * @return Reference to this
	 */
	CBuffer & AddLines(const std::string & lines, const std::string & color = "");
	/**
	 * Adds escape sequence to current line - meaning it will not count toward the total length of line
	 * @param sequence Escape sequence to be added
	 * @return Reference to this
	 */
	CBuffer & AddEscapeSequence(const std::string & sequence);
	/**
	 * Same as AddText method
	 * @param line Line to be added
	 * @return Reference to this
	 */
	CBuffer & operator<<(const std::string & line);
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
	CBuffer & operator+=(const std::string & line);
	/**
	 * Same as Append method
	 * @param other Buffer to be appended
	 * @return Reference to this
	 */
	CBuffer & operator+=(CBuffer && other);
	/**
	 * Moves the buffer next to our buffer - vertically
	 * @param other R-value reference to buffer that we want to merge with our current buffer
	 * @return Reference to this
	 */
	CBuffer & Concat(CBuffer && other);
	/**
	 * Centers the buffer so that it is in center of the screen and left aligned (all of the lines share same left side)
	 * @return Reference to this
	 */
	CBuffer & CenterHorizontal();
	/**
	 * Centers the buffer in vertical way - adds spaces on top
	 * @param height Height of the window
	 * @return
	 */
	CBuffer & CenterVertical(size_t height);
	/**
	 * Adds text to the line at the given index
	 * @param idx Index of the line to which we want to append
	 * @param line Line we want to append
	 * @param color Color of the line
	 * @return Reference to this
	 */
	CBuffer & AddTextAt(size_t idx, const std::string & line, const std::string & color = "");
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
	/**
	 * @return Height of the buffer
	 */
	size_t Height() const
	{return m_Buffer.size();}
	/**
	 * @return Longest line in the buffer
	 */
	size_t Width() const;
	
private:
	size_t m_Width;						//!< Width of the buffer
	std::vector<size_t> m_Sizes;		//!< Sizes of the individual lines - stored in order to scale buffer according to text size and not escape sequences
	std::vector<std::string> m_Buffer;	//!< Buffer with lines
};