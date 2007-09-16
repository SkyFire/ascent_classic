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

#ifndef SCRIPTMGR_H
#define SCRIPTMGR_H

#define SCRIPTLIB_VERSION_MAJOR 3
#define SCRIPTLIB_VERSION_MINOR 0
#define MAKE_SCRIPT_VERSION(major, minor) (uint32)(((uint16)major << 16) | ((uint16)minor))
#define SCRIPT_MODULE void*
#define ADD_CREATURE_FACTORY_FUNCTION(cl) static CreatureAIScript * Create(Creature * c) { return new cl(c); }
#define ADD_GOSSIP_FACTORY_FUNCTION(cl) static GossipScript * Create() { return new cl; }

class Channel;
class Guild;
struct Quest;
enum ServerHookEvents
{
	SERVER_HOOK_EVENT_ON_NEW_CHARACTER		= 1,
	SERVER_HOOK_EVENT_ON_KILL_PLAYER		= 2,
	SERVER_HOOK_EVENT_ON_FIRST_ENTER_WORLD	= 3,
	SERVER_HOOK_EVENT_ON_ENTER_WORLD		= 4,
	SERVER_HOOK_EVENT_ON_GUILD_JOIN			= 5,
	SERVER_HOOK_EVENT_ON_DEATH				= 6,
	SERVER_HOOK_EVENT_ON_REPOP				= 7,
	SERVER_HOOK_EVENT_ON_EMOTE				= 8,
	SERVER_HOOK_EVENT_ON_ENTER_COMBAT		= 9,
	SERVER_HOOK_EVENT_ON_CAST_SPELL			= 10,
	SERVER_HOOK_EVENT_ON_TICK				= 11,
	SERVER_HOOK_EVENT_ON_LOGOUT_REQUEST		= 12,
	SERVER_HOOK_EVENT_ON_LOGOUT				= 13,
	SERVER_HOOK_EVENT_ON_QUEST_ACCEPT		= 14,
	SERVER_HOOK_EVENT_ON_ZONE				= 15,
	SERVER_HOOK_EVENT_ON_CHAT				= 16,
	SERVER_HOOK_EVENT_ON_LOOT				= 17,
	SERVER_HOOK_EVENT_ON_GUILD_CREATE		= 18,
	SERVER_HOOK_EVENT_ON_ENTER_WORLD_2		= 19,

	NUM_SERVER_HOOKS,
};

/* Hook typedefs */
typedef bool(*tOnNewCharacter)(uint32 Race, uint32 Class, WorldSession * Session, const char * Name);
typedef void(*tOnKillPlayer)(Player * pPlayer, Player * pVictim);
typedef void(*tOnFirstEnterWorld)(Player * pPlayer);
typedef void(*tOnEnterWorld)(Player * pPlayer);
typedef void(*tOnGuildCreate)(Player * pLeader, Guild * pGuild);
typedef void(*tOnGuildJoin)(Player * pPlayer, Guild * pGuild);
typedef void(*tOnDeath)(Player * pPlayer);
typedef bool(*tOnRepop)(Player * pPlayer);
typedef void(*tOnEmote)(Player * pPlayer, uint32 Emote);
typedef void(*tOnEnterCombat)(Player * pPlayer, Unit * pTarget);
typedef bool(*tOnCastSpell)(Player * pPlayer, SpellEntry * pSpell);
typedef void(*tOnTick)();
typedef bool(*tOnLogoutRequest)(Player * pPlayer);
typedef void(*tOnQuestAccept)(Player * pPlayer, Quest * pQuest);
typedef void(*tOnZone)(Player * pPlayer, uint32 Zone);
typedef void(*tOnChat)(Player * pPlayer, uint32 Type, uint32 Lang, const char * Message, const char * Misc);
typedef void(*tOnLoot)(Player * pPlayer, Unit * pTarget, uint32 Money, uint32 ItemId);

class Spell;
class Aura;
class Creature;
class CreatureAIScript;
class GossipScript;
class GameObjectAIScript;
class ScriptMgr;
struct ItemPrototype;
class QuestLogEntry;

/* Factory Imports (from script lib) */
typedef CreatureAIScript*(*exp_create_creature_ai)(Creature * pCreature);
typedef GameObjectAIScript*(*exp_create_gameobject_ai)(GameObject * pGameObject);
typedef GossipScript*(*exp_create_gossip_script)();
typedef bool(*exp_handle_dummy_spell)(uint32 i, Spell * pSpell);
typedef bool(*exp_handle_dummy_aura)(uint32 i, Aura * pAura, bool apply);
typedef void(*exp_script_register)(ScriptMgr * mgr);

typedef uint32(*exp_get_version)();

/* Hashmap typedefs */
typedef HM_NAMESPACE::hash_map<uint32, exp_create_creature_ai> CreatureCreateMap;
typedef HM_NAMESPACE::hash_map<uint32, exp_create_gameobject_ai> GameObjectCreateMap;
typedef HM_NAMESPACE::hash_map<uint32, exp_create_gossip_script> GossipCreateMap;
typedef HM_NAMESPACE::hash_map<uint32, exp_handle_dummy_aura> HandleDummyAuraMap;
typedef HM_NAMESPACE::hash_map<uint32, exp_handle_dummy_spell> HandleDummySpellMap;
typedef list<void*> ServerHookList;
typedef list<SCRIPT_MODULE> LibraryHandleMap;

class SERVER_DECL ScriptMgr : public Singleton<ScriptMgr>
{
public:

	friend class HookInterface;

	void LoadScripts();
	void UnloadScripts();

	CreatureAIScript * CreateAIScriptClassForEntry(Creature* pCreature);
	GameObjectAIScript * CreateAIScriptClassForGameObject(uint32 uEntryId, GameObject* pGameObject);

	bool CallScriptedDummySpell(uint32 uSpellId, uint32 i, Spell* pSpell);
	bool CallScriptedDummyAura( uint32 uSpellId, uint32 i, Aura* pAura, bool apply);

	GossipScript* GetGossipScript(uint32 uEntryId);

	void register_creature_script(uint32 entry, exp_create_creature_ai callback);
	void register_gameobject_script(uint32 entry, exp_create_gameobject_ai callback);
	void register_gossip_script(uint32 entry, exp_create_gossip_script callback);
	void register_dummy_aura(uint32 entry, exp_handle_dummy_aura callback);
	void register_dummy_spell(uint32 entry, exp_handle_dummy_spell callback);
	void register_hook(ServerHookEvents event, void * function_pointer);

protected:
	CreatureCreateMap _creatures;
	GameObjectCreateMap _gameobjects;
	GossipCreateMap _gossips;
	HandleDummyAuraMap _auras;
	HandleDummySpellMap _spells;
	LibraryHandleMap _handles;
	ServerHookList _hooks[NUM_SERVER_HOOKS];
};

class SERVER_DECL CreatureAIScript
{
public:
	CreatureAIScript(Creature* creature);
	virtual ~CreatureAIScript() {};

	virtual void OnCombatStart(Unit* mTarget) {}
	virtual void OnCombatStop(Unit* mTarget) {}
	virtual void OnDamageTaken(Unit* mAttacker, float fAmount) {}
	virtual void OnCastSpell(uint32 iSpellId) {}
	virtual void OnTargetParried(Unit* mTarget) {}
	virtual void OnTargetDodged(Unit* mTarget) {}
	virtual void OnTargetBlocked(Unit* mTarget, int32 iAmount) {}
	virtual void OnTargetCritHit(Unit* mTarget, float fAmount) {}
	virtual void OnTargetDied(Unit* mTarget) {}
	virtual void OnParried(Unit* mTarget) {}
	virtual void OnDodged(Unit* mTarget) {}
	virtual void OnBlocked(Unit* mTarget, int32 iAmount) {}
	virtual void OnCritHit(Unit* mTarget, float fAmount) {}
	virtual void OnHit(Unit* mTarget, float fAmount) {}
	virtual void OnDied(Unit *mKiller) {}
	virtual void OnAssistTargetDied(Unit* mAssistTarget) {}
	virtual void OnFear(Unit* mFeared, uint32 iSpellId) {}
	virtual void OnFlee(Unit* mFlee) {}
	virtual void OnCallForHelp() {}
	virtual void OnLoad() {}
	virtual void OnReachWP(uint32 iWaypointId, bool bForwards) {}
	virtual void OnLootTaken(Player* pPlayer, ItemPrototype *pItemPrototype) {}
	virtual void AIUpdate() {}

	void RegisterAIUpdateEvent(uint32 frequency);
	void RemoveAIUpdateEvent();

	virtual void Destroy() {}
	Creature* GetUnit() { return _unit; }

protected:
	Creature* _unit;
};

class SERVER_DECL GameObjectAIScript
{
public:
	GameObjectAIScript(GameObject* goinstance);
	virtual ~GameObjectAIScript() {}

	virtual void OnCreate() {}
	virtual void OnSpawn() {}
	virtual void OnDespawn() {}
	virtual void OnLootTaken(Player * pLooter, ItemPrototype *pItemInfo) {}
	virtual void OnActivate(Player * pPlayer) {}
	virtual void AIUpdate() {}
	virtual void Destroy() {}

	void RegisterAIUpdateEvent(uint32 frequency);

protected:

	GameObject* _gameobject;
};

class SERVER_DECL GossipScript
{
public:
	GossipScript();
	virtual ~GossipScript() {} 

	virtual void GossipHello(Creature* pCreature, Player* Plr, bool AutoSend);
	virtual void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId);
	virtual void GossipEnd(Creature* pCreature, Player* Plr);
	virtual void Destroy();

	bool AutoCreated;
};

class SERVER_DECL QuestScript
{
public:
	QuestScript(QuestLogEntry* qle);
	virtual ~QuestScript() {};

	virtual void OnQuestStart(Player* mTarget) {}
	virtual void OnQuestComplete(Player* mTarget) {}
	virtual void OnQuestCancel(Player* mTarget) {}
	virtual void OnGameObjectActivate(uint32 entry, Player* mTarget) {}
	virtual void OnCreatureKill(uint32 entry, Player* mTarget) {}
	virtual void OnExploreArea(uint32 areaId, Player* mTarget) {}
	virtual void OnPlayerItemPickup(uint32 itemId, uint32 totalCount, Player* mTarget) {}
	virtual void EventUpdate() {};

	void RegisterQuestEvent(uint32 frequency);
	void RemoveQuestEvent();

	virtual void Destroy() {}

protected:
	QuestLogEntry *_qLogEntry;
};

class SERVER_DECL HookInterface : public Singleton<HookInterface>
{
public:
	friend class ScriptMgr;

	bool OnNewCharacter(uint32 Race, uint32 Class, WorldSession * Session, const char * Name);
	void OnKillPlayer(Player * pPlayer, Player * pVictim);
	void OnFirstEnterWorld(Player * pPlayer);
	void OnEnterWorld(Player * pPlayer);
	void OnGuildCreate(Player * pLeader, Guild * pGuild);
	void OnGuildJoin(Player * pPlayer, Guild * pGuild);
	void OnDeath(Player * pPlayer);
	bool OnRepop(Player * pPlayer);
	void OnEmote(Player * pPlayer, uint32 Emote);
	void OnEnterCombat(Player * pPlayer, Unit * pTarget);
	bool OnCastSpell(Player * pPlayer, SpellEntry * pSpell);
	bool OnLogoutRequest(Player * pPlayer);
	void OnQuestAccept(Player * pPlayer, Quest * pQuest);
	void OnZone(Player * pPlayer, uint32 Zone);
	void OnChat(Player * pPlayer, uint32 Type, uint32 Lang, const char * Message, const char * Misc);
	void OnLoot(Player * pPlayer, Unit * pTarget, uint32 Money, uint32 ItemId);
	void OnEnterWorld2(Player * pPlayer);
};

#define sScriptMgr ScriptMgr::getSingleton()
#define sHookInterface HookInterface::getSingleton()

#endif
