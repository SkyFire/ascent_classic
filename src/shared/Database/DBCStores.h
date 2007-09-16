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

#ifndef __SPELLSTORE_H
#define __SPELLSTORE_H

#include "Common.h"
#include "DataStore.h"
#include "Timer.h"

struct ItemSetEntry
{
	uint32 id;				  //1
	uint32 name;				//2
	uint32 unused_shit[15];	  //3 - 9
	uint32 flag;				//10 constant
	uint32 itemid[8];		   //11 - 18
	uint32 more_unused_shit[9]; //19 - 27
	uint32 SpellID[8];		  //28 - 35
	uint32 itemscount[8];	   //36 - 43
	uint32 RequiredSkillID;	 //44
	uint32 RequiredSkillAmt;	//45
};

struct Lock
{
	uint32 Id;
	uint32 locktype[5]; // 0 - no lock, 1 - item needed for lock, 2 - min lockping skill needed
	uint32 unk1[3];
	uint32 lockmisc[5]; // if type is 1 here is a item to unlock, else is unknow for now
	uint32 unk2[3];
	uint32 minlockskill[5]; // min skill in lockpiking to unlock.
	uint32 unk3[11];
};

struct emoteentry
{
	uint32 Id;
	uint32 name;
	uint32 textid;
	uint32 textid2;
	uint32 textid3;
	uint32 textid4;
	uint32 unk1;
	uint32 textid5;
	uint32 unk2;
	uint32 textid6;
	uint32 unk3;
	uint32 unk4;
	uint32 unk5;
	uint32 unk6;
	uint32 unk7;
	uint32 unk8;
	uint32 unk9;
	uint32 unk10;
	uint32 unk11;
};

struct skilllinespell //SkillLineAbility.dbc
{
	uint32 Id;
	uint32 skilline;
	uint32 spell;
	uint32 unk1;
	uint32 unk2;
	uint32 unk3;
	uint32 unk4;
	uint32 unk5;
	uint32 next;
	uint32 minrank;
	uint32 grey;
	uint32 green;
	uint32 unk10;
	uint32 unk11;
	uint32 reqTP;
};

struct EnchantEntry
{
	uint32 Id;
	uint32 type[3];
	int32 min[3];//for compat, in practice min==max
	int32 max[3];
	uint32 spell[3];
	uint32 Name;
	uint32 NameAlt1;
	uint32 NameAlt2;
	uint32 NameAlt3;
	uint32 NameAlt4;
	uint32 NameAlt5;
	uint32 NameAlt6;
	uint32 NameAlt7;
	uint32 NameAlt8;
	uint32 NameAlt9;
	uint32 NameAlt10;
	uint32 NameAlt11;
	uint32 NameAlt12;
	uint32 NameAlt13;
	uint32 NameAlt14;
	uint32 NameAlt15;
	uint32 NameFlags;
	uint32 visual;
	uint32 EnchantGroups;
	uint32 GemEntry;
	uint32 unk7;//Gem Related

};

struct GemPropertyEntry{
	uint32 Entry;
	uint32 EnchantmentID;
	uint32 unk1;//bool
	uint32 unk2;//bool
	uint32 SocketMask;
};

struct skilllineentry //SkillLine.dbc
{
	uint32 id;
	uint32 type;
	uint32 unk1;
	uint32 Name;
	uint32 NameAlt1;
	uint32 NameAlt2;
	uint32 NameAlt3;
	uint32 NameAlt4;
	uint32 NameAlt5;
	uint32 NameAlt6;
	uint32 NameAlt7;
	uint32 NameAlt8;
	uint32 NameAlt9;
	uint32 NameAlt10;
	uint32 NameAlt11;
	uint32 NameAlt12;
	uint32 NameAlt13;
	uint32 NameAlt14;
	uint32 NameAlt15;
	uint32 NameFlags;
	uint32 Description;
	uint32 DescriptionAlt1;
	uint32 DescriptionAlt2;
	uint32 DescriptionAlt3;
	uint32 DescriptionAlt4;
	uint32 DescriptionAlt5;
	uint32 DescriptionAlt6;
	uint32 DescriptionAlt7;
	uint32 DescriptionAlt8;
	uint32 DescriptionAlt9;
	uint32 DescriptionAlt10;
	uint32 DescriptionAlt11;
	uint32 DescriptionAlt12;
	uint32 DescriptionAlt13;
	uint32 DescriptionAlt14;
	uint32 DescriptionAlt15;
	uint32 DescriptionFlags;
	uint32 unk2;

};

// Struct for the entry in Spell.dbc
struct SpellEntry
{
	uint32 Id;							  //1
	uint32 School;						  //2
	uint32 Category;						//3
	uint32 field4;						  //4 something like spelltype 0: general spells 1: Pet spells 2: Disguise / transormation spells 3: enchantment spells
	uint32 DispelType;					  //5
	uint32 MechanicsType;				   //6
	uint32 Attributes;					  //7
	uint32 AttributesEx;					//8
	uint32 Flags3;						  //9
	uint32 Flags4;						 //10 // Flags to
	uint32 field11;						 //11 // Flags....
	uint32 unk201_1;						//12 // Flags 2.0.1 unknown one
	uint32 RequiredShapeShift;			  //13 // Flags BitMask for shapeshift spells
	uint32 UNK14;						   //14-> this is wrong // Flags BitMask for which shapeshift forms this spell can NOT be used in.
	uint32 Targets;						 //15 - N / M
	uint32 TargetCreatureType;			  //16
	uint32 RequiresSpellFocus;			  //17
	uint32 CasterAuraState;				 //18
	uint32 TargetAuraState;				 //19
	uint32 unk201_2;						//20 2.0.1 unknown two
	uint32 unk201_3;						//21 2.0.1 unknown three
	uint32 CastingTimeIndex;				//22
	uint32 RecoveryTime;					//23
	uint32 CategoryRecoveryTime;			//24 recoverytime
	uint32 InterruptFlags;				  //25
	uint32 AuraInterruptFlags;			  //26
	uint32 ChannelInterruptFlags;		   //27
	uint32 procFlags;					   //28
	uint32 procChance;					  //29
	int32 procCharges;					 //30
	uint32 maxLevel;						//31
	uint32 baseLevel;					   //32
	uint32 spellLevel;					  //33
	uint32 DurationIndex;				   //34
	uint32 powerType;					   //35
	uint32 manaCost;						//36
	uint32 manaCostPerlevel;				//37
	uint32 manaPerSecond;				   //38
	uint32 manaPerSecondPerLevel;		   //39
	uint32 rangeIndex;					  //40
	float  speed;						   //41
	uint32 modalNextSpell;				  //42
	uint32 maxstack;						//43
	uint32 Totem[2];						//44 - 45
	uint32 Reagent[8];					  //46 - 53
	uint32 ReagentCount[8];				 //54 - 61
	uint32 EquippedItemClass;			   //62
	uint32 EquippedItemSubClass;			//63
	uint32 RequiredItemFlags;			   //64
	uint32 Effect[3];					   //65 - 67
	uint32 EffectDieSides[3];			   //68 - 70
	uint32 EffectBaseDice[3];			   //71 - 73
	float  EffectDicePerLevel[3];		   //74 - 76
	float  EffectRealPointsPerLevel[3];	 //77 - 79
	int32  EffectBasePoints[3];			 //80 - 82
	int32  EffectMechanic[3];			   //83 - 85	   Related to SpellMechanic.dbc
	uint32 EffectImplicitTargetA[3];		//86 - 88
	uint32 EffectImplicitTargetB[3];		//89 - 91
	uint32 EffectRadiusIndex[3];			//92 - 94
	uint32 EffectApplyAuraName[3];		  //95 - 97
	uint32 EffectAmplitude[3];			  //98 - 100
	float  Effectunknown[3];				//101 - 103	 This value is the $ value from description
	uint32 EffectChainTarget[3];			//104 - 106
	uint32 EffectSpellGroupRelation[3];	 //107 - 109	 Not sure maybe we should rename it. its the relation to field: SpellGroupType
	uint32 EffectMiscValue[3];			  //110 - 112
	uint32 EffectTriggerSpell[3];		   //113 - 115
	float  EffectPointsPerComboPoint[3];	//116 - 118
	uint32 SpellVisual;					 //119
	uint32 field114;						//120
	uint32 dummy;						   //121
	uint32 CoSpell;						 //122   activeIconID;
	uint32 spellPriority;				   //123
	uint32 Name;							//124
	uint32 NameAlt1;						//125
	uint32 NameAlt2;						//126
	uint32 NameAlt3;						//127
	uint32 NameAlt4;						//128
	uint32 NameAlt5;						//129
	uint32 NameAlt6;						//130
	uint32 NameAlt7;						//131
	uint32 NameAlt8;						//132
	uint32 NameAlt9;						//133
	uint32 NameAlt10;					   //134
	uint32 NameAlt11;					   //135
	uint32 NameAlt12;					   //136
	uint32 NameAlt13;					   //137
	uint32 NameAlt14;					   //138
	uint32 NameAlt15;					   //139
	uint32 NameFlags;					   //140
	uint32 Rank;							//141
	uint32 RankAlt1;						//142
	uint32 RankAlt2;						//143
	uint32 RankAlt3;						//144
	uint32 RankAlt4;						//145
	uint32 RankAlt5;						//146
	uint32 RankAlt6;						//147
	uint32 RankAlt7;						//148
	uint32 RankAlt8;						//149
	uint32 RankAlt9;						//150
	uint32 RankAlt10;					   //151
	uint32 RankAlt11;					   //152
	uint32 RankAlt12;					   //153
	uint32 RankAlt13;					   //154
	uint32 RankAlt14;					   //155
	uint32 RankAlt15;					   //156
	uint32 RankFlags;					   //157
	uint32 Description;					 //158
	uint32 DescriptionAlt1;				 //159
	uint32 DescriptionAlt2;				 //160
	uint32 DescriptionAlt3;				 //161
	uint32 DescriptionAlt4;				 //162
	uint32 DescriptionAlt5;				 //163
	uint32 DescriptionAlt6;				 //164
	uint32 DescriptionAlt7;				 //165
	uint32 DescriptionAlt8;				 //166
	uint32 DescriptionAlt9;				 //167
	uint32 DescriptionAlt10;				//168
	uint32 DescriptionAlt11;				//169
	uint32 DescriptionAlt12;				//170
	uint32 DescriptionAlt13;				//171
	uint32 DescriptionAlt14;				//172
	uint32 DescriptionAlt15;				//173
	uint32 DescriptionFlags;				//174
	uint32 BuffDescription;				 //175
	uint32 BuffDescriptionAlt1;			 //176
	uint32 BuffDescriptionAlt2;			 //177
	uint32 BuffDescriptionAlt3;			 //178
	uint32 BuffDescriptionAlt4;			 //179
	uint32 BuffDescriptionAlt5;			 //180
	uint32 BuffDescriptionAlt6;			 //181
	uint32 BuffDescriptionAlt7;			 //182
	uint32 BuffDescriptionAlt8;			 //183
	uint32 BuffDescriptionAlt9;			 //184
	uint32 BuffDescriptionAlt10;			//185
	uint32 BuffDescriptionAlt11;			//186
	uint32 BuffDescriptionAlt12;			//187
	uint32 BuffDescriptionAlt13;			//188
	uint32 BuffDescriptionAlt14;			//189
//	uint32 BuffDescriptionAlt15;			//190
	uint32 proc_interval;				//190 - !!! Using it instead of BuffDescriptionAlt15 !!!
	uint32 buffdescflags;				   //191
	uint32 ManaCostPercentage;			  //192
	uint32 unkflags;						//193 
	uint32 StartRecoveryTime;			   //194
	uint32 StartRecoveryCategory;		   //195
	uint32 SpellFamilyName;				 //196
	uint32 SpellGroupType;				  //197   flags 
	uint32 unkne;						   //198   flags hackwow=shit 
	uint32 MaxTargets;					  //199 
	uint32 Spell_Dmg_Type;				  //200   dmg_class Integer	  0=None, 1=Magic, 2=Melee, 3=Ranged
	uint32 FG;							  //201   0,1,2 related to Spell_Dmg_Type I think
	int32 FH;							   //202   related to paladin aura's 
	float dmg_multiplier[3];				//203 - 205   if the name is correct I dono
	uint32 FL;							  //206   only one spellid:6994 has this value = 369
	uint32 FM;							  //207   only one spellid:6994 has this value = 4
	//uint32 FN;							  //208   only one spellid:26869  has this flag = 1

	// soz guys, gotta use these 3 vars :p
	//uint32 unk201_4; // these are related to creating a item through a spell
	//uint32 unk201_5;
	//uint32 unk201_6; // related to custom spells, summon spell quest related spells
	
	// custom shit
	uint32 buffType;						//209

	// this protects players from having >1 rank of a spell
	uint32 RankNumber;					  //210
	uint32 NameHash;						//211

	uint32 DiminishStatus;				//212

};

struct ItemExtendedCostEntry
{
	uint32 costid;
	uint32 honor;
	uint32 arena;
	uint32 item[5];
	uint32 count[5];
};

struct TalentEntry
{
	uint32  TalentID;
	uint32  TalentTree;
	uint32  Row;
	uint32  Col;
	uint32  RankID[5];
	uint32  unk[4];
	uint32  DependsOn;
	uint32  unk1[2];
	uint32  DependsOnRank;
	uint32  unk2[4];
};
struct Trainerspell
{
	uint32 Id;
	uint32 skilline1;
	uint32 skilline2;
	uint32 skilline3;
	uint32 maxlvl;
	uint32 charclass;
};
struct SpellCastTime
{
	uint32 ID;
	uint32 CastTime;
	uint32 unk1;
	uint32 unk2;
};

struct SpellRadius
{
	uint32 ID;
	float Radius;
	float unk1;
	float Radius2;
};

struct SpellRange
{
	uint32 ID;
	float minRange;
	float maxRange;
	uint32 unks[35];
};

struct SpellDuration
{
	uint32 ID;
	uint32 Duration1;
	uint32 Duration2;
	uint32 Duration3;
};

struct RandomProps
{
	uint32 ID;
	uint32 name1;
	uint32 spells[3];
	uint32 unk1;
	uint32 unk2;
	uint32 name2;
	uint32 RankAlt1;
	uint32 RankAlt2;
	uint32 RankAlt3;
	uint32 RankAlt4;
	uint32 RankAlt5;
	uint32 RankAlt6;
	uint32 RankAlt7;
	uint32 RankAlt8;
	uint32 RankAlt9;
	uint32 RankAlt10;
	uint32 RankAlt11;
	uint32 RankAlt12;
	uint32 RankAlt13;
	uint32 RankAlt14;
	uint32 RankAlt15;
	uint32 RankFlags;

};

struct AreaTable
{
	uint32 AreaId;
	uint32 mapId;
	uint32 ZoneId;
	uint32 explorationFlag;
	uint32 AreaFlags;
	uint32 unk2;
	uint32 unk3;
	uint32 unk4;
	uint32 EXP;//not XP
	uint32 unk5;
	uint32 level;
	uint32 name;
	uint32 nameAlt1;
	uint32 nameAlt2;
	uint32 nameAlt3;
	uint32 nameAlt4;
	uint32 nameAlt5;
	uint32 nameAlt6;
	uint32 nameAlt7;
	uint32 nameAlt8;
	uint32 nameAlt9;
	uint32 nameAlt10;
	uint32 nameAlt11;
	uint32 nameAlt12;
	uint32 nameAlt13;
	uint32 nameAlt14;
	uint32 nameAlt15;
	uint32 nameFlags;
	uint32 category;
	uint32 unk7;
	uint32 unk8;
	uint32 unk9;
	uint32 unk10;
	uint32 unk11;
	uint32 unk12;
};

struct FactionTemplateDBC
{
	uint32 ID;
	uint32 Faction;
	uint32 FactionGroup;
	uint32 Mask;
	uint32 FriendlyMask;
	uint32 HostileMask;
	uint32 EnemyFactions[4];
	uint32 FriendlyFactions[4];
};

struct AuctionHouseDBC
{
	uint32 id;
	uint32 unk;
	uint32 fee;
	uint32 tax;
	char* name;
	char* nameAlt1;
	char* nameAlt2;
	char* nameAlt3;
	char* nameAlt4;
	char* nameAlt5;
	char* nameAlt6;
	char* nameAlt7;
	char* nameAlt8;
	char* nameAlt9;
	char* nameAlt10;
	char* nameAlt11;
	char* nameAlt12;
	char* nameAlt13;
	char* nameAlt14;
	char* nameAlt15;
	char* nameFlags;
};

struct FactionDBC
{
	uint32 ID;
	int32 RepListId;
	uint32 baseRepMask[4];
	uint32 unk1[4];
	int32 baseRepValue[4];
	uint32 unk2[4];
	uint32 parentFaction;
	uint32 Name;
	uint32 shit[16];
	uint32 Description;
	uint32 shit2[16];
};

struct WorldMapArea
{
	uint32	ID;
	uint32	mapId;
	uint32	zoneId;
	uint32	zoneName;
	float	y1, y2;
	float	x1, x2;
	uint32 unk;
};

struct WorldMapOverlay
{
	uint32 ID;
	uint32 worldMapAreaID;
	uint32 areaTableID;
	uint32 unk1;
	uint32 unk2;
	uint32 unk3;
	uint32 unk4;
	uint32 unk5;
	uint32 name;
	uint32 areaW; 
	uint32 areaH; 
	uint32 areaX;  
	uint32 areaY;  
	uint32 y1;  
	uint32 x1;
	uint32 y2; 
	uint32 x2; 
};

struct DBCTaxiNode
{
	uint32 id;
	uint32 mapid;
	float x;
	float y;
	float z;
	uint32 name;
	uint32 namealt1;
	uint32 namealt2;
	uint32 namealt3;
	uint32 namealt4;
	uint32 namealt5;
	uint32 namealt6;
	uint32 namealt7;
	uint32 namealt8;
	uint32 namealt9;
	uint32 namealt10;
	uint32 namealt11;
	uint32 namealt12;
	uint32 namealt13;
	uint32 namealt14;
	uint32 namealt15;
	uint32 nameflags;
	uint32 horde_mount;
	uint32 alliance_mount;
};

struct DBCTaxiPath
{
	uint32 id;
	uint32 from;
	uint32 to;
	uint32 price;
};

struct DBCTaxiPathNode
{
	uint32 id;
	uint32 path;
	uint32 seq;
	uint32 mapid;
	float x;
	float y;
	float z;
	uint32 unk1;
	uint32 waittime;
	uint32 unk2;
	uint32 unk3;
};

struct GraveyardEntry
{
	uint32  someid;
	uint32  mapid;
	float   x;
	float   y;
	float   z;
	uint32  name;
	uint32  not_used[15];
	uint32  flag;
};

struct NameGenEntry
{
	int32  id;
	uint32  offsetindex;
	int32  unk1;
	bool   unk2;
};

struct CreatureSpellDataEntry
{
	uint32 id;
	uint32 Spells[3];
	uint32 PHSpell;
	uint32 Cooldowns[3];
	uint32 PH;
};

struct CharRaceEntry
{
	uint32 race_id;
	uint32 unk1;
	uint32 faction_id;
	uint32 unk2;
	uint32 unk3;
	uint32 unk4;
	uint32 unk5;
	uint32 unk6;
	uint32 team_id;
	uint32 unk7;
	uint32 unk8;
	uint32 unk9;
	uint32 name1;
	uint32 cinematic_id;
	uint32 name2;
	uint32 unk10;
	uint32 unk11;
	uint32 unk12;
	uint32 unk13;
	uint32 unk14;
	uint32 unk15;
	uint32 unk16;
	uint32 unk17;
	uint32 unk18;
	uint32 unk19;
	uint32 unk20;
	uint32 unk21;
	uint32 unk22;
	uint32 unk23;
	uint32 unk24;
	uint32 unk25;
	uint32 unk26;
	uint32 unk27;
	uint32 unk28;
	uint32 unk29;
};

struct CharClassEntry
{
	uint32 class_id;
	uint32 unk1;
	uint32 power_type;
	uint32 unk2;
	uint32 name;
	uint32 namealt1;
	uint32 namealt2;
	uint32 namealt3;
	uint32 namealt4;
	uint32 namealt5;
	uint32 namealt6;
	uint32 namealt7;
	uint32 namealt8;
	uint32 namealt9;
	uint32 namealt10;
	uint32 namealt11;
	uint32 namealt12;
	uint32 namealt13;
	uint32 namealt14;
	uint32 namealt15;
	uint32 nameflags;
	uint32 unk3;
	uint32 unk4;
	uint32 unk5;
};

struct CreatureFamilyEntry
{
	uint32 ID;
	float minsize;
	uint32 minlevel;
	float maxsize;
	uint32 maxlevel;
	uint32 skilline;
	uint32 tameable;
	uint32 pet_food_id;
	uint32 name;
	uint32 namealt1;
	uint32 namealt2;
	uint32 namealt3;
	uint32 namealt4;
	uint32 namealt5;
	uint32 namealt6;
	uint32 namealt7;
	uint32 namealt8;
	uint32 namealt9;
	uint32 namealt10;
	uint32 namealt11;
	uint32 namealt12;
	uint32 namealt13;
	uint32 namealt14;
	uint32 namealt15;
	uint32 nameflags;
	uint32 unk1;
};

struct MapEntry
{
	uint32 id;
	uint32 name_internal;
	uint32 map_type;
	uint32 unk;
	uint32 real_name;
	uint32 unk_1;
	uint32 unk1;
	uint32 unk2;
	uint32 unk3;
	uint32 unk4;
	uint32 unk5;
	uint32 unk6;
	uint32 unk7;
	uint32 unk8;
	uint32 unk9;
	uint32 unk10;
	uint32 unk11;
	uint32 unk12;
	uint32 unk13;
	uint32 unk14;
	uint32 unk15;
	uint32 unk16;
	uint32 unk17;
	uint32 unk18;
	uint32 unk19;
	uint32 unk20;
	uint32 unk21;
	uint32 unk22;
	uint32 unk23;
	uint32 unk24;
	uint32 unk25;
	uint32 unk26;
	uint32 unk27;
	uint32 unk28;
	uint32 unk29;
	uint32 unk30;
	uint32 unk31;
	uint32 unk32;
	uint32 unk33;
	uint32 unk34;
	uint32 unk35;
	uint32 unk36;
	uint32 unk37;
	uint32 unk38;
	uint32 unk39;
	uint32 unk40;
	uint32 unk41;
	uint32 unk42;
	uint32 unk43;
	uint32 unk44;
	uint32 unk45;
	uint32 unk46;
	uint32 unk47;
	uint32 unk48;
	uint32 unk49;
	uint32 unk50;
	uint32 unk51;
	uint32 unk52;
	uint32 unk53;
	uint32 unk54;
	uint32 unk55;
	uint32 unk56;
	uint32 unk57;
	uint32 unk58;
	uint32 unk59;
	uint32 unk60;
	uint32 unk61;
	uint32 unk62;
	uint32 unk63;
	uint32 unk64;
	uint32 unk65;
	uint32 unk66;
	uint32 unk67;
	uint32 unk68;
	uint32 unk69;
	uint32 aunk1;
	uint32 aunk2;
	uint32 aunk3;
	uint32 aunk4;
	uint32 aunk5;
	uint32 aunk6;
	uint32 aunk7;
	uint32 aunk8;
	uint32 aunk9;
	uint32 aunk10;
	uint32 aunk11;
	uint32 aunk12;
	uint32 aunk13;
	uint32 aunk14;
	uint32 aunk15;
	uint32 aunk16;
	uint32 aunk17;
	uint32 aunk18;
	uint32 aunk19;
	uint32 aunk20;
	uint32 aunk21;
	uint32 aunk22;
	uint32 aunk23;
	uint32 aunk24;
	uint32 bunk1;
	uint32 bunk2;
	uint32 bunk3;
	uint32 bunk4;
	uint32 bunk5;
	uint32 bunk6;
	uint32 bunk7;
	uint32 bunk8;
	uint32 bunk9;
	uint32 bunk10;
	uint32 bunk11;
	uint32 bunk12;
	uint32 bunk13;
	uint32 bunk14;
	uint32 bunk15;
	uint32 bunk16;
	uint32 bunk17;
	uint32 bunk18;
	uint32 bunk19;
	uint32 bunk20;
	uint32 bunk21;
	uint32 bunk22;
	uint32 bunk23;
	uint32 bunk24;
};

inline float GetRadius(SpellRadius *radius)
{
	return radius->Radius;
}
inline uint32 GetCastTime(SpellCastTime *time)
{
	return time->CastTime;
}
inline float GetMaxRange(SpellRange *range)
{
	return range->maxRange;
}
inline float GetMinRange(SpellRange *range)
{
	return range->minRange;
}
inline uint32 GetDuration(SpellDuration *dur)
{
	return dur->Duration1;
}


struct TransportAnimation
{
	uint32 ID;
	uint32 EntryID;
	uint32 Time;
	float PlusX;
	float PlusY;
	float PlusZ;
	uint32 MovementType;
};
// You need two lines like this for every new DBC store

defineIndexedDBCStore(GemPropertiesStore,GemPropertyEntry);
defineIndexedDBCStore(ItemSetStore,ItemSetEntry);
defineIndexedDBCStore(LockStore,Lock);
defineIndexedDBCStore(SpellStore,SpellEntry);
defineIndexedDBCStore(DurationStore,SpellDuration);
defineIndexedDBCStore(RangeStore,SpellRange);
defineIndexedDBCStore(EmoteStore,emoteentry);
defineIndexedDBCStore(RadiusStore,SpellRadius);
defineIndexedDBCStore(CastTimeStore,SpellCastTime);
defineIndexedDBCStore(AreaStore,AreaTable);
defineIndexedDBCStore(WorldMapAreaStore,WorldMapArea);
defineDBCStore(WorldMapOverlayStore,WorldMapOverlay);
defineIndexedDBCStore(FactionTmpStore,FactionTemplateDBC);
defineIndexedDBCStore(FactionStore,FactionDBC);
defineIndexedDBCStore(EnchantStore,EnchantEntry);
defineIndexedDBCStore(RandomPropStore,RandomProps);
defineDBCStore(SkillStore,skilllinespell);
defineIndexedDBCStore(SkillLineStore,skilllineentry);
defineDBCStore(TaxiNodeStore,DBCTaxiNode);
defineDBCStore(TaxiPathStore,DBCTaxiPath);
defineDBCStore(TaxiPathNodeStore,DBCTaxiPathNode);
defineDBCStore(WorldSafeLocsStore,GraveyardEntry);
defineIndexedDBCStore(TransportAnimationStore,TransportAnimation);
defineDBCStore(NameGenStore, NameGenEntry);
defineIndexedDBCStore(AuctionHouseStore,AuctionHouseDBC);
defineDBCStore(TalentStore,TalentEntry);
defineIndexedDBCStore(CreatureSpellDataStore, CreatureSpellDataEntry);
defineIndexedDBCStore(CreatureFamilyStore, CreatureFamilyEntry);
defineIndexedDBCStore(CharClassStore, CharClassEntry);
defineIndexedDBCStore(CharRaceStore, CharRaceEntry);
defineIndexedDBCStore(MapStore, MapEntry);
defineIndexedDBCStore(ItemExtendedCostStore,ItemExtendedCostEntry);

#define sGemPropertiesStore GemPropertiesStore::getSingleton()
#define sRandomPropStore RandomPropStore::getSingleton()
#define sEnchantStore EnchantStore::getSingleton()
#define sSpellStore SpellStore::getSingleton()
#define sLockStore LockStore::getSingleton()
#define sSkillLineStore SkillLineStore::getSingleton()
#define sSkillStore SkillStore::getSingleton()
#define sEmoteStore EmoteStore::getSingleton()
#define sSpellDuration DurationStore::getSingleton()
#define sSpellRange RangeStore::getSingleton()
#define sSpellRadius RadiusStore::getSingleton()
#define sCastTime CastTimeStore::getSingleton()
#define sAreaStore AreaStore::getSingleton()
#define sWorldMapAreaStore WorldMapAreaStore::getSingleton()
#define sWorldMapOverlayStore WorldMapOverlayStore::getSingleton()
#define sFactionTmpStore FactionTmpStore::getSingleton()
#define sFactionStore FactionStore::getSingleton()
#define sTaxiNodeStore TaxiNodeStore::getSingleton()
#define sTaxiPathStore TaxiPathStore::getSingleton()
#define sTaxiPathNodeStore TaxiPathNodeStore::getSingleton()
#define sItemSetStore ItemSetStore::getSingleton()
#define sWorldSafeLocsStore WorldSafeLocsStore::getSingleton()
#define sTransportAnimationStore TransportAnimationStore::getSingleton()
#define sAuctionHouseStore AuctionHouseStore::getSingleton()
#define sTalentStore TalentStore::getSingleton()
#define sCreatureSpellStore CreatureSpellDataStore::getSingleton()
#define sCreatureFamilyStore CreatureFamilyStore::getSingleton()
#define sCharClassStore CharClassStore::getSingleton()
#define sCharRaceStore CharRaceStore::getSingleton()
#define sMapStore MapStore::getSingleton()
#define sItemExtendedCostStore ItemExtendedCostStore::getSingleton()

#endif
