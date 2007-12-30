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

#include "stdafx.h"
#include "Database/DatabaseEnv.h"
#include "Common.h"
#include "Skill.h"
#include <sstream>

CLog Log;
Database sDatabase;

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
	const char * username = "root";
	const char * password = "";
	const char * dbname = "moo";
	if(!sDatabase.Initialize("localhost", 3306, username, password, dbname, 1, 65536))
	{
		Log.Error("SQL", "could not connect");
		return 1;
	}

	QueryResult * result = sDatabase.Query("SELECT * FROM guilds");
	do 
	{
		uint32 guildid = result->Fetch()[0].GetUInt32();
		printf("converting guild '%s' id %u.\n", result->Fetch()[1].GetString(), result->Fetch()[0].GetUInt32());
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

