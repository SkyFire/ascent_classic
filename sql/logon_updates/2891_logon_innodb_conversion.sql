-- Remove MysISAM Indexes

alter table accounts drop index login;

-- Convert to InnoDB

alter table accounts type=InnoDB;
alter table ipbans type=InnoDB;

-- Add InnoDB Indices

ALTER TABLE `accounts` ADD UNIQUE `a`(`login`);
ALTER TABLE `ipbans` ADD UNIQUE `a`(`ip`);
