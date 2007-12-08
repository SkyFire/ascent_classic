DROP TABLE IF EXISTS `account_data`;
CREATE TABLE `account_data` (
  `acct` int(30) NOT NULL,
  `accountdata0` text NOT NULL,
  `accountdata1` text NOT NULL,
  `accountdata2` text NOT NULL,
  `accountdata3` text NOT NULL,
  `accountdata4` text NOT NULL,
  `accountdata5` text NOT NULL,
  `accountdata6` text NOT NULL,
  `accountdata7` text NOT NULL,
  PRIMARY KEY (`acct`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
