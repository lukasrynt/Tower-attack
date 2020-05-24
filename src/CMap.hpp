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
	explicit CMap();
	CMap(const CMap & src) = delete;
	CMap & operator=(const CMap & src) = delete;
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
	bool WaveIsRunning() const
	{return !m_Troops.empty();}
	CMap & PlaceTroops();
	CMap & PlaceTowers();
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
	 * @param windowWidth Size of the window to be rendered into
	 * @return Created buffer
	 */
	CBuffer CreateBuffer(size_t width) const;

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
	
	// TESTING STUFF
	void VisualizePath(pos_t start, pos_t goal);
	void Visualize(const std::deque<pos_t> & positions);
private:
	// VARIABLES
	CGate m_Gate;
	int m_Cols;													//!< map's columns
	int m_Rows;													//!< map's rows
	size_t m_TowerCount;
	std::vector<std::shared_ptr<CTrooper>> m_Troops;			//!< pointers to troopers on the map
	std::deque<std::shared_ptr<CTower>> m_Towers;				//!< pointers to towers on the map
	std::unordered_map<pos_t,std::shared_ptr<CTile>> m_Map; 	//!< two dimensional map
	std::map<int, pos_t> m_Spawns;								//!< spawns on the map mapped to their indexes
	std::map<pos_t, std::deque<pos_t>> m_Paths;					//!< paths from spawn points to finish mapped to positions of spawns
	std::shared_ptr<CUnitStack> m_UnitStack;					//!< unit stack containing all towers and troopers templates
	
	// LOADING
	CMap & LoadMap(std::istream & in);
	CMap & LoadMapInfo(std::istream & in);
	CMap & LoadEntities(std::istream & in);
	void LoadWallLine(std::istream & in, int row);
	void LoadMapCenter(std::istream & in, int row);
	bool LoadWallChar(char ch, pos_t position);
	bool LoadCenterChar(char ch, pos_t position);
	bool LoadEntity(pos_t position, char ch);
	static void DeleteWs(std::istream & in);
	bool InitSpawner(pos_t position, char ch);
	bool InitGatePosition(pos_t position);
	bool FindPathsFromSpawn();
	
	
	void LoadTroops(std::istream & in, char ch);
	void LoadTowers(std::istream & in, char ch);
	
	// RENDER
	CBuffer RenderMap(size_t width) const;
	
	// SAVING
	void SaveMapInfo(std::ostream & out) const;
	void SaveMap(std::ostream & out) const;
	void SaveEntities(std::ostream & out) const;
	
	// UPDATE PHASE
	bool CheckTroopsDeaths();
	bool MoveOnMap(std::shared_ptr<CTrooper> & troop);
	bool MoveTroops();
	bool TowerAttack();
};