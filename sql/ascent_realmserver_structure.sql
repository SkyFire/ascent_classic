-- MySQL dump 10.11
--
-- Host: localhost    Database: ascent_logon
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
-- Table structure for table `accounts`
--

DROP TABLE IF EXISTS `accounts`;
CREATE TABLE `accounts` (
  `acct` bigint(20) unsigned NOT NULL auto_increment,
  `login` varchar(255) collate utf8_unicode_ci NOT NULL default '',
  `password` varchar(28) collate utf8_unicode_ci NOT NULL default '',
  `gm` varchar(27) collate utf8_unicode_ci NOT NULL default '',
  `banned` int(1) unsigned NOT NULL default '0',
  `lastlogin` timestamp NOT NULL default '0000-00-00 00:00:00',
  `lastip` varchar(15) collate utf8_unicode_ci NOT NULL default '',
  `email` varchar(50) collate utf8_unicode_ci NOT NULL default '',
  `flags` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`acct`,`login`),
  UNIQUE KEY `acct` (`acct`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Table structure for table `ipbans`
--

DROP TABLE IF EXISTS `ipbans`;
CREATE TABLE `ipbans` (
  `ip` varchar(15) collate utf8_unicode_ci NOT NULL,
  `expire` int(16) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ip`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2007-07-26  5:29:52
