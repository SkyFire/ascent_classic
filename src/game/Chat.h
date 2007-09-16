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

#ifndef WOWSERVER_CHAT_H
#define WOWSERVER_CHAT_H

class ChatHandler;
class WorldSession;
class Player;
class Unit;

enum ChatMsg
{
	CHAT_MSG_ADDON									= -1,
	CHAT_MSG_SAY									= 0x00,
	CHAT_MSG_PARTY									= 0x01,
	CHAT_MSG_RAID									= 0x02,
	CHAT_MSG_GUILD									= 0x03,
	CHAT_MSG_OFFICER								= 0x04,
	CHAT_MSG_YELL									= 0x05,
	CHAT_MSG_WHISPER								= 0x06,
	CHAT_MSG_WHISPER_INFORM							= 0x07,
	CHAT_MSG_EMOTE									= 0x08,
	CHAT_MSG_TEXT_EMOTE								= 0x09,
	CHAT_MSG_SYSTEM									= 0x0A,
	CHAT_MSG_MONSTER_SAY							= 0x0B,
	CHAT_MSG_MONSTER_YELL							= 0x0C,
	CHAT_MSG_MONSTER_EMOTE							= 0x0D,
	CHAT_MSG_CHANNEL								= 0x0E,
	CHAT_MSG_CHANNEL_JOIN							= 0x0F,
	CHAT_MSG_CHANNEL_LEAVE							= 0x10,
	CHAT_MSG_CHANNEL_LIST							= 0x11,
	CHAT_MSG_CHANNEL_NOTICE							= 0x12,
	CHAT_MSG_CHANNEL_NOTICE_USER					= 0x13,
	CHAT_MSG_AFK									= 0x14,
	CHAT_MSG_DND									= 0x15,
	CHAT_MSG_COMBAT_LOG								= 0x16,
	CHAT_MSG_IGNORED								= 0x17,
	CHAT_MSG_SKILL									= 0x18,
	CHAT_MSG_LOOT									= 0x19,
	CHAT_MSG_BATTLEGROUND_EVENT						= 0x53,
	CHAT_MSG_RAIDLEADER								= 0x57,
	CHAT_MSG_RAIDWARNING							= 0x58,
};


enum Languages
{
	LANG_UNIVERSAL								= 0x00,
	LANG_ORCISH								   = 0x01,
	LANG_DARNASSIAN							   = 0x02,
	LANG_TAURAHE								  = 0x03,
	LANG_DWARVISH								 = 0x06,
	LANG_COMMON								   = 0x07,
	LANG_DEMONIC								  = 0x08,
	LANG_TITAN									= 0x09,
	LANG_THELASSIAN							   = 0x0A,
	LANG_DRACONIC								 = 0x0B,
	LANG_KALIMAG								  = 0x0C,
	LANG_GNOMISH								  = 0x0D,
	LANG_TROLL									= 0x0E,
	NUM_LANGUAGES								 = 0x0E
};

/*#define MSG_COLOR_YELLOW	"|r"
#define MSG_COLOR_RED	   "|cffff2020"
#define MSG_COLOR_GREEN	 "|c1f40af20"
#define MSG_COLOR_LIGHTRED  "|cffff6060"*/

#define MSG_COLOR_LIGHTRED	  "|cffff6060"
#define MSG_COLOR_LIGHTBLUE	 "|cff00ccff"
#define MSG_COLOR_BLUE		  "|cff0000ff"
#define MSG_COLOR_GREEN		 "|cff00ff00"
#define MSG_COLOR_RED		   "|cffff0000"
#define MSG_COLOR_GOLD		  "|cffffcc00"
#define MSG_COLOR_GREY		  "|cff888888"
#define MSG_COLOR_WHITE		 "|cffffffff"
#define MSG_COLOR_SUBWHITE	  "|cffbbbbbb"
#define MSG_COLOR_MAGENTA	   "|cffff00ff"
#define MSG_COLOR_YELLOW		"|cffffff00"
#define MSG_COLOR_CYAN		  "|cff00ffff"

#define CHECKSESSION if(m_session == NULL) return NULL; \
	if(m_session->GetPlayer() == NULL) return NULL;

class ChatCommand
{
public:
	const char *	   Name;
	char			   CommandGroup;
	bool (ChatHandler::*Handler)(const char* args, WorldSession *m_session) ;
	std::string		Help;
	ChatCommand *    ChildCommands;
	uint32			 NormalValueField;
	uint32			 MaxValueField;
	uint16			 ValueType;	// 0 = nothing, 1 = uint, 2 = float
};

class SERVER_DECL CommandTableStorage : public Singleton<CommandTableStorage>
{
	ChatCommand * _modifyCommandTable;
	ChatCommand * _debugCommandTable;
	ChatCommand * _waypointCommandTable;
	ChatCommand * _GMTicketCommandTable;
	ChatCommand * _GameObjectCommandTable;
	ChatCommand * _BattlegroundCommandTable;
	ChatCommand * _NPCCommandTable;
	ChatCommand * _CheatCommandTable;
	ChatCommand * _honorCommandTable;
	ChatCommand * _petCommandTable;
	ChatCommand * _recallCommandTable;
	ChatCommand * _commandTable;

	ChatCommand * GetSubCommandTable(const char * name);
public:
	void Init();
	void Dealloc();
	void Load();
	void Override(const char * command, const char * level);
	inline ChatCommand * Get() { return _commandTable; }
};

class SERVER_DECL ChatHandler : public Singleton<ChatHandler>
{
	friend class CommandTableStorage;
public:
	ChatHandler();
	~ChatHandler();

	WorldPacket * FillMessageData( uint32 type, uint32 language,  const char* message,uint64 guid, uint8 flag = 0) const;
	WorldPacket * FillSystemMessageData( const char* message ) const;

	int ParseCommands(const char* text, WorldSession *session);
	
	void SystemMessage(WorldSession *m_session, const char *message, ...);
	void ColorSystemMessage(WorldSession *m_session, const char *colorcode, const char *message, ...);
	void RedSystemMessage(WorldSession *m_session, const char *message, ...);
	void GreenSystemMessage(WorldSession *m_session, const char *message, ...);
	void BlueSystemMessage(WorldSession *m_session, const char *message, ...);
	void RedSystemMessageToPlr(Player* plr, const char *message, ...);
	void GreenSystemMessageToPlr(Player* plr, const char *message, ...);
	void BlueSystemMessageToPlr(Player* plr, const char *message, ...);
	void SystemMessageToPlr(Player *plr, const char *message, ...);
	   
protected:

	bool hasStringAbbr(const char* s1, const char* s2);
	void SendMultilineMessage(WorldSession *m_session, const char *str);

	bool ExecuteCommandInTable(ChatCommand *table, const char* text, WorldSession *m_session);
	bool ShowHelpForCommand(WorldSession *m_session, ChatCommand *table, const char* cmd);

	ChatCommand* getCommandTable();

	// Level 0 commands
	bool HandleHelpCommand(const char* args, WorldSession *m_session);
	bool HandleCommandsCommand(const char* args, WorldSession *m_session);
	bool HandleNYICommand(const char* args, WorldSession *m_session);
	bool HandleAcctCommand(const char* args, WorldSession *m_session);
	bool HandleStartCommand(const char* args, WorldSession *m_session);
	bool HandleInfoCommand(const char* args, WorldSession *m_session);
	bool HandleDismountCommand(const char* args, WorldSession *m_session);
	bool HandleSaveCommand(const char* args, WorldSession *m_session);
	bool HandleGMListCommand(const char* args, WorldSession *m_session);

	// Level 1 commands
	bool CmdSetValueField(WorldSession *m_session, uint32 field, uint32 fieldmax, const char *fieldname, const char* args);
	bool CmdSetFloatField(WorldSession *m_session, uint32 field, uint32 fieldmax, const char *fieldname, const char* args);
	bool HandleSummonCommand(const char* args, WorldSession *m_session);
	bool HandleAppearCommand(const char* args, WorldSession *m_session);
	bool HandleAnnounceCommand(const char* args, WorldSession *m_session);
	bool HandleWAnnounceCommand(const char* args, WorldSession *m_session);
	bool HandleGMOnCommand(const char* args, WorldSession *m_session);
	bool HandleGMOffCommand(const char* args, WorldSession *m_session);
	bool HandleGPSCommand(const char* args, WorldSession *m_session);
	bool HandleKickCommand(const char* args, WorldSession *m_session);
	bool HandleTaxiCheatCommand(const char* args, WorldSession *m_session);
	bool HandleModifySpeedCommand(const char* args, WorldSession *m_session);

	// Debug Commands
	bool HandleDebugInFrontCommand(const char* args, WorldSession *m_session);
	bool HandleShowReactionCommand(const char* args, WorldSession *m_session);
	bool HandleAIMoveCommand(const char* args, WorldSession *m_session);
	bool HandleMoveInfoCommand(const char* args, WorldSession *m_session);
	bool HandleDistanceCommand(const char* args, WorldSession *m_session);
	bool HandleFaceCommand(const char* args, WorldSession *m_session);
	bool HandleSetBytesCommand(const char* args, WorldSession *m_session);
	bool HandleGetBytesCommand(const char* args, WorldSession *m_session);
	bool HandleDebugLandWalk(const char* args, WorldSession *m_session);
	bool HandleDebugUnroot(const char* args, WorldSession *m_session);
	bool HandleDebugRoot(const char* args, WorldSession *m_session);
	bool HandleDebugWaterWalk(const char* args, WorldSession *m_session);
	bool HandleAggroRangeCommand(const char* args, WorldSession *m_session);
	bool HandleKnockBackCommand(const char* args, WorldSession *m_session);
	bool HandleFadeCommand(const char* args, WorldSession *m_session);
	bool HandleThreatModCommand(const char* args, WorldSession *m_session);
	bool HandleCalcThreatCommand(const char* args, WorldSession *m_session);
	bool HandleThreatListCommand(const char* args, WorldSession *m_session);
	bool HandleNpcSpawnLinkCommand(const char* args, WorldSession *m_session);
	bool HandleGoStateLinkCommand(const char* args, WorldSession *m_session);
	bool HandleSilentPlayerCommand(const char* args, WorldSession *m_session);
	bool HandleDebugDumpCoordsCommmand(const char * args, WorldSession * m_session);
	bool HandleSendRunSpeedChange(const char * args, WorldSession * m_session);

	//WayPoint Commands
	bool HandleWPAddCommand(const char* args, WorldSession *m_session);
	bool HandleWPShowCommand(const char* args, WorldSession *m_session);
	bool HandleWPHideCommand(const char* args, WorldSession *m_session);
	bool HandleWPDeleteCommand(const char* args, WorldSession *m_session);
	bool HandleWPFlagsCommand(const char* args, WorldSession *m_session);
	bool HandleWPMoveHereCommand(const char* args, WorldSession *m_session);
	bool HandleWPWaitCommand(const char* args, WorldSession *m_session);
	bool HandleWPEmoteCommand(const char* args, WorldSession *m_session);
	bool HandleWPSkinCommand(const char* args, WorldSession *m_session);
	bool HandleWPChangeNoCommand(const char* args, WorldSession *m_session);
	bool HandleWPInfoCommand(const char* args, WorldSession *m_session);
	bool HandleWPMoveTypeCommand(const char* args, WorldSession *m_session);
	bool HandleSaveWaypoints(const char* args, WorldSession * m_session);
	bool HandleGenerateWaypoints(const char* args, WorldSession * m_session);
	bool HandleDeleteWaypoints(const char* args, WorldSession * m_session);

	// Level 2 commands
	bool HandleGUIDCommand(const char* args, WorldSession *m_session);
	bool HandleNameCommand(const char* args, WorldSession *m_session);
	bool HandleSubNameCommand(const char* args, WorldSession *m_session);
	bool HandleDeleteCommand(const char* args, WorldSession *m_session);
	bool HandleDeMorphCommand(const char* args, WorldSession *m_session);
	bool HandleItemCommand(const char* args, WorldSession *m_session);
	bool HandleItemRemoveCommand(const char* args, WorldSession *m_session);
	bool HandleRunCommand(const char* args, WorldSession *m_session);
	bool HandleNPCFlagCommand(const char* args, WorldSession *m_session);
	bool HandleSaveAllCommand(const char* args, WorldSession *m_session);
	bool HandleRegenerateGossipCommand(const char* args, WorldSession *m_session);
	bool CreateGuildCommand(const char* args, WorldSession *m_session);
	bool HandleStartBGCommand(const char* args, WorldSession *m_session);
	bool HandlePauseBGCommand(const char* args, WorldSession *m_session);
	bool HandleResetScoreCommand(const char* args, WorldSession *m_session);
	bool HandleBGInfoCommnad(const char *args, WorldSession *m_session);
	bool HandleInvincibleCommand(const char *args, WorldSession *m_session);
	bool HandleInvisibleCommand(const char *args, WorldSession *m_session);
	bool HandleKillCommand(const char *args, WorldSession *m_session);
	bool HandleAddSpiritCommand(const char *args, WorldSession *m_session);
	bool HandleNPCFactionCommand(const char *args, WorldSession *m_session);
	bool HandleCreatureSpawnCommand(const char *args, WorldSession *m_session);
	bool HandleGOSelect(const char *args, WorldSession *m_session);
	bool HandleGODelete(const char *args, WorldSession *m_session);
	bool HandleGOSpawn(const char *args, WorldSession *m_session);
	bool HandleGOInfo(const char *args, WorldSession *m_session);
	bool HandleGOEnable(const char *args, WorldSession *m_session);
	bool HandleGOActivate(const char* args, WorldSession *m_session);
	bool HandleGORotate(const char * args, WorldSession * m_session);
	bool HandleGOMove(const char * args, WorldSession * m_session);
	bool HandleAddAIAgentCommand(const char* args, WorldSession *m_session);
	bool HandleDelAIAgentCommand(const char* args, WorldSession *m_session);
	bool HandleListAIAgentCommand(const char* args, WorldSession *m_session);

	// Level 3 commands
	bool HandleRenameGuildCommand(const char* args, WorldSession *m_session);
	bool HandleMassSummonCommand(const char* args, WorldSession *m_session);
	bool HandleSecurityCommand(const char* args, WorldSession *m_session);
	bool HandleWorldPortCommand(const char* args, WorldSession *m_session);
	bool HandleAddWeaponCommand(const char* args, WorldSession *m_session);
	bool HandleAllowMovementCommand(const char* args, WorldSession *m_session);
	bool HandleMoveCommand(const char* args, WorldSession *m_session);
	bool HandleLearnCommand(const char* args, WorldSession *m_session);
	bool HandleReviveCommand(const char* args, WorldSession *m_session);
	bool HandleMorphCommand(const char* args, WorldSession *m_session);
	bool HandleAddGraveCommand(const char* args, WorldSession *m_session);
	bool HandleAddSHCommand(const char* args, WorldSession *m_session);
	bool HandleExploreCheatCommand(const char* args, WorldSession *m_session);
	bool HandleLevelUpCommand(const char* args, WorldSession *m_session);
	bool HandleGMTicketGetAllCommand(const char* args, WorldSession *m_session);
	bool HandleGMTicketGetByIdCommand(const char* args, WorldSession *m_session);
	bool HandleGMTicketDelByIdCommand(const char* args, WorldSession *m_session);
	bool HandleAddSkillCommand(const char* args, WorldSession *m_session);
	bool HandleAddInvItemCommand(const char* args, WorldSession *m_session);
	bool HandleWeatherCommand(const char* args, WorldSession *m_session);
	bool HandleGetRankCommand(const char* args, WorldSession *m_session);
	bool HandleSetRankCommand(const char* args, WorldSession *m_session);
	bool HandleResetReputationCommand(const char* args, WorldSession *m_session);
	bool HandleLearnSkillCommand(const char* args, WorldSession *m_session);
	bool HandleModifySkillCommand(const char* args, WorldSession *m_session);
	bool HandleRemoveSkillCommand(const char* args, WorldSession *m_session);
	bool HandleNpcInfoCommand(const char* args, WorldSession *m_session);
	bool HandleEmoteCommand(const char* args, WorldSession *m_session);
	bool HandleIncreaseWeaponSkill(const char* args, WorldSession *m_session);
	bool HandleCastSpellCommand(const char* args, WorldSession *m_session);
	bool HandleCastSpellNECommand(const char* args, WorldSession *m_session);
	bool HandleCellDeleteCommand(const char* args, WorldSession *m_session);
	bool HandleAddRestXPCommand(const char* args, WorldSession *m_session);
	bool HandleModifyGoldCommand(const char* args, WorldSession *m_session);
	bool HandleGenerateNameCommand(const char* args, WorldSession *m_session);
	bool HandleMonsterSayCommand(const char* args, WorldSession *m_session);
	bool HandleMonsterYellCommand(const char* args, WorldSession* m_session);
	bool HandleNpcComeCommand(const char* args, WorldSession* m_session);
	bool HandleBattlegroundCommand(const char* args, WorldSession *m_session);
	bool HandleSetWorldStateCommand(const char* args, WorldSession *m_session);
	bool HandlePlaySoundCommand(const char* args, WorldSession *m_session);
	bool HandleSetBattlefieldStatusCommand(const char* args, WorldSession *m_session);
	bool HandleAttackerInfoCommand(const char* args, WorldSession *m_session);
	bool HandleShowAttackersCommand(const char* args, WorldSession *m_session);
	bool HandleNpcReturnCommand(const char* args, WorldSession* m_session);
	bool HandleCreateAccountCommand(const char* args, WorldSession *m_session);
	bool HandleSetRateCommand(const char* args, WorldSession* m_session);
	bool HandleGetRateCommand(const char* args, WorldSession* m_session);
	bool HandleResetLevelCommand(const char* args, WorldSession *m_session);
	bool HandleResetTalentsCommand(const char* args, WorldSession *m_session);
	bool HandleResetSpellsCommand(const char* args, WorldSession *m_session);
	bool HandleNpcFollowCommand(const char* args, WorldSession * m_session);
	bool HandleFormationLink1Command(const char* args, WorldSession * m_session);
	bool HandleFormationLink2Command(const char* args, WorldSession * m_session);
	bool HandleNullFollowCommand(const char* args, WorldSession * m_session);
	bool HandleFormationClearCommand(const char* args, WorldSession * m_session);
	bool HandleResetSkillsCommand(const char* args, WorldSession * m_session);
	bool HandlePlayerInfo(const char* args, WorldSession * m_session);

	//Ban
	bool HandleBanCharacterCommand(const char* args, WorldSession *m_session);
	bool HandleUnBanCharacterCommand(const char* args, WorldSession *m_session);

	//BG
	bool HandleSetBGScoreCommand(const char* args, WorldSession *m_session);

	Player* getSelectedChar(WorldSession *m_session, bool showerror = true);
	Creature * getSelectedCreature(WorldSession *m_session, bool showerror = true);
	bool HandleGOScale(const char* args, WorldSession *m_session);
	bool HandleUptimeCommand(const char* args, WorldSession* m_session);
	bool HandleReviveStringcommand(const char* args, WorldSession* m_session);
	bool HandleMountCommand(const char* args, WorldSession* m_session);
	bool HandleGetPosCommand(const char* args, WorldSession* m_session);
	bool HandleGetTransporterTime(const char* args, WorldSession* m_session);
	bool HandleSendItemPushResult(const char* args, WorldSession* m_session);
	bool HandleGOAnimProgress(const char * args, WorldSession * m_session);
	bool HandleGOExport(const char * args, WorldSession * m_session);
	bool HandleNpcExport(const char * args, WorldSession * m_session);
	bool HandleRemoveAurasCommand(const char *args, WorldSession *m_session);
	bool HandleParalyzeCommand(const char* args, WorldSession *m_session);
	bool HandleUnParalyzeCommand(const char* args, WorldSession *m_session);
	bool HandleSetMotdCommand(const char* args, WorldSession* m_session);
	bool HandleAddItemSetCommand(const char* args, WorldSession* m_session);
	bool HandleTriggerCommand(const char* args, WorldSession* m_session);
	bool HandleModifyValueCommand(const char* args, WorldSession* m_session);
	bool HandleModifyBitCommand(const char* args, WorldSession* m_session);
	bool HandleGoInstanceCommand(const char* args, WorldSession* m_session);
	bool HandleCreateInstanceCommand(const char* args, WorldSession* m_session);
	bool HandleBattlegroundExitCommand(const char* args, WorldSession* m_session);
	bool HandleExitInstanceCommand(const char* args, WorldSession* m_session);
	bool HandleCooldownCheatCommand(const char* args, WorldSession* m_session);
	bool HandleCastTimeCheatCommand(const char* args, WorldSession* m_session);
	bool HandlePowerCheatCommand(const char* args, WorldSession* m_session);
	bool HandleGodModeCommand(const char* args, WorldSession* m_session);
	bool HandleShowCheatsCommand(const char* args, WorldSession* m_session);
	bool HandleFlySpeedCheatCommand(const char* args, WorldSession* m_session);
	bool HandleStackCheatCommand(const char* args, WorldSession * m_session);
	bool HandleFlyCommand(const char* args, WorldSession* m_session);
	bool HandleLandCommand(const char* args, WorldSession* m_session);

	bool HandleDBReloadCommand(const char* args, WorldSession* m_session);
	bool HandleResetHPCommand(const char* args, WorldSession* m_session);
	
	// honor
	bool HandleAddHonorCommand(const char* args, WorldSession* m_session);
	bool HandleAddKillCommand(const char* args, WorldSession* m_session);
	bool HandleGlobalHonorDailyMaintenanceCommand(const char* args, WorldSession* m_session);
	bool HandleNextDayCommand(const char* args, WorldSession* m_session);
	bool HandlePVPCreditCommand(const char* args, WorldSession* m_session);
	
	bool HandleSpawnSpiritGuideCommand(const char* args, WorldSession * m_session);
	bool HandleUnlearnCommand(const char* args, WorldSession * m_session);
	bool HandleModifyLevelCommand(const char* args, WorldSession* m_session);
	
	// pet
	bool HandleCreatePetCommand(const char* args, WorldSession* m_session);
	bool HandleAddPetSpellCommand(const char* args, WorldSession* m_session);
	bool HandleRemovePetSpellCommand(const char* args, WorldSession* m_session);
	bool HandleEnableRenameCommand(const char* args, WorldSession* m_session);
	bool HandleRenamePetCommand(const char* args, WorldSession* m_session);

	bool HandleShutdownCommand(const char* args, WorldSession* m_session);
	bool HandleShutdownRestartCommand(const char* args, WorldSession* m_session);

	bool HandleAllowWhispersCommand(const char* args, WorldSession* m_session);
	bool HandleBlockWhispersCommand(const char* args, WorldSession* m_session);

	bool HandleAdvanceAllSkillsCommand(const char* args, WorldSession* m_session);
	bool HandleKillBySessionCommand(const char* args, WorldSession* m_session);
	bool HandleKillByPlayerCommand(const char* args, WorldSession* m_session);

	bool HandleUnlockMovementCommand(const char* args, WorldSession* m_session);
	bool HandleCastAllCommand(const char* args, WorldSession* m_session);

	//Recall Commands
	bool HandleRecallListCommand(const char* args, WorldSession *m_session);
	bool HandleRecallGoCommand(const char* args, WorldSession *m_session);
	bool HandleRecallAddCommand(const char* args, WorldSession *m_session);
	bool HandleRecallDelCommand(const char* args, WorldSession *m_session);
	bool HandleModPeriodCommand(const char* args, WorldSession * m_session);
	bool HandleGlobalPlaySoundCommand(const char* args, WorldSession * m_session);
	bool HandleRecallPortPlayerCommand(const char* args, WorldSession * m_session);

	bool HandleBanAccountCommand(const char * args, WorldSession * m_session);
	bool HandleIPBanCommand(const char * args, WorldSession * m_session);
	bool HandleIPUnBanCommand(const char * args, WorldSession * m_session);

	bool HandleRemoveItemCommand(const char * args, WorldSession * m_session);
	bool HandleRenameCommand(const char * args, WorldSession * m_session);
	bool HandleForceRenameCommand(const char * args, WorldSession * m_session);
	bool HandleGetStandingCommand(const char * args, WorldSession * m_session);
	bool HandleSetStandingCommand(const char * args, WorldSession * m_session);
	bool HandleGetBaseStandingCommand(const char * args, WorldSession * m_session);

	bool HandleReloadAccountsCommand(const char * args, WorldSession * m_session);
	bool HandleLookupItemCommand(const char * args, WorldSession * m_session);
	bool HandleLookupCreatureCommand(const char * args, WorldSession * m_session);
	bool HandleLookupObjectCommand(const char * args, WorldSession * m_session);
	bool HandleLookupSpellCommand(const char * args, WorldSession * m_session);

	bool HandleReloadScriptsCommand(const char * args, WorldSession * m_session);
	bool HandleNpcPossessCommand(const char * args, WorldSession * m_session);
	bool HandleNpcUnPossessCommand(const char * args, WorldSession * m_session);
	bool HandleChangePasswordCommand(const char * args, WorldSession * m_session);
	bool HandleRehashCommand(const char * args, WorldSession * m_session);

	/** AI AGENT DEBUG COMMANDS */
	bool HandleAIAgentDebugBegin(const char * args, WorldSession * m_session);
	bool HandleAIAgentDebugContinue(const char * args, WorldSession * m_session);
	bool HandleAIAgentDebugSkip(const char * args, WorldSession * m_session);

	bool HandleAddGuardCommand(const char * args, WorldSession * m_session);
};


#define sChatHandler ChatHandler::getSingleton()
#endif
