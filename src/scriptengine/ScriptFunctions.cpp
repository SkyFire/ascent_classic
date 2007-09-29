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
 * GameMonkey Script License
 * Copyright (c) 2003 Auran Development Ltd.
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 */

#include "StdAfx.h"

int ScriptEngine_RegisterAreaTriggerEvent(gmThread * a_thread)
{
	// Param 1: Areatrigger ID
	// Param 2: Function to execute
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_FUNCTION_PARAM(func, 1);
	
	// Get 'this' pointer pointing to scriptengine
	ScriptEngine * eng = GetThisPointer<ScriptEngine>(a_thread);
	eng->AddAreaTriggerEvent(entry, func);
	
	return GM_OK;
}

int ScriptEngine_RegisterGameObjectEvent(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(3);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_INT_PARAM(event, 1);
	GM_CHECK_FUNCTION_PARAM(func, 2);

	GetThisPointer<ScriptEngine>(a_thread)->AddGameObjectEvent(entry, event, func);
	return GM_OK;
}

int ScriptEngine_RegisterSpellEvent(gmThread * a_thread)
{
	return GM_OK;
}

int ScriptEngine_RegisterUnitEvent(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(3);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_INT_PARAM(event, 1);
	GM_CHECK_FUNCTION_PARAM(func, 2);

	GetThisPointer<ScriptEngine>(a_thread)->AddCreatureEvent(entry, event, func);
	return GM_OK;
}

int ScriptEngine_RegisterPlayerEvent(gmThread * a_thread)
{
	return GM_OK;
}

int ScriptEngine_RegisterQuestEvent(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(3);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_INT_PARAM(event, 1);
	GM_CHECK_FUNCTION_PARAM(func, 2);

	GetThisPointer<ScriptEngine>(a_thread)->AddQuestEvent(entry, event, func);
	return GM_OK;
}

// Player Functions
int Player_Teleport(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(4);
	GM_CHECK_INT_PARAM(mapId, 0);
	GM_CHECK_FLOAT_PARAM(posX, 1);
	GM_CHECK_FLOAT_PARAM(posY, 2);
	GM_CHECK_FLOAT_PARAM(posZ, 3);

	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->SafeTeleport(mapId, 0, posX, posY, posZ, 0);
	return GM_OK;
}

int Player_GetLevel(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt((int)GetThisPointer<Player>(a_thread)->getLevel());
	return GM_OK;
}

int Player_GetClass(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt((int)GetThisPointer<Player>(a_thread)->getClass());
	return GM_OK;
}

int Player_GetRace(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt((int)GetThisPointer<Player>(a_thread)->getRace());
	return GM_OK;
}

int Player_SendAreaTriggerMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(message, 0);

	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->SendAreaTriggerMessage(message);
	return GM_OK;
}

int Player_BroadcastMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(message, 0);

	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->BroadcastMessage(message);
	return GM_OK;
}

int Player_GetReputationRank(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(faction, 0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt(GetThisPointer<Player>(a_thread)->GetStandingRank(faction));
    return GM_OK;
}

int Player_GetReputationValue(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(faction, 0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt(GetThisPointer<Player>(a_thread)->GetStanding(faction));
	return GM_OK;
}

int Player_HasFinishedQuest(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(questid, 0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	Player *p = GetThisPointer<Player>(a_thread);
	if(p->HasFinishedQuest(questid))
		a_thread->PushInt(1);
	else
		a_thread->PushInt(0);

	return GM_OK;
}

int Player_IsGroupLeader(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	Player * p = GetThisPointer<Player>(a_thread);
	if(p->InGroup() && p->IsGroupLeader())
		a_thread->PushInt(1);
	else
		a_thread->PushInt(0);

	return GM_OK;
}

int Player_JoinInstance(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(5);
	GM_CHECK_INT_PARAM(mapId, 0);
	GM_CHECK_FLOAT_PARAM(posX, 1);
	GM_CHECK_FLOAT_PARAM(posY, 2);
	GM_CHECK_FLOAT_PARAM(posZ, 3);
	GM_CHECK_FLOAT_PARAM(posO, 4);

	MapInfo *pMapinfo = NULL;
	pMapinfo = WorldMapInfoStorage.LookupEntry(mapId);
	bool result = sWorldCreator.CheckInstanceForObject(static_cast<Object*>(GetThisPointer<Player>(a_thread)), pMapinfo);
	if(result)
	{
		GetThisPointer<Player>(a_thread)->SaveEntryPoint(mapId);
		GetThisPointer<Player>(a_thread)->SafeTeleport(mapId, 0, LocationVector(posX, posY, posZ, posO));
	}

	return GM_OK;
}

/* Areatrigger events */
int AreaTrigger_GetEntry(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	a_thread->PushInt(GetThisPointer<AreaTrigger>(a_thread)->AreaTriggerID);
	return GM_OK;
}


/* Unit events */
int Unit_Despawn(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(delay, 0);
	GM_CHECK_INT_PARAM(respawntime, 1);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	if(pUnit->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	Creature * pCreature = ((Creature*)pUnit);
	pCreature->Despawn(delay, respawntime);

	return GM_OK;
}

int Unit_Emote(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(emote, 0);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->Emote((EmoteType)emote);
	return GM_OK;
}

int Unit_SendChatMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(msg, 0);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, msg);
	return GM_OK;
}

int Unit_CastSpell(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spellid, 0);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->CastSpell(pUnit, sSpellStore.LookupEntry(spellid), true);
	return GM_OK;
}
int Unit_SetStandState(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(stateid, 0);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->SetStandState(stateid);

	return GM_OK;
}
int Player_AddItem(gmThread * a_thread)
{
	bool push = false;
	if(a_thread->GetNumParams() == 2)
	{
		// Assume to send an item push
		push = true;
	}
	else if(a_thread->GetNumParams() == 3)
	{
		// Choice for push
		GM_CHECK_INT_PARAM(push_, 2);
		push = (push_ > 0) ? true : false;
	}
	else
        GM_CHECK_NUM_PARAMS(2);

	GM_CHECK_INT_PARAM(itemid, 0);
	GM_CHECK_INT_PARAM(count, 1);

	Player * pPlayer = GetThisPointer<Player>(a_thread);
	if(pPlayer->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	ItemPrototype * proto = ItemPrototypeStorage.LookupEntry(itemid);
	if(!proto)
		return GM_EXCEPTION;

	if(pPlayer->GetItemInterface()->CanReceiveItem(proto, count) == 0)
	{
		int acount;
		int bcount;
		if(proto->MaxCount && proto->MaxCount <= (uint32)count)
		{
			acount = 1;
			bcount = count;
		}
		else
		{
			acount = count;
			bcount = 0;
		}

		for(int i = 0; i < acount; ++i)
		{
			Item * pItem = objmgr.CreateItem(itemid, pPlayer);
			if(!pPlayer->GetItemInterface()->AddItemToFreeSlot(pItem))
			{
				delete pItem;
				break;
			}
		}

		if(push)
		{
			WorldPacket data(45);
			pPlayer->GetSession()->BuildItemPushResult(&data, pPlayer->GetGUID(), ITEM_PUSH_TYPE_RECEIVE, count, itemid, 0);
			pPlayer->GetSession()->SendPacket(&data);
		}
	}

	return GM_OK;
}

int Player_RemoveItem(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(itemid, 0);
	GM_CHECK_INT_PARAM(count, 1);

	Player * pPlayer = GetThisPointer<Player>(a_thread);
	if(pPlayer->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	pPlayer->GetItemInterface()->RemoveItemAmt(itemid, count);
	return GM_OK;
}

int Player_LearnSpell(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spellid, 0);

	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->addSpell(spellid);
	return GM_OK;
}

int Player_RemoveSpell(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spellid, 0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->removeSpell(spellid, false, false, 0);
	return GM_OK;
}

int Unit_CastSpellOnTarget(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(spellid, 0);
	
	Unit * pTarget = (Unit*)a_thread->ParamUser_NoCheckTypeOrParam(1);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	
	if(!pTarget)
		return GM_OK;

	SpellEntry * pSpellEntry = sSpellStore.LookupEntry(spellid);

	pThis->CastSpell(pTarget, pSpellEntry, true);
	return GM_OK;
}

int Unit_TimedEmote(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(emoteid, 0);
	GM_CHECK_INT_PARAM(timer, 1);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(timer)
		pThis->EventAddEmote((EmoteType)emoteid, timer);
	else
		pThis->SetUInt32Value(UNIT_NPC_EMOTESTATE, emoteid);

	return GM_OK;
}

int Unit_RegisterTimer(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(3);
	GM_CHECK_INT_PARAM(delay, 0);
	GM_CHECK_FUNCTION_PARAM(func, 1);
	GM_CHECK_INT_PARAM(repeats, 2);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	sEventMgr.AddEvent(((Creature*)pThis), &Creature::TriggerScriptEvent, ((void*)func), EVENT_SCRIPT_UPDATE_EVENT, delay, repeats,0);
	return GM_OK;
}

int Unit_DeregisterTimer(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	sEventMgr.RemoveEvents(((Creature*)pThis), EVENT_SCRIPT_UPDATE_EVENT);
	return GM_OK;
}

int Unit_SpawnMonster(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(4);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_FLOAT_PARAM(posX, 1);
	GM_CHECK_FLOAT_PARAM(posY, 2);
	GM_CHECK_FLOAT_PARAM(posZ, 3);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	CreatureProto * p = CreatureProtoStorage.LookupEntry(entry);
	if(!p)
		return GM_EXCEPTION;

	Creature * pCreature = pThis->GetMapMgr()->CreateCreature();
	pCreature->spawnid = 0;
	pCreature->m_spawn = 0;
	pCreature->Load(p, posX, posY, posZ);
	pCreature->SetMapId(pThis->GetMapId());
	pCreature->SetInstanceID(pThis->GetInstanceID());
	pCreature->PushToWorld(pThis->GetMapMgr());

	return GM_OK;
}

/* Base function */
int Unit_RemoveAura(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spid, 0);
	GetThisPointer<Unit>(a_thread)->RemoveAura(spid);
	return GM_OK;
}

int Player_HasQuest(gmThread * a_thread)
{
        GM_CHECK_NUM_PARAMS(1);
        GM_CHECK_INT_PARAM(questid, 0);
        if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
                return GM_EXCEPTION;

        Player *p = GetThisPointer<Player>(a_thread);
        if(p->GetQuestLogForEntry(questid))
                a_thread->PushInt(1);
        else
                a_thread->PushInt(0);

        return GM_OK;
}

int Player_HasItem(gmThread * a_thread)
{
        GM_CHECK_NUM_PARAMS(1);
        GM_CHECK_INT_PARAM(itemid, 0);

        Player * pPlayer = GetThisPointer<Player>(a_thread);
        if(pPlayer->GetTypeId() != TYPEID_PLAYER)
                return GM_EXCEPTION;

        if(pPlayer->GetItemInterface()->GetItemCount(itemid, 0))
                a_thread->PushInt(1);
        else
                a_thread->PushInt(0);

        return GM_OK;
}

int GameObject_Despawn(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(timer, 0);

	GameObject * pThis = GetThisPointer<GameObject>(a_thread);
	pThis->Despawn(timer);
	return GM_OK;
}

int GM_RAND(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(min, 0);
	GM_CHECK_INT_PARAM(max, 1);
	a_thread->PushInt(sRand.randInt(max-min)+min);
	return GM_OK;
}

int Quest_GetID(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Quest * pThis = GetThisPointer<Quest>(a_thread);
	a_thread->PushInt(pThis->id);
	return GM_OK;
}

int GM_RegisterEvent(gmThread * a_thread)
{
	Object * pThis = GetThisPointer<Object>(a_thread);
	if(a_thread->GetNumParams() < 2)
		return GM_EXCEPTION;

	if(a_thread->ParamType(0) != GM_INT || a_thread->ParamType(1) != GM_FUNCTION)
		return GM_EXCEPTION;

	uint32 argc = a_thread->GetNumParams() - 2;
	uint32 * argv = argc ? new uint32[argc] : 0;
	uint32 * argt = argc ? new uint32[argc] : 0;
	
	/* see if any of the arguments are our user types -> if so we need to convert the pointers. */
	for(uint32 i = 0; i < argc; ++i)
	{
		void * pointer = 0;
		uint32 t = 0;
		for(list<gmType>::iterator itr = ScriptSystem->m_allowedTypes.begin(); itr != ScriptSystem->m_allowedTypes.end(); ++itr)
		{
			pointer = a_thread->Param(2+i).GetUserSafe((*itr));
			if(pointer)
			{
				t = *itr;
				break;
			}
		}

		if(pointer)
		{
			argt[i] = uint32( (t << 16) | uint32(GM_USER) );
			argv[i] = (uint32)pointer;
		}
		else
		{
			argt[i] = 0 | uint32( ( uint32(a_thread->ParamType(2+i)) << 16) );
			argv[i] = *(uint32*)&a_thread->Param(2+i).m_value;
		}
	}

	/* set up the event :) */
	sEventMgr.AddEvent(pThis, &Object::GMScriptEvent, (void*)a_thread->ParamRef(1), argc, argv, argt, EVENT_GMSCRIPT_EVENT, a_thread->ParamInt(0), 1, 0);
	return GM_OK;
}

void Object::GMScriptEvent(void * function, uint32 argc, uint32 * argv, uint32 * argt)
{
	ScriptSystem->GetLock().Acquire();

	gmFunctionObject * func = (gmFunctionObject*)function;
	ASSERT(func->GetType() == GM_FUNCTION);
	
	/* strange if we had a function without any arguments, that means no 'this' pointer :P *shrugs* */
	ScriptSystem->m_userObjectCounter = argc;
	/*if(argc)
	{
		ASSERT((uint16)argt != 0);			// is a user pointer
		ScriptSystem->SetVariable(0, (void*)argv[0], (gmType)(argt[0] >> 16));
	}*/

	gmCall call;
	if(!call.BeginFunction(ScriptSystem->GetMachine(), func, /*argc ? ScriptSystem->m_variables[0] : */gmVariable::s_null, false))
	{
		printf("Could not execute delayed function.");
		ScriptSystem->DumpErrors();
	}
	else
	{
        /* the rest of the arguments */
		if(argc)
		{
			for(uint32 i = 0; i < argc; ++i)
			{
				if((uint16)argt[i] != 0)		// user pointer
				{
					ScriptSystem->SetVariable(i, (void*)argv[i], (gmType)(argt[i] >> 16));
					call.AddParam(ScriptSystem->m_variables[i]);
				}
				else		// normal param
				{
					ScriptSystem->m_variables[i].m_type = UINT32_HIPART(argt[i]);
					*(uint32*)&ScriptSystem->m_variables[i].m_value = argv[i];

					// push it!
					call.AddParam(ScriptSystem->m_variables[i]);
				}
			}
		}

		/* fly away! */
		call.End();
		ScriptSystem->DumpErrors();
	}

	ScriptSystem->GetLock().Release();
	delete [] argt;
	delete [] argv;
}


// Escort Quest System
int Unit_CreateCustomWaypointMap(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);

	Creature * pCreature = GetThisPointer<Creature>(a_thread);
	if(pCreature->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	if(pCreature->m_custom_waypoint_map)
	{
		for(WayPointMap::iterator itr = pCreature->m_custom_waypoint_map->begin(); itr != pCreature->m_custom_waypoint_map->end(); ++itr)
			delete (*itr);
		delete pCreature->m_custom_waypoint_map;
	}

	pCreature->m_custom_waypoint_map = new WayPointMap;
	pCreature->GetAIInterface()->SetWaypointMap(pCreature->m_custom_waypoint_map);
	return GM_OK;
}

int Unit_CreateWaypoint(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(7);
	GM_CHECK_FLOAT_PARAM(x, 0);
	GM_CHECK_FLOAT_PARAM(y, 1);
	GM_CHECK_FLOAT_PARAM(z, 2);
	GM_CHECK_FLOAT_PARAM(o, 3);
	GM_CHECK_INT_PARAM(waittime, 4);
	GM_CHECK_INT_PARAM(flags, 5);
	GM_CHECK_INT_PARAM(modelid, 6);

	Creature * pCreature = GetThisPointer<Creature>(a_thread);
	if(pCreature->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	if(!pCreature->m_custom_waypoint_map)
	{
		pCreature->m_custom_waypoint_map = new WayPointMap;
		pCreature->GetAIInterface()->SetWaypointMap(pCreature->m_custom_waypoint_map);
	}

	if(!modelid)
		modelid = pCreature->GetUInt32Value(UNIT_FIELD_DISPLAYID);

	WayPoint * wp = new WayPoint;
	wp->id = pCreature->m_custom_waypoint_map->size() + 1;
	wp->x = x;
	wp->y = y;
	wp->z = z;
	wp->o = o;
	wp->flags = flags;
	wp->backwardskinid = modelid;
	wp->forwardskinid = modelid;
	wp->backwardemoteid = wp->forwardemoteid = 0;
	wp->backwardemoteoneshot = wp->forwardemoteoneshot = false;
	wp->waittime = waittime;

	pCreature->m_custom_waypoint_map->push_back(wp);
	return GM_OK;
}

int Unit_SpawnWithoutWorld(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(4);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_FLOAT_PARAM(posX, 1);
	GM_CHECK_FLOAT_PARAM(posY, 2);
	GM_CHECK_FLOAT_PARAM(posZ, 3);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	CreatureProto * p = CreatureProtoStorage.LookupEntry(entry);
	if(!p)
		return GM_EXCEPTION;

	Creature * pCreature = pThis->GetMapMgr()->CreateCreature();
	pCreature->spawnid = 0;
	pCreature->m_spawn = 0;
	pCreature->Load(p, posX, posY, posZ);
	pCreature->SetMapId(pThis->GetMapId());
	pCreature->SetInstanceID(pThis->GetInstanceID());
	
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)pCreature;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_unitType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;
	return GM_OK;
}

int Unit_GetHealthPct(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
    a_thread->PushFloat((float) pThis->GetUInt32Value(UNIT_FIELD_HEALTH) * 100.0f / pThis->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
	return GM_OK;
}

int Unit_AddToWorld(gmThread * a_thread)
{
	/* we need to provide a mapmgr to "leech" from. */
	GM_CHECK_NUM_PARAMS(1);
	/*if(a_thread->ParamType(0) != ScriptSystem->m_unitType)
	{
		a_thread->GetMachine()->GetLog().LogEntry("expecting param %d as user type Unit", 0);
		return GM_EXCEPTION;
	}
	
	Object * leech = (Object*)a_thread->ParamUser_NoCheckTypeOrParam(0);*/
	GM_CHECK_USER_PARAM(Object*, ScriptSystem->m_unitType, leech, 0);
	
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(!pThis->IsInWorld())
	{
		pThis->PushToWorld(leech->GetMapMgr());
	}

	return GM_OK;
}

int Unit_MoveToWaypoint(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(wp, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		pThis->MoveToWaypoint(wp);

	return GM_OK;
}

int Unit_Delete(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		((Creature*)pThis)->SafeDelete();

	return GM_OK;
}

int Unit_SetCombatCapable(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(capable, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		pThis->GetAIInterface()->disable_melee = (capable > 0) ? true : false;

	return GM_OK;
}

int Unit_HaltMovement(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(timer, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		pThis->GetAIInterface()->StopMovement(timer);

	return GM_OK;
}

int Player_MarkQuestObjectiveAsComplete(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(questId, 0);
	GM_CHECK_INT_PARAM(objective, 1);

	Player * pThis = GetThisPointer<Player>(a_thread);
	QuestLogEntry * qle = pThis->GetQuestLogForEntry(questId);
	if(!qle)
		return GM_OK;

	Quest * q = qle->GetQuest();
	qle->SetMobCount(objective, q->required_mobcount[objective]);
	qle->SendUpdateAddKill(objective);
	if(qle->CanBeFinished())
		qle->SendQuestComplete();

	return GM_OK;
}

int Unit_SetMovementType(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(mtype, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		pThis->GetAIInterface()->setMoveType(mtype);

	return GM_OK;
}

int Unit_SetEscortTarget(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_USER_PARAM(Player*, ScriptSystem->m_playerType, leech, 0);

	Creature * pThis = GetThisPointer<Creature>(a_thread);
	pThis->m_escorter = leech;
	return GM_OK;
}

int Unit_HasEscortTarget(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Creature * pThis = GetThisPointer<Creature>(a_thread);
	if(pThis->m_escorter != 0)
		a_thread->PushInt(1);
	else
		a_thread->PushInt(0);
	return GM_OK;
}

int Unit_GetEscortTarget(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Creature * pThis = GetThisPointer<Creature>(a_thread);
	if(pThis->m_escorter == 0)
		return GM_EXCEPTION;

	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)pThis->m_escorter;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_playerType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;
	return GM_OK;
}

int Player_SendNotification(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(msg, 0);
	Player * pThis = GetThisPointer<Player>(a_thread);
	pThis->GetSession()->SendNotification(msg);
	return GM_OK;
}

int Player_SendSystemMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(msg, 0);
	Player * pThis = GetThisPointer<Player>(a_thread);
	sChatHandler.SystemMessage(pThis->GetSession(), msg);
	return GM_OK;
}

int Unit_SetNPCFlags(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(mtype, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		pThis->SetUInt32Value(UNIT_NPC_FLAGS, mtype);

	return GM_OK;
}

int Unit_DestroyCustomWaypointMap(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		((Creature*)pThis)->DestroyCustomWaypointMap();
	return GM_OK;
}


int Unit_ClearEscortTarget(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		((Creature*)pThis)->m_escorter = 0;
	return GM_OK;
}

int GameObject_PlayCustomAnim(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(anim, 0);
	GameObject * pThis = GetThisPointer<GameObject>(a_thread);
	WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM, 12);
	data << pThis->GetGUID() << uint32(anim);
	pThis->SendMessageToSet(&data, true, false);
	return GM_OK;
}

int GameObject_SetActive(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(state, 0);
	GameObject * pThis = GetThisPointer<GameObject>(a_thread);
	pThis->SetUInt32Value(GAMEOBJECT_DYN_FLAGS, state);
	return GM_OK;
}

int Player_Knockback(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(4);
	GM_CHECK_FLOAT_PARAM(dx, 0);
	GM_CHECK_FLOAT_PARAM(dy, 1);
	GM_CHECK_FLOAT_PARAM(affect1, 2);
	GM_CHECK_FLOAT_PARAM(affect2, 3);

	Player * pThis = GetThisPointer<Player>(a_thread);
	WorldPacket data(SMSG_MOVE_KNOCK_BACK, 30);
	data << pThis->GetNewGUID();
	data << getMSTime();
	data << dx << dy << affect1 << affect2;
	pThis->SendMessageToSet(&data, true);
	return GM_OK;
}

int Unit_GetGuid(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	a_thread->PushInt(pThis->GetGUIDLow());
	return GM_OK;
}

int Unit_GetPlayer(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(guid, 0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	Player * plr = pThis->GetMapMgr() ? pThis->GetMapMgr()->GetPlayer(guid) : 0;
	if(!plr)
		return GM_EXCEPTION;

	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)plr;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_playerType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;

	return GM_OK;
}

int Unit_GetUnit(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(guid, 0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	Unit * plr = pThis->GetMapMgr() ? pThis->GetMapMgr()->GetUnit(guid) : 0;
	if(!plr)
		return GM_EXCEPTION;

	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)plr;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_unitType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;

	return GM_OK;
}

int Unit_GetGameObject(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(guid, 0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	GameObject * plr = pThis->GetMapMgr() ? pThis->GetMapMgr()->GetGameObject(guid) : 0;
	if(!plr)
		return GM_EXCEPTION;

	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)plr;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_gameObjectType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;

	return GM_OK;
}

int Unit_ChangeEntry(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(entry, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	WorldPacket data(200);
	pThis->BuildFieldUpdatePacket(&data, OBJECT_FIELD_ENTRY, entry);
	pThis->SendMessageToSet(&data, true);
	return GM_OK;
}

int Unit_ChangeModel(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(model, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	pThis->SetUInt32Value(UNIT_FIELD_DISPLAYID, model);
	return GM_OK;
}

int Unit_ChangeScale(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_FLOAT_PARAM(scale, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	pThis->SetFloatValue(OBJECT_FIELD_SCALE_X, scale);
	return GM_OK;
}

int Unit_ChangeFaction(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(meh, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	pThis->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, meh);
	pThis->_setFaction();
	pThis->UpdateOppFactionSet();
	return GM_OK;
}

int Unit_TextEmote(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(msg, 0);
	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->SendChatMessage(CHAT_MSG_MONSTER_EMOTE, LANG_UNIVERSAL, msg);
	return GM_OK;
}

int Unit_SendChatMessageAltEntry(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_STRING_PARAM(msg, 1);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->SendChatMessageAlternateEntry(entry, CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, msg);
	return GM_OK;
}

int GM_GetUnitBySqlId(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(id, 0);
	Object * o = GetThisPointer<Object>(a_thread);
	Creature * pc = o->IsInWorld() ? o->GetMapMgr()->GetSqlIdCreature(id) : NULL;
	if(!pc)
	{
		GM_EXCEPTION_MSG("could not find entrysqlid");
		return GM_EXCEPTION;
	}

	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)pc;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_unitType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;

	return GM_OK;
}

int Unit_PlaySoundToSet(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(id, 0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << uint32(id);
	pThis->SendMessageToSet(&data, true);
	return GM_OK;
}

/**
 * Updates a player to the specified level
 */
int Player_SetLevel(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(level, 0);

	Player * p = GetThisPointer<Player>(a_thread);

	int32 curLevel = p->getLevel();
	if ( curLevel >= level )	// player has already reached or exceeded the requested level
		return GM_OK;

	// deny the setting of level above server limits
	if ( p->GetSession()->HasFlag(ACCOUNT_FLAG_XPACK_01) )
	{
		if ( level > (int32)sWorld.Expansion1LevelCap )
			level = (int32)sWorld.Expansion1LevelCap;
	} else {
		if ( level > (int32)sWorld.LevelCap )
			level = (int32)sWorld.LevelCap;
	}

	for( ; curLevel < level ; curLevel++ )
	{
		p->GiveXP(p->GetUInt32Value(PLAYER_NEXT_LEVEL_XP) - p->GetUInt32Value(PLAYER_XP), p->GetGUID(), true);
	}

	sSocialMgr.SendUpdateToFriends( p );

	return GM_OK;
}

/**
 * Updates player level by the specified amount
 */
int Player_LevelUp(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(amount, 0);
	
	if (amount < 1)
		amount = 1;

	Player * p = GetThisPointer<Player>(a_thread);

	uint32 curLevel = p->getLevel();		// current level
	uint32 endLevel = curLevel + amount;		// result level

	if ( p->GetSession()->HasFlag(ACCOUNT_FLAG_XPACK_01) )
	{
		if ( endLevel > sWorld.Expansion1LevelCap )
			endLevel = sWorld.Expansion1LevelCap;
	} else {
		if ( endLevel > sWorld.LevelCap )
			endLevel = sWorld.LevelCap;
	};

	for( ; curLevel < endLevel; curLevel++ )
	{
		p->GiveXP(p->GetUInt32Value(PLAYER_NEXT_LEVEL_XP) - p->GetUInt32Value(PLAYER_XP), p->GetGUID(), true);
	}

	sSocialMgr.SendUpdateToFriends( p );

	return GM_OK;
}

/**
 * Player_Kick()
 *
 * Kicks the player from the server
 *
 * @param int Delay in seconds before kicking
 */
int Player_Kick(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(delay, 0);		// delay seconds

	if (delay < 0)
		delay = 0;

	GetThisPointer<Player>(a_thread)->Kick(delay * 1000);
	return GM_OK;
}

int Unit_SendYellMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(text, 0);
	GetThisPointer<Unit>(a_thread)->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, text);
	return GM_OK;
}

/*int Player_GetSelectedCreature(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(show_message, 0);

	Player * pThis = GetThisPointer<Player>(a_thread);
	Creature * pCreature = sChatHandler.getSelectedCreature(pThis->GetSession(), show_message);
	if(ScriptSystem->m_userObjectCounter == 9)
		return GM_EXCEPTION;

	gmUserObject * obj = ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter++];
	obj->m_userType = ScriptSystem->m_unitType;
	obj->m_user = pCreature;

	a_thread->PushUser(obj);
}
*/


int GM_SPRINTF(gmThread * a_thread)
{
	if(a_thread->GetNumParams() < 1)
		return GM_EXCEPTION;

	char buffer[2048];
	int param_count = a_thread->GetNumParams()-1;
	char** params = param_count ? new char*[param_count] : NULL;
	GM_CHECK_STRING_PARAM(format,0);
    for(int i = 0; i < param_count; ++i)
	{
		if(a_thread->Param(i).m_type == GM_STRING)
			params[i] = (char*)a_thread->ParamString(i, "INVALID_STRING");
		else
			params[i] = (char*)a_thread->Param(i).m_value.m_ref;
	}

	/* this is the ugly bit. */
	char ** p = params;
	switch(param_count)
	{
	case 0:
		snprintf(buffer, 2048, format);
		break;

	case 1:
		snprintf(buffer, 2048, format, params[0]);
		break;

	case 2:
		snprintf(buffer, 2048, format, params[0], params[1]);
		break;

	case 3:
		snprintf(buffer, 2048, format, p[0], p[1], p[2]);
		break;

	case 4:
		snprintf(buffer, 2048, format, p[0], p[1], p[2], p[3]);
		break;

	case 5:
		snprintf(buffer, 2048, format, p[0], p[1], p[2], p[3], p[4]);
		break;

	case 6:
		snprintf(buffer, 2048, format, p[0], p[1], p[2], p[3], p[4], p[5]);
		break;

	case 7:
		snprintf(buffer, 2048, format, p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
		break;

	case 8:
		snprintf(buffer, 2048, format, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
		break;

	case 9:
		snprintf(buffer, 2048, format, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);
		break;

	case 10:
		snprintf(buffer, 2048, format, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[10]);
		break;

	default:
		delete [] p;
		GM_EXCEPTION_MSG("Expecting less than 10 arguments to sprintf.");
		return GM_EXCEPTION;
		break;
	}

    a_thread->PushNewString(buffer, strlen(buffer));
	delete [] params;
	return GM_OK;
}

int Unit_GetName(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Object * obj = GetThisPointer<Object>(a_thread);
	if(!obj) return GM_EXCEPTION;

	switch(obj->GetTypeId())
	{
	case TYPEID_PLAYER:
		a_thread->PushNewString(((Player*)obj)->GetName());
		break;

	case TYPEID_UNIT:
		{
			/* are we a pet ? */
			if(obj->GetGUIDHigh() == HIGHGUID_PET)
				a_thread->PushNewString(((Pet*)obj)->GetName().c_str());
			else
				a_thread->PushNewString(((Creature*)obj)->GetCreatureName() ? ((Creature*)obj)->GetCreatureName()->Name : "Unknown Entity");
		}break;

	default:
		GM_EXCEPTION_MSG("Unknown typeid.");
		return GM_EXCEPTION;
	}

	return GM_OK;
}

int GM_GetDistance(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_USER_PARAM(Object*, ScriptSystem->m_unitType, obj1, 0);
	GM_CHECK_USER_PARAM(Object*, ScriptSystem->m_unitType, obj2, 1);

	if(!obj1->IsInWorld() || !obj2->IsInWorld())
	{
		GM_EXCEPTION_MSG("Objects not in world.");
		return GM_EXCEPTION;
	}

	a_thread->PushFloat(obj1->GetDistance2dSq(obj2));
	return GM_OK;
}

int Unit_GetClosestPlayer(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	if(!pThis->IsInWorld())
	{
		GM_EXCEPTION_MSG("Unit is not in world!");
		return GM_EXCEPTION;
	}
	
	Player * closest = NULL;
	for(set<Player*>::iterator itr = pThis->GetInRangePlayerSetBegin(); itr != pThis->GetInRangePlayerSetEnd(); ++itr)
	{
		if((closest == NULL || closest->GetDistance2dSq(pThis) > (*itr)->GetDistance2dSq(pThis)) && pThis != (*itr))
			closest = (*itr);
	}

	if(closest == NULL)
	{
		return GM_EXCEPTION;
	}

	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)closest;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_unitType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;
	
	return GM_OK;
}

int Unit_GetRandomPlayer(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	if(!pThis->IsInWorld())
	{
		GM_EXCEPTION_MSG("Unit is not in world!");
		return GM_EXCEPTION;
	}
	uint32 count= 0;
	for(set<Player*>::iterator itr = pThis->GetInRangePlayerSetBegin(); itr != pThis->GetInRangePlayerSetEnd(); ++itr)
		count++;
	if (count==0)
		return GM_EXCEPTION;
	uint32 r = rand() %count;
	count=0;
	Player* result = NULL;
	for(set<Player*>::iterator itr = pThis->GetInRangePlayerSetBegin(); itr != pThis->GetInRangePlayerSetEnd(); ++itr)
	{
		if (count!=r)
			count++;
		else
		{
			result = (*itr);
			break;
		}
	}
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)result;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_unitType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;
	return GM_OK;
}



int Unit_GetClosestUnit(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	if(!pThis->IsInWorld())
	{
		GM_EXCEPTION_MSG("Unit is not in world!");
		return GM_EXCEPTION;
	}

	Unit * closest = NULL;
	for(set<Object*>::iterator itr = pThis->GetInRangeSetBegin(); itr != pThis->GetInRangeSetEnd(); ++itr)
	{
		if(closest == NULL || closest->GetDistance2dSq(pThis) > (*itr)->GetDistance2dSq(pThis) && pThis != (*itr) && (*itr)->GetTypeId() != TYPEID_GAMEOBJECT)
			closest = (Unit*)(*itr);
	}

	if(closest == NULL)
	{
		return GM_EXCEPTION;
	}

	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)closest;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_unitType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;

	return GM_OK;
}

int Unit_InCombat(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(!pThis->IsInWorld())
	{
		return GM_EXCEPTION;
	}
	if(pThis->isInCombat())
		a_thread->PushInt(1);
	else
		a_thread->PushInt(0);
	return GM_OK;
}

int Unit_GetClosestUnitByEntry(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(entry, 0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	Creature * closest = NULL;
	for(set<Object*>::iterator itr = pThis->GetInRangeSetBegin(); itr != pThis->GetInRangeSetEnd(); ++itr)
	{
		if(closest == NULL || closest->GetDistance2dSq(pThis) > (*itr)->GetDistance2dSq(pThis) && pThis != (*itr) && (*itr)->GetTypeId() != TYPEID_GAMEOBJECT && (*itr)->GetTypeId() != TYPEID_PLAYER && ((Creature*)(*itr))->GetEntry() == entry)
			closest = (Creature*)(*itr);
	}
	if(closest == NULL)
	{
		a_thread->PushNull();
		return GM_OK;
	}
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)closest;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_unitType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;
	return GM_OK;
}

int Unit_GetPositionX(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	if(!pThis->IsInWorld())
		return GM_EXCEPTION;

	a_thread->PushFloat(pThis->GetPositionX());
	return GM_OK;
}

int Unit_GetPositionY(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	if(!pThis->IsInWorld())
		return GM_EXCEPTION;

	a_thread->PushFloat(pThis->GetPositionY());
	return GM_OK;
}

int Unit_GetPositionZ(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	if(!pThis->IsInWorld())
		return GM_EXCEPTION;

	a_thread->PushFloat(pThis->GetPositionZ());
	return GM_OK;
}

int Unit_GetFacing(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Object * pThis = GetThisPointer<Object>(a_thread);
	if(!pThis->IsInWorld())
		return GM_EXCEPTION;

	a_thread->PushFloat(pThis->GetOrientation());
	return GM_OK;
}

int Unit_AddThreat(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_USER_PARAM(Object*, ScriptSystem->m_unitType, target, 0);
	GM_CHECK_INT_PARAM(damage, 1);

	if(target->GetTypeId() == TYPEID_GAMEOBJECT) // Come on, wtf are you smoking?
		return GM_OK;

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	pThis->GetAIInterface()->AttackReaction((Unit*)target, damage, 0);

	return GM_OK;
}

int Unit_ClearHateList(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);

	Unit * pThis = GetThisPointer<Unit>(a_thread);

	if(!pThis->IsInWorld())
	{
		GM_EXCEPTION_MSG("Unit is not in world!");
		return GM_EXCEPTION;
	}

	if(pThis->GetTypeId() != TYPEID_UNIT) //this should never ocure
		return GM_EXCEPTION;

	pThis->GetAIInterface()->WipeTargetList();

	return GM_OK;
}

int Unit_ForgetHate(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_USER_PARAM(Unit*, ScriptSystem->m_unitType, target, 0);

	Unit * pThis = GetThisPointer<Unit>(a_thread);

	if(!pThis->IsInWorld())
	{
		GM_EXCEPTION_MSG("Unit is not in world!");
		return GM_EXCEPTION;
	}

	if(pThis->GetTypeId() != TYPEID_UNIT) //this should never ocure
		return GM_EXCEPTION;

	pThis->GetAIInterface()->RemoveThreatByPtr(target);

	return GM_OK;
}

int Unit_GetMostHated(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(!pThis->IsInWorld())
	{
		GM_EXCEPTION_MSG("Unit is not in world!");
		return GM_EXCEPTION;
	}

	if(pThis->GetTypeId() != TYPEID_UNIT) //this should never ocure
		return GM_EXCEPTION;

	Unit * mosthated = pThis->GetAIInterface()->GetMostHated();

	if(mosthated == NULL)
	{
		return GM_EXCEPTION;
	}

	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)mosthated;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_unitType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;

	return GM_OK;
}

int Unit_IsInMeleRange(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_USER_PARAM(Unit*, ScriptSystem->m_unitType, unit1, 0);
	GM_CHECK_USER_PARAM(Unit*, ScriptSystem->m_unitType, unit2, 1);

	float distance = unit1->CalcDistance(unit2);
	float combatrange=unit1->GetAIInterface()->_CalcCombatRange(unit2, false);

	a_thread->PushInt(combatrange>=distance);
	return GM_OK;
}

int Unit_ReturnToSpawn(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(!pThis->IsInWorld())
		return GM_OK;

	Creature * pCreature = static_cast<Creature*>(pThis);
	pCreature->GetAIInterface()->SetAIState(STATE_IDLE);
	pCreature->GetAIInterface()->WipeHateList();
	pCreature->GetAIInterface()->WipeTargetList();
	pCreature->GetAIInterface()->MoveTo(pCreature->m_spawn->x, pCreature->m_spawn->y, pCreature->m_spawn->z, pCreature->m_spawn->o);

	return GM_OK;
}

int Unit_Spawngameobject(gmThread * a_thread)
{
    GM_CHECK_NUM_PARAMS(6);
    GM_CHECK_INT_PARAM(entry, 0);
    GM_CHECK_INT_PARAM(mapID, 1);
    GM_CHECK_FLOAT_PARAM(posX, 2);
    GM_CHECK_FLOAT_PARAM(posY, 3);
    GM_CHECK_FLOAT_PARAM(posZ, 4);
    GM_CHECK_FLOAT_PARAM(facing, 5);
    Unit * pThis = GetThisPointer<Unit>(a_thread);
    GameObjectInfo * p = GameObjectNameStorage.LookupEntry(entry);
    if(!p)
        return GM_EXCEPTION;
    GameObject * pGameObject = pThis->GetMapMgr()->CreateGameObject();
    pGameObject->spawnid = 0;
    pGameObject->m_spawn = 0;
    if(!pGameObject->CreateFromProto(entry,mapID,posX,posY,posZ,facing))
           {
                delete pGameObject;
                return 0;
            }
    pGameObject->SetUInt32Value(GAMEOBJECT_DYN_FLAGS,1);
    pGameObject->SetMapId(pThis->GetMapId());
    pGameObject->SetInstanceID(pThis->GetInstanceID());    
    pGameObject->PushToWorld(pThis->GetMapMgr());
    return GM_OK;
}