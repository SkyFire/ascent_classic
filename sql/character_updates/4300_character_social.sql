DROP TABLE social;
CREATE TABLE `social_friends` (
  `character_guid` int(30) NOT NULL,
  `friend_guid` int(30) NOT NULL,
  `note` varchar(100) NOT NULL,
  PRIMARY KEY (`character_guid`,`friend_guid`),
  KEY `a` (`character_guid`),
  KEY `b` (`friend_guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `social_ignores` (
  `character_guid` int(30) NOT NULL,
  `ignore_guid` int(30) NOT NULL,
  PRIMARY KEY (`character_guid`,`ignore_guid`),
  KEY `a` (`character_guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
