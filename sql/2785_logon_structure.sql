/*
SQLyog Enterprise - MySQL GUI v6.13
MySQL - 5.0.41-community-nt : Database - logon
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

/*Table structure for table `accounts` */

CREATE TABLE `accounts` (
  `acct` int(10) unsigned NOT NULL auto_increment COMMENT 'Unique ID',
  `login` varchar(32) collate utf8_unicode_ci NOT NULL COMMENT 'Login username',
  `password` varchar(32) collate utf8_unicode_ci NOT NULL COMMENT 'Login password',
  `gm` varchar(32) collate utf8_unicode_ci NOT NULL default '' COMMENT 'Game permissions',
  `banned` tinyint(3) unsigned NOT NULL default '0' COMMENT 'Account Standing',
  `lastlogin` timestamp NOT NULL default '0000-00-00 00:00:00' COMMENT 'Last login timestamp',
  `lastip` varchar(16) collate utf8_unicode_ci NOT NULL default '' COMMENT 'Last remote address',
  `email` varchar(64) collate utf8_unicode_ci NOT NULL default '' COMMENT 'Contact e-mail address',
  `flags` tinyint(3) unsigned NOT NULL default '0' COMMENT 'Client flags',
  `forceLanguage` varchar(5) collate utf8_unicode_ci NOT NULL default 'enUS',
  PRIMARY KEY  (`acct`),
  UNIQUE KEY `login` (`login`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='Account Information';

/*Table structure for table `ipbans` */

CREATE TABLE `ipbans` (
  `ip` varchar(16) collate utf8_unicode_ci NOT NULL default '' COMMENT 'Remote host',
  `expire` timestamp NOT NULL default '0000-00-00 00:00:00' COMMENT 'Expiry time (s)',
  PRIMARY KEY  (`ip`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='IPBanner';

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;