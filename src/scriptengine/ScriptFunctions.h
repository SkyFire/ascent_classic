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

#ifndef SCRIPT_FUNCTIONS_H
#define SCRIPT_FUNCTIONS_H

#include "ScriptEngine.h"

/* Helper Functions */
template<class T>
T * GetThisPointer(gmThread * a_thread)
{
	return (T*)a_thread->ThisUser();
}

/* Script Engine Functions */
	int GM_CDECL ScriptEngine_RegisterUnitEvent(gmThread * a_thread);
	int GM_CDECL ScriptEngine_RegisterGameObjectEvent(gmThread * a_thread);
	int GM_CDECL ScriptEngine_RegisterSpellEvent(gmThread * a_thread);
	int GM_CDECL ScriptEngine_RegisterAreaTriggerEvent(gmThread * a_thread);
	int GM_CDECL ScriptEngine_RegisterPlayerEvent(gmThread * a_thread);
	int GM_CDECL ScriptEngine_RegisterQuestEvent(gmThread * a_thread);

/* Player Functions */
	int GM_CDECL Player_BroadcastMessage(gmThread * a_thread);
	int GM_CDECL Player_SendAreaTriggerMessage(gmThread * a_thread);
	int GM_CDECL Player_GetLevel(gmThread * a_thread);
	int GM_CDECL Player_GetClass(gmThread * a_thread);
	int GM_CDECL Player_GetRace(gmThread * a_thread);
	int GM_CDECL Player_Teleport(gmThread * a_thread);
	int GM_CDECL Unit_RemoveAura(gmThread * a_thread);
    int GM_CDECL Player_HasQuest(gmThread * a_thread);
    int GM_CDECL Player_HasItem(gmThread * a_thread);
	int GM_CDECL Player_HasFinishedQuest(gmThread * a_thread);
	int GM_CDECL Player_GetReputationRank(gmThread * a_thread);
	int GM_CDECL Player_GetReputationValue(gmThread * a_thread);
	int GM_CDECL Player_IsGroupLeader(gmThread * a_thread);
	int GM_CDECL Player_LearnSpell(gmThread * a_thread);
	int GM_CDECL Player_RemoveSpell(gmThread * a_thread);
	int GM_CDECL Player_AddItem(gmThread * a_thread);
	int GM_CDECL Player_RemoveItem(gmThread * a_thread);
	int GM_CDECL Player_MarkQuestObjectiveAsComplete(gmThread * a_thread);
	int GM_CDECL Player_SendNotification(gmThread * a_thread);
	int GM_CDECL Player_SendSystemMessage(gmThread * a_thread);
	int GM_CDECL Player_Knockback(gmThread * a_thread);
	int GM_CDECL Player_JoinInstance(gmThread * a_thread);
	int GM_CDECL Player_SetLevel(gmThread * a_thread);
	int GM_CDECL Player_LevelUp(gmThread * a_thread);
	int GM_CDECL Player_Kick(gmThread * a_thread);

/* AreaTrigger Functions */
	int GM_CDECL AreaTrigger_GetEntry(gmThread * a_thread);

/* Unit Functions */
	int GM_CDECL Unit_Despawn(gmThread * a_thread);
	int GM_CDECL Unit_SendChatMessage(gmThread * a_thread);
	int GM_CDECL Unit_SendChatMessageAltEntry(gmThread * a_thread);
	int GM_CDECL Unit_Emote(gmThread * a_thread);
	int GM_CDECL Unit_CastSpell(gmThread * a_thread);
	int GM_CDECL Unit_SetStandState(gmThread * a_thread);
	int GM_CDECL Unit_CastSpellOnTarget(gmThread * a_thread);
	int GM_CDECL Unit_TimedEmote(gmThread * a_thread);
	int GM_CDECL Unit_RegisterTimer(gmThread * a_thread);
	int GM_CDECL Unit_DeregisterTimer(gmThread * a_thread);
	int GM_CDECL Unit_SpawnMonster(gmThread * a_thread);
	int GM_CDECL Unit_SendYellMessage(gmThread * a_thread);
	int GM_CDECL Unit_Spawngameobject(gmThread * a_thread);
	int GM_CDECL Unit_GetGuid(gmThread * a_thread);
	int GM_CDECL Unit_GetPlayer(gmThread * a_thread);
	int GM_CDECL Unit_GetGameObject(gmThread * a_thread);
	int GM_CDECL Unit_GetUnit(gmThread * a_thread);
	int GM_CDECL Unit_GetName(gmThread * a_thread);
	int GM_CDECL Unit_GetHealthPct(gmThread * a_thread);
	int GM_CDECL Unit_GetClosestPlayer(gmThread * a_thread);
	int GM_CDECL Unit_GetClosestUnit(gmThread * a_thread);
	int GM_CDECL Unit_GetRandomPlayer(gmThread * a_thread);
	int GM_CDECL Unit_InCombat(gmThread * a_thread);
	int GM_CDECL Unit_ClearHateList(gmThread * a_thread);
	int GM_CDECL Unit_ForgetHate(gmThread * a_thread);
	int GM_CDECL Unit_GetMostHated(gmThread * a_thread);
	int GM_CDECL Unit_AddThreat(gmThread * a_thread);
	int GM_CDECL Unit_IsInMeleRange(gmThread * a_thread);
	int GM_CDECL Unit_SpawnMonsterWithFaction(gmThread * a_thread);
	int GM_CDECL Unit_FullCastSpell(gmThread * a_thread);
	
	// Escort Quest System
		int GM_CDECL Unit_SpawnWithoutWorld(gmThread * a_thread);
		int GM_CDECL Unit_AddToWorld(gmThread * a_thread);
		int GM_CDECL Unit_CreateCustomWaypointMap(gmThread * a_thread);
		int GM_CDECL Unit_CreateWaypoint(gmThread * a_thread);
		int GM_CDECL Unit_MoveToWaypoint(gmThread * a_thread);
		int GM_CDECL Unit_Delete(gmThread * a_thread);
		int GM_CDECL Unit_SetCombatCapable(gmThread * a_thread);
		int GM_CDECL Unit_HaltMovement(gmThread * a_thread);
		int GM_CDECL Unit_SetMovementType(gmThread * a_thread);
		int GM_CDECL Unit_SetEscortTarget(gmThread * a_thread);
		int GM_CDECL Unit_HasEscortTarget(gmThread * a_thread);
		int GM_CDECL Unit_GetEscortTarget(gmThread * a_thread);
		int GM_CDECL Unit_ClearEscortTarget(gmThread * a_thread);
		int GM_CDECL Unit_SetNPCFlags(gmThread * a_thread);
		int GM_CDECL Unit_DestroyCustomWaypointMap(gmThread * a_thread);
		int GM_CDECL Unit_ChangeEntry(gmThread * a_thread);
		int GM_CDECL Unit_ChangeScale(gmThread * a_thread);
		int GM_CDECL Unit_TextEmote(gmThread * a_thread);
		int GM_CDECL Unit_ChangeModel(gmThread * a_thread);
		int GM_CDECL Unit_ChangeFaction(gmThread * a_thread);
		int GM_CDECL Unit_PlaySoundToSet(gmThread * a_thread);
		int GM_CDECL Unit_GetClosestUnitByEntry(gmThread * a_thread);
		int GM_CDECL Unit_GetPositionX(gmThread * a_thread);
		int GM_CDECL Unit_GetPositionY(gmThread * a_thread);
		int GM_CDECL Unit_GetPositionZ(gmThread * a_thread);
		int GM_CDECL Unit_GetFacing(gmThread * a_thread);
		int GM_CDECL Unit_ReturnToSpawn(gmThread * a_thread);

/* GameObject Functions */
	int GM_CDECL GameObject_Despawn(gmThread * a_thread);
	int GM_CDECL GameObject_PlayCustomAnim(gmThread * a_thread);
	int GM_CDECL GameObject_SetActive(gmThread * a_thread);

/* Misc Helper Functions */
	int GM_CDECL GM_RAND(gmThread * a_thread);
	int GM_CDECL GM_SPRINTF(gmThread * a_thread);
	int GM_CDECL GM_RegisterEvent(gmThread * a_thread);
	int GM_CDECL GM_GetUnitBySqlId(gmThread * a_thread);
	int GM_CDECL GM_GetDistance(gmThread * a_thread);

/* Quest Functions */
	int GM_CDECL Quest_GetID(gmThread * a_thread);

#endif
