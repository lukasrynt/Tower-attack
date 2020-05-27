/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 06/05/2020.
*/

#include "CBuffer.hpp"

using namespace std;

CBuffer & CBuffer::Append(const string & line, const string & color)
{
	m_Sizes.push_back(line.length());
	string res = line;
	if (!color.empty())
		res = color + line + Colors::RESET;
	m_Buffer.emplace_back(move(res));
	return *this;
}

CBuffer & CBuffer::AddText(const string & text, const string & color)
{
	if (m_Sizes.empty())
		m_Sizes.emplace_back();
	m_Sizes.back() += text.length();
	string res = text;
	if (!color.empty())
		res = color + text + Colors::RESET;
	if (m_Buffer.empty())
		m_Buffer.emplace_back();
	m_Buffer.back() += res;
	return *this;
}

CBuffer & CBuffer::AddLines(const string & lines, const string & color)
{
	bool empty = color.empty();
	if (!empty)
		AddEscapeSequence(color);
	stringstream ss(lines);
	string line;
	while (getline(ss, line))
		Append(line);
	if (!empty)
		AddEscapeSequence(Colors::RESET);
	return *this;
}

CBuffer & CBuffer::AddEscapeSequence(const string & sequence)
{
	if (m_Buffer.empty())
		m_Buffer.emplace_back();
	m_Buffer.back() += sequence;
	return *this;
}

CBuffer & CBuffer::Concat(CBuffer && other)
{
	const int SPACE = 20;
	size_t longest = Width();
	
	for (size_t i = 0; i < other.m_Buffer.size(); ++i)
	{
		string line;
		if (i < m_Buffer.size())
		{
			AddTextAt(i, string(longest - m_Sizes[i] + SPACE, ' ').append(other.m_Buffer[i]));
			m_Sizes[i] += other.m_Sizes[i];
		}
		else
			Append(string(longest + SPACE, ' ').append(other.m_Buffer[i]));
	}
	return *this;
}

CBuffer & CBuffer::CenterHorizontal()
{
	size_t longest = Width();
	if (longest > m_Width)
		return *this;
	for (auto & str : m_Buffer)
		str = string((m_Width - longest) / 2, ' ').append(str);
	return *this;
}

size_t CBuffer::Width() const
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
	move (other.m_Sizes.begin(), other.m_Sizes.end(), back_inserter(m_Sizes));
	return *this;
}

CBuffer & CBuffer::operator+=(CBuffer && other)
{
	return Append(move(other));
}

ostream & operator<<(ostream & out, const CBuffer & self)
{
	// print buffer
	for (const auto & str : self.m_Buffer)
		if (!(out << noskipws << str << endl))
			return out;
	return out;
}

CBuffer & CBuffer::operator<<(const string & line)
{
	return AddText(line);
}

CBuffer & CBuffer::operator<<(const CTile & tile)
{
	AddText(""s + tile.GetChar(), tile.GetColor());
	return *this;
}

CBuffer & CBuffer::operator+=(const string & line)
{
	return Append(line);
}

CBuffer & CBuffer::AddTextAt(size_t idx, const string & text, const string & color)
{
	if (m_Sizes.empty())
		m_Sizes.emplace_back();
	string res = text;
	if (!color.empty())
		res = color + res + Colors::RESET;
	At(idx) += res;
	return *this;
}

string & CBuffer::At(size_t idx)
{
	if (idx > m_Buffer.size())
		throw out_of_range("Given index is out of bounds of buffer");
	return m_Buffer[idx];
}

CBuffer & CBuffer::CenterVertical(size_t height)
{
	CBuffer tmp{m_Width};
	if (Height() < height)
		for (size_t i = 0; i < (height - Height()) / 2; ++i)
			tmp.Append("");
	tmp.Append(move(*this));
	*this = tmp;
	return *this;
}
