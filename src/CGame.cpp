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
	m_UnitStack = make_shared<CUnitStack>();
	m_Map.AssignUnitStack(m_UnitStack);
	m_Waves.AssignUnitStack(m_UnitStack);
	
	while (true)
	{
		// load signature char
		char ch = LoadSignatureChar(in);
		if (!signs.insert(ch).second)
		{
			in.setstate(ios::failbit);
			return in;
		}
		
		// load appropriate object
		switch (ch)
		{
			case '@':
			case '$':
			case '*':
			case '%':
				m_UnitStack->Load();
		}
		else if (ch == 'D')
			LoadMapDimensions(specifications);
		else if (ch == 'G')
			LoadGateHealth(specifications);
		else if (ch == 'W')
			LoadWaves(specifications);
		else if (ch == '#')
			m_Map.LoadMap(in);
		else if (in.eof())
		{
			if (!CheckLoaded(signs))
			{
				in.setstate(ios::failbit);
				return in;
			}
			break;
		}
		else
			throw invalid_file("unknown signature character.");
	}
	
	return in;
}

istream & CGame::LoadSaved(std::istream & in)
{
	set<char> signs;
	m_UnitStack = make_shared<CUnitStack>();
	m_Map.AssignUnitStack(m_UnitStack);
	m_Waves.AssignUnitStack(m_UnitStack);
	
	
	
	return in;

}




istream & CGame::Load(istream & in)
{
	set<char> signs;
	m_UnitStack = make_shared<CUnitStack>();
	m_Map.AssignUnitStack(m_UnitStack);
	m_Waves.AssignUnitStack(m_UnitStack);
	
	while (true)
	{
		char ch = LoadSignatureChar(inFile);
		vector<int> specifications;
		
		// Load specifications only in case there was a signature char
		if (ch != '#' && ch)
			specifications = LoadSpecifications(inFile);
		if (!signs.insert(ch).second)
			throw invalid_file("redefining object is not allowed.");
		if (ch == '@'
			|| ch == '$'
			|| ch == '*'
			|| ch == '%')
			m_UnitStack->LoadUnitSpecifications(specifications, ch);
		else if (ch == 'D')
			LoadMapDimensions(specifications);
		else if (ch == 'G')
			LoadGateHealth(specifications);
		else if (ch == 'W')
			LoadWaves(specifications);
		else if (ch == '#')
			m_Map.LoadMap(inFile, !filename.find(".sav"));
		else if (inFile.eof())
		{
			CheckLoaded(signs);
			break;
		}
		else
			throw invalid_file("unknown signature character.");
	}
}

void CGame::LoadMapDimensions(vector<int> specifications)
{
	if (specifications.size() != 2)
		throw invalid_file("Invalid number of arguments for 'D'");
	m_Map.SetMapDimensions(specifications[0], specifications[1]);
}

void CGame::LoadGateHealth(vector<int> specifications)
{
	if (specifications.size() != 1)
		throw invalid_file("Invalid number of arguments for 'G'.");
	m_Map.SetGateHealth(specifications[0]);
}

void CGame::LoadWaves(vector<int> specifications)
{
	if (specifications.size() != 2)
		throw invalid_file("Invalid number of arguments for 'W'");
	m_Waves.SetWavesSpecifications(specifications[0], specifications[1]);
}

bool CGame::CheckLoaded(const set<char> & signs)
{
	m_Map.CheckSpawnCount(m_Waves.GetWaveSize());
	set<char> check{'M', 'G', 'W', '#', '@', '*'};
	for (const auto & ch : check)
	{
		if (signs.find(ch) == signs.end())
			return false;
	}
	return true;
}

char CGame::LoadSignatureChar(std::istream &in)
{
	char brOp = 0, brCl = 0, div = 0, res = 0;
	
	// check that the first character isn't map character
	if (!(in >> brOp))
		return 0;
	if (brOp == '#')
	{
		in.putback(brOp);
		return brOp;
	}
	else if (brOp != '(')
		return 0;
	
	// scan the rest of signature char
	if (!(in >> res >> brCl >> div)
		|| brCl != ')'
		|| div != ':')
		return 0;
	return res;
}

vector<int> CGame::LoadSpecifications(istream & in)
{
	vector<int> specifications;
	int num = 0;
	char delimit = 0;
	while (true)
	{
		if (!(in >> num >> delimit)
			|| num < 0
			|| delimit != ',')
			break;
		specifications.push_back(num);
	}
	if (delimit != ';')
		throw invalid_file("Wrong specifications format");
	specifications.push_back(num);
	return specifications;
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
	return out << "(S):" << m_WaveOn << ';';
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