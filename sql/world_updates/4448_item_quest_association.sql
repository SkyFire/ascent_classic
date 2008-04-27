-- ----------------------------
-- Table structure for item_quest_association
-- ----------------------------
CREATE TABLE `item_quest_association` (
  `item` int(11) NOT NULL DEFAULT '0',
  `quest` int(11) NOT NULL DEFAULT '0',
  `item_count` int(11) NOT NULL DEFAULT '0',
  UNIQUE KEY `item` (`item`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='Quest System';

-- ----------------------------
-- Example http://www.wowhead.com/?quest=1016
-- ----------------------------
INSERT INTO `item_quest_association` VALUES ('12220', '1016', '5');