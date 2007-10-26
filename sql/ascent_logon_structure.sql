CREATE TABLE IF NOT EXISTS `accounts` (
	`acct` int unsigned NOT NULL auto_increment COMMENT 'Unique ID',
	`login` varchar(32) NOT NULL COMMENT 'Login username',
	`password` varchar(32) NOT NULL COMMENT 'Login password',
	`gm` varchar(32) NOT NULL DEFAULT '' COMMENT 'Game permissions',
	`banned` tinyint unsigned NOT NULL DEFAULT 0 COMMENT 'Account Standing',
	`lastlogin` timestamp NOT NULL DEFAULT 0 COMMENT 'Last login timestamp',
	`lastip` varchar(16) NOT NULL DEFAULT '' COMMENT 'Last remote address',
	`email` varchar(64) NOT NULL DEFAULT '' COMMENT 'Contact e-mail address',
	`flags` tinyint UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Client flags',
--	`created` datetime NOT NULL DEFAULT 0 COMMENT 'Creation date',		-- source?
	PRIMARY KEY (`acct`),
	UNIQUE KEY (`login`)
) COMMENT='Account Information';


CREATE TABLE IF NOT EXISTS `ipbans` (
	`ip` varchar(16) NOT NULL DEFAULT '' COMMENT 'Remote host',
	`expire` timestamp NOT NULL DEFAULT 0 COMMENT 'Expiry time (s)',
	PRIMARY KEY (`ip`)
) COMMENT 'IPBanner';