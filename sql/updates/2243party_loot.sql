ALTER TABLE `creatureloot` ADD COLUMN `ffa_loot` INTEGER UNSIGNED NOT NULL DEFAULT 0 AFTER `maxcount`;
-- those dont realy need this column but since they are loaded using template function i surely dont want to make an exceptions, tho it may save some memory.
ALTER TABLE `fishingloot` ADD COLUMN `ffa_loot` INTEGER UNSIGNED NOT NULL DEFAULT 0 AFTER `maxcount`;
ALTER TABLE `itemloot` ADD COLUMN `ffa_loot` INTEGER UNSIGNED NOT NULL DEFAULT 0 AFTER `maxcount`;
ALTER TABLE `pickpocketingloot` ADD COLUMN `ffa_loot` INTEGER UNSIGNED NOT NULL DEFAULT 0 AFTER `maxcount`;
ALTER TABLE `prospectingloot` ADD COLUMN `ffa_loot` INTEGER UNSIGNED NOT NULL DEFAULT 0 AFTER `maxcount`;
ALTER TABLE `skinningloot` ADD COLUMN `ffa_loot` INTEGER UNSIGNED NOT NULL DEFAULT 0 AFTER `maxcount`;
ALTER TABLE `objectloot` ADD COLUMN `ffa_loot` INTEGER UNSIGNED NOT NULL DEFAULT 0 AFTER `maxcount`;

