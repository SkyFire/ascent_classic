
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
CREATE TABLE `accounts` (
  `acct` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Unique ID',
  `login` varchar(32) COLLATE utf8_unicode_ci NOT NULL COMMENT 'Login username',
  `password` varchar(32) COLLATE utf8_unicode_ci NOT NULL COMMENT 'Login password',
  `gm` varchar(32) COLLATE utf8_unicode_ci NOT NULL DEFAULT '' COMMENT 'Game permissions',
  `banned` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Account Standing',
  `lastlogin` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT 'Last login timestamp',
  `lastip` varchar(16) COLLATE utf8_unicode_ci NOT NULL DEFAULT '' COMMENT 'Last remote address',
  `email` varchar(64) COLLATE utf8_unicode_ci NOT NULL DEFAULT '' COMMENT 'Contact e-mail address',
  `flags` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Client flags',
  `forceLanguage` varchar(5) COLLATE utf8_unicode_ci NOT NULL DEFAULT 'enUS',
  PRIMARY KEY (`acct`),
  UNIQUE KEY `login` (`login`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='Account Information';
SET character_set_client = @saved_cs_client;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ipbans` (
  `ip` varchar(16) COLLATE utf8_unicode_ci NOT NULL DEFAULT '' COMMENT 'Remote host',
  `expire` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT 'Expiry time (s)',
  PRIMARY KEY (`ip`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='IPBanner';
SET character_set_client = @saved_cs_client;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

/*
MySQL Data Transfer
Source Host: localhost
Source Database: logon
Target Host: localhost
Target Database: logon
Date: 10/30/2007 9:21:50 PM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for account_data
-- ----------------------------
CREATE TABLE `account_data` (
  `acct` int(30) NOT NULL,
  `uiconfig0` longtext,
  `uiconfig1` longtext,
  `uiconfig2` longtext,
  `uiconfig3` longtext,
  `uiconfig4` longtext,
  `uiconfig5` longtext,
  `uiconfig6` longtext,
  `uiconfig7` longtext,
  `uiconfig8` longtext,
  PRIMARY KEY  (`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records 
-- ----------------------------
