/*
 * Ascent MMORPG Server
 * LUA Interface
 * Copyright (C) 2007 Burlex <burlex@gmail.com>
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

#ifndef __ANGELSCRIPTENGINE_H
#define __ANGELSCRIPTENGINE_H

#include "angelscript/include/angelscript.h"

class AngelScriptEngine;
class AngelScriptEngineMgr;

extern AngelScriptEngineMgr g_asMgr;
extern AngelScriptEngine* g_engine;

// Quest Events
enum QuestEvents
{
	QUEST_EVENT_ON_COMPLETE		= 1,
	QUEST_EVENT_ON_ACCEPT		= 2,
	QUEST_EVENT_COUNT,
};

// Creature Events
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

// GameObject Events
enum GameObjectEvents
{
	GAMEOBJECT_EVENT_ON_SPAWN	= 1,
	GAMEOBJECT_EVENT_ON_USE		= 2,
	GAMEOBJECT_EVENT_COUNT,
};

// Random Flags
enum RandomFlags
{
	RANDOM_ANY				= 0,
	RANDOM_IN_SHORTRANGE	= 1,
	RANDOM_IN_MIDRANGE		= 2,
	RANDOM_IN_LONGRANGE		= 3,
	RANDOM_WITH_MANA		= 4,
	RANDOM_WITH_RAGE		= 5,
	RANDOM_WITH_ENERGY		= 6,
	RANDOM_NOT_MAINTANK		= 7,
	RANDOM_COUNT,
};

class AngelScriptEngine
{
private:

//	Mutex m_Lock;

public:
	AngelScriptEngine();
	~AngelScriptEngine();

	void LoadScripts();
	void LoadScript( string filename );

	void Startup();
	void Shutdown();
	void Restart();

//	ASCENT_INLINE Mutex& GetLock() { return m_Lock; }

};

struct AngelScriptUnitBinding
{
	const char* Functions[CREATURE_EVENT_COUNT];
};

struct AngelScriptQuestBinding
{
	const char* Functions[QUEST_EVENT_COUNT];
};

struct AngelScriptGameObjectBinding
{
	const char* Functions[GAMEOBJECT_EVENT_COUNT];
};

class AngelScriptEngineMgr
{
private:

	typedef HM_NAMESPACE::hash_map< uint32, AngelScriptUnitBinding > UnitBindingMap;
	typedef HM_NAMESPACE::hash_map< uint32, AngelScriptQuestBinding > QuestBindingMap;
	typedef HM_NAMESPACE::hash_map< uint32, AngelScriptGameObjectBinding > GameObjectBindingMap;

	UnitBindingMap m_unitBinding;
	QuestBindingMap m_questBinding;
	GameObjectBindingMap m_gameobjectBinding;

public:

	AngelScriptEngine* m_engine;

	void Startup();
	void Shutdown();
	void Restart();

//	LuaUnitBinding * GetUnitBinding(uint32 Id)
//	{
//		UnitBindingMap::iterator itr = m_unitBinding.find(Id);
//		return (itr == m_unitBinding.end()) ? NULL : &itr->second;
//	}

//	LuaQuestBinding * GetQuestBinding(uint32 Id)
//	{
//		QuestBindingMap::iterator itr = m_questBinding.find(Id);
//		return (itr == m_questBinding.end()) ? NULL : &itr->second;
//	}

//	LuaGameObjectBinding * GetGameObjectBinding(uint32 Id)
//	{
//		GameObjectBindingMap::iterator itr =m_gameobjectBinding.find(Id);
//		return (itr == m_gameobjectBinding.end()) ? NULL : &itr->second;
//	}
};

#endif
