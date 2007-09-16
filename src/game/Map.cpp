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

// Class Map
// Holder for all instances of each mapmgr, handles transferring
// players between, and template holding.

#include "StdAfx.h"

Map::Map(uint32 mapid)
{
	memset(spawns,0,sizeof(CellSpawns*) * _sizeX);

	_mapInfo = WorldMapInfoStorage.LookupEntry(mapid);
	_mapId = mapid;

	bool instance;
	if(_mapInfo)
	{
		// for non-instances, create our one instance.
		instance= _mapInfo->type != INSTANCE_NULL;
	 } else 
	{
		instance = IS_INSTANCE(_mapId);
	}

	//new stuff Load Spawns
	LoadSpawns(false);

	// Setup terrain
	_terrain = new TerrainMgr(sWorld.MapPath, _mapId, instance);
	if(!_terrain->LoadTerrainHeader())
	{
		delete _terrain;
		_terrain = NULL;
	}

	if(!instance)
		CreateMapMgrInstance();

	// get our name
	me = sMapStore.LookupEntry(_mapId);
	if(me)
	{
		name = sMapStore.LookupString(me->real_name);
	}
	else
	{
		name = "Unknown";
	}
}

Map::~Map()
{
	sLog.outString("  Deleting all instances of map %u", _mapId);
	for(std::map<uint32, MapMgr*>::iterator it = _instances.begin();
		it != _instances.end(); ++it)
	{
		it->second->_shutdown = true;
		delete it->second;
	}
	delete _terrain;

	for(uint32 x=0;x<_sizeX;x++)
	{
		if(spawns[x])
		{
			for(uint32 y=0;y<_sizeY;y++)
			{
				if(spawns[x][y])
				{	
					CellSpawns * sp=spawns[x][y];
					for(CreatureSpawnList::iterator i = sp->CreatureSpawns.begin();i!=sp->CreatureSpawns.end();i++)
						delete (*i);
					for(GOSpawnList::iterator it = sp->GOSpawns.begin();it!=sp->GOSpawns.end();it++)
						delete (*it);

					delete sp;
					spawns[x][y]=NULL;
				}
			}
			delete [] spawns[x];
		}
	}

	for(CreatureSpawnList::iterator i = staticSpawns.CreatureSpawns.begin(); i != staticSpawns.CreatureSpawns.end(); ++i)
		delete *i;
	for(GOSpawnList::iterator i = staticSpawns.GOSpawns.begin(); i != staticSpawns.GOSpawns.end(); ++i)
		delete *i;
}

MapMgr * Map::GetInstance(uint32 instanceId)
{
	if(_mapInfo && _mapInfo->type == INSTANCE_NULL)
		return GetFirstInstance();
	else if(!IS_INSTANCE(_mapId ))
		return GetFirstInstance();
	return InstanceExists(instanceId);
}

MapMgr * Map::GetRawInstance(uint32 instanceid)
{
	listmutex.Acquire();
	InstanceMap::iterator itr = _instances.find(instanceid);
	if(itr == _instances.end())
	{
		listmutex.Release();
		return NULL;
	}
	MapMgr*rv = itr->second;
	listmutex.Release();
	return rv;
}

MapMgr * Map::InstanceExists(uint32 instanceId)
{
	listmutex.Acquire();
	// we called the wrong number? :P
	InstanceMap::iterator itr = _instances.find(instanceId);
	if(itr == _instances.end())
	{
		listmutex.Release();
		return NULL;
	}

	MapMgr*rv = itr->second;
	if(rv && rv->IsDeletionPending())
	{
		listmutex.Release();
		return NULL;
	}
	listmutex.Release();
	return rv;
}

MapMgr * Map::GetInstance(Object* obj)
{
	// lazy
	MapMgr * mapMgr = GetInstance(obj->GetInstanceID());
	if(!mapMgr)
	{
		// Oops, we're trying to join an invalid instance
		if(obj->GetTypeId() == TYPEID_PLAYER)
		{
			Player *plr = static_cast<Player*>(obj);
			sChatHandler.RedSystemMessage(plr->GetSession(), "You tried to join an invalid instance (%u on map %u). Repopping at %s.", obj->GetInstanceID(), _mapId, (plr->m_bgEntryPointX != 0.0f ? "entry point" : "exit battleground"));
			if(plr->m_bgEntryPointX != 0.0f)
			{
				plr->SetPosition(plr->m_bgEntryPointX, plr->m_bgEntryPointY, plr->m_bgEntryPointZ,
					plr->m_bgEntryPointO, true);
				plr->SetMapId(plr->m_bgEntryPointMap);
				plr->SetInstanceID(plr->m_bgEntryPointInstance);
			} else if(_mapInfo != NULL) {
				plr->SetMapId(_mapInfo->repopmapid);
				plr->SetPosition(_mapInfo->repopx, _mapInfo->repopy, _mapInfo->repopz, 3.14f);
				plr->SetInstanceID(0);
			} else {
				PlayerCreateInfo *Info = objmgr.GetPlayerCreateInfo(plr->getRace(), plr->getClass());
				plr->SetMapId(Info->mapId);
				plr->SetInstanceID(Info->mapId+1);
				plr->SetPosition(Info->positionX, Info->positionY, Info->positionZ, 0, true);
			}
			plr->_Relocate(plr->GetMapId(), plr->GetPosition(), true, true);
			return NULL;
		} else {
			// this will destroy the creature :p
			return 0;
		}
	}
	if(mapMgr && mapMgr->IsDeletionPending()) { return NULL; }
	return mapMgr;
}

MapMgr * Map::CreateMapMgrInstance(uint32 instanceid)
{
	uint32 instanceId;
	if(instanceid)
		instanceId = instanceid;
	else
		instanceId = sWorldCreator.GenerateInstanceID();

	MapMgr *mapMgr = new MapMgr(this, _mapId, instanceId);
	listmutex.Acquire(); 
	ASSERT(_instances.find(instanceId) == _instances.end());
	_instances[instanceId]=mapMgr;
	listmutex.Release();
	launch_thread(mapMgr);

	return mapMgr;
}

void Map::DestroyMapMgrInstance(uint32 instanceId)
{
	listmutex.Acquire();
	InstanceMap::iterator it = _instances.find(instanceId);
	ASSERT(it != _instances.end());

	sLog.outError("Deleting instance %u of map %u", instanceId, _mapId);
	if(it->second->thread_is_alive)
	{
		/*sLog.outString("Adding delete pending to mapmgr");
		Crash_Log->AddLine("Adding delete pending to mapmgr. Callstack:");
#ifdef WIN32
		CStackWalker ws;
		ws.ShowCallstack();
#endif*/
		it->second->delete_pending = true;
		it->second->SetThreadState(THREADSTATE_TERMINATE);
		listmutex.Release();
		return;
	}

	if(it->second->m_battleground)
		it->second->TeleportPlayers();

	delete it->second;
	_instances.erase(it);
	listmutex.Release();
}

MapMgr * Map::GetFirstInstance()
{
	MapMgr *rv;
	listmutex.Acquire();
	InstanceMap::iterator it = _instances.begin();
	ASSERT(it != _instances.end());
	rv=it->second;
	if(rv && rv->IsDeletionPending())
	{
		listmutex.Release();
		return NULL;
	}
	listmutex.Release();
	return rv;
}

void Map::BuildXMLStats(char * m_file)
{
	char tmp[200];
	strcpy(tmp, "");
#define pushline strcat(m_file, tmp)

	InstanceMap::iterator itr = _instances.begin();
	MapMgr * mgr;
	for(; itr != _instances.end(); ++itr)
	{
		mgr = itr->second;
		snprintf(tmp, 200, "	<instance>\n");																	   pushline;
		snprintf(tmp, 200, "	  <map>%u</map>\n", (unsigned int)_mapId);														  pushline;
		snprintf(tmp, 200, "	  <maptype>%u</maptype>\n", (unsigned int)(_mapInfo ? _mapInfo->type : 0));						   pushline;
		snprintf(tmp, 200, "	  <players>%u</players>\n", (unsigned int)mgr->GetPlayerCount());								   pushline;
		snprintf(tmp, 200, "	  <maxplayers>%u</maxplayers>\n", (unsigned int)(_mapInfo ? _mapInfo->playerlimit : 0));			  pushline;
		snprintf(tmp, 200, "	  <state>%s</state>\n", mgr->HasPlayers() ? "ACTIVE" : "IDLE");					   pushline;
		snprintf(tmp, 200, "	  <creationtime>%s</creationtime>\n", asctime(localtime(&mgr->CreationTime)));		pushline;   
		snprintf(tmp, 200, "	  <expirytime>%s</expirytime>\n", mgr->ExpiryTime ? asctime(localtime(&mgr->ExpiryTime)) : "Never");			  pushline;
		snprintf(tmp, 200, "	</instance>\n");																	  pushline;
	}
#undef pushline
}

void Map::LoadSpawns(bool reload)
{
	//uint32 st=getMSTime();
	CreatureSpawnCount = 0;
	if(reload)//perform cleanup
	for(uint32 x=0;x<_sizeX;x++)
		for(uint32 y=0;y<_sizeY;y++)
		if(spawns[x][y])
		{	
			CellSpawns * sp=spawns[x][y];
			for(CreatureSpawnList::iterator i = sp->CreatureSpawns.begin();i!=sp->CreatureSpawns.end();i++)
				delete (*i);
			for(GOSpawnList::iterator it = sp->GOSpawns.begin();it!=sp->GOSpawns.end();it++)
				delete (*it);

			delete sp;
			spawns[x][y]=NULL;
		}

	QueryResult * result = WorldDatabase.Query("SELECT * FROM creature_spawns WHERE Map = %u",this->_mapId);
	if(result)
	{
		do{
			Field * fields = result->Fetch();
			CreatureSpawn * cspawn = new CreatureSpawn;
			cspawn->id = fields[0].GetUInt32();
			cspawn->form = FormationMgr::getSingleton().GetFormation(cspawn->id);
			cspawn->entry = fields[1].GetUInt32();
			cspawn->x = fields[3].GetFloat();
			cspawn->y = fields[4].GetFloat();
			cspawn->z = fields[5].GetFloat();
			cspawn->o = fields[6].GetFloat();
			uint32 cellx=float2int32(((_maxX-cspawn->x)/_cellSize));
			uint32 celly=float2int32(((_maxY-cspawn->y)/_cellSize));
			if(spawns[cellx]==NULL)
			{
				spawns[cellx]=new CellSpawns*[_sizeY];
				memset(spawns[cellx],0,sizeof(CellSpawns*)*_sizeY);
			}

			if(!spawns[cellx][celly])
				spawns[cellx][celly]=new CellSpawns;
			cspawn->movetype = fields[7].GetUInt32();
			cspawn->displayid = fields[8].GetUInt32();
			cspawn->factionid = fields[9].GetUInt32();
			cspawn->flags = fields[10].GetUInt32();
			cspawn->bytes = fields[11].GetUInt32();
			cspawn->bytes2 = fields[12].GetUInt32();
			cspawn->emote_state = fields[13].GetUInt32();
			cspawn->respawnNpcLink = fields[14].GetUInt32();
			spawns[cellx][celly]->CreatureSpawns.insert(cspawn);
			++CreatureSpawnCount;
		}while(result->NextRow());

		delete result;
	}

	result = WorldDatabase.Query("SELECT * FROM creature_staticspawns WHERE Map = %u",this->_mapId);
	if(result)
	{
		do{
			Field * fields = result->Fetch();
			CreatureSpawn * cspawn = new CreatureSpawn;
			cspawn->id = fields[0].GetUInt32();
			cspawn->form = FormationMgr::getSingleton().GetFormation(cspawn->id);
			cspawn->entry = fields[1].GetUInt32();
			cspawn->x = fields[3].GetFloat();
			cspawn->y = fields[4].GetFloat();
			cspawn->z = fields[5].GetFloat();
			cspawn->o = fields[6].GetFloat();
			cspawn->movetype = fields[7].GetUInt32();
			cspawn->displayid = fields[8].GetUInt32();
			cspawn->factionid = fields[9].GetUInt32();
			cspawn->flags = fields[10].GetUInt32();
			cspawn->bytes = fields[11].GetUInt32();
			cspawn->bytes2 = fields[12].GetUInt32();
			cspawn->emote_state = fields[13].GetUInt32();
			cspawn->respawnNpcLink = fields[14].GetUInt32();
			staticSpawns.CreatureSpawns.insert(cspawn);
			++CreatureSpawnCount;
		}while(result->NextRow());

		delete result;
	}

	GameObjectSpawnCount = 0;
	result = WorldDatabase.Query("SELECT * FROM gameobject_staticspawns WHERE Map = %u",this->_mapId);
	if(result)
	{
		do{
			Field * fields = result->Fetch();
			GOSpawn * gspawn = new GOSpawn;
			gspawn->entry = fields[1].GetUInt32();
			gspawn->id = fields[0].GetUInt32();
			gspawn->x=fields[3].GetFloat();
			gspawn->y=fields[4].GetFloat();
			gspawn->z=fields[5].GetFloat();
			gspawn->facing=fields[6].GetFloat();
			gspawn->o =fields[7].GetFloat();
			gspawn->o1=fields[8].GetFloat();
			gspawn->o2=fields[9].GetFloat();
			gspawn->o3=fields[10].GetFloat();
			gspawn->state=fields[11].GetUInt32();
			gspawn->flags=fields[12].GetUInt32();
			gspawn->faction=fields[13].GetUInt32();
			gspawn->scale = fields[14].GetFloat();
			gspawn->stateNpcLink = fields[15].GetUInt32();
			staticSpawns.GOSpawns.insert(gspawn);
			++GameObjectSpawnCount;
		}while(result->NextRow());

		delete result;
	}

	result = WorldDatabase.Query("SELECT * FROM gameobject_spawns WHERE Map = %u",this->_mapId);
	if(result)
	{
		do{
			Field * fields = result->Fetch();
			GOSpawn * gspawn = new GOSpawn;
			gspawn->entry = fields[1].GetUInt32();
			gspawn->id = fields[0].GetUInt32();
			gspawn->x=fields[3].GetFloat();
			gspawn->y=fields[4].GetFloat();
			gspawn->z=fields[5].GetFloat();
			gspawn->facing=fields[6].GetFloat();
			gspawn->o =fields[7].GetFloat();
			gspawn->o1=fields[8].GetFloat();
			gspawn->o2=fields[9].GetFloat();
			gspawn->o3=fields[10].GetFloat();
			gspawn->state=fields[11].GetUInt32();
			gspawn->flags=fields[12].GetUInt32();
			gspawn->faction=fields[13].GetUInt32();
			gspawn->scale = fields[14].GetFloat();
			gspawn->stateNpcLink = fields[15].GetUInt32();

			uint32 cellx=float2int32(((_maxX-gspawn->x)/_cellSize));
			uint32 celly=float2int32(((_maxY-gspawn->y)/_cellSize));
			if(spawns[cellx]==NULL)
			{
				spawns[cellx]=new CellSpawns*[_sizeY];
				memset(spawns[cellx],0,sizeof(CellSpawns*)*_sizeY);
			}

			if(!spawns[cellx][celly])
				spawns[cellx][celly]=new CellSpawns;

			spawns[cellx][celly]->GOSpawns.insert(gspawn);
			++GameObjectSpawnCount;
		}while(result->NextRow());

		delete result;
	}

	Log.Notice("Map", "%u creatures / %u gameobjects on map %u cached.", CreatureSpawnCount, GameObjectSpawnCount, _mapId);
}

MapMgr * Map::GetInstanceByGroup(Group *pGroup, Player * pCaller)
{
	MapMgr * pInstance = NULL;
	listmutex.Acquire();
	InstanceMap::iterator itr = _instances.begin();

	for(; itr != _instances.end(); ++itr)
	{
		pInstance = itr->second;
		//instances on hardreset cant be accessed again, ignore them to avoid problems
		//and ofc allow new instances to be created since save manager was already reseted.
		if(pInstance->IsDeletionPending()) { continue; }
        //skip any heroic mode instance if player is not set to heroic difficulty.
        if(pInstance->GetMapInfo() && pInstance->GetMapInfo()->type == INSTANCE_MULTIMODE && pInstance->iInstanceMode != pGroup->GetLeader()->iInstanceType) {  continue; }
        if(pInstance->GetMapInfo() && pInstance->GetMapInfo()->type == INSTANCE_RAID || pInstance->GetMapInfo() && pInstance->GetMapInfo()->type == INSTANCE_MULTIMODE && pInstance->iInstanceMode == MODE_HEROIC && pGroup->GetLeader()->iInstanceType == MODE_HEROIC)
		{
			//Detects if the player requesting the instance is already saved to one.
			if(sInstanceSavingManager.IsPlayerSavedToInstanceId(pInstance->GetMapId(), pInstance->GetInstanceID(), pCaller))
			{
				listmutex.Release();
				return pInstance;
			}
			else
			{
				if(sInstanceSavingManager.IsPlayerSavedToInstanceId(pInstance->GetMapId(), pInstance->GetInstanceID(), pGroup->GetLeader()))
				{
					listmutex.Release();
					return pInstance;
				}
				else
				{
					if(pInstance->GetGroupSignature() == pGroup->GetID()) // > 1
					{
						listmutex.Release();
						return pInstance;
					}
					else //instance was not made with a group, prolly leader made a group, attach group to this instance and return it
					{
						//check if the instance creator leader matchs the new group created.
						if(pGroup->GetLeader()->GetGUID() == pInstance->GetCreator())
						{
							pInstance->SetGroupSignature(pGroup->GetID());
							listmutex.Release();
							return pInstance;
						}
					}
				}
			}
		}
		else
		{
			if(pInstance->GetGroupSignature() == pGroup->GetID()) // > 1
			{
				listmutex.Release();
				return pInstance;
			}
			else //instance was not made with a group, prolly leader made a group, attach group to this instance and return it
			{
				//check if the instance creator leader matchs the new group created.
				if(pGroup->GetLeader()->GetGUID() == pInstance->GetCreator())
				{
					pInstance->SetGroupSignature(pGroup->GetID());
					listmutex.Release();
					return pInstance;
				}
			}
		}
	}

	listmutex.Release();
	return NULL;
}

MapMgr * Map::GetInstanceByCreator(Player *pCreator)
{
	MapMgr * pInstance = NULL;
	listmutex.Acquire();
	InstanceMap::iterator itr = _instances.begin();

	for(; itr != _instances.end(); ++itr)
	{
		pInstance = itr->second;
		if(pInstance->IsDeletionPending()) { listmutex.Release(); return NULL; }
        if(pInstance->GetMapInfo() && pInstance->GetMapInfo()->type == INSTANCE_MULTIMODE && pInstance->iInstanceMode != pCreator->iInstanceType) {  continue; }
        if(pInstance->GetMapInfo() && pInstance->GetMapInfo()->type == INSTANCE_RAID || pInstance->GetMapInfo() && pInstance->GetMapInfo()->type == INSTANCE_MULTIMODE && pInstance->iInstanceMode == MODE_HEROIC && pCreator->iInstanceType == MODE_HEROIC)
		{
			if(sInstanceSavingManager.IsPlayerSavedToInstanceId(pInstance->GetMapId(), pInstance->GetInstanceID(), pCreator))
			{
				listmutex.Release();
				return pInstance;
			}
		}
		else
		{
			//check if creator exists, to avoid problems we dont compare pointer address to detect if they are true,
			//better use guid cause player can logout etc and login back to the instance with a diference address.
			if(pInstance->GetCreator() && pCreator && !pInstance->GetGroupSignature())
			{
				if(pInstance->GetCreator() == pCreator->GetGUID())
				{
					listmutex.Release();
					return pInstance;
				}
			}
		}
	}

	listmutex.Release();
	return NULL;
}

MapMgr * Map::GetInstanceByGroupInstanceId(uint32 InstanceID, bool Lock)
{
	MapMgr * pInstance = NULL;

	if(Lock)
		listmutex.Acquire();

	InstanceMap::iterator itr = _instances.find(InstanceID);

	if(itr != _instances.end())
	{
		pInstance = itr->second;
		if(Lock)
			listmutex.Release();

		return pInstance;
	}
	else
	{
		if(Lock)
			listmutex.Release();

		return NULL;
	}
	return NULL;
}
