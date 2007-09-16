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

#ifndef _PLAYER_H
#define _PLAYER_H
struct BGScore;
class Channel;
class Creature;
class Battleground;
class TaxiPath;
class GameObject;
class Transporter;
class Corpse;
class Guild;
class Pet;
class Charter;
struct LevelInfo;
#define myabs(a) (a<0)?(-a):a
#define MAX_PET_NO 3
#define PLAYER_NORMAL_RUN_SPEED 7.0
#define MONSTER_NORMAL_RUN_SPEED 8.0
#define PLAYER_NORMAL_SWIM_SPEED 4.722222
#define PLAYER_NORMAL_FLIGHT_SPEED 7.0
#define PLAYER_HONORLESS_TARGET_SPELL 2479
//====================================================================
//  Inventory
//  Holds the display id and item type id for objects in
//  a character's inventory
//====================================================================
enum Classes
{
	WARRIOR = 1,
	PALADIN = 2,
	HUNTER = 3,
	ROGUE = 4,
	PRIEST = 5,
	SHAMAN = 7,
	MAGE = 8,
	WARLOCK = 9,
	DRUID = 11,
};
enum Races
{
	RACE_HUMAN = 1,
	RACE_ORC = 2,
	RACE_DWARF = 3,
	RACE_NIGHTELF = 4,
	RACE_UNDEAD = 5,
	RACE_TAUREN = 6,
	RACE_GNOME = 7,
	RACE_TROLL = 8,
	RACE_BLOODELF = 10,
	RACE_DRAENEI = 11,
};
enum PlayerStatus
{
	NONE			 = 0,
	TRANSFER_PENDING = 1,
};

enum RankTitles
{
	PVPTITLE_NONE                   = 0x0,
	PVPTITLE_PRIVATE                = 0x02,
	PVPTITLE_CORPORAL               = 0x04,
	PVPTITLE_SERGEANT               = 0x08,
	PVPTITLE_MASTER_SERGEANT        = 0x10,
	PVPTITLE_SERGEANT_MAJOR         = 0x20,
	PVPTITLE_KNIGHT                 = 0x40,
	PVPTITLE_KNIGHT_LIEUTENANT      = 0x80,
	PVPTITLE_KNIGHT_CAPTAIN         = 0x100,
	PVPTITLE_KNIGHT_CHAMPION        = 0x200,
	PVPTITLE_LIEUTENANT_COMMANDER   = 0x400,
	PVPTITLE_COMMANDER              = 0x800,
	PVPTITLE_MARSHAL                = 0x1000,
	PVPTITLE_FIELD_MARSHAL          = 0x2000,
	PVPTITLE_GRAND_MARSHAL          = 0x4000,
	PVPTITLE_SCOUT                  = 0x8000,
	PVPTITLE_GRUNT                  = 0x10000,
	PVPTITLE_HSERGEANT              = 0x20000,
	PVPTITLE_SENIOR_SERGEANT        = 0x40000,
	PVPTITLE_FIRST_SERGEANT         = 0x80000,
	PVPTITLE_STONE_GUARD            = 0x100000,
	PVPTITLE_BLOOD_GUARD            = 0x200000,
	PVPTITLE_LEGIONNAIRE            = 0x400000,
	PVPTITLE_CENTURION              = 0x800000,
	PVPTITLE_CHAMPION               = 0x1000000,
	PVPTITLE_LIEUTENANT_GENERAL     = 0x2000000,
	PVPTITLE_GENERAL                = 0x4000000,
	PVPTITLE_WARLORD                = 0x8000000,
	PVPTITLE_HIGH_WARLORD           = 0x10000000,
};

enum PvPAreaStatus
{
    AREA_ALLIANCE = 1,
    AREA_HORDE = 2,
    AREA_CONTESTED = 3,
    AREA_PVPARENA = 4,
};

enum PlayerMovementType
{
    MOVE_ROOT	    = 1,
    MOVE_UNROOT	    = 2,
    MOVE_WATER_WALK = 3,
    MOVE_LAND_WALK  = 4,
};
enum PlayerSpeedType
{
    RUN	            = 1,
    RUNBACK         = 2,
    SWIM	        = 3,
    SWIMBACK        = 4,
    WALK	        = 5,
    FLY	            = 6,
};
enum Standing
{
    STANDING_HATED,
    STANDING_HOSTILE,
    STANDING_UNFRIENDLY,
    STANDING_NEUTRAL,
    STANDING_FRIENDLY,
    STANDING_HONORED,
    STANDING_REVERED,
    STANDING_EXALTED
};
enum PlayerFlags
{
    PLAYER_FLAG_PARTY_LEADER		= 0x01,
    PLAYER_FLAG_AFK					= 0x02,
    PLAYER_FLAG_DND					= 0x04,
    PLAYER_FLAG_GM					= 0x08,
    PLAYER_FLAG_DEATH_WORLD_ENABLE  = 0x10,
    PLAYER_FLAG_RESTING				= 0x20,
    PLAYER_FLAG_UNKNOWN1            = 0x40,
    PLAYER_FLAG_FREE_FOR_ALL_PVP	= 0x80,
    PLAYER_FLAG_UNKNOWN2            = 0x100,
    PLAYER_FLAG_PVP_TOGGLE			= 0x200,
    PLAYER_FLAG_NOHELM				= 0x400,
    PLAYER_FLAG_NOCLOAK				= 0x800,
    PLAYER_FLAG_NEED_REST_3_HOURS	= 0x1000,
    PLAYER_FLAG_NEED_REST_5_HOURS	= 0x2000,
};

enum CharterTypes
{
	CHARTER_TYPE_GUILD			= 0,
	CHARTER_TYPE_ARENA_2V2		= 1,
	CHARTER_TYPE_ARENA_3V3		= 2,
	CHARTER_TYPE_ARENA_5V5		= 3,
	NUM_CHARTER_TYPES			= 4,
};

enum ArenaTeamTypes
{
	ARENA_TEAM_TYPE_2V2			= 0,
	ARENA_TEAM_TYPE_3V3			= 1,
	ARENA_TEAM_TYPE_5V5			= 2,
	NUM_ARENA_TEAM_TYPES		= 3,
};

struct spells
{
	uint16  spellId;
	uint16  slotId;
};
#pragma pack(push,1)
struct ActionButton
{
	uint16  Action;
	uint8   Type;
	uint8   Misc;
};
#pragma pack(pop)

struct CreateInfo_ItemStruct
{
	uint32  protoid;
	uint8   slot;
	uint32  amount;
};
struct CreateInfo_SkillStruct
{
	uint32  skillid;
	uint32  currentval;
	uint32  maxval;
};
struct CreateInfo_ActionBarStruct
{
	uint32  button;
	uint32  action;
	uint32  type;
	uint32  misc;
};
struct PlayerCreateInfo{
	uint8   index;
	uint8   race;
	uint32  factiontemplate;
	uint8   class_;
	uint32  mapId;
	uint32  zoneId;
	float   positionX;
	float   positionY;
	float   positionZ;
	uint16  displayId;
	uint8   strength;
	uint8   ability;
	uint8   stamina;
	uint8   intellect;
	uint8   spirit;
	uint32  health;
	uint32  mana;
	uint32  rage;
	uint32  focus;
	uint32  energy;
	uint32  attackpower;
	float   mindmg;
	float   maxdmg;
	std::list<CreateInfo_ItemStruct> items;
	std::list<CreateInfo_SkillStruct> skills;
	std::list<CreateInfo_ActionBarStruct> actionbars;
	std::list<uint16> spell_list;
	//uint32 item[10];
	//uint8 item_slot[10];
	//uint16 spell[10];
};
struct DamageSplit
{
	Player* caster;
	Aura*   aura;
	uint32  miscVal;
	union
	{
		uint32 damage;
		float damagePCT;
	};
};

struct LoginAura
{
    uint32 id;
    uint32 dur;
};

const float SpellCritFromInt[74][12] = { //[level][class]
	// War Paladin Hunter Rog Priest  -  Shaman  Mage    Warlock -  Druid
	   {0, 0, 0     , 0     , 0, 0     , 0, 0     , 0     , 0     , 0, 0      }, //level 0
       {0, 0, 0.0832, 0.0699, 0, 0.1710, 0, 0.1333, 0.1637, 0.1500, 0, 0.1431 },
       {0, 0, 0.0793, 0.0666, 0, 0.1636, 0, 0.1272, 0.1574, 0.1435, 0, 0.1369 },
       {0, 0, 0.0793, 0.0666, 0, 0.1568, 0, 0.1217, 0.1516, 0.1375, 0, 0.1312 },
       {0, 0, 0.0757, 0.0635, 0, 0.1505, 0, 0.1217, 0.1411, 0.1320, 0, 0.1259 },
       {0, 0, 0.0757, 0.0635, 0, 0.1394, 0, 0.1166, 0.1364, 0.1269, 0, 0.1211 },
       {0, 0, 0.0724, 0.0608, 0, 0.1344, 0, 0.1120, 0.1320, 0.1222, 0, 0.1166 },
       {0, 0, 0.0694, 0.0608, 0, 0.1297, 0, 0.1077, 0.1279, 0.1179, 0, 0.1124 },
       {0, 0, 0.0694, 0.0583, 0, 0.1254, 0, 0.1037, 0.1240, 0.1138, 0, 0.1124 },
       {0, 0, 0.0666, 0.0583, 0, 0.1214, 0, 0.1000, 0.1169, 0.1100, 0, 0.1086 },
       {0, 0, 0.0666, 0.0559, 0, 0.1140, 0, 0.1000, 0.1137, 0.1065, 0, 0.0984 },
       {0, 0, 0.0640, 0.0559, 0, 0.1045, 0, 0.0933, 0.1049, 0.0971, 0, 0.0926 },
       {0, 0, 0.0616, 0.0538, 0, 0.0941, 0, 0.0875, 0.0930, 0.0892, 0, 0.0851 },
       {0, 0, 0.0594, 0.0499, 0, 0.0875, 0, 0.0800, 0.0871, 0.0825, 0, 0.0807 },
       {0, 0, 0.0574, 0.0499, 0, 0.0784, 0, 0.0756, 0.0731, 0.0767, 0, 0.0750 },
       {0, 0, 0.0537, 0.0466, 0, 0.0724, 0, 0.0700, 0.0671, 0.0717, 0, 0.0684 },
       {0, 0, 0.0537, 0.0466, 0, 0.0684, 0, 0.0666, 0.0639, 0.0688, 0, 0.0656 },
       {0, 0, 0.0520, 0.0451, 0, 0.0627, 0, 0.0636, 0.0602, 0.0635, 0, 0.0617 },
       {0, 0, 0.0490, 0.0424, 0, 0.0597, 0, 0.0596, 0.0568, 0.0600, 0, 0.0594 },
       {0, 0, 0.0490, 0.0424, 0, 0.0562, 0, 0.0571, 0.0538, 0.0569, 0, 0.0562 },
       {0, 0, 0.0462, 0.0399, 0, 0.0523, 0, 0.0538, 0.0505, 0.0541, 0, 0.0516 },
       {0, 0, 0.0450, 0.0388, 0, 0.0502, 0, 0.0518, 0.0487, 0.0516, 0, 0.0500 },
       {0, 0, 0.0438, 0.0388, 0, 0.0470, 0, 0.0500, 0.0460, 0.0493, 0, 0.0477 },
       {0, 0, 0.0427, 0.0368, 0, 0.0453, 0, 0.0474, 0.0445, 0.0471, 0, 0.0463 },
       {0, 0, 0.0416, 0.0358, 0, 0.0428, 0, 0.0459, 0.0422, 0.0446, 0, 0.0437 },
       {0, 0, 0.0396, 0.0350, 0, 0.0409, 0, 0.0437, 0.0405, 0.0429, 0, 0.0420 },
       {0, 0, 0.0387, 0.0341, 0, 0.0392, 0, 0.0424, 0.0390, 0.0418, 0, 0.0409 },
       {0, 0, 0.0387, 0.0333, 0, 0.0376, 0, 0.0412, 0.0372, 0.0398, 0, 0.0394 },
       {0, 0, 0.0370, 0.0325, 0, 0.0362, 0, 0.0394, 0.0338, 0.0384, 0, 0.0384 },
       {0, 0, 0.0362, 0.0318, 0, 0.0348, 0, 0.0383, 0.0325, 0.0367, 0, 0.0366 },
       {0, 0, 0.0347, 0.0304, 0, 0.0333, 0, 0.0368, 0.0312, 0.0355, 0, 0.0346 },
       {0, 0, 0.0340, 0.0297, 0, 0.0322, 0, 0.0354, 0.0305, 0.0347, 0, 0.0339 },
       {0, 0, 0.0333, 0.0297, 0, 0.0311, 0, 0.0346, 0.0294, 0.0333, 0, 0.0325 },
       {0, 0, 0.0326, 0.0285, 0, 0.0301, 0, 0.0333, 0.0286, 0.0324, 0, 0.0318 },
       {0, 0, 0.0320, 0.0280, 0, 0.0289, 0, 0.0325, 0.0278, 0.0311, 0, 0.0309 },
       {0, 0, 0.0308, 0.0269, 0, 0.0281, 0, 0.0314, 0.0269, 0.0303, 0, 0.0297 },
       {0, 0, 0.0303, 0.0264, 0, 0.0273, 0, 0.0304, 0.0262, 0.0295, 0, 0.0292 },
       {0, 0, 0.0297, 0.0264, 0, 0.0263, 0, 0.0298, 0.0254, 0.0284, 0, 0.0284 },
       {0, 0, 0.0287, 0.0254, 0, 0.0256, 0, 0.0289, 0.0248, 0.0277, 0, 0.0276 },
       {0, 0, 0.0282, 0.0250, 0, 0.0249, 0, 0.0283, 0.0241, 0.0268, 0, 0.0269 },
       {0, 0, 0.0273, 0.0241, 0, 0.0241, 0, 0.0272, 0.0235, 0.0262, 0, 0.0256 },
       {0, 0, 0.0268, 0.0237, 0, 0.0235, 0, 0.0267, 0.0230, 0.0256, 0, 0.0252 },
       {0, 0, 0.0264, 0.0237, 0, 0.0228, 0, 0.0262, 0.0215, 0.0248, 0, 0.0244 },
       {0, 0, 0.0256, 0.0229, 0, 0.0223, 0, 0.0254, 0.0211, 0.0243, 0, 0.0240 },
       {0, 0, 0.0256, 0.0225, 0, 0.0216, 0, 0.0248, 0.0206, 0.0236, 0, 0.0233 },
       {0, 0, 0.0248, 0.0218, 0, 0.0210, 0, 0.0241, 0.0201, 0.0229, 0, 0.0228 },
       {0, 0, 0.0245, 0.0215, 0, 0.0206, 0, 0.0235, 0.0197, 0.0224, 0, 0.0223 },
       {0, 0, 0.0238, 0.0212, 0, 0.0200, 0, 0.0231, 0.0192, 0.0220, 0, 0.0219 },
       {0, 0, 0.0231, 0.0206, 0, 0.0196, 0, 0.0226, 0.0188, 0.0214, 0, 0.0214 },
       {0, 0, 0.0228, 0.0203, 0, 0.0191, 0, 0.0220, 0.0184, 0.0209, 0, 0.0209 },
       {0, 0, 0.0222, 0.0197, 0, 0.0186, 0, 0.0215, 0.0179, 0.0204, 0, 0.0202 },
       {0, 0, 0.0219, 0.0194, 0, 0.0183, 0, 0.0210, 0.0176, 0.0200, 0, 0.0198 },
       {0, 0, 0.0216, 0.0192, 0, 0.0178, 0, 0.0207, 0.0173, 0.0195, 0, 0.0193 },
       {0, 0, 0.0211, 0.0186, 0, 0.0175, 0, 0.0201, 0.0170, 0.0191, 0, 0.0191 },
       {0, 0, 0.0208, 0.0184, 0, 0.0171, 0, 0.0199, 0.0166, 0.0186, 0, 0.0186 },
       {0, 0, 0.0203, 0.0179, 0, 0.0166, 0, 0.0193, 0.0162, 0.0182, 0, 0.0182 },
       {0, 0, 0.0201, 0.0177, 0, 0.0164, 0, 0.0190, 0.0154, 0.0179, 0, 0.0179 },
       {0, 0, 0.0198, 0.0175, 0, 0.0160, 0, 0.0187, 0.0151, 0.0176, 0, 0.0176 },
       {0, 0, 0.0191, 0.0170, 0, 0.0157, 0, 0.0182, 0.0149, 0.0172, 0, 0.0173 },
       {0, 0, 0.0189, 0.0168, 0, 0.0154, 0, 0.0179, 0.0146, 0.0168, 0, 0.0169 },
       {0, 0, 0.0185, 0.0164, 0, 0.0151, 0, 0.0175, 0.0143, 0.0165, 0, 0.0164 },
       {0, 0, 0.0157, 0.0157, 0, 0.0148, 0, 0.0164, 0.0143, 0.0159, 0, 0.0162 },
       {0, 0, 0.0153, 0.0154, 0, 0.0145, 0, 0.0159, 0.0143, 0.0154, 0, 0.0157 },
       {0, 0, 0.0148, 0.0150, 0, 0.0143, 0, 0.0152, 0.0143, 0.0148, 0, 0.0150 },
       {0, 0, 0.0143, 0.0144, 0, 0.0139, 0, 0.0147, 0.0142, 0.0143, 0, 0.0146 },
       {0, 0, 0.0140, 0.0141, 0, 0.0137, 0, 0.0142, 0.0142, 0.0138, 0, 0.0142 },
       {0, 0, 0.0136, 0.0137, 0, 0.0134, 0, 0.0138, 0.0138, 0.0135, 0, 0.0137 },
       {0, 0, 0.0133, 0.0133, 0, 0.0132, 0, 0.0134, 0.0133, 0.0130, 0, 0.0133 },
       {0, 0, 0.0131, 0.0130, 0, 0.0130, 0, 0.0131, 0.0131, 0.0127, 0, 0.0131 },
       {0, 0, 0.0128, 0.0128, 0, 0.0127, 0, 0.0128, 0.0128, 0.0125, 0, 0.0128 },
       {0, 0, 0.0125, 0.0125, 0, 0.0125, 0, 0.0125, 0.0125, 0.0122, 0, 0.0125 },  // level 70
};





/*
Exalted	        1,000	 Access to racial mounts. Capped at 999.7
Revered	        21,000	 Heroic mode keys for Outland dungeons
Honored	        12,000	 10% discount from faction vendors
Friendly	    6,000
Neutral	        3,000
Unfriendly	    3,000	 Cannot buy, sell or interact.
Hostile	        3,000	 You will always be attacked on sight
Hated	        36,000 
*/
enum FactionRating
{
	HATED,
	HOSTILE,
	UNFRIENDLY,
	NEUTRAL,
	FRIENDLY,
	HONORED,
	REVERED,
	EXALTED
};
struct FactionReputation
{
	int32 standing;
	uint8 flag;
	int32 baseStanding;
	inline int32 CalcStanding() { return standing - baseStanding; }
	inline bool Positive() { return standing >= 0; }
	FactionRating CalcRating()
	{		
		register long st=CalcStanding();
		if(st>=42000)
			return EXALTED;
		if(st>=21000)
			return REVERED;
		if(st>=9000)
			return HONORED;
		if(st>=3000)
			return FRIENDLY;
		if(st>=0)
			return NEUTRAL;
		if(st>-3000)
			return UNFRIENDLY;
		if(st>-6000)
			return HOSTILE;
		return HATED;
	}
};
struct PlayerInfo
{
	uint64 guid;
	uint32 acct;
	std::string name;
	uint32 race;
	uint32 gender;
	uint32 cl;
	uint32 team;
	
	//guild stuff
	uint32 Rank;//guild rank
	std::string publicNote;
	std::string officerNote;
	time_t lastOnline;
	uint32 lastZone;
	uint32 lastLevel;
	Group * m_Group;
	uint8 subGroup;

	Player * m_loggedInPlayer;
};
struct PlayerPet
{
	string name;
	uint32 entry;
	string fields;
	uint32 xp;
	bool active;
	uint32 number;
	uint32 level;
	uint32 happiness;
	uint32 happinessupdate;
	string actionbar;
	bool summon;
	uint32 autocastspell;
	uint32 loyaltypts;
	uint32 loyaltyupdate;
	char loyaltylvl;
};
enum MeetingStoneQueueStatus
{
	MEETINGSTONE_STATUS_NONE								= 0,
	MEETINGSTONE_STATUS_JOINED_MEETINGSTONE_QUEUE_FOR	   = 1,
	MEETINGSTONE_STATUS_PARTY_MEMBER_LEFT_LFG			   = 2,
	MEETINGSTONE_STATUS_PARTY_MEMBER_REMOVED_PARTY_REMOVED  = 3,
	MEETINGSTONE_STATUS_LOOKING_FOR_NEW_PARTY_IN_QUEUE	  = 4,
	MEETINGSTONE_STATUS_NONE_UNK							= 5,
};
enum ItemPushResultTypes
{
	ITEM_PUSH_TYPE_LOOT			 = 0x00000000,
	ITEM_PUSH_TYPE_RECEIVE		  = 0x00000001,
	ITEM_PUSH_TYPE_CREATE		   = 0x00000002,
};
struct WeaponModifier
{
	uint32 wclass;
	uint32 subclass;
	float value;
};
struct PetActionBar
{
	uint32 spell[10];
};
struct ItemCooldown
{
	uint16 SpellID;
	uint16 ItemEntry;
	uint16 SpellCategory;	   //this maybe got from spell id, maybe saved for speed
	uint32 CooldownTimeStamp;
	uint32 Cooldown;
};
struct classScriptOverride
{
	uint32 id;
	uint32 effect;
	uint32 aura;
	uint32 damage;
	bool percent;
};
class Spell;
class Item;
class Container;
class WorldSession;
class ItemInterface;
class GossipMenu;
struct TaxiPathNode;

#define RESTSTATE_RESTED			 1
#define RESTSTATE_NORMAL			 2
#define RESTSTATE_TIRED100		     3
#define RESTSTATE_TIRED50			 4
#define RESTSTATE_EXHAUSTED		     5
#define UNDERWATERSTATE_NONE		 0
#define UNDERWATERSTATE_SWIMMING	 1
#define UNDERWATERSTATE_UNDERWATER   2
#define UNDERWATERSTATE_RECOVERING   4
#define UNDERWATERSTATE_TAKINGDAMAGE 8
#define UNDERWATERSTATE_FATIGUE	     16
#define UNDERWATERSTATE_LAVA		 32
#define UNDERWATERSTATE_SLIME		 64
enum TRADE_STATUS
{
	TRADE_STATUS_PLAYER_BUSY	    = 0x00,
	TRADE_STATUS_PROPOSED		    = 0x01,
	TRADE_STATUS_INITIATED		    = 0x02,
	TRADE_STATUS_CANCELLED		    = 0x03,
	TRADE_STATUS_ACCEPTED		    = 0x04,
	TRADE_STATUS_ALREADY_TRADING    = 0x05,
	TRADE_STATUS_PLAYER_NOT_FOUND   = 0x06,
	TRADE_STATUS_STATE_CHANGED	    = 0x07,
	TRADE_STATUS_COMPLETE		    = 0x08,
	TRADE_STATUS_UNACCEPTED		    = 0x09,
	TRADE_STATUS_TOO_FAR_AWAY	    = 0x0A,
	TRADE_STATUS_WRONG_FACTION	    = 0x0B,
	TRADE_STATUS_FAILED			    = 0x0C,
	TRADE_STATUS_DEAD			    = 0x0D,
	TRADE_STATUS_PETITION		    = 0x0E,
	TRADE_STATUS_PLAYER_IGNORED	    = 0x0F,
};
enum TRADE_DATA
{
	TRADE_GIVE		= 0x00,
	TRADE_RECEIVE	 = 0x01,
};
enum DUEL_STATUS
{
	DUEL_STATUS_OUTOFBOUNDS,
	DUEL_STATUS_INBOUNDS
};
enum DUEL_STATE
{
	DUEL_STATE_REQUESTED,
	DUEL_STATE_STARTED,
	DUEL_STATE_FINISHED
};
enum DUEL_WINNER
{
	DUEL_WINNER_KNOCKOUT,
	DUEL_WINNER_RETREAT,
};
#define PLAYER_ATTACK_TIMEOUT_INTERVAL	5000
#define PLAYER_FORCED_RESURECT_INTERVAL	360000 // 1000*60*6= 6 minutes 

struct PlayerSkill
{
	skilllineentry * Skill;
	uint32 CurrentValue;
	uint32 MaximumValue;
	uint32 BonusValue;
	float GetSkillUpChance();
	void Reset(uint32 Id);
};

enum SPELL_INDEX
{
	SPELL_TYPE_INDEX_CURSE		= 0,
	SPELL_TYPE_INDEX_MARK		= 1,
	SPELL_TYPE_INDEX_STING		= 2,
	NUM_SPELL_TYPE_INDEX		= 3,
};

class ArenaTeam;
//====================================================================
//  Player
//  Class that holds every created character on the server.
//
//  TODO:  Attach characters to user accounts
//====================================================================
typedef std::set<uint32>                            SpellSet;
typedef std::set<ItemCooldown*>                     ItemCooldownSet;
typedef std::list<classScriptOverride*>             ScriptOverrideList;
typedef std::set<uint32>                            SaveSet;
typedef std::map<uint32, uint32>                    SpellCooldownHolderMap;
typedef std::map<uint64, ByteBuffer*>               SplineMap;
typedef std::map<uint32, ScriptOverrideList* >      SpellOverrideMap;
typedef std::map<uint32, uint32>                    SpellOverrideExtraAuraMap;
typedef std::map<uint32, FactionReputation*>        ReputationMap;
typedef std::map<uint32, uint64>                    SoloSpells;
typedef std::map<SpellEntry*, pair<uint32, uint32> >StrikeSpellMap;
typedef std::map<uint32, OnHitSpell >               StrikeSpellDmgMap;
typedef std::map<uint32, PlayerSkill>				SkillMap;
typedef std::set<Player**>							ReferenceSet;

class SERVER_DECL Player : public Unit
{
	friend class WorldSession;
	friend class Pet;

protected:
	SkillMap m_skills;
	ReferenceSet m_references;

public:

	/************************************************************************/
	/* Reference System	                                                    */
	/************************************************************************/

	inline void AddReference(Player ** ptr) { m_references.insert(ptr); *ptr = this; }
	inline void RemoveReference(Player ** ptr) { m_references.erase(ptr); *ptr = NULL; }

	/************************************************************************/
	/* Skill System															*/
	/************************************************************************/

	void _AdvanceSkillLine(uint32 SkillLine, uint32 Count = 1);
	void _AddSkillLine(uint32 SkillLine, uint32 Current, uint32 Max);
	uint32 _GetSkillLineMax(uint32 SkillLine);
	uint32 _GetSkillLineCurrent(uint32 SkillLine, bool IncludeBonus = true);
	void _RemoveSkillLine(uint32 SkillLine);
	void _UpdateMaxSkillCounts();
	void _ModifySkillBonus(uint32 SkillLine, int32 Delta);
	void _ModifySkillBonusByType(uint32 SkillType, int32 Delta);
	bool _HasSkillLine(uint32 SkillLine);
	void RemoveSpellsFromLine(uint32 skill_line);
	void _RemoveAllSkills();
	void _RemoveLanguages();
	void _AddLanguages(bool All);
	void _AdvanceAllSkills(uint32 count);
	void _ModifySkillMaximum(uint32 SkillLine, uint32 NewMax);

protected:
	void _UpdateSkillFields();
    
public:
	Player ( uint32 high, uint32 low );
	~Player ( );
	bool ok_to_remove;
	PlayerInfo * m_playerInfo;
	Unit * m_spellTypeTargets[NUM_SPELL_TYPE_INDEX];
	void RemoveSpellTargets(uint32 Type);
	void RemoveSpellIndexReferences(uint32 Type);
	void SetSpellTargetType(uint32 Type, Unit* target);

	void AddToWorld();
	void RemoveFromWorld();
	bool Create ( WorldPacket &data );
	
	void Update( uint32 time );
	void BuildEnumData( WorldPacket * p_data );
    void BuildFlagUpdateForNonGroupSet(uint32 index, uint32 flag);
	std::string m_afk_reason;
	void SetAFKReason(std::string reason) { m_afk_reason = reason; };
	inline const char* GetName() { return m_name.c_str(); }
	inline std::string* GetNameString() { return &m_name; }
	void Die();
	//void KilledMonster(uint32 entry, const uint64 &guid);
	void GiveXP(uint32 xp, const uint64 &guid, bool allowbonus);   // to stop rest xp being given
	void ModifyBonuses(uint32 type,int32 val);
	std::map<uint32, uint32> m_wratings;

	ArenaTeam * m_arenaTeams[NUM_ARENA_TEAM_TYPES];
	
    /************************************************************************/
    /* Taxi                                                                 */
    /************************************************************************/
    inline TaxiPath*    GetTaxiPath() { return m_CurrentTaxiPath; }
    inline bool         GetTaxiState() { return m_onTaxi; }
    const uint32&       GetTaximask( uint8 index ) const { return m_taximask[index]; }
    void                LoadTaxiMask(const char* data);
    void                TaxiStart(TaxiPath* path, uint32 modelid, uint32 start_node);
    void                JumpToEndTaxiNode(TaxiPath * path);
    void                EventDismount(uint32 money, float x, float y, float z);
    void                EventTaxiInterpolate();

    inline void         SetTaxiState    (bool state) { m_onTaxi = state; }
    inline void         SetTaximask     (uint8 index, uint32 value ) { m_taximask[index] = value; }
    inline void         SetTaxiPath     (TaxiPath *path) { m_CurrentTaxiPath = path; }
    inline void         SetTaxiPos()	{m_taxi_pos_x = m_position.x; m_taxi_pos_y = m_position.y; m_taxi_pos_z = m_position.z;}
    inline void         UnSetTaxiPos()	{m_taxi_pos_x = 0; m_taxi_pos_y = 0; m_taxi_pos_z = 0; }
    // Taxi related variables
	vector<TaxiPath*>   m_taxiPaths;
    TaxiPath*           m_CurrentTaxiPath;
    uint32              taxi_model_id;
	uint32              lastNode;
    uint32              m_taxi_ride_time;
    uint32              m_taximask[8];
    float               m_taxi_pos_x;
    float               m_taxi_pos_y;
    float               m_taxi_pos_z;
    bool                m_onTaxi;
    
    /************************************************************************/
    /* Quests                                                               */
    /************************************************************************/
	bool HasQuests() 
	{
		for(int i = 0; i < 25; ++i)
		{
			if(m_questlog[i] != 0)
				return true;
		}
		return false;
	}

	int32                GetOpenQuestSlot();
	QuestLogEntry*       GetQuestLogForEntry(uint32 quest);
	inline QuestLogEntry*GetQuestLogInSlot(uint32 slot)  { return m_questlog[slot]; }
    inline uint64        GetQuestSharer()                { return m_questSharer; }
    
    inline void         SetQuestSharer(uint64 guid)     { m_questSharer = guid; }
    void                SetQuestLogSlot(QuestLogEntry *entry, uint32 slot);
    
    inline void         PushToRemovedQuests(uint32 questid)	{ m_removequests.insert(questid);}
    void                AddToFinishedQuests(uint32 quest_id);
    void                EventTimedQuestExpire(Quest *qst, QuestLogEntry *qle, uint32 log_slot);
	
	bool                HasFinishedQuest(uint32 quest_id);
	bool                HasQuestForItem(uint32 itemid);
    bool                CanFinishQuest(Quest* qst);

    //Quest related variables
	uint32 m_questbarrier1[25];
    QuestLogEntry*      m_questlog[25];
	uint32 m_questbarrier2[25];
    std::set<uint32>    m_QuestGOInProgress;
    std::set<uint32>    m_removequests;
    std::set<uint32>    m_finishedQuests;
    uint64              m_questSharer;
    uint32              timed_quest_slot;

    /************************************************************************/
    /* Stun Immobilize                                                      */
    /************************************************************************/
    void SetTriggerStunOrImmobilize(uint32 newtrigger,uint32 new_chance)
    {
        trigger_on_stun = newtrigger;
        trigger_on_stun_chance = new_chance;
    }
    void EventStunOrImmobilize(Unit *proc_target);

    
    void EventPortToGM(Player *p);
	inline uint32 GetTeam() { return m_team; }

	inline bool IsInFeralForm()
	{
		int s = GetShapeShift();
		if(!s)return false;
		//Fight forms that do not use player's weapon
		if(s==1 || s == 5 || s == 8)
			return true;
		else 
			return false;
	}
	void CalcDamage();
	uint32 GetMainMeleeDamage(uint32 AP_owerride); //i need this for windfury

    const uint64& GetSelection( ) const { return m_curSelection; }
	const uint64& GetTarget( ) const { return m_curTarget; }
	void SetSelection(const uint64 &guid) { m_curSelection = guid; }
	void SetTarget(const uint64 &guid) { m_curTarget = guid; }
	
    /************************************************************************/
    /* Spells                                                               */
    /************************************************************************/
	bool HasSpell(uint32 spell);
	bool HasDeletedSpell(uint32 spell);
	int	GetMaxLearnedSpellLevel(uint32 spell);
	void smsg_InitialSpells();
	void addSpell(uint32 spell_idy);
	void removeSpellByHashName(uint32 hash);
	bool removeSpell(uint32 SpellID, bool MoveToDeleted, bool SupercededSpell, uint32 SupercededSpellID);

    // PLEASE DO NOT INLINE!
    void AddOnStrikeSpell(SpellEntry* sp, uint32 delay)
    {
        m_onStrikeSpells.insert( map< SpellEntry*, pair<uint32, uint32> >::value_type( sp, make_pair( delay, 0 ) ) );
    }
    void RemoveOnStrikeSpell(SpellEntry *sp)
    {
        m_onStrikeSpells.erase(sp);
    }
    void AddOnStrikeSpellDamage(uint32 spellid, uint32 mindmg, uint32 maxdmg)
    {
        OnHitSpell sp;
        sp.spellid = spellid;
        sp.mindmg = mindmg;
        sp.maxdmg = maxdmg;
        m_onStrikeSpellDmg[spellid] = sp;
    }
    void RemoveOnStrikeSpellDamage(uint32 spellid)
    {
        m_onStrikeSpellDmg.erase(spellid);
    }

    //Spells variables
    StrikeSpellMap      m_onStrikeSpells;
    StrikeSpellDmgMap   m_onStrikeSpellDmg;
    SpellOverrideMap    mSpellOverrideMap;
    SpellSet            m_SSSPecificSpells;
    SpellSet            mSpells;
    SpellSet            mDeletedSpells;

    /************************************************************************/
    /* Actionbar                                                            */
    /************************************************************************/
	void                setAction(uint8 button, uint16 action, uint8 type, uint8 misc);
	void                SendInitialActions();
    bool                m_actionsDirty;
	
    /************************************************************************/
    /* Reputation                                                           */
    /************************************************************************/
	void                ModStanding(uint32 Faction, int32 Value);
	int32               GetStanding(uint32 Faction);
	int32               GetBaseStanding(uint32 Faction);
	void                SetStanding(uint32 Faction, int32 Value);
	void                SetAtWar(uint32 Faction, bool Set);
	bool                IsAtWar(uint32 Faction);
	Standing            GetStandingRank(uint32 Faction);
	bool                IsHostileBasedOnReputation(FactionDBC * dbc);
	void                UpdateInrangeSetsBasedOnReputation();
	void                Reputation_OnKilledUnit(Unit * pUnit, bool InnerLoop);
	void                Reputation_OnTalk(FactionDBC * dbc);
	static Standing     GetReputationRankFromStanding(int32 Standing_);
	
    /************************************************************************/
    /* Factions                                                             */
    /************************************************************************/
	void smsg_InitialFactions();
	uint32 GetFactionId();
    // factions variables
    int32 pctReputationMod;

    /************************************************************************/
    /* PVP                                                                  */
    /************************************************************************/
	inline uint8 GetPVPRank()
	{
		return (uint8)((GetUInt32Value(PLAYER_BYTES_3) >> 24) & 0xFF);
	}
	inline void SetPVPRank(int newrank)
	{
		SetUInt32Value(PLAYER_BYTES_3, ((GetUInt32Value(PLAYER_BYTES_3) & 0x00FFFFFF) | (uint8(newrank) << 24)));
	}

    /************************************************************************/
    /* Groups                                                               */
    /************************************************************************/
	void                SetInviter(uint64 pInviter) { m_GroupInviter = pInviter; }
	uint64              GetInviter() { return m_GroupInviter; }
	inline bool         InGroup() { return (m_Group != NULL && !m_GroupInviter); }
	bool                IsGroupLeader()
	{
		if(m_Group != NULL)
		{
			if(m_Group->GetLeader() == this)
				return true;
		}
		return false;
	}
	inline int          HasBeenInvited() { return m_GroupInviter != 0; }
	inline Group*       GetGroup() { return m_Group; }
	inline void         SetGroup(Group* grp) { m_Group = grp; }
	inline uint32       GetSubGroup() { return m_SubGroup; }
	inline void         SetSubGroup(uint32 group) { m_SubGroup = group; }
    bool                IsGroupMember(Player *plyr);
	inline bool         IsBanned() { return ((m_banned > 0) ? true : false); }
    inline void         SetBanned() { m_banned = 4;}
	inline void         SetBanned(string Reason) { m_banned = 4; m_banreason = Reason;}
	inline void         UnSetBanned() { m_banned = 0; }
	inline string       GetBanReason() {return m_banreason;}

    /************************************************************************/
    /* Guilds                                                               */
    /************************************************************************/
	inline  bool        IsInGuild() {return (bool)GetUInt32Value(PLAYER_GUILDID);}
	inline uint32       GetGuildId() { return m_uint32Values[PLAYER_GUILDID]; }
	void                SetGuildId(uint32 guildId);
	inline uint32       GetGuildRank() { return m_uint32Values[PLAYER_GUILDRANK]; }
	void                SetGuildRank(uint32 guildRank) { SetUInt32Value(PLAYER_GUILDRANK, guildRank); }
	uint64              GetGuildInvitersGuid() { return m_invitersGuid; }
	void                SetGuildInvitersGuid( uint64 guid ) { m_invitersGuid = guid; }
	void                UnSetGuildInvitersGuid() { m_invitersGuid = 0; }
  
    /************************************************************************/
    /* Duel                                                                 */
    /************************************************************************/
    void                RequestDuel(Player *pTarget);
	void                DuelBoundaryTest();
	void                EndDuel(uint8 WinCondition);
	void                DuelCountdown();
	void                SetDuelStatus(uint8 status) { m_duelStatus = status; }
	inline uint8        GetDuelStatus() { return m_duelStatus; }
	void                SetDuelState(uint8 state) { m_duelState = state; }
	inline uint8        GetDuelState() { return m_duelState; }
    // duel variables
    Player*             DuelingWith;

    /************************************************************************/
    /* Trade                                                                */
    /************************************************************************/
	void                SendTradeUpdate(void);
	inline void         ResetTradeVariables()
	{
		mTradeGold = 0;
		memset(&mTradeItems, 0, sizeof(Item*) * 8);
		mTradeStatus = 0;
		mTradeTarget = 0;
	}
	
    /************************************************************************/
    /* Pets                                                                 */
    /************************************************************************/
	inline void         SetSummon(Pet *pet) { m_Summon = pet; }
	inline Pet*         GetSummon(void) { return m_Summon; }
	uint32              GeneratePetNumber(void);
	void                RemovePlayerPet(uint32 pet_number);
	inline void         AddPlayerPet(PlayerPet* pet, uint32 index) { m_Pets[index] = pet; }
	inline PlayerPet*   GetPlayerPet(uint32 idx)
	{
		std::map<uint32, PlayerPet*>::iterator itr = m_Pets.find(idx);
		if(itr != m_Pets.end()) return itr->second;
		else
			return NULL;
	}
	void                SpawnPet(uint32 pet_number);
	void                DespawnPet();
	uint32              GetFirstPetNumber(void)
	{
		if(m_Pets.size() == 0) return 0;
		std::map<uint32, PlayerPet*>::iterator itr = m_Pets.begin();
		return itr->first;
	}
	inline PlayerPet*   GetFirstPet(void) { return GetPlayerPet(GetFirstPetNumber()); }
	inline void         SetStableSlotCount(uint8 count) { m_StableSlotCount = count; }
	inline uint8        GetStableSlotCount(void) { return m_StableSlotCount; }

    /************************************************************************/
    /* Item Interface                                                       */
    /************************************************************************/
	inline ItemInterface* GetItemInterface() { return m_ItemInterface; } // Player Inventory Item storage
	inline void         ApplyItemMods(Item *item, int8 slot, bool apply,bool justdrokedown=false) {  _ApplyItemMods(item, slot, apply,justdrokedown); }
    // item interface variables
    ItemInterface *     m_ItemInterface;
	
    /************************************************************************/
    /* Loot                                                                 */
    /************************************************************************/
	inline const uint64& GetLootGUID() const { return m_lootGuid; }
	inline void         SetLootGUID(const uint64 &guid) { m_lootGuid = guid; }
	void                SendLoot(uint64 guid,uint8 loot_type);
    // loot variables
    uint64              m_lootGuid;
    uint64              m_currentLoot;
    bool                bShouldHaveLootableOnCorpse;

    /************************************************************************/
    /* World Session                                                        */
    /************************************************************************/
	inline WorldSession* GetSession() const { return m_session; }
	void SetSession(WorldSession *s) { m_session = s; }
	void SetBindPoint(float x, float y, float z, uint32 m, uint32 v) { m_bind_pos_x = x; m_bind_pos_y = y; m_bind_pos_z = z; m_bind_mapid = m; m_bind_zoneid = v;}
	void SendDelayedPacket(WorldPacket *data, bool bDeleteOnSend)
	{
		if(data == NULL) return;
		if(GetSession() != NULL) GetSession()->SendPacket(data);
		if(bDeleteOnSend) delete data;
	}
	float offhand_dmg_mod;
	float GetSpellTimeMod(uint32 id);
	int GetSpellDamageMod(uint32 id);
	int32 GetSpellManaMod(uint32 id);
	
	// Talents
	// These functions build a specific type of A9 packet
	uint32 __fastcall BuildCreateUpdateBlockForPlayer( ByteBuffer *data, Player *target );
	void DestroyForPlayer( Player *target ) const;
	
	std::list<LoginAura> loginauras;

    std::set<uint32> OnMeleeAuras;

    /************************************************************************/
    /* Player loading and savings                                           */
    /* Serialize character to db                                            */
    /************************************************************************/
	void SaveToDB(bool bNewCharacter);
	void SaveAuras(stringstream&);
	bool LoadFromDB(uint32 guid);
	void LoadFromDB_Light(Field *fields, uint32 guid);
	void LoadPropertiesFromDB();
	void LoadNamesFromDB(uint32 guid);
	void DeleteFromDB();
	bool m_FirstLogin;

    /************************************************************************/
    /* Death system                                                         */
    /************************************************************************/
	void SpawnCorpseBody();
	void SpawnCorpseBones();
	void CreateCorpse();
	void KillPlayer();
	void ResurrectPlayer();
	void BuildPlayerRepop();
	void RepopRequestedPlayer();
	void DeathDurabilityLoss(double percent);
	void RepopAtGraveyard(float ox, float oy, float oz, uint32 mapid);
	
    /************************************************************************/
    /* Movement system                                                      */
    /************************************************************************/
	void SetMovement(uint8 pType, uint32 flag);
	void SetPlayerSpeed(uint8 SpeedType, float value);
	float GetPlayerSpeed(){return m_runSpeed;}
	uint8 m_currentMovement;
	bool m_isMoving;
	//Invisibility stuff
	bool m_isGmInvisible;
	
    /************************************************************************/
    /* Channel stuff                                                        */
    /************************************************************************/
	void JoinedChannel(Channel *c);
	void LeftChannel(Channel *c);
	void CleanupChannels();
	//Attack stuff
	void EventAttackStart();
	void EventAttackStop();
	void EventAttackUpdateSpeed() { }
	void EventDeath();
	//Note:ModSkillLine -> value+=amt;ModSkillMax -->value=amt; --wierd
	float GetSkillUpChance(uint32 id);
	//inline std::list<struct skilllines>getSkillLines() { return m_skilllines; }
	float SpellCrtiticalStrikeRatingBonus;
	float SpellHasteRatingBonus;
	void UpdateAttackSpeed();
	void UpdateChances();
	void UpdateStats();
	void UpdateHit(int32 hit);
   
	bool canCast(SpellEntry *m_spellInfo);
	inline float GetBlockFromSpell() { return m_blockfromspell; }
	inline float GetSpellCritFromSpell() { return m_spellcritfromspell; }
	inline float GetHitFromMeleeSpell() { return m_hitfrommeleespell; }
	inline float GetHitFromSpell() { return m_hitfromspell; }
	inline float GetParryFromSpell() { return m_parryfromspell; }
	inline float GetDodgeFromSpell() { return m_dodgefromspell; }
	void SetBlockFromSpell(float value) { m_blockfromspell = value; }
	void SetSpellCritFromSpell(float value) { m_spellcritfromspell = value; }
	void SetParryFromSpell(float value) { m_parryfromspell = value; }
	void SetDodgeFromSpell(float value) { m_dodgefromspell = value; }
	void SetHitFromMeleeSpell(float value) { m_hitfrommeleespell = value; }
	void SetHitFromSpell(float value) { m_hitfromspell = value; }
	inline uint32 GetHealthFromSpell() { return m_healthfromspell; }
	inline uint32 GetManaFromSpell() { return m_manafromspell; }
	void SetHealthFromSpell(uint32 value) { m_healthfromspell = value;}
	void SetManaFromSpell(uint32 value) { m_manafromspell = value;}
	uint32 CalcTalentResetCost(uint32 resetnum);
	void SendTalentResetConfirm();
	uint32 GetTalentResetTimes() { return m_talentresettimes; }
	inline void SetTalentResetTimes(uint32 value) { m_talentresettimes = value; }
	void SetPlayerStatus(uint8 pStatus) { m_status = pStatus; }
	inline uint8 GetPlayerStatus() { return m_status; }
	const float& GetBindPositionX( ) const { return m_bind_pos_x; }
	const float& GetBindPositionY( ) const { return m_bind_pos_y; }
	const float& GetBindPositionZ( ) const { return m_bind_pos_z; }
	const uint32& GetBindMapId( ) const { return m_bind_mapid; }
	const uint32& GetBindZoneId( ) const { return m_bind_zoneid; }
	inline uint8 GetShapeShift()
	{
		return GetByte(UNIT_FIELD_BYTES_1,2);
	}
	inline void setAttackTimer(int32 time, bool offhand)
	{
		if(!time)
			time = offhand ? m_uint32Values[UNIT_FIELD_BASEATTACKTIME_01] : m_uint32Values[UNIT_FIELD_BASEATTACKTIME];

		time += (time*modAttackTimeIncreasePCT)/100;

		if(offhand)
			m_attackTimer_1 = getMSTime() + time;
		else
			m_attackTimer = getMSTime() + time;
		//do not exit combat just because we are trying to delay attack (for whatever reason, like spellcasting)
		if(!sEventMgr.HasEvent(this,EVENT_ATTACK_TIMEOUT)) //do not add excesive attack events 
			sEventMgr.AddEvent(this,&Player::EventAttackStop,EVENT_ATTACK_TIMEOUT,time+PLAYER_ATTACK_TIMEOUT_INTERVAL,1,0); //attack timeout on no attack after 5 seconds
		else sEventMgr.ModifyEventTimeLeft(this,EVENT_ATTACK_TIMEOUT,time+PLAYER_ATTACK_TIMEOUT_INTERVAL,true);
	}
	
	inline void delayAttackTimer(int32 delay)
	{
		if(!delay)
			return;

		m_attackTimer += delay;
		m_attackTimer_1 += delay;

		if (delay > 0)
		{
			if(sEventMgr.HasEvent(this,EVENT_ATTACK_TIMEOUT))
			{
				sEventMgr.ModifyEventTimeLeft(this,EVENT_ATTACK_TIMEOUT,delay+PLAYER_ATTACK_TIMEOUT_INTERVAL,true);
			}
		}
	}
	
	void SetShapeShift(uint8 ss);

	uint32 m_furorChance;

    //Showing Units WayPoints
	AIInterface* waypointunit;
	void SaveGuild();
	
	uint32 m_nextSave;
	//Tutorials
	uint32 GetTutorialInt(uint32 intId );
	void SetTutorialInt(uint32 intId, uint32 value);
	//Base stats calculations
	//void CalcBaseStats();
	// Rest
	void AddRestXP(uint32 amount);
	uint32 SubtractRestXP(uint32 amount);
	uint32 CalculateRestXP(uint32 seconds);
	uint32 m_lastRestUpdate;
	void EventPlayerRest();
	void ApplyPlayerRestState(bool apply);
	void UpdateRestState();
	//falling?
	uint32 m_fallTime;
	bool bSafeFall;
	// Gossip
	GossipMenu* CurrentGossipMenu;
	void CleanupGossipMenu();
	void Gossip_Complete();
	void Gossip_SendPOI(float X, float Y, uint32 Icon, uint32 Flags, uint32 Data, const char* Name);
	int m_lifetapbonus;
	uint32 m_lastShotTime;
	
	bool m_bUnlimitedBreath;
	uint32 m_UnderwaterTime;
	uint32 m_UnderwaterState;
	uint32 m_SwimmingTime;
	uint32 m_BreathDamageTimer;
	// Visible objects
	bool CanSee(Object* obj);
	inline bool IsVisible(Object* pObj) { return !(m_visibleObjects.find(pObj) == m_visibleObjects.end()); }
	void AddInRangeObject(Object* pObj);
	void OnRemoveInRangeObject(Object* pObj);
	void ClearInRangeSet();
	inline void AddVisibleObject(Object* pObj) { m_visibleObjects.insert(pObj); }
	inline void RemoveVisibleObject(Object* pObj) { m_visibleObjects.erase(pObj); }
	inline void RemoveVisibleObject(InRangeSet::iterator itr) { m_visibleObjects.erase(itr); }
	inline InRangeSet::iterator FindVisible(Object * obj) { return m_visibleObjects.find(obj); }
	inline void RemoveIfVisible(Object * obj)
	{
		InRangeSet::iterator itr = m_visibleObjects.find(obj);
		if(itr == m_visibleObjects.end())
			return;

		m_visibleObjects.erase(obj);
		PushOutOfRange(obj->GetNewGUID());
	}

	inline bool GetVisibility(Object * obj, InRangeSet::iterator *itr)
	{
		*itr = m_visibleObjects.find(obj);
		return ((*itr) != m_visibleObjects.end());
	}

	inline InRangeSet::iterator GetVisibleSetBegin() { return m_visibleObjects.begin(); }
	inline InRangeSet::iterator GetVisibleSetEnd() { return m_visibleObjects.end(); }
	
	//Transporters
	bool m_lockTransportVariables;
	uint64 m_TransporterGUID;
	float m_TransporterX;
	float m_TransporterY;
	float m_TransporterZ;
	float m_TransporterO;
	float m_TransporterUnk;
	// Misc
	void EventCannibalize(uint32 amount);
	void EventReduceDrunk(bool full);
	bool m_AllowAreaTriggerPort;
	void EventAllowTiggerPort(bool enable);
	int32 m_rangedattackspeedmod;
	int32 m_meleeattackspeedmod;
	uint32 m_modblockvalue;
	void SendInitialLogonPackets();
	void Reset_Spells();
	void Reset_Talents();
	void Reset_ToLevel1();
	// Battlegrounds xD
	CBattleground * m_bg;
	CBattleground * m_pendingBattleground;
	uint32 m_bgEntryPointMap;
	float m_bgEntryPointX;	
	float m_bgEntryPointY;
	float m_bgEntryPointZ;
	float m_bgEntryPointO;
	int32 m_bgEntryPointInstance;
	bool m_bgHasFlag;
	bool m_bgIsQueued;
	uint32 m_bgQueueType;
	uint32 m_bgQueueInstanceId;
	void EventRepeatSpell();
	void EventCastRepeatedSpell(uint32 spellid, Unit *target);
	bool CanShootRangedWeapon(uint32 spellid, Unit *target, bool autoshot);
	uint32 m_AutoShotDuration;
	uint32 m_AutoShotAttackTimer;
	bool m_onAutoShot;
	uint64 m_AutoShotTarget;
	float m_AutoShotStartX;
	float m_AutoShotStartY;
	float m_AutoShotStartZ;
	SpellEntry *m_AutoShotSpell;
	void _InitialReputation();
	void EventActivateGameObject(GameObject* obj);
	void EventDeActivateGameObject(GameObject* obj);
	void UpdateNearbyGameObjects();
	
	void CalcResistance(uint32 type);
	inline float res_M_crit_get(){return m_resist_critical[0];}
	inline void res_M_crit_set(float newvalue){m_resist_critical[0]=newvalue;}
	inline float res_R_crit_get(){return m_resist_critical[1];}
	inline void res_R_crit_set(float newvalue){m_resist_critical[1]=newvalue;}
	uint32 FlatResistanceModifierPos[7];
	uint32 FlatResistanceModifierNeg[7];
	uint32 BaseResistanceModPctPos[7];
	uint32 BaseResistanceModPctNeg[7];
	uint32 ResistanceModPctPos[7];
	uint32 ResistanceModPctNeg[7];
	float m_resist_critical[2];//when we are a victim we can have talents to decrease chance for critical hit. This is a negative value and it's added to critchances
	float m_resist_hit[3]; // 0 = melee; 1= ranged; 2=spells
	float SpellDmgDoneByInt[7];
	float SpellHealDoneByInt[7];
	float SpellDmgDoneBySpr[7];
	float SpellHealDoneBySpr[7];
	uint32 m_modphyscritdmgPCT;
	uint32 m_RootedCritChanceBonus;
	uint32 m_ModInterrMRegenPCT;
	float m_RegenManaOnSpellResist;
	
	uint32 FlatStatModPos[5];
	uint32 FlatStatModNeg[5];
	uint32 StatModPctPos[5];
	uint32 StatModPctNeg[5];
	uint32 TotalStatModPctPos[5];
	uint32 TotalStatModPctNeg[5];
	int32 IncreaseDamageByType[12]; //mod dmg by creature type
	float IncreaseDamageByTypePCT[12];
	float IncreaseCricticalByTypePCT[12];
	int32 DetectedRange;
	float PctIgnoreRegenModifier;
	uint32 m_retainedrage;
/*	
	union {
		float mRatingToPct[37];
		uint32 mRatingToPoint[37]; //block, skill.. cant be decimal values
	};
*/
	inline uint32* GetPlayedtime() { return m_playedtime; };
	void CalcStat(uint32 t);
	float CalcRating(uint32 t);
	void RecalcAllRatings();
	void RegenerateMana(float RegenPct);
	void RegenerateHealth(bool inCombat);
	void RegenerateEnergy(float RegenPct);
	void LooseRage();
	uint32 SoulStone;
	uint32 SoulStoneReciever;
	void removeSoulStone();

	bool bReincarnation;

	map<uint32, WeaponModifier> damagedone;
	map<uint32, WeaponModifier> tocritchance;
	uint32 Seal;
	uint32 judgespell;
	bool cannibalize;
	uint8 cannibalizeCount;
	int32 rageFromDamageDealt;
	// GameObject commands
	GameObject *m_GM_SelectedGO;
	
	void _Relocate(uint32 mapid,const LocationVector & v, bool sendpending, bool force_new_world);
	void AddItemsToWorld();
	void RemoveItemsFromWorld();
	
	uint32 m_ShapeShifted;
	uint32 m_MountSpellId;
    
    inline bool IsMounted() {return m_MountSpellId; }
	
    bool bHasBindDialogOpen;
	bool bGMTagOn;
	uint32 TrackingSpell;
	void _EventCharmAttack();
	void _Kick();
	void Kick(uint32 delay = 0);
	void SoftDisconnect();
	uint32 m_KickDelay;
	Unit * m_CurrentCharm;
	Transporter * m_CurrentTransporter;
	
	Object * GetSummonedObject () {return m_SummonedObject;};
	void SetSummonedObject (Object * t_SummonedObject) {m_SummonedObject = t_SummonedObject;};
	uint32 roll;
	void AddRecoverCooldown(SpellEntry * spellInfo);
	void AddCooldown(uint32 spell,uint32 time);
	void AddCategoryCooldown(uint32 cat, uint32 time);
	void AddGlobalCooldown(uint32 time);
	uint32 GetGlobalCooldown() { return GlobalCooldown; }
	void SetGlobalCooldown(uint32 time) { GlobalCooldown = time; }
	void ClearCooldownsOnLine(uint32 skill_line, uint32 called_from);
	void ClearCooldownForSpell(uint32 spell_id);
	bool bProcessPending;
	Mutex _bufferS;
	void PushUpdateData(ByteBuffer *data, uint32 updatecount);
    void PushCreationData(ByteBuffer *data, uint32 updatecount);
	void PushOutOfRange(const WoWGuid & guid);
	void ProcessPendingUpdates();
	bool __fastcall CompressAndSendUpdateBuffer(uint32 size, const uint8* update_buffer);
	void ClearAllPendingUpdates();
	
	uint32 GetArmorProficiency() { return armor_proficiency; }
	uint32 GetWeaponProficiency() { return weapon_proficiency; }
	void ResetHeartbeatCoords();
	float _lastHeartbeatX;
	float _lastHeartbeatY;
	uint32 _heartBeatDisabledUntil;
	uint32 _delayAntiFlyUntil;
	uint32 _lastHeartbeatTime;
	void AddSplinePacket(uint64 guid, ByteBuffer* packet);
	ByteBuffer* GetAndRemoveSplinePacket(uint64 guid);
	void ClearSplinePackets();
	bool ExitInstance();
	void SaveEntryPoint(uint32 mapId);
	bool CooldownCheat;
	bool CastTimeCheat;
	bool GodModeCheat;
	bool PowerCheat;
	bool FlyCheat;
	void ZoneUpdate(uint32 ZoneId);
	inline uint32 GetAreaID() { return m_AreaID; }
	void SetAreaID(uint32 area) { m_AreaID = area; }
	
	
	std::string Lfgcomment;
	uint16 LfgDungeonId[3];
	uint8 LfgType[3];
	bool m_Autojoin;
	bool m_AutoAddMem;
	void StopMirrorTimer(uint32 Type);
	BGScore m_bgScore;
	uint32 m_bgTeam;
	void UpdateChanceFields();
	//Honor Variables
	uint32 m_honorToday;
	uint32 m_honorYesterday;
	
	uint32 m_honorPoints;
	uint32 m_killsToday;
	uint32 m_killsYesterday;
	uint32 m_killsLifetime;
	uint32 m_arenaPoints;
	bool m_honorless;
	uint32 m_lastSeenWeather;
	set<Object*> m_visibleFarsightObjects;
	void EventTeleport(uint32 mapid, float x, float y, float z);
	void ApplyLevelInfo(LevelInfo* Info, uint32 Level);
	void BroadcastMessage(const char* Format, ...);
	map<uint32, set<uint32> > SummonSpells;
	map<uint32, PetSpellMap*> PetSpells;
	void AddSummonSpell(uint32 Entry, uint32 SpellID);
	void RemoveSummonSpell(uint32 Entry, uint32 SpellID);
	set<uint32>* GetSummonSpells(uint32 Entry);
	LockedQueue<WorldPacket*> delayedPackets;
	set<Player *> gmTargets;
	uint32 m_UnderwaterMaxTime;
	uint32 m_UnderwaterLastDmg;
	inline void setMyCorpse(Corpse * corpse) { myCorpse = corpse; }
	inline Corpse * getMyCorpse() { return myCorpse; }
	bool bCorpseCreateable;
	uint32 m_resurrectHealth, m_resurrectMana;
	uint64 resurrector;
	bool blinked;
	uint16 m_speedhackChances;
	uint32 m_explorationTimer;
	// DBC stuff
	CharRaceEntry * myRace;
	CharClassEntry * myClass;
	Unit * linkTarget;
	bool stack_cheat;
	bool SafeTeleport(uint32 MapID, uint32 InstanceID, float X, float Y, float Z, float O);
	bool SafeTeleport(uint32 MapID, uint32 InstanceID, const LocationVector & vec);
	
	void EventSafeTeleport(uint32 MapID, uint32 InstanceID, LocationVector vec)
	{
		SafeTeleport(MapID, InstanceID, vec);
	}

	Guild * myGuild;
	/*****************
	  PVP Stuff
	******************/
	uint32 m_pvpTimer;
	
	//! Is PVP flagged?
	inline bool IsPvPFlagged() { return HasFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PVP); }
	inline void SetPvPFlag()
	{
		// reset the timer as well..
		StopPvPTimer();
		SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PVP);
	}
	//! Removal
	inline void RemovePvPFlag()
	{
		StopPvPTimer();
		RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PVP);			
	}
	//! Do this on /pvp off
	inline void ResetPvPTimer();
	//! Stop the timer for pvp off
	inline void StopPvPTimer() { m_pvpTimer = 0; }
	
	//! Called at login to add the honorless buff, etc.
	void LoginPvPSetup();
	//! Update our pvp area (called when zone changes)
	void UpdatePvPArea();
	//! PvP Toggle (called on /pvp)
	void PvPToggle();
	bool CanCastDueToCooldown(SpellEntry * spellid);
	void SaveHonorFields();
	inline uint32 LastHonorResetTime() const { return m_lastHonorResetTime; }
	inline void LastHonorResetTime(uint32 val) { m_lastHonorResetTime = val; }
	uint32 OnlineTime;
	bool tutorialsDirty;
	LevelInfo * lvlinfo;
	void CalculateBaseStats();
	uint32 load_health;
	uint32 load_mana;
	void CompleteLoading();
	set<SpellEntry *> castSpellAtLogin;
	void OnPushToWorld();
	void OnWorldPortAck();
	uint32 m_TeleportState;
	set<Unit*> visiblityChangableSet;
	bool m_beingPushed;
	bool CanSignCharter(Charter * charter, Player * requester);
	Charter * m_charters[NUM_CHARTER_TYPES];
	uint32 flying_aura;
	stringstream LoadAuras;
	bool resend_speed;
	bool rename_pending;
	uint32 iInstanceType;
	inline void SetName(string& name) { m_name = name; }
	// spell to (delay, last time)
	
	FactionReputation * reputationByListId[128];
	
	uint64 m_comboTarget;
	int8 m_comboPoints;
	bool m_retainComboPoints;
	int8 m_spellcomboPoints; // rogue talent Ruthlessness will change combopoints while consuming them. solutions 1) add post cast prochandling, 2) delay adding the CP
	void UpdateComboPoints();

	inline void AddComboPoints(uint64 target, uint8 count)
	{
        if(m_comboTarget == target)
			m_comboPoints += count;
		else
		{
			m_comboTarget = target;
			m_comboPoints = count;
		}
		UpdateComboPoints();
	}

	inline void NullComboPoints() { if(!m_retainComboPoints) { m_comboTarget = 0; m_comboPoints = 0; m_spellcomboPoints=0; } UpdateComboPoints(); }
	Unit *GetSoloSpellTarget(uint32 spell_id);
	void  SetSoloSpellTarget(uint32 spellid,uint64 newtarget);
	uint32 m_speedChangeCounter;

	void SendAreaTriggerMessage(const char * message, ...);
	void Set_Mute_on_player(uint32 until);
	void Remove_Mute_on_player();
	uint32 HasMuteOnPlayer(){return chat_disabled_until;};
        
	// Trade Target
	//Player *getTradeTarget() {return mTradeTarget;};

	inline Player * GetTradeTarget()
	{
		if(!IsInWorld()) return 0;
		return m_mapMgr->GetPlayer(mTradeTarget);
	}

	Item *getTradeItem(uint32 slot) {return mTradeItems[slot];};
        
	// Water level related stuff (they are public because they need to be accessed fast)
	// Nose level of the character (needed for proper breathing)
	float m_noseLevel;

	/* Mind Control */
	void Possess(Unit * pTarget);
	void UnPossess();

	/* Last Speeds */
	inline void UpdateLastSpeeds()
	{
		m_lastRunSpeed = m_runSpeed;
		m_lastRunBackSpeed = m_backWalkSpeed;
		m_lastSwimSpeed = m_swimSpeed;
		m_lastRunBackSpeed = m_backSwimSpeed;
		m_lastFlySpeed = m_flySpeed;
	}

	void RemoteRevive()
	{
		ResurrectPlayer();
		SetMovement(MOVE_UNROOT, 5);
		SetPlayerSpeed(RUN, (float)7);
		SetPlayerSpeed(SWIM, (float)4.9);
		SetMovement(MOVE_LAND_WALK, 8);
		SetUInt32Value(UNIT_FIELD_HEALTH, GetUInt32Value(UNIT_FIELD_MAXHEALTH) );
	}

	LocationVector m_last_group_position;
	int32 m_rap_mod_pct;
	void SummonRequest(uint32 Requestor, uint32 ZoneID, uint32 MapID, uint32 InstanceID, const LocationVector & Position);
	uint8 m_lastMoveType;

#ifdef CLUSTERING
	void EventRemoveAndDelete();
#endif

	Creature * m_tempSummon;
	bool m_deathVision;
	SpellEntry * last_heal_spell;
	LocationVector m_sentTeleportPosition;

	void RemoveFromBattlegroundQueue();

    /************************************************************************/
    /* Spell Packet wharper Please keep this separated                      */
    /************************************************************************/
    void SendCastResult(uint32 SpellId, uint8 ErrorMessage, uint32 Extra);
    /************************************************************************/
    /* End of SpellPacket Wharper                                           */
    /************************************************************************/

protected:
	LocationVector m_summonPos;
	uint32 m_summonInstanceId;
	uint32 m_summonMapId;
	uint32 m_summoner;

	uint32 iActivePet;
	void _SetCreateBits(UpdateMask *updateMask, Player *target) const;
	void _SetUpdateBits(UpdateMask *updateMask, Player *target) const;
	void _SetVisibleBits(UpdateMask *updateMask, Player *target) const;
/* Update system components */
	ByteBuffer bUpdateBuffer;
    ByteBuffer bCreationBuffer;
	uint32 mUpdateCount;
    uint32 mCreationCount;
	uint32 mOutOfRangeIdCount;
	ByteBuffer mOutOfRangeIds;
	SplineMap _splineMap;
/* End update system */
	void _LoadTutorials();
	void _SaveTutorials();
	void _SaveInventory(bool firstsave);
	void _LoadBagInventory(uint32 playerguid, uint8 bagslot);
	void _SaveQuestLogEntry();
	void _LoadQuestLogEntry();
	void _LoadInventoryLight();
	// DK
	void _LoadGuild();
	void _LoadPet();
	void _LoadPetNo();
	void _LoadPetSpells();
	void _SavePet();
	void _SavePetSpells();
	void _SaveItemCooldown();
	void _LoadItemCooldown();
	void _SaveSpellCoolDownSecurity();
	void _LoadSpellCoolDownSecurity();
	void _ApplyItemMods(Item *item, int8 slot,bool apply,bool justdrokedown=false);
   
	void _EventAttack(bool offhand);
	void _EventExploration();
	void UpdateCooldowns();
	// Water level related stuff
	void SetNoseLevel();
	// Cooldown stuff
	std::map<uint32,uint32>	 SpellCooldownMap;
	std::map<uint32,uint32>	 SpellCooldownCategoryMap;
	ItemCooldownSet			 m_itemcooldown;
	uint32					 GlobalCooldown;
	/************************************************************************/
	/* Trade																*/
	/************************************************************************/
	Item* mTradeItems[8];
	uint32 mTradeGold;
	uint64 mTradeTarget;
	uint32 mTradeStatus;

    /************************************************************************/
    /* Player Class systems, info and misc things                           */
    /************************************************************************/
    PlayerCreateInfo *info;
	uint32      m_AttackMsgTimer;	// "too far away" and "wrong facing" timer
	bool        m_attacking;
	std::string m_name;	// max 21 character name
	uint32      m_Tutorials[8];

    // Character Ban
	uint32      m_banned;
	string      m_banreason;
	uint32      m_AreaID;
	Pet*        m_Summon;
	uint32      m_PetNumberMax;
	std::map<uint32, PlayerPet*> m_Pets;
	
    uint64      m_invitersGuid; // It is guild inviters guid ,0 when its not used
    

    // bind
	float m_bind_pos_x;
	float m_bind_pos_y;
	float m_bind_pos_z;
	uint32 m_bind_mapid;
	uint32 m_bind_zoneid;
	std::list<ItemSet> m_itemsets;
	//Duel
	uint32 m_duelCountdownTimer;
	uint8 m_duelStatus;
	uint8 m_duelState;
	// Rested State Stuff
	uint32 m_timeLogoff;
	// Played time
	uint32 m_playedtime[3];
	uint8 m_isResting;
	uint8 m_restState;
	uint32 m_restAmount;
	//combat mods
	float m_blockfromspell;
	float m_blockfromspellPCT;
	float m_critfromspell;
	float m_spellcritfromspell;
	float m_dodgefromspell;
	float m_parryfromspell;
	float m_hitfromspell;
	float m_hitfrommeleespell;
	//stats mods
	uint32 m_healthfromspell;
	uint32 m_manafromspell;
	uint32 m_healthfromitems;
	uint32 m_manafromitems;
	
	uint32 armor_proficiency;
	uint32 weapon_proficiency;
	// Talents
	uint32 m_talentresettimes;
	// STATUS
	uint8 m_status;
	// guid of current target
	uint64 m_curTarget;
	// guid of current selection
	uint64 m_curSelection;
	// Raid
	uint8 m_targetIcon;
	//Player Action Bar
	ActionButton mActions[120];
	// Player Reputation
	ReputationMap m_reputation;
	// Pointer to this char's game client
	WorldSession *m_session;
	// Channels
	std::list<Channel*> m_channels;
	// Visible objects
	std::set<Object*> m_visibleObjects;
	// Groups/Raids
	Group* m_Group;
	uint32 m_SubGroup;
	uint64 m_GroupInviter;
	uint8 m_StableSlotCount;

    // Fishing related
	Object *m_SummonedObject;

    // other system
    SoloSpells	solospelltarget;
	Corpse *    myCorpse;

	uint32      m_cooldownTimer;
	uint32      m_lastHonorResetTime;
	uint32      _fields[PLAYER_END];
    uint32	    chat_disabled_until;    //force player to be silent. Yeah i'm pissed of on noobs
	uint32	    trigger_on_stun;        //bah, warrior talent but this will not get triggered on triggered spells if used on proc so i'm forced to used a special variable
	uint32	    trigger_on_stun_chance; //also using this for mage "Frostbite" talent

	uint32 m_team;
	float       m_lastRunSpeed;
	float       m_lastRunBackSpeed;
	float       m_lastSwimSpeed;
	float       m_lastBackSwimSpeed;
	float       m_lastFlySpeed;
};

#endif
