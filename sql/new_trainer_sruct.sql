# MySQL-Front Dump 2.5
#
# Host: localhost   Database: ascent2
# --------------------------------------------------------
# Server version 5.0.45-community-nt


#
# Table structure for table 'trainer_defs'
#

DROP TABLE IF EXISTS `trainer_defs`;
CREATE TABLE IF NOT EXISTS `trainer_defs` (
  `entry` int(11) unsigned NOT NULL DEFAULT '0' ,
  `req_rep` int(11) unsigned NOT NULL DEFAULT '0' ,
  `req_rep_val` int(11) unsigned DEFAULT '0' ,
  `req_class` int(11) unsigned NOT NULL DEFAULT '0' ,
  `req_level` int(11) unsigned NOT NULL DEFAULT '0' ,
  `trainer_type` int(11) unsigned NOT NULL DEFAULT '0' ,
  `can_train_msg` text ,
  `cannot_train_msg` text ,
  PRIMARY KEY (`entry`),
  UNIQUE KEY entry (`entry`),
   KEY entry_2 (`entry`)
);



#
# Table structure for table 'trainer_spells'
#

DROP TABLE IF EXISTS `trainer_spells`;
CREATE TABLE IF NOT EXISTS `trainer_spells` (
  `entry` int(11) unsigned NOT NULL DEFAULT '0' ,
  `cast_spell` int(11) unsigned NOT NULL DEFAULT '0' ,
  `spellcost` int(11) unsigned NOT NULL DEFAULT '0' ,
  `reqspell` int(11) unsigned NOT NULL DEFAULT '0' ,
  `reqskill` int(11) unsigned NOT NULL DEFAULT '0' ,
  `reqskillvalue` int(11) unsigned NOT NULL DEFAULT '0' ,
  `reqlevel` int(11) unsigned NOT NULL DEFAULT '0' ,
  `deletespell` int(11) unsigned NOT NULL DEFAULT '0' ,
  `is_prof` tinyint(1) unsigned NOT NULL DEFAULT '0' ,
  PRIMARY KEY (`entry`,`cast_spell`),
   KEY entry (`entry`)
);

