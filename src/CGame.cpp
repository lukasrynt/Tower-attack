/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CGame.hpp"
#include "ExInvalidInput.hpp"

using namespace std;

CGame::CGame()
	: m_WaveOn(false),
	  m_GameState(EGameState::GAME_RUNNING),
	  m_Resources(0)
{}

/**********************************************************************************************************************/
// LOADING
istream & operator>>(istream & in, CGame & self)
{
	set<char> signs;
	bool end = false;
	self.SetUnitStack();
	
	while (!end)
		if (!self.LoadObjects(in, signs, end))
			return in;
	
	self.m_WaveOn = self.m_Map.WaveIsRunning();
	
	// check if everything was defined correctly
	if (!self.CheckDefined(signs))
		in.setstate(ios::failbit);
	return in;
}

void CGame::SetUnitStack()
{
	m_UnitStack = make_shared<CUnitStack>();
	m_Map.AssignUnitStack(m_UnitStack);
	m_Waves.AssignUnitStack(m_UnitStack);
}

istream & CGame::LoadObjects(istream & in, set<char> & signs, bool & end)
{
	// load signature char
	char ch = CGame::LoadSignatureChar(in);
	if (!signs.insert(ch).second)
	{
		in.setstate(ios::failbit);
		return in;
	}
	
	// load appropriate object
	switch (ch)
	{
		case 'U':
			if (!(in >> *m_UnitStack))
				return in;
			break;
		case 'W':
			if (!(in >> m_Waves))
				return in;
			break;
		case 'M':
			if (!(in >> m_Map))
				return in;
			break;
		default:
			end = true;
	}
	// in case eof was reached - it is ok
	in.clear(ios::goodbit);
	return in;
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

bool CGame::CheckSaved() const
{
	return m_Map.CheckSaved()
		&& m_UnitStack->Check()
		&& m_Map.CheckSpawnCount(m_Waves.GetWaveCnt());
}

bool CGame::CheckNew() const
{
	return m_Map.CheckNew()
		&& m_UnitStack->Check()
		&& m_Waves.CheckNew()
		&& m_Map.CheckSpawnCount(m_Waves.GetWaveCnt());
}

/**********************************************************************************************************************/
// SAVING
ostream & operator<<(ostream & out, const CGame & self)
{
	if (!(out << *self.m_UnitStack)
		|| !(out << self.m_Waves)
		|| !(out << self.m_Map))
		return out;
	return out;
}

/**********************************************************************************************************************/
// INGAME
void CGame::Update()
{
	vector<CTrooper*> spawns;
	spawns = m_Waves.Update(m_Map.SpawnsFree());
	if (!spawns.empty())
		m_Map.Spawn(spawns);
	if (!m_Map.Update(m_WaveOn))
		m_GameState = EGameState::GAME_WON;
	
	// check game end
	if (!m_Map.WaveIsRunning() && m_Waves.Lost() && !Won())
		m_GameState = EGameState::GAME_OVER;
}

CBuffer CGame::Render(int windowWidth) const
{
	return m_Waves.Render(windowWidth) + m_UnitStack->Render(windowWidth);//) += m_Map.Render(windowWidth);
}

void CGame::Visualize(const deque<pos_t> & positions)
{
	m_Map.Visualize(positions);
	//m_Map.Render(cout);
}
