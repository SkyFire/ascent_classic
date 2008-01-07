-- Remove Indices

alter table account_data drop index a;
alter table account_forced_permissions drop index a;
alter table arenateams drop index a;
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

-- Add Indices

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
ALTER TABLE `questlog` ADD INDEX `a`(`index`), INDEX `b`(`player_guid`), INDEX `c`(`quest_id`), INDEX `d`(`slot`);
ALTER TABLE `server_settings` ADD UNIQUE `a`(`setting_id`);
ALTER TABLE `social` ADD INDEX `a`(`guid`, `socialguid`, `flags`);
ALTER TABLE `tutorials` ADD UNIQUE `a`(`playerId`);
