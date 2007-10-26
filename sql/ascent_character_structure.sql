DROP TABLE IF EXISTS `arenateams`;
CREATE TABLE IF NOT EXISTS `arenateams` (
	`id` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Unique ID',
	`type` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Type: 0-2v2 1-3v3 2-5v5',
	`leader` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Leading character guid',
	`name` varchar(24) NOT NULL DEFAULT '' COMMENT 'Team name',
	`emblemstyle` int UNSIGNED NOT NULL DEFAULT 0,
	`emblemcolour` int UNSIGNED NOT NULL DEFAULT 0,
	`borderstyle` int UNSIGNED NOT NULL DEFAULT 0,
	`bordercolor` int UNSIGNED NOT NULL DEFAULT 0,
	`backgroundcolor` int UNSIGNED NOT NULL DEFAULT 0,
	`rating` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Server Rating',
	`data` varchar(24) NOT NULL COMMENT '(played_week) (won_week) (played_season) (won_season)',
	`ranking` int unsigned NOT NULL COMMENT 'Server Ranking',
	`player_data1` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	`player_data2` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	`player_data3` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	`player_data4` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	`player_data5` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	`player_data6` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	`player_data7` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	`player_data8` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	`player_data9` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	`player_data10` varchar(32) NOT NULL COMMENT '(player_guid) (played_week) (won_week) (played_season) (won_season)',
	PRIMARY KEY (`id`)
) COMMENT 'Arena Teams';


DROP TABLE IF EXISTS `auctions`;
CREATE TABLE IF NOT EXISTS `auctions` (
	`auctionId` int UNSIGNED NOT NULL auto_increment,
	`auctionHouse` int UNSIGNED NOT NULL,
	`item` bigint UNSIGNED NOT NULL,
	`owner` int UNSIGNED NOT NULL,
	`buyout` int UNSIGNED NOT NULL DEFAULT 0,
	`time` int UNSIGNED NOT NULL,
	`bidder` int UNSIGNED NOT NULL,
	`bid` int UNSIGNED NOT NULL,
	`deposit` int UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`auctionId`),
	KEY (`auctionHouse`)
) COMMENT 'Auctioned items';


DROP TABLE IF EXISTS `characters`;
CREATE TABLE IF NOT EXISTS `characters` (
	`guid` int UNSIGNED NOT NULL,
	`acct` int UNSIGNED NOT NULL,
	`name` varchar(21) NOT NULL,
	`race` tinyint UNSIGNED NOT NULL,
	`class` tinyint UNSIGNED NOT NULL,
	`gender` tinyint UNSIGNED NOT NULL,
	`custom_faction` int UNSIGNED NOT NULL DEFAULT 0,
	`level` tinyint UNSIGNED NOT NULL,
	`xp` int UNSIGNED NOT NULL,
	`exploration_data` longtext NOT NULL,
	`skills` longtext NOT NULL,
	`watched_faction_index` int UNSIGNED NOT NULL DEFAULT 0,
	`selected_pvp_title` int UNSIGNED NOT NULL DEFAULT 0,
	`available_pvp_titles` int UNSIGNED NOT NULL DEFAULT 0,
	`gold` int UNSIGNED NOT NULL DEFAULT 0,
	`ammo_id` int UNSIGNED NOT NULL DEFAULT 0,
	`available_prof_points` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`available_talent_points` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`current_hp` int UNSIGNED NOT NULL DEFAULT 0,
	`current_power` int UNSIGNED NOT NULL DEFAULT 0,
	`pvprank` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`bytes` int UNSIGNED NOT NULL DEFAULT 0,
	`bytes2` int UNSIGNED NOT NULL DEFAULT 0,
	`player_flags` int UNSIGNED NOT NULL DEFAULT 0,
	`player_bytes` int UNSIGNED NOT NULL DEFAULT 0,
	`positionX` float NOT NULL DEFAULT 0,
	`positionY` float NOT NULL DEFAULT 0,
	`positionZ` float NOT NULL DEFAULT 0,
	`orientation` float NOT NULL DEFAULT 0,
	`mapId` int UNSIGNED NOT NULL DEFAULT 0,
	`zoneId` int UNSIGNED NOT NULL DEFAULT 0,
	`taximask` longtext NOT NULL,
	`banned` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`banReason` varchar(50) NOT NULL DEFAULT '',
	`timestamp` int UNSIGNED NOT NULL DEFAULT 0,
	`online` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`bindpositionX` float NOT NULL DEFAULT 0,
	`bindpositionY` float NOT NULL DEFAULT 0,
	`bindpositionZ` float NOT NULL DEFAULT 0,
	`bindmapId` int UNSIGNED NOT NULL DEFAULT 0,
	`bindzoneId` int UNSIGNED NOT NULL DEFAULT 0,
	`isResting` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`restState` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`restTime` int UNSIGNED NOT NULL DEFAULT 0,
	`playedtime` longtext NOT NULL,
	`deathstate` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`TalentResetTimes` int UNSIGNED NOT NULL DEFAULT 0,
	`first_login` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`forced_rename_pending` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`publicNote` varchar(32) NOT NULL DEFAULT '',
	`officerNote` varchar(32) NOT NULL DEFAULT '',
	`guildid` int UNSIGNED NOT NULL DEFAULT 0,
	`guildRank` int UNSIGNED NOT NULL DEFAULT 0,
	`arenaPoints` int UNSIGNED NOT NULL DEFAULT 0,
	`totalstableslots` int UNSIGNED NOT NULL DEFAULT 0,
	`instance_id` int UNSIGNED NOT NULL DEFAULT 0,
	`entrypointmap` int UNSIGNED NOT NULL DEFAULT 0,
	`entrypointx` float NOT NULL DEFAULT 0,
	`entrypointy` float NOT NULL DEFAULT 0,
	`entrypointz` float NOT NULL DEFAULT 0,
	`entrypointo` float NOT NULL DEFAULT 0,
	`entrypointinstance` int UNSIGNED NOT NULL DEFAULT 0,
	`taxi_path` int UNSIGNED NOT NULL DEFAULT 0,
	`taxi_lastnode` int UNSIGNED NOT NULL DEFAULT 0,
	`taxi_mountid` int UNSIGNED NOT NULL DEFAULT 0,
	`transporter` int UNSIGNED NOT NULL DEFAULT 0,
	`transporter_xdiff` float NOT NULL DEFAULT 0,
	`transporter_ydiff` float NOT NULL DEFAULT 0,
	`transporter_zdiff` float NOT NULL DEFAULT 0,
	`spells` longtext NOT NULL,
	`deleted_spells` longtext NOT NULL,
	`reputation` longtext NOT NULL,
	`actions` longtext NOT NULL,
	`auras` longtext NOT NULL,
	`finished_quests` longtext NOT NULL,
	`honorPointsToAdd` int NOT NULL,
	`killsToday` int UNSIGNED NOT NULL DEFAULT 0,
	`killsYesterday` int UNSIGNED NOT NULL DEFAULT 0,
	`killsLifeTime` int UNSIGNED NOT NULL DEFAULT 0,
	`honorToday` int UNSIGNED NOT NULL DEFAULT 0,
	`honorYesterday` int UNSIGNED NOT NULL DEFAULT 0,
	`honorPoints` int UNSIGNED NOT NULL DEFAULT 0,
	`difficulty` int UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`guid`),
	KEY (`acct`),
	KEY (`guildid`)
) COMMENT 'Characters';


DROP TABLE IF EXISTS `charters`;
CREATE TABLE IF NOT EXISTS `charters` (
	`charterId` int UNSIGNED NOT NULL,
	`charterType` int UNSIGNED NOT NULL DEFAULT 0,
	`leaderGuid` int UNSIGNED NOT NULL DEFAULT 0,
	`guildName` varchar(32) NOT NULL DEFAULT '',
	`itemGuid` bigint UNSIGNED NOT NULL DEFAULT 0,
	`signer1` int UNSIGNED NOT NULL DEFAULT 0,
	`signer2` int UNSIGNED NOT NULL DEFAULT 0,
	`signer3` int UNSIGNED NOT NULL DEFAULT 0,
	`signer4` int UNSIGNED NOT NULL DEFAULT 0,
	`signer5` int UNSIGNED NOT NULL DEFAULT 0,
	`signer6` int UNSIGNED NOT NULL DEFAULT 0,
	`signer7` int UNSIGNED NOT NULL DEFAULT 0,
	`signer8` int UNSIGNED NOT NULL DEFAULT 0,
	`signer9` int UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`charterId`),
	UNIQUE KEY (`leaderGuid`)
);


DROP TABLE IF EXISTS `corpses`;
CREATE TABLE IF NOT EXISTS `corpses` (
	`guid` int UNSIGNED NOT NULL DEFAULT 0,
	`positionX` float NOT NULL DEFAULT 0,
	`positionY` float NOT NULL DEFAULT 0,
	`positionZ` float NOT NULL DEFAULT 0,
	`orientation` float NOT NULL DEFAULT 0,
	`zoneId` int UNSIGNED NOT NULL DEFAULT 0,
	`mapId` int UNSIGNED NOT NULL DEFAULT 0,
	`instanceId` int UNSIGNED NOT NULL DEFAULT 0,
	`data` longtext NOT NULL,
	PRIMARY KEY (`guid`)
);


DROP TABLE IF EXISTS `gm_tickets`;
CREATE TABLE IF NOT EXISTS `gm_tickets` (
	`guid` int UNSIGNED NOT NULL DEFAULT 0,
	`name` varchar(200) NOT NULL,
	`level` int UNSIGNED NOT NULL DEFAULT 0,
	`type` int UNSIGNED NOT NULL DEFAULT 0,
	`posX` float NOT NULL DEFAULT 0,
	`posY` float NOT NULL DEFAULT 0,
	`posZ` float NOT NULL DEFAULT 0,
	`message` text NOT NULL,
	`timestamp` int UNSIGNED NULL,
	PRIMARY KEY (`guid`)
);

DROP TABLE IF EXISTS `groups`;
CREATE TABLE IF NOT EXISTS `groups` (
	`group_id` int UNSIGNED NOT NULL,
	`group_type` tinyint UNSIGNED NOT NULL,
	`subgroup_count` tinyint UNSIGNED NOT NULL,
	`loot_method` tinyint UNSIGNED NOT NULL,
	`loot_threshold` tinyint UNSIGNED NOT NULL,
	`group1member1` int UNSIGNED NOT NULL,
	`group1member2` int UNSIGNED NOT NULL,
	`group1member3` int UNSIGNED NOT NULL,
	`group1member4` int UNSIGNED NOT NULL,
	`group1member5` int UNSIGNED NOT NULL,
	`group2member1` int UNSIGNED NOT NULL,
	`group2member2` int UNSIGNED NOT NULL,
	`group2member3` int UNSIGNED NOT NULL,
	`group2member4` int UNSIGNED NOT NULL,
	`group2member5` int UNSIGNED NOT NULL,
	`group3member1` int UNSIGNED NOT NULL,
	`group3member2` int UNSIGNED NOT NULL,
	`group3member3` int UNSIGNED NOT NULL,
	`group3member4` int UNSIGNED NOT NULL,
	`group3member5` int UNSIGNED NOT NULL,
	`group4member1` int UNSIGNED NOT NULL,
	`group4member2` int UNSIGNED NOT NULL,
	`group4member3` int UNSIGNED NOT NULL,
	`group4member4` int UNSIGNED NOT NULL,
	`group4member5` int UNSIGNED NOT NULL,
	`group5member1` int UNSIGNED NOT NULL,
	`group5member2` int UNSIGNED NOT NULL,
	`group5member3` int UNSIGNED NOT NULL,
	`group5member4` int UNSIGNED NOT NULL,
	`group5member5` int UNSIGNED NOT NULL,
	`group6member1` int UNSIGNED NOT NULL,
	`group6member2` int UNSIGNED NOT NULL,
	`group6member3` int UNSIGNED NOT NULL,
	`group6member4` int UNSIGNED NOT NULL,
	`group6member5` int UNSIGNED NOT NULL,
	`group7member1` int UNSIGNED NOT NULL,
	`group7member2` int UNSIGNED NOT NULL,
	`group7member3` int UNSIGNED NOT NULL,
	`group7member4` int UNSIGNED NOT NULL,
	`group7member5` int UNSIGNED NOT NULL,
	`group8member1` int UNSIGNED NOT NULL,
	`group8member2` int UNSIGNED NOT NULL,
	`group8member3` int UNSIGNED NOT NULL,
	`group8member4` int UNSIGNED NOT NULL,
	`group8member5` int UNSIGNED NOT NULL,
	`timestamp` int UNSIGNED NOT NULL,
	PRIMARY KEY (`group_id`)
);


DROP TABLE IF EXISTS `guilds`;
CREATE TABLE IF NOT EXISTS `guilds` (
	`guildId` int UNSIGNED NOT NULL auto_increment,
	`guildName` varchar(32) NOT NULL,
	`leaderGuid` int UNSIGNED NOT NULL DEFAULT 0,
	`emblemStyle` int UNSIGNED NOT NULL DEFAULT 0,
	`emblemColor` int UNSIGNED NOT NULL DEFAULT 0,
	`borderStyle` int UNSIGNED NOT NULL DEFAULT 0,
	`borderColor` int UNSIGNED NOT NULL DEFAULT 0,
	`backgroundColor` int UNSIGNED NOT NULL DEFAULT 0,
	`guildInfo` varchar(100) NOT NULL DEFAULT '',
	`motd` varchar(100) NOT NULL DEFAULT '',
	`createdate` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY (`guildId`)
--	UNIQUE KEY (`leaderGuid`),
--	UNIQUE KEY (`guildName`)
) COMMENT 'Guilds';


DROP TABLE IF EXISTS `guild_ranks`;
CREATE TABLE IF NOT EXISTS `guild_ranks` (
	`guildId` int UNSIGNED NOT NULL,
	`rankId` int UNSIGNED NOT NULL DEFAULT 0,
	`rankName` varchar(32) NOT NULL DEFAULT '',
	`rankRights` int UNSIGNED NOT NULL DEFAULT 0
) COMMENT 'Guild Ranks';


DROP TABLE IF EXISTS `instances`;
CREATE TABLE IF NOT EXISTS `instances` (
	`instanceid` int UNSIGNED NOT NULL DEFAULT 0,
	`mapid` int UNSIGNED NOT NULL DEFAULT 0,
	`npc_data` longtext NOT NULL DEFAULT '',
	`player_data` longtext NOT NULL DEFAULT '',
	`creation` int UNSIGNED NOT NULL DEFAULT 0,
	`expire` int UNSIGNED NOT NULL DEFAULT 0,
	`difficulty` int UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`instanceid`)
);


DROP TABLE IF EXISTS `mailbox`;
CREATE TABLE IF NOT EXISTS `mailbox` (
	`message_id` int UNSIGNED NOT NULL DEFAULT 0,
	`message_type` int UNSIGNED NOT NULL DEFAULT 0,
	`player_guid` int UNSIGNED NOT NULL DEFAULT 0,
	`sender_guid` int UNSIGNED NOT NULL DEFAULT 0,
	`subject` varchar(255) NOT NULL DEFAULT '',
	`body` longtext NOT NULL DEFAULT '',
	`money` int UNSIGNED NOT NULL DEFAULT 0,
	`attached_item_guid` bigint UNSIGNED NOT NULL DEFAULT 0,
	`cod` int UNSIGNED NOT NULL DEFAULT 0,
	`stationary` int UNSIGNED NOT NULL DEFAULT 0,
	`expiry_time` int UNSIGNED NOT NULL DEFAULT 0,
	`delivery_time` int UNSIGNED NOT NULL DEFAULT 0,
	`copy_made` int UNSIGNED NOT NULL DEFAULT 0,
	`read_flag` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`deleted_flag` tinyint UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY(`message_id`)
);


DROP TABLE IF EXISTS `playercooldownitems`;
CREATE TABLE IF NOT EXISTS `playercooldownitems` (
	`OwnerGuid` int UNSIGNED NOT NULL DEFAULT 0,
	`ItemEntry` int UNSIGNED NOT NULL DEFAULT 0,
	`SpellID` int UNSIGNED NOT NULL DEFAULT 0,
	`SpellCategory` int UNSIGNED NOT NULL DEFAULT 0,
	`CooldownTimeStamp` int UNSIGNED NOT NULL DEFAULT 0,
	`Cooldown` int UNSIGNED NOT NULL DEFAULT 0
);


DROP TABLE IF EXISTS `playercooldownsecurity`;
CREATE TABLE IF NOT EXISTS `playercooldownsecurity` (
	`OwnerGuid` int UNSIGNED NOT NULL DEFAULT 0,
	`SpellID` int UNSIGNED NOT NULL DEFAULT 0,
	`TimeStamp` int UNSIGNED NOT NULL DEFAULT 0
);


DROP TABLE IF EXISTS `playeritems`;
CREATE TABLE IF NOT EXISTS `playeritems` (
	`ownerguid` int UNSIGNED NOT NULL DEFAULT 0,
	`guid` bigint UNSIGNED NOT NULL DEFAULT 0,
	`entry` int UNSIGNED NOT NULL DEFAULT 0,
	`creator` int UNSIGNED NOT NULL DEFAULT 0,
	`count` int UNSIGNED NOT NULL DEFAULT 0,
	`charges` int UNSIGNED NOT NULL DEFAULT 0,
	`flags` int UNSIGNED NOT NULL DEFAULT 0,
	`randomprop` int UNSIGNED NOT NULL DEFAULT 0,
	`randomsuffix` int NOT NULL DEFAULT 0,		-- unsigned?
	`itemtext` int UNSIGNED NOT NULL DEFAULT 0,
	`durability` int UNSIGNED NOT NULL DEFAULT 0,
	`containerslot` int NOT NULL DEFAULT -1,
	`slot` tinyint NOT NULL DEFAULT 0,	-- unsigned?
	`enchantments` longtext NOT NULL,
	PRIMARY KEY (`guid`),
	INDEX (`ownerguid`)
);


DROP TABLE IF EXISTS `playerpets`;
CREATE TABLE IF NOT EXISTS `playerpets` (
	`ownerguid` int UNSIGNED NOT NULL DEFAULT 0,
	`petnumber` int NOT NULL DEFAULT 0,	-- unsigned?
	`name` varchar(21) NOT NULL,
	`entryid` int UNSIGNED NOT NULL DEFAULT 0,
	`data` longtext NOT NULL,
	`xp` int UNSIGNED NOT NULL DEFAULT 0,
	`active` tinyint UNSIGNED NOT NULL DEFAULT 0,
	`level` int UNSIGNED NOT NULL DEFAULT 0,
	`happiness` int NOT NULL DEFAULT 0,
	`actionbar` varchar(200) NOT NULL,
	`happinessupdate` int UNSIGNED NOT NULL DEFAULT 0,
	`summon` int UNSIGNED NOT NULL DEFAULT 0,
	`autocastspell` int UNSIGNED NOT NULL DEFAULT 0,
	`loyaltypts` int UNSIGNED NOT NULL DEFAULT 0,
	`loyaltyupdate` int UNSIGNED NOT NULL DEFAULT 0
--	UNIQUE KEY (`ownerguid`,`petnumber`)
);


DROP TABLE IF EXISTS `playerpetspells`;
CREATE TABLE IF NOT EXISTS `playerpetspells` (
	`ownerguid` int UNSIGNED NOT NULL DEFAULT 0,
	`petnumber` int UNSIGNED NOT NULL DEFAULT 0,
	`spellid` int UNSIGNED NOT NULL DEFAULT 0,
	`flags` smallint UNSIGNED NOT NULL DEFAULT 0
);


DROP TABLE IF EXISTS `playersummonspells`;
CREATE TABLE IF NOT EXISTS `playersummonspells` (
	`ownerguid` int UNSIGNED NOT NULL DEFAULT 0,
	`entryid` int UNSIGNED NOT NULL DEFAULT 0,
	`spellid` int UNSIGNED NOT NULL DEFAULT 0
);

DROP TABLE IF EXISTS `questlog`;
CREATE TABLE IF NOT EXISTS `questlog` (
	`index` bigint UNSIGNED NOT NULL auto_increment,
	`player_guid` int UNSIGNED NOT NULL DEFAULT 0,
	`quest_id` int UNSIGNED NOT NULL DEFAULT 0,
	`slot` int UNSIGNED NOT NULL DEFAULT 0,
	`time_left` int UNSIGNED NOT NULL DEFAULT 0,
	`explored_area1` int UNSIGNED NOT NULL DEFAULT 0,
	`explored_area2` int UNSIGNED NOT NULL DEFAULT 0,
	`explored_area3` int UNSIGNED NOT NULL DEFAULT 0,
	`explored_area4` int UNSIGNED NOT NULL DEFAULT 0,
	`mob_kill1` int UNSIGNED NOT NULL DEFAULT 0,
	`mob_kill2` int UNSIGNED NOT NULL DEFAULT 0,
	`mob_kill3` int UNSIGNED NOT NULL DEFAULT 0,
	`mob_kill4` int UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`index`)
);


DROP TABLE IF EXISTS `server_settings`;
CREATE TABLE IF NOT EXISTS `server_settings` (
	`setting_id` varchar(200) NOT NULL,
	`setting_value` int NOT NULL,
	PRIMARY KEY (`setting_id`)
);


DROP TABLE IF EXISTS `social`;
CREATE TABLE IF NOT EXISTS `social` (
	`guid` int UNSIGNED NOT NULL DEFAULT 0,
	`socialguid` int UNSIGNED NOT NULL DEFAULT 0,
	`flags` varchar(21) NOT NULL,
	`noticed` tinyint UNSIGNED NOT NULL DEFAULT 1,
	PRIMARY KEY (`guid`,`socialguid`)
);


DROP TABLE IF EXISTS `tutorials`;
CREATE TABLE IF NOT EXISTS `tutorials` (
	`playerId` int UNSIGNED NOT NULL DEFAULT 0,
	`tut0` int UNSIGNED NOT NULL DEFAULT 0,
	`tut1` int UNSIGNED NOT NULL DEFAULT 0,
	`tut2` int UNSIGNED NOT NULL DEFAULT 0,
	`tut3` int UNSIGNED NOT NULL DEFAULT 0,
	`tut4` int UNSIGNED NOT NULL DEFAULT 0,
	`tut5` int UNSIGNED NOT NULL DEFAULT 0,
	`tut6` int UNSIGNED NOT NULL DEFAULT 0,
	`tut7` int UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`playerId`)
);
