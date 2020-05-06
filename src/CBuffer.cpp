/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 06/05/2020.
*/

#include "CBuffer.hpp"

using namespace std;

CBuffer & CBuffer::AddLine(string line, string color)
{
	string res = move(line);
	m_Sizes.push_back(res.length());	//TODO detect if the line is longer than window size
	if (!color.empty())
		res = move(color) + res + Colors::RESET;
	m_Buffer.emplace_back(move(res));
	return *this;
}

CBuffer & CBuffer::AddText(string text, string color)
{
	if (m_Sizes.empty())
		m_Sizes.emplace_back();
	string res = move(text);
	m_Sizes.back() += res.length();
	if (!color.empty())
		res = move(color) + res + Colors::RESET;
	if (m_Buffer.empty())
		m_Buffer.emplace_back();
	m_Buffer.back() += res;
	return *this;
}

CBuffer & CBuffer::AddEscapeSequence(std::string sequence)
{
	if (m_Buffer.empty())
		m_Buffer.emplace_back();
	string res = move(sequence);
	m_Buffer.back() += res;
	return *this;
}

CBuffer & CBuffer::Concat(CBuffer && other)
{
	const int SPACE = 20;
	size_t longest = LongestSize();
	
	for (size_t i = 0; i < other.m_Buffer.size(); ++i)
	{
		string line;
		if (i < m_Buffer.size())
		{
			AddTextAt(i, string(longest - m_Sizes[i] + SPACE, ' ').append(other.m_Buffer[i]));
			m_Sizes[i] += other.m_Sizes[i];
		}
		else
			AddLine(string(longest + SPACE, ' ').append(other.m_Buffer[i]));
	}
	return *this;
}

CBuffer & CBuffer::Center()
{
	size_t longest = LongestSize();
	for (auto & str : m_Buffer)
		str = string((m_WindowWidth - longest) / 2,' ') + str;
	return *this;
}

size_t CBuffer::LongestSize() const
{
	size_t tmp = 0;
	for (size_t size : m_Sizes)
		if (tmp < size)
			tmp = size;
	return tmp;
}

CBuffer & CBuffer::Append(CBuffer && other)
{
	move(other.m_Buffer.begin(), other.m_Buffer.end(), back_inserter(m_Buffer));
	return *this;
}

CBuffer & CBuffer::operator+=(CBuffer && other)
{
	return Append(move(other));
}

ostream & operator<<(ostream & out, const CBuffer & self)
{
	for (const auto & str : self.m_Buffer)
		if (!(out << std::noskipws << str << std::endl))
			return out;
	return out;
}

CBuffer & CBuffer::operator<<(string line)
{
	return AddText(move(line));
}

CBuffer & CBuffer::operator<<(const CTile & tile)
{
	AddText(""s + tile.GetChar(), tile.GetColor());
	return *this;
}

CBuffer & CBuffer::operator+=(string line)
{
	return AddLine(move(line));
}

CBuffer & CBuffer::AddTextAt(size_t idx, std::string text, std::string color)
{
	if (m_Sizes.empty())
		m_Sizes.emplace_back();
	string res = move(text);
	if (!color.empty())
		res = move(color) + res + Colors::RESET;
	At(idx) += res;
	return *this;
}

std::string & CBuffer::At(size_t idx)
{
	if (idx > m_Buffer.size())
		throw out_of_range("Given index is out of bounds of buffer");
	return m_Buffer[idx];
}
