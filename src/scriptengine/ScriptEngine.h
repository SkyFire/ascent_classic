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

#include "gmMachine.h"
#include "gmUserObject.h"
#include "gmCall.h"

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

/** Forward Declarations
 */
class Creature;
class Unit;
class Player;
struct Quest;
struct AreaTrigger;

/** Quest Events
 */
enum QuestEvents
{
	QUEST_EVENT_ON_COMPLETE		= 1,
	QUEST_EVENT_ON_ACCEPT		= 2,
	QUEST_EVENT_CAN_ACCEPT		= 3,
	QUEST_EVENT_COUNT,
};

/** Creature Events
 */
enum CreatureEvents
{
	CREATURE_EVENT_ON_ENTER_COMBAT		= 1,
	CREATURE_EVENT_ON_LEAVE_COMBAT		= 2,
	CREATURE_EVENT_ON_KILLED_TARGET		= 3,
	CREATURE_EVENT_ON_DIED				= 4,
	CREATURE_EVENT_AI_TICK				= 5,
	CREATURE_EVENT_ON_SPAWN				= 6,
	CREATURE_EVENT_ON_GOSSIP_TALK		= 7,
	CREATURE_EVENT_ON_REACH_WP			= 8,
	CREATURE_EVENT_ON_LEAVE_LIMBO		= 9,
	CREATURE_EVENT_PLAYER_ENTERS_RANGE	= 10,
	CREATURE_EVENT_COUNT,
};

/** GameObject Events
 */
enum GameObjectEvents
{
	GAMEOBJECT_EVENT_ON_SPAWN			= 1,
	GAMEOBJECT_EVENT_ON_USE				= 2,
	GAMEOBJECT_EVENT_COUNT,
};

/** @class ScriptEngine
 * Provides an interface for creatures to interface with serverside scripts. This class is created
 * once per instance.
 */

class ScriptEngine
{
	/** gmMachine Instance
	 */
	gmMachine * m_machine;

public:
	/** gmType variables (for player, unit, gameobject)
	 */
	gmType m_playerType;
	gmType m_unitType;
	gmType m_gameObjectType;
	gmType m_questType;
	gmType m_spellType;
	gmType m_auraType;
	gmType m_areaTriggerType;
	gmType m_scriptEngineType;
	
	/** Allowed pointer types
	 */
	list<gmType> m_allowedTypes;

protected:
	/** ScriptMap - binding of events to gm functions
	 */
	typedef HM_NAMESPACE::hash_map<uint32, map<uint32, gmFunctionObject*> > ScriptMap;
	typedef HM_NAMESPACE::hash_map<uint32, gmFunctionObject*> SingleScriptMap;
	ScriptMap m_PlayerStorageMap;
	ScriptMap m_unitMap;
	ScriptMap m_gameObjectMap;
	ScriptMap m_questMap;
	ScriptMap m_spellMap;
	ScriptMap m_auraMap;
	SingleScriptMap m_areaTriggerMap;

	/** Mutex - ensures a script isn't executed twice at once
	 */
	Mutex m_lock;

public:

	/** Lock Grabber
	 */
	inline Mutex & GetLock() { return m_lock; }

	/** User objects, used in passing of arguments and used for 'this' pointer.
	 */
	gmVariable m_variables[10];
	gmUserObject * m_userObjects[10];
	
	/** UserObject counter */
	uint32 m_userObjectCounter;

protected:
	/** Gets the function table of scriptable exports.
	 */
	void SetPlayerFunctionTable();
	void SetUnitFunctionTable();
	void SetGameObjectFunctionTable();
	void SetQuestFunctionTable();
	void SetSpellFunctionTable();
	void SetAuraFunctionTable();
	void SetAreaTriggerFunctionTable();
	void SetScriptEngineFunctionTable();

public:
	/** Function pointer setting/adding public functions
	 */
	inline void AddAreaTriggerEvent(uint32 Entry, gmFunctionObject * func) { m_areaTriggerMap[Entry] = func; }
	inline void AddQuestEvent(uint32 Entry, uint32 Type, gmFunctionObject * func) { m_questMap[Entry].insert( make_pair( Type, func ) ); }
	inline void AddCreatureEvent(uint32 Entry, uint32 Type, gmFunctionObject * func) { m_unitMap[Entry].insert( make_pair( Type, func ) ); }
	inline void AddGameObjectEvent(uint32 Entry, uint32 Type, gmFunctionObject * func) { m_gameObjectMap[Entry].insert( make_pair( Type, func ) ); }

	/** Constructor - does nothing but nulls out variables
	 */
	ScriptEngine();

	/** Destructor - does nothing but frees pointers
	 */
	~ScriptEngine();

	/** Reloads the script system, recompiling all scripts and registering any functions.
	 */
	void Reload();

	/** Registers a script trigger to a function in script.
	 */
	void RegisterAreaTriggerEvent(uint32 Entry, gmFunctionObject * Function, gmType Type);

	/** Executes a script file
	 */
	void ExecuteScriptFile(const char * filename);

	/** Dumps the error log from the script engine
	 */
	void DumpErrors();

	/** Sets up a gm call with the specified number of arguments
	 */
	void DoGMCall(gmFunctionObject * obj, uint32 ArgumentCount, int * return_value);

	/** Sets up a userobject and variable to this type and pointer.
	 */
	inline void SetVariable(uint32 Index, void * Pointer, gmType Type)
	{
		m_userObjects[Index]->m_userType = Type;
		m_userObjects[Index]->m_user = Pointer;
		m_variables[Index].SetUser(m_userObjects[Index]);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Looks up script for areatrigger id x and executes it, return value of true if you can activate it.
	 */
	bool OnActivateAreaTrigger(AreaTrigger * at, Player * plr);
	
	/** Looks up a script on quest event and executes it.
	 */
	bool OnQuestEvent(Quest * quest, Creature * pQuestGiver, Player * plr, uint32 Event);
	bool OnQuestRequireEvent(Quest * quest, Creature * pQuestGiver, Player * plr, uint32 Event);

	/** Looks up a script on creature event and executes it.
	 */
	bool OnCreatureEvent(Creature * pCreature, gmFunctionObject * pointer);
	bool OnCreatureEvent(Creature * pCreature, Unit * pAttacker, uint32 Event);
	bool OnCreatureEventArg(Creature * pCreature, uint32 Argument, uint32 Event);

	/** Looks up a script on a gameobject and executes it.
	 */
	bool OnGameObjectEvent(GameObject * pGameObject, Player * pUser, uint32 Event);

	/** Returns the machine
	 */
	inline gmMachine * GetMachine() { return m_machine; }

	/** Has event of type?
	 */
	bool HasEventType(uint32 Entry, uint32 Event);
};

/* gonna make one global scriptengine for testing */
extern ScriptEngine * ScriptSystem;

#endif

