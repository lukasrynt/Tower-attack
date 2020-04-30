/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CWaves.hpp"
#include "Colors.hpp"
#include "CArmoredTrooper.hpp"
#include "CMageTower.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CWaves::CWaves()
	: m_Selected(0),
	  m_WaveCnt(0),
	  m_MaxSize(0),
	  m_Frames(20),
	  m_ReleasingWave(false)
{}

CWaves::~CWaves()
{
	for (auto & wave : m_Waves)
		for (auto & troop : wave)
			delete troop;
}

void CWaves::AssignUnitStack(shared_ptr<CUnitStack> unitStack)
{
	m_UnitStack = move(unitStack);
}

/**********************************************************************************************************************/
// LOADING
std::istream & CWaves::Load(std::istream &in)
{
	while (true)
	{
		// check opening bracket
		char ch = 0;
		if (!(in >> ch))
			return in;
		
		// if we have read something else than [ on the beggining of the row we need to quit
		if (ch != '[')
		{
			in.putback(ch);
			break;
		}
		
		// load the troopers in the wawes
		LoadWaves(in);
	}
	return in;
}

std::istream & CWaves::LoadWaves(std::istream & in)
{
	deque<CTrooper *> wave;
	size_t counter = 0;
	char ch = 0;
	while (true)
	{
		// check if the character is correct
		if (!(in >> ch))
			return in;
		
		// if we have empty char, just increase size
		if (ch == ' ')
		{
			counter++;
			continue;
		}
		
		// if we have reached ending bracket - break cycle
		if (ch == ']')
			break;
		
		// add trooper to wave
		if (m_UnitStack->IsTrooperChar(ch))
		{
			counter++;
			wave.push_back(m_UnitStack->CreateTrooperAt(ch));
		}
		else
		{
			in.setstate(ios::failbit);
			return in;
		}
	}
	
	if(!CheckCounter(counter))
	{
		in.setstate(ios::failbit);
		return in;
	}
	
	m_Waves.push_back(wave);
	return in;
}

bool CWaves::CheckCounter(size_t counter)
{
	if (m_MaxSize && counter != m_MaxSize)
		return false;
	
	// save class attributes
	if (!m_MaxSize)
		m_MaxSize = counter;
	return true;
}

int CWaves::GetWaveSize() const
{
	return m_WaveCnt;
}

/**********************************************************************************************************************/
// SAVING
ostream & CWaves::Save(ostream & out) const
{
	out << "(W): " << endl;
	if (!m_Waves.empty())
	{
		for (const auto & wave : m_Waves)
		{
			out << '[';
			for (const auto & troop : wave)
				out << *troop;
			out << string(m_MaxSize - wave.size(), ' ') << ']' << endl;
		}
	}
	return out;
}

/**********************************************************************************************************************/
// RENDER
void CWaves::Render() const
{
	cout << endl << Colors::fg_green << string(10 + m_MaxSize, '-') << Colors::color_reset << endl;
	
	size_t idx = 0;
	for (const auto & wave : m_Waves)
	{
		if (idx == m_Selected)
			cout << Colors::bg_green;
		cout	<< "Wave " << ++idx << ": [";
		for (const auto & troop : wave)
			cout << *troop;
		cout << string(m_MaxSize - wave.size(), ' ') << "]"
			 << Colors::color_reset << endl;
	}
	
	cout << Colors::fg_green << string(10 + m_MaxSize, '-') << Colors::color_reset << endl;
}

/**********************************************************************************************************************/
// INPUT PROCESSING
void CWaves::Cycle()
{
	if (++m_Selected == m_WaveCnt)
		m_Selected = 0;
}

bool CWaves::Release()
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
	if (m_Waves.empty() || m_Waves[m_Selected].size() == m_MaxSize)
		return false;
	
	// add trooper to the wave
	auto troop = m_UnitStack->CreateSelected();
	troop->SetSpawn(m_Selected + 1);
	m_Waves[m_Selected].emplace_back(troop);
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

