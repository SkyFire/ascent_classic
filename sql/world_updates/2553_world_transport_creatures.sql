
CREATE TABLE `transport_creatures` (
  `transport_entry` int(10) unsigned NOT NULL,
  `creature_entry` int(10) unsigned NOT NULL,
  `position_x` float NOT NULL,
  `position_y` float NOT NULL,
  `position_z` float NOT NULL,
  `orientation` float NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

