/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
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

#include "StdAfx.h"

initialiseSingleton( ChatHandler );
initialiseSingleton(CommandTableStorage);

ChatCommand * ChatHandler::getCommandTable()
{
	ASSERT(false);
	return 0;
}

ChatCommand * CommandTableStorage::GetSubCommandTable(const char * name)
{
	if(!strcmp(name, "unit"))
		return _unitCommandTable;
	else if(!strcmp(name, "debug"))
		return _debugCommandTable;
	else if(!strcmp(name, "waypoint"))
		return _waypointCommandTable;
	else if(!strcmp(name, "gm"))
		return _gmCommandTable;
	else if(!strcmp(name, "gobject"))
		return _gameobjectCommandTable;
	else if(!strcmp(name, "battleground"))
		return _battlegroundCommandTable;
	else if(!strcmp(name, "cheat"))
		return _cheatCommandTable;
	else if(!strcmp(name, "teleport"))
		return _teleportCommandTable;
	else if(!strcmp(name, "pvp"))
		return _pvpCommandTable;
	else if(!strcmp(name, "guild"))
		return _guildCommandTable;
	else if(!strcmp(name, "quest"))
		return _questCommandTable;
	else if(!strcmp(name, "server"))
		return _serverCommandTable;
	else if(!strcmp(name, "lookup"))
		return _lookupCommandTable;
	return 0;
}

#define dupe_command_table(ct, dt) this->dt = (ChatCommand*)allocate_and_copy(sizeof(ct)/* / sizeof(ct[0])*/, ct)
ASCENT_INLINE void* allocate_and_copy(uint32 len, void * pointer)
{
	void * data = (void*)malloc(len);
	memcpy(data, pointer, len);
	return data;
}

void CommandTableStorage::Load()
{
	QueryResult * result = WorldDatabase.Query("SELECT * FROM command_overrides");
	if(!result) return;

	do 
	{
		const char * name = result->Fetch()[0].GetString();
		const char * level = result->Fetch()[1].GetString();
		Override(name, level);
	} while(result->NextRow());
	delete result;
}

void CommandTableStorage::Override(const char * command, const char * level)
{
	ASSERT(level[0] != '\0');
	char * cmd = strdup(command);

	// find the command we're talking about
	char * sp = strchr(cmd, ' ');
	const char * command_name = cmd;
	const char * subcommand_name = 0;

	if(sp != 0)
	{
		// we're dealing with a subcommand.
		*sp = 0;
		subcommand_name = sp + 1;
	}

	size_t len1 = strlen(command_name);
	size_t len2 = subcommand_name ? strlen(subcommand_name) : 0;

	// look for the command.
	ChatCommand * p = &_commandTable[0];
	while(p->Name != 0)
	{
		if(!strnicmp(p->Name, command_name, len1))
		{
			// this is the one we wanna modify
			if(!subcommand_name)
			{
				// no subcommand, we can change it.
				p->CommandGroup = level[0];
				printf("Changing command level of command `%s` to %c.\n", p->Name, level[0]);
			}
			else
			{
				// assume this is a subcommand, loop the second set.
				ChatCommand * p2 = p->ChildCommands;
				if(!p2)
				{
					printf("Invalid command specified for override: %s\n", command_name);
				}
				else
				{
					while(p2->Name != 0)
					{
						if(!strnicmp("*",subcommand_name,1))
						{
								p2->CommandGroup = level[0];
								printf("Changing command level of command (wildcard) `%s`:`%s` to %c.\n", p->Name, p2->Name, level[0]);
						}else{
							if(!strnicmp(p2->Name, subcommand_name, len2))
							{
								// change the level
								p2->CommandGroup = level[0];
								printf("Changing command level of command `%s`:`%s` to %c.\n", p->Name, p2->Name, level[0]);
								break;
							}
						}
						p2++;
					}
					if(p2->Name == 0)
					{
						if(strnicmp("*",subcommand_name,1)) //Hacky.. meh.. -DGM
						{
							printf("Invalid subcommand referenced: `%s` under `%s`.\n", subcommand_name, p->Name);
						}
						break;
					}
				}
			}
			break;
		}
		++p;
	}

	if(p->Name == 0)
	{
		printf("Invalid command referenced: `%s`\n", command_name);
	}

	free(cmd);
}

void CommandTableStorage::Dealloc()
{
	free( _unitCommandTable );
	free( _debugCommandTable );
	free( _waypointCommandTable );
	free( _gmCommandTable );
	free( _gameobjectCommandTable );
	free( _battlegroundCommandTable );
	free( _cheatCommandTable );
	free( _teleportCommandTable );
	free( _pvpCommandTable );
	free( _guildCommandTable);
	free( _questCommandTable );
	free( _serverCommandTable );
	free( _lookupCommandTable );
	free( _commandTable );
}

void CommandTableStorage::Init()
{
	static ChatCommand unitCommandTable[] =
	{
		{ "hp",					'u', NULL,	"Health Points/HP",	NULL, UNIT_FIELD_HEALTH,	UNIT_FIELD_MAXHEALTH, 1 },
		{ "mana",				'u', NULL,	"Mana Points/MP",	  NULL, UNIT_FIELD_POWER1,	UNIT_FIELD_MAXPOWER1, 1 },
		{ "rage",				'u', NULL,	"Rage Points",		 NULL, UNIT_FIELD_POWER2,	UNIT_FIELD_MAXPOWER2, 1 },
		{ "energy",				'u', NULL,	"Energy Points",	   NULL, UNIT_FIELD_POWER4,	UNIT_FIELD_MAXPOWER4, 1 },
		{ "level",				'p', &ChatHandler::HandleModifyLevelCommand,"Level", NULL, 0, 0, 0 },
		{ "armor",				'u', NULL,	"Armor",			   NULL, UNIT_FIELD_STAT1,			  0,		   1 },
		{ "holy",				'u', NULL,	"Holy Resistance",	 NULL, UNIT_FIELD_RESISTANCES_01,	 0,		   1 },
		{ "fire",				'u', NULL,	"Fire Resistance",	 NULL, UNIT_FIELD_RESISTANCES_02,	 0,		   1 },
		{ "nature",				'u', NULL,	"Nature Resistance",   NULL, UNIT_FIELD_RESISTANCES_03,	 0,		   1 },
		{ "frost",				'u', NULL,	"Frost Resistance",	NULL, UNIT_FIELD_RESISTANCES_04,	 0,		   1 },
		{ "shadow",				'u', NULL,	"Shadow Resistance",   NULL, UNIT_FIELD_RESISTANCES_05,	 0,		   1 },
		{ "arcane",				'u', NULL,	"Arcane Resistance",   NULL, UNIT_FIELD_RESISTANCES_06,	 0,		   1 },
		{ "damage",				'u', NULL,	"Unit Damage Min/Max", NULL, UNIT_FIELD_MINDAMAGE,  UNIT_FIELD_MAXDAMAGE,2 },
		{ "scale",				'u', NULL,	"Size/Scale",		  NULL, OBJECT_FIELD_SCALE_X, 0,					2 },
		{ "gold",				'p', &ChatHandler::HandleModifyGoldCommand,  "Gold/Money/Copper",	  NULL,   0,  0,  0 },
		{ "speed",				'u', &ChatHandler::HandleModifySpeedCommand, "Movement Speed",		 NULL,   0,  0,  0 },
		{ "nativedisplayid",	'u', NULL, "Native Display ID", NULL, UNIT_FIELD_NATIVEDISPLAYID, 0,			  1 },
		{ "displayid" ,			'u', NULL,	"Display ID",		  NULL, UNIT_FIELD_DISPLAYID,	   0,			  1 },
		{ "flags" ,				'u', NULL,	"Unit Flags",		  NULL, UNIT_FIELD_FLAGS,		   0,			  1 },
		{ "faction",			'u', NULL,	"Faction Template",	NULL, UNIT_FIELD_FACTIONTEMPLATE, 0,			  1 },
		{ "dynamicflags",		'u',NULL,	"Dynamic Flags",	   NULL, UNIT_DYNAMIC_FLAGS,		 0,			  1 },
		{ "talentpoints",		'p',NULL,	"Talent Points",	   NULL, PLAYER_CHARACTER_POINTS1,   0,			  1 },
		{ "happiness",			'u', NULL,	"Happiness",			 NULL, UNIT_FIELD_POWER5,	UNIT_FIELD_MAXPOWER5, 1 },
		{ "spirit",				'u', NULL,	"Spirit",			  NULL, UNIT_FIELD_STAT0,		   0,			  1 },
		{ "boundingradius",		'u',NULL,  "Bounding Radius",	 NULL, UNIT_FIELD_BOUNDINGRADIUS,		 0,			  2 },
		{ "combatreach",		'u',NULL,	 "Combat Reach",		NULL, UNIT_FIELD_COMBATREACH, 0, 2 },
		{ "emotestate",			'u', NULL,	 "NPC Emote State",	 NULL, UNIT_NPC_EMOTESTATE, 0, 1 },
		{ "bytes",				'u',NULL,"Bytes",NULL,UNIT_FIELD_BYTES_0,0,1},
		{ "vendoradditem",		'n', &ChatHandler::HandleItemCommand,	  "Adds to vendor",				 NULL, 0, 0, 0},
		{ "vendorremoveitem",	'n', &ChatHandler::HandleItemRemoveCommand,"Removes from vendor.",		   NULL, 0, 0, 0},
		{ "npcflags",			'n', &ChatHandler::HandleNPCFlagCommand,	   "Changes NPC flags",			  NULL, 0, 0, 0},
		{ "deletenpc",			'n', &ChatHandler::HandleDeleteCommand,		"Deletes mob from db and world.", NULL, 0, 0, 0},
		{ "info",				'n', &ChatHandler::HandleNpcInfoCommand,	   "Displays NPC information",	   NULL, 0, 0, 0},
		{ "addAgent",			'n', &ChatHandler::HandleAddAIAgentCommand,	".npc addAgent <agent> <procEvent> <procChance> <procCount> <spellId> <spellType> <spelltargetType> <spellCooldown> <floatMisc1> <Misc2>",NULL, 0, 0, 0},
		{ "listAgent",			'n', &ChatHandler::HandleListAIAgentCommand,   ".npc listAgent",NULL, 0, 0, 0},
		{ "say",				'p', &ChatHandler::HandleMonsterSayCommand,	".npc say <text> - Makes selected mob say text <text>.", NULL, 0, 0, 0 },
		{ "yell",				'p', &ChatHandler::HandleMonsterYellCommand,   ".npc yell <Text> - Makes selected mob yell text <text>.", NULL, 0, 0, 0},
		{ "come",				'u', &ChatHandler::HandleNpcComeCommand,	   ".npc come - Makes npc move to your position", NULL, 0, 0, 0 },
		{ "return",				'u', &ChatHandler::HandleNpcReturnCommand,	 ".npc return - Returns ncp to spawnpoint.", NULL, 0, 0, 0 },
		{ "spawn",				'n', &ChatHandler::HandleCreatureSpawnCommand, ".npc spawn - Spawns npc of entry <id>", NULL, 0, 0, 0 },
		{ "spawnlink",			'n', &ChatHandler::HandleNpcSpawnLinkCommand, ".spawnlink sqlentry", NULL, 0, 0, 0 },
		{ "possess",			'p', &ChatHandler::HandleNpcPossessCommand, ".npc possess - Possess an npc (mind control)", NULL, 0, 0, 0 },
		{ "unpossess",			'p', &ChatHandler::HandleNpcUnPossessCommand, ".npc unpossess - Unposses any currently possessed npc.", NULL, 0, 0, 0 },
		{ "select",				'n', &ChatHandler::HandleNpcSelectCommand, ".npc select - selects npc closest", NULL, 0, 0, 0 },
		{ "createpet",			'p',&ChatHandler::HandleCreatePetCommand, "Creates a pet with <entry>.", NULL, 0, 0, 0 },
		{ "renamepet",			'p',&ChatHandler::HandleRenamePetCommand, "Renames a pet to <name>.", NULL, 0, 0, 0 },
		{ "addpetspell",		'p',&ChatHandler::HandleAddPetSpellCommand, "Teaches pet <spell>.", NULL, 0, 0, 0 },
		{ "removepetspell",		'p',&ChatHandler::HandleRemovePetSpellCommand, "Removes pet spell <spell>.", NULL, 0, 0, 0 },
		{ "kill",				'u', &ChatHandler::HandleKillCommand,		  ".kill - Kills selected unit.",   NULL, 0, 0, 0},
		{ "killplr" ,			'p', &ChatHandler::HandleKillByPlrCommand,         ".killplr <name> - Kills specified player" , NULL , 0 , 0 , 0 },
		{ "revive",				'p', &ChatHandler::HandleReviveCommand,		"Revives you.",				   NULL, 0, 0, 0},
		{ "reviveplr",			'p', &ChatHandler::HandleReviveStringcommand,  "Revives player specified.",	  NULL, 0, 0, 0},
		{ "demorph",			'p', &ChatHandler::HandleDeMorphCommand,	   "Demorphs from morphed model.",   NULL, 0, 0, 0},
		{ "mount",				'p', &ChatHandler::HandleMountCommand,		 "Mounts into modelid x.",		 NULL, 0, 0, 0},
		{ "dismount",			'p', &ChatHandler::HandleDismountCommand,	  "Dismounts.",					 NULL, 0, 0, 0},
		{ "resetreputation",	'p',&ChatHandler::HandleResetReputationCommand, ".resetreputation - Resets reputation to start levels. (use on characters that were made before reputation fixes.)", NULL, 0, 0, 0},
		{ "resetspells",		'p', &ChatHandler::HandleResetSpellsCommand,   ".resetspells - Resets all spells to starting spells of targeted player. DANGEROUS.", NULL, 0, 0, 0 },
		{ "resettalents",		'p', &ChatHandler::HandleResetTalentsCommand,  ".resettalents - Resets all talents of targeted player to that of their current level. DANGEROUS.", NULL, 0, 0, 0 },
		{ "resetskills",		'p', &ChatHandler::HandleResetSkillsCommand ,  ".resetskills - Resets all skills.", NULL, 0, 0, 0 },
		{ "additem",			'p', &ChatHandler::HandleAddInvItemCommand,	"",							   NULL, 0, 0, 0},
		{ "removeitem",			'p', &ChatHandler::HandleRemoveItemCommand,	"Removes item %u count %u.", NULL, 0, 0, 0 },
		{ "learn",				'p', &ChatHandler::HandleLearnCommand,		 "Learns spell",				   NULL, 0, 0, 0},
		{ "unlearn",			'p', &ChatHandler::HandleUnlearnCommand,	   "Unlearns spell",				 NULL, 0, 0, 0},
		{ "getskilllevel",		'p', &ChatHandler::HandleGetSkillLevelCommand, "Gets the current level of a skill",NULL,0,0,0}, //DGM (maybe add to playerinfo?)
		{ "getskillinfo",		'p', &ChatHandler::HandleGetSkillsInfoCommand, "Gets all the skills from a player",NULL,0,0,0},
		{ "learnskill",			'p', &ChatHandler::HandleLearnSkillCommand,	".learnskill <skillid> (optional) <value> <maxvalue> - Learns skill id skillid.", NULL, 0, 0, 0},
		{ "advanceskill",		'p', &ChatHandler::HandleModifySkillCommand,   "advanceskill <skillid> <amount, optional, default = 1> - Advances skill line x times..", NULL, 0, 0, 0},
		{ "removeskill",		'p', &ChatHandler::HandleRemoveSkillCommand,   ".removeskill <skillid> - Removes skill",		 NULL, 0, 0, 0 },
		{ "increaseweaponskill",'p', &ChatHandler::HandleIncreaseWeaponSkill, ".increaseweaponskill <count> - Increase eqipped weapon skill x times (defaults to 1).", NULL, 0, 0, 0},
		{ "clearcooldowns",		'p', &ChatHandler::HandleClearCooldownsCommand, "Clears all cooldowns for your class.", NULL, 0, 0, 0 },
		{ "removeauras",		'p', &ChatHandler::HandleRemoveAurasCommand,   "Removes all auras from target",  NULL, 0, 0, 0},
		{ "paralyze",			'p', &ChatHandler::HandleParalyzeCommand,	  "Roots/Paralyzes the target.",	NULL, 0, 0, 0 },
		{ "unparalyze",			'p', &ChatHandler::HandleUnParalyzeCommand,	"Unroots/Unparalyzes the target.",NULL, 0, 0, 0 },
		{ "additemset",			'p', &ChatHandler::HandleAddItemSetCommand,	"Adds item set to inv.",		  NULL, 0, 0, 0 },
		{ "advanceallskills",	'p', &ChatHandler::HandleAdvanceAllSkillsCommand, "Advances all skills <x> points.", NULL, 0, 0, 0 },
		{ "castspell",			'p', &ChatHandler::HandleCastSpellCommand,	 ".castspell <spellid> - Casts spell on target.",  NULL, 0, 0, 0 },
		{ "npcfollow",			'n', &ChatHandler::HandleNpcFollowCommand, "Sets npc to follow you", NULL, 0, 0, 0 },
		{ "nullfollow",			'n', &ChatHandler::HandleNullFollowCommand, "Sets npc to not follow anything", NULL, 0, 0, 0 },
		{ "formationlink1",		'n', &ChatHandler::HandleFormationLink1Command, "Sets formation master.", NULL, 0, 0, 0 },
		{ "formationlink2",		'n', &ChatHandler::HandleFormationLink2Command, "Sets formation slave with distance and angle", NULL, 0, 0, 0 },
		{ "formationclear",		'n', &ChatHandler::HandleFormationClearCommand, "Removes formation from creature", NULL, 0, 0, 0 },
		{ "showitems",			'u', &ChatHandler::HandleShowItems, "test for ItemIterator", NULL, 0, 0, 0 },
		{ "removesickness",		'p', &ChatHandler::HandleRemoveRessurectionSickessAuraCommand,   "Removes ressurrection sickness from the target",  NULL, 0, 0, 0},
#ifdef USE_SPECIFIC_AIAGENTS
		{ "spawnbot",			'p', &ChatHandler::HandlePetSpawnAIBot, ".pet spawnbot <type> - spawn a helper bot for your aid", NULL, 0, 0, 0 },
#endif
		{ NULL,		  0, NULL,	 "",					NULL, 0, 0  }
	};
	dupe_command_table(unitCommandTable, _unitCommandTable);

	static ChatCommand debugCommandTable[] =
	{
		{ "infront",		'd', &ChatHandler::HandleDebugInFrontCommand,  "",							   NULL, 0, 0, 0},
		{ "showreact",		'd', &ChatHandler::HandleShowReactionCommand,  "",							   NULL, 0, 0, 0},
		{ "aimove",			'd', &ChatHandler::HandleAIMoveCommand,		"",							   NULL, 0, 0, 0},
		{ "dist",			'd', &ChatHandler::HandleDistanceCommand,	  "",							   NULL, 0, 0, 0},
		{ "face",			'd', &ChatHandler::HandleFaceCommand,		  "",							   NULL, 0, 0, 0},
		{ "moveinfo",		'd', &ChatHandler::HandleMoveInfoCommand,	  "",							   NULL, 0, 0, 0},
		{ "setbytes",		'd', &ChatHandler::HandleSetBytesCommand,	  "",							   NULL, 0, 0, 0},
		{ "getbytes",		'd', &ChatHandler::HandleGetBytesCommand,	  "",							   NULL, 0, 0, 0},
		{ "unroot",			'd', &ChatHandler::HandleDebugUnroot,		  "",							   NULL, 0, 0, 0},
		{ "root",			'd', &ChatHandler::HandleDebugRoot,			"",							   NULL, 0, 0, 0},
		{ "landwalk",		'd', &ChatHandler::HandleDebugLandWalk,		"",							   NULL, 0, 0, 0},
		{ "waterwalk",		'd', &ChatHandler::HandleDebugWaterWalk,	   "",							   NULL, 0, 0, 0},
		{ "castspellne",	'd', &ChatHandler::HandleCastSpellNECommand,   ".castspellne <spellid> - Casts spell on target (only plays animations, doesnt handle effects or range/facing/etc.", NULL, 0, 0, 0 },
		{ "aggrorange",		'd', &ChatHandler::HandleAggroRangeCommand,	".aggrorange - Shows aggro Range of the selected Creature.", NULL, 0, 0, 0 },
		{ "knockback ",		'd', &ChatHandler::HandleKnockBackCommand,	 ".knockback <value> - Knocks you back.", NULL, 0, 0, 0 },
		{ "fade ",			'd', &ChatHandler::HandleFadeCommand,		  ".fade <value> - calls ModThreatModifyer().", NULL, 0, 0, 0 },
		{ "threatMod ",		'd', &ChatHandler::HandleThreatModCommand,	 ".threatMod <value> - calls ModGeneratedThreatModifyer().", NULL, 0, 0, 0 },
		{ "calcThreat ",	'd', &ChatHandler::HandleCalcThreatCommand,	".calcThreat <dmg> <spellId> - calculates threat.", NULL, 0, 0, 0 },
		{ "threatList ",	'd', &ChatHandler::HandleThreatListCommand,	".threatList  - returns all AI_Targets of the selected Creature.", NULL, 0, 0, 0 },
		{ "gettptime",		'd', &ChatHandler::HandleGetTransporterTime,   "grabs transporter travel time",NULL, 0, 0, 0 },
		{ "itempushresult",	'd',&ChatHandler::HandleSendItemPushResult,  "sends item push result", NULL, 0, 0, 0 },
		{ "setbit",			'd',  &ChatHandler::HandleModifyBitCommand,	"",							   NULL, 0, 0, 0},
		{ "setvalue",		'd', &ChatHandler::HandleModifyValueCommand,   "",							   NULL, 0, 0, 0},
		{ "aispelltestbegin",'d', &ChatHandler::HandleAIAgentDebugBegin, "", NULL, 0, 0, 0 },
		{ "aispelltestcontinue",'d', &ChatHandler::HandleAIAgentDebugContinue, "", NULL, 0, 0, 0 },
		{ "aispelltestskip",'d', &ChatHandler::HandleAIAgentDebugSkip, "", NULL, 0, 0, 0 },
		{ "dumpcoords",		'd', &ChatHandler::HandleDebugDumpCoordsCommmand, "", NULL, 0, 0, 0 },
        { "sendpacket",		'd', &ChatHandler::HandleSendpacket, "<opcode ID>, <data>", NULL, 0, 0, 0 },
		{ "sqlquery",		'd', &ChatHandler::HandleSQLQueryCommand, "<sql query>", NULL, 0, 0, 0 },
		{ "rangecheck",		'd', &ChatHandler::HandleRangeCheckCommand, "Checks the 'yard' range and internal range between the player and the target.", NULL, 0, 0, 0 },
		{ "setallratings",	'd', &ChatHandler::HandleRatingsCommand, "Sets rating values to incremental numbers based on their index.", NULL, 0, 0, 0 },
		{ "testlos",		'd', &ChatHandler::HandleCollisionTestLOS, "tests los", NULL, 0, 0, 0 },
		{ "testindoor",		'd', &ChatHandler::HandleCollisionTestIndoor, "tests indoor", NULL, 0, 0, 0 },
		{ "getheight",		'd', &ChatHandler::HandleCollisionGetHeight, "Gets height", NULL, 0, 0, 0 },
		{ NULL,		   0, NULL,									  "",							   NULL, 0, 0  }
	};
	dupe_command_table(debugCommandTable, _debugCommandTable);

	static ChatCommand waypointCommandTable[] =
	{
		{ "add",		'w', &ChatHandler::HandleWPAddCommand,		 "Add wp at current pos",		  NULL, 0, 0, 0},
		{ "show",		'w', &ChatHandler::HandleWPShowCommand,		"Show wp's for creature",		 NULL, 0, 0, 0},
		{ "hide",		'w', &ChatHandler::HandleWPHideCommand,		"Hide wp's for creature",		 NULL, 0, 0, 0},
		{ "delete",		'w', &ChatHandler::HandleWPDeleteCommand,	  "Delete selected wp",			 NULL, 0, 0, 0},
		{ "movehere",	'w', &ChatHandler::HandleWPMoveHereCommand,	"Move to this wp",				NULL, 0, 0, 0},
		{ "flags",		'w', &ChatHandler::HandleWPFlagsCommand,	   "Wp flags",					   NULL, 0, 0, 0},
		{ "waittime",	'w', &ChatHandler::HandleWPWaitCommand,		"Wait time at this wp",		   NULL, 0, 0, 0},
		{ "emote",		'w', &ChatHandler::HandleWPEmoteCommand,	   "Emote at this wp",			   NULL, 0, 0, 0},
		{ "skin",		'w', &ChatHandler::HandleWPSkinCommand,		"Skin at this wp",				NULL, 0, 0, 0},
		{ "change",		'w', &ChatHandler::HandleWPChangeNoCommand,	"Change at this wp",			  NULL, 0, 0, 0},
		{ "info",		'w', &ChatHandler::HandleWPInfoCommand,		"Show info for wp",			   NULL, 0, 0, 0},
		{ "movetype",	'w', &ChatHandler::HandleWPMoveTypeCommand,	"Movement type at wp",			NULL, 0, 0, 0},
		{ "generate",	'w', &ChatHandler::HandleGenerateWaypoints,	"Randomly generate wps",		  NULL, 0, 0, 0},
		{ "save",		'w', &ChatHandler::HandleSaveWaypoints,		"Save all waypoints",			  NULL, 0, 0, 0},
		{ "deleteall",	'w', &ChatHandler::HandleDeleteWaypoints,	  "Delete all waypoints",			  NULL, 0, 0, 0},
		{ "addfly",		'w', &ChatHandler::HandleWaypointAddFlyCommand, "Adds a flying waypoint", NULL, 0, 0, 0 },
		{ NULL,			0, NULL,									 "",							   NULL, 0, 0  }
	};
	dupe_command_table(waypointCommandTable, _waypointCommandTable);

	static ChatCommand GMCommandTable[] =
	{
		{ "getticket",				'g', &ChatHandler::HandleGMTicketGetAllCommand,  "Gets GM Ticket",			   NULL, 0, 0, 0},
		{ "getticketbyid",			'g', &ChatHandler::HandleGMTicketGetByIdCommand, "Gets GM Ticket by ID",		 NULL, 0, 0, 0},
		{ "deleteticketbyid",		'g', &ChatHandler::HandleGMTicketDelByIdCommand, "Deletes GM Ticket by ID",	  NULL, 0, 0, 0},
		{ "banaccount",				'a', &ChatHandler::HandleAccountBannedCommand,   "Ban account. .account ban name timeperiod", NULL, 0, 0, 0 },
		{ "unbanaccount",			'z', &ChatHandler::HandleAccountUnbanCommand,		"Unbans account x.", NULL, 0, 0, 0 },
		{ "createaccount",			'z', &ChatHandler::HandleCreateAccountCommand,   "Create account. Pass it username password email.", NULL, 0, 0, 0 },
		{ "changeemail",			'z', &ChatHandler::HandleAccountEmailCommand,	   "Sets email address on account. Pass it username email_address.", NULL, 0, 0, 0},
		{ "changeflags",			'z', &ChatHandler::HandleAccountFlagsCommand,	   "Sets flags on account. Pass it username and 0 for Non-TBC or 8 for TBC.", NULL, 0, 0, 0},
		{ "setprivileges",			'z', &ChatHandler::HandleAccountLevelCommand,    "Sets gm level on account. Pass it username and 0,1,2,3,az, etc.", NULL, 0, 0, 0 },
		{ "changepassword",			'z', &ChatHandler::HandleAccountPasswordCommand, "Sets password on account. Pass it username password.",	NULL, 0, 0, 0},
		{ "mute",					'y', &ChatHandler::HandleAccountMuteCommand,		"Mutes account for <timeperiod>.", NULL, 0, 0, 0 },
		{ "unmute",					'y', &ChatHandler::HandleAccountUnmuteCommand,	"Unmutes account <x>", NULL, 0, 0, 0 },
		{ "banchar",				'y', &ChatHandler::HandleBanCharacterCommand,  "Bans character x with or without reason",			  NULL, 0, 0, 0},
		{ "unbanchar",				'y', &ChatHandler::HandleUnBanCharacterCommand,"Unbans character x",			 NULL, 0, 0, 0},
		{ "kick",					'g', &ChatHandler::HandleKickCommand,		  "Kicks player from server",	   NULL, 0, 0, 0},
		{ "list",					'0', &ChatHandler::HandleGMListCommand,		"Shows active GM's",			  NULL, 0, 0, 0},
		{ "off",					'g', &ChatHandler::HandleGMOffCommand,		 "Sets GM tag off",				NULL, 0, 0, 0},
		{ "on",						'g', &ChatHandler::HandleGMOnCommand,		  "Sets GM tag on",				 NULL, 0, 0, 0},
		{ "agro",					'g', &ChatHandler::HandleInvincibleCommand,	".agro - Toggles your agro on and off.", NULL, 0, 0, 0},
		{ "invisible",				'g', &ChatHandler::HandleInvisibleCommand,	 ".invisible - Toggles INVINCIBILITY and INVISIBILITY (mobs won't attack you and nobody can see you, but they can see your chat messages)", NULL, 0, 0, 0},
		{ "allowwhispers",			'g', &ChatHandler::HandleAllowWhispersCommand, "Allows whispers from player <s> while in gmon mode.", NULL, 0, 0, 0 },
		{ "blockwhispers",			'g', &ChatHandler::HandleBlockWhispersCommand, "Blocks whispers from player <s> while in gmon mode.", NULL, 0, 0, 0 },
		{ "disconnectplayer",		'y', &ChatHandler::HandleKillByPlayerCommand, "Disconnects the player with name <s>.", NULL, 0, 0, 0 },
		{ "disconnectaccount",		'y', &ChatHandler::HandleKillBySessionCommand, "Disconnects the session with account name <s>.", NULL, 0, 0, 0 },
		{ "addipban",				'y', &ChatHandler::HandleIPBanCommand, "Adds an address to the IP ban table: <address> [duration]\nDuration must be a number optionally followed by a character representing the calendar subdivision to use (h>hours, d>days, w>weeks, m>months, y>years, default minutes)\nLack of duration results in a permanent ban.", NULL, 0, 0, 0 },
		{ "delipban",				'y', &ChatHandler::HandleIPUnBanCommand, "Deletes an address from the IP ban table: <address>", NULL, 0, 0, 0},
		{ "renamechar",				'y', &ChatHandler::HandleRenameCommand, "Renames character x to y.", NULL, 0, 0, 0 },
		{ "forcerenamechar",		'y', &ChatHandler::HandleForceRenameCommand, "Forces character x to rename his char next login", NULL, 0, 0, 0 },
		{ "whisperblock",			'g', &ChatHandler::HandleWhisperBlockCommand, "Blocks like .gmon except without the <GM> tag", NULL, 0, 0, 0 },
		{ "getstanding",			'p', &ChatHandler::HandleGetStandingCommand, "Gets standing of faction %u.", NULL, 0, 0, 0 },
		{ "setstanding",			'p', &ChatHandler::HandleSetStandingCommand, "Sets stanging of faction %u.", NULL, 0, 0, 0 },
		{ "logcomment" ,			'g' , &ChatHandler::HandleGmLogCommentCommand, "Adds a comment to the GM log for the admins to read." , NULL , 0 , 0 , 0 },
		{ NULL,			0, NULL,									   "",							 NULL, 0, 0  }
	};
	dupe_command_table(GMCommandTable, _gmCommandTable);

	static ChatCommand GuildCommandTable[] =
	{
		{ "create",			'f', &ChatHandler::CreateGuildCommand,  "Creates a guild.",			   NULL, 0, 0, 0},
		{ "rename",			'f', &ChatHandler::HandleRenameGuildCommand, "Renames a guild.",		 NULL, 0, 0, 0},
		{ "members",		'f', &ChatHandler::HandleGuildMembersCommand, "Lists guildmembers and their ranks.",	  NULL, 0, 0, 0},
		{ "removeplayer",	'f', &ChatHandler::HandleGuildRemovePlayerCommand, "Removes a player from a guild.",		 NULL, 0, 0, 0},
		{ "disband",		'f', &ChatHandler::HandleGuildDisbandCommand, "Disbands the guild of your target.",		 NULL, 0, 0, 0},
		{ NULL,			0, NULL,									   "",							 NULL, 0, 0  }
	};
	dupe_command_table(GuildCommandTable, _guildCommandTable);

	static ChatCommand GameObjectCommandTable[] =
	{
		{ "select",		'o', &ChatHandler::HandleGOSelect,   "Selects the nearest GameObject to you",	NULL, 0, 0, 0},
		{ "delete",		'o', &ChatHandler::HandleGODelete,   "Deletes selected GameObject",			  NULL, 0, 0, 0},
		{ "spawn",		'o', &ChatHandler::HandleGOSpawn,	"Spawns a GameObject by ID",				NULL, 0, 0, 0},
		{ "info",		'o', &ChatHandler::HandleGOInfo,	 "Gives you informations about selected GO", NULL, 0, 0, 0},
		{ "activate",	'o', &ChatHandler::HandleGOActivate, "Activates/Opens the selected GO.",		 NULL, 0, 0, 0},
		{ "enable",		'o', &ChatHandler::HandleGOEnable,   "Enables the selected GO for use.",		 NULL, 0, 0, 0},
		{ "scale",		'o', &ChatHandler::HandleGOScale,	"Sets scale of selected GO",				NULL, 0, 0, 0},
		{ "animprogress",'o', &ChatHandler::HandleGOAnimProgress, "Sets anim progress",				   NULL, 0, 0, 0 },
		{ "export",		'o', &ChatHandler::HandleGOExport,   "Exports the current GO selected",		  NULL, 0, 0, 0 },
		{ "move",		'o', &ChatHandler::HandleGOMove, "Moves gameobject to player xyz", NULL, 0, 0, 0 },
		{ "rotate",		'o', &ChatHandler::HandleGORotate, "Rotates gameobject x degrees", NULL, 0, 0, 0 },
		{ NULL,			0, NULL,						   "",										 NULL, 0, 0  }
	};
	dupe_command_table(GameObjectCommandTable, _gameobjectCommandTable);

	static ChatCommand BattlegroundCommandTable[] = 
	{
		{ "setbgscore",		'b', &ChatHandler::HandleSetBGScoreCommand,	"<Teamid> <Score> - Sets battleground score. 2 Arguments. ", NULL, 0, 0, 0},
		{ "start",			'b', &ChatHandler::HandleStartBGCommand,	   "Starts current battleground match.",  NULL, 0, 0, 0},
		{ "pause",			'b', &ChatHandler::HandlePauseBGCommand,	   "Pauses current battleground match.",  NULL, 0, 0, 0},
		{ "info",			'b', &ChatHandler::HandleBGInfoCommnad,		"Displays information about current battleground.", NULL, 0, 0, 0},
		{ "menu",			'b', &ChatHandler::HandleBattlegroundCommand,  "Shows BG Menu",					   NULL, 0, 0, 0 },
		{ "setworldstate",	'b',&ChatHandler::HandleSetWorldStateCommand, "<var> <val> - Var can be in hex. WS Value.", NULL, 0, 0, 0 },
		{ "playsound",		'b', &ChatHandler::HandlePlaySoundCommand,	 "<val>. Val can be in hex.",		   NULL, 0, 0, 0 },
		{ "setstatus",		'b', &ChatHandler::HandleSetBattlefieldStatusCommand,".setbfstatus - NYI.",		   NULL, 0, 0, 0 },
		{ "leave",			'b', &ChatHandler::HandleBattlegroundExitCommand, "Leaves the current battleground.", NULL, 0, 0, 0 },
		{ NULL,			0, NULL,									 "",									NULL, 0, 0  }
	};
	dupe_command_table(BattlegroundCommandTable, _battlegroundCommandTable);


	static ChatCommand CheatCommandTable[] =
	{
		{ "status",		'c', &ChatHandler::HandleShowCheatsCommand, "Shows active cheats.",			 NULL, 0, 0, 0 },
		{ "taxi",		'c', &ChatHandler::HandleTaxiCheatCommand,	 "Enables all taxi nodes.",	   NULL, 0, 0, 0},
		{ "cooldown",   'c', &ChatHandler::HandleCooldownCheatCommand, "Enables no cooldown cheat.",	NULL, 0, 0, 0 },
		{ "casttime",   'c', &ChatHandler::HandleCastTimeCheatCommand, "Enables no cast time cheat.",   NULL, 0, 0, 0 },
		{ "power",		'c', &ChatHandler::HandlePowerCheatCommand, "Disables mana consumption etc.",   NULL, 0, 0, 0 },
		{ "god",		'c', &ChatHandler::HandleGodModeCommand, "Sets god mode, prevents you from taking damage.", NULL, 0, 0, 0 },
		{ "fly",		'c', &ChatHandler::HandleFlyCommand, "Sets fly mode",						   NULL, 0, 0, 0 },
		{ "land",		'c', &ChatHandler::HandleLandCommand, "Unsets fly mode",						NULL, 0, 0, 0 },
		{ "explore",	'c', &ChatHandler::HandleExploreCheatCommand, "Reveals the unexplored parts of the map.", NULL, 0, 0, 0 },
		{ "stack",		'c', &ChatHandler::HandleStackCheatCommand, "Enables aura stacking cheat.", NULL, 0, 0, 0 },
		{ "triggerpass",'c', &ChatHandler::HandleTriggerpassCheatCommand, "Ignores area trigger prerequisites.", NULL, 0, 0, 0 },
		{ NULL,		   0, NULL,							"",									   NULL, 0, 0, 0 },
	};
	dupe_command_table(CheatCommandTable, _cheatCommandTable);

	static ChatCommand pvpCommandTable[] =
	{
		{ "addhonorpoints",		'p', &ChatHandler::HandleAddHonorCommand,	  "Adds x amount of honor points/currency",NULL,0,0,0},
		{ "addkills",			'p', &ChatHandler::HandleAddKillCommand,	   "Adds x amount of honor kills", NULL, 0, 0, 0 },
		{ "globaldailyupdate",	'p', &ChatHandler::HandleGlobalHonorDailyMaintenanceCommand, "Daily honor field moves", NULL, 0, 0, 0},
		{ "singledailyupdate",	'p', &ChatHandler::HandleNextDayCommand, "Daily honor field moves for selected player only", NULL,0,0,0},
		{ "pvpcredit",			'p', &ChatHandler::HandlePVPCreditCommand, "Sends PVP credit packet, with specified rank and points", NULL,0,0,0},
		{ NULL,0,NULL,"",NULL,0,0,0},
	};
	dupe_command_table(pvpCommandTable, _pvpCommandTable);

	static ChatCommand teleCommandTable[] =
	{
		{ "list",			't', &ChatHandler::HandleRecallListCommand,	   "List recall locations",		  NULL, 0, 0, 0},
		{ "port",			't', &ChatHandler::HandleRecallGoCommand,		 "Port to recalled location",	  NULL, 0, 0, 0},
		{ "add",			't', &ChatHandler::HandleRecallAddCommand,		"Add recall location",			NULL, 0, 0, 0},
		{ "del",			't', &ChatHandler::HandleRecallDelCommand,		"Remove a recall location",	   NULL, 0, 0, 0},
		{ "porttarget",		'p', &ChatHandler::HandleRecallPortPlayerCommand, "recall ports player", NULL, 0, 0, 0 },
		{ "appear",			'g', &ChatHandler::HandleAppearCommand,		"Teleports to x's position.",	 NULL, 0, 0, 0},
		{ "summon",			'p', &ChatHandler::HandleSummonCommand,		"Summons x to your position",	 NULL, 0, 0, 0},
		{ "start",			't', &ChatHandler::HandleStartCommand,		 "Teleport's you to a starting location",							   NULL, 0, 0, 0},
		{ "worldport",		't', &ChatHandler::HandleWorldPortCommand,	 "Teleports you to <map> <x> <y> <z> (<o>).",							   NULL, 0, 0, 0},
		{ "gotrig",			't', &ChatHandler::HandleTriggerCommand,	   "Warps to areatrigger <id>",	  NULL, 0, 0, 0 },
		{ "exitinstance",	't', &ChatHandler::HandleExitInstanceCommand,  "Exits current instance, return to entry point.", NULL, 0, 0, 0 },
		{ NULL,		   0,  NULL,										"",							   NULL, 0, 0, 0},
	};
	dupe_command_table(teleCommandTable, _teleportCommandTable);

	static ChatCommand questCommandTable[] =
	{
		{ "addboth",   'q', &ChatHandler::HandleQuestAddBothCommand,	"Add quest <id> to the targeted NPC as start & finish",	NULL, 0, 0, 0},
		{ "addfinish", 'q', &ChatHandler::HandleQuestAddFinishCommand,	"Add quest <id> to the targeted NPC as finisher",		NULL, 0, 0, 0},
		{ "addstart",  'q', &ChatHandler::HandleQuestAddStartCommand,	"Add quest <id> to the targeted NPC as starter",		NULL, 0, 0, 0},
		{ "delboth",   'q', &ChatHandler::HandleQuestDelBothCommand,	"Delete quest <id> from the targeted NPC as start & finish",	NULL, 0, 0, 0},
		{ "delfinish", 'q', &ChatHandler::HandleQuestDelFinishCommand,	"Delete quest <id> from the targeted NPC as finisher",	NULL, 0, 0, 0},
		{ "delstart",  'q', &ChatHandler::HandleQuestDelStartCommand,	"Delete quest <id> from the targeted NPC as starter",	NULL, 0, 0, 0},
		{ "complete",  'q', &ChatHandler::HandleQuestFinishCommand,	    "Complete/Finish quest <id>",							NULL, 0, 0, 0},
		{ "finisher",  'q', &ChatHandler::HandleQuestFinisherCommand,	"Lookup quest finisher for quest <id>",					NULL, 0, 0, 0},
		{ "item",	   'q', &ChatHandler::HandleQuestItemCommand,		"Lookup itemid necessary for quest <id>",				NULL, 0, 0, 0},
		{ "list",	   'q', &ChatHandler::HandleQuestListCommand,		"Lists the quests for the npc <id>",					NULL, 0, 0, 0},
		{ "load",	   'q', &ChatHandler::HandleQuestLoadCommand,		"Loads quests from database",							NULL, 0, 0, 0},
		{ "lookup",	   'q', &ChatHandler::HandleQuestLookupCommand,		"Looks up quest string x",								NULL, 0, 0, 0},
		{ "giver",	   'q', &ChatHandler::HandleQuestGiverCommand,		"Lookup quest giver for quest <id>",					NULL, 0, 0, 0},
		{ "remove",	   'q', &ChatHandler::HandleQuestRemoveCommand,		"Removes the quest <id> from the targeted player",		NULL, 0, 0, 0},
		{ "reward",	   'q', &ChatHandler::HandleQuestRewardCommand,		"Shows reward for quest <id>",							NULL, 0, 0, 0},
		{ "status",	   'q', &ChatHandler::HandleQuestStatusCommand,		"Lists the status of quest <id>",						NULL, 0, 0, 0},
		{ "spawn",	   'q', &ChatHandler::HandleQuestSpawnCommand,		"Port to spawn location for quest <id>",				NULL, 0, 0, 0},
		{ "start",	   'q', &ChatHandler::HandleQuestStartCommand,		"Starts quest <id>",									NULL, 0, 0, 0},
		{ NULL,		    0,  NULL,										"",														NULL, 0, 0, 0},
	};
	dupe_command_table(questCommandTable, _questCommandTable);

	static ChatCommand serverCommandTable[] =
	{
		{ "announce",		's', &ChatHandler::HandleAnnounceCommand,	  "Sends Msg To All",			   NULL, 0, 0, 0},
		{ "wannounce",		's', &ChatHandler::HandleWAnnounceCommand,	 "Sends Widescreen Msg To All",	NULL, 0, 0, 0},
		{ "masssummon",		'z', &ChatHandler::HandleMassSummonCommand, ".masssummon - Summons all players.", NULL, 0, 0, 0},
		{ "info",			'0', &ChatHandler::HandleInfoCommand,		  "Server info",					NULL, 0, 0, 0},
		{ "saveall",		's', &ChatHandler::HandleSaveAllCommand,	   "Save's all playing characters",  NULL, 0, 0, 0},
		{ "setmotd",		's', &ChatHandler::HandleSetMotdCommand,	   "Sets MOTD",					  NULL, 0, 0, 0 },
		{ "reloadtable",	'm', &ChatHandler::HandleDBReloadCommand,	  "Reloads some of the database tables", NULL, 0, 0, 0 },
		{ "shutdown",		'z', &ChatHandler::HandleShutdownCommand, "Initiates server shutdown in <x> seconds.", NULL, 0, 0, 0 },
		{ "restart",		'z', &ChatHandler::HandleShutdownRestartCommand, "Initiates server restart in <x> seconds.", NULL, 0, 0, 0 },
		{ "castall",		'z', &ChatHandler::HandleCastAllCommand, "Makes all players online cast spell <x>.", NULL, 0, 0, 0},
		{ "dispelall",		'z', &ChatHandler::HandleDispelAllCommand, "dispels all negative or positive auras on the realm. Specify 1 for positive, otherwise, negative auras will be removed.",NULL,0,0,0},
		{ "playsound",		'z', &ChatHandler::HandleGlobalPlaySoundCommand, "plays a sound id to the entire server", NULL, 0, 0, 0 },
		{ "rehashconfig",	'z', &ChatHandler::HandleRehashCommand, "reloads the config file", NULL, 0, 0, 0 },
		{ "renameallinvalidchars", 'z', &ChatHandler::HandleRenameAllCharacter, "renames all invalid character names on the server", NULL, 0,0, 0 },
		{ NULL,		    0,  NULL,										"",														NULL, 0, 0, 0},
	};
	dupe_command_table(serverCommandTable, _serverCommandTable);

	static ChatCommand lookupCommandTable[] =
	{
		{ "item",		'l', &ChatHandler::HandleLookupItemCommand, "looks up an item string", NULL, 0, 0, 0 },
		{ "quest",		'l', &ChatHandler::HandleQuestLookupCommand, "looks up a quest string", NULL, 0, 0, 0 },
		{ "creature",	'l', &ChatHandler::HandleLookupCreatureCommand, "looks up an item string", NULL, 0, 0, 0 },
		{ NULL,		    0,  NULL,										"",														NULL, 0, 0, 0},
	};
	dupe_command_table(lookupCommandTable, _lookupCommandTable);

	static ChatCommand commandTable[] = {
		{ "commands",		'0', &ChatHandler::HandleCommandsCommand,		"shows commands",				 NULL, 0, 0, 0},
		{ "help",			'0', &ChatHandler::HandleHelpCommand,			"shows help for commands",		 NULL, 0, 0, 0},
		{ "gps",			'0', &ChatHandler::HandleGPSCommand,		   "shows your current position",				 NULL, 0, 0, 0},
		{ "createarenateam",'g', &ChatHandler::HandleCreateArenaTeamCommands, "creates an arena team", NULL, 0, 0, 0 },
		{ "unit",			'u', NULL,									 "",				 unitCommandTable, 0, 0, 0},
		{ "waypoint",		'w', NULL,									 "",			   waypointCommandTable, 0, 0, 0},
		{ "debug",			'd', NULL,									 "",				  debugCommandTable, 0, 0, 0},
		{ "gm",				'g', NULL,									 "",			   GMCommandTable, 0, 0, 0},
		{ "gobject",		'o', NULL,									 "",			 GameObjectCommandTable, 0, 0, 0},
		{ "battleground",	'b', NULL,									 "",		   BattlegroundCommandTable, 0, 0, 0},
		{ "cheat"	   ,	'c', NULL,									 "",				  CheatCommandTable, 0, 0, 0},
		{ "pvp"	   ,		'm', NULL,									 "",				  pvpCommandTable, 0, 0, 0},
		{ "quest",			'q', NULL,									 "",				 questCommandTable, 0, 0, 0},
		{ "teleport",		't', NULL,									 "",				 teleCommandTable, 0, 0, 0},
		{ "guild",			'f', NULL,									 "",				 GuildCommandTable, 0, 0, 0},
		{ "server",			's', NULL,									 "",				 serverCommandTable, 0, 0, 0},
		{ "lookup",			'l', NULL,									 "",				 lookupCommandTable, 0, 0, 0},
		{ NULL,		  0, NULL,										 "",							   NULL, 0, 0  }
	};
	dupe_command_table(commandTable, _commandTable);

	/* set the correct pointers */
	ChatCommand * p = &_commandTable[0];
	while(p->Name != 0)
	{
		if(p->ChildCommands != 0)
		{
			// Set the correct pointer.
			ChatCommand * np = GetSubCommandTable(p->Name);
			ASSERT(np);
			p->ChildCommands = np;
		}
		++p;
	}
}

ChatHandler::ChatHandler()
{
	new CommandTableStorage;
	CommandTableStorage::getSingleton().Init();
	SkillNameManager = new SkillNameMgr;
}

ChatHandler::~ChatHandler()
{
	CommandTableStorage::getSingleton().Dealloc();
	delete CommandTableStorage::getSingletonPtr();
	delete SkillNameManager;
}

bool ChatHandler::hasStringAbbr(const char* s1, const char* s2)
{
	for(;;)
	{
		if( !*s2 )
			return true;
		else if( !*s1 )
			return false;
		else if( tolower( *s1 ) != tolower( *s2 ) )
			return false;
		s1++; s2++;
	}
}

void ChatHandler::SendMultilineMessage(WorldSession *m_session, const char *str)
{
	char * start = (char*)str, *end;
	for(;;)
	{
        end = strchr(start, '\n');
		if(!end)
			break;

		*end = '\0';
		SystemMessage(m_session, start);
		start = end + 1;
	}
	if(*start != '\0')
		SystemMessage(m_session, start);
}

bool ChatHandler::ExecuteCommandInTable(ChatCommand *table, const char* text, WorldSession *m_session)
{
	std::string cmd = "";

	// get command
	while (*text != ' ' && *text != '\0')
	{
		cmd += *text;
		text++;
	}

	while (*text == ' ') text++; // skip whitespace

	if(!cmd.length())
		return false;

	for(uint32 i = 0; table[i].Name != NULL; i++)
	{
		if(!hasStringAbbr(table[i].Name, cmd.c_str()))
			continue;

		if(table[i].CommandGroup != '0' && !m_session->CanUseCommand(table[i].CommandGroup))
			continue;

		if(table[i].ChildCommands != NULL)
		{
			if(!ExecuteCommandInTable(table[i].ChildCommands, text, m_session))
			{
				if(table[i].Help != "")
					SendMultilineMessage(m_session, table[i].Help.c_str());
				else
				{
					GreenSystemMessage(m_session, "Available Subcommands:");
					for(uint32 k=0; table[i].ChildCommands[k].Name;k++)
					{
						if(table[i].ChildCommands[k].CommandGroup != '0' && m_session->CanUseCommand(table[i].ChildCommands[k].CommandGroup))
							SystemMessage(m_session, " |cffffff00%s|r |cffff0000-|r |cff00ccff%s|r", table[i].ChildCommands[k].Name, table[i].ChildCommands[k].Help.size() ? table[i].ChildCommands[k].Help.c_str() : "No Help Available");
					}
				}
			}

			return true;
		}
		
		// Check for field-based commands
		if(table[i].Handler == NULL && (table[i].MaxValueField || table[i].NormalValueField))
		{
			bool result = false;
			if(strlen(text) == 0)
			{
				RedSystemMessage(m_session, "No values specified.");
			}
			if(table[i].ValueType == 2)
				result = CmdSetFloatField(m_session, table[i].NormalValueField, table[i].MaxValueField, table[i].Name, text);
			else
				result = CmdSetValueField(m_session, table[i].NormalValueField, table[i].MaxValueField, table[i].Name, text);
			if(!result)
				RedSystemMessage(m_session, "Must be in the form of (command) <value>, or, (command) <value> <maxvalue>");
		}
		else
		{
			if(!(this->*(table[i].Handler))(text, m_session))
			{
				if(table[i].Help != "")
					SendMultilineMessage(m_session, table[i].Help.c_str());
				else
				{
					RedSystemMessage(m_session, "Incorrect syntax specified. Try .help %s for the correct syntax.", table[i].Name);
				}
			}
		}

		return true;
	}
	return false;
}

int ChatHandler::ParseCommands(const char* text, WorldSession *session)
{
	if (!session)
		return 0;

	if(!*text)
		return 0;

	if(session->GetPermissionCount() == 0 && sWorld.m_reqGmForCommands)
		return 0;

	if(text[0] != '!' && text[0] != '.' && text[0] != '@') // let's not confuse users
		return 0;

	/* skip '..' :P that pisses me off */
	if(text[1] == '.')
		return 0;

	text++;

	if(!ExecuteCommandInTable(CommandTableStorage::getSingleton().Get(), text, session))
	{
		SystemMessage(session, "There is no such command, or you do not have access to it.");
	}

	return 1;
}

WorldPacket * ChatHandler::FillMessageData( uint32 type, uint32 language, const char *message,uint64 guid , uint8 flag) const
{
	//Packet    structure
	//uint8	    type;
	//uint32	language;
	//uint64	guid;
	//uint64	guid;
	//uint32	len_of_text;
	//char	    text[];		 // not sure ? i think is null terminated .. not null terminated
	//uint8	    afk_state;
	ASSERT(type != CHAT_MSG_CHANNEL);
	   //channels are handled in channel handler and so on
	uint32 messageLength = (uint32)strlen((char*)message) + 1;

	WorldPacket *data = new WorldPacket(SMSG_MESSAGECHAT, messageLength + 30);

	*data << (uint8)type;
	*data << language;

	*data << guid;
	*data << uint32(0);

	*data << guid;

	*data << messageLength;
	*data << message;

	*data << uint8(flag);
	return data;
}

WorldPacket* ChatHandler::FillSystemMessageData(const char *message) const
{
	uint32 messageLength = (uint32)strlen((char*)message) + 1;

	WorldPacket * data = new WorldPacket(SMSG_MESSAGECHAT, 30 + messageLength);
	*data << (uint8)CHAT_MSG_SYSTEM;
	*data << (uint32)LANG_UNIVERSAL;
	
	*data << (uint64)0; // Who cares about guid when there's no nickname displayed heh ?
	*data << (uint32)0;
	*data << (uint64)0;

	*data << messageLength;
	*data << message;

	*data << uint8(0);

	return data;
}

Player * ChatHandler::getSelectedChar(WorldSession *m_session, bool showerror)
{
	uint64 guid;
	Player *chr;
	
	guid = m_session->GetPlayer()->GetSelection();
	
	if (guid == 0)
	{
		if(showerror) 
			GreenSystemMessage(m_session, "Auto-targeting self.");
		chr = m_session->GetPlayer(); // autoselect
	}
	else
		chr = m_session->GetPlayer()->GetMapMgr()->GetPlayer((uint32)guid);
	
	if(chr == NULL)
	{
		if(showerror) 
			RedSystemMessage(m_session, "This command requires that you select a player.");
		return NULL;
	}

	return chr;
}

Creature * ChatHandler::getSelectedCreature(WorldSession *m_session, bool showerror)
{
	uint64 guid;
	Creature *creature;

	guid = m_session->GetPlayer()->GetSelection();
	if(GUID_HIPART(guid)==HIGHGUID_PET)
		creature = m_session->GetPlayer()->GetMapMgr()->GetPet((uint32)guid);
	else
		creature = m_session->GetPlayer()->GetMapMgr()->GetCreature((uint32)guid);
	
	if(creature != NULL)
		return creature;
	else
	{
		if(showerror) 
			RedSystemMessage(m_session, "This command requires that you select a creature.");
		return NULL;
	}
}

void ChatHandler::SystemMessage(WorldSession *m_session, const char* message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	WorldPacket * data = FillSystemMessageData(msg1);
	if(m_session != NULL) 
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::ColorSystemMessage(WorldSession *m_session, const char* colorcode, const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	char msg[1024];
	snprintf(msg, 1024, "%s%s|r", colorcode, msg1);
	WorldPacket * data = FillSystemMessageData(msg);
	if(m_session != NULL) 
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::RedSystemMessage(WorldSession *m_session, const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	char msg[1024];
	snprintf(msg, 1024,"%s%s|r", MSG_COLOR_LIGHTRED/*MSG_COLOR_RED*/, msg1);
	WorldPacket * data = FillSystemMessageData(msg);
	if(m_session != NULL) 
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::GreenSystemMessage(WorldSession *m_session, const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	char msg[1024];
	snprintf(msg, 1024, "%s%s|r", MSG_COLOR_GREEN, msg1);
	WorldPacket * data = FillSystemMessageData(msg);
	if(m_session != NULL) 
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::BlueSystemMessage(WorldSession *m_session, const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	char msg[1024];
	snprintf(msg, 1024,"%s%s|r", MSG_COLOR_LIGHTBLUE, msg1);
	WorldPacket * data = FillSystemMessageData(msg);
	if(m_session != NULL) 
		m_session->SendPacket(data);
	delete data;
}

void ChatHandler::RedSystemMessageToPlr(Player* plr, const char *message, ...)
{
	if( !message || !plr->GetSession() ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	RedSystemMessage(plr->GetSession(), (const char*)msg1);
}

void ChatHandler::GreenSystemMessageToPlr(Player* plr, const char *message, ...)
{
	if( !message || !plr->GetSession() ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	GreenSystemMessage(plr->GetSession(), (const char*)msg1);
}

void ChatHandler::BlueSystemMessageToPlr(Player* plr, const char *message, ...)
{
	if( !message || !plr->GetSession() ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	BlueSystemMessage(plr->GetSession(), (const char*)msg1);
}

void ChatHandler::SystemMessageToPlr(Player *plr, const char* message, ...)
{
	if( !message || !plr->GetSession() ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024,message,ap);
	SystemMessage(plr->GetSession(), msg1);
}

bool ChatHandler::CmdSetValueField(WorldSession *m_session, uint32 field, uint32 fieldmax, const char *fieldname, const char *args)
{
	if(!args) return false;
	char* pvalue = strtok((char*)args, " ");
	uint32 mv, av;

	if (!pvalue)
		return false;
	else
		av = atol(pvalue);

	if(fieldmax)
	{
		char* pvaluemax = strtok(NULL, " ");   
		if (!pvaluemax)
			return false;  
		else
			mv = atol(pvaluemax);
	}
	else
	{
		mv = 0;
	}

	if (av <= 0 && mv > 0)
	{  
		RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
		return true;   
	}
	if(fieldmax)
	{
		if(mv < av || mv <= 0)
		{
			RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
			return true;  
		}
	}

	Player *plr = getSelectedChar(m_session, false);
	if(plr)
	{  
		sGMLog.writefromsession(m_session, "used modify field value: %s, %u on %s", fieldname, av, plr->GetName());
		if(fieldmax)
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %d/%d.", fieldname, plr->GetName(), av, mv);
			GreenSystemMessageToPlr(plr, "%s set your %s to %d/%d.", m_session->GetPlayer()->GetName(), fieldname, av, mv);
		}
		else
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %d.", fieldname, plr->GetName(), av);
			GreenSystemMessageToPlr(plr, "%s set your %s to %d.", m_session->GetPlayer()->GetName(), fieldname, av);
		}

		if(field == UNIT_FIELD_STAT1) av /= 2;
		if(field == UNIT_FIELD_BASE_HEALTH) 
		{
			plr->SetUInt32Value(UNIT_FIELD_HEALTH, av);
		}

		plr->SetUInt32Value(field, av);

		if(fieldmax) {
			plr->SetUInt32Value(fieldmax, mv);
		}
	}
	else
	{
		Creature *cr = getSelectedCreature(m_session, false);
		if(cr)
		{
			if(!(field < UNIT_END && fieldmax < UNIT_END)) return false;
			std::string creaturename = "Unknown Being";
			if(cr->GetCreatureName())
				creaturename = cr->GetCreatureName()->Name;
			if(fieldmax)
				BlueSystemMessage(m_session, "Setting %s of %s to %d/%d.", fieldname, creaturename.c_str(), av, mv);
			else
				BlueSystemMessage(m_session, "Setting %s of %s to %d.", fieldname, creaturename.c_str(), av);
			sGMLog.writefromsession(m_session, "used modify field value: [creature]%s, %u on %s", fieldname, av, creaturename.c_str());
			if(field == UNIT_FIELD_STAT1) av /= 2;
			if(field == UNIT_FIELD_BASE_HEALTH) 
				cr->SetUInt32Value(UNIT_FIELD_HEALTH, av);

			switch(field)
			{
			case UNIT_FIELD_FACTIONTEMPLATE:
				{
					if(cr->m_spawn)
						WorldDatabase.Execute("UPDATE creature_spawns SET faction = %u WHERE entry = %u", av, cr->m_spawn->entry);
				}break;
			case UNIT_NPC_FLAGS:
				{
					if(cr->proto)
						WorldDatabase.Execute("UPDATE creature_proto SET npcflags = %u WHERE entry = %u", av, cr->proto->Id);
				}break;
			}

			cr->SetUInt32Value(field, av);

			if(fieldmax) {
				cr->SetUInt32Value(fieldmax, mv);
			}
			// reset faction
			if(field == UNIT_FIELD_FACTIONTEMPLATE)
				cr->_setFaction();

			cr->SaveToDB();
		}
		else
		{
			RedSystemMessage(m_session, "Invalid Selection.");
		}
	}
	return true;
}

bool ChatHandler::CmdSetFloatField(WorldSession *m_session, uint32 field, uint32 fieldmax, const char *fieldname, const char *args)
{
	char* pvalue = strtok((char*)args, " ");
	float mv, av;

	if (!pvalue)
		return false;
	else
		av = (float)atof(pvalue);

	if(fieldmax)
	{
		char* pvaluemax = strtok(NULL, " ");   
		if (!pvaluemax)
			return false;  
		else
			mv = (float)atof(pvaluemax);
	}
	else
	{
		mv = 0;
	}

	if (av <= 0)
	{  
		RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
		return true;   
	}
	if(fieldmax)
	{
		if(mv < av || mv <= 0)
		{
			RedSystemMessage(m_session, "Values are invalid. Value must be < max (if max exists), and both must be > 0.");
			return true;  
		}
	}

	Player *plr = getSelectedChar(m_session, false);
	if(plr)
	{  
		sGMLog.writefromsession(m_session, "used modify field value: %s, %f on %s", fieldname, av, plr->GetName());
		if(fieldmax)
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %.1f/%.1f.", fieldname, plr->GetName(), av, mv);
			GreenSystemMessageToPlr(plr, "%s set your %s to %.1f/%.1f.", m_session->GetPlayer()->GetName(), fieldname, av, mv);
		}
		else
		{
			BlueSystemMessage(m_session, "You set the %s of %s to %.1f.", fieldname, plr->GetName(), av);
			GreenSystemMessageToPlr(plr, "%s set your %s to %.1f.", m_session->GetPlayer()->GetName(), fieldname, av);
		}
		plr->SetFloatValue(field, av);
		if(fieldmax) plr->SetFloatValue(fieldmax, mv);
	}
	else
	{
		Creature *cr = getSelectedCreature(m_session, false);
		if(cr)
		{
			if(!(field < UNIT_END && fieldmax < UNIT_END)) return false;
			std::string creaturename = "Unknown Being";
			if(cr->GetCreatureName())
				creaturename = cr->GetCreatureName()->Name;
			if(fieldmax)
				BlueSystemMessage(m_session, "Setting %s of %s to %.1f/%.1f.", fieldname, creaturename.c_str(), av, mv);
			else
				BlueSystemMessage(m_session, "Setting %s of %s to %.1f.", fieldname, creaturename.c_str(), av);
			cr->SetFloatValue(field, av);
			sGMLog.writefromsession(m_session, "used modify field value: [creature]%s, %f on %s", fieldname, av, creaturename.c_str());
			if(fieldmax) {
				cr->SetFloatValue(fieldmax, mv);
			}
			//cr->SaveToDB();
		}
		else
		{
			RedSystemMessage(m_session, "Invalid Selection.");
		}
	}
	return true;
}

bool ChatHandler::HandleGetPosCommand(const char* args, WorldSession *m_session)
{
	/*if(m_session->GetPlayer()->GetSelection() == 0) return false;
	Creature *creature = objmgr.GetCreature(m_session->GetPlayer()->GetSelection());

	if(!creature) return false;
	BlueSystemMessage(m_session, "Creature Position: \nX: %f\nY: %f\nZ: %f\n", creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ());
	return true;*/

	uint32 spell = atol(args);
	SpellEntry *se = dbcSpell.LookupEntry(spell);
	if(se)
		BlueSystemMessage(m_session, "SpellIcon for %d is %d", se->Id, se->field114);
	return true;
}
