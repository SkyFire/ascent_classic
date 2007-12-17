alter table guilds add column bankBalance int(30) default 0 not null;
alter table guilds drop guildbank_Tabs;


CREATE TABLE `guild_banktabs` (
  `GuildId` INTEGER(11) NOT NULL DEFAULT 0,
  `TabId` VARCHAR(32) NOT NULL DEFAULT '',
  `tabName` VARCHAR(32) NOT NULL DEFAULT '',
  `tabIcon` VARCHAR(32) NOT NULL DEFAULT '');

CREATE TABLE `guild_bankitems` (
  `guildID` INTEGER(11) UNSIGNED NOT NULL DEFAULT '0',
  `tabID` INTEGER(11) UNSIGNED NOT NULL DEFAULT '0',
  `slotID` INTEGER(11) UNSIGNED NOT NULL DEFAULT '0',
  `itemGuid` INTEGER(11) UNSIGNED NOT NULL DEFAULT '0');
