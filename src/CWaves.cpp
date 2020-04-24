#include "CWaves.hpp"
#include "Colors.hpp"
#include "CArmoredTrooper.hpp"
#include "CMageTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CWaves::CWaves()
	: m_SelectedWave(0),
	  m_SelectedTroop(0),
	  m_WaveCnt(0),
	  m_WaveMaxSize(0),
	  m_Frames(20),
	  m_ReleasingWave(false)
{}

CWaves::~CWaves()
{}

/**********************************************************************************************************************/
// LOADING
void CWaves::SetWavesSpecifications(int waveCnt, int maxWaveSize)
{
	m_WaveCnt = waveCnt;
	m_WaveMaxSize = maxWaveSize;
	InitWaves();
}

void CWaves::InitWaves()
{
	for (size_t i = 0; i < m_WaveCnt; i++)
	{
		deque<CTrooper*> wave;
		m_Waves.push_back(wave);
	}
}

int CWaves::GetWaveSize() const
{
	return m_WaveCnt;
}

/**********************************************************************************************************************/
// RENDER
void CWaves::Render() const
{
	PrintWaves();
	PrintTroops();
}

void CWaves::PrintTroops() const
{
	cout << endl << Colors::fg_cyan << string(3 * + m_Troops.size(), '-') << Colors::color_reset << endl;
	
	size_t idx = 0;
	for (const auto & troop : m_Troops)
	{
		if (idx++ == m_SelectedTroop)
			cout << Colors::bg_cyan;
		cout << *troop.second << Colors::color_reset << string(3, ' ');
	}
	
	cout << endl << Colors::fg_cyan << string(3 * + m_Troops.size(), '-') << Colors::color_reset << endl << endl;
}

void CWaves::PrintWaves() const
{
	cout << endl << Colors::fg_green << string(10 + m_WaveMaxSize, '-') << Colors::color_reset << endl;
	
	size_t idx = 0;
	for (const auto & wave : m_Waves)
	{
		if (idx == m_SelectedWave)
			cout << Colors::bg_green;
		cout	<< "Wave " << ++idx << ": [";
		for (const auto & troop : wave)
			cout << *troop;
		cout << string(m_WaveMaxSize - wave.size(), ' ') << "]"
			<< Colors::color_reset << endl;
	}
	
	cout << Colors::fg_green << string(10 + m_WaveMaxSize, '-') << Colors::color_reset << endl;
}

/**********************************************************************************************************************/
// INPUT PROCESSING
void CWaves::CycleWaves()
{
	if (++m_SelectedWave == m_WaveCnt)
		m_SelectedWave = 0;
}

void CWaves::CycleTroops()
{
	if (++m_SelectedTroop == m_Troops.size())
		m_SelectedTroop = 0;
}

bool CWaves::ReleaseWave()
{
	// check if the waves are not empty
	bool empty = true;
	for (const auto & wave : m_Waves)
		if (!wave.empty())
			empty = false;
		
	if (!empty)
		m_ReleasingWave = true;
	return !empty;
}

bool CWaves::AddTroop()
{
	if (m_Waves.empty() || m_Waves[m_SelectedWave].size() == m_WaveMaxSize)
		return false;
	
	// add trooper to the wave
	auto troop = m_Troops[m_SelectedTroop]->Clone();
	troop->SetSpawn(m_SelectedWave + 1);
	m_Waves[m_SelectedWave].emplace_back(troop);
	return true;
}

/**********************************************************************************************************************/
// UPDATE
CTrooper * CWaves::Update(bool & waveOn)
{
	if (!m_ReleasingWave || !m_Frames.ActionAllowed())
		return nullptr;

	// if all waves are empty we leave the WaveStarted on false
	waveOn = true;
	m_ReleasingWave = false;
	for (auto & m_Wave : m_Waves)
	{
		if (m_Wave.empty())
			continue;
		CTrooper * troop = m_Wave.front();
		m_Wave.pop_front();
		m_ReleasingWave = true;
		return troop;
	}
	return nullptr;
}

