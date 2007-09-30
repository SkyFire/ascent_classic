CREATE TABLE `spell_proc_data` (
  `name_hash` int(30) NOT NULL,
  `proc_chance` int(30) NOT NULL,
  `forced_proc_flags` int(30) NOT NULL default '-1',
  PRIMARY KEY  (`name_hash`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;