/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CGame.hpp"
#include "ExInvalidInput.hpp"

using namespace std;

/**********************************************************************************************************************/
// LOADING
istream & operator>>(istream & in, CGame & self)
{
	self.AssignUnitStack();
	
	// load individual objects until we reach character that isn't defined
	set<char> signs;
	while (self.LoadObjects(in, signs));
	
	self.m_WaveOn = self.m_Map.WaveIsRunning();
	
	// check if everything was defined correctly
	if (!self.CheckDefined(signs))
		in.setstate(ios::failbit);
	return in;
}

void CGame::AssignUnitStack()
{
	m_UnitStack = make_shared<CUnitStack>();
	m_Map.AssignUnitStack(m_UnitStack);
	m_Waves.AssignUnitStack(m_UnitStack);
}

bool CGame::LoadObjects(istream & in, set<char> & signs)
{
	// load signature char
	char ch = CGame::LoadSignatureChar(in);
	if (!signs.insert(ch).second)
		in.setstate(ios::failbit);
	
	// load appropriate object
	switch (ch)
	{
		case 'U':
			in >> *m_UnitStack;
			break;
		case 'W':
			try
			{
				in >> m_Waves;
			}
			catch (const ios::failure & e)
			{
				if (in.eof())
					return false;
				in.clear(ios::goodbit);
			}
			break;
		case 'M':
			try
			{
				in >> m_Map;
			}
			catch (const ios::failure & e)
			{
				if (in.eof())
					return false;
				in.clear(ios::goodbit);
			}
			break;
		default:
			return false;
	}
	return true;
}

bool CGame::CheckDefined(const set<char> & signs)
{
	m_Map.CheckSpawnCount(m_Waves.GetWaveSize());
	set<char> check{'U', 'M', 'W'};
	for (const auto & ch : check)
		if (signs.find(ch) == signs.end())
			return false;
	return true;
}

char CGame::LoadSignatureChar(istream & in)
{
	char brOp, brCl, res;
	if (!(in >> brOp >> res >> brCl)
		|| brOp != '('
		|| brCl != ')')
		return 0;
	return res;
}

bool CGame::CheckSaved()
{
	m_Map.PlaceTroops()
		.PlaceTowers();
	return m_Map.CheckSaved()
		&& m_UnitStack->Check()
		&& m_Map.CheckSpawnCount(m_Waves.GetWaveCnt());
}

bool CGame::CheckNew()
{
	m_Map.GenerateTowers()
		.PlaceTowers();
	return m_Map.CheckNew()
		&& m_UnitStack->Check()
		&& m_Waves.CheckNew()
		&& m_Map.CheckSpawnCount(m_Waves.GetWaveCnt());
}

/**********************************************************************************************************************/
// SAVING
ostream & operator<<(ostream & out, const CGame & self)
{
	return out << *self.m_UnitStack
		<< self.m_Waves
		<< self.m_Map;
}

/**********************************************************************************************************************/
// INGAME
void CGame::Update()
{
	vector<unique_ptr<CTrooper>> spawns;
	spawns = m_Waves.Update(m_Map.SpawnsFree());
	if (!spawns.empty())
		m_Map.Spawn(spawns);
	if (!m_Map.Update(m_WaveOn))
		m_GameState = EGameState::GAME_WON;
	
	// check game end
	if (!m_Map.WaveIsRunning() && m_Waves.Lost() && !Won())
		m_GameState = EGameState::GAME_OVER;
}

CBuffer CGame::CreateBuffer(size_t width) const
{
	return move(CBuffer{width}
		.Append(move(m_Waves.Draw(width).Concat(m_UnitStack->CreateBuffer(width)).CenterVertical()))
		.Append(move(CBuffer{width}.Append().Append()))
		.Append(m_Map.Draw(width)));
}

void CGame::Visualize(const deque<pos_t> & positions)
{
	m_Map.Visualize(positions);
	cout << m_Map.Draw(50);
}
