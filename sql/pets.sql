DROP TABLE `playerpets`;

CREATE TABLE `playerpets` (
  `ownerguid` bigint(20) NOT NULL default '0',
  `petnumber` int(11) NOT NULL default '0',
  `name` varchar(21) NOT NULL default '',
  `entryid` bigint(20) NOT NULL default '0',
  `data` longtext NOT NULL,
  `xp` int(11) NOT NULL default '0',
  `active` tinyint(1) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  `happiness` int(11) NOT NULL default '0',
  `actionbar` varchar(200) NOT NULL default '',
  `happinessupdate` int(11) NOT NULL default '0',
  `summon` int(11) NOT NULL default '0',
  `autocastspell` int(11) NOT NULL default '0',
  `loyaltypts` int(11) NOT NULL default '0',
  `loyaltyupdate` int(11) NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;