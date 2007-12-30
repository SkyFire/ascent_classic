ALTER TABLE `playerpets` CHANGE `entryId` `entry` int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `playerpets` CHANGE `data` `fields` longtext COLLATE utf8_unicode_ci NOT NULL;
ALTER TABLE `playerpets` DROP COLUMN `autocastspell`;