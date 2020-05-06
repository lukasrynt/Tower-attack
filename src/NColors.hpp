/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <string>

/**
 * Useful string constants to make terminal more colorful
 */
namespace Colors
{
	const std::string BG_BLACK		=	"\033[1;40m";	//!< Sets the background to black color.
	const std::string BG_RED		=	"\033[1;41m";	//!< Sets the background to red color.
	const std::string BG_GREEN		= 	"\033[1;42m";	//!< Sets the background to green color.
	const std::string BG_YELLOW		=	"\033[1;43m";	//!< Sets the background to yellow color.
	const std::string BG_BLUE 		=	"\033[1;44m";	//!< Sets the background to blue color.
	const std::string BG_MAGENTA 	= 	"\033[1;45m";	//!< Sets the background to magenta color.
	const std::string BG_CYAN  		=	"\033[1;46m";	//!< Sets the background to cyan color.
	const std::string BG_WHITE   	= 	"\033[1;47m";	//!< Sets the background to white color.
	const std::string FG_BLACK		=	"\033[1;30m";	//!< Sets the foreground to black color.
	const std::string FG_RED		=	"\033[1;31m";	//!< Sets the foreground to red color.
	const std::string FG_GREEN		= 	"\033[1;32m";	//!< Sets the foreground to green color.
	const std::string FG_YELLOW		= 	"\033[1;33m";	//!< Sets the foreground to yellow color.
	const std::string FG_BLUE		= 	"\033[1;34m";	//!< Sets the foreground to blue color.
	const std::string FG_MAGENTA	= 	"\033[1;35m";	//!< Sets the foreground to magenta color.
	const std::string FG_CYAN		= 	"\033[1;36m";	//!< Sets the foreground to cyan color.
	const std::string FG_WHITE		= 	"\033[1;37m";	//!< Sets the foreground to white color.
	const std::string RESET 		= 	"\033[0m";		//!< Resets previously selected colors.
}