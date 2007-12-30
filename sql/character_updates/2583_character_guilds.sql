-- MySQL dump 10.13
--
-- Host: localhost    Database: cdb
-- ------------------------------------------------------
-- Server version	5.1.22-rc-community

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
-- Table structure for table `guild_bankitems`
--

DROP TABLE IF EXISTS `guild_bankitems`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `guild_bankitems` (
  `guildId` int(30) NOT NULL,
  `tabId` int(30) NOT NULL,
  `slotId` int(30) NOT NULL,
  `itemGuid` int(30) NOT NULL,
  PRIMARY KEY (`guildId`,`tabId`,`slotId`),
  KEY `a` (`guildId`),
  KEY `b` (`tabId`),
  KEY `c` (`slotId`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `guild_bankitems`
--

LOCK TABLES `guild_bankitems` WRITE;
/*!40000 ALTER TABLE `guild_bankitems` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_bankitems` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_banklogs`
--

DROP TABLE IF EXISTS `guild_banklogs`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `guild_banklogs` (
  `log_id` int(30) NOT NULL,
  `guildid` int(30) NOT NULL,
  `tabid` int(30) NOT NULL COMMENT 'tab 6 is money logs',
  `action` int(5) NOT NULL,
  `player_guid` int(30) NOT NULL,
  `item_entry` int(30) NOT NULL,
  `stack_count` int(30) NOT NULL,
  `timestamp` int(30) NOT NULL,
  PRIMARY KEY (`log_id`,`guildid`),
  KEY `a` (`guildid`),
  KEY `b` (`tabid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `guild_banklogs`
--

LOCK TABLES `guild_banklogs` WRITE;
/*!40000 ALTER TABLE `guild_banklogs` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_banklogs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_banktabs`
--

DROP TABLE IF EXISTS `guild_banktabs`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `guild_banktabs` (
  `guildId` int(30) NOT NULL,
  `tabId` int(30) NOT NULL,
  `tabName` varchar(200) NOT NULL,
  `tabIcon` varchar(200) NOT NULL,
  PRIMARY KEY (`guildId`,`tabId`),
  KEY `a` (`guildId`),
  KEY `b` (`tabId`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `guild_banktabs`
--

LOCK TABLES `guild_banktabs` WRITE;
/*!40000 ALTER TABLE `guild_banktabs` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_banktabs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_data`
--

DROP TABLE IF EXISTS `guild_data`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `guild_data` (
  `guildid` int(30) NOT NULL,
  `playerid` int(30) NOT NULL,
  `guildRank` int(30) NOT NULL,
  `publicNote` varchar(300) NOT NULL,
  `officerNote` varchar(300) NOT NULL,
  `lastWithdrawReset` int(30) NOT NULL DEFAULT '0',
  `withdrawlsSinceLastReset` int(30) NOT NULL DEFAULT '0',
  `lastItemWithdrawReset0` int(30) NOT NULL DEFAULT '0',
  `itemWithdrawlsSinceLastReset0` int(30) NOT NULL DEFAULT '0',
  `lastItemWithdrawReset1` int(30) NOT NULL,
  `itemWithdrawlsSinceLastReset1` int(30) NOT NULL,
  `lastItemWithdrawReset2` int(30) NOT NULL,
  `itemWithdrawlsSinceLastReset2` int(30) NOT NULL,
  `lastItemWithdrawReset3` int(30) NOT NULL,
  `itemWithdrawlsSinceLastReset3` int(30) NOT NULL,
  `lastItemWithdrawReset4` int(30) NOT NULL,
  `itemWithdrawlsSinceLastReset4` int(30) NOT NULL,
  `lastItemWithdrawReset5` int(30) NOT NULL,
  `itemWithdrawlsSinceLastReset5` int(30) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `guild_data`
--

LOCK TABLES `guild_data` WRITE;
/*!40000 ALTER TABLE `guild_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_logs`
--

DROP TABLE IF EXISTS `guild_logs`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `guild_logs` (
  `log_id` int(30) NOT NULL,
  `guildid` int(30) NOT NULL,
  `timestamp` int(30) NOT NULL,
  `event_type` int(30) NOT NULL,
  `misc1` int(30) NOT NULL,
  `misc2` int(30) NOT NULL,
  `misc3` int(30) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `guild_logs`
--

LOCK TABLES `guild_logs` WRITE;
/*!40000 ALTER TABLE `guild_logs` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_logs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guild_ranks`
--

DROP TABLE IF EXISTS `guild_ranks`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `guild_ranks` (
  `guildId` int(6) unsigned NOT NULL DEFAULT '0',
  `rankId` int(1) NOT NULL DEFAULT '0',
  `rankName` varchar(255) NOT NULL DEFAULT '',
  `rankRights` int(3) unsigned NOT NULL DEFAULT '0',
  `goldLimitPerDay` int(30) NOT NULL DEFAULT '0',
  `bankTabFlags0` int(30) NOT NULL DEFAULT '0',
  `itemStacksPerDay0` int(30) NOT NULL DEFAULT '0',
  `bankTabFlags1` int(30) NOT NULL DEFAULT '0',
  `itemStacksPerDay1` int(30) NOT NULL DEFAULT '0',
  `bankTabFlags2` int(30) NOT NULL DEFAULT '0',
  `itemStacksPerDay2` int(30) NOT NULL DEFAULT '0',
  `bankTabFlags3` int(30) NOT NULL DEFAULT '0',
  `itemStacksPerDay3` int(30) NOT NULL DEFAULT '0',
  `bankTabFlags4` int(30) NOT NULL DEFAULT '0',
  `itemStacksPerDay4` int(30) NOT NULL DEFAULT '0',
  `bankTabFlags5` int(30) NOT NULL DEFAULT '0',
  `itemStacksPerDay5` int(30) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildId`,`rankId`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `guild_ranks`
--

LOCK TABLES `guild_ranks` WRITE;
/*!40000 ALTER TABLE `guild_ranks` DISABLE KEYS */;
/*!40000 ALTER TABLE `guild_ranks` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `guilds`
--

DROP TABLE IF EXISTS `guilds`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `guilds` (
  `guildId` bigint(20) NOT NULL AUTO_INCREMENT,
  `guildName` varchar(32) NOT NULL DEFAULT '',
  `leaderGuid` bigint(20) NOT NULL DEFAULT '0',
  `emblemStyle` int(10) NOT NULL DEFAULT '0',
  `emblemColor` int(10) NOT NULL DEFAULT '0',
  `borderStyle` int(10) NOT NULL DEFAULT '0',
  `borderColor` int(10) NOT NULL DEFAULT '0',
  `backgroundColor` int(10) NOT NULL DEFAULT '0',
  `guildInfo` varchar(100) NOT NULL DEFAULT '',
  `motd` varchar(100) NOT NULL DEFAULT '',
  `createdate` int(30) NOT NULL DEFAULT '0',
  `bankTabCount` int(30) NOT NULL DEFAULT '0',
  `bankBalance` int(30) NOT NULL,
  PRIMARY KEY (`guildId`),
  UNIQUE KEY `guildId` (`guildId`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `guilds`
--

LOCK TABLES `guilds` WRITE;
/*!40000 ALTER TABLE `guilds` DISABLE KEYS */;
/*!40000 ALTER TABLE `guilds` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2007-12-18 21:44:22
