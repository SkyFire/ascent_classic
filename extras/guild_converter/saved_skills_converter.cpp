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

/* EDIT THESE TO YOUR SERVERS CONFIGURATION */
const char * username = "root";
const char * password = "";
const char * dbname = "moo";

#include "stdafx.h"
#include "Database/DatabaseEnv.h"
#include "Common.h"
#include "Skill.h"
#include <sstream>
#include <conio.h>

CLog Log;
Database sDatabase;

const char * data_table_create_string = "CREATE TABLE `guild_data_new` (\n"
"  `guildid` int(30) NOT NULL,\n"
"  `playerid` int(30) NOT NULL,\n"
"  `guildRank` int(30) NOT NULL,\n"
"  `publicNote` varchar(300) NOT NULL,\n"
"  `officerNote` varchar(300) NOT NULL,\n"
"  `lastWithdrawReset` int(30) NOT NULL DEFAULT '0',\n"
"  `withdrawlsSinceLastReset` int(30) NOT NULL DEFAULT '0',\n"
"  `lastItemWithdrawReset0` int(30) NOT NULL DEFAULT '0',\n"
"  `itemWithdrawlsSinceLastReset0` int(30) NOT NULL DEFAULT '0',\n"
"  `lastItemWithdrawReset1` int(30) NOT NULL,\n"
"  `itemWithdrawlsSinceLastReset1` int(30) NOT NULL,\n"
"  `lastItemWithdrawReset2` int(30) NOT NULL,\n"
"  `itemWithdrawlsSinceLastReset2` int(30) NOT NULL,\n"
"  `lastItemWithdrawReset3` int(30) NOT NULL,\n"
"  `itemWithdrawlsSinceLastReset3` int(30) NOT NULL,\n"
"  `lastItemWithdrawReset4` int(30) NOT NULL,\n"
"  `itemWithdrawlsSinceLastReset4` int(30) NOT NULL,\n"
"  `lastItemWithdrawReset5` int(30) NOT NULL,\n"
"  `itemWithdrawlsSinceLastReset5` int(30) NOT NULL,\n"
"  KEY `a` (`guildid`,`playerid`)\n"
") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n";

const char * rank_table_create_string = "CREATE TABLE `guild_ranks_new` (\n"
"`guildId` int(6) unsigned NOT NULL DEFAULT '0',\n"
"`rankId` int(1) NOT NULL DEFAULT '0',\n"
"`rankName` varchar(255) NOT NULL DEFAULT '',\n"
"`rankRights` int(3) unsigned NOT NULL DEFAULT '0',\n"
"`goldLimitPerDay` int(30) NOT NULL DEFAULT '0',\n"
"`bankTabFlags0` int(30) NOT NULL DEFAULT '0',\n"
"`itemStacksPerDay0` int(30) NOT NULL DEFAULT '0',\n"
"`bankTabFlags1` int(30) NOT NULL DEFAULT '0',\n"
"`itemStacksPerDay1` int(30) NOT NULL DEFAULT '0',\n"
"`bankTabFlags2` int(30) NOT NULL DEFAULT '0',\n"
"`itemStacksPerDay2` int(30) NOT NULL DEFAULT '0',\n"
"`bankTabFlags3` int(30) NOT NULL DEFAULT '0',\n"
"`itemStacksPerDay3` int(30) NOT NULL DEFAULT '0',\n"
"`bankTabFlags4` int(30) NOT NULL DEFAULT '0',\n"
"`itemStacksPerDay4` int(30) NOT NULL DEFAULT '0',\n"
"`bankTabFlags5` int(30) NOT NULL DEFAULT '0',\n"
"`itemStacksPerDay5` int(30) NOT NULL DEFAULT '0',\n"
"PRIMARY KEY (`guildId`,`rankId`),\n"
"KEY `a` (`guildId`,`rankId`)\n"
") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n";

const char * table1_s = "CREATE TABLE `guild_bankitems` (\n"
"  `guildId` int(30) NOT NULL,\n"
"  `tabId` int(30) NOT NULL,\n"
"  `slotId` int(30) NOT NULL,\n"
"  `itemGuid` int(30) NOT NULL,\n"
"  PRIMARY KEY (`guildId`,`tabId`,`slotId`),\n"
"  KEY `a` (`guildId`),\n"
"  KEY `b` (`tabId`),\n"
"  KEY `c` (`slotId`)\n"
") ENGINE=MyISAM DEFAULT CHARSET=latin1;";

const char * table2_s = "CREATE TABLE `guild_banklogs` (\n"
"  `log_id` int(30) NOT NULL,\n"
"  `guildid` int(30) NOT NULL,\n"
"  `tabid` int(30) NOT NULL COMMENT 'tab 6 is money logs',\n"
"  `action` int(5) NOT NULL,\n"
"  `player_guid` int(30) NOT NULL,\n"
"  `item_entry` int(30) NOT NULL,\n"
"  `stack_count` int(30) NOT NULL,\n"
"  `timestamp` int(30) NOT NULL,\n"
"  PRIMARY KEY (`log_id`,`guildid`),\n"
"  KEY `a` (`guildid`),\n"
"  KEY `b` (`tabid`)\n"
") ENGINE=MyISAM DEFAULT CHARSET=latin1;";

const char * table3_s = "CREATE TABLE `guild_banktabs` (\n"
"  `guildId` int(30) NOT NULL,\n"
"  `tabId` int(30) NOT NULL,\n"
"  `tabName` varchar(200) NOT NULL,\n"
"  `tabIcon` varchar(200) NOT NULL,\n"
"  PRIMARY KEY (`guildId`,`tabId`),\n"
"  KEY `a` (`guildId`),\n"
"  KEY `b` (`tabId`)\n"
") ENGINE=MyISAM DEFAULT CHARSET=latin1;";

const char * table4_s = "CREATE TABLE `guild_logs` (\n"
"  `log_id` int(30) NOT NULL,\n"
"  `guildid` int(30) NOT NULL,\n"
"  `timestamp` int(30) NOT NULL,\n"
"  `event_type` int(30) NOT NULL,\n"
"  `misc1` int(30) NOT NULL,\n"
"  `misc2` int(30) NOT NULL,\n"
"  `misc3` int(30) NOT NULL\n"
") ENGINE=MyISAM DEFAULT CHARSET=latin1;";

string escapestring(const char * str)
{
	char buf[65536] = {0};
	uint32 ind = sDatabase.GetConnection();
	MYSQL * my = sDatabase.GetMy(ind);
    if(mysql_real_escape_string(my, buf, str, (unsigned long)strlen(str)) != 0)
	{
		sDatabase.FreeMy(ind);
		return string(buf);
	}
	else
	{
		sDatabase.FreeMy(ind);
		return string(str);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	Log.loglevel = 999;

	if(!sDatabase.Initialize("localhost", 3306, username, password, dbname, 1, 65536))
	{
		Log.Error("SQL", "could not connect");
		return 1;
	}

	printf("MAKE SURE YOU HAVE MADE A FULL BACKUP OF YOUR DATABASE BEFOREHAND!\n");
	printf("IF YOU HAVE DONE THIS, PLEASE CLOSE THIS WINDOW NOW AND DO SO!\n");
	printf("IF YOU HAVE MADE A BACKUP, HIT ENTER TO CONTINUE.\n");
	getch();

	Log.Notice("convert", "Making new tables");
	sDatabase.Query("DROP TABLE guild_data_new");
	sDatabase.Query("DROP TABLE guild_ranks_new");
	sDatabase.Query(data_table_create_string);
	sDatabase.Query(rank_table_create_string);
	QueryResult * result = sDatabase.Query("SELECT * FROM guilds");
	do 
	{
		uint32 guildid = result->Fetch()[0].GetUInt32();
		Log.Notice("convert", "converting guild '%s' id %u.", result->Fetch()[1].GetString(), result->Fetch()[0].GetUInt32());
		QueryResult * result2 = sDatabase.Query("SELECT * FROM guild_ranks WHERE guildId = %u ORDER BY rankId ASC", guildid);
		QueryResult * result3 = sDatabase.Query("SELECT COUNT(guid) FROM characters WHERE guildid = %u", guildid);
		
		if(!result2 || result3==NULL || result3->Fetch()[0].GetUInt32() == 0)
		{
			sDatabase.Execute("DELETE FROM guilds WHERE guildId = %u", guildid);
			continue;
		}

		map<uint32, uint32> rankMap;
		uint32 lowestrank=3;
		uint32 expected_id=0;

		do 
		{
			uint32 rankid = result2->Fetch()[1].GetUInt32();
			string rankname = result2->Fetch()[2].GetString();
			uint32 rankrights = result2->Fetch()[3].GetUInt32();

			rankMap.insert(make_pair(rankid, expected_id));
			if(expected_id>lowestrank)
				lowestrank=expected_id;

			if(rankid != expected_id)
				printf("guild %u rank %u -> %u\n", guildid, rankid, expected_id);
			
			sDatabase.Query("INSERT INTO guild_ranks_new VALUES(%u, %u, \"%s\", %u, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)", guildid, expected_id, escapestring(rankname.c_str()).c_str(), rankrights);
			expected_id++;
		} while (result2->NextRow());
		delete result2;

		result3 = sDatabase.Query("SELECT guid, publicNote, officerNote, guildRank FROM characters WHERE guildid = %u", guildid);
		do 
		{
			uint32 pguid = result3->Fetch()[0].GetUInt32();
			string pnote = result3->Fetch()[1].GetString();
			string onote = result3->Fetch()[2].GetString();
			uint32 grank = result3->Fetch()[3].GetUInt32();

			map<uint32,uint32>::iterator itr = rankMap.find(grank);
			if(itr == rankMap.end())
				grank = lowestrank;
			else
				grank = itr->second;

			sDatabase.Query("INSERT INTO guild_data_new VALUES(%u, %u, %u, \"%s\", \"%s\", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)",
				guildid, pguid, grank, escapestring(pnote.c_str()).c_str(), escapestring(onote.c_str()).c_str());

		} while (result3->NextRow());
		delete result3;

	} while (result->NextRow());
	delete result;

	Log.Notice("convert", "doing some sanity checks...");
	sDatabase.Query("DELETE FROM guild_ranks_new WHERE rankId > 9");

	Log.Notice("convert", "moving new tables to proper names...");
	sDatabase.Query("DROP TABLE guild_ranks");
	sDatabase.Query("RENAME TABLE guild_ranks_new TO guild_ranks");
	sDatabase.Query("RENAME TABLE guild_data_new TO guild_data");

	Log.Notice("convert", "altering table characters structure... 1... ");
	sDatabase.Query("ALTER TABLE characters DROP COLUMN publicNote");
	Log.Notice("convert", "altering table characters structure... 2... ");
	sDatabase.Query("ALTER TABLE characters DROP COLUMN officerNote");
	Log.Notice("convert", "altering table characters structure... 3... ");
	sDatabase.Query("ALTER TABLE characters DROP COLUMN guildid");
	Log.Notice("convert", "altering table characters structure... 4... ");
	sDatabase.Query("ALTER TABLE characters DROP COLUMN guildrank");


	Log.Notice("convert", "Updating guild table structure...");
	sDatabase.Query("ALTER TABLE guilds DROP COLUMN createdate");
	sDatabase.Query("ALTER TABLE guilds ADD COLUMN createdate int(30) NOT NULL DEFAULT '%u'", time(NULL));
	sDatabase.Query("ALTER TABLE guilds ADD COLUMN bankTabCount int(30) NOT NULL DEFAULT '0'");
	sDatabase.Query("ALTER TABLE guilds ADD COLUMN bankBalance int(30) NOT NULL");

	sDatabase.Query(table1_s);
	sDatabase.Query(table2_s);
	sDatabase.Query(table3_s);
	sDatabase.Query(table4_s);

	sDatabase.Shutdown();
	Log.Success("Program", "Exit normally");
	return 0;
}


bool IsValidSkill(uint32 SkillID)
{
    switch (SkillID)
    {
    case SKILL_FROST:
    case SKILL_FIRE:
    case SKILL_ARMS:
    case SKILL_COMBAT:
    case SKILL_SUBTLETY:
    case SKILL_POISONS:
    case SKILL_SWORDS:
    case SKILL_AXES:
    case SKILL_BOWS:
    case SKILL_GUNS:
    case SKILL_BEAST_MASTERY:
    case SKILL_SURVIVAL:
    case SKILL_MACES:
    case SKILL_2H_SWORDS:
    case SKILL_HOLY:
    case SKILL_SHADOW:
    case SKILL_DEFENSE:
    case SKILL_LANG_COMMON:
    case SKILL_RACIAL_DWARVEN:
    case SKILL_LANG_ORCISH:
    case SKILL_LANG_DWARVEN:
    case SKILL_LANG_DARNASSIAN:
    case SKILL_LANG_TAURAHE:
    case SKILL_DUAL_WIELD:
    case SKILL_RACIAL_TAUREN:
    case SKILL_ORC_RACIAL:
    case SKILL_RACIAL_NIGHT_ELF:
    case SKILL_FIRST_AID:
    case SKILL_FERAL_COMBAT:
    case SKILL_LANG_THALASSIAN:
    case SKILL_STAVES:
    case SKILL_LANG_DRACONIC:
    case SKILL_LANG_DEMON_TONGUE:
    case SKILL_LANG_TITAN:
    case SKILL_LANG_OLD_TONGUE:
    case SKILL_SURVIVAL2:
    case SKILL_RIDING_HORSE:
    case SKILL_RIDING_WOLF:
    case SKILL_RIDING_RAM:
    case SKILL_RIDING_TIGER:
    case SKILL_SWIMING:
    case SKILL_2H_MACES:
    case SKILL_UNARMED:
    case SKILL_MARKSMANSHIP:
    case SKILL_BLACKSMITHING:
    case SKILL_LEATHERWORKING:
    case SKILL_ALCHEMY:
    case SKILL_2H_AXES:
    case SKILL_DAGGERS:
    case SKILL_THROWN:
    case SKILL_HERBALISM:
    case SKILL_GENERIC_DND:
    case SKILL_RETRIBUTION:
    case SKILL_COOKING:
    case SKILL_MINING:
    case SKILL_PET_IMP:
    case SKILL_PET_FELHUNTER:
    case SKILL_TAILORING:
    case SKILL_ENGINERING:
    case SKILL_PET_SPIDER:
    case SKILL_PET_VOIDWALKER:
    case SKILL_PET_SUCCUBUS:
    case SKILL_PET_INFERNAL:
    case SKILL_PET_DOOMGUARD:
    case SKILL_PET_WOLF:
    case SKILL_PET_CAT:
    case SKILL_PET_BEAR:
    case SKILL_PET_BOAR:
    case SKILL_PET_CROCILISK:
    case SKILL_PET_CARRION_BIRD:
    case SKILL_PET_GORILLA:
    case SKILL_PET_CRAB:
    case SKILL_PET_RAPTOR:
    case SKILL_PET_TALLSTRIDER:
    case SKILL_RACIAL_UNDED:
    case SKILL_CROSSBOWS:
    case SKILL_SPEARS:
    case SKILL_WANDS:
    case SKILL_POLEARMS:
    case SKILL_ATTRIBUTE_ENCHANCEMENTS:
    case SKILL_SLAYER_TALENTS:
    case SKILL_MAGIC_TALENTS:
    case SKILL_DEFENSIVE_TALENTS:
    case SKILL_PET_SCORPID:
    case SKILL_ARCANE:
    case SKILL_PET_TURTLE:
    case SKILL_FURY:
    case SKILL_PROTECTION:
    case SKILL_BEAST_TRAINING:
    case SKILL_PROTECTION2:
    case SKILL_PET_TALENTS:
    case SKILL_PLATE_MAIL:
    case SKILL_ASSASSINATION:
    case SKILL_LANG_GNOMISH:
    case SKILL_LANG_TROLL:
    case SKILL_ENCHANTING:
    case SKILL_DEMONOLOGY:
    case SKILL_AFFLICTION:
    case SKILL_FISHING:
    case SKILL_ENHANCEMENT:
    case SKILL_RESTORATION:
    case SKILL_ELEMENTAL_COMBAT:
    case SKILL_SKINNING:
    case SKILL_LEATHER:
    case SKILL_CLOTH:
    case SKILL_MAIL:
    case SKILL_SHIELD:
    case SKILL_FIST_WEAPONS:
    case SKILL_TRACKING_BEAST:
    case SKILL_TRACKING_HUMANOID:
    case SKILL_TRACKING_DEMON:
    case SKILL_TRACKING_UNDEAD:
    case SKILL_TRACKING_DRAGON:
    case SKILL_TRACKING_ELEMENTAL:
    case SKILL_RIDING_RAPTOR:
    case SKILL_RIDING_MECHANOSTRIDER:
    case SKILL_RIDING_UNDEAD_HORSE:
    case SKILL_RESTORATION2:
    case SKILL_BALANCE:
    case SKILL_DESTRUCTION:
    case SKILL_HOLY2:
    case SKILL_DISCIPLINE:
    case SKILL_LOCKPICKING:
    case SKILL_PET_BAT:
    case SKILL_PET_HYENA:
    case SKILL_PET_OWL:
    case SKILL_PET_WIND_SERPENT:
    case SKILL_LANG_GUTTERSPEAK:
    case SKILL_RIDING_KODO:
    case SKILL_RACIAL_TROLL:
    case SKILL_RACIAL_GNOME:
    case SKILL_RACIAL_HUMAN:
    case SKILL_JEWELCRAFTING:
    case SKILL_RACIAL_BLOODELF:
    case SKILL_PET_EVENT_REMOTECONTROL:
    case SKILL_LANG_DRAENEI:
    case SKILL_DRAENEI_RACIAL:
    case SKILL_PET_FELGUARD:
    case SKILL_RIDING:
    case SKILL_PET_DRAGONHAWK:
    case SKILL_PET_NETHER_RAY:
    case SKILL_PET_SPOREBAT:
    case SKILL_PET_WARP_STALKER:
    case SKILL_PET_RAVAGER:
    case SKILL_PET_SERPENT:
    case SKILL_INTERNAL:
    // return true because these are all ( Nah... Almost ) valid skill id's
        return true;

        break;
    // Its not a valid skill ID
    default:
        return false;

        break;
    }
}

