/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CGame.hpp"

using namespace std;

/**********************************************************************************************************************/
// LOADING
void CGame::Load(const string & filename)
{
	set<char> signs;
	while (true)
	{
		ifstream inFile(filename);
		char ch = LoadSignatureChar(inFile);
		vector<int> specifications = LoadSpecifications(inFile);
		if (!signs.insert(ch).second)
			throw invalid_file("redefining object is not allowed.");
		if (ch == '@'
			|| ch == '$'
			|| ch == '*'
			|| ch == '%')
			m_UnitStack->LoadUnitSpecifications(specifications, ch);
		else if (ch == 'M')
			LoadMapDimensions(specifications);
		else if (ch == 'G')
			LoadGateHealth(specifications);
		else if (ch == 'W')
			LoadWaves(specifications);
		else if (ch == '#')
			LoadMap(inFile, filename.find(".sav"));
		else if (inFile.eof())
		{
			CheckLoaded();
			break;
		}
		else
			throw invalid_file("unknown signature character.");
	}
}

void CGame::LoadMapDimensions(std::vector<int> specifications)
{
	if (specifications.size() != 2)
		throw invalid_file("Invalid number of arguments for 'M'");
	m_Map.SetMapDimensions(specifications[0], specifications[1]);
}

void CGame::LoadGateHealth(std::vector<int> specifications)
{
	if (specifications.size() != 1)
		throw invalid_file("Invalid number of arguments for 'G'.");
	m_Map.SetGateHealth(specifications[0]);
}

void CGame::LoadWaves(std::vector<int> specifications)
{
	if (specifications.size() != 1)
		throw invalid_file("Invalid number of arguments for 'W'");
	m_Waves.SetWavesSpecifications(specifications[0], specifications[1]);
}

void CGame::LoadMap(std::istream & in, bool saved)
{
	if (saved)
		m_Map.LoadSavedMap(in);
	else
		m_Map.LoadNewMap(in);
}

void CGame::CheckLoaded()
{
	m_Map.CheckSpawnCount(m_Waves.GetWaveSize());
}

char CGame::LoadSignatureChar(std::istream &in)
{
	char brOp = 0, brCl = 0, div = 0, res = 0;
	
	// check that first character is ( map - marks the signature char
	if (!(in >> brOp)
		|| brOp != '(')
	{
		in.putback(brOp);
		return 0;
	}
	
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
		throw runtime_error("Wrong specifications format");
	specifications.push_back(num);
	return specifications;
}

/**********************************************************************************************************************/
// SAVING
void CGame::Save(const std::string &filename) const
{

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
		EndGame();
}

void CGame::Render() const
{
	m_Waves.Render();
	m_Map.Render();
}

bool CGame::ProcessInput(char ch)
{
	switch (ch)
	{
		case 0:
			break;
		case '1':
			m_Waves.CycleWaves();
			break;
		case '2':
			m_Waves.CycleTroops();
			break;
		case 'a':
			AddTroopToWave();
			break;
		case 'p':
			StartWave();
			break;
		case 'q':
			EndGame();
			break;
		default:
			throw runtime_error("Invalid input, read the options above.");
	}
}

void CGame::StartWave()
{
	if (m_WaveOn)
		throw runtime_error("Wave already running.");
	if (m_Waves.ReleaseWave())
		m_WaveOn = true;
	else
		throw runtime_error("Cannot start an empty wave.");
}

void CGame::AddTroopToWave()
{
	if (!m_Waves.AddTroop())
		throw runtime_error("Current wave is full.");
}

void CGame::EndGame()
{
	m_GameOn = false;
}