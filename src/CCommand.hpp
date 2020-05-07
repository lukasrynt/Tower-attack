/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 05/05/2020.
*/

#pragma once

#include <functional>
#include <utility>


class CCommand
{
public:
	CCommand(std::function<void()> command, std::string help = "", std::string color = "")
		: m_Command(std::move(command)),
		  m_Help(std::move(help)),
		  m_Color(std::move(color))
	{}
	
	void Execute()
	{m_Command();}
	
	std::string Help() const
	{return m_Help;}
	
	std::string Color() const
	{return m_Color;}

private:
	std::function<void()> m_Command;
	std::string m_Help;
	std::string m_Color;
};