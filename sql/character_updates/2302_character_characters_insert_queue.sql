-- MySQL dump 10.11
--
-- Host: localhost    Database: character
-- ------------------------------------------------------
-- Server version	5.0.45-community-nt

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
-- Table structure for table `characters_insert_queue`
--

DROP TABLE IF EXISTS `characters_insert_queue`;
CREATE TABLE `characters_insert_queue` (
  `insert_temp_guid` int(10) unsigned NOT NULL default '0',
  `acct` int(10) unsigned NOT NULL default '0',
  `name` varchar(21) collate utf8_unicode_ci NOT NULL,
  `race` tinyint(3) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `gender` tinyint(3) unsigned NOT NULL default '0',
  `custom_faction` int(30) NOT NULL default '0',
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
  `banned` int(40) NOT NULL,
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
  `arenaPoints` int(10) NOT NULL,
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
  `honorPointsToAdd` int(10) NOT NULL,
  `killsToday` int(10) unsigned NOT NULL default '0',
  `killsYesterday` int(10) unsigned NOT NULL default '0',
  `killsLifeTime` int(10) unsigned NOT NULL default '0',
  `honorToday` int(10) unsigned NOT NULL default '0',
  `honorYesterday` int(10) unsigned NOT NULL default '0',
  `honorPoints` int(10) unsigned NOT NULL default '0',
  `difficulty` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `guid` (`insert_temp_guid`),
  KEY `acct` (`acct`),
  KEY `guildid` (`guildid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Dumping data for table `characters_insert_queue`
--

LOCK TABLES `characters_insert_queue` WRITE;
/*!40000 ALTER TABLE `characters_insert_queue` DISABLE KEYS */;
/*!40000 ALTER TABLE `characters_insert_queue` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2007-11-12 14:45:11
