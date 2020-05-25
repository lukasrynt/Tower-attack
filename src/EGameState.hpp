/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 01/05/2020.
*/

#pragma once

/**
 * State of the game
 */
enum class EGameState
{
	GAME_RUNNING,	//!< Game is running
	GAME_OVER,		//!< Game has ended with player losing
	GAME_WON,		//!< Game has ended with player winning
	GAME_QUIT		//!< Player has quit the game
};