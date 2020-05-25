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
	return CPosition{m_X - 1, m_Y};
}

CPosition CPosition::right() const
{
	return CPosition{m_X + 1, m_Y};
}

CPosition CPosition::up() const
{
	return CPosition{m_X, m_Y - 1};
}

CPosition CPosition::down() const
{
	return CPosition{m_X, m_Y + 1};
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
	return (m_Y < rows
			&& m_Y >= 0
			&& m_X < cols
			&& m_X >= 0);
}

deque<CPosition> CPosition::GetRadius(int distance) const
{
	set<CPosition> visited;
	deque<CPosition> positions, res;
	visited.insert({m_X, m_Y});
	positions.emplace_back(m_X, m_Y);
	while (!positions.empty())
	{
		for (const auto & neighbour : positions.front().GetDiagNeighbours())
		{
			if (!visited.count(neighbour))
			{
				int dist = round<int>(Distance(neighbour));
				if (dist == distance)
					res.push_back(neighbour);
				else if (dist < distance)
					positions.push_back(neighbour);
				visited.insert(neighbour);
			}
		}
		positions.pop_front();
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
