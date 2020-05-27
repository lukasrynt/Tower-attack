/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 24.4.2020
 */

#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <queue>
#include <list>
#include <memory>
#include "CTrooper.hpp"
#include "CTower.hpp"
#include "escape_sequences.h"
#include "CPath.hpp"
#include "CUnitStack.hpp"
#include "CGate.hpp"
#include "CBuffer.hpp"

/**
 * Map with troopers, walls, spawn points and towers
 * Takes care of rendering map, updating all units
 * and evaluating game state.
 */
class CMap
{
public:
	// INIT
	/**
	 * Assigns unit stack to map
	 * @param unitStack Pointer to CUnitStack that we want to assign
	 */
	void AssignUnitStack(std::shared_ptr<CUnitStack> unitStack);
	
	// LOADING
	/**
	 * Loads map from input file stream
	 * @param in Input stream
	 * @param self Map to be loaded
	 * @return in
	 */
	friend std::istream & operator>>(std::istream & in, CMap & self);
	/**
	 * Check if new map was loaded correctly. That means there are no turrets or attackers and there is a gate and at least one spawn.
	 * @return true if the map was correctly loaded.
	 */
	bool CheckNew() const;
	/**
	 * Check if the saved map was loaded correctly. That means there is at least one spawn and a gate
	 * @return
	 */
	bool CheckSaved() const;
	/**
	 * Checks if the spawn count is the same the number provided
	 * @param count Count of waves provided from CWaves
	 * @return true if the numbers are the same
	 */
	bool CheckSpawnCount(int count) const;
	/**
	 * @return True if the wave is currently running
	 */
	bool WaveIsRunning() const
	{return !m_Troops.empty();}
	/**
	 * Places loaded troops on map
	 * @return Reference to this
	 */
	CMap & PlaceTroops();
	/**
	 * Places loaded towers on map
	 * @return Reference to this
	 */
	CMap & PlaceTowers();
	/**
	 * Generate towers and their positions on map
	 * @return Reference to this
	 */
	CMap & GenerateTowers();
	
	// SAVING
	/**
	 * Saves the map on the given output stream
	 * @param out Output stream
	 * @param self Map to be saved
	 * @return out
	 */
	friend std::ostream & operator<<(std::ostream & out, const CMap & self);

	// RENDER
	/**
	 * Create buffer for further renderings
	 * @param width Size of the window
	 * @return Created buffer
	 */
	CBuffer Draw(size_t width) const;

	// UPDATE
	/**
	 * Spawns the give troops on the map
	 * @param spawns Troopers that were released from CWaves
	 */
	void Spawn(std::vector<std::unique_ptr<CTrooper>> & spawns);
	/**
	 * Updates the map. Moves troopers and executes tower attacks
	 * @param waveOn Signal that the wave is on, will be set to false if all troops on the map have either died or reached end
	 * @return false if we won the game - the gate has fallen
	 */
	bool Update(bool & waveOn);
	/**
	 * Checks whether the spawns are occupied or not - the spot next to them is free
	 * @return map of spawns idx and their occupancy status
	 */
	std::map<int, bool> SpawnsFree() const;
	
	// TESTING
	/**
	 * Visualizes path from start to goal
	 * @param start Start position
	 * @param goal Goal position
	 */
	void Visualize(pos_t start, pos_t goal);
	/**
	 * Visualizes positions on the map
	 * @param positions Positions to be visualized
	 */
	void Visualize(const std::deque<pos_t> & positions);
private:
	// VARIABLES
	CGate m_Gate;												//!< The enemy
	int m_Cols = 0;												//!< Map's columns
	int m_Rows = 0;												//!< Map's rows
	size_t m_TowerCount = 0;									//!< Number of towers we want to place
	std::vector<std::shared_ptr<CTrooper>> m_Troops;			//!< Troopers on the map - for easier access
	std::deque<std::shared_ptr<CTower>> m_Towers;				//!< Towers on the map - for easier access
	std::unordered_map<pos_t,std::shared_ptr<CTile>> m_Map; 	//!< Two dimensional map
	std::map<int, pos_t> m_Spawns;								//!< Spawns on the map mapped to their indexes
	std::map<pos_t, std::deque<pos_t>> m_Paths;					//!< Paths from spawn points to finish mapped to positions of spawns
	std::shared_ptr<CUnitStack> m_UnitStack;					//!< Unit stack containing all towers and troopers templates
	
	// LOADING
	/**
	 * Loads map from input stream
	 * @param in Input stream
	 * @return Reference to this
	 * @throws invalid_format when no path from spawner to gate has been found
	 */
	CMap & LoadMap(std::istream & in);
	/**
	 * Loads map information in the following format.
	 * m_Rows m_Cols m_Gate m_TowerCount
	 * @param in Input stream
	 * @return Reference to this
	 */
	CMap & LoadMapInfo(std::istream & in);
	/**
	 * Loads entities that were previously on the map along with their counters and positions
	 * @param in Input stream
	 * @return Reference to this
	 * @throws invalid_format if unit stack wan't earlier defined
	 */
	CMap & LoadEntities(std::istream & in);
	/**
	 * Loads a single line of wall characters - there needs to be line around the edges so that no trooper can escape
	 * @param in Input stream
	 * @param row Current row
	 * @throws invalid_format if invalid character is read
	 */
	void LoadWallLine(std::istream & in, int row);
	/**
	 * Loads single line in the center of the map - there can be any (allowed) character
	 * @param in Input stream
	 * @param row Current row
	 * @throws invalid_format if invalid character is read
	 */
	void LoadMapCenter(std::istream & in, int row);
	/**
	 * Checks the character that is allowed in walls (wall, spawn, gate)
	 * @param ch Character
	 * @param position Current position
	 * @return True if the character can be present in wall
	 * @throws invalid_format if invalid character is read
	 */
	bool LoadWallChar(char ch, pos_t position);
	/**
	 * Checks any character that is allowed on the map (that is wall characters + entities and space)
	 * @param ch Character
	 * @param position Current position
	 * @return True if the character is allowed
	 */
	bool LoadCenterChar(char ch, pos_t position);
	/**
	 * Checks entity character and skips it (replace tower with wall and trooper with space).
	 * Entities themselves are loaded independently
	 * @param position Current position
	 * @param ch Character
	 * @return True if the character is allowed
	 */
	bool LoadEntityChar(pos_t position, char ch);
	/**
	 * Initializes the spawner and checks if it hasn't been defined already (we can only have one 1 spawner etc.)
	 * @param position Current position
	 * @param ch Character
	 * @return True if the spawner is alright
	 */
	bool InitSpawner(pos_t position, char ch);
	/**
	 * Initializes gate position and check if it hasn't been defined already
	 * @param position Current position
	 * @return True if the gate is alright
	 */
	bool InitGatePosition(pos_t position);
	/**
	 * Looks for paths from spawner to gate
	 * @return True if all paths have been found
	 */
	bool FindPathsFromSpawner();
	/**
	 * Loads trooper in the entity section, clones it from template and saves it to troops vector
	 * @param in Input stream
	 * @param ch Character of the trooper
	 * @throws invalid_format if trooper is placed on inappropriate place
	 */
	void LoadTroops(std::istream & in, char ch);
	/**
	 * Loads trooper in the entity section, clones it from template and saves it to towers vector
	 * @param in Input stream
	 * @param ch Character of the tower
	 * @throws invalid_format if tower is placed on inappropriate place
	 */
	void LoadTowers(std::istream & in, char ch);
	
	// DRAW
	/**
	 * Creates buffer with map
	 * @param width Width of the screen
	 * @return Created buffer
	 */
	CBuffer DrawMap(size_t width) const;
	/**
	 * Creates buffer with troops currently walking on the map
	 * @param width Width of the screen
	 * @return Created buffer
	 */
	CBuffer DrawTroopsOnMap(size_t width) const;
	
	// SAVING
	/**
	 * Saves map info in the following format:
	 * m_Rows m_Cols m_Gate m_TowerCount
	 * @param out Output stream
	 */
	void SaveMapInfo(std::ostream & out) const;
	/**
	 * Saves map to output stream
	 * @param out Output stream
	 */
	void SaveMap(std::ostream & out) const;
	/**
	 * Saves all entities on the map along with their current status (frames, hp, position)
	 * @param out Output stream
	 */
	void SaveEntities(std::ostream & out) const;
	
	// UPDATE PHASE
	/**
	 * Checks if some of the troopers hasn't died and if they did, they will be removed
	 * @return False if no trooper is left standing - the wave is not running anymore
	 */
	bool CheckTroopsDeaths();
	/**
	 * Moves trooper on the map
	 * @param troop Troop to be moved
	 * @return True if the trooper has reached the gate
	 */
	bool MoveOnMap(std::shared_ptr<CTrooper> & troop);
	/**
	 * Move all troops and the map and damage gate if necessary
	 * @return True if the wave is still running - some troopers still haven't reached the gate
	 */
	bool MoveTroops();
	/**
	 * Attack with individual towers
	 * @return False if no trooper is left standing - the wave is not running anymore
	 */
	bool TowerAttack();
};