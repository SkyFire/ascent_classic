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
struct GuildRank;
class Pet;
class Charter;
struct LevelInfo;
#define myabs(a) (a<0)?(-a):a
#define MAX_PET_NO 3
#define PLAYER_NORMAL_RUN_SPEED 7.0f
#define PLAYER_NORMAL_SWIM_SPEED 4.722222f
#define PLAYER_NORMAL_FLIGHT_SPEED 7.0f
#define PLAYER_HONORLESS_TARGET_SPELL 2479
#define MONSTER_NORMAL_RUN_SPEED 8.0f
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
	std::set<uint32> spell_list;
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

const double CritFromAgi[74][12] = { // level class
	// - War Paladin Hunter Rogue Priest - Shaman Mage Warlock - Druid
	{0, 0     , 0     , 0     , 0     , 0     , 0, 0     , 0     , 0     , 0, 0      }, // level 0
	{0, 0.2500, 0.2174, 0.2840, 0.4476, 0.0909, 0, 0.1663, 0.0771, 0.1500, 0, 0.2020 },
	{0, 0.2381, 0.2070, 0.2834, 0.4290, 0.0909, 0, 0.1663, 0.0771, 0.1500, 0, 0.2020 },
	{0, 0.2381, 0.2070, 0.2711, 0.4118, 0.0909, 0, 0.1583, 0.0771, 0.1429, 0, 0.1923 },
	{0, 0.2273, 0.1976, 0.2530, 0.3813, 0.0865, 0, 0.1583, 0.0735, 0.1429, 0, 0.1923 },
	{0, 0.2174, 0.1976, 0.2430, 0.3677, 0.0865, 0, 0.1511, 0.0735, 0.1429, 0, 0.1836 },
	{0, 0.2083, 0.1890, 0.2337, 0.3550, 0.0865, 0, 0.1511, 0.0735, 0.1364, 0, 0.1836 },
	{0, 0.2083, 0.1890, 0.2251, 0.3321, 0.0865, 0, 0.1511, 0.0735, 0.1364, 0, 0.1756 },
	{0, 0.2000, 0.1812, 0.2171, 0.3217, 0.0826, 0, 0.1446, 0.0735, 0.1364, 0, 0.1756 },
	{0, 0.1923, 0.1812, 0.2051, 0.3120, 0.0826, 0, 0.1446, 0.0735, 0.1304, 0, 0.1683 },
	{0, 0.1923, 0.1739, 0.1984, 0.2941, 0.0826, 0, 0.1385, 0.0701, 0.1304, 0, 0.1553 },
	{0, 0.1852, 0.1739, 0.1848, 0.2640, 0.0826, 0, 0.1385, 0.0701, 0.1250, 0, 0.1496 },
	{0, 0.1786, 0.1672, 0.1670, 0.2394, 0.0790, 0, 0.1330, 0.0701, 0.1250, 0, 0.1496 },
	{0, 0.1667, 0.1553, 0.1547, 0.2145, 0.0790, 0, 0.1330, 0.0701, 0.1250, 0, 0.1443 },
	{0, 0.1613, 0.1553, 0.1441, 0.1980, 0.0790, 0, 0.1279, 0.0701, 0.1200, 0, 0.1443 },
	{0, 0.1563, 0.1449, 0.1330, 0.1775, 0.0790, 0, 0.1231, 0.0671, 0.1154, 0, 0.1346 },
	{0, 0.1515, 0.1449, 0.1267, 0.1660, 0.0757, 0, 0.1188, 0.0671, 0.1111, 0, 0.1346 },
	{0, 0.1471, 0.1403, 0.1194, 0.1560, 0.0757, 0, 0.1188, 0.0671, 0.1111, 0, 0.1303 },
	{0, 0.1389, 0.1318, 0.1117, 0.1450, 0.0757, 0, 0.1147, 0.0671, 0.1111, 0, 0.1262 },
	{0, 0.1351, 0.1318, 0.1060, 0.1355, 0.0727, 0, 0.1147, 0.0671, 0.1071, 0, 0.1262 },
	{0, 0.1282, 0.1242, 0.0998, 0.1271, 0.0727, 0, 0.1073, 0.0643, 0.1034, 0, 0.1122 },
	{0, 0.1282, 0.1208, 0.0962, 0.1197, 0.0727, 0, 0.1073, 0.0643, 0.1000, 0, 0.1122 },
	{0, 0.1250, 0.1208, 0.0910, 0.1144, 0.0727, 0, 0.1039, 0.0643, 0.1000, 0, 0.1092 },
	{0, 0.1190, 0.1144, 0.0872, 0.1084, 0.0699, 0, 0.1039, 0.0643, 0.0968, 0, 0.1063 },
	{0, 0.1163, 0.1115, 0.0829, 0.1040, 0.0699, 0, 0.1008, 0.0617, 0.0968, 0, 0.1063 },
	{0, 0.1111, 0.1087, 0.0797, 0.0980, 0.0699, 0, 0.0978, 0.0617, 0.0909, 0, 0.1010 },
	{0, 0.1087, 0.1060, 0.0767, 0.0936, 0.0673, 0, 0.0950, 0.0617, 0.0909, 0, 0.1010 },
	{0, 0.1064, 0.1035, 0.0734, 0.0903, 0.0673, 0, 0.0950, 0.0617, 0.0909, 0, 0.0985 },
	{0, 0.1020, 0.1011, 0.0709, 0.0865, 0.0673, 0, 0.0924, 0.0617, 0.0882, 0, 0.0962 },
	{0, 0.1000, 0.0988, 0.0680, 0.0830, 0.0649, 0, 0.0924, 0.0593, 0.0882, 0, 0.0962 },
	{0, 0.0962, 0.0945, 0.0654, 0.0792, 0.0649, 0, 0.0875, 0.0593, 0.0833, 0, 0.0878 },
	{0, 0.0943, 0.0925, 0.0637, 0.0768, 0.0649, 0, 0.0875, 0.0593, 0.0833, 0, 0.0859 },
	{0, 0.0926, 0.0925, 0.0614, 0.0741, 0.0627, 0, 0.0853, 0.0593, 0.0811, 0, 0.0859 },
	{0, 0.0893, 0.0887, 0.0592, 0.0715, 0.0627, 0, 0.0831, 0.0571, 0.0811, 0, 0.0841 },
	{0, 0.0877, 0.0870, 0.0575, 0.0691, 0.0627, 0, 0.0831, 0.0571, 0.0789, 0, 0.0824 },
	{0, 0.0847, 0.0836, 0.0556, 0.0664, 0.0606, 0, 0.0792, 0.0571, 0.0769, 0, 0.0808 },
	{0, 0.0833, 0.0820, 0.0541, 0.0643, 0.0606, 0, 0.0773, 0.0551, 0.0750, 0, 0.0792 },
	{0, 0.0820, 0.0820, 0.0524, 0.0628, 0.0606, 0, 0.0773, 0.0551, 0.0732, 0, 0.0777 },
	{0, 0.0794, 0.0791, 0.0508, 0.0609, 0.0586, 0, 0.0756, 0.0551, 0.0732, 0, 0.0777 },
	{0, 0.0781, 0.0776, 0.0493, 0.0592, 0.0586, 0, 0.0756, 0.0551, 0.0714, 0, 0.0762 },
	{0, 0.0758, 0.0750, 0.0481, 0.0572, 0.0586, 0, 0.0723, 0.0532, 0.0698, 0, 0.0709 },
	{0, 0.0735, 0.0737, 0.0470, 0.0556, 0.0568, 0, 0.0707, 0.0532, 0.0682, 0, 0.0696 },
	{0, 0.0725, 0.0737, 0.0457, 0.0542, 0.0568, 0, 0.0707, 0.0532, 0.0682, 0, 0.0696 },
	{0, 0.0704, 0.0713, 0.0444, 0.0528, 0.0551, 0, 0.0693, 0.0532, 0.0667, 0, 0.0685 },
	{0, 0.0694, 0.0701, 0.0433, 0.0512, 0.0551, 0, 0.0679, 0.0514, 0.0667, 0, 0.0673 },
	{0, 0.0676, 0.0679, 0.0421, 0.0497, 0.0551, 0, 0.0665, 0.0514, 0.0638, 0, 0.0651 },
	{0, 0.0667, 0.0669, 0.0413, 0.0486, 0.0534, 0, 0.0652, 0.0514, 0.0625, 0, 0.0641 },
	{0, 0.0649, 0.0659, 0.0402, 0.0474, 0.0534, 0, 0.0639, 0.0498, 0.0625, 0, 0.0641 },
	{0, 0.0633, 0.0639, 0.0391, 0.0464, 0.0519, 0, 0.0627, 0.0498, 0.0612, 0, 0.0631 },
	{0, 0.0625, 0.0630, 0.0382, 0.0454, 0.0519, 0, 0.0627, 0.0498, 0.0600, 0, 0.0621 },
	{0, 0.0610, 0.0612, 0.0373, 0.0440, 0.0519, 0, 0.0605, 0.0482, 0.0588, 0, 0.0585 },
	{0, 0.0595, 0.0604, 0.0366, 0.0431, 0.0505, 0, 0.0594, 0.0482, 0.0577, 0, 0.0577 },
	{0, 0.0588, 0.0596, 0.0358, 0.0422, 0.0505, 0, 0.0583, 0.0482, 0.0577, 0, 0.0569 },
	{0, 0.0575, 0.0580, 0.0350, 0.0412, 0.0491, 0, 0.0583, 0.0467, 0.0566, 0, 0.0561 },
	{0, 0.0562, 0.0572, 0.0341, 0.0404, 0.0491, 0, 0.0573, 0.0467, 0.0556, 0, 0.0561 },
	{0, 0.0549, 0.0557, 0.0334, 0.0394, 0.0478, 0, 0.0554, 0.0467, 0.0545, 0, 0.0546 },
	{0, 0.0543, 0.0550, 0.0328, 0.0386, 0.0478, 0, 0.0545, 0.0454, 0.0536, 0, 0.0539 },
	{0, 0.0532, 0.0544, 0.0321, 0.0378, 0.0466, 0, 0.0536, 0.0454, 0.0526, 0, 0.0531 },
	{0, 0.0521, 0.0530, 0.0314, 0.0370, 0.0466, 0, 0.0536, 0.0454, 0.0517, 0, 0.0525 },
	{0, 0.0510, 0.0524, 0.0307, 0.0364, 0.0454, 0, 0.0528, 0.0441, 0.0517, 0, 0.0518 },
	{0, 0.0500, 0.0512, 0.0301, 0.0355, 0.0454, 0, 0.0512, 0.0441, 0.0500, 0, 0.0493 },
	{0, 0.0469, 0.0491, 0.0297, 0.0334, 0.0443, 0, 0.0496, 0.0435, 0.0484, 0, 0.0478 },
	{0, 0.0442, 0.0483, 0.0290, 0.0322, 0.0444, 0, 0.0486, 0.0432, 0.0481, 0, 0.0472 },
	{0, 0.0418, 0.0472, 0.0284, 0.0307, 0.0441, 0, 0.0470, 0.0424, 0.0470, 0, 0.0456 },
	{0, 0.0397, 0.0456, 0.0279, 0.0296, 0.0433, 0, 0.0456, 0.0423, 0.0455, 0, 0.0447 },
	{0, 0.0377, 0.0446, 0.0273, 0.0286, 0.0426, 0, 0.0449, 0.0422, 0.0448, 0, 0.0438 },
	{0, 0.0360, 0.0437, 0.0270, 0.0276, 0.0419, 0, 0.0437, 0.0411, 0.0435, 0, 0.0430 },
	{0, 0.0344, 0.0425, 0.0264, 0.0268, 0.0414, 0, 0.0427, 0.0412, 0.0436, 0, 0.0424 },
	{0, 0.0329, 0.0416, 0.0259, 0.0262, 0.0412, 0, 0.0417, 0.0408, 0.0424, 0, 0.0412 },
	{0, 0.0315, 0.0408, 0.0254, 0.0256, 0.0410, 0, 0.0408, 0.0404, 0.0414, 0, 0.0406 },
	{0, 0.0303, 0.0400, 0.0250, 0.0250, 0.0400, 0, 0.0400, 0.0400, 0.0405, 0, 0.0400 }, // level 70
	{0, 0.0297, 0.0393, 0.0246, 0.0244, 0.0390, 0, 0.0392, 0.0396, 0.0396, 0, 0.0394 },
	{0, 0.0292, 0.0385, 0.0242, 0.0238, 0.0381, 0, 0.0384, 0.0393, 0.0387, 0, 0.0388 },
	{0, 0.0287, 0.0378, 0.0238, 0.0233, 0.0372, 0, 0.0377, 0.0389, 0.0379, 0, 0.0383 },
};

const double SpellCritFromInt[74][12] = { //[level][class]
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
	ASCENT_INLINE int32 CalcStanding() { return standing - baseStanding; }
	ASCENT_INLINE bool Positive() { return standing >= 0; }
	FactionRating CalcRating()
	{		
		//changed by zack : nocturno patch : items bougth from Quartermasters relly on full standing value
//		register long st=CalcStanding();
		register long st=standing;
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
	~PlayerInfo();
	uint32 guid;
	uint32 acct;
	char * name;
	uint32 race;
	uint32 gender;
	uint32 cl;
	uint32 team;
	
	time_t lastOnline;
	uint32 lastZone;
	uint32 lastLevel;
	Group * m_Group;
	int8 subGroup;

	Player * m_loggedInPlayer;
	Guild * guild;
	GuildRank * guildRank;
	GuildMember * guildMember;
};
struct PlayerPet
{
	string name;
	uint32 entry;
	string fields;
	uint32 xp;
	bool active;
	char stablestate;
	uint32 number;
	uint32 level;
	uint32 happiness;
	uint32 happinessupdate;
	string actionbar;
	bool summon;
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

struct SSAura
{
	uint32 spellid;
	uint32 forms;
};

enum SPELL_INDEX
{
	SPELL_TYPE_INDEX_MARK			= 1,
	SPELL_TYPE_INDEX_POLYMORPH		= 2,
	SPELL_TYPE_INDEX_FEAR			= 3,
	SPELL_TYPE_INDEX_SAP			= 4,
	SPELL_TYPE_INDEX_SCARE_BEAST	= 5,
	SPELL_TYPE_INDEX_HIBERNATE		= 6,
	SPELL_TYPE_INDEX_EARTH_SHIELD	= 7,
	SPELL_TYPE_INDEX_CYCLONE		= 8,
	SPELL_TYPE_INDEX_BANISH			= 9,
	NUM_SPELL_TYPE_INDEX			= 10,
};

#define PLAYER_RATING_MODIFIER_RANGED_SKILL						PLAYER_FIELD_COMBAT_RATING_1
#define PLAYER_RATING_MODIFIER_DEFENCE							PLAYER_FIELD_COMBAT_RATING_01
#define PLAYER_RATING_MODIFIER_DODGE							PLAYER_FIELD_COMBAT_RATING_02
#define PLAYER_RATING_MODIFIER_PARRY							PLAYER_FIELD_COMBAT_RATING_03
#define PLAYER_RATING_MODIFIER_BLOCK							PLAYER_FIELD_COMBAT_RATING_04
#define PLAYER_RATING_MODIFIER_MELEE_HIT						PLAYER_FIELD_COMBAT_RATING_05
#define PLAYER_RATING_MODIFIER_RANGED_HIT						PLAYER_FIELD_COMBAT_RATING_06
#define PLAYER_RATING_MODIFIER_SPELL_HIT						PLAYER_FIELD_COMBAT_RATING_07
#define PLAYER_RATING_MODIFIER_MELEE_CRIT						PLAYER_FIELD_COMBAT_RATING_08
#define PLAYER_RATING_MODIFIER_RANGED_CRIT						PLAYER_FIELD_COMBAT_RATING_09
#define PLAYER_RATING_MODIFIER_SPELL_CRIT						PLAYER_FIELD_COMBAT_RATING_10
#define PLAYER_RATING_MODIFIER_MELEE_HIT_AVOIDANCE				PLAYER_FIELD_COMBAT_RATING_11 // GUESSED
#define PLAYER_RATING_MODIFIER_RANGED_HIT_AVOIDANCE				PLAYER_FIELD_COMBAT_RATING_12 // GUESSED
#define PLAYER_RATING_MODIFIER_SPELL_HIT_AVOIDANCE				PLAYER_FIELD_COMBAT_RATING_13 // GUESSED
#define PLAYER_RATING_MODIFIER_MELEE_CRIT_RESILIENCE			PLAYER_FIELD_COMBAT_RATING_14
#define PLAYER_RATING_MODIFIER_RANGED_CRIT_RESILIENCE			PLAYER_FIELD_COMBAT_RATING_15
#define PLAYER_RATING_MODIFIER_SPELL_CRIT_RESILIENCE			PLAYER_FIELD_COMBAT_RATING_16
#define PLAYER_RATING_MODIFIER_MELEE_HASTE						PLAYER_FIELD_COMBAT_RATING_17
#define PLAYER_RATING_MODIFIER_RANGED_HASTE						PLAYER_FIELD_COMBAT_RATING_18
#define PLAYER_RATING_MODIFIER_SPELL_HASTE						PLAYER_FIELD_COMBAT_RATING_19
#define PLAYER_RATING_MODIFIER_MELEE_MAIN_HAND_SKILL			PLAYER_FIELD_COMBAT_RATING_20
#define PLAYER_RATING_MODIFIER_MELEE_OFF_HAND_SKILL				PLAYER_FIELD_COMBAT_RATING_21
// #define UNKNOWN												PLAYER_FIELD_COMBAT_RATING_22
// #define UNKNOWN												PLAYER_FIELD_COMBAT_RATING_23

class ArenaTeam;
//====================================================================
//  Player
//  Class that holds every created character on the server.
//
//  TODO:  Attach characters to user accounts
//====================================================================
typedef std::set<uint32>	                        SpellSet;
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

//#define OPTIMIZED_PLAYER_SAVING

class SERVER_DECL Player : public Unit
{
	friend class WorldSession;
	friend class Pet;
	friend class SkillIterator;

protected:
	SkillMap m_skills;

public:

	ASCENT_INLINE Guild * GetGuild() { return m_playerInfo->guild; }
	ASCENT_INLINE GuildMember * GetGuildMember() { return m_playerInfo->guildMember; }
	ASCENT_INLINE GuildRank * GetGuildRankS() { return m_playerInfo->guildRank; }

	void EventGroupFullUpdate();

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


	void RecalculateHonor();

protected:
	void _UpdateSkillFields();
    
public:
	Player ( uint32 high, uint32 low );
	~Player ( );
	bool ok_to_remove;
	PlayerInfo * m_playerInfo;
	uint64 m_spellIndexTypeTargets[NUM_SPELL_TYPE_INDEX];
	void OnLogin();//custom stuff on player login.
	void RemoveSpellTargets(uint32 Type);
	void RemoveSpellIndexReferences(uint32 Type);
	void SetSpellTargetType(uint32 Type, Unit* target);

	void AddToWorld();
	void AddToWorld(MapMgr * pMapMgr);
	void RemoveFromWorld();
	bool Create ( WorldPacket &data );
	
	void Update( uint32 time );
	void BuildEnumData( WorldPacket * p_data );
    void BuildFlagUpdateForNonGroupSet(uint32 index, uint32 flag);
	std::string m_afk_reason;
	void SetAFKReason(std::string reason) { m_afk_reason = reason; };
	ASCENT_INLINE const char* GetName() { return m_name.c_str(); }
	ASCENT_INLINE std::string* GetNameString() { return &m_name; }
	void Die();
	//void KilledMonster(uint32 entry, const uint64 &guid);
	void GiveXP(uint32 xp, const uint64 &guid, bool allowbonus);   // to stop rest xp being given
	void ModifyBonuses(uint32 type,int32 val);
	std::map<uint32, uint32> m_wratings;

	ArenaTeam * m_arenaTeams[NUM_ARENA_TEAM_TYPES];
	
    /************************************************************************/
    /* Taxi                                                                 */
    /************************************************************************/
    ASCENT_INLINE TaxiPath*    GetTaxiPath() { return m_CurrentTaxiPath; }
    ASCENT_INLINE bool         GetTaxiState() { return m_onTaxi; }
    const uint32&       GetTaximask( uint8 index ) const { return m_taximask[index]; }
    void                LoadTaxiMask(const char* data);
    void                TaxiStart(TaxiPath* path, uint32 modelid, uint32 start_node);
    void                JumpToEndTaxiNode(TaxiPath * path);
    void                EventDismount(uint32 money, float x, float y, float z);
    void                EventTaxiInterpolate();

    ASCENT_INLINE void         SetTaxiState    (bool state) { m_onTaxi = state; }
    ASCENT_INLINE void         SetTaximask     (uint8 index, uint32 value ) { m_taximask[index] = value; }
    ASCENT_INLINE void         SetTaxiPath     (TaxiPath *path) { m_CurrentTaxiPath = path; }
    ASCENT_INLINE void         SetTaxiPos()	{m_taxi_pos_x = m_position.x; m_taxi_pos_y = m_position.y; m_taxi_pos_z = m_position.z;}
    ASCENT_INLINE void         UnSetTaxiPos()	{m_taxi_pos_x = 0; m_taxi_pos_y = 0; m_taxi_pos_z = 0; }
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
	ASCENT_INLINE QuestLogEntry*GetQuestLogInSlot(uint32 slot)  { return m_questlog[slot]; }
    ASCENT_INLINE uint32        GetQuestSharer()                { return m_questSharer; }
    
    ASCENT_INLINE void         SetQuestSharer(uint32 guid)     { m_questSharer = guid; }
    void                SetQuestLogSlot(QuestLogEntry *entry, uint32 slot);
    
    ASCENT_INLINE void         PushToRemovedQuests(uint32 questid)	{ m_removequests.insert(questid);}
    void                AddToFinishedQuests(uint32 quest_id);
    void                EventTimedQuestExpire(Quest *qst, QuestLogEntry *qle, uint32 log_slot);
	
	bool                HasFinishedQuest(uint32 quest_id);
	bool                HasQuestForItem(uint32 itemid);
    bool                CanFinishQuest(Quest* qst);
	bool                HasQuestSpell(uint32 spellid);
	void                RemoveQuestSpell(uint32 spellid);
	bool                HasQuestMob(uint32 entry);
	void                RemoveQuestMob(uint32 entry);

    //Quest related variables
	uint32 m_questbarrier1[25];
    QuestLogEntry*      m_questlog[25];
	uint32 m_questbarrier2[25];
    std::set<uint32>    m_QuestGOInProgress;
    std::set<uint32>    m_removequests;
    std::set<uint32>    m_finishedQuests;
    uint32              m_questSharer;
    uint32              timed_quest_slot;
	std::set<uint32>    quest_spells;
	std::set<uint32>    quest_mobs;

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
	ASCENT_INLINE uint32 GetTeam() { return m_team; }
	ASCENT_INLINE void SetTeam(uint32 t) { m_team = t; m_bgTeam=t; }
	ASCENT_INLINE void ResetTeam() { m_team = myRace->team_id==7 ? 0 : 1; m_bgTeam=m_team; }

	ASCENT_INLINE bool IsInFeralForm()
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
	std::set<SSAura*>    m_ssAuras;


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
	ASCENT_INLINE uint8 GetPVPRank()
	{
		return (uint8)((GetUInt32Value(PLAYER_BYTES_3) >> 24) & 0xFF);
	}
	ASCENT_INLINE void SetPVPRank(int newrank)
	{
		SetUInt32Value(PLAYER_BYTES_3, ((GetUInt32Value(PLAYER_BYTES_3) & 0x00FFFFFF) | (uint8(newrank) << 24)));
	}

    /************************************************************************/
    /* Groups                                                               */
    /************************************************************************/
	void                SetInviter(uint32 pInviter) { m_GroupInviter = pInviter; }
	ASCENT_INLINE uint32       GetInviter() { return m_GroupInviter; }
	ASCENT_INLINE bool         InGroup() { return (m_playerInfo->m_Group != NULL && !m_GroupInviter); }
	bool                IsGroupLeader()
	{
		if(m_playerInfo->m_Group != NULL)
		{
			if(m_playerInfo->m_Group->GetLeader() == m_playerInfo)
				return true;
		}
		return false;
	}
	ASCENT_INLINE int          HasBeenInvited() { return m_GroupInviter != 0; }
	ASCENT_INLINE Group*       GetGroup() { return m_playerInfo->m_Group; }
	ASCENT_INLINE int8		   GetSubGroup() { return m_playerInfo->subGroup; }
    bool                IsGroupMember(Player *plyr);
	ASCENT_INLINE bool         IsBanned()
	{
		if(m_banned)
		{
			if(m_banned < 100 || (uint32)UNIXTIME < m_banned)
				return true;
		}
		return false;
	}
    ASCENT_INLINE void         SetBanned() { m_banned = 4;}
	ASCENT_INLINE void         SetBanned(string Reason) { m_banned = 4; m_banreason = Reason;}
	ASCENT_INLINE void         SetBanned(uint32 timestamp, string& Reason) { m_banned = timestamp; m_banreason = Reason; }
	ASCENT_INLINE void         UnSetBanned() { m_banned = 0; }
	ASCENT_INLINE string       GetBanReason() {return m_banreason;}

    /************************************************************************/
    /* Guilds                                                               */
    /************************************************************************/
	ASCENT_INLINE  bool        IsInGuild() {return (m_uint32Values[PLAYER_GUILDID] != 0) ? true : false;}
	ASCENT_INLINE uint32       GetGuildId() { return m_uint32Values[PLAYER_GUILDID]; }
	void                SetGuildId(uint32 guildId);
	ASCENT_INLINE uint32       GetGuildRank() { return m_uint32Values[PLAYER_GUILDRANK]; }
	void                SetGuildRank(uint32 guildRank);
	uint32              GetGuildInvitersGuid() { return m_invitersGuid; }
	void                SetGuildInvitersGuid( uint32 guid ) { m_invitersGuid = guid; }
	void                UnSetGuildInvitersGuid() { m_invitersGuid = 0; }
  
    /************************************************************************/
    /* Duel                                                                 */
    /************************************************************************/
    void                RequestDuel(Player *pTarget);
	void                DuelBoundaryTest();
	void                EndDuel(uint8 WinCondition);
	void                DuelCountdown();
	void                SetDuelStatus(uint8 status) { m_duelStatus = status; }
	ASCENT_INLINE uint8        GetDuelStatus() { return m_duelStatus; }
	void                SetDuelState(uint8 state) { m_duelState = state; }
	ASCENT_INLINE uint8        GetDuelState() { return m_duelState; }
    // duel variables
    Player*             DuelingWith;

    /************************************************************************/
    /* Trade                                                                */
    /************************************************************************/
	void                SendTradeUpdate(void);
	void         ResetTradeVariables()
	{
		mTradeGold = 0;
		memset(&mTradeItems, 0, sizeof(Item*) * 8);
		mTradeStatus = 0;
		mTradeTarget = 0;
	}
	
    /************************************************************************/
    /* Pets                                                                 */
    /************************************************************************/
	ASCENT_INLINE void         SetSummon(Pet *pet) { m_Summon = pet; }
	ASCENT_INLINE Pet*         GetSummon(void) { return m_Summon; }
	uint32              GeneratePetNumber(void);
	void                RemovePlayerPet(uint32 pet_number);
	ASCENT_INLINE void         AddPlayerPet(PlayerPet* pet, uint32 index) { m_Pets[index] = pet; }
	ASCENT_INLINE PlayerPet*   GetPlayerPet(uint32 idx)
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
	ASCENT_INLINE PlayerPet*   GetFirstPet(void) { return GetPlayerPet(GetFirstPetNumber()); }
	ASCENT_INLINE void         SetStableSlotCount(uint8 count) { m_StableSlotCount = count; }
	ASCENT_INLINE uint8        GetStableSlotCount(void) { return m_StableSlotCount; }
	uint32              GetUnstabledPetNumber(void)
	{
		if(m_Pets.size() == 0) return 0;
		std::map<uint32, PlayerPet*>::iterator itr = m_Pets.begin();
		for(;itr != m_Pets.end();itr++)
			if(itr->second->stablestate == STABLE_STATE_ACTIVE)
				return itr->first;
		return 0;
	}

    /************************************************************************/
    /* Item Interface                                                       */
    /************************************************************************/
	ASCENT_INLINE ItemInterface* GetItemInterface() { return m_ItemInterface; } // Player Inventory Item storage
	ASCENT_INLINE void         ApplyItemMods(Item *item, int8 slot, bool apply,bool justdrokedown=false) {  _ApplyItemMods(item, slot, apply,justdrokedown); }
    // item interface variables
    ItemInterface *     m_ItemInterface;
	
    /************************************************************************/
    /* Loot                                                                 */
    /************************************************************************/
	ASCENT_INLINE const uint64& GetLootGUID() const { return m_lootGuid; }
	ASCENT_INLINE void         SetLootGUID(const uint64 &guid) { m_lootGuid = guid; }
	void                SendLoot(uint64 guid,uint8 loot_type);
    // loot variables
    uint64              m_lootGuid;
    uint64              m_currentLoot;
    bool                bShouldHaveLootableOnCorpse;

    /************************************************************************/
    /* World Session                                                        */
    /************************************************************************/
	ASCENT_INLINE WorldSession* GetSession() const { return m_session; }
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
	void SetTalentHearthOfWildPCT(int value){hearth_of_wild_pct=value;}
	void EventTalentHearthOfWildChange(bool apply);
	
	std::list<LoginAura> loginauras;

    std::set<uint32> OnMeleeAuras;

    /************************************************************************/
    /* Player loading and savings                                           */
    /* Serialize character to db                                            */
    /************************************************************************/
	void SaveToDB(bool bNewCharacter);
	void SaveAuras(stringstream&);
	bool LoadFromDB(uint32 guid);
	void LoadFromDBProc(QueryResultVector & results);

	void LoadNamesFromDB(uint32 guid);
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
	//ASCENT_INLINE std::list<struct skilllines>getSkillLines() { return m_skilllines; }
	float SpellCrtiticalStrikeRatingBonus;
	float SpellHasteRatingBonus;
	void UpdateAttackSpeed();
	void UpdateChances();
	void UpdateStats();
	void UpdateHit(int32 hit);
   
	bool canCast(SpellEntry *m_spellInfo);
	ASCENT_INLINE float GetBlockFromSpell() { return m_blockfromspell; }
	ASCENT_INLINE float GetSpellCritFromSpell() { return m_spellcritfromspell; }
	ASCENT_INLINE float GetHitFromMeleeSpell() { return m_hitfrommeleespell; }
	ASCENT_INLINE float GetHitFromSpell() { return m_hitfromspell; }
	ASCENT_INLINE float GetParryFromSpell() { return m_parryfromspell; }
	ASCENT_INLINE float GetDodgeFromSpell() { return m_dodgefromspell; }
	void SetBlockFromSpell(float value) { m_blockfromspell = value; }
	void SetSpellCritFromSpell(float value) { m_spellcritfromspell = value; }
	void SetParryFromSpell(float value) { m_parryfromspell = value; }
	void SetDodgeFromSpell(float value) { m_dodgefromspell = value; }
	void SetHitFromMeleeSpell(float value) { m_hitfrommeleespell = value; }
	void SetHitFromSpell(float value) { m_hitfromspell = value; }
	ASCENT_INLINE uint32 GetHealthFromSpell() { return m_healthfromspell; }
	ASCENT_INLINE uint32 GetManaFromSpell() { return m_manafromspell; }
	void SetHealthFromSpell(uint32 value) { m_healthfromspell = value;}
	void SetManaFromSpell(uint32 value) { m_manafromspell = value;}
	uint32 CalcTalentResetCost(uint32 resetnum);
	void SendTalentResetConfirm();
	uint32 GetTalentResetTimes() { return m_talentresettimes; }
	ASCENT_INLINE void SetTalentResetTimes(uint32 value) { m_talentresettimes = value; }
	void SetPlayerStatus(uint8 pStatus) { m_status = pStatus; }
	ASCENT_INLINE uint8 GetPlayerStatus() { return m_status; }
	const float& GetBindPositionX( ) const { return m_bind_pos_x; }
	const float& GetBindPositionY( ) const { return m_bind_pos_y; }
	const float& GetBindPositionZ( ) const { return m_bind_pos_z; }
	const uint32& GetBindMapId( ) const { return m_bind_mapid; }
	const uint32& GetBindZoneId( ) const { return m_bind_zoneid; }
	ASCENT_INLINE uint8 GetShapeShift()
	{
		return GetByte(UNIT_FIELD_BYTES_1,2);
	}

	
	void delayAttackTimer(int32 delay)
	{
		if(!delay)
			return;

		m_attackTimer += delay;
		m_attackTimer_1 += delay;
	}
	
	void SetShapeShift(uint8 ss);

	uint32 m_furorChance;

    //Showing Units WayPoints
	AIInterface* waypointunit;
	
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
	ASCENT_INLINE bool IsVisible(Object* pObj) { return !(m_visibleObjects.find(pObj) == m_visibleObjects.end()); }
	void AddInRangeObject(Object* pObj);
	void OnRemoveInRangeObject(Object* pObj);
	void ClearInRangeSet();
	ASCENT_INLINE void AddVisibleObject(Object* pObj) { m_visibleObjects.insert(pObj); }
	ASCENT_INLINE void RemoveVisibleObject(Object* pObj) { m_visibleObjects.erase(pObj); }
	ASCENT_INLINE void RemoveVisibleObject(InRangeSet::iterator itr) { m_visibleObjects.erase(itr); }
	ASCENT_INLINE InRangeSet::iterator FindVisible(Object * obj) { return m_visibleObjects.find(obj); }
	ASCENT_INLINE void RemoveIfVisible(Object * obj)
	{
		InRangeSet::iterator itr = m_visibleObjects.find(obj);
		if(itr == m_visibleObjects.end())
			return;

		m_visibleObjects.erase(obj);
		PushOutOfRange(obj->GetNewGUID());
	}

	ASCENT_INLINE bool GetVisibility(Object * obj, InRangeSet::iterator *itr)
	{
		*itr = m_visibleObjects.find(obj);
		return ((*itr) != m_visibleObjects.end());
	}

	ASCENT_INLINE InRangeSet::iterator GetVisibleSetBegin() { return m_visibleObjects.begin(); }
	ASCENT_INLINE InRangeSet::iterator GetVisibleSetEnd() { return m_visibleObjects.end(); }
	
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
	uint32 m_modblockabsorbvalue;
	uint32 m_modblockvaluefromspells;
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
	int32 CanShootRangedWeapon(uint32 spellid, Unit *target, bool autoshot);
	uint32 m_AutoShotDuration;
	uint32 m_AutoShotAttackTimer;
	bool m_onAutoShot;
	uint64 m_AutoShotTarget;
	SpellEntry *m_AutoShotSpell;
	void _InitialReputation();
	void EventActivateGameObject(GameObject* obj);
	void EventDeActivateGameObject(GameObject* obj);
	void UpdateNearbyGameObjects();
	
	void CalcResistance(uint32 type);
	ASCENT_INLINE float res_M_crit_get(){return m_resist_critical[0];}
	ASCENT_INLINE void res_M_crit_set(float newvalue){m_resist_critical[0]=newvalue;}
	ASCENT_INLINE float res_R_crit_get(){return m_resist_critical[1];}
	ASCENT_INLINE void res_R_crit_set(float newvalue){m_resist_critical[1]=newvalue;}
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
	int32 m_ModInterrMRegen;
	float m_RegenManaOnSpellResist;
	uint32 m_casted_amount[7]; //Last casted spells amounts. Need for some spells. Like Ignite etc. DOesn't count HoTs and DoTs. Only directs
	
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
	ASCENT_INLINE uint32* GetPlayedtime() { return m_playedtime; };
	void CalcStat(uint32 t);
	float CalcRating(uint32 t);
	void RecalcAllRatings();
	void RegenerateMana(bool is_interrupted);
	void RegenerateHealth(bool inCombat);
	void RegenerateEnergy();
	void LooseRage();
	
    uint32 SoulStone;
	uint32 SoulStoneReceiver;
	void removeSoulStone();

    ASCENT_INLINE uint32 GetSoulStoneReceiver(){return SoulStoneReceiver;}
    ASCENT_INLINE void SetSoulStoneReceiver(uint32 StoneGUID){SoulStoneReceiver = StoneGUID;}
    ASCENT_INLINE uint32 GetSoulStone(){return SoulStone;}
    ASCENT_INLINE void SetSoulStone(uint32 StoneID){SoulStone = StoneID;}

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
	
#ifndef CLUSTERING
	void _Relocate(uint32 mapid,const LocationVector & v, bool sendpending, bool force_new_world, uint32 instance_id);
#else
	void RelocateCallback(uint32 instance_were_going_to);
#endif
	void AddItemsToWorld();
	void RemoveItemsFromWorld();
	
	uint32 m_ShapeShifted;
	uint32 m_MountSpellId;
    
	ASCENT_INLINE bool IsMounted() {return (m_MountSpellId!=0 ? true : false); }
	
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
	void ResetAllCooldowns();
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
	time_t _heartBeatDisabledUntil;
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
	ASCENT_INLINE uint32 GetAreaID() { return m_AreaID; }
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
	uint32 m_honorPointsToAdd;
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
	ASCENT_INLINE void setMyCorpse(Corpse * corpse) { myCorpse = corpse; }
	ASCENT_INLINE Corpse * getMyCorpse() { return myCorpse; }
	bool bCorpseCreateable;
	uint32 m_resurrectHealth, m_resurrectMana;
	uint32 resurrector;
	bool blinked;
	uint16 m_speedhackChances;
	uint32 m_explorationTimer;
	// DBC stuff
	CharRaceEntry * myRace;
	CharClassEntry * myClass;
	Unit * linkTarget;
	bool stack_cheat;
	bool triggerpass_cheat;
	bool SafeTeleport(uint32 MapID, uint32 InstanceID, float X, float Y, float Z, float O);
	bool SafeTeleport(uint32 MapID, uint32 InstanceID, const LocationVector & vec);
	void SafeTeleport(MapMgr * mgr, const LocationVector & vec);
	void EjectFromInstance();
	bool raidgrouponlysent;
	
	void EventSafeTeleport(uint32 MapID, uint32 InstanceID, LocationVector vec)
	{
		SafeTeleport(MapID, InstanceID, vec);
	}

	/*****************
	  PVP Stuff
	******************/
	uint32 m_pvpTimer;
	
	//! Is PVP flagged?
	ASCENT_INLINE bool IsPvPFlagged() { return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP); }
	ASCENT_INLINE void SetPvPFlag()
	{
		// reset the timer as well..
		StopPvPTimer();
		SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP);
	}
	//! Removal
	ASCENT_INLINE void RemovePvPFlag()
	{
		StopPvPTimer();
		RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP);
	}
	//! Do this on /pvp off
	ASCENT_INLINE void ResetPvPTimer();
	//! Stop the timer for pvp off
	ASCENT_INLINE void StopPvPTimer() { m_pvpTimer = 0; }
	
	//! Called at login to add the honorless buff, etc.
	void LoginPvPSetup();
	//! Update our pvp area (called when zone changes)
	void UpdatePvPArea();
	//! PvP Toggle (called on /pvp)
	void PvPToggle();
	bool CanCastDueToCooldown(SpellEntry * spellid);
	bool CanCastItemDueToCooldown(ItemPrototype * pProto, uint32 x);
	ASCENT_INLINE uint32 LastHonorResetTime() const { return m_lastHonorResetTime; }
	ASCENT_INLINE void LastHonorResetTime(uint32 val) { m_lastHonorResetTime = val; }
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
	ASCENT_INLINE void SetName(string& name) { m_name = name; }
	// spell to (delay, last time)
	
	FactionReputation * reputationByListId[128];
	
	uint64 m_comboTarget;
	int8 m_comboPoints;
	bool m_retainComboPoints;
	int8 m_spellcomboPoints; // rogue talent Ruthlessness will change combopoints while consuming them. solutions 1) add post cast prochandling, 2) delay adding the CP
	void UpdateComboPoints();

	ASCENT_INLINE void AddComboPoints(uint64 target, uint8 count)
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

	ASCENT_INLINE void NullComboPoints() { if(!m_retainComboPoints) { m_comboTarget = 0; m_comboPoints = 0; m_spellcomboPoints=0; } UpdateComboPoints(); }
	uint32 m_speedChangeCounter;

	void SendAreaTriggerMessage(const char * message, ...);
	void Set_Mute_on_player(uint32 until);
	void Remove_Mute_on_player();
	uint32 HasMuteOnPlayer(){return chat_disabled_until;};
        
	// Trade Target
	//Player *getTradeTarget() {return mTradeTarget;};

	ASCENT_INLINE Player * GetTradeTarget()
	{
		if(!IsInWorld()) return 0;
		return m_mapMgr->GetPlayer((uint32)mTradeTarget);
	}

	Item *getTradeItem(uint32 slot) {return mTradeItems[slot];};
        
	// Water level related stuff (they are public because they need to be accessed fast)
	// Nose level of the character (needed for proper breathing)
	float m_noseLevel;

	/* Mind Control */
	void Possess(Unit * pTarget);
	void UnPossess();

	/* Last Speeds */
	ASCENT_INLINE void UpdateLastSpeeds()
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
#ifdef OPTIMIZED_PLAYER_SAVING
	void save_LevelXP();
	void save_Skills();
	void save_ExploreData();
	void save_Gold();
	void save_Misc();
	void save_PositionHP();
	void save_BindPosition();
	void save_Honor();
	void save_EntryPoint();
	void save_Taxi();
	void save_Transporter();
	void save_Spells();
	void save_Actions();
	void save_Reputation();
	void save_Auras();
	void save_InstanceType();
	void save_Zone();
	void save_PVP();
#endif

#ifdef CLUSTERING
	void EventRemoveAndDelete();
	void PackPlayerData(ByteBuffer & data);
	bool UnpackPlayerData(ByteBuffer & data);
#endif

	Creature * m_tempSummon;
	bool m_deathVision;
	SpellEntry * last_heal_spell;
	LocationVector m_sentTeleportPosition;

	void RemoveFromBattlegroundQueue();
	void FullHPMP();
	uint32 m_arenateaminviteguid;

    /************************************************************************/
    /* Spell Packet wrapper Please keep this separated                      */
    /************************************************************************/
    void SendCastResult(uint32 SpellId, uint8 ErrorMessage, uint32 Extra);
    void SendSpellCoolDown(uint32 SpellID, uint16 Time);
    void SendLevelupInfo(uint32 level, uint32 Hp, uint32 Mana, uint32 Stat0, uint32 Stat1, uint32 Stat2, uint32 Stat3, uint32 Stat4);
    void SendLogXPGain(uint64 guid, uint32 NormalXP, uint32 RestedXP, bool type);
    void SendBindPointUpdate(float x,float y,float z,uint32 mapid,uint32 zoneid);
    void SendSetProficiency(uint8 ItemClass, uint32 Proficiency);
    void SendEnvironmentalDamageLog(uint64 & guid, uint8 type, uint32 damage);
    void SendLoginVerifyWorld(uint32 MapId, float x, float y, float z, float o);
    void SendLoginVerifyWorld();
    /************************************************************************/
    /* End of SpellPacket wrapper                                           */
    /************************************************************************/

	Mailbox m_mailBox;
	bool m_waterwalk;
	bool m_setwaterwalk;
	bool m_setflycheat;
	uint32 m_areaspirithealer_guid;

	ASCENT_INLINE bool IsAttacking() {return m_attacking; }

	static void InitVisibleUpdateBits();
	static UpdateMask m_visibleUpdateMask;

	void CopyAndSendDelayedPacket(WorldPacket * data);

protected:
	LocationVector m_summonPos;
	uint32 m_summonInstanceId;
	uint32 m_summonMapId;
	uint32 m_summoner;

	uint32 iActivePet;
	void _SetCreateBits(UpdateMask *updateMask, Player *target) const;
	void _SetUpdateBits(UpdateMask *updateMask, Player *target) const;
/* Update system components */
	ByteBuffer bUpdateBuffer;
    ByteBuffer bCreationBuffer;
	uint32 mUpdateCount;
    uint32 mCreationCount;
	uint32 mOutOfRangeIdCount;
	ByteBuffer mOutOfRangeIds;
	SplineMap _splineMap;
/* End update system */
	void _LoadTutorials(QueryResult * result);
	void _SaveTutorials();
	void _SaveInventory(bool firstsave);
	void _SaveQuestLogEntry();
	void _LoadQuestLogEntry(QueryResult * result);
	// DK
	void _LoadPet(QueryResult * result);
	void _LoadPetNo();
	void _LoadPetSpells(QueryResult * result);
	void _SavePet();
	void _SavePetSpells();
	void _SaveItemCooldown();
	void _LoadItemCooldown(QueryResult * result);
	void _SaveSpellCoolDownSecurity();
	void _LoadSpellCoolDownSecurity(QueryResult * result);
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
	uint32 mTradeTarget;
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
	
    uint32      m_invitersGuid; // It is guild inviters guid ,0 when its not used
    

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
	uint32 m_GroupInviter;
	uint8 m_StableSlotCount;

    // Fishing related
	Object *m_SummonedObject;

    // other system
	Corpse *    myCorpse;

	uint32      m_cooldownTimer;
	uint32      m_lastHonorResetTime;
	uint32      _fields[PLAYER_END];
    uint32	    chat_disabled_until;    //force player to be silent. Yeah i'm pissed of on noobs
	uint32	    trigger_on_stun;        //bah, warrior talent but this will not get triggered on triggered spells if used on proc so i'm forced to used a special variable
	uint32	    trigger_on_stun_chance; //also using this for mage "Frostbite" talent
	int			hearth_of_wild_pct;		//druid hearth of wild talent used on shapeshifting. We eighter know what is last talent level or memo on learn

	uint32 m_team;
	float       m_lastRunSpeed;
	float       m_lastRunBackSpeed;
	float       m_lastSwimSpeed;
	float       m_lastBackSwimSpeed;
	float       m_lastFlySpeed;


	void RemovePendingPlayer();
};

class SkillIterator
{
	SkillMap::iterator m_itr;
	SkillMap::iterator m_endItr;
	bool m_searchInProgress;
	Player * m_target;
public:
	SkillIterator(Player* target) : m_searchInProgress(false),m_target(target) {}
	~SkillIterator() { if(m_searchInProgress) { EndSearch(); } }

	void BeginSearch()
	{
		// iteminterface doesn't use mutexes, maybe it should :P
		ASSERT(!m_searchInProgress);
		m_itr = m_target->m_skills.begin();
		m_endItr = m_target->m_skills.end();
		m_searchInProgress=true;
	}

	void EndSearch()
	{
		// nothing here either
		ASSERT(m_searchInProgress);
		m_searchInProgress=false;
	}

	PlayerSkill* operator*() const
	{
		return &m_itr->second;
	}

	PlayerSkill* operator->() const
	{
		return &m_itr->second;
	}

	void Increment()
	{
		if(!m_searchInProgress)
			BeginSearch();

		if(m_itr==m_endItr)
			return;

		++m_itr;
	}

	ASCENT_INLINE PlayerSkill* Grab() { return &m_itr->second; }
	ASCENT_INLINE bool End() { return (m_itr==m_endItr)?true:false; }
};

#endif
