/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

/**
 * Useful string constants to make terminal more colorful
 */
namespace Colors
{
	constexpr const char * const BG_BLACK	=	"\033[1;40m";	//!< Sets the background to black color.
	constexpr const char * const BG_RED		=	"\033[1;41m";	//!< Sets the background to red color.
	constexpr const char * const BG_GREEN	= 	"\033[1;42m";	//!< Sets the background to green color.
	constexpr const char * const BG_YELLOW	=	"\033[1;43m";	//!< Sets the background to yellow color.
	constexpr const char * const BG_BLUE 	=	"\033[1;44m";	//!< Sets the background to blue color.
	constexpr const char * const BG_MAGENTA = 	"\033[1;45m";	//!< Sets the background to magenta color.
	constexpr const char * const BG_CYAN  	=	"\033[1;46m";	//!< Sets the background to cyan color.
	constexpr const char * const BG_WHITE   = 	"\033[1;47m";	//!< Sets the background to white color.
	
	constexpr const char * const FG_BLACK	=	"\033[1;30m";	//!< Sets the foreground to black color.
	constexpr const char * const FG_RED		=	"\033[1;31m";	//!< Sets the foreground to red color.
	constexpr const char * const FG_GREEN	= 	"\033[1;32m";	//!< Sets the foreground to green color.
	constexpr const char * const FG_YELLOW	= 	"\033[1;33m";	//!< Sets the foreground to yellow color.
	constexpr const char * const FG_BLUE	= 	"\033[1;34m";	//!< Sets the foreground to blue color.
	constexpr const char * const FG_MAGENTA	= 	"\033[1;35m";	//!< Sets the foreground to magenta color.
	constexpr const char * const FG_CYAN	= 	"\033[1;36m";	//!< Sets the foreground to cyan color.
	constexpr const char * const FG_WHITE	= 	"\033[1;37m";	//!< Sets the foreground to white color.
	
	constexpr const char * const RESET 		= 	"\033[0m";		//!< Resets previously selected colors.
}