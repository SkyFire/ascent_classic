/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
// MapCell.cpp
//
#include "StdAfx.h"

MapCell::~MapCell()
{
	RemoveObjects();
}

void MapCell::Init(uint32 x, uint32 y, uint32 mapid, MapMgr *mapmgr)
{
	_mapmgr = mapmgr;
	_active = false;
	_loaded = false;
	_playerCount = 0;
	_x=x;
	_y=y;
	_unloadpending=false;
}

void MapCell::AddObject(Object *obj)
{
	if(obj->IsPlayer())
		++_playerCount;

	_objects.insert(obj);
}

void MapCell::RemoveObject(Object *obj)
{
	if(obj->IsPlayer())
		--_playerCount;

	_objects.erase(obj);
}

void MapCell::SetActivity(bool state)
{	
	if(!_active && state)
	{
		// Move all objects to active set.
		for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); ++itr)
		{
			if(!(*itr)->Active && (*itr)->CanActivate())
				(*itr)->Activate(_mapmgr);
		}

		if(_unloadpending)
			CancelPendingUnload();

#ifdef COLLISION
		CollideInterface.ActivateTile(_mapmgr->GetMapId(), _x/8, _y/8);
#endif

	} else if(_active && !state)
	{
		// Move all objects from active set.
		for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); ++itr)
		{
			if((*itr)->Active)
				(*itr)->Deactivate(_mapmgr);
		}

		if(sWorld.map_unload_time && !_unloadpending)
			QueueUnloadPending();

#ifdef COLLISION
		CollideInterface.DeactivateTile(_mapmgr->GetMapId(), _x/8, _y/8);
#endif
	}

	_active = state; 

}
void MapCell::RemoveObjects()
{
	ObjectSet::iterator itr;
	uint32 count = 0;
	//uint32 ltime = getMSTime();

	/* delete objects in pending respawn state */
	for(itr = _respawnObjects.begin(); itr != _respawnObjects.end(); ++itr)
	{
		switch((*itr)->GetGUIDHigh())
		{
		case HIGHGUID_UNIT: {
			_mapmgr->_reusable_guids_creature.push_back( (*itr)->GetGUIDLow() );
			static_cast< Creature* >( *itr )->m_respawnCell=NULL;
			delete static_cast< Creature* >( *itr );
			}break;

		case HIGHGUID_GAMEOBJECT: {
			_mapmgr->_reusable_guids_gameobject.push_back( (*itr)->GetGUIDLow() );
			static_cast< GameObject* >( *itr )->m_respawnCell=NULL;
			delete static_cast< GameObject* >( *itr );
			}break;
		}
	}
	_respawnObjects.clear();

	//This time it's simpler! We just remove everything :)
	for(itr = _objects.begin(); itr != _objects.end(); )
	{
		count++;

		Object *obj = (*itr);

		itr++;

		if(!obj || (_unloadpending && obj->GetGUIDHigh()==HIGHGUID_TRANSPORTER) || !obj->m_loadedFromDB)
			continue;

		if(_unloadpending && obj->GetTypeId()==TYPEID_CORPSE && obj->GetUInt32Value(CORPSE_FIELD_OWNER) != 0)
			continue;

		if( obj->Active )
			obj->Deactivate( _mapmgr );

		if( obj->IsInWorld() )
			obj->RemoveFromWorld( true );

		if( obj->GetTypeId() == TYPEID_UNIT )
		{
			if(obj->IsPet())
				delete static_cast< Pet* >( obj );
			else
				delete static_cast< Creature* >( obj );
		}
		else if(obj->GetTypeId() == TYPEID_GAMEOBJECT)
		{
			if( obj->GetGUIDHigh() == HIGHGUID_TRANSPORTER )
				delete static_cast< Transporter* >( obj );
			else
				delete static_cast< GameObject* >( obj );
		}
		else if( obj->GetTypeId() == TYPEID_DYNAMICOBJECT )
			delete static_cast< DynamicObject* >( obj );
		else if( obj->GetTypeId() == TYPEID_CORPSE )
		{
			delete static_cast< Corpse* >( obj );
		}
	}

	_playerCount = 0;
	_loaded = false;
}


void MapCell::LoadObjects(CellSpawns * sp)
{
	_loaded = true;
	Instance * pInstance = _mapmgr->pInstance;

	if(sp->CreatureSpawns.size())//got creatures
	{
		for(CreatureSpawnList::iterator i=sp->CreatureSpawns.begin();i!=sp->CreatureSpawns.end();i++)
		{
			if(pInstance)
			{
				if(pInstance->m_killedNpcs.find((*i)->id) != pInstance->m_killedNpcs.end())
					continue;

/*				if((*i)->respawnNpcLink && pInstance->m_killedNpcs.find((*i)->respawnNpcLink) != pInstance->m_killedNpcs.end())
					continue;*/
			}

			Creature * c=_mapmgr->CreateCreature();

			c->SetMapId(_mapmgr->GetMapId());
			c->SetInstanceID(_mapmgr->GetInstanceID());
			c->m_loadedFromDB = true;

            if(c->Load(*i, _mapmgr->iInstanceMode, _mapmgr->GetMapInfo()))
			{
				if(!c->CanAddToWorld())
					delete c;

				c->PushToWorld(_mapmgr);
			}
			else
				delete c;//missing proto or smth of that kind
		}
	}

	if(sp->GOSpawns.size())//got GOs
	{
		for(GOSpawnList::iterator i=sp->GOSpawns.begin();i!=sp->GOSpawns.end();i++)
		{
			GameObject * go=_mapmgr->CreateGameObject();
			if(go->Load(*i))
			{
				uint32 state = go->GetUInt32Value(GAMEOBJECT_STATE);

				// FIXME - burlex
				/*
				if(pInstance && pInstance->FindObject((*i)->stateNpcLink))
				{
					go->SetUInt32Value(GAMEOBJECT_STATE, (state ? 0 : 1));
				}*/			   

				go->m_loadedFromDB = true;
				go->PushToWorld(_mapmgr);
			}
			else
				delete go;//missing proto or smth of that kind
		}
	}
}


void MapCell::QueueUnloadPending()
{
	if(_unloadpending)
		return;

	_unloadpending = true;
	//Log.Debug("MapCell", "Queueing pending unload of cell %u %u", _x, _y);
	sEventMgr.AddEvent(_mapmgr, &MapMgr::UnloadCell,(uint32)_x,(uint32)_y,MAKE_CELL_EVENT(_x,_y),sWorld.map_unload_time * 1000,1,0);
}

void MapCell::CancelPendingUnload()
{
	//Log.Debug("MapCell", "Cancelling pending unload of cell %u %u", _x, _y);
	if(!_unloadpending)
		return;

	sEventMgr.RemoveEvents(_mapmgr,MAKE_CELL_EVENT(_x,_y));
}

void MapCell::Unload()
{
	//Log.Debug("MapCell", "Unloading cell %u %u", _x, _y);
	ASSERT(_unloadpending);
	if(_active)
		return;

	RemoveObjects();
	_unloadpending=false;
}
