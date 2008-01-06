-- Remove MysISAM Indexes

alter table account_data drop index a;
alter table auctions drop index auctionHouse;
alter table characters drop index acct;
alter table characters_insert_queue drop index guid;
alter table characters_insert_queue drop index acct;
alter table characters_insert_queue drop index guildid;
alter table charters drop index leaderGuid;
alter table guild_bankitems drop index a;
alter table guild_bankitems drop index b;
alter table guild_bankitems drop index c;
alter table guild_banklogs drop index a;
alter table guild_banklogs drop index b;
alter table guild_banktabs drop index a;
alter table guild_banktabs drop index b;
alter table guilds drop index guildId;
alter table playeritems drop index ownerguid;
alter table playeritems_external drop index guid;
alter table playeritems_external drop index ownerguid;
alter table playeritems_insert_queue drop index ownerguid;

-- convert to InnoDB

alter table account_data type=InnoDB;
alter table account_forced_permissions type=InnoDB;
alter table arenateams type=InnoDB;
alter table auctions type=InnoDB;
alter table characters type=InnoDB;
alter table characters_insert_queue type=InnoDB;
alter table charters type=InnoDB;
alter table corpses type=InnoDB;
alter table gm_tickets type=InnoDB;
alter table groups type=InnoDB;
alter table guild_bankitems type=InnoDB;
alter table guild_banklogs type=InnoDB;
alter table guild_banktabs type=InnoDB;
alter table guild_checkpoints type=InnoDB;
alter table guild_data type=InnoDB;
alter table guild_logs type=InnoDB;
alter table guild_ranks type=InnoDB;
alter table guilds type=InnoDB;
alter table instances type=InnoDB;
alter table mailbox type=InnoDB;
alter table playercooldownitems type=InnoDB;
alter table playercooldownsecurity type=InnoDB;
alter table playeritems type=InnoDB;
alter table playeritems_external type=InnoDB;
alter table playeritems_insert_queue type=InnoDB;
alter table playerpets type=InnoDB;
alter table playerpetspells type=InnoDB;
alter table playersummonspells type=InnoDB;
alter table questlog type=InnoDB;
alter table server_settings type=InnoDB;
alter table social type=InnoDB;
alter table tutorials type=InnoDB;

-- Add InnoDB Indices

ALTER TABLE `account_data` ADD UNIQUE `a`(`acct`);
ALTER TABLE `account_forced_permissions` ADD UNIQUE `a`(`login`);
ALTER TABLE `arenateams` ADD UNIQUE `a`(`id`);
ALTER TABLE `auctions` ADD UNIQUE `a`(`auctionId`), ADD INDEX `b`(`auctionHouse`);
ALTER TABLE `characters` ADD UNIQUE `a`(`guid`), ADD INDEX `b`(`acct`), ADD INDEX `c`(`name`), ADD INDEX `d`(`banned`, `online`);
ALTER TABLE `charters` ADD UNIQUE `a`(`charterId`);
ALTER TABLE `corpses` ADD UNIQUE `a`(`guid`), ADD INDEX `b`(`mapId`, `instanceId`);
ALTER TABLE `gm_tickets` ADD UNIQUE `a`(`guid`);
ALTER TABLE `groups` ADD UNIQUE `a`(`group_id`);
ALTER TABLE `guild_bankitems` ADD INDEX `a`(`guildId`, `tabId`, `slotId`);
ALTER TABLE `guild_banklogs` ADD INDEX `a`(`log_id`, `guildid`);
ALTER TABLE `guild_banktabs` ADD INDEX `a`(`guildId`, `tabId`);
ALTER TABLE `guild_data` ADD INDEX `a`(`guildid`, `playerid`);
ALTER TABLE `guild_logs` ADD INDEX `a`(`log_id`, `guildid`);
ALTER TABLE `guild_ranks` ADD INDEX `a`(`guildId`, `rankId`);
ALTER TABLE `guilds` ADD UNIQUE `a`(`guildId`);
ALTER TABLE `instances` ADD UNIQUE `a`(`id`), ADD INDEX `b`(`mapid`), ADD INDEX `c`(`expiration`);
ALTER TABLE `mailbox` ADD UNIQUE `a`(`message_id`), ADD INDEX `b`(`player_guid`);
ALTER TABLE `playercooldownitems` ADD INDEX `a`(`OwnerGuid`), ADD INDEX `b`(`ItemEntry`, `CooldownTimeStamp`, `Cooldown`);
ALTER TABLE `playercooldownsecurity` ADD INDEX `a`(`OwnerGuid`, `SpellID`, `TimeStamp`);
ALTER TABLE `playeritems` ADD UNIQUE `a`(`guid`), ADD INDEX `b`(`ownerguid`), ADD INDEX `c`(`itemtext`);
ALTER TABLE `playeritems_external` ADD INDEX `a`(`ownerguid`), ADD UNIQUE `b`(`guid`);
ALTER TABLE `playerpets` ADD INDEX `a`(`ownerguid`, `petnumber`);
ALTER TABLE `playerpetspells` ADD INDEX `a`(`ownerguid`, `petnumber`);
ALTER TABLE `playersummonspells` ADD INDEX `a`(`ownerguid`, `entryid`);
ALTER TABLE `questlog` ADD INDEX `a`(`index`, `player_guid`, `quest_id`, `slot`);
ALTER TABLE `server_settings` ADD UNIQUE `a`(`setting_id`);
ALTER TABLE `social` ADD INDEX `a`(`guid`, `socialguid`, `flags`);
ALTER TABLE `tutorials` ADD UNIQUE `a`(`playerId`);
