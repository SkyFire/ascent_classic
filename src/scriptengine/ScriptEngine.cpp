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
ScriptEngine * ScriptSystem;

#ifndef WIN32
#include <dirent.h>
#endif

ScriptEngine::ScriptEngine()
{
	m_playerType = m_unitType = m_gameObjectType = m_questType = m_spellType = m_auraType = m_areaTriggerType = m_scriptEngineType = -1;
	m_machine = 0;
}

ScriptEngine::~ScriptEngine()
{
	if(m_machine)
	{
		//for(uint32 i = 0; i < 10; ++i)
			//m_machine->DestructDeleteObject(m_userObjects[i]);
		m_machine->GetGC()->DestructAll();
		
		delete m_machine;
	}
}

void ScriptEngine::Reload()
{
	if(m_machine)
	{
		m_playerType = m_unitType = m_gameObjectType = m_questType = m_spellType = m_auraType = m_areaTriggerType = m_scriptEngineType = -1;
		for(int i = 0; i < 10; ++i)
			m_userObjects[i] = 0;

		m_PlayerStorageMap.clear();
		m_unitMap.clear();
		m_gameObjectMap.clear();
		m_questMap.clear();
		m_auraMap.clear();
		m_areaTriggerMap.clear();
		
		m_machine->ResetAndFreeMemory();
		delete m_machine;

		m_machine = 0;
	}

	m_machine = new gmMachine;
	
	/* create our types */
	m_playerType		= m_machine->CreateUserType("Player");
	m_unitType			= m_machine->CreateUserType("Unit");
	m_gameObjectType	= m_machine->CreateUserType("GameObject");
	m_questType			= m_machine->CreateUserType("Quest");
	m_spellType			= m_machine->CreateUserType("Spell");
	m_auraType			= m_machine->CreateUserType("Aura");
	m_areaTriggerType	= m_machine->CreateUserType("AreaTrigger");
	m_scriptEngineType	= m_machine->CreateUserType("ScriptEngine");

	m_allowedTypes.push_back(m_playerType);
	m_allowedTypes.push_back(m_unitType);
	m_allowedTypes.push_back(m_gameObjectType);
	m_allowedTypes.push_back(m_questType);
	m_allowedTypes.push_back(m_spellType);
	m_allowedTypes.push_back(m_auraType);
	m_allowedTypes.push_back(m_areaTriggerType);
	m_allowedTypes.push_back(m_scriptEngineType);

	/* register all our functions inside the machine */
	SetPlayerFunctionTable();
	SetUnitFunctionTable();
	SetGameObjectFunctionTable();
	SetQuestFunctionTable();
	SetSpellFunctionTable();
	SetAuraFunctionTable();
	SetAreaTriggerFunctionTable();
	SetScriptEngineFunctionTable();

	/* allocate our user data variables */
	for(int i = 0; i < 10; ++i)
	{
		m_userObjects[i] = m_machine->AllocUserObject(this, m_scriptEngineType);
		m_machine->AddCPPOwnedGMObject(m_userObjects[i]);
	}

	sLog.outString("Compiling GameMonkey Scripts...");

#ifdef WIN32
	/* compile the scripts */
	WIN32_FIND_DATA fd;
	HANDLE f = FindFirstFile("scripts\\*.gm", &fd);
	if(f != INVALID_HANDLE_VALUE)
	{
		do 
		{
			string fname = "scripts/";
			fname += fd.cFileName;

			ExecuteScriptFile(fname.c_str());
		} while(FindNextFile(f, &fd));
		FindClose(f);
	}
#else
	/* compile scripts */
	struct dirent ** list;
	int filecount = scandir("scripts/", &list, 0, 0);
	if(!filecount || !list || filecount < 0)
		return;

	char * ext;
	while(filecount--)
	{
		ext = strrchr(list[filecount]->d_name, '.');
		if(ext != NULL && !strcmp(ext, ".gm"))
		{
			string full_path = "scripts/" + string(list[filecount]->d_name);
			ExecuteScriptFile(full_path.c_str());
		}
		free(list[filecount]);
	}
	free(list);
#endif

	printf("\nScripts compiled.\n\n");
}

void ScriptEngine::ExecuteScriptFile(const char * filename)
{
	char * data;
	FILE * f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	data = new char[size+1];
	fread(data, 1, size, f);
	data[size] = 0;

	fclose(f);

	/* setup 'this' pointer */
	m_userObjects[0]->m_user = this;
	m_userObjects[0]->m_userType = m_scriptEngineType;

	/* set 'this' variable */
	m_variables[0].SetUser(m_userObjects[0]);

	int threadid;
	printf("  %s: ", strstr(filename, "/")+1);
	int no_errors = m_machine->ExecuteString(data, &threadid, true, filename, &m_variables[0]);
	printf("%u errors.\n", no_errors);
	if(no_errors)
	{
		printf("Errors occured while compiling %s.\n", filename);
		DumpErrors();
	}

	delete [] data;
}

void ScriptEngine::DumpErrors()
{
	// sLog.outString("Dumping errors from script action: ");
	
	bool first = true;
	const char * message = m_machine->GetLog().GetEntry(first);
	while(message)
	{
		printf("GM_Debug:  %s", message);
		first = false;
		message = m_machine->GetLog().GetEntry(first);
	}
	// sLog.outString("End of error dump.");
}

void ScriptEngine::DoGMCall(gmFunctionObject * obj, uint32 ArgumentCount, int * return_value)
{
	gmCall call;
	if(call.BeginFunction(m_machine, obj, m_variables[0], false))
	{
		for(uint32 i = 0; i < ArgumentCount; ++i)
			call.AddParam(m_variables[1+i]);

		m_userObjectCounter = ArgumentCount + 1;
		call.End();

		if(return_value != 0)
		{
			int v;
			if(call.GetReturnedInt(v))
				*return_value = v;
			else
				*return_value = 1;
		}

		DumpErrors();
	}
	else
	{
		printf("Could not find function!");
		DumpErrors();
	}
}

bool ScriptEngine::OnActivateAreaTrigger(AreaTrigger * at, Player * plr)
{
	SingleScriptMap::iterator itr = m_areaTriggerMap.find(at->AreaTriggerID);
	if(itr == m_areaTriggerMap.end())
		return true;

	/*map<uint32, gmFunctionObject*>::iterator it2 = itr->second.find(0);
	if(it2 == itr->second.end())
		return true;*/

	m_lock.Acquire();

	ASSERT(itr->second->GetType() == GM_FUNCTION);

	// Setup 'this' pointer to the areatrigger struct.
	m_userObjects[0]->m_user = at;
	m_userObjects[0]->m_userType = m_areaTriggerType;
	m_variables[0].SetUser(m_userObjects[0]);

	// Setup the first argument (the player entering)
	m_userObjects[1]->m_user = plr;
	m_userObjects[1]->m_userType = m_playerType;
	m_variables[1].SetUser(m_userObjects[1]);
	m_userObjectCounter = 2;

	// Setup the call.
	gmCall call;
	if(call.BeginFunction(m_machine, itr->second, m_variables[0], false))
	{
		call.AddParam(m_variables[1]);
		call.End();

		DumpErrors();
		int res;
		if(!call.GetReturnedInt(res))
		{
			printf("Call failed.");
			m_lock.Release();
			return true;
		}

		m_lock.Release();
		return (res > 0) ? true : false;
	}
	else
	{
		printf("Could not find function!");
		DumpErrors();
		m_lock.Release();
		return true;
	}
}

bool ScriptEngine::OnQuestEvent(Quest * quest, Creature * pQuestGiver, Player * plr, uint32 Event)
{
	if(!m_questMap.size())
		return false;

	ScriptMap::iterator itr = m_questMap.find(quest->id);
	if(itr == m_questMap.end())
		return false;

	map<uint32, gmFunctionObject*>::iterator it2 = itr->second.find(Event);
	if(it2 == itr->second.end() )
		return false;

	m_lock.Acquire();
	SetVariable(0, quest, m_questType);
	SetVariable(1, pQuestGiver, m_unitType);
	SetVariable(2, plr, m_playerType);

	DoGMCall(it2->second, 2, 0);
	m_lock.Release();
	return true;
}

bool ScriptEngine::OnQuestRequireEvent(Quest * quest, Creature * pQuestGiver, Player * plr, uint32 Event)
{
	if(!m_questMap.size())
		return true;;

	ScriptMap::iterator itr = m_questMap.find(quest->id);
	if(itr == m_questMap.end())
		return true;

	map<uint32, gmFunctionObject*>::iterator it2 = itr->second.find(Event);
	if(it2 == itr->second.end() )
		return true;

	m_lock.Acquire();
	SetVariable(0, quest, m_questType);
	SetVariable(1, pQuestGiver, m_unitType);
	SetVariable(2, plr, m_playerType);

	int ret;
	DoGMCall(it2->second, 2, &ret);
	m_lock.Release();
	return (ret > 0) ? true : false;
}

bool ScriptEngine::OnCreatureEvent(Creature * pCreature, Unit * pAttacker, uint32 Event)
{
	if(!m_unitMap.size())
		return false;

	ScriptMap::iterator itr = m_unitMap.find(pCreature->GetEntry());
	if(itr == m_unitMap.end())
		return false;

	map<uint32, gmFunctionObject*>::iterator it2 = itr->second.find(Event);
	if(it2 == itr->second.end() )
		return false;

	m_lock.Acquire();
	SetVariable(0, pCreature, m_unitType);
	SetVariable(1, pAttacker, m_playerType);

	DoGMCall(it2->second, 1, 0);
	m_lock.Release();
	return true;
}

bool ScriptEngine::OnCreatureEventArg(Creature * pCreature, uint32 Argument, uint32 Event)
{
	if(!m_unitMap.size())
		return false;

	ScriptMap::iterator itr = m_unitMap.find(pCreature->GetEntry());
	if(itr == m_unitMap.end())
		return false;

	map<uint32, gmFunctionObject*>::iterator it2 = itr->second.find(Event);
	if(it2 == itr->second.end() )
		return false;

	gmFunctionObject * obj = it2->second;
	m_lock.Acquire();

	ASSERT(obj->GetType() == GM_FUNCTION);

	SetVariable(0, pCreature, m_unitType);
	gmCall call;
	if(call.BeginFunction(m_machine, obj, m_variables[0], false))
	{
		call.AddParamInt(Argument);
		m_userObjectCounter = 2;
		call.End();
		DumpErrors();
	}
	else
	{
		printf("Could not find function!");
		DumpErrors();
	}

	m_lock.Release();
	return true;
}

bool ScriptEngine::OnCreatureEvent(Creature * pCreature, gmFunctionObject * pointer)
{
	if(!m_unitMap.size())
		return false;

	m_lock.Acquire();
	SetVariable(0, pCreature, m_unitType);
	DoGMCall(pointer, 0, 0);
	m_lock.Release();
	return true;
}

bool ScriptEngine::OnGameObjectEvent(GameObject * pGameObject, Player * pUser, uint32 Event)
{
	if(!m_gameObjectMap.size())
		return false;

	ScriptMap::iterator itr = m_gameObjectMap.find(pGameObject->GetEntry());
	if(itr == m_gameObjectMap.end())
		return false;

	map<uint32, gmFunctionObject*>::iterator it2 = itr->second.find(Event);
	if(it2 == itr->second.end() )
		return false;

	m_lock.Acquire();
	SetVariable(0, pGameObject, m_gameObjectType);
	SetVariable(1, pUser, m_playerType);

	DoGMCall(it2->second, 1, 0);
	m_lock.Release();
	return true;
}

bool ScriptEngine::HasEventType(uint32 Entry, uint32 Event)
{
	ScriptMap::iterator itr = m_unitMap.find(Entry);
	if(itr == m_unitMap.end())
		return false;

	if(itr->second.find(Event) != itr->second.end())
		return false;

	return true;
}
