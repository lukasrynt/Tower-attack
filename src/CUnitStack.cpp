/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */
#include "CUnitStack.hpp"

using namespace std;

/**********************************************************************************************************************/
// LOADING
istream & operator>>(istream & in, CUnitStack & stack)
{
	map<char,unique_ptr<CTrooper>> origTroops;
	map<char,unique_ptr<CTower>> origTowers;
	CUnitStack::CreateOriginals(origTroops, origTowers);
	while (true)
	{
		// load first character - type of unit
		char ch;
		in >> ch;
		
		// load coresponding unit
		if(!stack.LoadUnit(in, ch, origTroops, origTowers))
			break;
	}
	return in;
}

void CUnitStack::CreateOriginals (map<char,unique_ptr<CTrooper>> & origTroops, map<char,unique_ptr<CTower>> & origTowers)
{
	origTroops.insert({'T', make_unique<CTrooper>()});
	origTroops.insert({'A', make_unique<CArmoredTrooper>()});
	origTowers.insert({'R', make_unique<CArcherTower>()});
	origTowers.insert({'M', make_unique<CMageTower>()});
}

bool CUnitStack::LoadUnit(istream & in, char ch, const map<char,unique_ptr<CTrooper>> & origTroops, const map<char,unique_ptr<CTower>> & origTowers)
{
	if (origTroops.count(ch))
	{
		unique_ptr<CTrooper> trooper{origTroops.at(ch)->Clone()};
		trooper->LoadTemplate(in);
		if (!CharIsValid(trooper->GetChar()))
			return false;
		m_Troops.insert({trooper->GetChar(), move(trooper)});
	}
	else if (origTowers.count(ch))
	{
		unique_ptr<CTower> tower{origTowers.at(ch)->Clone()};
		tower->LoadTemplate(in);
		if (!CharIsValid(tower->GetChar()))
			return false;
		m_Towers.insert({tower->GetChar(), move(tower)});
	}
	else
	{
		in.putback(ch);
		return false;
	}
	return true;
}

bool CUnitStack::IsTowerChar(char ch) const
{
	for (const auto & tower : m_Towers)
		if (ch == tower.first)
			return true;
	return false;
}

bool CUnitStack::IsTrooperChar(char ch) const
{
	for (const auto & trooper : m_Troops)
		if (ch == trooper.first)
			return true;
	return false;
}

bool CUnitStack::CharIsValid(char ch)
{
	string forbidden(FORBIDDEN_CHARS);
	return forbidden.find(ch) == string::npos;
}

bool CUnitStack::Check() const
{
	return !m_Towers.empty()
		&& !m_Troops.empty();
}

/**********************************************************************************************************************/
// INTERFACE
unique_ptr<CTower> CUnitStack::CreateTowerAt(char ch) const
{
	if (m_Towers.count(ch))
		return m_Towers.at(ch)->Clone();
	return nullptr;
}

unique_ptr<CTrooper> CUnitStack::CreateTrooperAt(char ch) const
{
	if (m_Troops.count(ch))
		return m_Troops.at(ch)->Clone();
	return nullptr;
}

unique_ptr<CTrooper> CUnitStack::CreateSelected() const
{
	char ch = FindSelected();
	if (m_Troops.count(ch))
		return m_Troops.at(ch)->Clone();
	return nullptr;
}

CBuffer CUnitStack::CreateBuffer(int windowWidth) const
{
	return move(CBuffer{windowWidth}
						.Append("Units:", Colors::FG_CYAN)
						.Append(string(4 * m_Troops.size(), '-'), Colors::FG_CYAN)
						.Append(RenderTroops())
						.Append(string(4 * m_Troops.size(), '-'), Colors::FG_CYAN));
}

CBuffer CUnitStack::CreateTroopsInfoBuffer(int windowWidth) const
{
	CBuffer buffer{windowWidth};
	buffer.Append()
			.Append("Basic trooper", string(Colors::BG_YELLOW) + Colors::FG_BLACK)
			.Append(" ● Basic unit with limited health and options.", Colors::FG_YELLOW)
			.Append(" ● Will always take the shortest route to finish.", Colors::FG_YELLOW);
	for (const auto & troop : m_Troops)
		// for this way of formatting i see no other way than this, i want to have some common info on the troops and then the units
		if (troop.second->GetType() == ETileType::BASIC_TROOP)
			buffer.Append(troop.second->CreateInfoBuffer(windowWidth));
	
	
	buffer.Append("Basic trooper", string(Colors::BG_CYAN) + Colors::FG_BLACK)
			.Append(" ● Armored trooper can take more damage than normal troops.", Colors::FG_CYAN)
			.Append(" ● He can wall up to prevent incoming damage, before his shields deplete.", Colors::FG_CYAN);
	for (const auto & troop : m_Troops)
		if (troop.second->GetType() == ETileType::ARMORED_TROOP)
			buffer.Append(troop.second->CreateInfoBuffer(windowWidth));
	return buffer;
}

CBuffer CUnitStack::CreateTowersInfoBuffer(int windowWidth) const
{
	CBuffer buffer{windowWidth};
	buffer.Append()
			.Append("Archer tower", string(Colors::BG_RED) + Colors::FG_BLACK)
			.Append(" ● Basic tower with archer attacks.", Colors::FG_RED)
			.Append(" ● Can focus only one trooper at once and it will be always the closest.", Colors::FG_RED);
	for (const auto & tower : m_Towers)
		if (tower.second->GetType() == ETileType::ARCHER_TOWER)
			buffer.Append(tower.second->CreateInfoBuffer(windowWidth));
	
	buffer.Append("Mage tower", string(Colors::BG_BLUE) + Colors::FG_BLACK)
			.Append(" ● Mage tower which can cast magic attacks", Colors::FG_BLUE)
			.Append(" ● Magic wave is the main attack the tower can cast. It will damage troopers in radius around the tower.",
					Colors::FG_BLUE);
	for (const auto & tower : m_Towers)
		if (tower.second->GetType() == ETileType::MAGE_TOWER)
			buffer.Append(tower.second->CreateInfoBuffer(windowWidth));
	return buffer;
}

string CUnitStack::RenderTroops() const
{
	size_t idx = 0;
	string line;
	for (const auto & troop : m_Troops)
	{
		line += ' ';
		if (idx++ == m_Selected)
			line += Colors::BG_CYAN;
		line += troop.second->GetChar() + string(Colors::RESET) + string(2, ' ');
	}
	return line;
}

int CUnitStack::GetSelectedPrice() const
{
	return m_Troops.at(FindSelected())->GetPrice();
}

char CUnitStack::FindSelected() const
{
	size_t idx = 0;
	for (const auto & troop : m_Troops)
		if (idx++ == m_Selected)
			return troop.first;
	return 0;
}

void CUnitStack::Cycle() const
{
	if (++m_Selected == m_Troops.size())
		m_Selected = 0;
}

bool CUnitStack::Lost(int resources) const
{
	for (const auto & troop : m_Troops)
		if (resources - troop.second->GetPrice() >= 0)
			return false;
	return true;
}

/**********************************************************************************************************************/
// SAVING
ostream & operator<<(ostream & out, const CUnitStack & stack)
{
	out << "(U)" << endl;
	for (auto & troop : stack.m_Troops)
		troop.second->SaveTemplate(out) << endl;
	
	for (auto & tower : stack.m_Towers)
		tower.second->SaveTemplate(out) << endl;
	return out << endl;
}

string CUnitStack::GetTowerChars() const {
	string message;
	for (const auto & tower : m_Towers)
		message += tower.first;
	return message;
}
