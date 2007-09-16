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

#include "StdAfx.h"
#ifndef WIN32
    #include <dlfcn.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <cstdlib>
    #include <cstring>
#endif

initialiseSingleton(ScriptMgr);
initialiseSingleton(HookInterface);

void ScriptMgr::LoadScripts()
{
	if(!HookInterface::getSingletonPtr())
		new HookInterface;

	sLog.outString("Loading External Script Libraries...");
	sLog.outString("");

	string start_path = Config.MainConfig.GetStringDefault("Script", "BinaryLocation", "script_bin") + "\\";
	string search_path = start_path + "*.";

	/* Loading system for win32 */
#ifdef WIN32
	search_path += "dll";

	WIN32_FIND_DATA data;
	uint32 count = 0;
	HANDLE find_handle = FindFirstFile(search_path.c_str(), &data);
	if(find_handle == INVALID_HANDLE_VALUE)
		sLog.outError("  No external scripts found! Server will continue to function with limited functionality.");
	else
	{
		do
		{
			string full_path = start_path + data.cFileName;
			HMODULE mod = LoadLibrary(full_path.c_str());
			printf("  %s : 0x%.08X : ", data.cFileName, ((uint32)mod));
			if(mod == 0)
				printf("error!\n");
			else
			{
				// find version import
				exp_get_version vcall = (exp_get_version)GetProcAddress(mod, "_exp_get_version");
				exp_script_register rcall = (exp_script_register)GetProcAddress(mod, "_exp_script_register");
				if(vcall == 0 || rcall == 0)
				{
					printf("version functions not found!\n");
					FreeLibrary(mod);
				}
				else
				{
					uint32 version = vcall();
					if(UINT32_LOPART(version) == SCRIPTLIB_VERSION_MINOR && UINT32_HIPART(version) == SCRIPTLIB_VERSION_MAJOR)
					{
						_handles.push_back(((SCRIPT_MODULE)mod));
						printf("v%u.%u : ", UINT32_HIPART(version), UINT32_LOPART(version));
						rcall(this);
						printf("loaded.\n");

						++count;
					}
					else
					{
						FreeLibrary(mod);
						printf("version mismatch!\n");						
					}
				}
			}
		}
		while(FindNextFile(find_handle, &data));
		FindClose(find_handle);
		sLog.outString("");
		sLog.outString("Loaded %u external libraries.", count);
		sLog.outString("");
	}
#else
	/* Loading system for *nix */
	struct dirent ** list;
	int filecount = scandir(PREFIX "/lib/", &list, 0, 0);
	uint32 count = 0;

	if(!filecount || !list || filecount < 0)
		sLog.outError("  No external scripts found! Server will continue to function with limited functionality.");
	else
	{
char *ext;
		while(filecount--)
		{
			ext = strrchr(list[filecount]->d_name, '.');
			if (ext != NULL && !strcmp(ext, ".so")) {
				string full_path = "../lib/" + string(list[filecount]->d_name);
				SCRIPT_MODULE mod = dlopen(full_path.c_str(), RTLD_NOW);
				printf("  %s : 0x%08X : ", list[filecount]->d_name, (unsigned int)mod);
				if(mod == 0)
					printf("error! [%s]\n", dlerror());
				else
				{
					// find version import
					exp_get_version vcall = (exp_get_version)dlsym(mod, "_exp_get_version");
					exp_script_register rcall = (exp_script_register)dlsym(mod, "_exp_script_register");
					if(vcall == 0 || rcall == 0)
					{
						printf("version functions not found!\n");
						dlclose(mod);
					}
					else
					{
						uint32 version = vcall();
						if(UINT32_LOPART(version) == SCRIPTLIB_VERSION_MINOR && UINT32_HIPART(version) == SCRIPTLIB_VERSION_MAJOR)
						{
							_handles.push_back(((SCRIPT_MODULE)mod));
							printf("v%u.%u : ", UINT32_HIPART(version), UINT32_LOPART(version));
							rcall(this);
							printf("loaded.\n");

							++count;
						}
						else
						{
							dlclose(mod);
							printf("version mismatch!\n");						
						}
					}
				}
			}
			free(list[filecount]);
		}
		free(list);
		sLog.outString("");
		sLog.outString("Loaded %u external libraries.", count);
		sLog.outString("");
	}
#endif
}

void ScriptMgr::UnloadScripts()
{
	if(HookInterface::getSingletonPtr())
		delete HookInterface::getSingletonPtr();

	LibraryHandleMap::iterator itr = _handles.begin();
	for(; itr != _handles.end(); ++itr)
	{
#ifdef WIN32
		FreeLibrary(((HMODULE)*itr));
#else
		dlclose(*itr);
#endif
	}
	_handles.clear();
}

void ScriptMgr::register_creature_script(uint32 entry, exp_create_creature_ai callback)
{
	_creatures.insert( CreatureCreateMap::value_type( entry, callback ) );
}

void ScriptMgr::register_gameobject_script(uint32 entry, exp_create_gameobject_ai callback)
{
	_gameobjects.insert( GameObjectCreateMap::value_type( entry, callback ) );
}

void ScriptMgr::register_dummy_aura(uint32 entry, exp_handle_dummy_aura callback)
{
	_auras.insert( HandleDummyAuraMap::value_type( entry, callback ) );
}

void ScriptMgr::register_dummy_spell(uint32 entry, exp_handle_dummy_spell callback)
{
	_spells.insert( HandleDummySpellMap::value_type( entry, callback ) );
}

void ScriptMgr::register_gossip_script(uint32 entry, exp_create_gossip_script callback)
{
	_gossips.insert( GossipCreateMap::value_type( entry, callback ) );
}

CreatureAIScript* ScriptMgr::CreateAIScriptClassForEntry(Creature* pCreature)
{
	CreatureCreateMap::iterator itr = _creatures.find(pCreature->GetEntry());
	if(itr == _creatures.end())
		return NULL;

	exp_create_creature_ai function_ptr = itr->second;
	return (function_ptr)(pCreature);
}

GameObjectAIScript * ScriptMgr::CreateAIScriptClassForGameObject(uint32 uEntryId, GameObject* pGameObject)
{
	GameObjectCreateMap::iterator itr = _gameobjects.find(pGameObject->GetEntry());
	if(itr == _gameobjects.end())
		return NULL;

	exp_create_gameobject_ai function_ptr = itr->second;
	return (function_ptr)(pGameObject);
}

GossipScript * ScriptMgr::GetGossipScript(uint32 uEntryId)
{
	GossipCreateMap::iterator itr = _gossips.find(uEntryId);
	if(itr == _gossips.end())
	{
		return new GossipScript;
	}

	exp_create_gossip_script function_ptr = itr->second;
	return (function_ptr)();
}

bool ScriptMgr::CallScriptedDummySpell(uint32 uSpellId, uint32 i, Spell* pSpell)
{
	HandleDummySpellMap::iterator itr = _spells.find(uSpellId);
	if(itr == _spells.end())
		return false;

	exp_handle_dummy_spell function_ptr = itr->second;
	return (function_ptr)(i, pSpell);
}

bool ScriptMgr::CallScriptedDummyAura(uint32 uSpellId, uint32 i, Aura* pAura, bool apply)
{
	HandleDummyAuraMap::iterator itr = _auras.find(uSpellId);
	if(itr == _auras.end())
		return false;

	exp_handle_dummy_aura function_ptr = itr->second;
	return (function_ptr)(i, pAura, apply);
}

/* CreatureAI Stuff */
CreatureAIScript::CreatureAIScript(Creature* creature) : _unit(creature)
{

}

void CreatureAIScript::RegisterAIUpdateEvent(uint32 frequency)
{
	sEventMgr.AddEvent(_unit, &Creature::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0,0);
}

void CreatureAIScript::RemoveAIUpdateEvent()
{
	sEventMgr.RemoveEvents(_unit, EVENT_SCRIPT_UPDATE_EVENT);
}

/* GameObjectAI Stuff */

GameObjectAIScript::GameObjectAIScript(GameObject* goinstance) : _gameobject(goinstance)
{

}

void GameObjectAIScript::RegisterAIUpdateEvent(uint32 frequency)
{
	sEventMgr.AddEvent(_gameobject, &GameObject::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0,0);
}


/* InstanceAI Stuff */

InstanceScript::InstanceScript(MapMgr *instance) : _instance(instance)
{
}

/* QuestScript Stuff */

QuestScript::QuestScript(QuestLogEntry *qle) : _qLogEntry(qle)
{
}

void QuestScript::RegisterQuestEvent(uint32 frequency)
{
	sEventMgr.AddEvent(_qLogEntry, &QuestLogEntry::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0,0);
}

void QuestScript::RemoveQuestEvent()
{
	sEventMgr.RemoveEvents(_qLogEntry, EVENT_SCRIPT_UPDATE_EVENT);
}

/* Gossip Stuff*/

GossipScript::GossipScript()
{
	AutoCreated = false;
}

void GossipScript::GossipEnd(Creature* pCreature, Player* Plr)
{
	Plr->CleanupGossipMenu();
}

void GossipScript::GossipHello(Creature* pCreature, Player* Plr, bool AutoSend)
{
	GossipMenu *Menu;
	uint32 TextID = 2;
	Trainer *pTrainer = pCreature->GetTrainer();
	uint32 Text = objmgr.GetGossipTextForNpc(pCreature->GetEntry());
	if(Text != 0)
	{
		GossipText * text = NpcTextStorage.LookupEntry(Text);
		if(text != 0)
			TextID = Text;
	}

	objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), TextID, Plr);
	
	uint32 flags = pCreature->GetUInt32Value(UNIT_NPC_FLAGS);

	if(flags & UNIT_NPC_FLAG_VENDOR)
		Menu->AddItem(1, "I would like to browse your goods", 1);
	
	if(flags & UNIT_NPC_FLAG_TRAINER && pTrainer != 0)
	{
		string name = pCreature->GetCreatureName()->Name;
		string::size_type pos = name.find(" ");	  // only take first name
		if(pos != string::npos)
			name = name.substr(0, pos);

		string msg = "I seek ";

		if(pTrainer->RequiredClass && pTrainer->RequiredClass != Plr->getClass())
		{
			//							   WARRIOR   PALADIN HUNTER  ROGUE   PRIEST  -   SHAMAN  MAGE   WARLOCK  - DRUID
			uint32 notclass[13] =   {0	  ,5721	 ,3976   ,5839   ,4797   ,4435   ,0  ,5006   ,539	,5836   ,0,4774};
			Menu->SetTextID(notclass[pTrainer->RequiredClass]);
		} else if(pTrainer->RequiredClass) {
			//							   WARRIOR   PALADIN HUNTER  ROGUE   PRIEST  -   SHAMAN  MAGE   WARLOCK  - DRUID
			uint32 isclass[13] =	{0	  ,1040	 ,3974   ,4864   ,4835   ,4436   ,0  ,5005   ,538	,5835   ,0,4775};
			Menu->SetTextID(isclass[pTrainer->RequiredClass]);

			switch(Plr->getClass())
			{
			case MAGE:
				msg += "mage";
				break;
			case SHAMAN:
				msg += "shaman";
				break;
			case WARRIOR:
				msg += "warrior";
				break;
			case PALADIN:
				msg += "paladin";
				break;
			case WARLOCK:
				msg += "warlock";
				break;
			case HUNTER:
				msg += "hunter";
				break;
			case ROGUE:
				msg += "rogue";
				break;
			case DRUID:
				msg += "druid";
				break;
			case PRIEST:
				msg += "priest";
				break;
			}
			msg += " training, ";
			msg += name;
			msg += ".";

			Menu->AddItem(3, msg.c_str(), 2);

		}
		else
		{
			msg += "training, ";
			msg += name;
			msg += ".";

			Menu->AddItem(3, msg.c_str(), 2);
		}
	}

	if(flags & UNIT_NPC_FLAG_TAXIVENDOR)
		Menu->AddItem(2, "Give me a ride.", 3);

	if(flags & UNIT_NPC_FLAG_AUCTIONEER)
		Menu->AddItem(0, "I would like to make a bid.", 4);

	if(flags & UNIT_NPC_FLAG_INNKEEPER)
		Menu->AddItem(5, "Make this inn your home.", 5);

	if(flags & UNIT_NPC_FLAG_BANKER)
		Menu->AddItem(0, "I would like to check my deposit box.", 6);

	if(flags & UNIT_NPC_FLAG_SPIRITHEALER)
		Menu->AddItem(0, "Bring me back to life.", 7);

	if(flags & UNIT_NPC_FLAG_PETITIONER)
		Menu->AddItem(0, "How do I create a guild?", 8);

	if(flags & UNIT_NPC_FLAG_TABARDCHANGER)
		Menu->AddItem(0, "I want to create a guild crest.", 9);

	if(flags & UNIT_NPC_FLAG_BATTLEFIELDPERSON)
		Menu->AddItem(0, "I would like to go to the battleground.", 10);

	if( pTrainer &&
		pTrainer->RequiredClass &&					  // class trainer
		pTrainer->RequiredClass == Plr->getClass() &&   // correct class
		pCreature->getLevel() > 10 &&				   // creature level
		Plr->getLevel() > 10 )						  // player level
	{
		Menu->AddItem(0, "I would like to reset my talents.", 11);
	}

	if(AutoSend)
		Menu->SendTo(Plr);
}

void GossipScript::GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
{
	switch(IntId)
	{
	case 1:
		// vendor
		Plr->GetSession()->SendInventoryList(pCreature);
		break;
	case 2:
		// trainer
		Plr->GetSession()->SendTrainerList(pCreature);
		break;
	case 3:
		// taxi
		Plr->GetSession()->SendTaxiList(pCreature);
		break;
	case 4:
		// auction
		Plr->GetSession()->SendAuctionList(pCreature);
		break;
	case 5:
		// innkeeper
		Plr->GetSession()->SendInnkeeperBind(pCreature);
		break;
	case 6:
		// banker
		Plr->GetSession()->SendBankerList(pCreature);
		break;
	case 7:
		// spirit
		Plr->GetSession()->SendSpiritHealerRequest(pCreature);
		break;
	case 8:
		// petition
		Plr->GetSession()->SendCharterRequest(pCreature);
		break;
	case 9:
		// guild crest
		Plr->GetSession()->SendTabardHelp(pCreature);
		break;
	case 10:
		// battlefield
		Plr->GetSession()->SendBattlegroundList(pCreature, 2);
		break;
	case 11:
		// switch to talent reset message
		{
			GossipMenu *Menu;
			objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 5674, Plr);
			Menu->AddItem(0, "I understand, continue.", 12);
			Menu->SendTo(Plr);
		}break;
	case 12:
		// talent reset
		{
			Plr->Gossip_Complete();
			Plr->SendTalentResetConfirm();
		}break;

	default:
		sLog.outError("Unknown IntId %u on entry %u", IntId, pCreature->GetEntry());
		break;
	}	
}

void GossipScript::Destroy()
{
	delete this;
}

void ScriptMgr::register_hook(ServerHookEvents event, void * function_pointer)
{
	ASSERT(event < NUM_SERVER_HOOKS);
	_hooks[event].push_back(function_pointer);
}

/* Hook Implementations */
#define OUTER_LOOP_BEGIN(type, fptr_type) if(!sScriptMgr._hooks[type].size()) { \
	return; } \
	fptr_type call; \
	for(ServerHookList::iterator itr = sScriptMgr._hooks[type].begin(); itr != sScriptMgr._hooks[type].end(); ++itr) { \
	call = ((fptr_type)*itr);

#define OUTER_LOOP_END }

#define OUTER_LOOP_BEGIN_COND(type, fptr_type) if(!sScriptMgr._hooks[type].size()) { \
	return true; } \
	fptr_type call; \
	bool ret_val = true; \
	for(ServerHookList::iterator itr = sScriptMgr._hooks[type].begin(); itr != sScriptMgr._hooks[type].end(); ++itr) { \
		call = ((fptr_type)*itr);

#define OUTER_LOOP_END_COND } return ret_val;

bool HookInterface::OnNewCharacter(uint32 Race, uint32 Class, WorldSession * Session, const char * Name)
{
	OUTER_LOOP_BEGIN_COND(SERVER_HOOK_EVENT_ON_NEW_CHARACTER, tOnNewCharacter)
		ret_val = (call)(Race, Class, Session, Name);
	OUTER_LOOP_END_COND
}

void HookInterface::OnKillPlayer(Player * pPlayer, Player * pVictim)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_KILL_PLAYER, tOnKillPlayer)
		(call)(pPlayer, pVictim);
	OUTER_LOOP_END
}

void HookInterface::OnFirstEnterWorld(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_FIRST_ENTER_WORLD, tOnFirstEnterWorld)
		(call)(pPlayer);
	OUTER_LOOP_END
}

void HookInterface::OnEnterWorld(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ENTER_WORLD, tOnEnterWorld)
		(call)(pPlayer);
	OUTER_LOOP_END
}

void HookInterface::OnGuildCreate(Player * pLeader, Guild * pGuild)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_GUILD_CREATE, tOnGuildCreate)
		(call)(pLeader, pGuild);
	OUTER_LOOP_END
}

void HookInterface::OnGuildJoin(Player * pPlayer, Guild * pGuild)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_GUILD_JOIN, tOnGuildJoin)
		(call)(pPlayer, pGuild);
	OUTER_LOOP_END
}

void HookInterface::OnDeath(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_DEATH, tOnDeath)
		(call)(pPlayer);
	OUTER_LOOP_END
}

bool HookInterface::OnRepop(Player * pPlayer)
{
	OUTER_LOOP_BEGIN_COND(SERVER_HOOK_EVENT_ON_REPOP, tOnRepop)
		ret_val = (call)(pPlayer);
	OUTER_LOOP_END_COND
}

void HookInterface::OnEmote(Player * pPlayer, uint32 Emote)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_EMOTE, tOnEmote)
		(call)(pPlayer, Emote);
	OUTER_LOOP_END
}

void HookInterface::OnEnterCombat(Player * pPlayer, Unit * pTarget)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ENTER_COMBAT, tOnEnterCombat)
		(call)(pPlayer, pTarget);
	OUTER_LOOP_END
}

bool HookInterface::OnCastSpell(Player * pPlayer, SpellEntry* pSpell)
{
	OUTER_LOOP_BEGIN_COND(SERVER_HOOK_EVENT_ON_CAST_SPELL, tOnCastSpell)
		ret_val = (call)(pPlayer, pSpell);
	OUTER_LOOP_END_COND
}

bool HookInterface::OnLogoutRequest(Player * pPlayer)
{
	OUTER_LOOP_BEGIN_COND(SERVER_HOOK_EVENT_ON_LOGOUT, tOnLogoutRequest)
		ret_val = (call)(pPlayer);
	OUTER_LOOP_END_COND
}

void HookInterface::OnQuestAccept(Player * pPlayer, Quest * pQuest)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_QUEST_ACCEPT, tOnQuestAccept)
		(call)(pPlayer, pQuest);
	OUTER_LOOP_END
}

void HookInterface::OnZone(Player * pPlayer, uint32 Zone)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ZONE, tOnZone)
		(call)(pPlayer, Zone);
	OUTER_LOOP_END
}

void HookInterface::OnChat(Player * pPlayer, uint32 Type, uint32 Lang, const char * Message, const char * Misc)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_CHAT, tOnChat)
		(call)(pPlayer, Type, Lang, Message, Misc);
	OUTER_LOOP_END
}

void HookInterface::OnLoot(Player * pPlayer, Unit * pTarget, uint32 Money, uint32 ItemId)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_LOOT, tOnLoot)
		(call)(pPlayer, pTarget, Money, ItemId);
	OUTER_LOOP_END
}

void HookInterface::OnEnterWorld2(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ENTER_WORLD_2, tOnEnterWorld)
		(call)(pPlayer);
	OUTER_LOOP_END
}
