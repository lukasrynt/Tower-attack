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
istream & operator>>(istream & in, CWaves & waves)
{
	char ch;
	if (!(in >> waves.m_ReleasingWave))
		return in;
	
	while (true)
	{
		// check opening bracket
		if (!(in >> ch))
		{
			if (in.eof())
				in.clear(ios::goodbit);
			return in;
		}
		
		// if we have read something else than [ on the beggining of the row we need to quit
		if (ch != '[')
		{
			in.putback(ch);
			break;
		}
		
		// load the troopers in the wawes
		waves.LoadWaves(in);
	}
	return in;
}

std::istream & CWaves::LoadWaves(std::istream & in)
{
	if (!m_UnitStack)
		throw runtime_error("Unit stack not defined.");
	
	deque<CTrooper *> wave;
	size_t counter = 0;
	char ch = 0;
	while (true)
	{
		// check if the character is correct
		if (!(in.get(ch)))
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
	return m_Waves.size();
}

/**********************************************************************************************************************/
// SAVING
ostream & operator<<(ostream & out, const CWaves & waves)
{
	out << "(W)" << endl;
	out << waves.m_ReleasingWave << endl;
	for (const auto & wave : waves.m_Waves)
	{
		out << '[';
		for (const auto & troop : wave)
			out << *troop;
		out << string(waves.m_MaxSize - wave.size(), ' ') << ']' << endl;
	}
	return out << endl;
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
	if (++m_Selected == m_Waves.size())
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
	for (size_t i = 0; i < m_Waves.size(); ++i)
	{
		if (m_Waves[i].empty())
			continue;
		CTrooper * troop = m_Waves[i].front();
		m_Waves[i].pop_front();
		m_ReleasingWave = true;
		troop->SetSpawn(i + 1);
		return troop;
	}
	return nullptr;
}

