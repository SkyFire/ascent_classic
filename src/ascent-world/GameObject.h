/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2008 Ascent Team <http://www.ascentemu.com/>
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

#ifndef WOWSERVER_GAMEOBJECT_H
#define WOWSERVER_GAMEOBJECT_H

class Player;
class GameObjectAIScript;
class GameObjectTemplate;

#pragma pack(push,1)
struct GameObjectInfo
{
	uint32 ID;
	uint32 Type;
	uint32 DisplayID;
	char * Name;
	uint32 SpellFocus;
	uint32 sound1;
	uint32 sound2;
	uint32 sound3;
	uint32 sound4;
	uint32 sound5;
	uint32 sound6;
	uint32 sound7;
	uint32 sound8;
	uint32 sound9;
	uint32 Unknown1;
	uint32 Unknown2;
	uint32 Unknown3;
	uint32 Unknown4;
	uint32 Unknown5;
	uint32 Unknown6;
	uint32 Unknown7;
	uint32 Unknown8;
	uint32 Unknown9;
	uint32 Unknown10;
	uint32 Unknown11;
	uint32 Unknown12;
	uint32 Unknown13;
	uint32 Unknown14;
	uint32 *InvolvedQuestIds;
	uint32 InvolvedQuestCount;
};
#pragma pack(pop)

enum GAMEOBJECT_TYPES
{
	GAMEOBJECT_TYPE_DOOR			   = 0,
	GAMEOBJECT_TYPE_BUTTON			 = 1,
	GAMEOBJECT_TYPE_QUESTGIVER		 = 2,
	GAMEOBJECT_TYPE_CHEST			  = 3,
	GAMEOBJECT_TYPE_BINDER			 = 4,
	GAMEOBJECT_TYPE_GENERIC			= 5,
	GAMEOBJECT_TYPE_TRAP			   = 6,
	GAMEOBJECT_TYPE_CHAIR			  = 7,
	GAMEOBJECT_TYPE_SPELL_FOCUS		= 8,
	GAMEOBJECT_TYPE_TEXT			   = 9,
	GAMEOBJECT_TYPE_GOOBER			 = 10,
	GAMEOBJECT_TYPE_TRANSPORT		  = 11,
	GAMEOBJECT_TYPE_AREADAMAGE		 = 12,
	GAMEOBJECT_TYPE_CAMERA			 = 13,
	GAMEOBJECT_TYPE_MAP_OBJECT		 = 14,
	GAMEOBJECT_TYPE_MO_TRANSPORT	   = 15,
	GAMEOBJECT_TYPE_DUEL_ARBITER	   = 16,
	GAMEOBJECT_TYPE_FISHINGNODE		= 17,
	GAMEOBJECT_TYPE_RITUAL			 = 18,
	GAMEOBJECT_TYPE_MAILBOX			= 19,
	GAMEOBJECT_TYPE_AUCTIONHOUSE	   = 20,
	GAMEOBJECT_TYPE_GUARDPOST		  = 21,
	GAMEOBJECT_TYPE_SPELLCASTER		= 22,
	GAMEOBJECT_TYPE_MEETINGSTONE	   = 23,
	GAMEOBJECT_TYPE_FLAGSTAND		  = 24,
	GAMEOBJECT_TYPE_FISHINGHOLE		= 25,
	GAMEOBJECT_TYPE_FLAGDROP		   = 26,
};

enum GameObjectFlags
{
	GO_FLAG_IN_USE          = 0x01,                         //disables interaction while animated
	GO_FLAG_LOCKED          = 0x02,                         //require key, spell, event, etc to be opened. Makes "Locked" appear in tooltip
	GO_FLAG_INTERACT_COND   = 0x04,                         //cannot interact (condition to interact)
	GO_FLAG_TRANSPORT       = 0x08,                         //any kind of transport? Object can transport (elevator, boat, car)
	GO_FLAG_UNK1            = 0x10,                         //
	GO_FLAG_NODESPAWN       = 0x20,                         //never despawn, typically for doors, they just change state
	GO_FLAG_TRIGGERED       = 0x40,                         //typically, summoned objects. Triggered by spell or other events
};

enum GameObjectDynFlags
{
	GO_DYNFLAG_QUEST		= 0x09,
};

#define CALL_GO_SCRIPT_EVENT(obj, func) if(obj->GetTypeId() == TYPEID_GAMEOBJECT && static_cast<GameObject*>(obj)->GetScript() != NULL) static_cast<GameObject*>(obj)->GetScript()->func

class SERVER_DECL GameObject : public Object
{
public:
	/************************************************************************/
	/* LUA Stuff                                                            */
	/************************************************************************/
/*	typedef struct { const char *name; int(*mfunc)(lua_State*,GameObject*); } RegType;
	static const char className[];
	static RegType methods[];

	// a lua script cannot create a unit.
	GameObject(lua_State * L) { ASSERT(false); }*/

	GameObject(uint64 guid);
	~GameObject( );

	ASCENT_INLINE GameObjectInfo* GetInfo() { return pInfo; }
	ASCENT_INLINE void SetInfo(GameObjectInfo * goi) { pInfo = goi; }

	//void Create ( uint32 display_id, uint8 state, uint32 obj_field_entry, float scale, uint16 type, uint16 faction, uint32 mapid, float x, float y, float z, float ang );
   // void Create ( uint32 mapid, float x, float y, float z, float ang);
	bool CreateFromProto(uint32 entry,uint32 mapid, float x, float y, float z, float ang);
   
	bool Load(GOSpawn *spawn);

	virtual void Update(uint32 p_time);

	void Spawn(MapMgr * m);
	void Despawn(uint32 time);

	//void _EnvironmentalDamageUpdate();
	void UpdateTrapState();
	// Serialization
	void SaveToDB();
	void SaveToFile(std::stringstream & name);
	//bool LoadFromDB(uint32 guid);
	//void LoadFromDB(GameObjectTemplate *t);
	void DeleteFromDB();
	void EventCloseDoor();

	//Fishing stuff
	void UseFishingNode(Player *player);
	void EndFishing(Player* player,bool abort);
	void FishHooked(Player * player);
	
	// Quests
	void _LoadQuests();
	bool HasQuests() { return m_quests != NULL; };
	void AddQuest(QuestRelation *Q);
	void DeleteQuest(QuestRelation *Q);
	Quest *FindQuest(uint32 quest_id, uint8 quest_relation);
	uint16 GetQuestRelation(uint32 quest_id);
	uint32 NumOfQuests();
	std::list<QuestRelation *>::iterator QuestsBegin() { return m_quests->begin(); };
	std::list<QuestRelation *>::iterator QuestsEnd() { return m_quests->end(); };
	void SetQuestList(std::list<QuestRelation *>* qst_lst) { m_quests = qst_lst; };

	void SetSummoned(Unit *mob)
	{
		m_summoner = mob;
		m_summonedGo = true;
	}
	void _Expire();
	
	void ExpireAndDelete();

	ASCENT_INLINE bool isQuestGiver()
	{
		if(m_uint32Values[GAMEOBJECT_TYPE_ID] == 2)
			return true;
		else
			return false;
	};

	/// Quest data
	std::list<QuestRelation *>* m_quests;
   
	uint32 *m_ritualmembers;
	uint32 m_ritualcaster,m_ritualtarget;
	uint16 m_ritualspell;

	void InitAI();
	SpellEntry* spell;
	
	float range;
	uint8 checkrate;
	uint16 counter;
	int32 charges;//used for type==22,to limit number of usages.
	bool invisible;//invisible
	uint8 invisibilityFlag;
	Unit* m_summoner;
	int8 bannerslot;
	int8 bannerauraslot;
	CBattleground * m_battleground;

	void CallScriptUpdate();
   

	ASCENT_INLINE GameObjectAIScript* GetScript() { return myScript; }

	void TrapSearchTarget();	// Traps need to find targets faster :P

	ASCENT_INLINE bool HasAI() { return spell != 0; }
	GOSpawn * m_spawn;
	void OnPushToWorld();
	void OnRemoveInRangeObject(Object* pObj);
	void RemoveFromWorld(bool free_guid);

	ASCENT_INLINE bool CanMine(){return (mines_remaining > 0);}
	ASCENT_INLINE void UseMine(){ if(mines_remaining) mines_remaining--;}
	void CalcMineRemaining(bool force)
	{
		if(force || !mines_remaining)
			mines_remaining = GetInfo()->sound4 + RandomUInt(GetInfo()->sound5 - GetInfo()->sound4) - 1;
	}

	uint32 GetGOReqSkill();
	MapCell * m_respawnCell;

	ASCENT_INLINE void SetScript(GameObjectAIScript *pScript) { myScript = pScript; }

	// loooot
	void GenerateLoot();

protected:

	bool m_summonedGo;
	bool m_deleted;
	GameObjectInfo *pInfo;
	GameObjectAIScript * myScript;
	uint32 _fields[GAMEOBJECT_END];
	uint32 mines_remaining; //used for mining to mark times it can be mined

};

#endif

