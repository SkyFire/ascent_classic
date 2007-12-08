alter table characters drop column guildid;
alter table characters drop column guildRank;
alter table characters drop column publicNote;
alter table characters drop column officerNote;
alter table guilds modify column createdate int(30) not null default 0;

CREATE TABLE `guild_logs` (
  `log_id` int(30) NOT NULL,
  `guildid` int(30) NOT NULL,
  `timestamp` int(30) NOT NULL,
  `event_type` int(30) NOT NULL,
  `misc1` int(30) NOT NULL,
  `misc2` int(30) NOT NULL,
  `misc3` int(30) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `guild_data` (
  `guildid` int(30) NOT NULL,
  `playerid` int(30) NOT NULL,
  `guildRank` int(30) NOT NULL,
  `publicNote` varchar(300) NOT NULL,
  `officerNote` varchar(300) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;