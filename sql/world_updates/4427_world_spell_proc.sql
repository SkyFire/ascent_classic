CREATE TABLE `spell_proc` (
  `spellID` int(30) NOT NULL default '0',
  `ProcOnNameHash` int(30) unsigned NOT NULL default '0',
  PRIMARY KEY  (`spellID`,`ProcOnNameHash`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;