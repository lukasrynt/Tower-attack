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
#include "Colors.hpp"
#include "CPath.hpp"
#include "CUnitStack.hpp"

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
	~CMap();
	CMap(const CMap & src) = delete;
	CMap & operator=(const CMap & src) = delete;
	void AssignUnitStack(std::shared_ptr<CUnitStack> unitStack);
	
	// LOADING
	/**
	 * Loads map from input file
	 * @param in Input file stream
	 * @param saved Saved is true when we load saved game - there are different allowed characters in this case
	 */
	void LoadMap(std::istream & in, bool saved);
	/**
	 * Checks if the count provided is equivalent to number of count in map
	 * @param count Count of spawns
	 */
	void CheckSpawnCount(int count) const;
	void SetMapDimensions(int rows, int cols);
	void SetGateHealth(int hp);
	
	// SAVING
	bool Save(const char * filename) const;
	friend std::ostream & operator<<(std::ostream & out, const CMap & map);

	// RENDER
	void Render() const;
	void RenderMap() const;
	void RenderGate() const;

	// UPDATE
	void Spawn(CTrooper * trooper);
	bool Update(bool & waveOn);
	
	void VisualizePath(std::queue<pos_t> path);
private:
	// VARIABLES
	int m_Cols;									//!< map's columns
	int m_Rows;									//!< map's rows
	int m_GateMaxHp;							//!< number of gate's max health
	int m_GateHp;								//!< number of current gate's health
	pos_t m_Gate;								//!< pointer to the gate, to which the troopers must get
	std::deque<CTrooper*> m_Troops;				//!< pointers to troopers on the map
	std::deque<CTower*> m_Towers;				//!< pointers to towers on the map
	std::unordered_map<pos_t,CTile> m_Map; 		//!< two dimensional map
	std::map<int, pos_t> m_Spawns;				//!< spawns on the map mapped to their indexes
	std::map<pos_t, std::deque<pos_t>> m_Paths;	//!< paths from spawn points to finish mapped to positions of spawns
	std::shared_ptr<CUnitStack> m_UnitStack;	//!< unit stack containing all towers and troopers templates
	
	// LOADING
	void AddToMap(pos_t position, char ch, bool saved);
	void AddFromSaved(const CTile & tile);
	void InitSpawner(pos_t position, char ch);
	void InitGatePosition(pos_t position);
	
	// UPDATE PHASE
	void MoveTroops(bool & waveOn);
	void TowerAttack();
	void FindPaths();
	void DamageGate(int damage);
	bool GateDefeated() const;
};