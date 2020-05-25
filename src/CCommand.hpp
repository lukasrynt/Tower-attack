/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 05/05/2020.
*/

#pragma once

#include <functional>
#include <utility>

/**
 * Command from the user with help associated with it
 */
class CCommand
{
public:
	explicit CCommand(std::function<void()> command, std::string help = "", std::string color = "")
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
	std::function<void()> m_Command;	//!< Command to be executed
	std::string m_Help;					//!< Help for the command
	std::string m_Color;				//!< Color of the command help
};