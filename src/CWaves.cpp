/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#include "CWaves.hpp"
#include "escape_sequences.h"
#include "CArmoredTrooper.hpp"
#include "CMageTower.hpp"
#include "ExInvalidInput.hpp"

using namespace std;

/**********************************************************************************************************************/
// INIT
CWaves::CWaves() noexcept
	: m_Waves(0),
	  m_Selected(0),
	  m_MaxSize(0),
	  m_Frames(0),
	  m_ReleasingWave(false),
	  m_Resources(0)
{}

void CWaves::AssignUnitStack(shared_ptr<CUnitStack> unitStack)
{m_UnitStack = move(unitStack);}

/**********************************************************************************************************************/
// LOADING
istream & operator>>(istream & in, CWaves & waves)
{
	char ch;
	int tmp;
	in >> waves.m_ReleasingWave >> tmp >> waves.m_Frames >> waves.m_Resources;
	waves.m_Waves = vector<deque<unique_ptr<CTrooper>>>(tmp);
	size_t pos = 0;
	
	while (pos < 10)
	{
		// if we have read something else than [ on the beggining of the row we need to quit
		in >> ch;
		if (ch != '[')
		{
			in.putback(ch);
			break;
		}
		
		// load the troopers in the wawes
		waves.m_Waves[pos++] = waves.LoadWaves(in);
	}
	return in;
}

deque<unique_ptr<CTrooper>> CWaves::LoadWaves(istream & in)
{
	if (!m_UnitStack)
		in.setstate(ios::failbit);
	
	deque<unique_ptr<CTrooper>> wave;
	size_t counter = 0;
	while (true)
	{
		// check if the character is correct
		char ch = in.get();
		
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
			wave.emplace_back(m_UnitStack->CreateTrooperAt(ch));
		}
		else
			in.setstate(ios::failbit);
	}
	
	if(!CheckCounter(counter))
		in.setstate(ios::failbit);
	return wave;
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

bool CWaves::CheckNew() const
{
	for (const auto & wave : m_Waves)
		if (!wave.empty())
			return false;
		
	return !m_ReleasingWave;
}

/**********************************************************************************************************************/
// SAVING
ostream & operator<<(ostream & out, const CWaves & waves)
{
	out << "(W)" << endl;
	out << waves.m_ReleasingWave << ' ' << waves.m_Frames << ' ' << waves.m_Resources << endl;
	for (const auto & wave : waves.m_Waves)
	{
		out << '[';
		for (const auto & troop : wave)
			out << troop->GetChar();
		out << string(waves.m_MaxSize - wave.size(), ' ') << ']' << endl;
	}
	return out << endl;
}

/**********************************************************************************************************************/
// RENDER
CBuffer CWaves::CreateBuffer(int windowWidth) const
{
	CBuffer buffer{windowWidth};
	buffer.Append("Waves:", Colors::FG_GREEN)
			.Append(string(10 + m_MaxSize, '-'), Colors::FG_GREEN);
	
	size_t idx = 0;
	for (const auto & wave : m_Waves)
	{
		bool color = false;
		if (idx == m_Selected)
			color = true;
		string line = "Wave "s + to_string(++idx) + ": [";
		for (const auto & troop : wave)
			line += troop->GetChar();
		line += string(m_MaxSize - wave.size(), ' ') + "]";
		buffer.Append(line, color ? Colors::BG_GREEN : "");
	}
	buffer.Append(string(10 + m_MaxSize, '-'), Colors::FG_GREEN)
			.Append(to_string(m_Resources))
			.Append(" ©", Colors::FG_YELLOW);
	return buffer;
}

/**********************************************************************************************************************/
// INPUT PROCESSING
void CWaves::Cycle()
{
	if (++m_Selected == m_Waves.size())
		m_Selected = 0;
}

void CWaves::Release(bool & waveOn)
{
	// check if there isn't any running wave
	if (waveOn)
		throw invalid_input("Wave already running.");
	
	// release only if there is something in the wawes
	if (!Empty())
	{
		m_ReleasingWave = true;
		waveOn = true;
	}
	else
		throw invalid_input("Cannot start an empty wave");
}

void CWaves::AddTroop()
{
	if (m_ReleasingWave)
		throw invalid_input("Wave is already running.");
	
	if (m_Waves[m_Selected].size() == m_MaxSize)
		throw invalid_input("Current wave is full");
	
	int price = m_UnitStack->GetSelectedPrice();
	if (m_Resources - price < 0)
		throw invalid_input("Cannot afford unit.");
	
	// add trooper to the wave
	m_Resources -= price;
	m_Waves[m_Selected].emplace_back(m_UnitStack->CreateSelected());
}

void CWaves::DeleteTroop()
{
	if (m_ReleasingWave)
		throw invalid_input("Wave is alredy running.");
	
	if (m_Waves[m_Selected].empty())
		throw invalid_input("Current wave is empty.");
	
	// delete trooper from the back
	m_Resources += m_Waves[m_Selected].back()->GetPrice();
	m_Waves[m_Selected].pop_back();
}

/**********************************************************************************************************************/
// UPDATE
vector<unique_ptr<CTrooper>> CWaves::Update(const map<int,bool> & spawnsFree)
{
	// if 'p' wasn't pressed or we have yet to wait for action, return
	vector<unique_ptr<CTrooper>> res;
	if (!m_ReleasingWave || !m_Frames.ActionAllowed())
		return res;

	
	// synchronous spawning
	for (size_t i = 0; i < m_Waves.size(); ++i)
	{
		// if there is unit in front of spawner or the wave is empty switch to next one
		if (!spawnsFree.at(i + 1) || m_Waves[i].empty())
			continue;
		
		// get the first trooper in wave
		unique_ptr<CTrooper> spawned = move(m_Waves[i].front());
		m_Waves[i].pop_front();
		spawned->SetSpawn(i + 1);
		res.emplace_back(move(spawned));
	}
	
	// check if there are still troops to release
	m_ReleasingWave = !Empty();
	return res;
}

bool CWaves::Lost() const
{
	return Empty() && m_UnitStack->Lost(m_Resources);
}

bool CWaves::Empty() const
{
	bool empty = true;
	for (const auto & wave : m_Waves)
		if (!wave.empty())
			empty = false;
	return empty;
}
