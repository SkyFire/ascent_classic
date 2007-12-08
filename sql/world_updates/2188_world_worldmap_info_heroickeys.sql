UPDATE worldmap_info SET type=4 WHERE entry IN (552, 554, 553, 540, 542, 543, 547, 546, 545, 560, 269, 556, 555, 557, 558 );
ALTER TABLE `worldmap_info` ADD COLUMN `heroic_keyid_1` INTEGER(30) NOT NULL DEFAULT 0 AFTER `required_item`;
ALTER TABLE `worldmap_info` ADD COLUMN `heroic_keyid_2` INTEGER(30) NOT NULL DEFAULT 0 AFTER `heroic_keyid_1`;
UPDATE `worldmap_info` SET `heroic_keyid_1`=30633 WHERE entry IN (555,556,557,558);
UPDATE `worldmap_info` SET `heroic_keyid_1`=30634 WHERE entry IN (552,553,554);
UPDATE `worldmap_info` SET `heroic_keyid_1`=30635 WHERE entry IN (269,560);
UPDATE `worldmap_info` SET `heroic_keyid_1`=30623 WHERE entry IN (545,546,547);
UPDATE `worldmap_info` SET `heroic_keyid_1`=30637 WHERE entry IN (540,542,543);
UPDATE `worldmap_info` SET `heroic_keyid_2`=30622 WHERE entry IN (540,542,543);