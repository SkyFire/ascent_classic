/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
// WorldCreator.h
//

#ifndef __WORLDCREATOR_H
#define __WORLDCREATOR_H

enum INSTANCE_TYPE
{
	INSTANCE_NULL,
	INSTANCE_RAID,
	INSTANCE_NONRAID,
	INSTANCE_PVP,
	INSTANCE_MULTIMODE,
};

enum INSTANCE_MODE
{
    MODE_NORMAL = 0,
    MODE_HEROIC = 1,
    MODE_EPIC   = 2,
};

enum INSTANCE_ABORT_ERROR
{
	INSTANCE_ABORT_ERROR	 = 0x00,
	INSTANCE_ABORT_FULL	  = 0x01,
	INSTANCE_ABORT_NOT_FOUND = 0x02,
	INSTANCE_ABORT_TOO_MANY  = 0x03,
	INSTANCE_ABORT_ENCOUNTER = 0x04,
};

#define INSTANCE_OK  INSTANCE_ABORT_TOO_MANY + 0x01

class Map;
class MapMgr;

class Object;
class Group;
class Player;
class MapUpdaterThread;
class Battleground;

// first = mapid
// first = instance id
typedef std::map<uint32, std::map<uint32, std::pair<Map*, Battleground*> > > BattlegroundInstanceMap;

class SERVER_DECL FormationMgr : public Singleton < FormationMgr >
{
	map<uint32, Formation*> m_formations;
public:
	typedef std::map<uint32, Formation*> FormationMap;
    FormationMgr();
	~FormationMgr();

	Formation * GetFormation(uint32 sqlid)
	{
		FormationMap::iterator itr = m_formations.find(sqlid);
		return (itr == m_formations.end()) ? 0 : itr->second;
	}
};
class SERVER_DECL WorldCreator :  public Singleton < WorldCreator >
{
public:
	WorldCreator(TaskList * tl);	
	~WorldCreator();

	Map* GetMap(uint32 mapid);
	MapMgr * GetInstance(uint32 mapid, Object* obj);
	MapMgr * GetInstance(uint32 mapid, uint32 instanceId);
	MapMgr * GetInstance(uint32 instanceId);
	MapMgr * ISMGetInstanceBeforeRemoval(uint32 InstanceID, uint32 mapid, bool Lock);
	MapMgr * GetInstanceByGroup(Group *pGroup, Player *pPlayer, MapInfo *pMapInfo);
	MapMgr * GetInstanceByCreator(Player *pCreator, MapInfo *pMapInfo);

	//Normal instance management
	uint32 CreateInstance(Group *pGroup, Player *pPlayer, uint32 mapid, uint32 instanceid = 0, uint32 creation = 0, MapMgr ** destptr = 0, uint32 difficulty = 0);
	uint32 CreateInstance(uint32 mapid, uint32 instanceid, MapMgr ** destptr);

    void DeleteInstance(uint32 instanceid, uint32 mapid);
	bool CheckInstanceForObject(Object *obj, MapInfo *pMapinfo);

	uint32 GenerateInstanceID();
	void InstanceSoftReset(MapMgr *mMapMgr);
	void InstanceHardReset(MapMgr *mMapMgr);

	void BuildXMLStats(char * m_file);
	void CreateMap(uint32 mapid) { _CreateMap(mapid); }

private:
	Map* _CreateMap(uint32 mapid);
	std::map<uint32, Map*> _maps;

	uint32 m_InstanceHigh;

	//thread locks
	Mutex _busy;
};

#define sWorldCreator WorldCreator::getSingleton()
#define sInstanceSavingManager InstanceSavingManagement::getSingleton()

#endif
