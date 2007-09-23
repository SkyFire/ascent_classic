/*
MySQL Data Transfer
Source Host: localhost
Source Database: antrix
Target Host: localhost
Target Database: antrix
Date: 14/08/2007 23:34:30
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for weather
-- ----------------------------
CREATE TABLE `weather` (
  `zoneId` int(11) unsigned NOT NULL DEFAULT '0',
  `high_chance` int(11) unsigned NOT NULL DEFAULT '0',
  `high_type` int(11) unsigned NOT NULL DEFAULT '0',
  `med_chance` int(11) unsigned NOT NULL DEFAULT '0',
  `med_type` int(11) unsigned NOT NULL DEFAULT '0',
  `low_chance` int(11) unsigned NOT NULL DEFAULT '0',
  `low_type` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`zoneId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Weather System';

-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `weather` VALUES ('1', '80', '8', '5', '1', '1', '2');
INSERT INTO `weather` VALUES ('12', '20', '2', '10', '4', '0', '0');
INSERT INTO `weather` VALUES ('33', '80', '2', '40', '4', '20', '1');
INSERT INTO `weather` VALUES ('36', '80', '8', '10', '1', '0', '0');
INSERT INTO `weather` VALUES ('85', '10', '2', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('148', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('357', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('440', '25', '16', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('490', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('618', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('1377', '25', '16', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('3', '20', '16', '2', '2', '0', '0');
INSERT INTO `weather` VALUES ('4', '10', '1', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('8', '65', '1', '55', '2', '0', '0');
INSERT INTO `weather` VALUES ('10', '50', '2', '5', '1', '0', '0');
INSERT INTO `weather` VALUES ('11', '70', '2', '60', '4', '5', '1');
INSERT INTO `weather` VALUES ('14', '2', '2', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('15', '40', '2', '35', '1', '0', '0');
INSERT INTO `weather` VALUES ('16', '25', '2', '5', '1', '0', '0');
INSERT INTO `weather` VALUES ('17', '1', '2', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('28', '5', '2', '2', '1', '0', '0');
INSERT INTO `weather` VALUES ('38', '40', '2', '10', '4', '2', '1');
INSERT INTO `weather` VALUES ('40', '5', '2', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('41', '10', '1', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('44', '20', '2', '5', '1', '0', '0');
INSERT INTO `weather` VALUES ('45', '20', '2', '5', '1', '0', '0');
INSERT INTO `weather` VALUES ('46', '5', '1', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('47', '30', '2', '10', '4', '5', '1');
INSERT INTO `weather` VALUES ('51', '5', '2', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('130', '20', '2', '5', '4', '0', '0');
INSERT INTO `weather` VALUES ('139', '5', '2', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('141', '40', '2', '20', '4', '0', '0');
INSERT INTO `weather` VALUES ('215', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('267', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('331', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('361', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('400', '25', '16', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('405', '5', '2', '1', '16', '0', '0');
INSERT INTO `weather` VALUES ('406', '40', '2', '20', '4', '0', '0');
INSERT INTO `weather` VALUES ('1519', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('1637', '5', '2', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('1638', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('1657', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('3430', '5', '2', '0', '0', '0', '0');
INSERT INTO `weather` VALUES ('3433', '40', '2', '20', '4', '0', '0');
INSERT INTO `weather` VALUES ('3518', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('3521', '40', '2', '20', '4', '0', '0');
INSERT INTO `weather` VALUES ('3524', '40', '2', '20', '4', '0', '0');
INSERT INTO `weather` VALUES ('3525', '40', '2', '20', '4', '0', '0');
INSERT INTO `weather` VALUES ('3557', '40', '2', '20', '4', '5', '1');
INSERT INTO `weather` VALUES ('3703', '40', '2', '0', '0', '0', '0');
