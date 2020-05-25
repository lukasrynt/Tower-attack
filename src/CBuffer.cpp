/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 06/05/2020.
*/

#include "CBuffer.hpp"

using namespace std;

CBuffer & CBuffer::Append(string line, string color)
{
	string res = move(line);
	m_Sizes.push_back(res.length());
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

CBuffer & CBuffer::AddLines(const string & lines, string color)
{
	string col = move(color);
	bool empty = col.empty();
	if (!empty)
		AddEscapeSequence(move(col));
	stringstream ss(lines);
	string line;
	while (getline(ss, line))
		Append(line);
	if (!empty)
		AddEscapeSequence(Colors::RESET);
	return *this;
}

CBuffer & CBuffer::AddEscapeSequence(string sequence)
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

CBuffer & CBuffer::CenterVertical()
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

CBuffer & CBuffer::Append(CBuffer other)
{
	move(other.m_Buffer.begin(), other.m_Buffer.end(), back_inserter(m_Buffer));
	return *this;
}

CBuffer & CBuffer::operator+=(CBuffer other)
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
	return Append(move(line));
}

CBuffer & CBuffer::AddTextAt(size_t idx, string text, string color)
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

CBuffer & CBuffer::CenterHorizontal(size_t height)
{
	// center horizontally
	CBuffer tmp{m_Width};
	if (Height() < height)
		for (size_t i = 0; i < (height - Height()) / 2; ++i)
			tmp.Append("");
	tmp.Append(move(*this));
	*this = tmp;
	return *this;
}
