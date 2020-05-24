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
	set<char> signs;
	bool end = false;
	self.SetUnitStack();
	
	while (!end)
		self.LoadObjects(in, signs, end);
	
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

void CGame::LoadObjects(istream & in, set<char> & signs, bool & end)
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
			in >> m_Waves;
			break;
		case 'M':
			in >> m_Map;
			break;
		default:
			end = true;
	}
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

CBuffer CGame::CreateBuffer(int windowWidth) const
{
	CBuffer buffer{windowWidth};
	buffer += move(m_Waves.CreateBuffer(windowWidth).Concat(m_UnitStack->CreateBuffer(windowWidth)).Center());
	buffer += move(CBuffer{windowWidth}.Append().Append());
	buffer += m_Map.CreateBuffer(windowWidth);
	return buffer;
}

void CGame::Visualize(const deque<pos_t> & positions)
{
	m_Map.Visualize(positions);
	//m_Map.Render(cout);
}
