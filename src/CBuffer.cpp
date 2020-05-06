/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 06/05/2020.
*/

#include "CBuffer.hpp"

using namespace std;


CBuffer & CBuffer::AddCenteredLine(const std::string & line, const std::string & color)
{
	stringstream ss;
	ss << setw(m_WindowWidth / 2);
	if (!color.empty())
		ss << color;
	ss << line;
	if (!color.empty())
		ss << Colors::RESET;
	AddLine(ss.str());
	return *this;
}

CBuffer & CBuffer::AddCenteredLine(const std::string & line, int size, const std::string & color)
{
	stringstream ss;
	if (!color.empty())
		ss << color;
	ss << string((m_WindowWidth - size) / 2, ' ') << line;
	if (!color.empty())
		ss << Colors::RESET;
	AddLine(ss.str());
	return *this;
}

CBuffer & CBuffer::AddHeaderLine(const string & line, const string & color) {
	stringstream ss;
	if (!color.empty())
		ss << color;
	ss << string((m_WindowWidth - line.size()) / 2, ' ') << line;
	if (!color.empty())
		ss << Colors::RESET;
	AddLine(ss.str());
	return *this;
}

CBuffer & CBuffer::AddToCurrentLine(const std::string & text)
{
	if (m_Buffer.empty())
		m_Buffer.emplace_back();
	m_Buffer.back() += text;
	return *this;
}

CBuffer & CBuffer::AddToCurrentLine(const string & text, const string & color)
{
	if (m_Buffer.empty())
		m_Buffer.emplace_back();
	m_Buffer.back() += color + text + Colors::RESET;
	return *this;
}

CBuffer CBuffer::operator+(const CBuffer & other)
{
	const int SPACE = 20;
	size_t size = LongestSize() + SPACE + other.LongestSize();
	
	CBuffer res{m_WindowWidth};
	auto it2 = other.m_Buffer.begin();
	for (auto & it1 : m_Buffer)
	{
		string line;
		line.append(move(it1))
			.append(string(SPACE,' '));
		if (it2 != other.m_Buffer.end())
		{
			line.append(*it2);
			++it2;
		}
		res.AddCenteredLine(line, size);
	}
	res.AddCenteredLine("ahoj");
	return res;
}

int CBuffer::LongestSize() const
{
	size_t tmp = 0;
	for (const auto & line : m_Buffer)
		if (line.size() > tmp)
			tmp = line.size();
	return tmp;
}

CBuffer & CBuffer::operator+=(CBuffer other)
{
	move(other.m_Buffer.begin(), other.m_Buffer.end(), back_inserter(m_Buffer));
	return *this;
}

std::ostream & operator<<(ostream & out, const CBuffer & self) {
	for (const auto & str : self.m_Buffer)
		if (!(out << std::noskipws << str << std::endl))
			return out;
	return out;
}

CBuffer & CBuffer::operator<<(const string &line) {
	if (m_Buffer.empty())
		m_Buffer.emplace_back();
	m_Buffer.back() += line;
	return *this;
}

