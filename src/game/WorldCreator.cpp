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
// WorldCreator.cpp
//

#include "StdAfx.h"

initialiseSingleton( WorldCreator );
initialiseSingleton( FormationMgr );
//initialiseSingleton( InstanceSavingManagement );

WorldCreator::WorldCreator(TaskList * tl)
{
	new FormationMgr;

	// Create all non-instance type maps.
	QueryResult *result;
	m_InstanceHigh = 0;

	result = CharacterDatabase.Query( "SELECT MAX(instanceid) FROM instances" );
	if( result )
	{
		m_InstanceHigh = result->Fetch()[0].GetUInt32()+1;

		delete result;
	}
#ifndef CLUSTERING
	StorageContainerIterator<MapInfo> * itr = WorldMapInfoStorage.MakeIterator();
	while(!itr->AtEnd())
	{
		MapInfo * pInfo = itr->Get();
		if(pInfo->type == INSTANCE_NULL)
		{
			// Create it (we're a non-instance).
			// Add a loading task for this map.
			tl->AddTask(new Task(new CallbackP1<WorldCreator, uint32>(WorldCreator::getSingletonPtr(), &WorldCreator::CreateMap, pInfo->mapid)));
		}
		if(!itr->Inc())
			break;
	}
	itr->Destruct();
#endif
}

WorldCreator::~WorldCreator()
{
	std::map<uint32, Map*>::iterator it;

	for(it = _maps.begin(); it != _maps.end(); it++)
	{
		delete it->second;
	}

	delete FormationMgr::getSingletonPtr();
}

Map* WorldCreator::_CreateMap(uint32 mapid)
{
	std::map<uint32, Map*>::iterator it;
	it = _maps.find(mapid);
	ASSERT(it == _maps.end());

	Map *newmap = new Map(mapid);
	ASSERT(newmap);

	_maps.insert( std::pair<uint32, Map*>(mapid, newmap) );

	return newmap;
}

Map* WorldCreator::GetMap(uint32 mapid)
{
	std::map<uint32, Map*>::iterator it;
	it = _maps.find(mapid);

	if (it != _maps.end())
		return it->second;
	else
		return _CreateMap(mapid);
}

MapMgr* WorldCreator::GetInstance(uint32 mapid, Object* obj)
{
	// check inactive instances.
	if(obj->GetInstanceID() > 2)
	{
		InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(obj->GetInstanceID());
		if(ia != 0)
		{
			//create that inactive instance.
			//extra, it now checks if the instance should expire.
			MapInfo *pMapInfo = WorldMapInfoStorage.LookupEntry(ia->MapId);
			if(pMapInfo)
			{
				if((uint32)time(NULL) > (ia->Creation) + (pMapInfo->cooldown ? pMapInfo->cooldown : 604800))
				{
					sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
					sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
				}
				else
				{
					MapMgr * dm = 0;
                    CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
					obj->SetMapId(ia->MapId);
					delete ia;
					return dm;
				}
			}
			else
			{
				MapMgr * dm = 0;
                CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
				obj->SetMapId(ia->MapId);
				delete ia;
				return dm;
			}
		}
	}

	Map* mp = GetMap(mapid);
	if(!mp) return NULL;
	return mp->GetInstance(obj);
}

MapMgr* WorldCreator::GetInstance(uint32 mapid, uint32 instanceId)
{
	// check inactive instances.
	if(instanceId > 2)
	{
		InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(instanceId);
		if(ia != 0)
		{
			// create that inactive instance.
			MapInfo *pMapInfo = WorldMapInfoStorage.LookupEntry(ia->MapId);
			if(pMapInfo)
			{
				if((uint32)time(NULL) > (ia->Creation) + (pMapInfo->cooldown ? pMapInfo->cooldown : 604800))
				{
					sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
					sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
				}
				else
				{
					MapMgr * dm = 0;
                    CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
					delete ia;
					return dm;
				}
			}
			else
			{
				MapMgr * dm = 0;
                CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
				delete ia;
				return dm;
			}
		}
	}

	Map* mp = GetMap(mapid);
	if(!mp) return NULL;
	return mp->GetInstance(instanceId);
}

MapMgr* WorldCreator::GetInstance(uint32 instanceId)
{
	// check inactive instances.
	if(instanceId > 2)
	{
		InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(instanceId);
		if(ia != 0)
		{
			// create that inactive instance.
			MapInfo *pMapInfo = WorldMapInfoStorage.LookupEntry(ia->MapId);
			if(pMapInfo)
			{
				if((uint32)time(NULL) > (ia->Creation) + (pMapInfo->cooldown ? pMapInfo->cooldown : 604800))
				{
					sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
					sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
				}
				else
				{
					MapMgr * dm = 0;
                    CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
					delete ia;
					return dm;
				}
			}
			else
			{
				MapMgr * dm = 0;
                CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
				delete ia;
				return dm;
			}
		}
	}

	std::map<uint32, Map*>::iterator it;
	MapMgr *instance;
	for(it = _maps.begin(); it != _maps.end(); it++)
	{
		instance = it->second->InstanceExists(instanceId);
		if(instance != NULL)
			return instance;

	}
	return NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance Manager
// instance creation and basic management
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool WorldCreator::CheckInstanceForObject(Object *obj, MapInfo *pMapinfo)
{
	WorldPacket data(4);
	Player *_player = NULL;

	if(obj->GetTypeId() == TYPEID_PLAYER)
	{
		_player = static_cast<Player*>(obj);
	}

	if(pMapinfo && _player)
	{
		switch(pMapinfo->type)
		{
			case INSTANCE_NONRAID:
			case INSTANCE_RAID:
            case INSTANCE_MULTIMODE:
				{
					//instance creation detection types
					//case 1, player is inside a group aka not soloing
					//onoes: leader can be inside a instance already and make a group or its a fresh group, noone inside
					 
					 if(_player->InGroup()) //group created, join leader instance.
					 {
						 Group *pGroup = _player->GetGroup();
						 if(pGroup)
						 {
							 //check if instance already exists(player solo created instance and made group after)
							 MapMgr *pInstance = sWorldCreator.GetInstanceByGroup(pGroup, _player, pMapinfo);
							 if(pInstance)
							 {
								 //INSTANCE_OK
								 if(pInstance->IsCombatInProgress())
								 {
									WorldPacket msg;
									msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
									msg << uint32(0) << "Encounter in progress." << uint8(0) << uint8(0);
									_player->GetSession()->SendPacket(&msg);
									return false;
								 }
								 if(pInstance->GetPlayerCount() >= pMapinfo->playerlimit)
								 {
									data.Initialize(SMSG_TRANSFER_ABORTED);
									data << uint32(INSTANCE_ABORT_FULL);
									_player->GetSession()->SendPacket(&data);
									return false;
								 }
								 _player->SetInstanceID(pInstance->GetInstanceID());
							 }
							 else
							 {
								//group leader didnt made any instance yet, create instance for this group.
								uint32 id = sWorldCreator.CreateInstance(pGroup, pGroup->GetLeader(), pMapinfo->mapid);
								// again, NULL might not be 0
								//if(id != NULL)
								if(id != 0)
								{
									//INSTANCE CREATED
									_player->SetInstanceID(id);
								}
								else
								{
									data.Initialize(SMSG_TRANSFER_ABORTED);
									data << uint32(INSTANCE_ABORT_ERROR);
									_player->GetSession()->SendPacket(&data);
									return false;
								}
							 }
						 }
						 else
						 {
							 data.Initialize(SMSG_TRANSFER_ABORTED);
							 data << uint32(INSTANCE_ABORT_ERROR);
							 _player->GetSession()->SendPacket(&data);
							 return false;
						 }
					 }
					 else
					 {
						 MapMgr *pInstance = sWorldCreator.GetInstanceByCreator(_player, pMapinfo);
						 if(pInstance)
						 {
							//INSTANCE_OK
							if(pInstance->IsCombatInProgress())
							{
								WorldPacket msg;
								msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
								msg << uint32(0) << "Encounter in progress." << uint8(0) << uint8(0);
								_player->GetSession()->SendPacket(&msg);
								return false;
							}
							if(pInstance->GetPlayerCount() >= pMapinfo->playerlimit)
							{
								data.Initialize(SMSG_TRANSFER_ABORTED);
								data << uint32(INSTANCE_ABORT_FULL);
								_player->GetSession()->SendPacket(&data);
								return false;
							}
							 _player->SetInstanceID(pInstance->GetInstanceID());
						 }
						 else
						 {
							 uint32 id2 = sWorldCreator.CreateInstance(NULL, _player, pMapinfo->mapid);
							 //if(id2 != NULL)
							 if(id2 != 0)
							 {
								_player->SetInstanceID(id2);
							 }
							 else
							 {
								data.Initialize(SMSG_TRANSFER_ABORTED);
								data << uint32(INSTANCE_ABORT_ERROR);
								_player->GetSession()->SendPacket(&data);
								return false;
							 }
						 }
					 }
				}break;
		}
	}
	return true;
}

uint32 WorldCreator::CreateInstance(Group *pGroup, Player *pPlayer, uint32 mapid, uint32 instanceid, uint32 creation, MapMgr ** destptr, uint32 difficulty)
{
	if(pGroup == NULL && pPlayer == NULL && instanceid == 0)
	{
		return INSTANCE_ABORT_ERROR;
	}

	Map* instance_map = GetMap(mapid);
	ASSERT(instance_map != NULL);

	MapMgr * pInstance = instance_map->CreateMapMgrInstance(instanceid);
	ASSERT(pInstance);
	if(creation)
	{
		pInstance->SetNewExpireTime(creation);
	}
	if(pGroup)
	{
		pInstance->SetGroupSignature(pGroup->GetID());
        pInstance->iInstanceMode = pGroup->GetLeader()->iInstanceType;
	}
	if(pPlayer)
	{
		pInstance->SetCreator(pPlayer);
        pInstance->iInstanceMode = pPlayer->iInstanceType;
	}
    if(difficulty)
    {
       pInstance->iInstanceMode = difficulty;
    }
    //Capt: We do not realy need to store this one if we are not doing something with it.
	sInstanceSavingManager.SaveInstance(pInstance); //first time instance saving holder
	if(destptr)
		*destptr = pInstance;

	return pInstance->GetInstanceID();
}

uint32 WorldCreator::CreateInstance(uint32 mapid, uint32 instanceid, MapMgr ** destptr)
{
	Map* instance_map = GetMap(mapid);
	ASSERT(instance_map != NULL);

	MapMgr * pInstance = instance_map->CreateMapMgrInstance(instanceid);
	ASSERT(pInstance);
	if(destptr)
		*destptr = pInstance;

	return pInstance->GetInstanceID();
}


MapMgr *WorldCreator::GetInstanceByGroup(Group *pGroup, Player *pPlayer, MapInfo *pMapInfo)
{
	MapMgr * mgr = GetMap(pMapInfo->mapid)->GetInstanceByGroup(pGroup, pPlayer);
	if(mgr)
	{
		return mgr;
	}
	else
	{
		
		bool result = sInstanceSavingManager.IsPlayerSavedToMap(pMapInfo->mapid, pPlayer);
		if(result)
		{
            Instance_Map_InstanceId_Holder *p = sInstanceSavingManager.GetRaidAndMMInstance(pMapInfo->mapid, pPlayer);
			if(p)
			{
				InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(p->GetInstanceID());
				if(ia != 0)
				{
					//create that inactive instance.
					//extra, it now checks if the instance should expire.
					MapInfo *pMapInfo = WorldMapInfoStorage.LookupEntry(ia->MapId);
					if(pMapInfo)
					{
						if((uint32)time(NULL) > (ia->Creation) + (pMapInfo->cooldown ? pMapInfo->cooldown : 604800))
						{
							sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
							sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
						}
						else
						{
							MapMgr * dm = 0;
							CreateInstance(NULL,NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
							delete ia;
							return dm;
						}
					}
					else
					{
						MapMgr * dm = 0;
                        CreateInstance(NULL,NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
						delete ia;
						return dm;
					}
				}

			}
			
		}
		
	}
	return NULL;
}

MapMgr *WorldCreator::GetInstanceByCreator(Player *pCreator, MapInfo *pMapInfo)
{
	MapMgr *mgr = GetMap(pMapInfo->mapid)->GetInstanceByCreator(pCreator);
	if(mgr)
	{
		return mgr;
	}
	else
	{
		bool result = sInstanceSavingManager.IsPlayerSavedToMap(pMapInfo->mapid, pCreator);
		if(result)
		{
            Instance_Map_InstanceId_Holder *p = sInstanceSavingManager.GetRaidAndMMInstance(pMapInfo->mapid, pCreator);
			if(p)
			{
				InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(p->GetInstanceID());
				if(ia != 0)
				{
					//create that inactive instance.
					//extra, it now checks if the instance should expire.
					MapInfo *pMapInfo = WorldMapInfoStorage.LookupEntry(ia->MapId);
					if(pMapInfo)
					{
						if((uint32)time(NULL) > (ia->Creation) + (pMapInfo->cooldown ? pMapInfo->cooldown : 604800))
						{
							sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
							sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
						}
						else
						{
							MapMgr * dm = 0;
                            CreateInstance(NULL,NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
							delete ia;
							return dm;
						}
					}
					else
					{
						MapMgr * dm = 0;
						CreateInstance(NULL,NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
						delete ia;
						return dm;
					}
				}

			}
			
		}
	}
	return NULL;
}

uint32 WorldCreator::GenerateInstanceID()
{
	m_InstanceHigh++;
	return m_InstanceHigh;
}

void WorldCreator::DeleteInstance(uint32 instanceid, uint32 mapid)
{
	_busy.Acquire();
	MapMgr * mapMgr = NULL;
	mapMgr = GetMap(mapid)->GetRawInstance(instanceid);

	if(mapMgr)
	{
		if(!mapMgr->HasPlayers())
		{
			GetMap(mapid)->DestroyMapMgrInstance(instanceid);
			sLog.outDebug("Instance %i on map %i deleted with success\n", instanceid, mapid);
		}
	}
	else
	{
		sLog.outDebug("Instance %i failed to be deleted, reason: Doesnt Exist\n", instanceid);
	}
	_busy.Release();
}

void WorldCreator::InstanceSoftReset(MapMgr *mMapMgr)
{
	uint32 mapid = mMapMgr->GetMapId();
	uint32 instanceid = mMapMgr->GetInstanceID();
	//delete public instance WoWInstance to avoid "rats".
	DeleteInstance(mMapMgr->GetInstanceID(), mapid);
	//delete instance save data
	sInstanceSavingManager.RemoveSavedInstance(mapid, instanceid, false);
}

void WorldCreator::InstanceHardReset(MapMgr *mMapMgr)
{
	uint32 mapid = mMapMgr->GetMapId();
	uint32 instanceid = mMapMgr->GetInstanceID();
	//delete public instance WoWInstance to avoid "rats".
	DeleteInstance(mMapMgr->GetInstanceID(), mapid);
	//delete instance save data
	sInstanceSavingManager.RemoveSavedInstance(mapid, instanceid, true);
}

// Only usable by Instance save manager when removing players from the manager.
MapMgr * WorldCreator::ISMGetInstanceBeforeRemoval(uint32 InstanceID, uint32 mapid, bool Lock)
{
	if(InstanceID > 2)
	{
		InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(InstanceID);
		if(ia != 0)
		{
			// create that inactive instance.
			MapInfo *pMapInfo = WorldMapInfoStorage.LookupEntry(ia->MapId);
			if(pMapInfo)
			{
				if((uint32)time(NULL) > (ia->Creation) + (pMapInfo->cooldown ? pMapInfo->cooldown : 604800))
				{
					sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
				}
				else
				{
					MapMgr * dm = 0;
                    CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
					delete ia;
					return dm;
				}
			}
			else
			{
				MapMgr * dm = 0;
                CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm, ia->difficulty);
				delete ia;
				return dm;
			}
		}
	}

	return GetMap(mapid)->GetInstanceByGroupInstanceId(InstanceID, Lock);
}

void WorldCreator::BuildXMLStats(char * m_file)
{
	_busy.Acquire();
	map<uint32, Map*>::iterator itr = _maps.begin();
	for(; itr != _maps.end(); ++itr)
	{
		itr->second->BuildXMLStats(m_file);
	}
	_busy.Release();
}

FormationMgr::FormationMgr()
{
	QueryResult * res = WorldDatabase.Query("SELECT * FROM creature_formations");
	if(res)
	{
		Formation *f ;
		do 
		{
			f = new Formation;
			f->fol = res->Fetch()[1].GetUInt32();
			f->ang = res->Fetch()[2].GetFloat();
			f->dist = res->Fetch()[3].GetFloat();
			m_formations[res->Fetch()[0].GetUInt32()] = f;
		} while(res->NextRow());
		delete res;
	}
}

FormationMgr::~FormationMgr()
{
	FormationMap::iterator itr;
	for(itr = m_formations.begin(); itr != m_formations.end(); ++itr)
		delete itr->second;
}
