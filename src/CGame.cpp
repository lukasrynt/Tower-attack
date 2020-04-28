/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CGame.hpp"
#include "ExInvalidInput.hpp"

using namespace std;

CGame::CGame()
	: m_GameOn(true),
	  m_WaveOn(false)
{}

/**********************************************************************************************************************/
// LOADING

istream & CGame::LoadNew(istream & in)
{
	set<char> signs;
	bool end = false;
	m_UnitStack = make_shared<CUnitStack>();
	m_Map.AssignUnitStack(m_UnitStack);
	m_Waves.AssignUnitStack(m_UnitStack);
	string commonSigns = "@$%*WWDG#";
	
	while (!end)
	{
		// load signature char
		char ch = LoadSignatureChar(in);
		if (!signs.insert(ch).second)
		{
			in.setstate(ios::failbit);
			return in;
		}
		
		// load appropriate object
		if (commonSigns.find(ch) != string::npos)
		{
			if (!LoadCommon(in, ch, false))
				return in;
		}
		else
			end = true;
	}
	
	// check eof and signature chars
	if (!in.eof() || !CheckLoaded(signs))
		in.setstate(ios::failbit);
	else
		in.clear(ios::goodbit);
	return in;
}

istream & CGame::LoadSaved(istream & in)
{
	set<char> signs;
	bool end = false;
	m_UnitStack = make_shared<CUnitStack>();
	m_Map.AssignUnitStack(m_UnitStack);
	m_Waves.AssignUnitStack(m_UnitStack);
	string commonSigns = "@$*%WDG#";
	
	while (!end)
	{
		// load signature char
		char ch = LoadSignatureChar(in);
		if (!signs.insert(ch).second)
		{
			in.setstate(ios::failbit);
			return in;
		}
		
		// load appropriate object
		if (commonSigns.find(ch) != string::npos)
		{
			if (!LoadCommon(in, ch, false))
				return in;
		}
		else if (ch == '[')
		{
			if (!m_Waves.LoadTroops(in))
				return in;
		}
		else if (ch == 'S')
		{
			if (!LoadState(in))
				return in;
		}
		else if (ch == 'P')
		{
		
		}
		else
			end = true;
	}
	
	// check eof and signature chars
	if (!in.eof() || !CheckLoaded(signs))
		in.setstate(ios::failbit);
	else
		in.clear(ios::goodbit);
	return in;
}

std::istream & CGame::LoadState(std::istream &in)
{
	char del;
	if (!(in >> m_WaveOn >> del)
		|| del != ';')
		return in;
	return in;
}

istream & CGame::LoadCommon(std::istream &in, char ch, bool saved)
{
	switch (ch)
	{
		case '@':
		case '$':
		case '*':
		case '%':
			m_UnitStack->Load(ch, in);
			break;
		case 'W':
			if (!m_Waves.Load(in))
				return in;
			break;
		case 'D':
			if (!m_Map.LoadDimensions(in))
				return in;
			break;
		case 'G':
			if (!m_Map.LoadGate(in))
				return in;
			break;
		case '#':
			if (!m_Map.LoadMap(in, saved))
				return in;
			break;
		default:
			break;
	}
	return in;
}

bool CGame::CheckLoaded(const set<char> & signs)
{
	m_Map.CheckSpawnCount(m_Waves.GetWaveSize());
	set<char> check{'D', 'G', 'W', '#', '@', '*'};
	for (const auto & ch : check)
		if (signs.find(ch) == signs.end())
			return false;
	return true;
}

char CGame::LoadSignatureChar(istream & in)
{
	char brOp = 0, brCl = 0, div = 0, res = 0;
	if (!(in >> brOp >> res >> brCl >> div)
		|| brOp != '('
		|| brCl != ')'
		|| div != ':')
		return 0;
	return res;
}

/**********************************************************************************************************************/
// SAVING
ostream & CGame::Save(ostream & out) const
{
	if (!(m_UnitStack->Save(out))
		|| !(m_Waves.Save(out))
		|| !(SaveState(out))
		|| !(m_Map.Save(out)))
		return out;
	return out;
}

ostream & CGame::SaveState(ostream & out) const
{
	return out << "(S): " << m_WaveOn << ';' << endl;
}

/**********************************************************************************************************************/
// INGAME

bool CGame::IsOn() const
{
	return m_GameOn;
}

void CGame::Update()
{
	CTrooper * troop = m_Waves.Update(m_WaveOn);
	if (troop)
		m_Map.Spawn(troop);
	if (!m_Map.Update(m_WaveOn))
		End();
}

void CGame::Render() const
{
	m_Waves.Render();
	m_UnitStack->Render();
	m_Map.Render();
}

void CGame::ProcessInput(char ch)
{
	switch (ch)
	{
		case 0:
			break;
		case '1':
			m_Waves.Cycle();
			break;
		case '2':
			m_UnitStack->Cycle();
			break;
		case 'a':
			AddTroopToWave();
			break;
		case 'p':
			StartWave();
			break;
		case 'q':
			End();
			break;
		default:
			throw invalid_input("Invalid input, read the options above.");
	}
}

void CGame::StartWave()
{
	if (m_WaveOn)
		throw invalid_input("Wave already running.");
	if (m_Waves.Release())
		m_WaveOn = true;
	else
		throw invalid_input("Cannot start an empty wave.");
}

void CGame::AddTroopToWave()
{
	if (!m_Waves.AddTroop())
		throw invalid_input("Current wave is full.");
}

void CGame::End()
{
	m_GameOn = false;
}
