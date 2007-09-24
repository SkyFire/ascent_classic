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

struct PlayerSkill
{
    uint32 SkillID;
    uint32 CurrentValue;
    uint32 MaximumValue;
    void Reset(uint32 Id)
    {
        MaximumValue = 0;
        CurrentValue = 0;
        SkillID = Id;
    }
};

// some proto's
bool IsValidSkill(uint32 SkillID);

int _tmain(int argc, _TCHAR* argv[])
{
	Log.loglevel = 999;
	const char * username = "";
	const char * password = "";
	const char * dbname = "";
	if(!sDatabase.Initialize("localhost", 3306, username, password, dbname, 1, 65536))
	{
		Log.Error("SQL", "could not connect");
		return 1;
	}
	QueryResult * result = sDatabase.Query("SELECT guid, skills, name FROM characters");
	do 
	{
        uint64          playerguid  = result->Fetch()[0].GetUInt64();
		char *          skills      = strdup(result->Fetch()[1].GetString());
        const char *    playername  = result->Fetch()[2].GetString();
		if(strchr(skills, ',') != 0)
		{
			// need to convert
			uint32 i = 0;
			stringstream ss;
			char * start = skills;
			char * end;
			for(;;)
			{
				// skill, skill+1, <add>0
				end = strchr(start, ',');
				if(!end)
					break;
				*end = '\0';
				uint32 val = atoi(start);
				start = end + 1;
				ss << val << ";";
				++i;
				if(!(i % 2))
					ss << "0 ";
			}

			sDatabase.Execute("UPDATE characters SET skills = '%s' WHERE guid = %u", ss.str().c_str(), result->Fetch()[0].GetUInt32());
			Log.Debug("SQL", "UPDATE characters SET skills = '%s' WHERE guid = %u\n", ss.str().c_str(), result->Fetch()[0].GetUInt32());
		}
        else
        // contains ' ' and NOT ','
        if (strchr(skills, ' ') && !strchr(skills, ','))
        {
            stringstream ss;
            char * start = skills;
            char * end;
            uint32 v1,v2,v3;
            PlayerSkill sk;
            uint32 errors = 0;
            uint32 validskills = 0;
            for(;;)
            {
                end = strchr(start, ' ');
                if(!end)
                    break;

                *end = 0;
                v1 = atol(start);
                start = end + 1;

                end = strchr(start, ' ');
                if(!end)
                    break;

                *end = 0;
                v2 = atol(start);
                start = end + 1;

                end = strchr(start, ' ');
                if(!end)
                    break;

                v3 = atol(start);
                start = end + 1;
                
                v1&=0xFFFF;
                if (IsValidSkill(v1))
                {
                    // for info systems
                    validskills++;

                    sk.Reset(v1 & 0xFFFF);
                    sk.CurrentValue = v2 & 0xFFFF;
                    sk.MaximumValue = (v2 >> 16) & 0xFFFF;

                    ss << sk.SkillID << ";";
                    ss << sk.CurrentValue << ";";
                    ss << sk.MaximumValue << ";";
                }
                else
                {
                    errors++;
                }

            }

            // fuck yea we are running on paranoid mode....
            // extra check just to make sure..... fuck I am paranoid
            uint32 ParanoidMode = 0;
            for (uint32 xxx = 0; xxx < ss.str().size(); xxx++)
            {
                if (ss.str().c_str()[xxx] == ';')
                    ParanoidMode++;
            }

            // the result should be dividable by 3 and has no result
            if ((ParanoidMode % 3) != 0)
            {
                printf("Something went VERY WRONG!!!!!!!!!!!!!!!!!!!!\n");
                printf("we are closing down so you can find out what did happen....\n");
                printf("The player that is fucked up: %s he has guid: %u", playername, playerguid);
                system("pause");
                exit(-1);
            }

            sDatabase.Execute("UPDATE characters SET skills = '%s' WHERE guid = %u", ss.str().c_str(), result->Fetch()[0].GetUInt32());
            printf("Parsed:%s [%u] valid and [%u] invalid skill entry's\n",playername, validskills, errors);
        }
	} while(result->NextRow());
	delete result;
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

