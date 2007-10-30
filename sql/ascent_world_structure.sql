
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
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ai_agents` (
  `entryId` int(10) unsigned NOT NULL DEFAULT '0',
  `AI_AGENT` smallint(5) unsigned NOT NULL DEFAULT '0',
  `procEvent` int(10) unsigned NOT NULL DEFAULT '0',
  `procChance` int(10) unsigned NOT NULL DEFAULT '0',
  `procCount` int(10) unsigned NOT NULL DEFAULT '0',
  `spellId` int(10) unsigned NOT NULL DEFAULT '0',
  `spellType` int(10) unsigned NOT NULL DEFAULT '0',
  `spelltargetType` int(10) unsigned NOT NULL DEFAULT '0',
  `spellCooldown` int(11) NOT NULL DEFAULT '0',
  `floatMisc1` float NOT NULL DEFAULT '0',
  `Misc2` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`entryId`,`AI_AGENT`,`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ai_threattospellid` (
  `spellId` int(10) unsigned NOT NULL DEFAULT '0',
  `mod` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `areatriggers` (
  `AreaTriggerid` int(10) unsigned NOT NULL DEFAULT '0',
  `Type` tinyint(3) unsigned DEFAULT '0',
  `Mapid` int(10) unsigned DEFAULT NULL,
  `Screen` int(10) unsigned DEFAULT NULL,
  `Name` varchar(100) COLLATE utf8_unicode_ci DEFAULT '0',
  `x` float NOT NULL DEFAULT '0',
  `y` float NOT NULL DEFAULT '0',
  `z` float NOT NULL DEFAULT '0',
  `o` float NOT NULL DEFAULT '0',
  `required_honor_rank` int(10) unsigned NOT NULL DEFAULT '0',
  `required_level` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`AreaTriggerid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `auctionhouse` (
  `entryid` int(11) NOT NULL,
  `auctioneer` bigint(20) unsigned NOT NULL DEFAULT '0',
  `AHid` int(11) NOT NULL DEFAULT '0',
  UNIQUE KEY `entryid` (`entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `banned_names` (
  `name` varchar(30) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `banned_phrases` (
  `phrase` varchar(250) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`phrase`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `clientaddons` (
  `id` int(10) unsigned NOT NULL,
  `name` varchar(50) COLLATE utf8_unicode_ci DEFAULT NULL,
  `crc` bigint(20) unsigned DEFAULT NULL,
  `banned` int(10) unsigned NOT NULL DEFAULT '0',
  `showinlist` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `index` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `command_overrides` (
  `command_name` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `access_level` varchar(10) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`command_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creature_formations` (
  `creature_sqlid` int(10) unsigned NOT NULL DEFAULT '0',
  `followtarget_sqlid` int(10) unsigned NOT NULL DEFAULT '0',
  `follow_angle` float NOT NULL DEFAULT '0',
  `follow_dist` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`creature_sqlid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creature_names` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `creature_name` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `Subname` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `Flags1` int(10) unsigned DEFAULT '0',
  `type` int(10) unsigned DEFAULT '0',
  `Family` int(10) unsigned DEFAULT '0',
  `Rank` int(10) unsigned DEFAULT '0',
  `unk4` int(10) unsigned DEFAULT '0',
  `SpellDataID` int(10) unsigned DEFAULT NULL,
  `male_displayid` int(11) NOT NULL,
  `female_displayid` int(11) NOT NULL,
  `unknown_int1` int(11) NOT NULL DEFAULT '0',
  `unknown_int2` int(11) NOT NULL DEFAULT '0',
  `unknown_float1` float NOT NULL DEFAULT '1',
  `unknown_float2` float NOT NULL DEFAULT '1',
  `Civilian` int(10) unsigned DEFAULT NULL,
  `Leader` tinyint(3) unsigned DEFAULT NULL,
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creature_names_localized` (
  `id` int(30) unsigned NOT NULL,
  `language_code` varchar(5) COLLATE utf8_unicode_ci NOT NULL,
  `name` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `subname` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`,`language_code`),
  KEY `lol` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creature_proto` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `minlevel` int(10) unsigned NOT NULL,
  `maxlevel` int(10) unsigned NOT NULL,
  `faction` int(10) unsigned NOT NULL DEFAULT '0',
  `minhealth` int(10) unsigned NOT NULL,
  `maxhealth` int(10) unsigned NOT NULL,
  `mana` int(10) unsigned NOT NULL DEFAULT '0',
  `scale` float NOT NULL DEFAULT '0',
  `npcflags` int(10) unsigned NOT NULL DEFAULT '0',
  `attacktime` int(10) unsigned NOT NULL DEFAULT '0',
  `mindamage` float NOT NULL DEFAULT '0',
  `maxdamage` float NOT NULL DEFAULT '0',
  `rangedattacktime` int(10) unsigned NOT NULL DEFAULT '0',
  `rangedmindamage` float unsigned NOT NULL DEFAULT '0',
  `rangedmaxdamage` float unsigned NOT NULL DEFAULT '0',
  `mountdisplayid` int(10) unsigned NOT NULL DEFAULT '0',
  `item1slotdisplay` int(10) unsigned NOT NULL DEFAULT '0',
  `item1info1` int(10) unsigned NOT NULL DEFAULT '0',
  `item1info2` int(10) unsigned NOT NULL DEFAULT '0',
  `item2slotdisplay` int(10) unsigned NOT NULL DEFAULT '0',
  `item2info1` int(10) unsigned NOT NULL DEFAULT '0',
  `item2info2` int(10) unsigned NOT NULL DEFAULT '0',
  `item3slotdisplay` int(10) unsigned NOT NULL DEFAULT '0',
  `item3info1` int(10) unsigned NOT NULL DEFAULT '0',
  `item3info2` int(10) unsigned NOT NULL DEFAULT '0',
  `respawntime` int(10) unsigned NOT NULL DEFAULT '0',
  `resistance0_armor` int(10) unsigned NOT NULL DEFAULT '0',
  `resistance1` int(10) unsigned NOT NULL DEFAULT '0',
  `resistance2` int(10) unsigned NOT NULL DEFAULT '0',
  `resistance3` int(10) unsigned NOT NULL DEFAULT '0',
  `resistance4` int(10) unsigned NOT NULL DEFAULT '0',
  `resistance5` int(10) unsigned NOT NULL DEFAULT '0',
  `resistance6` int(10) unsigned NOT NULL DEFAULT '0',
  `combat_reach` float NOT NULL DEFAULT '0',
  `bounding_radius` float NOT NULL DEFAULT '0',
  `auras` longtext COLLATE utf8_unicode_ci NOT NULL,
  `boss` int(10) unsigned NOT NULL DEFAULT '0',
  `money` int(11) NOT NULL DEFAULT '0',
  `invisibility_type` int(10) unsigned NOT NULL,
  `death_state` int(10) unsigned NOT NULL,
  `walk_speed` float NOT NULL DEFAULT '2.5',
  `run_speed` float NOT NULL DEFAULT '8',
  `fly_speed` float NOT NULL DEFAULT '14',
  `extra_a9_flags` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`),
  UNIQUE KEY `ID` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creature_quest_finisher` (
  `id` int(10) unsigned NOT NULL DEFAULT '0',
  `quest` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creature_quest_starter` (
  `id` int(10) unsigned NOT NULL DEFAULT '0',
  `quest` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creature_spawns` (
  `id` int(10) unsigned NOT NULL,
  `entry` int(11) NOT NULL,
  `Map` int(11) NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `o` float NOT NULL,
  `movetype` int(11) NOT NULL DEFAULT '0',
  `displayid` int(10) unsigned NOT NULL DEFAULT '0',
  `factionid` int(11) NOT NULL DEFAULT '14',
  `flags` int(11) NOT NULL DEFAULT '0',
  `bytes` int(11) NOT NULL DEFAULT '0',
  `bytes2` int(11) NOT NULL DEFAULT '0',
  `emote_state` int(11) NOT NULL DEFAULT '0',
  `npc_respawn_link` int(11) NOT NULL DEFAULT '0',
  `channel_spell` int(11) NOT NULL DEFAULT '0',
  `channel_target_sqlid` int(11) NOT NULL DEFAULT '0',
  `channel_target_sqlid_creature` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creature_staticspawns` (
  `id` int(10) unsigned NOT NULL,
  `entry` int(11) NOT NULL,
  `Map` int(11) NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `o` float NOT NULL,
  `movetype` int(11) NOT NULL DEFAULT '0',
  `displayid` int(10) unsigned NOT NULL DEFAULT '0',
  `factionid` int(11) NOT NULL DEFAULT '35',
  `flags` int(11) NOT NULL DEFAULT '0',
  `bytes` int(11) NOT NULL DEFAULT '0',
  `bytes2` int(11) NOT NULL DEFAULT '0',
  `emote_state` int(11) NOT NULL DEFAULT '0',
  `npc_respawn_link` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creature_waypoints` (
  `creatureid` int(10) unsigned NOT NULL DEFAULT '0',
  `waypointid` int(10) unsigned NOT NULL DEFAULT '0',
  `x` float NOT NULL DEFAULT '0',
  `y` float NOT NULL DEFAULT '0',
  `z` float NOT NULL DEFAULT '0',
  `waittime` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `forwardemoteoneshot` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `forwardemoteid` int(10) unsigned NOT NULL DEFAULT '0',
  `backwardemoteoneshot` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `backwardemoteid` int(10) unsigned NOT NULL DEFAULT '0',
  `forwardskinid` int(10) unsigned NOT NULL DEFAULT '0',
  `backwardskinid` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`creatureid`,`waypointid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `creatureloot` (
  `index` int(10) unsigned NOT NULL,
  `entryid` int(11) NOT NULL DEFAULT '0',
  `itemid` int(11) NOT NULL DEFAULT '0',
  `percentchance` float NOT NULL DEFAULT '0',
  `heroicpercentchance` float NOT NULL DEFAULT '0',
  `mincount` int(11) NOT NULL DEFAULT '1',
  `maxcount` int(11) NOT NULL DEFAULT '1',
  PRIMARY KEY (`entryid`,`itemid`,`percentchance`,`heroicpercentchance`,`mincount`,`maxcount`),
  UNIQUE KEY `index` (`index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `fishing` (
  `Zone` int(10) unsigned NOT NULL DEFAULT '0',
  `MinSkill` int(10) unsigned DEFAULT NULL,
  `MaxSkill` int(10) unsigned DEFAULT NULL,
  UNIQUE KEY `Zone` (`Zone`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `fishingloot` (
  `index` int(10) unsigned NOT NULL,
  `entryid` int(10) unsigned NOT NULL DEFAULT '0',
  `itemid` int(10) unsigned NOT NULL DEFAULT '0',
  `percentchance` float NOT NULL DEFAULT '0',
  `heroicpercentchance` float NOT NULL DEFAULT '0',
  `mincount` int(10) unsigned NOT NULL DEFAULT '1',
  `maxcount` int(10) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`index`),
  UNIQUE KEY `index` (`index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `gameobject_names` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `Type` int(10) unsigned NOT NULL DEFAULT '0',
  `DisplayID` int(10) unsigned NOT NULL DEFAULT '0',
  `Name` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `spellfocus` int(10) unsigned NOT NULL DEFAULT '0',
  `sound1` int(10) unsigned NOT NULL DEFAULT '0',
  `sound2` int(10) unsigned NOT NULL DEFAULT '0',
  `sound3` int(10) unsigned NOT NULL DEFAULT '0',
  `sound4` int(10) unsigned NOT NULL DEFAULT '0',
  `sound5` int(10) unsigned NOT NULL DEFAULT '0',
  `sound6` int(10) unsigned NOT NULL DEFAULT '0',
  `sound7` int(10) unsigned NOT NULL DEFAULT '0',
  `sound8` int(10) unsigned NOT NULL DEFAULT '0',
  `sound9` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown1` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown2` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown3` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown4` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown5` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown6` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown7` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown8` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown9` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown10` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown11` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown12` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown13` int(10) unsigned NOT NULL DEFAULT '0',
  `unknown14` int(10) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `gameobject_names_localized` (
  `entry` int(30) NOT NULL,
  `language_code` varchar(5) COLLATE utf8_unicode_ci NOT NULL,
  `name` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`entry`,`language_code`),
  KEY `lol` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `gameobject_quest_finisher` (
  `id` int(10) unsigned NOT NULL DEFAULT '0',
  `quest` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `gameobject_quest_item_binding` (
  `gameobject_entry` int(11) NOT NULL DEFAULT '0',
  `quest_id` int(11) NOT NULL DEFAULT '0',
  `item_id` int(11) NOT NULL DEFAULT '0',
  `item_count` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `gameobject_quest_pickup_binding` (
  `gameobject_entry` int(11) NOT NULL DEFAULT '0',
  `quest_id` int(11) NOT NULL DEFAULT '0',
  `required_count` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `gameobject_quest_starter` (
  `id` int(10) unsigned NOT NULL DEFAULT '0',
  `quest` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `gameobject_spawns` (
  `id` int(10) unsigned NOT NULL,
  `Entry` int(10) unsigned NOT NULL DEFAULT '0',
  `Map` int(10) unsigned NOT NULL DEFAULT '0',
  `x` float NOT NULL DEFAULT '0',
  `y` float NOT NULL DEFAULT '0',
  `z` float NOT NULL DEFAULT '0',
  `Facing` float NOT NULL DEFAULT '0',
  `o1` float NOT NULL DEFAULT '0',
  `o2` float NOT NULL DEFAULT '0',
  `o3` float NOT NULL DEFAULT '0',
  `o4` float NOT NULL DEFAULT '0',
  `State` int(10) unsigned NOT NULL DEFAULT '0',
  `Flags` int(10) unsigned NOT NULL DEFAULT '0',
  `Faction` int(10) unsigned NOT NULL DEFAULT '0',
  `Scale` float NOT NULL DEFAULT '0',
  `stateNpcLink` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `Map` (`Map`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `gameobject_staticspawns` (
  `id` int(11) NOT NULL,
  `entry` int(11) NOT NULL,
  `map` int(11) NOT NULL DEFAULT '0',
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `facing` float NOT NULL,
  `o` float NOT NULL,
  `o1` float NOT NULL,
  `o2` float NOT NULL,
  `o3` float NOT NULL,
  `state` int(11) NOT NULL DEFAULT '0',
  `flags` int(11) NOT NULL DEFAULT '0',
  `faction` int(11) NOT NULL DEFAULT '0',
  `scale` float NOT NULL,
  `respawnNpcLink` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `graveyards` (
  `id` int(10) unsigned NOT NULL,
  `x` float NOT NULL DEFAULT '0',
  `y` float NOT NULL DEFAULT '0',
  `z` float NOT NULL DEFAULT '0',
  `o` float NOT NULL DEFAULT '0',
  `zoneid` int(10) unsigned NOT NULL DEFAULT '0',
  `adjacentzoneid` int(10) unsigned NOT NULL DEFAULT '0',
  `mapid` int(10) unsigned NOT NULL DEFAULT '0',
  `factionid` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `index` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `item_randomprop_groups` (
  `entry_id` int(11) NOT NULL,
  `randomprops_entryid` int(11) NOT NULL,
  `chance` float NOT NULL,
  PRIMARY KEY (`entry_id`,`randomprops_entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `item_randomsuffix_groups` (
  `entry_id` int(11) NOT NULL,
  `randomsuffix_entryid` int(11) NOT NULL,
  `chance` float NOT NULL,
  PRIMARY KEY (`entry_id`,`randomsuffix_entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `itemloot` (
  `index` int(10) unsigned NOT NULL,
  `entryid` int(10) unsigned NOT NULL DEFAULT '0',
  `itemid` int(10) unsigned NOT NULL DEFAULT '25',
  `percentchance` float NOT NULL DEFAULT '0',
  `heroicpercentchance` float NOT NULL DEFAULT '0',
  `mincount` int(10) unsigned NOT NULL DEFAULT '1',
  `maxcount` int(10) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`index`),
  UNIQUE KEY `index` (`index`),
  KEY `i_gameobj_loot_entry` (`entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `itempages` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `text` longtext COLLATE utf8_unicode_ci NOT NULL,
  `next_page` int(10) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `itempages_localized` (
  `entry` int(30) NOT NULL,
  `language_code` varchar(5) COLLATE utf8_unicode_ci NOT NULL,
  `text` text COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`entry`,`language_code`),
  KEY `a` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `items` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `class` int(11) NOT NULL DEFAULT '0',
  `subclass` int(11) NOT NULL DEFAULT '0',
  `field4` int(11) NOT NULL DEFAULT '-1',
  `name1` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `name2` varchar(255) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `name3` varchar(255) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `name4` varchar(255) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `displayid` int(10) unsigned NOT NULL DEFAULT '0',
  `quality` int(11) NOT NULL DEFAULT '0',
  `flags` int(11) NOT NULL DEFAULT '0',
  `buyprice` int(11) NOT NULL DEFAULT '0',
  `sellprice` int(11) NOT NULL DEFAULT '0',
  `inventorytype` int(11) NOT NULL DEFAULT '0',
  `allowableclass` int(11) NOT NULL DEFAULT '0',
  `allowablerace` int(11) NOT NULL DEFAULT '0',
  `itemlevel` int(11) NOT NULL DEFAULT '0',
  `requiredlevel` int(11) NOT NULL DEFAULT '0',
  `RequiredSkill` int(11) NOT NULL DEFAULT '0',
  `RequiredSkillRank` int(11) NOT NULL DEFAULT '0',
  `RequiredSkillSubRank` int(11) NOT NULL DEFAULT '0',
  `RequiredPlayerRank1` int(11) NOT NULL DEFAULT '0',
  `RequiredPlayerRank2` int(11) NOT NULL DEFAULT '0',
  `RequiredFaction` int(11) NOT NULL DEFAULT '0',
  `RequiredFactionStanding` int(11) NOT NULL DEFAULT '0',
  `Unique` int(11) NOT NULL DEFAULT '0',
  `maxcount` int(11) NOT NULL DEFAULT '0',
  `ContainerSlots` int(11) NOT NULL DEFAULT '0',
  `stat_type1` int(11) NOT NULL DEFAULT '0',
  `stat_value1` int(11) NOT NULL DEFAULT '0',
  `stat_type2` int(11) NOT NULL DEFAULT '0',
  `stat_value2` int(11) NOT NULL DEFAULT '0',
  `stat_type3` int(11) NOT NULL DEFAULT '0',
  `stat_value3` int(11) NOT NULL DEFAULT '0',
  `stat_type4` int(11) NOT NULL DEFAULT '0',
  `stat_value4` int(11) NOT NULL DEFAULT '0',
  `stat_type5` int(11) NOT NULL DEFAULT '0',
  `stat_value5` int(11) NOT NULL DEFAULT '0',
  `stat_type6` int(11) NOT NULL DEFAULT '0',
  `stat_value6` int(11) NOT NULL DEFAULT '0',
  `stat_type7` int(11) NOT NULL DEFAULT '0',
  `stat_value7` int(11) NOT NULL DEFAULT '0',
  `stat_type8` int(11) NOT NULL DEFAULT '0',
  `stat_value8` int(11) NOT NULL DEFAULT '0',
  `stat_type9` int(11) NOT NULL DEFAULT '0',
  `stat_value9` int(11) NOT NULL DEFAULT '0',
  `stat_type10` int(11) NOT NULL DEFAULT '0',
  `stat_value10` int(11) NOT NULL DEFAULT '0',
  `dmg_min1` float NOT NULL DEFAULT '0',
  `dmg_max1` float NOT NULL DEFAULT '0',
  `dmg_type1` int(11) NOT NULL DEFAULT '0',
  `dmg_min2` float NOT NULL DEFAULT '0',
  `dmg_max2` float NOT NULL DEFAULT '0',
  `dmg_type2` int(11) NOT NULL DEFAULT '0',
  `dmg_min3` float NOT NULL DEFAULT '0',
  `dmg_max3` float NOT NULL DEFAULT '0',
  `dmg_type3` int(11) NOT NULL DEFAULT '0',
  `dmg_min4` float NOT NULL DEFAULT '0',
  `dmg_max4` float NOT NULL DEFAULT '0',
  `dmg_type4` int(11) NOT NULL DEFAULT '0',
  `dmg_min5` float NOT NULL DEFAULT '0',
  `dmg_max5` float NOT NULL DEFAULT '0',
  `dmg_type5` int(11) NOT NULL DEFAULT '0',
  `armor` int(11) NOT NULL DEFAULT '0',
  `holy_res` int(11) NOT NULL DEFAULT '0',
  `fire_res` int(11) NOT NULL DEFAULT '0',
  `nature_res` int(11) NOT NULL DEFAULT '0',
  `frost_res` int(11) NOT NULL DEFAULT '0',
  `shadow_res` int(11) NOT NULL DEFAULT '0',
  `arcane_res` int(11) NOT NULL DEFAULT '0',
  `delay` int(11) NOT NULL DEFAULT '0',
  `ammo_type` int(11) NOT NULL DEFAULT '0',
  `range` float NOT NULL DEFAULT '0',
  `spellid_1` int(11) NOT NULL DEFAULT '0',
  `spelltrigger_1` int(11) NOT NULL DEFAULT '0',
  `spellcharges_1` int(11) NOT NULL DEFAULT '0',
  `spellcooldown_1` int(11) NOT NULL DEFAULT '0',
  `spellcategory_1` int(11) NOT NULL DEFAULT '0',
  `spellcategorycooldown_1` int(11) NOT NULL DEFAULT '0',
  `spellid_2` int(11) NOT NULL DEFAULT '0',
  `spelltrigger_2` int(11) NOT NULL DEFAULT '0',
  `spellcharges_2` int(11) NOT NULL DEFAULT '0',
  `spellcooldown_2` int(11) NOT NULL DEFAULT '0',
  `spellcategory_2` int(11) NOT NULL DEFAULT '0',
  `spellcategorycooldown_2` int(11) NOT NULL DEFAULT '0',
  `spellid_3` int(11) NOT NULL DEFAULT '0',
  `spelltrigger_3` int(11) NOT NULL DEFAULT '0',
  `spellcharges_3` int(11) NOT NULL DEFAULT '0',
  `spellcooldown_3` int(11) NOT NULL DEFAULT '0',
  `spellcategory_3` int(11) NOT NULL DEFAULT '0',
  `spellcategorycooldown_3` int(11) NOT NULL DEFAULT '0',
  `spellid_4` int(11) NOT NULL DEFAULT '0',
  `spelltrigger_4` int(11) NOT NULL DEFAULT '0',
  `spellcharges_4` int(11) NOT NULL DEFAULT '0',
  `spellcooldown_4` int(11) NOT NULL DEFAULT '0',
  `spellcategory_4` int(11) NOT NULL DEFAULT '0',
  `spellcategorycooldown_4` int(11) NOT NULL DEFAULT '0',
  `spellid_5` int(11) NOT NULL DEFAULT '0',
  `spelltrigger_5` int(11) NOT NULL DEFAULT '0',
  `spellcharges_5` int(11) NOT NULL DEFAULT '0',
  `spellcooldown_5` int(11) NOT NULL DEFAULT '0',
  `spellcategory_5` int(11) NOT NULL DEFAULT '0',
  `spellcategorycooldown_5` int(11) NOT NULL DEFAULT '0',
  `bonding` int(11) NOT NULL DEFAULT '0',
  `description` varchar(255) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `page_id` int(11) NOT NULL DEFAULT '0',
  `page_language` int(11) NOT NULL DEFAULT '0',
  `page_material` int(11) NOT NULL DEFAULT '0',
  `quest_id` int(11) NOT NULL DEFAULT '0',
  `lock_id` int(11) NOT NULL DEFAULT '0',
  `lock_material` int(11) NOT NULL DEFAULT '0',
  `sheathID` int(11) NOT NULL DEFAULT '0',
  `randomprop` int(11) NOT NULL DEFAULT '0',
  `unk203_1` int(11) NOT NULL DEFAULT '0',
  `block` int(11) NOT NULL DEFAULT '0',
  `itemset` int(11) NOT NULL DEFAULT '0',
  `MaxDurability` int(11) NOT NULL DEFAULT '0',
  `ZoneNameID` int(11) NOT NULL DEFAULT '0',
  `mapid` int(11) DEFAULT NULL,
  `bagfamily` int(11) NOT NULL DEFAULT '0',
  `TotemCategory` int(11) DEFAULT NULL,
  `socket_color_1` int(11) DEFAULT NULL,
  `unk201_3` int(11) NOT NULL DEFAULT '0',
  `socket_color_2` int(11) DEFAULT NULL,
  `unk201_5` int(11) NOT NULL DEFAULT '0',
  `socket_color_3` int(11) DEFAULT NULL,
  `unk201_7` int(11) NOT NULL DEFAULT '0',
  `socket_bonus` int(11) DEFAULT NULL,
  `GemProperties` int(11) DEFAULT NULL,
  `ItemExtendedCost` int(11) DEFAULT NULL,
  `ReqDisenchantSkill` int(11) NOT NULL DEFAULT '-1',
  `unk2` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`),
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `items_localized` (
  `entry` int(30) NOT NULL,
  `language_code` varchar(5) COLLATE utf8_unicode_ci NOT NULL,
  `name` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `description` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`entry`,`language_code`),
  KEY `lol` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `npc_gossip_textid` (
  `entryid` int(10) unsigned NOT NULL DEFAULT '0',
  `textid` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`entryid`),
  UNIQUE KEY `index` (`entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `npc_monstersay` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `event` int(10) unsigned NOT NULL DEFAULT '0',
  `chance` float NOT NULL DEFAULT '0',
  `language` int(10) unsigned NOT NULL DEFAULT '0',
  `type` int(10) unsigned NOT NULL DEFAULT '0',
  `monstername` longtext COLLATE utf8_unicode_ci,
  `text0` longtext COLLATE utf8_unicode_ci,
  `text1` longtext COLLATE utf8_unicode_ci,
  `text2` longtext COLLATE utf8_unicode_ci,
  `text3` longtext COLLATE utf8_unicode_ci,
  `text4` longtext COLLATE utf8_unicode_ci
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `npc_text` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `prob0` float NOT NULL DEFAULT '0',
  `text0_0` longtext COLLATE utf8_unicode_ci NOT NULL,
  `text0_1` longtext COLLATE utf8_unicode_ci NOT NULL,
  `lang0` int(10) unsigned NOT NULL DEFAULT '0',
  `em0_0` int(10) unsigned NOT NULL DEFAULT '0',
  `em0_1` int(10) unsigned NOT NULL DEFAULT '0',
  `em0_2` int(10) unsigned NOT NULL DEFAULT '0',
  `em0_3` int(10) unsigned NOT NULL DEFAULT '0',
  `em0_4` int(10) unsigned NOT NULL DEFAULT '0',
  `em0_5` int(10) unsigned NOT NULL DEFAULT '0',
  `prob1` float NOT NULL DEFAULT '0',
  `text1_0` longtext COLLATE utf8_unicode_ci NOT NULL,
  `text1_1` longtext COLLATE utf8_unicode_ci NOT NULL,
  `lang1` int(10) unsigned NOT NULL DEFAULT '0',
  `em1_0` int(10) unsigned NOT NULL DEFAULT '0',
  `em1_1` int(10) unsigned NOT NULL DEFAULT '0',
  `em1_2` int(10) unsigned NOT NULL DEFAULT '0',
  `em1_3` int(10) unsigned NOT NULL DEFAULT '0',
  `em1_4` int(10) unsigned NOT NULL DEFAULT '0',
  `em1_5` int(10) unsigned NOT NULL DEFAULT '0',
  `prob2` float NOT NULL DEFAULT '0',
  `text2_0` longtext COLLATE utf8_unicode_ci NOT NULL,
  `text2_1` longtext COLLATE utf8_unicode_ci NOT NULL,
  `lang2` int(10) unsigned NOT NULL DEFAULT '0',
  `em2_0` int(10) unsigned NOT NULL DEFAULT '0',
  `em2_1` int(10) unsigned NOT NULL DEFAULT '0',
  `em2_2` int(10) unsigned NOT NULL DEFAULT '0',
  `em2_3` int(10) unsigned NOT NULL DEFAULT '0',
  `em2_4` int(10) unsigned NOT NULL DEFAULT '0',
  `em2_5` int(10) unsigned NOT NULL DEFAULT '0',
  `prob3` float NOT NULL DEFAULT '0',
  `text3_0` longtext COLLATE utf8_unicode_ci NOT NULL,
  `text3_1` longtext COLLATE utf8_unicode_ci NOT NULL,
  `lang3` int(10) unsigned NOT NULL DEFAULT '0',
  `em3_0` int(10) unsigned NOT NULL DEFAULT '0',
  `em3_1` int(10) unsigned NOT NULL DEFAULT '0',
  `em3_2` int(10) unsigned NOT NULL DEFAULT '0',
  `em3_3` int(10) unsigned NOT NULL DEFAULT '0',
  `em3_4` int(10) unsigned NOT NULL DEFAULT '0',
  `em3_5` int(10) unsigned NOT NULL DEFAULT '0',
  `prob4` float NOT NULL DEFAULT '0',
  `text4_0` longtext COLLATE utf8_unicode_ci NOT NULL,
  `text4_1` longtext COLLATE utf8_unicode_ci NOT NULL,
  `lang4` int(10) unsigned NOT NULL DEFAULT '0',
  `em4_0` int(10) unsigned NOT NULL DEFAULT '0',
  `em4_1` int(10) unsigned NOT NULL DEFAULT '0',
  `em4_2` int(10) unsigned NOT NULL DEFAULT '0',
  `em4_3` int(10) unsigned NOT NULL DEFAULT '0',
  `em4_4` int(10) unsigned NOT NULL DEFAULT '0',
  `em4_5` int(10) unsigned NOT NULL DEFAULT '0',
  `prob5` float NOT NULL DEFAULT '0',
  `text5_0` longtext COLLATE utf8_unicode_ci NOT NULL,
  `text5_1` longtext COLLATE utf8_unicode_ci NOT NULL,
  `lang5` int(10) unsigned NOT NULL DEFAULT '0',
  `em5_0` int(10) unsigned NOT NULL DEFAULT '0',
  `em5_1` int(10) unsigned NOT NULL DEFAULT '0',
  `em5_2` int(10) unsigned NOT NULL DEFAULT '0',
  `em5_3` int(10) unsigned NOT NULL DEFAULT '0',
  `em5_4` int(10) unsigned NOT NULL DEFAULT '0',
  `em5_5` int(10) unsigned NOT NULL DEFAULT '0',
  `prob6` float NOT NULL DEFAULT '0',
  `text6_0` longtext COLLATE utf8_unicode_ci NOT NULL,
  `text6_1` longtext COLLATE utf8_unicode_ci NOT NULL,
  `lang6` int(10) unsigned NOT NULL DEFAULT '0',
  `em6_0` int(10) unsigned NOT NULL DEFAULT '0',
  `em6_1` int(10) unsigned NOT NULL DEFAULT '0',
  `em6_2` int(10) unsigned NOT NULL DEFAULT '0',
  `em6_3` int(10) unsigned NOT NULL DEFAULT '0',
  `em6_4` int(10) unsigned NOT NULL DEFAULT '0',
  `em6_5` int(10) unsigned NOT NULL DEFAULT '0',
  `prob7` float NOT NULL DEFAULT '0',
  `text7_0` longtext COLLATE utf8_unicode_ci NOT NULL,
  `text7_1` longtext COLLATE utf8_unicode_ci NOT NULL,
  `lang7` int(10) unsigned NOT NULL DEFAULT '0',
  `em7_0` int(10) unsigned NOT NULL DEFAULT '0',
  `em7_1` int(10) unsigned NOT NULL DEFAULT '0',
  `em7_2` int(10) unsigned NOT NULL DEFAULT '0',
  `em7_3` int(10) unsigned NOT NULL DEFAULT '0',
  `em7_4` int(10) unsigned NOT NULL DEFAULT '0',
  `em7_5` int(10) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `npc_text_localized` (
  `entry` int(30) NOT NULL,
  `language_code` varchar(5) COLLATE utf8_unicode_ci NOT NULL,
  `text0` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text0_1` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text1` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text1_1` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text2` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text2_1` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text3` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text3_1` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text4` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text4_1` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text5` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text5_1` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text6` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text6_1` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text7` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `text7_1` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`entry`,`language_code`),
  KEY `lol` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `objectloot` (
  `index` int(10) unsigned NOT NULL,
  `entryid` int(10) unsigned NOT NULL DEFAULT '0',
  `itemid` int(10) unsigned NOT NULL DEFAULT '0',
  `percentchance` float NOT NULL DEFAULT '0',
  `heroicpercentchance` float NOT NULL DEFAULT '0',
  `mincount` int(10) unsigned NOT NULL DEFAULT '1',
  `maxcount` int(10) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`index`),
  UNIQUE KEY `index` (`index`),
  KEY `entryid` (`entryid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `petdefaultspells` (
  `entry` int(11) NOT NULL DEFAULT '0',
  `spell` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `pickpocketingloot` (
  `index` int(10) unsigned NOT NULL,
  `entryid` int(10) unsigned NOT NULL DEFAULT '0',
  `itemid` int(10) unsigned NOT NULL DEFAULT '25',
  `percentchance` float NOT NULL DEFAULT '100',
  `heroicpercentchance` float DEFAULT '0',
  `mincount` int(11) DEFAULT '1',
  `maxcount` int(11) DEFAULT '1',
  PRIMARY KEY (`index`),
  UNIQUE KEY `index` (`index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `playercreateinfo` (
  `Index` tinyint(3) unsigned NOT NULL,
  `race` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `factiontemplate` int(10) unsigned NOT NULL DEFAULT '0',
  `class` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `mapID` int(10) unsigned NOT NULL DEFAULT '0',
  `zoneID` int(10) unsigned NOT NULL DEFAULT '0',
  `positionX` float NOT NULL DEFAULT '0',
  `positionY` float NOT NULL DEFAULT '0',
  `positionZ` float NOT NULL DEFAULT '0',
  `displayID` smallint(5) unsigned NOT NULL DEFAULT '0',
  `BaseStrength` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BaseAgility` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BaseStamina` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BaseIntellect` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BaseSpirit` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BaseHealth` int(10) unsigned NOT NULL DEFAULT '0',
  `BaseMana` int(10) unsigned NOT NULL DEFAULT '0',
  `BaseRage` int(10) unsigned NOT NULL DEFAULT '0',
  `BaseFocus` int(10) unsigned NOT NULL DEFAULT '0',
  `BaseEnergy` int(10) unsigned NOT NULL DEFAULT '0',
  `attackpower` int(10) unsigned NOT NULL DEFAULT '0',
  `mindmg` float NOT NULL DEFAULT '0',
  `maxdmg` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `playercreateinfo_bars` (
  `race` tinyint(3) unsigned DEFAULT NULL,
  `class` tinyint(3) unsigned DEFAULT NULL,
  `button` int(10) unsigned DEFAULT NULL,
  `action` int(10) unsigned DEFAULT NULL,
  `type` int(10) unsigned DEFAULT NULL,
  `misc` int(10) unsigned DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `playercreateinfo_items` (
  `indexid` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `protoid` int(10) unsigned NOT NULL DEFAULT '0',
  `slotid` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `amount` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `playercreateinfo_skills` (
  `indexid` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `skillid` int(10) unsigned NOT NULL DEFAULT '0',
  `level` int(10) unsigned NOT NULL DEFAULT '0',
  `maxlevel` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `playercreateinfo_spells` (
  `indexid` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `spellid` smallint(5) unsigned NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `prospectingloot` (
  `index` int(10) unsigned NOT NULL,
  `entryid` int(10) unsigned NOT NULL DEFAULT '0',
  `itemid` int(10) unsigned NOT NULL DEFAULT '25',
  `percentchance` float NOT NULL DEFAULT '0',
  `heroicpercentchance` float NOT NULL DEFAULT '0',
  `mincount` int(10) unsigned NOT NULL DEFAULT '1',
  `maxcount` int(10) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`index`),
  UNIQUE KEY `index` (`index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `quests` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `ZoneId` int(10) unsigned NOT NULL DEFAULT '0',
  `QuestSort` int(10) unsigned NOT NULL DEFAULT '0',
  `QuestFlags` int(10) unsigned NOT NULL DEFAULT '0',
  `MinLevel` int(10) unsigned NOT NULL DEFAULT '0',
  `MaxLevel` int(10) unsigned NOT NULL DEFAULT '0',
  `Type` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredRaces` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredClass` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredTradeskill` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredRepFaction` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredRepValue` int(10) unsigned NOT NULL DEFAULT '0',
  `LimitTime` int(10) unsigned NOT NULL DEFAULT '0',
  `SpecialFlags` int(10) unsigned NOT NULL DEFAULT '0',
  `PrevQuestId` int(10) unsigned NOT NULL DEFAULT '0',
  `NextQuestId` int(10) unsigned NOT NULL DEFAULT '0',
  `srcItem` int(10) unsigned NOT NULL DEFAULT '0',
  `SrcItemCount` int(10) unsigned NOT NULL DEFAULT '0',
  `Title` char(255) COLLATE utf8_unicode_ci NOT NULL,
  `Details` text COLLATE utf8_unicode_ci NOT NULL,
  `Objectives` text COLLATE utf8_unicode_ci NOT NULL,
  `CompletionText` text COLLATE utf8_unicode_ci NOT NULL,
  `IncompleteText` text COLLATE utf8_unicode_ci NOT NULL,
  `EndText` text COLLATE utf8_unicode_ci NOT NULL,
  `ObjectiveText1` text COLLATE utf8_unicode_ci NOT NULL,
  `ObjectiveText2` text COLLATE utf8_unicode_ci NOT NULL,
  `ObjectiveText3` text COLLATE utf8_unicode_ci NOT NULL,
  `ObjectiveText4` text COLLATE utf8_unicode_ci NOT NULL,
  `ReqItemId1` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqItemId2` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqItemId3` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqItemId4` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqItemCount1` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqItemCount2` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqItemCount3` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqItemCount4` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqKillMobOrGOId1` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqKillMobOrGOId2` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqKillMobOrGOId3` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqKillMobOrGOId4` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqKillMobOrGOCount1` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqKillMobOrGOCount2` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqKillMobOrGOCount3` int(10) unsigned NOT NULL DEFAULT '0',
  `ReqKillMobOrGOCount4` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemId1` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemId2` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemId3` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemId4` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemId5` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemId6` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemCount1` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemCount2` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemCount3` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemCount4` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemCount5` int(10) unsigned NOT NULL DEFAULT '0',
  `RewChoiceItemCount6` int(10) unsigned NOT NULL DEFAULT '0',
  `RewItemId1` int(10) unsigned NOT NULL DEFAULT '0',
  `RewItemId2` int(10) unsigned NOT NULL DEFAULT '0',
  `RewItemId3` int(10) unsigned NOT NULL DEFAULT '0',
  `RewItemId4` int(10) unsigned NOT NULL DEFAULT '0',
  `RewItemCount1` int(10) unsigned NOT NULL DEFAULT '0',
  `RewItemCount2` int(10) unsigned NOT NULL DEFAULT '0',
  `RewItemCount3` int(10) unsigned NOT NULL DEFAULT '0',
  `RewItemCount4` int(10) unsigned NOT NULL DEFAULT '0',
  `RewRepFaction1` int(10) unsigned NOT NULL DEFAULT '0',
  `RewRepFaction2` int(10) unsigned NOT NULL DEFAULT '0',
  `RewRepValue1` int(11) NOT NULL DEFAULT '0',
  `RewRepValue2` int(11) NOT NULL DEFAULT '0',
  `RewRepLimit` int(10) unsigned NOT NULL DEFAULT '0',
  `RewMoney` int(10) unsigned NOT NULL DEFAULT '0',
  `RewXP` int(10) unsigned NOT NULL DEFAULT '0',
  `RewSpell` int(10) unsigned NOT NULL DEFAULT '0',
  `CastSpell` int(10) unsigned NOT NULL DEFAULT '0',
  `PointMapId` int(10) unsigned NOT NULL DEFAULT '0',
  `PointX` float NOT NULL DEFAULT '0',
  `PointY` float NOT NULL DEFAULT '0',
  `PointOpt` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredMoney` int(10) unsigned NOT NULL DEFAULT '0',
  `ExploreTrigger1` int(10) unsigned NOT NULL DEFAULT '0',
  `ExploreTrigger2` int(10) unsigned NOT NULL DEFAULT '0',
  `ExploreTrigger3` int(10) unsigned NOT NULL DEFAULT '0',
  `ExploreTrigger4` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredQuest1` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredQuest2` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredQuest3` int(10) unsigned NOT NULL DEFAULT '0',
  `RequiredQuest4` int(10) unsigned NOT NULL DEFAULT '0',
  `ReceiveItemId1` int(10) unsigned NOT NULL DEFAULT '0',
  `ReceiveItemId2` int(10) unsigned NOT NULL DEFAULT '0',
  `ReceiveItemId3` int(10) unsigned NOT NULL DEFAULT '0',
  `ReceiveItemId4` int(10) unsigned NOT NULL DEFAULT '0',
  `ReceiveItemCount1` int(10) unsigned NOT NULL DEFAULT '0',
  `ReceiveItemCount2` int(10) unsigned NOT NULL DEFAULT '0',
  `ReceiveItemCount3` int(10) unsigned NOT NULL DEFAULT '0',
  `ReceiveItemCount4` int(10) unsigned NOT NULL DEFAULT '0',
  `IsRepeatable` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`),
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `quests_localized` (
  `entry` int(30) NOT NULL,
  `language_code` varchar(5) COLLATE utf8_unicode_ci NOT NULL,
  `Title` text COLLATE utf8_unicode_ci NOT NULL,
  `Details` text COLLATE utf8_unicode_ci NOT NULL,
  `Objectives` text COLLATE utf8_unicode_ci NOT NULL,
  `CompletionText` text COLLATE utf8_unicode_ci NOT NULL,
  `IncompleteText` text COLLATE utf8_unicode_ci NOT NULL,
  `EndText` text COLLATE utf8_unicode_ci NOT NULL,
  `ObjectiveText1` text COLLATE utf8_unicode_ci NOT NULL,
  `ObjectiveText2` text COLLATE utf8_unicode_ci NOT NULL,
  `ObjectiveText3` text COLLATE utf8_unicode_ci NOT NULL,
  `ObjectiveText4` text COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`entry`,`language_code`),
  KEY `lol` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `recall` (
  `id` bigint(20) unsigned NOT NULL,
  `locname` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `map` int(10) unsigned NOT NULL DEFAULT '0',
  `positionX` float NOT NULL DEFAULT '0',
  `positionY` float NOT NULL DEFAULT '0',
  `positionZ` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `reputation_creature_onkill` (
  `creature_id` int(11) NOT NULL,
  `faction_change_alliance` int(11) NOT NULL,
  `faction_change_horde` int(11) NOT NULL,
  `change_value` int(11) NOT NULL,
  `rep_limit` int(11) NOT NULL,
  KEY `index` (`creature_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `reputation_faction_onkill` (
  `faction_id` int(11) NOT NULL,
  `change_factionid_alliance` int(11) NOT NULL,
  `change_deltamin_alliance` int(11) NOT NULL,
  `change_deltamax_alliance` int(11) NOT NULL,
  `change_factionid_horde` int(11) NOT NULL,
  `change_deltamin_horde` int(11) NOT NULL,
  `change_deltamax_horde` int(11) NOT NULL,
  KEY `factindex` (`faction_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `reputation_instance_onkill` (
  `mapid` int(11) NOT NULL,
  `mob_rep_reward` int(11) NOT NULL,
  `mob_rep_limit` int(11) NOT NULL,
  `boss_rep_reward` int(11) NOT NULL,
  `boss_rep_limit` int(11) NOT NULL,
  `faction_change_alliance` int(11) NOT NULL,
  `faction_change_horde` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `skinningloot` (
  `index` int(10) unsigned NOT NULL,
  `entryid` int(10) unsigned NOT NULL DEFAULT '0',
  `itemid` int(10) unsigned NOT NULL DEFAULT '0',
  `percentchance` float NOT NULL DEFAULT '0',
  `heroicpercentchance` float DEFAULT '0',
  `mincount` int(11) DEFAULT '1',
  `maxcount` int(11) DEFAULT '1',
  PRIMARY KEY (`index`),
  UNIQUE KEY `index` (`index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `spell_disable` (
  `spellid` int(11) NOT NULL,
  `replacement_spellid` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `spell_proc_data` (
  `name_hash` int(11) NOT NULL,
  `proc_chance` int(11) NOT NULL,
  `forced_proc_flags` int(11) NOT NULL DEFAULT '-1',
  PRIMARY KEY (`name_hash`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `spellextra` (
  `Id` int(10) unsigned NOT NULL,
  `specialtype` int(10) unsigned DEFAULT NULL,
  `enchantableslots` int(10) unsigned DEFAULT NULL,
  `ExtraFlags` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `spelloverride` (
  `overrideId` int(10) unsigned NOT NULL DEFAULT '0',
  `spellId` int(10) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `overrideId` (`overrideId`,`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `spells112` (
  `Id` int(11) DEFAULT '0',
  `School` int(11) DEFAULT '0',
  `Category` int(11) DEFAULT '0',
  `field4` int(11) DEFAULT '0',
  `DispelType` int(11) DEFAULT '0',
  `MechanicsType` int(11) DEFAULT '0',
  `Attributes` int(11) DEFAULT '0',
  `AttributesEx` int(11) DEFAULT '0',
  `Flags3` int(11) DEFAULT '0',
  `field10` int(11) DEFAULT '0',
  `field11` int(11) DEFAULT '0',
  `UNK12` int(11) DEFAULT '0',
  `UNK13` int(11) DEFAULT '0',
  `Targets` int(11) DEFAULT '0',
  `TargetCreatureType` int(11) DEFAULT '0',
  `RequiresSpellFocus` int(11) DEFAULT '0',
  `CasterAuraState` int(11) DEFAULT '0',
  `TargetAuraState` int(11) DEFAULT '0',
  `CastingTimeIndex` int(11) DEFAULT '0',
  `RecoveryTime` int(11) DEFAULT '0',
  `CategoryRecoveryTime` int(11) DEFAULT '0',
  `InterruptFlags` int(11) DEFAULT '0',
  `AuraInterruptFlags` int(11) DEFAULT '0',
  `ChannelInterruptFlags` int(11) DEFAULT '0',
  `procFlags` int(11) DEFAULT '0',
  `procChance` int(11) DEFAULT '0',
  `procCharges` int(11) DEFAULT '0',
  `maxLevel` int(11) DEFAULT '0',
  `baseLevel` int(11) DEFAULT '0',
  `spellLevel` int(11) DEFAULT '0',
  `DurationIndex` int(11) DEFAULT '0',
  `powerType` int(11) DEFAULT '0',
  `manaCost` int(11) DEFAULT '0',
  `manaCostPerlevel` int(11) DEFAULT '0',
  `manaPerSecond` int(11) DEFAULT '0',
  `manaPerSecondPerLevel` int(11) DEFAULT '0',
  `rangeIndex` int(11) DEFAULT '0',
  `speed` float DEFAULT '0',
  `modalNextSpell` int(11) DEFAULT '0',
  `maxstack` int(11) DEFAULT '0',
  `Totem1` int(11) DEFAULT '0',
  `Totem2` int(11) DEFAULT '0',
  `Reagent1` int(11) DEFAULT '0',
  `Reagent2` int(11) DEFAULT '0',
  `Reagent3` int(11) DEFAULT '0',
  `Reagent4` int(11) DEFAULT '0',
  `Reagent5` int(11) DEFAULT '0',
  `Reagent6` int(11) DEFAULT '0',
  `Reagent7` int(11) DEFAULT '0',
  `Reagent8` int(11) DEFAULT '0',
  `ReagentCount1` int(11) DEFAULT '0',
  `ReagentCount2` int(11) DEFAULT '0',
  `ReagentCount3` int(11) DEFAULT '0',
  `ReagentCount4` int(11) DEFAULT '0',
  `ReagentCount5` int(11) DEFAULT '0',
  `ReagentCount6` int(11) DEFAULT '0',
  `ReagentCount7` int(11) DEFAULT '0',
  `ReagentCount8` int(11) DEFAULT '0',
  `EquippedItemClass` int(11) DEFAULT '0',
  `EquippedItemSubClass` int(11) DEFAULT '0',
  `RequiredItemFlags` int(11) DEFAULT '0',
  `Effect1` int(11) DEFAULT '0',
  `Effect2` int(11) DEFAULT '0',
  `Effect3` int(11) DEFAULT '0',
  `EffectDieSides1` int(11) DEFAULT '0',
  `EffectDieSides2` int(11) DEFAULT '0',
  `EffectDieSides3` int(11) DEFAULT '0',
  `EffectBaseDice1` int(11) DEFAULT '0',
  `EffectBaseDice2` int(11) DEFAULT '0',
  `EffectBaseDice3` int(11) DEFAULT '0',
  `EffectDicePerLevel1` float DEFAULT '0',
  `EffectDicePerLevel2` float DEFAULT '0',
  `EffectDicePerLevel3` float DEFAULT '0',
  `EffectRealPointsPerLevel1` float DEFAULT '0',
  `EffectRealPointsPerLevel2` float DEFAULT '0',
  `EffectRealPointsPerLevel3` float DEFAULT '0',
  `EffectBasePoints1` int(11) DEFAULT '0',
  `EffectBasePoints2` int(11) DEFAULT '0',
  `EffectBasePoints3` int(11) DEFAULT '0',
  `Effectunk1901` int(11) DEFAULT '0',
  `Effectunk1902` int(11) DEFAULT '0',
  `Effectunk1903` int(11) DEFAULT '0',
  `EffectImplicitTargetA1` int(11) DEFAULT '0',
  `EffectImplicitTargetA2` int(11) DEFAULT '0',
  `EffectImplicitTargetA3` int(11) DEFAULT '0',
  `EffectImplicitTargetB1` int(11) DEFAULT '0',
  `EffectImplicitTargetB2` int(11) DEFAULT '0',
  `EffectImplicitTargetB3` int(11) DEFAULT '0',
  `EffectRadiusIndex1` int(11) DEFAULT '0',
  `EffectRadiusIndex2` int(11) DEFAULT '0',
  `EffectRadiusIndex3` int(11) DEFAULT '0',
  `EffectApplyAuraName1` int(11) DEFAULT '0',
  `EffectApplyAuraName2` int(11) DEFAULT '0',
  `EffectApplyAuraName3` int(11) DEFAULT '0',
  `EffectAmplitude1` int(11) DEFAULT '0',
  `EffectAmplitude2` int(11) DEFAULT '0',
  `EffectAmplitude3` int(11) DEFAULT '0',
  `Effectunknown1` float DEFAULT '0',
  `Effectunknown2` float DEFAULT '0',
  `Effectunknown3` float DEFAULT '0',
  `EffectChainTarget1` int(11) DEFAULT '0',
  `EffectChainTarget2` int(11) DEFAULT '0',
  `EffectChainTarget3` int(11) DEFAULT '0',
  `EffectSpellGroupRelation1` int(11) DEFAULT '0',
  `EffectSpellGroupRelation2` int(11) DEFAULT '0',
  `EffectSpellGroupRelation3` int(11) DEFAULT '0',
  `EffectMiscValue1` int(11) DEFAULT '0',
  `EffectMiscValue2` int(11) DEFAULT '0',
  `EffectMiscValue3` int(11) DEFAULT '0',
  `EffectTriggerSpell1` int(11) DEFAULT '0',
  `EffectTriggerSpell2` int(11) DEFAULT '0',
  `EffectTriggerSpell3` int(11) DEFAULT '0',
  `EffectPointsPerComboPoint1` float DEFAULT '0',
  `EffectPointsPerComboPoint2` float DEFAULT '0',
  `EffectPointsPerComboPoint3` float DEFAULT '0',
  `SpellVisual` int(11) DEFAULT '0',
  `field114` int(11) DEFAULT '0',
  `dummy` int(11) DEFAULT '0',
  `CoSpell` int(11) DEFAULT '0',
  `spellPriority` int(11) DEFAULT '0',
  `Name` longtext COLLATE utf8_unicode_ci,
  `NameAlt1` longtext COLLATE utf8_unicode_ci,
  `NameAlt2` longtext COLLATE utf8_unicode_ci,
  `NameAlt3` longtext COLLATE utf8_unicode_ci,
  `NameAlt4` longtext COLLATE utf8_unicode_ci,
  `NameAlt5` longtext COLLATE utf8_unicode_ci,
  `NameAlt6` longtext COLLATE utf8_unicode_ci,
  `NameAlt7` longtext COLLATE utf8_unicode_ci,
  `NameFlags` longtext COLLATE utf8_unicode_ci,
  `Rank` longtext COLLATE utf8_unicode_ci,
  `RankAlt1` longtext COLLATE utf8_unicode_ci,
  `RankAlt2` longtext COLLATE utf8_unicode_ci,
  `RankAlt3` longtext COLLATE utf8_unicode_ci,
  `RankAlt4` longtext COLLATE utf8_unicode_ci,
  `RankAlt5` longtext COLLATE utf8_unicode_ci,
  `RankAlt6` longtext COLLATE utf8_unicode_ci,
  `RankAlt7` longtext COLLATE utf8_unicode_ci,
  `RankFlags` longtext COLLATE utf8_unicode_ci,
  `Description` longtext COLLATE utf8_unicode_ci,
  `DescriptionAlt1` longtext COLLATE utf8_unicode_ci,
  `DescriptionAlt2` longtext COLLATE utf8_unicode_ci,
  `DescriptionAlt3` longtext COLLATE utf8_unicode_ci,
  `DescriptionAlt4` longtext COLLATE utf8_unicode_ci,
  `DescriptionAlt5` longtext COLLATE utf8_unicode_ci,
  `DescriptionAlt6` longtext COLLATE utf8_unicode_ci,
  `DescriptionAlt7` longtext COLLATE utf8_unicode_ci,
  `DescriptionFlags` longtext COLLATE utf8_unicode_ci,
  `BuffDescription` longtext COLLATE utf8_unicode_ci,
  `BuffDescriptionAlt1` longtext COLLATE utf8_unicode_ci,
  `BuffDescriptionAlt2` longtext COLLATE utf8_unicode_ci,
  `BuffDescriptionAlt3` longtext COLLATE utf8_unicode_ci,
  `BuffDescriptionAlt4` longtext COLLATE utf8_unicode_ci,
  `BuffDescriptionAlt5` longtext COLLATE utf8_unicode_ci,
  `BuffDescriptionAlt6` longtext COLLATE utf8_unicode_ci,
  `BuffDescriptionAlt7` longtext COLLATE utf8_unicode_ci,
  `buffdescflags` int(11) DEFAULT '0',
  `ManaCostPercentage` int(11) DEFAULT '0',
  `unkflags` int(11) DEFAULT '0',
  `StartRecoveryTime` int(11) DEFAULT '0',
  `StartRecoveryCategory` int(11) DEFAULT '0',
  `SpellFamilyName` int(11) DEFAULT '0',
  `SpellGroupType` int(11) DEFAULT '0',
  `unkne` int(11) DEFAULT '0',
  `MaxTargets` int(11) DEFAULT '0',
  `Spell_Dmg_Type` int(11) DEFAULT '0',
  `FG` int(11) DEFAULT '0',
  `FH` int(11) DEFAULT '0',
  `dmg_multiplier_1` float DEFAULT '0',
  `dmg_multiplier_2` float DEFAULT '0',
  `dmg_multiplier_3` float DEFAULT '0',
  `FL` int(11) DEFAULT '0',
  `FM` int(11) DEFAULT '0',
  `FN` int(11) DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `teleport_coords` (
  `id` int(10) unsigned NOT NULL DEFAULT '0',
  `name` char(255) COLLATE utf8_unicode_ci NOT NULL,
  `mapId` int(10) unsigned NOT NULL DEFAULT '0',
  `x` float NOT NULL DEFAULT '0',
  `y` float NOT NULL DEFAULT '0',
  `z` float NOT NULL DEFAULT '0',
  `totrigger` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `totemspells` (
  `spellId` int(10) unsigned NOT NULL DEFAULT '0',
  `spellToCast1` int(10) unsigned NOT NULL DEFAULT '0',
  `spellToCast2` int(10) unsigned NOT NULL DEFAULT '0',
  `spellToCast3` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `trainer_defs` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `required_skill` int(10) unsigned NOT NULL DEFAULT '0',
  `required_skillvalue` int(10) unsigned DEFAULT '0',
  `req_class` int(10) unsigned NOT NULL DEFAULT '0',
  `trainer_type` int(10) unsigned NOT NULL DEFAULT '0',
  `trainer_ui_window_message` text CHARACTER SET utf8,
  `can_train_gossip_textid` int(11) NOT NULL,
  `cannot_train_gossip_textid` int(11) NOT NULL,
  PRIMARY KEY (`entry`),
  UNIQUE KEY `entry` (`entry`),
  KEY `entry_2` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `trainer_spells` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `cast_spell` int(10) unsigned NOT NULL DEFAULT '0',
  `spellcost` int(10) unsigned NOT NULL DEFAULT '0',
  `reqspell` int(10) unsigned NOT NULL DEFAULT '0',
  `reqskill` int(10) unsigned NOT NULL DEFAULT '0',
  `reqskillvalue` int(10) unsigned NOT NULL DEFAULT '0',
  `reqlevel` int(10) unsigned NOT NULL DEFAULT '0',
  `deletespell` int(10) unsigned NOT NULL DEFAULT '0',
  `is_prof` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`,`cast_spell`),
  KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `transport_data` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `name` text COLLATE utf8_unicode_ci,
  `period` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `vendors` (
  `vendorGuid` int(10) unsigned NOT NULL DEFAULT '0',
  `itemGuid` int(10) unsigned NOT NULL DEFAULT '0',
  `amount` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `weather` (
  `zoneId` int(10) unsigned NOT NULL DEFAULT '0',
  `high_chance` int(10) unsigned NOT NULL DEFAULT '0',
  `high_type` int(10) unsigned NOT NULL DEFAULT '0',
  `med_chance` int(10) unsigned NOT NULL DEFAULT '0',
  `med_type` int(10) unsigned NOT NULL DEFAULT '0',
  `low_chance` int(10) unsigned NOT NULL DEFAULT '0',
  `low_type` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`zoneId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `worldmap_info` (
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `screenid` int(10) unsigned DEFAULT '0',
  `type` int(10) unsigned DEFAULT '0',
  `maxplayers` int(10) unsigned DEFAULT '0',
  `minlevel` int(10) unsigned DEFAULT '1',
  `repopx` float DEFAULT '0',
  `repopy` float DEFAULT '0',
  `repopz` float DEFAULT '0',
  `repopentry` int(10) unsigned DEFAULT '0',
  `area_name` varchar(100) COLLATE utf8_unicode_ci DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `cooldown` int(10) unsigned NOT NULL DEFAULT '0',
  `lvl_mod_a` int(10) unsigned NOT NULL DEFAULT '0',
  `required_quest` int(10) unsigned NOT NULL DEFAULT '0',
  `required_item` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `zoneguards` (
  `zoneId` int(10) unsigned NOT NULL,
  `hordeEntry` int(10) unsigned DEFAULT NULL,
  `allianceEntry` int(10) unsigned DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
SET character_set_client = @saved_cs_client;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

