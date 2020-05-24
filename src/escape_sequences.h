/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <string>
#include <cstring>

/**
 * Useful string constants to make terminal more colorful
 */
namespace Colors
{
	// source: http://web.archive.org/web/20131009193526/http://bitmote.com/index.php?post/2012/11/19/Using-ANSI-Color-Codes-to-Colorize-Your-Bash-Prompt-on-Linux
	//		   https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html
	constexpr const char * const BG_BLACK		=	"\x1b[1;40m";	//!< Sets the background to black color.
	constexpr const char * const BG_RED			=	"\x1b[1;41m";	//!< Sets the background to red color.
	constexpr const char * const BG_GREEN		= 	"\x1b[1;42m";	//!< Sets the background to green color.
	constexpr const char * const BG_YELLOW		=	"\x1b[1;43m";	//!< Sets the background to yellow color.
	constexpr const char * const BG_BLUE 		=	"\x1b[1;44m";	//!< Sets the background to blue color.
	constexpr const char * const BG_MAGENTA 	= 	"\x1b[1;45m";	//!< Sets the background to magenta color.
	constexpr const char * const BG_CYAN  		=	"\x1b[1;46m";	//!< Sets the background to cyan color.
	constexpr const char * const BG_WHITE   	= 	"\x1b[1;47m";	//!< Sets the background to white color.
	constexpr const char * const FG_BLACK		=	"\x1b[1;30m";	//!< Sets the foreground to black color.
	constexpr const char * const FG_RED			=	"\x1b[1;31m";	//!< Sets the foreground to red color.
	constexpr const char * const FG_GREEN		= 	"\x1b[1;32m";	//!< Sets the foreground to green color.
	constexpr const char * const FG_YELLOW		= 	"\x1b[1;33m";	//!< Sets the foreground to yellow color.
	constexpr const char * const FG_BLUE		= 	"\x1b[1;34m";	//!< Sets the foreground to blue color.
	constexpr const char * const FG_MAGENTA		= 	"\x1b[1;35m";	//!< Sets the foreground to magenta color.
	constexpr const char * const FG_CYAN		= 	"\x1b[1;36m";	//!< Sets the foreground to cyan color.
	constexpr const char * const FG_WHITE		= 	"\x1b[1;37m";	//!< Sets the foreground to white color.
	constexpr const char * const RESET 			= 	"\x1b[0m";		//!< Resets previously selected colors.
}

namespace Escapes
{
	constexpr const char * const RESET_SCREEN	=	"\x1b[2J";
	constexpr const char * const BOLD_TEXT		=	"\x1b[1m";
	constexpr const char * const UNDERLINE		=	"\x1b[4m";
	constexpr const char * const SHOW_CURSOR	=	"\x1b[?25h";
	constexpr const char * const HIDE_CURSOR	=	"\x1b[?25l";
	constexpr const char * const RESET_CURSOR   = 	"\x1b[H";
	inline std::string REPOSITION_CURSOR(int row, int col)
	{return std::string("\x1b[") + std::to_string(row) + ";"+ std::to_string(col) + 'H';}
	inline std::string CHANGE_WINDOW_SIZE(int height, int width)
	{
		return std::string("\x1b[8;") + std::to_string(height) + ';' + std::to_string(width) + 't';
	}
}