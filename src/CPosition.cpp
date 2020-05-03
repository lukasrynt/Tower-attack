/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include <deque>
#include <tuple>
#include "CPosition.hpp"

using namespace std;

CPosition::CPosition(int x, int y)
		: m_X(x),
		  m_Y(y)
{}

CPosition CPosition::left() const
{
	return CPosition(m_X - 1, m_Y);
}

CPosition CPosition::right() const
{
	return CPosition(m_X + 1, m_Y);
}

CPosition CPosition::up() const
{
	return CPosition(m_X, m_Y - 1);
}

CPosition CPosition::down() const
{
	return CPosition(m_X, m_Y + 1);
}

list<CPosition> CPosition::GetCrossNeighbours() const
{
	list<CPosition> res;
	res.push_back(left());
	res.push_back(right());
	res.push_back(up());
	res.push_back(down());
	return res;
}

std::list<CPosition> CPosition::GetDiagNeighbours() const
{
	list<CPosition> res = GetCrossNeighbours();
	res.push_back(left().up());
	res.push_back(left().down());
	res.push_back(right().up());
	res.push_back(right().down());
	return res;
}

bool CPosition::LiesInRange(int rows, int cols) const
{
	return (m_Y <= rows
			&& m_Y >= 0
			&& m_X <= cols
			&& m_X >= 0);
}

deque<vector<CPosition>> CPosition::GetRadius(int level) const
{
	CPosition leftTopDiag, rightTopDiag, leftBotDiag, rightBotDiag;
	leftTopDiag = rightTopDiag = leftBotDiag = rightBotDiag = *this;
	deque<vector<CPosition>> res;
	
	for (int i = 0; i < level; ++i)
	{
		vector<CPosition> layer;
		// move on diagonals
		layer.push_back(leftTopDiag = leftTopDiag.left().up());
		layer.push_back(rightTopDiag = rightTopDiag.right().up());
		layer.push_back(leftBotDiag = leftBotDiag.left().down());
		layer.push_back(rightBotDiag = rightBotDiag.right().down());
		
		// left edge
		for (CPosition tmp = leftTopDiag; tmp != leftBotDiag; tmp = tmp.down())
			layer.push_back(tmp);
		
		// bottom edge
		for (CPosition tmp = leftBotDiag; tmp != rightBotDiag; tmp = tmp.right())
			layer.push_back(tmp);
		
		// right edge
		for (CPosition tmp = rightBotDiag; tmp != rightTopDiag; tmp = tmp.up())
			layer.push_back(tmp);
		
		// top edge
		for (CPosition tmp = rightTopDiag; tmp != leftTopDiag; tmp = tmp.left())
			layer.push_back(tmp);
		
		res.emplace_back(layer);
	}
	
	return res;
}

bool operator<(const CPosition & self, const CPosition & other)
{
	return std::tie(self.m_X, self.m_Y) < std::tie(other.m_X, other.m_Y);
}

bool operator==(const CPosition & self, const CPosition & other)
{
	return self.m_X == other.m_X && self.m_Y == other.m_Y;
}

bool operator!=(const CPosition & self, const CPosition & other)
{
	return !(self == other);
}

istream & operator>>(istream & in, CPosition & self)
{
	char br1, br2;
	if (!(in >> br1 >> self.m_X >> self.m_Y >> br2)
		|| br1 != '('
		|| br2 != ')')
		in.setstate(std::ios::failbit);
	return in;
}
