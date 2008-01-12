ALTER TABLE `creature_spawns` ADD UNIQUE `a`(`id`), ADD INDEX `b`(`map`);
ALTER TABLE `creature_staticspawns` ADD INDEX `a`(`Map`);
ALTER TABLE `creature_waypoints` ADD UNIQUE `a`(`spawnid`, `waypointid`);
alter table spelloverride type=InnoDB;