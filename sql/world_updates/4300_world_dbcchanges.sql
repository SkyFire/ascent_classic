CREATE TABLE `items_extendedcost` (
  `ItemId` int(30) NOT NULL,
  `CostId` int(30) NOT NULL,
  PRIMARY KEY (`ItemId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

insert into items_extendedcost (select entry, ItemExtendedCost from items where ItemExtendedCost != 0);
ALTER TABLE items DROP COLUMN ItemExtendedCost;
ALTER TABLE items DROP COLUMN ArenaRankRequirement;