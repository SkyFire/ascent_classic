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

#ifndef WOWSERVER_CREATURE_H
#define WOWSERVER_CREATURE_H

class CreatureTemplate;

#define MAX_CREATURE_ITEMS 128
#define MAX_CREATURE_LOOT 8
#define MAX_PET_SPELL 4
#include "Map.h"

struct CreatureItem
{
	uint32 itemid;
	int amount;
};

struct CreatureInfo
{
	uint32 Id;
	char * Name;
	char * SubName;
	uint32 Flags1;
	uint32 Type;
	uint32 Family;
	uint32 Rank;
	uint32 Unknown1;
	uint32 SpellDataID;
	uint32 DisplayID;
	float unk2;
	float unk3;
	uint8  Civilian;
	uint8  Leader;

	std::string lowercase_name;
};

struct CreatureProto
{
	uint32 Id;
	uint32 MinLevel;
	uint32 MaxLevel;
	uint32 Faction;
	uint32 MinHealth;
	uint32 MaxHealth;
	uint32 Mana;
	float  Scale;
	uint32	NPCFLags;
	uint32 AttackTime;
	float MinDamage;
	float MaxDamage;
	uint32 RangedAttackTime;
	float RangedMinDamage;
	float RangedMaxDamage;
	uint32 MountedDisplayID;
	uint32 Item1SlotDisplay;
	uint32 Item1Info1;
	uint32 Item1Info2;
	uint32 Item2SlotDisplay;
	uint32 Item2Info1;
	uint32 Item2Info2;
	uint32 Item3SlotDisplay;
	uint32 Item3Info1;
	uint32 Item3Info2;
	uint32 RespawnTime;
	uint32 Resistances[7];
	float CombatReach;
	float BoundingRadius;
	char * aura_string;
	uint32 boss;
	uint32 money;
	uint32 invisibility_type;
	uint32 death_state;

	/* AI Stuff */
	bool m_canRangedAttack;
	bool m_canFlee;
	float m_fleeHealth;
	uint32 m_fleeDuration;
	bool m_canCallForHelp;
	float m_callForHelpHealth;

	set<uint32> start_auras;
	list<AI_Spell*> spells;
};

struct Formation{
	uint32 fol;
	float ang;
	float dist;
};
 
   
enum UNIT_TYPE
{
	NOUNITTYPE	  = 0,
	BEAST		   = 1,
	DRAGONSKIN	  = 2,
	DEMON		   = 3,
	ELEMENTAL	   = 4,
	GIANT		   = 5,
	UNDEAD		  = 6,
	HUMANOID		= 7,
	CRITTER		 = 8,
	MECHANICAL	  = 9,
	UNIT_TYPE_MISC  = 10,
};

enum FAMILY
{
	FAMILY_WOLF = 1,
	FAMILY_CAT,
	FAMILY_SPIDER,
	FAMILY_BEAR,
	FAMILY_BOAR,
	FAMILY_CROCILISK,
	FAMILY_CARRION_BIRD,
	FAMILY_CRAB,
	FAMILY_GORILLA,
	FAMILY_RAPTOR = 11,
	FAMILY_TALLSTRIDER ,
	FAMILY_FELHUNTER = 15,
	FAMILY_VOIDWALKER,
	FAMILY_SUCCUBUS,
	FAMILY_DOOMGUARD = 19,
	FAMILY_SCORPID,
	FAMILY_TURTLE, 
	FAMILY_IMP = 23,
	FAMILY_BAT,
	FAMILY_HYENA,
	FAMILY_OWL,
	FAMILY_WIND_SERPENT,
	FAMILY_REMOTE_CONTROL,
	FAMILY_FELGUARD,
	FAMILY_DRAGONHAWK,
	FAMILY_RAVAGER,
	FAMILY_WARP_STALKER,
	FAMILY_SPOREBAT,
	FAMILY_NETHER_RAY,
	FAMILY_SERPENT
};

enum ELITE
{
	ELITE_NORMAL = 0,
	ELITE_ELITE,
	ELITE_RAREELITE,
	ELITE_WORLDBOSS,
	ELITE_RARE
};

struct PetSpellCooldown
{
	uint32 spellId;
	int32 cooldown;
};

class CreatureAIScript;
class GossipScript;
class AuctionHouse;
struct Trainer;
#define CALL_SCRIPT_EVENT(obj, func) if(obj->GetTypeId() == TYPEID_UNIT && static_cast<Creature*>(obj)->GetScript() != NULL) static_cast<Creature*>(obj)->GetScript()->func

///////////////////
/// Creature object

class SERVER_DECL Creature : public Unit
{
public:

	Creature(uint32 high, uint32 low);
	virtual ~Creature();
    bool Load(CreatureSpawn *spawn, uint32 mode, MapInfo *info);
	void Load(CreatureProto * proto_, float x, float y, float z);

	void AddToWorld();
	void RemoveFromWorld(bool addrespawnevent);

	/// Creation
	void Create ( const char* creature_name, uint32 mapid, float x, float y, float z, float ang);	
	void CreateWayPoint ( uint32 WayPointID, uint32 mapid, float x, float y, float z, float ang);
	

	/// Updates
	virtual void Update( uint32 time );

	/// Creature inventory
	inline uint32 GetItemIdBySlot(uint32 slot) { return m_SellItems->at(slot).itemid; }
	inline uint32 GetItemAmountBySlot(uint32 slot) { return m_SellItems->at(slot).amount; }

	inline bool HasItems() { return ((m_SellItems != NULL) ? true : false); }

	int32 GetSlotByItemId(uint32 itemid)
	{
		uint32 slot = 0;
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
		{
			if(itr->itemid == itemid)
				return slot;
			else
				++slot;			
		}
		return -1;
	}

	uint32 GetItemAmountByItemId(uint32 itemid)
	{
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
		{
			if(itr->itemid == itemid)
				return ((itr->amount < 1) ? 1 : itr->amount);
		}
		return 0;
	}

	inline void GetSellItemBySlot(uint32 slot, CreatureItem &ci)
	{
		ci = m_SellItems->at(slot);
	}

	void GetSellItemByItemId(uint32 itemid, CreatureItem &ci)
	{
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
		{
			if(itr->itemid == itemid)
			{
				ci = (*itr);   
				return;
			}
		}
		ci.amount = 0;
		ci.itemid = 0;
	}

	inline std::vector<CreatureItem>::iterator GetSellItemBegin() { return m_SellItems->begin(); }
	inline std::vector<CreatureItem>::iterator GetSellItemEnd()   { return m_SellItems->end(); }
	inline uint32 GetSellItemCount() { return m_SellItems->size(); }
	
	void RemoveVendorItem(uint32 itemid)
	{
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
		{
			if(itr->itemid == itemid)
			{
				m_SellItems->erase(itr);
				return;
			}
		}
	}

	void AddVendorItem(uint32 itemid, uint32 amount);
	/// Quests
	void _LoadQuests();
	bool HasQuests() { return m_quests != NULL; };
	bool HasQuest(uint32 id, uint32 type)
	{
		if(!m_quests) return false;
		for(std::list<QuestRelation*>::iterator itr = m_quests->begin(); itr != m_quests->end(); ++itr)
		{
			if((*itr)->qst->id == id && (*itr)->type & type)
				return true;
		}
		return false;
	}
	void AddQuest(QuestRelation *Q);
	void DeleteQuest(QuestRelation *Q);
	Quest *FindQuest(uint32 quest_id, uint8 quest_relation);
	uint16 GetQuestRelation(uint32 quest_id);
	uint32 NumOfQuests();
	list<QuestRelation *>::iterator QuestsBegin() { return m_quests->begin(); };
	list<QuestRelation *>::iterator QuestsEnd() { return m_quests->end(); };
	void SetQuestList(std::list<QuestRelation *>* qst_lst) { m_quests = qst_lst; };

	inline bool isQuestGiver() { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER ); };
	
	int32 FlatResistanceMod[7];
	int32 BaseResistanceModPct[7];
	int32 ResistanceModPct[7];
	
	int32 FlatStatMod[5];
	int32 StatModPct[5];
	int32 TotalStatModPct[5];
	
	int32 ModDamageDone[7];
	float ModDamageDonePct[7];
	void CalcResistance(uint32 type);
	void CalcStat(uint32 type);
	void RegenerateHealth();
	void RegenerateMana();

	inline bool CanSee(Unit* obj)
	{
		if(!obj)
			return false;

		if(!obj->m_stealth)
			return true;

		float steathlevel = (float)obj->GetStealthLevel();
		if(steathlevel == 0.0f) steathlevel = 1.0f;

		float visibility =  (float)GetStealthDetect()/steathlevel;

		float invisRange =  visibility * 3 + GetFloatValue (UNIT_FIELD_BOUNDINGRADIUS) +obj->GetFloatValue (UNIT_FIELD_BOUNDINGRADIUS);
		if (GetDistance2dSq (obj) <= invisRange * invisRange) 
			return true;
		else
			return false;
	}

	//Make this unit face another unit
	bool setInFront(Unit* target);

	/// Looting
	void generateLoot();
	
	bool Skinned;

	bool Tagged;
	uint64 TaggerGuid;

	/// Misc
	inline void setEmoteState(uint8 emote) { m_emoteState = emote; };
	inline uint32 GetSQL_id() { return spawnid; };

	virtual void setDeathState(DeathState s);

	uint32 GetOldEmote() { return m_oldEmote; }

	// Serialization
	void SaveToDB();
	void SaveToFile(std::stringstream & name);
	//bool LoadFromDB(uint32 guid);
	//bool LoadFromDB(CreatureTemplate *t);
	void LoadAIAgents(CreatureTemplate * t);
	void LoadAIAgents();
	void DeleteFromDB();

	void OnJustDied();
	void OnRemoveCorpse();
	void OnRespawn(MapMgr * m);
	void SafeDelete();
	//void Despawn();
	void SummonExpire(); // this is used for guardians. They are non respawnable creatures linked to a player


	// In Range
	void AddInRangeObject(Object* pObj);
	void OnRemoveInRangeObject(Object* pObj);
	void ClearInRangeSet();

	// Demon
	void EnslaveExpire();

	// Pet
	void UpdatePet();
	uint32 GetEnslaveCount() { return m_enslaveCount; }
	void SetEnslaveCount(uint32 count) { m_enslaveCount = count; }
	uint32 GetEnslaveSpell() { return m_enslaveSpell; }
	void SetEnslaveSpell(uint32 spellId) { m_enslaveSpell = spellId; }
	bool RemoveEnslave();

	inline Player *GetTotemOwner() { return totemOwner; }
	inline void SetTotemOwner(Player *owner) { totemOwner = owner; }
	inline uint32 GetTotemSlot() { return totemSlot; }
	inline void SetTotemSlot(uint32 slot) { totemSlot = slot; }

	inline bool IsPickPocketed() { return m_PickPocketed; }
	inline void SetPickPocketed(bool val = true) { m_PickPocketed = val; }

	inline CreatureAIScript * GetScript() { return _myScriptClass; }
	void LoadScript();

	void CallScriptUpdate();

	uint32 m_TaxiNode;
	CreatureInfo *creature_info;
	inline CreatureInfo *GetCreatureName()
	{
		return creature_info; 
	}
	inline void SetCreatureName(CreatureInfo *ci) { creature_info = ci; }

	inline GossipScript* GetGossipScript() { return _gossipScript; }
	inline void SetGossipScript(GossipScript* GS) { _gossipScript = GS; }
	inline Trainer* GetTrainer() { return mTrainer; }
	void RegenerateFocus();

	CreatureFamilyEntry * myFamily;
	inline bool IsTotem() { return totemOwner != 0 && totemSlot != -1; }
	void TotemExpire();
	void FormationLinkUp(uint32 SqlId);
	bool haslinkupevent;
	WayPoint * CreateWaypointStruct();
	uint32 spawnid;
	uint32 original_emotestate;
	CreatureProto * proto;
	CreatureSpawn * m_spawn;
	void OnPushToWorld();
	void Despawn(uint32 delay, uint32 respawntime);
	void TriggerScriptEvent(void * func);

	AuctionHouse * auctionHouse;
	bool has_waypoint_text;
	bool has_combat_text;

	void DeleteMe();
	bool CanAddToWorld();

	WayPointMap * m_custom_waypoint_map;
	Player * m_escorter;
	void DestroyCustomWaypointMap();
	bool IsInLimboState() { return m_limbostate; }
	uint32 GetLineByFamily(CreatureFamilyEntry * family){return family->skilline ? family->skilline : 0;};
	void RemoveLimboState(Unit * healer);
	void SetGuardWaypoints();
	bool m_corpseEvent;
	MapCell * m_respawnCell;
protected:
	CreatureAIScript *_myScriptClass;
	bool m_limbostate;
	GossipScript *_gossipScript;
	Trainer* mTrainer;

	void _LoadGoods();
	void _LoadGoods(std::list<CreatureItem*>* lst);
	void _LoadMovement();

	/// Vendor data
	std::vector<CreatureItem>* m_SellItems;

	/// Taxi data
	uint32 mTaxiNode;

	/// Quest data
	std::list<QuestRelation *>* m_quests;
   
	/// Pet
	uint32 m_enslaveCount;
	uint32 m_enslaveSpell;

	Player * totemOwner;
	int32 totemSlot;

	bool m_PickPocketed;
	uint32 _fields[UNIT_END];
};

#endif
