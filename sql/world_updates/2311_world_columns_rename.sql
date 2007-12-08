/* -- Project Re-name --
In this project We've decided to start rename column names, and make them more understand able.

*/
-- Ai Agents
ALTER TABLE `ai_agents` CHANGE `entryId` `entry` INT(11) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `ai_agents` CHANGE `AI_AGENT` `type` SmallINT(5) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `ai_agents` CHANGE `procEvent` `event` INT(11) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `ai_agents` CHANGE `procChance` `chance` INT(11) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `ai_agents` CHANGE `procCount` `maxcount` INT(11) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `ai_agents` CHANGE `spellCooldown` `cooldown` INT(8) NOT NULL DEFAULT '0';
ALTER TABLE `ai_agents` CHANGE `spellId` `spell` INT(11) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `ai_agents` CHANGE `spellType` `spelltype` INT(11) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `ai_agents` CHANGE `spelltargetType` `targettype` INT(11) UNSIGNED NOT NULL DEFAULT '0';

-- ai_threattospellid
ALTER TABLE `ai_threattospellid` CHANGE `spellId` `spell` INT(11) UNSIGNED NOT NULL DEFAULT '0';

-- areatriggers
ALTER TABLE `areatriggers` CHANGE `AreaTriggerid` `entry` INT(11) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `areatriggers` CHANGE `Type` `type` TinyINT(3) UNSIGNED NULL DEFAULT '0';
ALTER TABLE `areatriggers` CHANGE `Mapid` `map` INT(11) UNSIGNED NULL;
ALTER TABLE `areatriggers` CHANGE `Screen` `screen` INT(11) UNSIGNED NULL;
ALTER TABLE `areatriggers` CHANGE `Name` `name` varchar(100) NULL DEFAULT '0';
ALTER TABLE `areatriggers` CHANGE `x` `position_x` float NOT NULL DEFAULT '0';
ALTER TABLE `areatriggers` CHANGE `y` `position_y` float NOT NULL DEFAULT '0';
ALTER TABLE `areatriggers` CHANGE `z` `position_z` float NOT NULL DEFAULT '0';
ALTER TABLE `areatriggers` CHANGE `o` `orientation` float NOT NULL DEFAULT '0';

-- auctionhouse
ALTER TABLE `auctionhouse` CHANGE `entryid` `id` INT(32) NOT NULL AUTO_INCREMENT;
ALTER TABLE `auctionhouse` CHANGE `auctioneer` `creature_entry` BigINT(64) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `auctionhouse` CHANGE `AHid` `group` INT(32) NOT NULL DEFAULT '0';

-- creature_formations
ALTER TABLE `creature_formations` CHANGE `creature_sqlid` `spawn_id` INT(10) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_formations` CHANGE `followtarget_sqlid` `target_spawn_id` INT(10) UNSIGNED NOT NULL DEFAULT '0';

-- creature_names
ALTER TABLE `creature_names` CHANGE `creature_name` `name` varchar(100) NOT NULL;
ALTER TABLE `creature_names` CHANGE `Subname` `subname` varchar(100) NOT NULL;
ALTER TABLE `creature_names` CHANGE `Family` `family` INT(10) UNSIGNED NULL DEFAULT '0';
ALTER TABLE `creature_names` CHANGE `Rank` `rank` INT(10) UNSIGNED NULL DEFAULT '0';
ALTER TABLE `creature_names` CHANGE `SpellDataID` `spelldataid` INT(10) UNSIGNED NULL;
ALTER TABLE `creature_names` CHANGE `unknown_int1` `male_displayid2` INT(30) NOT NULL DEFAULT '0';
ALTER TABLE `creature_names` CHANGE `unknown_int2` `female_displayid2` INT(30) NOT NULL DEFAULT '0';
ALTER TABLE `creature_names` CHANGE `Civilian` `civilian` INT(4) UNSIGNED NULL;
ALTER TABLE `creature_names` CHANGE `Leader` `leader` TinyINT(3) UNSIGNED NULL;

-- creature_proto
ALTER TABLE `creature_proto` CHANGE `resistance0_armor` `armor` INT(30) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_proto` CHANGE `item1slotdisplay` `equipmodel1` INT(30) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_proto` CHANGE `item2slotdisplay` `equipmodel2` INT(30) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_proto` CHANGE `item3slotdisplay` `equipmodel3` INT(30) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_proto` CHANGE `item1info1` `equipinfo1` INT(30) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_proto` CHANGE `item2info1` `equipinfo2` INT(30) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_proto` CHANGE `item3info1` `equipinfo3` INT(30) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_proto` CHANGE `item1info2` `equipslot1` INT(30) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_proto` CHANGE `item2info2` `equipslot2` INT(30) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_proto` CHANGE `item3info2` `equipslot3` INT(30) UNSIGNED NOT NULL DEFAULT '0';

-- creature_spawns
ALTER TABLE `creature_spawns` CHANGE `x` `position_x` float NOT NULL;
ALTER TABLE `creature_spawns` CHANGE `y` `position_y` float NOT NULL;
ALTER TABLE `creature_spawns` CHANGE `z` `position_z` float NOT NULL;
ALTER TABLE `creature_spawns` CHANGE `o` `orientation` float NOT NULL;
ALTER TABLE `creature_spawns` CHANGE `factionid` `faction` INT(30) NOT NULL DEFAULT '14';
ALTER TABLE `creature_spawns` CHANGE `Map` `map` INT(30) NOT NULL;

-- creature_waypoints
ALTER TABLE `creature_waypoints` CHANGE `creatureid` `spawnid` INT(10) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `creature_waypoints` CHANGE `x` `position_x` float NOT NULL DEFAULT '0';
ALTER TABLE `creature_waypoints` CHANGE `y` `position_y` float NOT NULL DEFAULT '0';
ALTER TABLE `creature_waypoints` CHANGE `z` `position_z` float NOT NULL DEFAULT '0';

-- gameobject_quest_item_binding
ALTER TABLE `gameobject_quest_item_binding` CHANGE `gameobject_entry` `entry` INT(11) DEFAULT '0' NOT NULL;
ALTER TABLE `gameobject_quest_item_binding` CHANGE `quest_id` `quest` INT(11) NOT NULL DEFAULT '0';
ALTER TABLE `gameobject_quest_item_binding` CHANGE `item_id` `item` INT(11) NOT NULL DEFAULT '0';

-- gameobject_quest_pickup_binding
ALTER TABLE `gameobject_quest_pickup_binding` CHANGE `gameobject_entry` `entry` INT(11) NOT NULL DEFAULT '0';
ALTER TABLE `gameobject_quest_pickup_binding` CHANGE `quest_id` `quest` INT(11) NOT NULL DEFAULT '0';

-- gameobject_spawns
ALTER TABLE `gameobject_spawns` CHANGE `x` `position_x` float NOT NULL DEFAULT '0';
ALTER TABLE `gameobject_spawns` CHANGE `y` `position_y` float NOT NULL DEFAULT '0';
ALTER TABLE `gameobject_spawns` CHANGE `z` `position_z` float NOT NULL DEFAULT '0';
ALTER TABLE `gameobject_spawns` CHANGE `o1` `orientation1` float NOT NULL DEFAULT '0';
ALTER TABLE `gameobject_spawns` CHANGE `o2` `orientation2` float NOT NULL DEFAULT '0';
ALTER TABLE `gameobject_spawns` CHANGE `o3` `orientation3` float NOT NULL DEFAULT '0';
ALTER TABLE `gameobject_spawns` CHANGE `o4` `orientation4` float NOT NULL DEFAULT '0';
ALTER TABLE `gameobject_spawns` CHANGE `Map` `map` int(10) unsigned NOT NULL DEFAULT '0';

-- graveyards
ALTER TABLE `graveyards` CHANGE `o` `orientation` float NOT NULL DEFAULT '0';
ALTER TABLE `graveyards` CHANGE `z` `position_z` float NOT NULL DEFAULT '0';
ALTER TABLE `graveyards` CHANGE `y` `position_y` float NOT NULL DEFAULT '0';
ALTER TABLE `graveyards` CHANGE `x` `position_x` float NOT NULL DEFAULT '0';
ALTER TABLE `graveyards` CHANGE `factionid` `faction` INT(10) UNSIGNED NOT NULL DEFAULT '0';

-- npc_gossip_textid
ALTER TABLE `npc_gossip_textid` CHANGE `entryid` `creatureid` INT(10) UNSIGNED NOT NULL DEFAULT '0';

-- quests
ALTER TABLE `quests` CHANGE `QuestSort` `sort` INT(10) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `quests` CHANGE `QuestFlags` `flags` INT(10) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `quests` CHANGE `MaxLevel` `questlevel` INT(10) UNSIGNED DEFAULT '0' NOT NULL;

-- recall
ALTER TABLE `recall` CHANGE `locname` `name` varchar(100) NOT NULL;

-- teleport_coords
ALTER TABLE `teleport_coords` CHANGE `x` `position_x` float NOT NULL DEFAULT '0';
ALTER TABLE `teleport_coords` CHANGE `y` `position_y` float NOT NULL DEFAULT '0';
ALTER TABLE `teleport_coords` CHANGE `z` `position_z` float NOT NULL DEFAULT '0';

-- totemspells
ALTER TABLE `totemspells` CHANGE `spellId` `spell` int(10) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `totemspells` CHANGE `spellToCast1` `castspell1` int(10) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `totemspells` CHANGE `spellToCast2` `castspell2` int(10) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `totemspells` CHANGE `spellToCast3` `castspell3` int(10) UNSIGNED NOT NULL DEFAULT '0';

-- vendors
ALTER TABLE `vendors` CHANGE `vendorGuid` `entry` INT(10) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `vendors` CHANGE `itemGuid` `item` INT(10) UNSIGNED NOT NULL DEFAULT '0';

-- zoneguards
ALTER TABLE `zoneguards` CHANGE `zoneId` `zone` INT(10) UNSIGNED NOT NULL;
ALTER TABLE `zoneguards` CHANGE `hordeEntry` `horde_entry` INT(10) UNSIGNED NULL;
ALTER TABLE `zoneguards` CHANGE `allianceEntry` `alliance_entry` INT(10) UNSIGNED NULL;