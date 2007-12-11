alter table guilds add column guildbank_Tabs int(30) default 0 not null;
alter table guild_ranks change column rankRights_1 goldLimitPerDay int(30) default 0 not null;
alter table guild_ranks change column rankRights_2 bankTabFlags int(30) default 0 not null;
alter table guild_ranks change column rankRights_3 itemStacksPerDay int(30) default 0 not null;
alter table guild_data add column lastWithdrawReset int(30) default 0 not null;
alter table guild_data add column withdrawlsSinceLastReset int(30) default 0 not null;
alter table guild_data add column lastItemWithdrawReset int(30) default 0 not null;
alter table guild_data add column itemWithdrawlsSinceLastReset int(30) default 0 not null;