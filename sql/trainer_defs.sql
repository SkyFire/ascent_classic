DROP TABLE IF EXISTS `trainer_defs`;
CREATE TABLE `trainer_defs` (
  `entry` int(11) unsigned NOT NULL DEFAULT '0',
  `required_skill` int(11) unsigned NOT NULL DEFAULT '0',
  `required_skillvalue` int(11) unsigned DEFAULT '0',
  `req_class` int(11) unsigned NOT NULL DEFAULT '0',
  `trainer_type` int(11) unsigned NOT NULL DEFAULT '0',
  `trainer_ui_window_message` text,
  `can_train_gossip_textid` int(11) NOT NULL,
  `cannot_train_gossip_textid` int(11) NOT NULL,
  PRIMARY KEY (`entry`),
  UNIQUE KEY `entry` (`entry`),
  KEY `entry_2` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Trainer System';
