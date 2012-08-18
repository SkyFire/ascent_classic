/**
 * Summit MMORPG Server Software
 * Copyright (c) 2008 Summit Server Team
 * See COPYING for license details.
 */

#ifndef __WORLD_STATE_MANAGER_H
#define __WORLD_STATE_MANAGER_H

/*
 * World State Manager Class
 * See doc/World State Manager.txt for implementation details
 */

// forward declaration for mapmgr
class MapMgr;

// some defines
#define FACTION_MASK_ALL -1
#define ZONE_MASK_ALL -1

class SERVER_DECL WorldStateManager
{
	struct WorldState
	{
		//uint32 StateId;		// index in map
		int32 FactionMask;
		int32 ZoneMask;
		uint32 Value;
	};

	typedef map<uint32, WorldState> WorldStateMap;

	// storing world state variables
	WorldStateMap m_states;

	// mapmgr we are working with.
	MapMgr& m_mapMgr;

	// synchronization object
	// shouldn't REALLY be needed, but we're paranoid..
	//Mutex m_lock;

	// public methods
public:

	// constructor, not much to do though, except set mapmgr reference
	WorldStateManager(MapMgr &mgr) : m_mapMgr(mgr) {}

	// bhoom! all cleaned up by C++ automatically
	~WorldStateManager() {}

	// initializes a world state.
	// use only at map creation.
	// it will do error checking and re-initialize existing elements, however it is a bad practice!
	// creating a world state will not send it to the player! this is because all world state creation should be done
	// at instance creation, not in the middle of it running!
	void CreateWorldState(uint32 uWorldStateId, uint32 uInitialValue, int32 iFactionMask = FACTION_MASK_ALL, int32 iZoneMask = ZONE_MASK_ALL);

	// updates a world state (sets and stores value, updates in clients on map)
	// NOTE: If the world state is not created/initialized, it WILL NOT BE UPDATED/SENT to the client!!!! 
	void UpdateWorldState(uint32 uWorldStateId, uint32 uValue);

	// sends the current world states to a new player on the map.
	// this should also be called upon changing zone.
	void SendWorldStates(Player *pPlayer);

	// clears world states for a player leaving the map.
	void ClearWorldStates(Player *pPlayer);

	// loads a setting from the database.
	static const string GetPersistantSetting(const char *szKeyName, const char *szValue);
	static void SetPersistantSetting(const char *szKeyName, const char *szValue);
};

struct WorldStateTemplate
{
	uint32 uField;
	int32 iFactionMask;
	int32 iZoneMask;
	uint32 uValue;
};

#define WST_NUM_MAPS 600
class WorldStateTemplateManager : public Singleton<WorldStateTemplateManager>
{
	typedef list<WorldStateTemplate> WorldStateTemplateList;
	WorldStateTemplateList m_templatesForMaps[WST_NUM_MAPS];
public:

	// loads predefined fields from database 
	void LoadFromDB();

	// applys a map template to a new instance
	void ApplyMapTemplate(MapMgr *pmgr);
};

#define sWorldStateTemplateManager WorldStateTemplateManager::getSingleton()

#endif			// __WORLD_STATE_MANAGER_H
