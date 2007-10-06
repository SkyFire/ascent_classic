DROP TABLE IF EXISTS `trainer_spells`;
CREATE TABLE `trainer_spells` (
  `entry` int(11) unsigned NOT NULL DEFAULT '0',
  `cast_spell` int(11) unsigned NOT NULL DEFAULT '0',
  `spellcost` int(11) unsigned NOT NULL DEFAULT '0',
  `reqspell` int(11) unsigned NOT NULL DEFAULT '0',
  `reqskill` int(11) unsigned NOT NULL DEFAULT '0',
  `reqskillvalue` int(11) unsigned NOT NULL DEFAULT '0',
  `reqlevel` int(11) unsigned NOT NULL DEFAULT '0',
  `deletespell` int(11) unsigned NOT NULL DEFAULT '0',
  `is_prof` tinyint(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`,`cast_spell`),
  KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Trainer System';
