-- MySQL dump 10.11
--
-- Host: localhost    Database: ascent_world
-- ------------------------------------------------------
-- Server version	5.0.41

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `ai_agents`
--

DROP TABLE IF EXISTS `ai_agents`;
CREATE TABLE `ai_agents` (
  `entryId` int(11) unsigned NOT NULL default '0',
  `AI_AGENT` smallint(5) unsigned NOT NULL default '0',
  `procEvent` int(11) unsigned NOT NULL default '0',
  `procChance` int(11) unsigned NOT NULL default '0',
  `procCount` int(11) unsigned NOT NULL default '0',
  `spellId` int(11) unsigned NOT NULL default '0',
  `spellType` int(11) unsigned NOT NULL default '0',
  `spelltargetType` int(11) unsigned NOT NULL default '0',
  `spellCooldown` int(8) NOT NULL default '0',
  `floatMisc1` float NOT NULL default '0',
  `Misc2` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entryId`,`AI_AGENT`,`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `ai_threattospellid`
--

DROP TABLE IF EXISTS `ai_threattospellid`;
CREATE TABLE `ai_threattospellid` (
  `spellId` int(11) unsigned NOT NULL default '0',
  `mod` int(11) NOT NULL default '0',
  PRIMARY KEY  (`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `areatriggers`
--

DROP TABLE IF EXISTS `areatriggers`;
CREATE TABLE `areatriggers` (
  `AreaTriggerid` int(11) unsigned NOT NULL default '0',
  `Type` tinyint(3) unsigned default NULL,
  `Mapid` int(11) unsigned default NULL,
  `Screen` int(11) unsigned default NULL,
  `Name` varchar(100) collate utf8_unicode_ci default '0',
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0',
  `required_honor_rank` int(11) unsigned NOT NULL default '0',
  `required_level` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`AreaTriggerid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `auctionhouse`
--

DROP TABLE IF EXISTS `auctionhouse`;
CREATE TABLE `auctionhouse` (
  `entryid` int(11) unsigned NOT NULL auto_increment,
  `auctioneer` int(11) unsigned NOT NULL default '0',
  `AHid` int(11) unsigned NOT NULL default '0',
  UNIQUE KEY `entryid` (`entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `auctions`
--

DROP TABLE IF EXISTS `auctions`;
CREATE TABLE `auctions` (
  `auctionId` int(11) unsigned NOT NULL auto_increment,
  `auctionhouse` int(11) unsigned default NULL,
  `item` bigint(10) unsigned default NULL,
  `owner` int(11) unsigned default NULL,
  `buyout` int(11) unsigned default NULL,
  `time` int(11) unsigned default NULL,
  `bidder` int(11) unsigned default NULL,
  `bid` int(11) unsigned default NULL,
  `deposit` int(11) unsigned default NULL,
  PRIMARY KEY  (`auctionId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `characters`
--

DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
  `guid` int(10) unsigned NOT NULL default '0',
  `acct` int(10) unsigned NOT NULL default '0',
  `name` varchar(21) collate utf8_unicode_ci NOT NULL,
  `race` tinyint(3) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `gender` tinyint(3) unsigned NOT NULL default '0',
  `level` int(10) unsigned NOT NULL default '0',
  `xp` int(10) unsigned NOT NULL default '0',
  `exploration_data` longtext collate utf8_unicode_ci NOT NULL,
  `skills` longtext collate utf8_unicode_ci NOT NULL,
  `watched_faction_index` int(10) unsigned NOT NULL default '0',
  `selected_pvp_title` int(10) unsigned NOT NULL default '0',
  `available_pvp_titles` int(10) unsigned NOT NULL default '0',
  `gold` int(10) unsigned NOT NULL default '0',
  `ammo_id` int(10) unsigned NOT NULL default '0',
  `available_prof_points` int(10) unsigned NOT NULL default '0',
  `available_talent_points` int(10) unsigned NOT NULL default '0',
  `current_hp` int(10) unsigned NOT NULL default '0',
  `current_power` int(10) unsigned NOT NULL default '0',
  `pvprank` tinyint(3) unsigned NOT NULL default '0',
  `bytes` int(10) unsigned NOT NULL default '0',
  `bytes2` int(10) unsigned NOT NULL default '0',
  `player_flags` int(10) unsigned NOT NULL default '0',
  `player_bytes` int(10) unsigned NOT NULL default '0',
  `positionX` float NOT NULL default '0',
  `positionY` float NOT NULL default '0',
  `positionZ` float NOT NULL default '0',
  `orientation` float NOT NULL default '0',
  `mapId` int(10) unsigned NOT NULL default '0',
  `zoneId` int(10) unsigned NOT NULL default '0',
  `taximask` longtext collate utf8_unicode_ci NOT NULL,
  `banned` int(10) unsigned NOT NULL default '0',
  `banReason` varchar(50) collate utf8_unicode_ci NOT NULL,
  `timestamp` int(11) NOT NULL,
  `online` int(11) default NULL,
  `bindpositionX` float NOT NULL default '0',
  `bindpositionY` float NOT NULL default '0',
  `bindpositionZ` float NOT NULL default '0',
  `bindmapId` int(10) unsigned NOT NULL default '0',
  `bindzoneId` int(10) unsigned NOT NULL default '0',
  `isResting` tinyint(3) unsigned NOT NULL default '0',
  `restState` tinyint(3) unsigned NOT NULL default '0',
  `restTime` int(10) unsigned NOT NULL default '0',
  `playedtime` longtext collate utf8_unicode_ci NOT NULL,
  `deathstate` int(10) unsigned NOT NULL default '0',
  `TalentResetTimes` int(10) unsigned NOT NULL default '0',
  `first_login` tinyint(3) unsigned NOT NULL default '0',
  `forced_rename_pending` tinyint(3) unsigned NOT NULL default '0',
  `publicNote` varchar(32) collate utf8_unicode_ci NOT NULL,
  `officerNote` varchar(32) collate utf8_unicode_ci NOT NULL,
  `guildid` int(10) unsigned NOT NULL default '0',
  `guildRank` int(10) unsigned NOT NULL default '0',
  `charterId` int(10) unsigned NOT NULL default '0',
  `totalstableslots` int(10) unsigned NOT NULL default '0',
  `instance_id` int(10) unsigned NOT NULL default '0',
  `entrypointmap` int(10) unsigned NOT NULL default '0',
  `entrypointx` float NOT NULL default '0',
  `entrypointy` float NOT NULL default '0',
  `entrypointz` float NOT NULL default '0',
  `entrypointo` float NOT NULL default '0',
  `entrypointinstance` int(10) unsigned NOT NULL default '0',
  `taxi_path` int(10) unsigned NOT NULL default '0',
  `taxi_lastnode` int(10) unsigned NOT NULL default '0',
  `taxi_mountid` int(10) unsigned NOT NULL default '0',
  `transporter` int(10) unsigned NOT NULL default '0',
  `transporter_xdiff` float NOT NULL default '0',
  `transporter_ydiff` float NOT NULL default '0',
  `transporter_zdiff` float NOT NULL default '0',
  `spells` longtext collate utf8_unicode_ci NOT NULL,
  `deleted_spells` longtext collate utf8_unicode_ci NOT NULL,
  `reputation` longtext collate utf8_unicode_ci NOT NULL,
  `actions` longtext collate utf8_unicode_ci NOT NULL,
  `auras` longtext collate utf8_unicode_ci NOT NULL,
  `finished_quests` longtext collate utf8_unicode_ci NOT NULL,
  `lastDailyReset` int(10) unsigned NOT NULL default '0',
  `killsToday` int(10) unsigned NOT NULL default '0',
  `killsYesterday` int(10) unsigned NOT NULL default '0',
  `killsLifeTime` int(10) unsigned NOT NULL default '0',
  `honorToday` int(10) unsigned NOT NULL default '0',
  `honorYesterday` int(10) unsigned NOT NULL default '0',
  `honorPoints` int(10) unsigned NOT NULL default '0',
  `difficulty` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `guid` (`guid`),
  KEY `acct` (`acct`),
  KEY `guildid` (`guildid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `charters`
--

DROP TABLE IF EXISTS `charters`;
CREATE TABLE `charters` (
  `charterId` int(10) unsigned NOT NULL default '0',
  `leaderGuid` int(10) unsigned NOT NULL default '0',
  `guildName` varchar(32) collate utf8_unicode_ci NOT NULL,
  `itemGuid` bigint(20) unsigned NOT NULL default '0',
  `signer1` int(10) unsigned NOT NULL default '0',
  `signer2` int(10) unsigned NOT NULL default '0',
  `signer3` int(10) unsigned NOT NULL default '0',
  `signer4` int(10) unsigned NOT NULL default '0',
  `signer5` int(10) unsigned NOT NULL default '0',
  `signer6` int(10) unsigned NOT NULL default '0',
  `signer7` int(10) unsigned NOT NULL default '0',
  `signer8` int(10) unsigned NOT NULL default '0',
  `signer9` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charterId`),
  UNIQUE KEY `leaderGuid` (`leaderGuid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `clientaddons`
--

DROP TABLE IF EXISTS `clientaddons`;
CREATE TABLE `clientaddons` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(50) collate utf8_unicode_ci default NULL,
  `crc` bigint(20) unsigned default NULL,
  `banned` int(10) unsigned NOT NULL default '0',
  `showinlist` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `index` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `command_overrides`
--

DROP TABLE IF EXISTS `command_overrides`;
CREATE TABLE `command_overrides` (
  `command_name` varchar(100) collate utf8_unicode_ci NOT NULL default '',
  `access_level` varchar(10) collate utf8_unicode_ci NOT NULL default '',
  PRIMARY KEY  (`command_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `corpses`
--

DROP TABLE IF EXISTS `corpses`;
CREATE TABLE `corpses` (
  `guid` int(10) unsigned NOT NULL default '0',
  `positionX` float NOT NULL default '0',
  `positionY` float NOT NULL default '0',
  `positionZ` float NOT NULL default '0',
  `orientation` float NOT NULL default '0',
  `zoneId` int(10) unsigned NOT NULL default '38',
  `mapId` int(10) unsigned NOT NULL default '0',
  `instanceId` int(10) unsigned NOT NULL default '0',
  `data` longtext collate utf8_unicode_ci NOT NULL,
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `creature_formations`
--

DROP TABLE IF EXISTS `creature_formations`;
CREATE TABLE `creature_formations` (
  `creature_sqlid` int(10) unsigned NOT NULL default '0',
  `followtarget_sqlid` int(10) unsigned NOT NULL default '0',
  `follow_angle` float NOT NULL default '0',
  `follow_dist` float NOT NULL default '0',
  PRIMARY KEY  (`creature_sqlid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `creature_names`
--

DROP TABLE IF EXISTS `creature_names`;
CREATE TABLE `creature_names` (
  `entry` int(10) unsigned NOT NULL default '0',
  `creature_name` varchar(100) collate utf8_unicode_ci NOT NULL,
  `Subname` varchar(100) collate utf8_unicode_ci NOT NULL,
  `Flags1` int(10) unsigned default '0',
  `type` int(10) unsigned default '0',
  `Family` int(10) unsigned default '0',
  `Rank` int(10) unsigned default '0',
  `unk4` int(10) unsigned default '0',
  `SpellDataID` int(10) unsigned default NULL,
  `displayid` int(10) unsigned default '0',
  `unk2` float NOT NULL default '1',
  `unk3` float NOT NULL default '1',
  `Civilian` tinyint(3) unsigned default NULL,
  `Leader` tinyint(3) unsigned default NULL,
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `creature_proto`
--

DROP TABLE IF EXISTS `creature_proto`;
CREATE TABLE `creature_proto` (
  `entry` int(10) unsigned NOT NULL default '0',
  `level` int(10) unsigned NOT NULL default '0',
  `faction` int(10) unsigned NOT NULL default '0',
  `health` int(10) unsigned NOT NULL default '0',
  `mana` int(10) unsigned NOT NULL default '0',
  `scale` float NOT NULL default '0',
  `npcflags` int(10) unsigned NOT NULL default '0',
  `attacktime` int(10) unsigned NOT NULL default '0',
  `mindamage` float NOT NULL default '0',
  `maxdamage` float NOT NULL default '0',
  `rangedattacktime` int(10) unsigned NOT NULL default '0',
  `rangedmindamage` float unsigned NOT NULL default '0',
  `rangedmaxdamage` float unsigned NOT NULL default '0',
  `mountdisplayid` int(10) unsigned NOT NULL default '0',
  `item1slotdisplay` int(10) unsigned NOT NULL default '0',
  `item1info1` int(10) unsigned NOT NULL default '0',
  `item1info2` int(10) unsigned NOT NULL default '0',
  `item2slotdisplay` int(10) unsigned NOT NULL default '0',
  `item2info1` int(10) unsigned NOT NULL default '0',
  `item2info2` int(10) unsigned NOT NULL default '0',
  `item3slotdisplay` int(10) unsigned NOT NULL default '0',
  `item3info1` int(10) unsigned NOT NULL default '0',
  `item3info2` int(10) unsigned NOT NULL default '0',
  `respawntime` int(10) unsigned NOT NULL default '0',
  `resistance0_armor` int(10) unsigned NOT NULL default '0',
  `resistance1` int(10) unsigned NOT NULL default '0',
  `resistance2` int(10) unsigned NOT NULL default '0',
  `resistance3` int(10) unsigned NOT NULL default '0',
  `resistance4` int(10) unsigned NOT NULL default '0',
  `resistance5` int(10) unsigned NOT NULL default '0',
  `resistance6` int(10) unsigned NOT NULL default '0',
  `combat_reach` float NOT NULL default '0',
  `bounding_radius` float NOT NULL default '0',
  `auras` longtext collate utf8_unicode_ci NOT NULL,
  `boss` int(10) unsigned NOT NULL default '0',
  `money` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`),
  UNIQUE KEY `ID` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `creature_quest_finisher`
--

DROP TABLE IF EXISTS `creature_quest_finisher`;
CREATE TABLE `creature_quest_finisher` (
  `id` int(10) unsigned NOT NULL default '0',
  `quest` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci ROW_FORMAT=FIXED;

--
-- Table structure for table `creature_quest_starter`
--

DROP TABLE IF EXISTS `creature_quest_starter`;
CREATE TABLE `creature_quest_starter` (
  `id` int(10) unsigned NOT NULL default '0',
  `quest` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci ROW_FORMAT=FIXED;

--
-- Table structure for table `creature_spawns`
--

DROP TABLE IF EXISTS `creature_spawns`;
CREATE TABLE `creature_spawns` (
  `id` int(10) unsigned NOT NULL default '0',
  `entry` int(10) unsigned NOT NULL default '0',
  `Map` int(10) unsigned NOT NULL default '0',
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0',
  `movetype` int(10) unsigned NOT NULL default '0',
  `displayid` int(10) unsigned NOT NULL default '0',
  `factionid` int(10) unsigned NOT NULL default '0',
  `flags` int(10) unsigned NOT NULL default '0',
  `bytes` int(10) unsigned NOT NULL default '0',
  `bytes2` int(10) unsigned NOT NULL default '0',
  `emote_state` int(10) unsigned NOT NULL default '0',
  `respawnNpcLink` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `id` (`id`),
  KEY `map` (`Map`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `creature_staticspawns`
--

DROP TABLE IF EXISTS `creature_staticspawns`;
CREATE TABLE `creature_staticspawns` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `entry` int(10) unsigned NOT NULL default '0',
  `map` int(10) unsigned NOT NULL default '0',
  `pos_x` float NOT NULL default '0',
  `pos_y` float NOT NULL default '0',
  `pos_z` float NOT NULL default '0',
  `pos_o` float NOT NULL default '0',
  `movetype` int(10) unsigned NOT NULL default '0',
  `displayid` int(10) unsigned NOT NULL default '0',
  `factionid` int(10) unsigned NOT NULL default '0',
  `flags` int(10) unsigned NOT NULL default '0',
  `bytes` int(10) unsigned NOT NULL default '0',
  `bytes2` int(10) unsigned NOT NULL default '0',
  `emote_state` int(10) unsigned NOT NULL default '0',
  `respawn_npclink` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `creature_waypoints`
--

DROP TABLE IF EXISTS `creature_waypoints`;
CREATE TABLE `creature_waypoints` (
  `creatureid` int(10) unsigned NOT NULL default '0',
  `waypointid` int(10) unsigned NOT NULL default '0',
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `waittime` int(10) unsigned NOT NULL default '0',
  `flags` int(10) unsigned NOT NULL default '0',
  `forwardemoteoneshot` tinyint(3) unsigned NOT NULL default '0',
  `forwardemoteid` int(10) unsigned NOT NULL default '0',
  `backwardemoteoneshot` tinyint(3) unsigned NOT NULL default '0',
  `backwardemoteid` int(10) unsigned NOT NULL default '0',
  `forwardskinid` int(10) unsigned NOT NULL default '0',
  `backwardskinid` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`creatureid`,`waypointid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `creatureloot`
--

DROP TABLE IF EXISTS `creatureloot`;
CREATE TABLE `creatureloot` (
  `index` int(10) unsigned NOT NULL auto_increment,
  `entryid` int(10) unsigned NOT NULL default '0',
  `itemid` int(10) unsigned NOT NULL default '0',
  `percentchance` float default NULL,
  PRIMARY KEY  (`index`),
  UNIQUE KEY `index` (`index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `fishing`
--

DROP TABLE IF EXISTS `fishing`;
CREATE TABLE `fishing` (
  `Zone` int(10) unsigned NOT NULL default '0',
  `MinSkill` int(10) unsigned default NULL,
  `MaxSkill` int(10) unsigned default NULL,
  UNIQUE KEY `Zone` (`Zone`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `fishingloot`
--

DROP TABLE IF EXISTS `fishingloot`;
CREATE TABLE `fishingloot` (
  `index` int(10) unsigned NOT NULL auto_increment,
  `entryid` int(10) unsigned NOT NULL default '0',
  `itemid` int(10) unsigned NOT NULL default '0',
  `percentchance` float NOT NULL default '0',
  PRIMARY KEY  (`index`),
  UNIQUE KEY `index` (`index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `gameobject_names`
--

DROP TABLE IF EXISTS `gameobject_names`;
CREATE TABLE `gameobject_names` (
  `entry` int(10) unsigned NOT NULL default '0',
  `Type` int(10) unsigned NOT NULL default '0',
  `DisplayID` int(10) unsigned NOT NULL default '0',
  `Name` varchar(100) collate utf8_unicode_ci NOT NULL,
  `sound0` int(10) unsigned NOT NULL default '0',
  `sound1` int(10) unsigned NOT NULL default '0',
  `sound2` int(10) unsigned NOT NULL default '0',
  `sound3` int(10) unsigned NOT NULL default '0',
  `sound4` int(10) unsigned NOT NULL default '0',
  `sound5` int(10) unsigned NOT NULL default '0',
  `sound6` int(10) unsigned NOT NULL default '0',
  `sound7` int(10) unsigned NOT NULL default '0',
  `sound8` int(10) unsigned NOT NULL default '0',
  `sound9` int(10) unsigned NOT NULL default '0',
  `unknown1` int(10) unsigned NOT NULL default '0',
  `unknown2` int(10) unsigned NOT NULL default '0',
  `unknown3` int(10) unsigned NOT NULL default '0',
  `unknown4` int(10) unsigned NOT NULL default '0',
  `unknown5` int(10) unsigned NOT NULL default '0',
  `unknown6` int(10) unsigned NOT NULL default '0',
  `unknown7` int(10) unsigned NOT NULL default '0',
  `unknown8` int(10) unsigned NOT NULL default '0',
  `unknown9` int(10) unsigned NOT NULL default '0',
  `unknown10` int(10) unsigned NOT NULL default '0',
  `unknown11` int(10) unsigned NOT NULL default '0',
  `unknown12` int(10) unsigned NOT NULL default '0',
  `unknown13` int(10) unsigned NOT NULL default '0',
  `unknown14` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `gameobject_quest_finisher`
--

DROP TABLE IF EXISTS `gameobject_quest_finisher`;
CREATE TABLE `gameobject_quest_finisher` (
  `id` int(10) unsigned NOT NULL default '0',
  `quest` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `gameobject_quest_item_binding`
--

DROP TABLE IF EXISTS `gameobject_quest_item_binding`;
CREATE TABLE `gameobject_quest_item_binding` (
  `gameobject_entry` int(10) unsigned NOT NULL default '0',
  `quest_id` int(10) unsigned NOT NULL default '0',
  `item_id` int(10) unsigned NOT NULL default '0',
  `item_count` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `gameobject_quest_pickup_binding`
--

DROP TABLE IF EXISTS `gameobject_quest_pickup_binding`;
CREATE TABLE `gameobject_quest_pickup_binding` (
  `gameobject_entry` int(10) unsigned NOT NULL default '0',
  `quest_id` int(10) unsigned NOT NULL default '0',
  `required_count` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `gameobject_quest_starter`
--

DROP TABLE IF EXISTS `gameobject_quest_starter`;
CREATE TABLE `gameobject_quest_starter` (
  `id` int(10) unsigned NOT NULL default '0',
  `quest` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `gameobject_spawns`
--

DROP TABLE IF EXISTS `gameobject_spawns`;
CREATE TABLE `gameobject_spawns` (
  `ID` int(10) unsigned NOT NULL default '0',
  `Entry` int(10) unsigned NOT NULL default '0',
  `Map` int(10) unsigned NOT NULL default '0',
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `Facing` float NOT NULL default '0',
  `o1` float NOT NULL default '0',
  `o2` float NOT NULL default '0',
  `o3` float NOT NULL default '0',
  `o4` float NOT NULL default '0',
  `State` int(10) unsigned NOT NULL default '0',
  `Flags` int(10) unsigned NOT NULL default '0',
  `Faction` int(10) unsigned NOT NULL default '0',
  `Scale` float NOT NULL default '0',
  `stateNpcLink` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `ID` (`ID`),
  KEY `Map` (`Map`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `gameobject_staticspawns`
--

DROP TABLE IF EXISTS `gameobject_staticspawns`;
CREATE TABLE `gameobject_staticspawns` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `entry` int(11) unsigned NOT NULL default '0',
  `map` int(11) unsigned NOT NULL default '0',
  `pos_x` float NOT NULL default '0',
  `pos_y` float NOT NULL default '0',
  `pos_z` float NOT NULL default '0',
  `pos_o` float NOT NULL default '0',
  `rot_x` float NOT NULL default '0',
  `rot_y` float NOT NULL default '0',
  `rot_z` float NOT NULL default '0',
  `rot_o` float NOT NULL default '0',
  `state` int(11) unsigned NOT NULL default '0',
  `flags` int(11) unsigned NOT NULL default '0',
  `faction` int(11) unsigned NOT NULL default '0',
  `scale` float NOT NULL default '0',
  `state_npclink` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `gm_tickets`
--

DROP TABLE IF EXISTS `gm_tickets`;
CREATE TABLE `gm_tickets` (
  `guid` int(10) unsigned NOT NULL default '0',
  `name` varchar(200) collate utf8_unicode_ci NOT NULL,
  `level` int(10) unsigned NOT NULL default '0',
  `type` int(10) unsigned NOT NULL default '0',
  `posX` float NOT NULL default '0',
  `posY` float NOT NULL default '0',
  `posZ` float NOT NULL default '0',
  `message` text collate utf8_unicode_ci NOT NULL,
  `timestamp` int(10) unsigned default NULL,
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `graveyards`
--

DROP TABLE IF EXISTS `graveyards`;
CREATE TABLE `graveyards` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0',
  `zoneid` int(10) unsigned NOT NULL default '0',
  `adjacentzoneid` int(10) unsigned NOT NULL default '0',
  `mapid` int(10) unsigned NOT NULL default '0',
  `factionid` int(10) unsigned NOT NULL default '0',
  `name` varchar(255) collate utf8_unicode_ci NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `index` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `guild_ranks`
--

DROP TABLE IF EXISTS `guild_ranks`;
CREATE TABLE `guild_ranks` (
  `guildId` int(10) unsigned NOT NULL default '0',
  `rankId` int(10) unsigned NOT NULL default '0',
  `rankName` varchar(255) collate utf8_unicode_ci NOT NULL,
  `rankRights` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `guilds`
--

DROP TABLE IF EXISTS `guilds`;
CREATE TABLE `guilds` (
  `guildId` int(10) unsigned NOT NULL auto_increment,
  `guildName` varchar(32) collate utf8_unicode_ci NOT NULL,
  `leaderGuid` bigint(20) unsigned NOT NULL default '0',
  `emblemStyle` int(10) unsigned NOT NULL default '0',
  `emblemColor` int(10) unsigned NOT NULL default '0',
  `borderStyle` int(10) unsigned NOT NULL default '0',
  `borderColor` int(10) unsigned NOT NULL default '0',
  `backgroundColor` int(10) unsigned NOT NULL default '0',
  `guildInfo` varchar(100) collate utf8_unicode_ci NOT NULL,
  `motd` varchar(100) collate utf8_unicode_ci NOT NULL,
  `createdate` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`guildId`),
  UNIQUE KEY `guildId` (`guildId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `instances`
--

DROP TABLE IF EXISTS `instances`;
CREATE TABLE `instances` (
  `instanceid` int(10) unsigned NOT NULL default '0',
  `mapid` int(10) unsigned NOT NULL default '0',
  `npc_data` longtext collate utf8_unicode_ci NOT NULL,
  `player_data` longtext collate utf8_unicode_ci NOT NULL,
  `creation` int(10) unsigned NOT NULL default '0',
  `expire` int(10) unsigned NOT NULL default '0',
  `difficulty` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`instanceid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `itemloot`
--

DROP TABLE IF EXISTS `itemloot`;
CREATE TABLE `itemloot` (
  `index` int(10) unsigned NOT NULL auto_increment,
  `entryid` int(10) unsigned NOT NULL default '0',
  `itemid` int(10) unsigned NOT NULL default '25',
  `percentchance` float NOT NULL default '1',
  PRIMARY KEY  (`index`),
  KEY `i_gameobj_loot_entry` (`entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `itempages`
--

DROP TABLE IF EXISTS `itempages`;
CREATE TABLE `itempages` (
  `entry` int(10) unsigned NOT NULL default '0',
  `text` longtext collate utf8_unicode_ci NOT NULL,
  `next_page` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `items`
--

DROP TABLE IF EXISTS `items`;
CREATE TABLE `items` (
  `entry` int(10) unsigned NOT NULL default '0',
  `class` int(10) unsigned NOT NULL default '0',
  `subclass` int(10) unsigned NOT NULL default '0',
  `field4` int(10) unsigned NOT NULL default '0',
  `name1` varchar(255) collate utf8_unicode_ci NOT NULL,
  `name2` varchar(255) collate utf8_unicode_ci NOT NULL,
  `name3` varchar(255) collate utf8_unicode_ci NOT NULL,
  `name4` varchar(255) collate utf8_unicode_ci NOT NULL,
  `displayid` int(10) unsigned NOT NULL default '0',
  `quality` int(10) unsigned NOT NULL default '0',
  `flags` int(10) unsigned NOT NULL default '0',
  `buyprice` int(10) unsigned NOT NULL default '0',
  `sellprice` int(10) unsigned NOT NULL default '0',
  `inventorytype` int(10) unsigned NOT NULL default '0',
  `allowableclass` int(10) unsigned NOT NULL default '0',
  `allowablerace` int(10) unsigned NOT NULL default '0',
  `itemlevel` int(10) unsigned NOT NULL default '0',
  `requiredlevel` int(10) unsigned NOT NULL default '0',
  `RequiredSkill` int(10) unsigned NOT NULL default '0',
  `RequiredSkillRank` int(10) unsigned NOT NULL default '0',
  `RequiredSkillSubRank` int(10) unsigned NOT NULL default '0',
  `RequiredPlayerRank1` int(10) unsigned NOT NULL default '0',
  `RequiredPlayerRank2` int(10) unsigned NOT NULL default '0',
  `RequiredFaction` int(10) unsigned NOT NULL default '0',
  `RequiredFactionStanding` int(10) unsigned NOT NULL default '0',
  `Unique` int(10) unsigned NOT NULL default '0',
  `maxcount` int(10) unsigned NOT NULL default '0',
  `ContainerSlots` int(10) unsigned NOT NULL default '0',
  `stat_type1` int(10) unsigned NOT NULL default '0',
  `stat_value1` int(11) NOT NULL default '0',
  `stat_type2` int(10) unsigned NOT NULL default '0',
  `stat_value2` int(11) NOT NULL default '0',
  `stat_type3` int(10) unsigned NOT NULL default '0',
  `stat_value3` int(11) NOT NULL default '0',
  `stat_type4` int(10) unsigned NOT NULL default '0',
  `stat_value4` int(11) NOT NULL default '0',
  `stat_type5` int(10) unsigned NOT NULL default '0',
  `stat_value5` int(11) NOT NULL default '0',
  `stat_type6` int(10) unsigned NOT NULL default '0',
  `stat_value6` int(11) NOT NULL default '0',
  `stat_type7` int(10) unsigned NOT NULL default '0',
  `stat_value7` int(11) NOT NULL default '0',
  `stat_type8` int(10) unsigned NOT NULL default '0',
  `stat_value8` int(11) NOT NULL default '0',
  `stat_type9` int(10) unsigned NOT NULL default '0',
  `stat_value9` int(11) NOT NULL default '0',
  `stat_type10` int(10) unsigned NOT NULL default '0',
  `stat_value10` int(11) NOT NULL default '0',
  `dmg_min1` float NOT NULL default '0',
  `dmg_max1` float NOT NULL default '0',
  `dmg_type1` int(10) unsigned NOT NULL default '0',
  `dmg_min2` float NOT NULL default '0',
  `dmg_max2` float NOT NULL default '0',
  `dmg_type2` int(10) unsigned NOT NULL default '0',
  `dmg_min3` float NOT NULL default '0',
  `dmg_max3` float NOT NULL default '0',
  `dmg_type3` int(10) unsigned NOT NULL default '0',
  `dmg_min4` float NOT NULL default '0',
  `dmg_max4` float NOT NULL default '0',
  `dmg_type4` int(10) unsigned NOT NULL default '0',
  `dmg_min5` float NOT NULL default '0',
  `dmg_max5` float NOT NULL default '0',
  `dmg_type5` int(10) unsigned NOT NULL default '0',
  `armor` int(10) unsigned NOT NULL default '0',
  `holy_res` int(10) unsigned NOT NULL default '0',
  `fire_res` int(10) unsigned NOT NULL default '0',
  `nature_res` int(10) unsigned NOT NULL default '0',
  `frost_res` int(10) unsigned NOT NULL default '0',
  `shadow_res` int(10) unsigned NOT NULL default '0',
  `arcane_res` int(10) unsigned NOT NULL default '0',
  `delay` int(10) unsigned NOT NULL default '0',
  `ammo_type` int(10) unsigned NOT NULL default '0',
  `range` float NOT NULL default '0',
  `spellid_1` int(10) unsigned NOT NULL default '0',
  `spelltrigger_1` int(10) unsigned NOT NULL default '0',
  `spellcharges_1` int(11) NOT NULL default '0',
  `spellcooldown_1` int(11) NOT NULL default '0',
  `spellcategory_1` int(10) unsigned NOT NULL default '0',
  `spellcategorycooldown_1` int(11) NOT NULL default '0',
  `spellid_2` int(10) unsigned NOT NULL default '0',
  `spelltrigger_2` int(10) unsigned NOT NULL default '0',
  `spellcharges_2` int(11) NOT NULL default '0',
  `spellcooldown_2` int(11) NOT NULL default '0',
  `spellcategory_2` int(10) unsigned NOT NULL default '0',
  `spellcategorycooldown_2` int(11) NOT NULL default '0',
  `spellid_3` int(10) unsigned NOT NULL default '0',
  `spelltrigger_3` int(10) unsigned NOT NULL default '0',
  `spellcharges_3` int(11) NOT NULL default '0',
  `spellcooldown_3` int(11) NOT NULL default '0',
  `spellcategory_3` int(10) unsigned NOT NULL default '0',
  `spellcategorycooldown_3` int(11) NOT NULL default '0',
  `spellid_4` int(10) unsigned NOT NULL default '0',
  `spelltrigger_4` int(10) unsigned NOT NULL default '0',
  `spellcharges_4` int(11) NOT NULL default '0',
  `spellcooldown_4` int(11) NOT NULL default '0',
  `spellcategory_4` int(10) unsigned NOT NULL default '0',
  `spellcategorycooldown_4` int(11) NOT NULL default '0',
  `spellid_5` int(10) unsigned NOT NULL default '0',
  `spelltrigger_5` int(10) unsigned NOT NULL default '0',
  `spellcharges_5` int(11) NOT NULL default '0',
  `spellcooldown_5` int(11) NOT NULL default '0',
  `spellcategory_5` int(10) unsigned NOT NULL default '0',
  `spellcategorycooldown_5` int(11) NOT NULL default '0',
  `bonding` int(10) unsigned NOT NULL default '0',
  `description` varchar(255) collate utf8_unicode_ci NOT NULL,
  `page_id` int(10) unsigned NOT NULL default '0',
  `page_language` int(10) unsigned NOT NULL default '0',
  `page_material` int(10) unsigned NOT NULL default '0',
  `quest_id` int(10) unsigned NOT NULL default '0',
  `lock_id` int(10) unsigned NOT NULL default '0',
  `lock_material` int(10) unsigned NOT NULL default '0',
  `sheathID` int(10) unsigned NOT NULL default '0',
  `randomprop` int(10) unsigned NOT NULL default '0',
  `unk203_1` int(10) unsigned NOT NULL default '0',
  `block` int(10) unsigned NOT NULL default '0',
  `itemset` int(10) unsigned NOT NULL default '0',
  `MaxDurability` int(10) unsigned NOT NULL default '0',
  `ZoneNameID` int(10) unsigned NOT NULL default '0',
  `mapid` int(10) unsigned default NULL,
  `bagfamily` int(10) unsigned NOT NULL default '0',
  `TotemCategory` int(10) unsigned default NULL,
  `socket_color_1` int(10) unsigned default NULL,
  `unk201_3` int(10) unsigned NOT NULL default '0',
  `socket_color_2` int(10) unsigned default NULL,
  `unk201_5` int(10) unsigned NOT NULL default '0',
  `socket_color_3` int(10) unsigned default NULL,
  `unk201_7` int(10) unsigned NOT NULL default '0',
  `socket_bonus` int(10) unsigned default NULL,
  `GemProperties` int(10) unsigned default NULL,
  `ItemExtendedCost` int(10) unsigned default NULL,
  `ReqDisenchantSkill` int(10) unsigned NOT NULL default '0',
  `unk2` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`),
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `lootrandomprop`
--

DROP TABLE IF EXISTS `lootrandomprop`;
CREATE TABLE `lootrandomprop` (
  `entryid` int(10) unsigned NOT NULL default '0',
  `propid` int(10) unsigned NOT NULL default '0',
  `percentchance` float default NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `mailbox`
--

DROP TABLE IF EXISTS `mailbox`;
CREATE TABLE `mailbox` (
  `message_id` int(10) unsigned NOT NULL default '0',
  `message_type` int(10) unsigned NOT NULL default '0',
  `player_guid` bigint(20) unsigned NOT NULL default '0',
  `sender_guid` bigint(20) unsigned NOT NULL default '0',
  `subject` varchar(255) collate utf8_unicode_ci NOT NULL,
  `body` longtext collate utf8_unicode_ci NOT NULL,
  `money` int(10) unsigned NOT NULL default '0',
  `attached_item_guid` bigint(20) unsigned NOT NULL default '0',
  `cod` int(10) unsigned NOT NULL default '0',
  `stationary` int(10) unsigned NOT NULL default '0',
  `expiry_time` int(10) unsigned NOT NULL default '0',
  `delivery_time` int(10) unsigned NOT NULL default '0',
  `copy_made` int(10) unsigned NOT NULL default '0',
  `read_flag` tinyint(3) unsigned NOT NULL default '0',
  `deleted_flag` tinyint(3) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `npc_gossip_textid`
--

DROP TABLE IF EXISTS `npc_gossip_textid`;
CREATE TABLE `npc_gossip_textid` (
  `entryid` int(10) unsigned NOT NULL default '0',
  `textid` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entryid`),
  UNIQUE KEY `index` (`entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `npc_monstersay`
--

DROP TABLE IF EXISTS `npc_monstersay`;
CREATE TABLE `npc_monstersay` (
  `entry` int(10) unsigned NOT NULL default '0',
  `event` int(10) unsigned NOT NULL default '0',
  `chance` float NOT NULL default '0',
  `language` int(10) unsigned NOT NULL default '0',
  `type` int(10) unsigned NOT NULL default '0',
  `monstername` longtext collate utf8_unicode_ci,
  `text0` longtext collate utf8_unicode_ci,
  `text1` longtext collate utf8_unicode_ci,
  `text2` longtext collate utf8_unicode_ci,
  `text3` longtext collate utf8_unicode_ci,
  `text4` longtext collate utf8_unicode_ci
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `npc_text`
--

DROP TABLE IF EXISTS `npc_text`;
CREATE TABLE `npc_text` (
  `entry` int(10) unsigned NOT NULL default '0',
  `prob0` float NOT NULL default '0',
  `text0_0` longtext collate utf8_unicode_ci NOT NULL,
  `text0_1` longtext collate utf8_unicode_ci NOT NULL,
  `lang0` int(10) unsigned NOT NULL default '0',
  `em0_0` int(10) unsigned NOT NULL default '0',
  `em0_1` int(10) unsigned NOT NULL default '0',
  `em0_2` int(10) unsigned NOT NULL default '0',
  `em0_3` int(10) unsigned NOT NULL default '0',
  `em0_4` int(10) unsigned NOT NULL default '0',
  `em0_5` int(10) unsigned NOT NULL default '0',
  `prob1` float NOT NULL default '0',
  `text1_0` longtext collate utf8_unicode_ci NOT NULL,
  `text1_1` longtext collate utf8_unicode_ci NOT NULL,
  `lang1` int(10) unsigned NOT NULL default '0',
  `em1_0` int(10) unsigned NOT NULL default '0',
  `em1_1` int(10) unsigned NOT NULL default '0',
  `em1_2` int(10) unsigned NOT NULL default '0',
  `em1_3` int(10) unsigned NOT NULL default '0',
  `em1_4` int(10) unsigned NOT NULL default '0',
  `em1_5` int(10) unsigned NOT NULL default '0',
  `prob2` float NOT NULL default '0',
  `text2_0` longtext collate utf8_unicode_ci NOT NULL,
  `text2_1` longtext collate utf8_unicode_ci NOT NULL,
  `lang2` int(10) unsigned NOT NULL default '0',
  `em2_0` int(10) unsigned NOT NULL default '0',
  `em2_1` int(10) unsigned NOT NULL default '0',
  `em2_2` int(10) unsigned NOT NULL default '0',
  `em2_3` int(10) unsigned NOT NULL default '0',
  `em2_4` int(10) unsigned NOT NULL default '0',
  `em2_5` int(10) unsigned NOT NULL default '0',
  `prob3` float NOT NULL default '0',
  `text3_0` longtext collate utf8_unicode_ci NOT NULL,
  `text3_1` longtext collate utf8_unicode_ci NOT NULL,
  `lang3` int(10) unsigned NOT NULL default '0',
  `em3_0` int(10) unsigned NOT NULL default '0',
  `em3_1` int(10) unsigned NOT NULL default '0',
  `em3_2` int(10) unsigned NOT NULL default '0',
  `em3_3` int(10) unsigned NOT NULL default '0',
  `em3_4` int(10) unsigned NOT NULL default '0',
  `em3_5` int(10) unsigned NOT NULL default '0',
  `prob4` float NOT NULL default '0',
  `text4_0` longtext collate utf8_unicode_ci NOT NULL,
  `text4_1` longtext collate utf8_unicode_ci NOT NULL,
  `lang4` int(10) unsigned NOT NULL default '0',
  `em4_0` int(10) unsigned NOT NULL default '0',
  `em4_1` int(10) unsigned NOT NULL default '0',
  `em4_2` int(10) unsigned NOT NULL default '0',
  `em4_3` int(10) unsigned NOT NULL default '0',
  `em4_4` int(10) unsigned NOT NULL default '0',
  `em4_5` int(10) unsigned NOT NULL default '0',
  `prob5` float NOT NULL default '0',
  `text5_0` longtext collate utf8_unicode_ci NOT NULL,
  `text5_1` longtext collate utf8_unicode_ci NOT NULL,
  `lang5` int(10) unsigned NOT NULL default '0',
  `em5_0` int(10) unsigned NOT NULL default '0',
  `em5_1` int(10) unsigned NOT NULL default '0',
  `em5_2` int(10) unsigned NOT NULL default '0',
  `em5_3` int(10) unsigned NOT NULL default '0',
  `em5_4` int(10) unsigned NOT NULL default '0',
  `em5_5` int(10) unsigned NOT NULL default '0',
  `prob6` float NOT NULL default '0',
  `text6_0` longtext collate utf8_unicode_ci NOT NULL,
  `text6_1` longtext collate utf8_unicode_ci NOT NULL,
  `lang6` int(10) unsigned NOT NULL default '0',
  `em6_0` int(10) unsigned NOT NULL default '0',
  `em6_1` int(10) unsigned NOT NULL default '0',
  `em6_2` int(10) unsigned NOT NULL default '0',
  `em6_3` int(10) unsigned NOT NULL default '0',
  `em6_4` int(10) unsigned NOT NULL default '0',
  `em6_5` int(10) unsigned NOT NULL default '0',
  `prob7` float NOT NULL default '0',
  `text7_0` longtext collate utf8_unicode_ci NOT NULL,
  `text7_1` longtext collate utf8_unicode_ci NOT NULL,
  `lang7` int(10) unsigned NOT NULL default '0',
  `em7_0` int(10) unsigned NOT NULL default '0',
  `em7_1` int(10) unsigned NOT NULL default '0',
  `em7_2` int(10) unsigned NOT NULL default '0',
  `em7_3` int(10) unsigned NOT NULL default '0',
  `em7_4` int(10) unsigned NOT NULL default '0',
  `em7_5` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `objectloot`
--

DROP TABLE IF EXISTS `objectloot`;
CREATE TABLE `objectloot` (
  `index` int(10) unsigned NOT NULL auto_increment,
  `entryid` int(10) unsigned NOT NULL default '0',
  `itemid` int(10) unsigned NOT NULL default '0',
  `percentchance` float default NULL,
  PRIMARY KEY  (`index`),
  KEY `entryid` (`entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `petdefaultspells`
--

DROP TABLE IF EXISTS `petdefaultspells`;
CREATE TABLE `petdefaultspells` (
  `entry` int(10) unsigned NOT NULL default '0',
  `spell` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playercooldownitems`
--

DROP TABLE IF EXISTS `playercooldownitems`;
CREATE TABLE `playercooldownitems` (
  `OwnerGuid` int(10) unsigned NOT NULL default '0',
  `ItemEntry` int(10) unsigned NOT NULL default '0',
  `SpellID` int(10) unsigned NOT NULL default '0',
  `SpellCategory` int(10) unsigned NOT NULL default '0',
  `CooldownTimeStamp` int(10) unsigned NOT NULL default '0',
  `Cooldown` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playercooldownsecurity`
--

DROP TABLE IF EXISTS `playercooldownsecurity`;
CREATE TABLE `playercooldownsecurity` (
  `OwnerGuid` int(10) unsigned NOT NULL default '0',
  `SpellID` int(10) unsigned NOT NULL default '0',
  `TimeStamp` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playercreateinfo`
--

DROP TABLE IF EXISTS `playercreateinfo`;
CREATE TABLE `playercreateinfo` (
  `Index` tinyint(3) unsigned NOT NULL auto_increment,
  `race` tinyint(3) unsigned NOT NULL default '0',
  `factiontemplate` int(10) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `mapID` int(10) unsigned NOT NULL default '0',
  `zoneID` int(10) unsigned NOT NULL default '0',
  `positionX` float NOT NULL default '0',
  `positionY` float NOT NULL default '0',
  `positionZ` float NOT NULL default '0',
  `displayID` smallint(5) unsigned NOT NULL default '0',
  `BaseStrength` tinyint(3) unsigned NOT NULL default '0',
  `BaseAgility` tinyint(3) unsigned NOT NULL default '0',
  `BaseStamina` tinyint(3) unsigned NOT NULL default '0',
  `BaseIntellect` tinyint(3) unsigned NOT NULL default '0',
  `BaseSpirit` tinyint(3) unsigned NOT NULL default '0',
  `BaseHealth` int(10) unsigned NOT NULL default '0',
  `BaseMana` int(10) unsigned NOT NULL default '0',
  `BaseRage` int(10) unsigned NOT NULL default '0',
  `BaseFocus` int(10) unsigned NOT NULL default '0',
  `BaseEnergy` int(10) unsigned NOT NULL default '0',
  `attackpower` int(10) unsigned NOT NULL default '0',
  `mindmg` float NOT NULL default '0',
  `maxdmg` float NOT NULL default '0',
  PRIMARY KEY  (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playercreateinfo_bars`
--

DROP TABLE IF EXISTS `playercreateinfo_bars`;
CREATE TABLE `playercreateinfo_bars` (
  `race` tinyint(3) unsigned default NULL,
  `class` tinyint(3) unsigned default NULL,
  `button` int(10) unsigned default NULL,
  `action` int(10) unsigned default NULL,
  `type` int(10) unsigned default NULL,
  `misc` int(10) unsigned default NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playercreateinfo_items`
--

DROP TABLE IF EXISTS `playercreateinfo_items`;
CREATE TABLE `playercreateinfo_items` (
  `indexid` tinyint(3) unsigned NOT NULL default '0',
  `protoid` int(10) unsigned NOT NULL default '0',
  `slotid` tinyint(3) unsigned NOT NULL default '0',
  `amount` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playercreateinfo_skills`
--

DROP TABLE IF EXISTS `playercreateinfo_skills`;
CREATE TABLE `playercreateinfo_skills` (
  `indexid` tinyint(3) unsigned NOT NULL default '0',
  `skillid` int(10) unsigned NOT NULL default '0',
  `level` int(10) unsigned NOT NULL default '0',
  `maxlevel` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playercreateinfo_spells`
--

DROP TABLE IF EXISTS `playercreateinfo_spells`;
CREATE TABLE `playercreateinfo_spells` (
  `indexid` tinyint(3) unsigned NOT NULL default '0',
  `spellid` smallint(5) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playeritems`
--

DROP TABLE IF EXISTS `playeritems`;
CREATE TABLE `playeritems` (
  `ownerguid` int(10) unsigned NOT NULL default '0',
  `guid` bigint(20) unsigned NOT NULL default '0',
  `entry` int(10) unsigned NOT NULL default '0',
  `creator` int(10) unsigned NOT NULL default '0',
  `count` int(10) unsigned NOT NULL default '0',
  `charges` int(10) unsigned NOT NULL default '0',
  `flags` int(10) unsigned NOT NULL default '0',
  `randomprop` int(10) unsigned NOT NULL default '0',
  `itemtext` int(10) unsigned NOT NULL default '0',
  `durability` int(10) unsigned NOT NULL default '0',
  `containerslot` int(11) default '-1' COMMENT 'couldnt find this being used in source',
  `slot` tinyint(4) NOT NULL default '0',
  `enchantments` longtext collate utf8_unicode_ci NOT NULL,
  PRIMARY KEY  (`guid`),
  UNIQUE KEY `guid` (`guid`),
  KEY `ownerguid` (`ownerguid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playerpets`
--

DROP TABLE IF EXISTS `playerpets`;
CREATE TABLE `playerpets` (
  `ownerguid` bigint(20) unsigned NOT NULL default '0',
  `petnumber` int(10) unsigned NOT NULL default '0',
  `name` varchar(21) collate utf8_unicode_ci NOT NULL,
  `entryid` int(10) unsigned NOT NULL default '0',
  `data` longtext collate utf8_unicode_ci NOT NULL,
  `xp` int(10) unsigned NOT NULL default '0',
  `active` tinyint(3) unsigned NOT NULL default '0',
  `level` int(10) unsigned NOT NULL default '0',
  `loyalty` int(10) unsigned NOT NULL default '0',
  `actionbar` varchar(200) collate utf8_unicode_ci NOT NULL,
  `nextloyaltyupdate` int(10) unsigned NOT NULL default '0',
  `summon` int(10) unsigned NOT NULL default '0',
  `autocastspell` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playerpetspells`
--

DROP TABLE IF EXISTS `playerpetspells`;
CREATE TABLE `playerpetspells` (
  `ownerguid` bigint(20) unsigned NOT NULL default '0',
  `petnumber` int(10) unsigned NOT NULL default '0',
  `spellid` int(10) unsigned NOT NULL default '0',
  `flags` smallint(5) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `playersummonspells`
--

DROP TABLE IF EXISTS `playersummonspells`;
CREATE TABLE `playersummonspells` (
  `ownerguid` bigint(20) unsigned NOT NULL default '0',
  `entryid` int(10) unsigned NOT NULL default '0',
  `spellid` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `questlog`
--

DROP TABLE IF EXISTS `questlog`;
CREATE TABLE `questlog` (
  `index` bigint(20) unsigned NOT NULL auto_increment,
  `player_guid` bigint(20) unsigned NOT NULL default '0',
  `quest_id` int(10) unsigned NOT NULL default '0',
  `slot` int(10) unsigned NOT NULL default '0',
  `time_left` int(10) unsigned NOT NULL default '0',
  `explored_area1` int(10) unsigned NOT NULL default '0',
  `explored_area2` int(10) unsigned NOT NULL default '0',
  `explored_area3` int(10) unsigned NOT NULL default '0',
  `explored_area4` int(10) unsigned NOT NULL default '0',
  `mob_kill1` int(10) unsigned NOT NULL default '0',
  `mob_kill2` int(10) unsigned NOT NULL default '0',
  `mob_kill3` int(10) unsigned NOT NULL default '0',
  `mob_kill4` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`index`),
  KEY `index` (`index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `quests`
--

DROP TABLE IF EXISTS `quests`;
CREATE TABLE `quests` (
  `entry` int(10) unsigned NOT NULL default '0',
  `ZoneId` int(10) unsigned NOT NULL default '0',
  `QuestSort` int(10) unsigned NOT NULL default '0',
  `QuestFlags` int(10) unsigned NOT NULL default '0',
  `MinLevel` int(10) unsigned NOT NULL default '0',
  `MaxLevel` int(10) unsigned NOT NULL default '0',
  `Type` int(10) unsigned NOT NULL default '0',
  `RequiredRaces` int(10) unsigned NOT NULL default '0',
  `RequiredClass` int(10) unsigned NOT NULL default '0',
  `RequiredTradeskill` int(10) unsigned NOT NULL default '0',
  `RequiredRepFaction` int(10) unsigned NOT NULL default '0',
  `RequiredRepValue` int(10) unsigned NOT NULL default '0',
  `LimitTime` int(10) unsigned NOT NULL default '0',
  `SpecialFlags` int(10) unsigned NOT NULL default '0',
  `PrevQuestId` int(10) unsigned NOT NULL default '0',
  `NextQuestId` int(10) unsigned NOT NULL default '0',
  `srcItem` int(10) unsigned NOT NULL default '0',
  `SrcItemCount` int(10) unsigned NOT NULL default '0',
  `Title` text collate utf8_unicode_ci NOT NULL,
  `Details` text collate utf8_unicode_ci NOT NULL,
  `Objectives` text collate utf8_unicode_ci NOT NULL,
  `CompletionText` text collate utf8_unicode_ci NOT NULL,
  `IncompleteText` text collate utf8_unicode_ci NOT NULL,
  `EndText` text collate utf8_unicode_ci NOT NULL,
  `ObjectiveText1` text collate utf8_unicode_ci NOT NULL,
  `ObjectiveText2` text collate utf8_unicode_ci NOT NULL,
  `ObjectiveText3` text collate utf8_unicode_ci NOT NULL,
  `ObjectiveText4` text collate utf8_unicode_ci NOT NULL,
  `ReqItemId1` int(10) unsigned NOT NULL default '0',
  `ReqItemId2` int(10) unsigned NOT NULL default '0',
  `ReqItemId3` int(10) unsigned NOT NULL default '0',
  `ReqItemId4` int(10) unsigned NOT NULL default '0',
  `ReqItemCount1` int(10) unsigned NOT NULL default '0',
  `ReqItemCount2` int(10) unsigned NOT NULL default '0',
  `ReqItemCount3` int(10) unsigned NOT NULL default '0',
  `ReqItemCount4` int(10) unsigned NOT NULL default '0',
  `ReqKillMobOrGOId1` int(10) unsigned NOT NULL default '0',
  `ReqKillMobOrGOId2` int(10) unsigned NOT NULL default '0',
  `ReqKillMobOrGOId3` int(10) unsigned NOT NULL default '0',
  `ReqKillMobOrGOId4` int(10) unsigned NOT NULL default '0',
  `ReqKillMobOrGOCount1` int(10) unsigned NOT NULL default '0',
  `ReqKillMobOrGOCount2` int(10) unsigned NOT NULL default '0',
  `ReqKillMobOrGOCount3` int(10) unsigned NOT NULL default '0',
  `ReqKillMobOrGOCount4` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemId1` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemId2` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemId3` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemId4` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemId5` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemId6` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemCount1` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemCount2` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemCount3` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemCount4` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemCount5` int(10) unsigned NOT NULL default '0',
  `RewChoiceItemCount6` int(10) unsigned NOT NULL default '0',
  `RewItemId1` int(10) unsigned NOT NULL default '0',
  `RewItemId2` int(10) unsigned NOT NULL default '0',
  `RewItemId3` int(10) unsigned NOT NULL default '0',
  `RewItemId4` int(10) unsigned NOT NULL default '0',
  `RewItemCount1` int(10) unsigned NOT NULL default '0',
  `RewItemCount2` int(10) unsigned NOT NULL default '0',
  `RewItemCount3` int(10) unsigned NOT NULL default '0',
  `RewItemCount4` int(10) unsigned NOT NULL default '0',
  `RewRepFaction1` int(10) unsigned NOT NULL default '0',
  `RewRepFaction2` int(10) unsigned NOT NULL default '0',
  `RewRepValue1` int(10) unsigned NOT NULL default '0',
  `RewRepValue2` int(10) unsigned NOT NULL default '0',
  `RewRepLimit` int(10) unsigned NOT NULL default '0',
  `RewMoney` int(10) unsigned NOT NULL default '0',
  `RewXP` int(10) unsigned NOT NULL default '0',
  `RewSpell` int(10) unsigned NOT NULL default '0',
  `CastSpell` int(10) unsigned NOT NULL default '0',
  `PointMapId` int(10) unsigned NOT NULL default '0',
  `PointX` float NOT NULL default '0',
  `PointY` float NOT NULL default '0',
  `PointOpt` int(10) unsigned NOT NULL default '0',
  `RequiredMoney` int(10) unsigned NOT NULL default '0',
  `ExploreTrigger1` int(10) unsigned NOT NULL default '0',
  `ExploreTrigger2` int(10) unsigned NOT NULL default '0',
  `ExploreTrigger3` int(10) unsigned NOT NULL default '0',
  `ExploreTrigger4` int(10) unsigned NOT NULL default '0',
  `RequiredQuest1` int(10) unsigned NOT NULL default '0',
  `RequiredQuest2` int(10) unsigned NOT NULL default '0',
  `RequiredQuest3` int(10) unsigned NOT NULL default '0',
  `RequiredQuest4` int(10) unsigned NOT NULL default '0',
  `ReceiveItemId1` int(10) unsigned NOT NULL default '0',
  `ReceiveItemId2` int(10) unsigned NOT NULL default '0',
  `ReceiveItemId3` int(10) unsigned NOT NULL default '0',
  `ReceiveItemId4` int(10) unsigned NOT NULL default '0',
  `ReceiveItemCount1` int(10) unsigned NOT NULL default '0',
  `ReceiveItemCount2` int(10) unsigned NOT NULL default '0',
  `ReceiveItemCount3` int(10) unsigned NOT NULL default '0',
  `ReceiveItemCount4` int(10) unsigned NOT NULL default '0',
  `IsRepeatable` int(11) NOT NULL default '0',
  PRIMARY KEY  (`entry`),
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `recall`
--

DROP TABLE IF EXISTS `recall`;
CREATE TABLE `recall` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `locname` varchar(10) collate utf8_unicode_ci NOT NULL,
  `mapid` int(10) unsigned NOT NULL default '0',
  `positionX` float NOT NULL default '0',
  `positionY` float NOT NULL default '0',
  `positionZ` float NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `reputation_creature_onkill`
--

DROP TABLE IF EXISTS `reputation_creature_onkill`;
CREATE TABLE `reputation_creature_onkill` (
  `creature_id` int(10) unsigned NOT NULL default '0',
  `faction_change_alliance` int(10) unsigned NOT NULL default '0',
  `faction_change_horde` int(10) unsigned NOT NULL default '0',
  `change_value` int(11) NOT NULL default '0',
  `rep_limit` int(11) NOT NULL default '0',
  KEY `index` (`creature_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `reputation_faction_onkill`
--

DROP TABLE IF EXISTS `reputation_faction_onkill`;
CREATE TABLE `reputation_faction_onkill` (
  `faction_id` int(30) NOT NULL default '0',
  `change_factionid_alliance` int(30) NOT NULL default '0',
  `change_deltamin_alliance` int(30) NOT NULL default '0',
  `change_deltamax_alliance` int(30) NOT NULL default '0',
  `change_factionid_horde` int(30) NOT NULL default '0',
  `change_deltamin_horde` int(30) NOT NULL default '0',
  `change_deltamax_horde` int(30) NOT NULL default '0',
  KEY `factindex` (`faction_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='source does not match this table';

--
-- Table structure for table `reputation_instance_onkill`
--

DROP TABLE IF EXISTS `reputation_instance_onkill`;
CREATE TABLE `reputation_instance_onkill` (
  `mapid` int(10) unsigned NOT NULL default '0',
  `mob_rep_reward` int(11) NOT NULL default '0',
  `mob_rep_limit` int(10) unsigned NOT NULL default '0',
  `boss_rep_reward` int(11) NOT NULL default '0',
  `boss_rep_limit` int(10) unsigned NOT NULL default '0',
  `faction_change_alliance` int(10) unsigned NOT NULL default '0',
  `faction_change_horde` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `skinningloot`
--

DROP TABLE IF EXISTS `skinningloot`;
CREATE TABLE `skinningloot` (
  `index` int(10) unsigned NOT NULL auto_increment,
  `entryid` int(10) unsigned NOT NULL default '0',
  `itemid` int(10) unsigned NOT NULL default '0',
  `percentchance` float NOT NULL default '0',
  PRIMARY KEY  (`index`),
  UNIQUE KEY `index` (`index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `social`
--

DROP TABLE IF EXISTS `social`;
CREATE TABLE `social` (
  `guid` bigint(20) unsigned NOT NULL default '0',
  `socialguid` bigint(20) unsigned NOT NULL default '0',
  `flags` varchar(21) collate utf8_unicode_ci NOT NULL,
  `noticed` tinyint(3) unsigned NOT NULL default '1',
  PRIMARY KEY  (`guid`,`socialguid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `spell_disable`
--

DROP TABLE IF EXISTS `spell_disable`;
CREATE TABLE `spell_disable` (
  `spellid` int(10) unsigned NOT NULL default '0',
  `replacement_spellid` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `spell_disable_trainers`
--

DROP TABLE IF EXISTS `spell_disable_trainers`;
CREATE TABLE `spell_disable_trainers` (
  `spellid` int(10) unsigned NOT NULL default '0',
  `replacement_spellid` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `spellextra`
--

DROP TABLE IF EXISTS `spellextra`;
CREATE TABLE `spellextra` (
  `Id` int(10) unsigned NOT NULL auto_increment,
  `specialtype` int(10) unsigned default NULL,
  `enchantableslots` int(10) unsigned default NULL,
  `ExtraFlags` int(10) unsigned default NULL,
  PRIMARY KEY  (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `spelloverride`
--

DROP TABLE IF EXISTS `spelloverride`;
CREATE TABLE `spelloverride` (
  `overrideId` int(10) unsigned NOT NULL default '0',
  `spellId` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `overrideId` (`overrideId`,`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `spells112`
--

DROP TABLE IF EXISTS `spells112`;
CREATE TABLE `spells112` (
  `Id` int(100) default '0',
  `School` int(100) default '0',
  `Category` int(100) default '0',
  `field4` int(100) default '0',
  `DispelType` int(100) default '0',
  `MechanicsType` int(100) default '0',
  `Attributes` int(100) default '0',
  `AttributesEx` int(100) default '0',
  `Flags3` int(100) default '0',
  `field10` int(100) default '0',
  `field11` int(100) default '0',
  `UNK12` int(100) default '0',
  `UNK13` int(100) default '0',
  `Targets` int(100) default '0',
  `TargetCreatureType` int(100) default '0',
  `RequiresSpellFocus` int(100) default '0',
  `CasterAuraState` int(100) default '0',
  `TargetAuraState` int(100) default '0',
  `CastingTimeIndex` int(100) default '0',
  `RecoveryTime` int(100) default '0',
  `CategoryRecoveryTime` int(100) default '0',
  `InterruptFlags` int(100) default '0',
  `AuraInterruptFlags` int(100) default '0',
  `ChannelInterruptFlags` int(100) default '0',
  `procFlags` int(100) default '0',
  `procChance` int(100) default '0',
  `procCharges` int(100) default '0',
  `maxLevel` int(100) default '0',
  `baseLevel` int(100) default '0',
  `spellLevel` int(100) default '0',
  `DurationIndex` int(100) default '0',
  `powerType` int(100) default '0',
  `manaCost` int(100) default '0',
  `manaCostPerlevel` int(100) default '0',
  `manaPerSecond` int(100) default '0',
  `manaPerSecondPerLevel` int(100) default '0',
  `rangeIndex` int(100) default '0',
  `speed` float default '0',
  `modalNextSpell` int(100) default '0',
  `maxstack` int(100) default '0',
  `Totem1` int(100) default '0',
  `Totem2` int(100) default '0',
  `Reagent1` int(100) default '0',
  `Reagent2` int(100) default '0',
  `Reagent3` int(100) default '0',
  `Reagent4` int(100) default '0',
  `Reagent5` int(100) default '0',
  `Reagent6` int(100) default '0',
  `Reagent7` int(100) default '0',
  `Reagent8` int(100) default '0',
  `ReagentCount1` int(100) default '0',
  `ReagentCount2` int(100) default '0',
  `ReagentCount3` int(100) default '0',
  `ReagentCount4` int(100) default '0',
  `ReagentCount5` int(100) default '0',
  `ReagentCount6` int(100) default '0',
  `ReagentCount7` int(100) default '0',
  `ReagentCount8` int(100) default '0',
  `EquippedItemClass` int(100) default '0',
  `EquippedItemSubClass` int(100) default '0',
  `RequiredItemFlags` int(100) default '0',
  `Effect1` int(100) default '0',
  `Effect2` int(100) default '0',
  `Effect3` int(100) default '0',
  `EffectDieSides1` int(100) default '0',
  `EffectDieSides2` int(100) default '0',
  `EffectDieSides3` int(100) default '0',
  `EffectBaseDice1` int(100) default '0',
  `EffectBaseDice2` int(100) default '0',
  `EffectBaseDice3` int(100) default '0',
  `EffectDicePerLevel1` float default '0',
  `EffectDicePerLevel2` float default '0',
  `EffectDicePerLevel3` float default '0',
  `EffectRealPointsPerLevel1` float default '0',
  `EffectRealPointsPerLevel2` float default '0',
  `EffectRealPointsPerLevel3` float default '0',
  `EffectBasePoints1` int(100) default '0',
  `EffectBasePoints2` int(100) default '0',
  `EffectBasePoints3` int(100) default '0',
  `Effectunk1901` int(100) default '0',
  `Effectunk1902` int(100) default '0',
  `Effectunk1903` int(100) default '0',
  `EffectImplicitTargetA1` int(100) default '0',
  `EffectImplicitTargetA2` int(100) default '0',
  `EffectImplicitTargetA3` int(100) default '0',
  `EffectImplicitTargetB1` int(100) default '0',
  `EffectImplicitTargetB2` int(100) default '0',
  `EffectImplicitTargetB3` int(100) default '0',
  `EffectRadiusIndex1` int(100) default '0',
  `EffectRadiusIndex2` int(100) default '0',
  `EffectRadiusIndex3` int(100) default '0',
  `EffectApplyAuraName1` int(100) default '0',
  `EffectApplyAuraName2` int(100) default '0',
  `EffectApplyAuraName3` int(100) default '0',
  `EffectAmplitude1` int(100) default '0',
  `EffectAmplitude2` int(100) default '0',
  `EffectAmplitude3` int(100) default '0',
  `Effectunknown1` float default '0',
  `Effectunknown2` float default '0',
  `Effectunknown3` float default '0',
  `EffectChainTarget1` int(100) default '0',
  `EffectChainTarget2` int(100) default '0',
  `EffectChainTarget3` int(100) default '0',
  `EffectSpellGroupRelation1` int(100) default '0',
  `EffectSpellGroupRelation2` int(100) default '0',
  `EffectSpellGroupRelation3` int(100) default '0',
  `EffectMiscValue1` int(100) default '0',
  `EffectMiscValue2` int(100) default '0',
  `EffectMiscValue3` int(100) default '0',
  `EffectTriggerSpell1` int(100) default '0',
  `EffectTriggerSpell2` int(100) default '0',
  `EffectTriggerSpell3` int(100) default '0',
  `EffectPointsPerComboPoint1` float default '0',
  `EffectPointsPerComboPoint2` float default '0',
  `EffectPointsPerComboPoint3` float default '0',
  `SpellVisual` int(100) default '0',
  `field114` int(100) default '0',
  `dummy` int(100) default '0',
  `CoSpell` int(100) default '0',
  `spellPriority` int(100) default '0',
  `Name` longtext collate utf8_unicode_ci,
  `NameAlt1` longtext collate utf8_unicode_ci,
  `NameAlt2` longtext collate utf8_unicode_ci,
  `NameAlt3` longtext collate utf8_unicode_ci,
  `NameAlt4` longtext collate utf8_unicode_ci,
  `NameAlt5` longtext collate utf8_unicode_ci,
  `NameAlt6` longtext collate utf8_unicode_ci,
  `NameAlt7` longtext collate utf8_unicode_ci,
  `NameFlags` longtext collate utf8_unicode_ci,
  `Rank` longtext collate utf8_unicode_ci,
  `RankAlt1` longtext collate utf8_unicode_ci,
  `RankAlt2` longtext collate utf8_unicode_ci,
  `RankAlt3` longtext collate utf8_unicode_ci,
  `RankAlt4` longtext collate utf8_unicode_ci,
  `RankAlt5` longtext collate utf8_unicode_ci,
  `RankAlt6` longtext collate utf8_unicode_ci,
  `RankAlt7` longtext collate utf8_unicode_ci,
  `RankFlags` longtext collate utf8_unicode_ci,
  `Description` longtext collate utf8_unicode_ci,
  `DescriptionAlt1` longtext collate utf8_unicode_ci,
  `DescriptionAlt2` longtext collate utf8_unicode_ci,
  `DescriptionAlt3` longtext collate utf8_unicode_ci,
  `DescriptionAlt4` longtext collate utf8_unicode_ci,
  `DescriptionAlt5` longtext collate utf8_unicode_ci,
  `DescriptionAlt6` longtext collate utf8_unicode_ci,
  `DescriptionAlt7` longtext collate utf8_unicode_ci,
  `DescriptionFlags` longtext collate utf8_unicode_ci,
  `BuffDescription` longtext collate utf8_unicode_ci,
  `BuffDescriptionAlt1` longtext collate utf8_unicode_ci,
  `BuffDescriptionAlt2` longtext collate utf8_unicode_ci,
  `BuffDescriptionAlt3` longtext collate utf8_unicode_ci,
  `BuffDescriptionAlt4` longtext collate utf8_unicode_ci,
  `BuffDescriptionAlt5` longtext collate utf8_unicode_ci,
  `BuffDescriptionAlt6` longtext collate utf8_unicode_ci,
  `BuffDescriptionAlt7` longtext collate utf8_unicode_ci,
  `buffdescflags` int(100) default '0',
  `ManaCostPercentage` int(100) default '0',
  `unkflags` int(100) default '0',
  `StartRecoveryTime` int(100) default '0',
  `StartRecoveryCategory` int(100) default '0',
  `SpellFamilyName` int(100) default '0',
  `SpellGroupType` int(100) default '0',
  `unkne` int(100) default '0',
  `MaxTargets` int(100) default '0',
  `Spell_Dmg_Type` int(100) default '0',
  `FG` int(100) default '0',
  `FH` int(100) default '0',
  `dmg_multiplier_1` float default '0',
  `dmg_multiplier_2` float default '0',
  `dmg_multiplier_3` float default '0',
  `FL` int(100) default '0',
  `FM` int(100) default '0',
  `FN` int(100) default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='not touching this :P';

--
-- Table structure for table `teleport_coords`
--

DROP TABLE IF EXISTS `teleport_coords`;
CREATE TABLE `teleport_coords` (
  `id` int(10) unsigned NOT NULL default '0',
  `name` char(255) collate utf8_unicode_ci NOT NULL,
  `mapId` int(10) unsigned NOT NULL default '0',
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `totrigger` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `totemspells`
--

DROP TABLE IF EXISTS `totemspells`;
CREATE TABLE `totemspells` (
  `spellId` int(10) unsigned NOT NULL default '0',
  `spellToCast1` int(10) unsigned NOT NULL default '0',
  `spellToCast2` int(10) unsigned NOT NULL default '0',
  `spellToCast3` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `trainers`
--

DROP TABLE IF EXISTS `trainers`;
CREATE TABLE `trainers` (
  `creature_id` int(10) unsigned NOT NULL default '0',
  `SkillLine1` int(10) unsigned NOT NULL default '0',
  `SkillLine2` int(10) unsigned NOT NULL default '0',
  `SkillLine3` int(10) unsigned NOT NULL default '0',
  `SkillLine4` int(10) unsigned NOT NULL default '0',
  `SkillLine5` int(10) unsigned NOT NULL default '0',
  `SkillLine6` int(10) unsigned NOT NULL default '0',
  `SkillLine7` int(10) unsigned NOT NULL default '0',
  `SkillLine8` int(10) unsigned NOT NULL default '0',
  `SkillLine9` int(10) unsigned NOT NULL default '0',
  `SkillLine10` int(10) unsigned NOT NULL default '0',
  `SkillLine11` int(10) unsigned NOT NULL default '0',
  `SkillLine12` int(10) unsigned NOT NULL default '0',
  `SkillLine13` int(10) unsigned NOT NULL default '0',
  `SkillLine14` int(10) unsigned NOT NULL default '0',
  `SkillLine15` int(10) unsigned NOT NULL default '0',
  `SkillLine16` int(10) unsigned NOT NULL default '0',
  `SkillLine17` int(10) unsigned NOT NULL default '0',
  `SkillLine18` int(10) unsigned NOT NULL default '0',
  `SkillLine19` int(10) unsigned NOT NULL default '0',
  `SkillLine20` int(10) unsigned NOT NULL default '0',
  `MaxLvL` int(10) unsigned NOT NULL default '0',
  `Class` int(10) unsigned default NULL,
  `TalkText` varchar(150) collate utf8_unicode_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `trainerspelloverride`
--

DROP TABLE IF EXISTS `trainerspelloverride`;
CREATE TABLE `trainerspelloverride` (
  `spellid` int(10) unsigned NOT NULL default '0',
  `cost` int(10) unsigned NOT NULL default '0',
  `requiredspell` int(10) unsigned NOT NULL default '0',
  `deletespell` int(10) unsigned NOT NULL default '0',
  `requiredskill` int(10) unsigned NOT NULL default '0',
  `requiredskillvalue` int(10) unsigned NOT NULL default '0',
  `reqlevel` int(10) unsigned NOT NULL default '0',
  `requiredclass` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `spellid` (`spellid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `transport_data`
--

DROP TABLE IF EXISTS `transport_data`;
CREATE TABLE `transport_data` (
  `entry` int(10) unsigned NOT NULL default '0',
  `name` text collate utf8_unicode_ci,
  `period` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `tutorials`
--

DROP TABLE IF EXISTS `tutorials`;
CREATE TABLE `tutorials` (
  `playerId` bigint(20) unsigned NOT NULL default '0',
  `tut0` int(10) unsigned NOT NULL default '0',
  `tut1` int(10) unsigned NOT NULL default '0',
  `tut2` int(10) unsigned NOT NULL default '0',
  `tut3` int(10) unsigned NOT NULL default '0',
  `tut4` int(10) unsigned NOT NULL default '0',
  `tut5` int(10) unsigned NOT NULL default '0',
  `tut6` int(10) unsigned NOT NULL default '0',
  `tut7` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`playerId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `vendors`
--

DROP TABLE IF EXISTS `vendors`;
CREATE TABLE `vendors` (
  `vendorGuid` int(10) unsigned NOT NULL default '0',
  `itemGuid` int(10) unsigned NOT NULL default '0',
  `amount` int(11) NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `weather`
--

DROP TABLE IF EXISTS `weather`;
CREATE TABLE `weather` (
  `zoneId` int(10) unsigned NOT NULL default '0',
  `effect_none` int(10) unsigned NOT NULL default '0',
  `effect_rain` int(10) unsigned NOT NULL default '0',
  `effect_snow` int(10) unsigned NOT NULL default '0',
  `effect_weak_rain` int(10) unsigned NOT NULL default '0',
  `effect_unk` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`zoneId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `worldmap_info`
--

DROP TABLE IF EXISTS `worldmap_info`;
CREATE TABLE `worldmap_info` (
  `mapid` int(10) unsigned NOT NULL default '0',
  `screenid` int(10) unsigned default '0',
  `type` int(10) unsigned default '0',
  `maxplayers` int(10) unsigned default '0',
  `minlevel` int(10) unsigned default '1',
  `repopx` float default '0',
  `repopy` float default '0',
  `repopz` float default '0',
  `repopmapid` int(10) unsigned default '0',
  `area_name` varchar(100) collate utf8_unicode_ci default '0',
  `flags` int(10) unsigned NOT NULL default '0',
  `cooldown` int(10) unsigned NOT NULL default '0',
  `lvl_mod_a` int(10) unsigned NOT NULL default '0',
  `required_quest` int(10) unsigned NOT NULL default '0',
  `required_item` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`mapid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2007-07-26  8:34:11
