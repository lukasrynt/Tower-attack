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
#include "NColors.hpp"
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
	~CMap();
	CMap(const CMap & src) = delete;
	CMap & operator=(const CMap & src) = delete;
	void AssignUnitStack(std::shared_ptr<CUnitStack> unitStack);
	
	// LOADING
	friend std::istream & operator>>(std::istream & in, CMap & self);
	bool CheckNew() const;
	bool CheckSaved() const;
	
	bool CheckSpawnCount(int count) const;
	void PlaceTroops();
	bool WaveIsRunning() const
	{return !m_Troops.empty();}
	
	// SAVING
	friend std::ostream & operator<<(std::ostream & out, const CMap & self);

	// RENDER
	CBuffer Render(int windowWidth) const;

	// UPDATE
	void Spawn(const std::vector<CTrooper*> & spawns);
	bool Update(bool & waveOn);
	std::map<int, bool> SpawnsFree() const;
	
	void VisualizePath(pos_t start, pos_t goal);
	void Visualize(const std::deque<pos_t>& positions);
private:
	// VARIABLES
	CGate m_Gate;
	int m_Cols;									//!< map's columns
	int m_Rows;									//!< map's rows
	std::vector<CTrooper*> m_Troops;			//!< pointers to troopers on the map
	std::deque<CTower*> m_Towers;				//!< pointers to towers on the map
	std::unordered_map<pos_t,CTile> m_Map; 		//!< two dimensional map
	std::map<int, pos_t> m_Spawns;				//!< spawns on the map mapped to their indexes
	std::map<pos_t, std::deque<pos_t>> m_Paths;	//!< paths from spawn points to finish mapped to positions of spawns
	std::shared_ptr<CUnitStack> m_UnitStack;	//!< unit stack containing all towers and troopers templates
	
	// LOADING
	void LoadMap(std::istream & in);
	void LoadMapInfo(std::istream & in);
	void LoadEntities(std::istream & in);
	
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
	CBuffer RenderMap(int windowWidth) const;
	
	// SAVING
	void SaveMapInfo(std::ostream & out) const;
	void SaveMap(std::ostream & out) const;
	void SaveEntities(std::ostream & out) const;
	
	// UPDATE PHASE
	bool CheckTroopsDeaths();
	bool MoveTroops();
	bool TowerAttack();
};