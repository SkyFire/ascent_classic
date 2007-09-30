/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "DBCStores.h"
#include "DataStore.h"
#include "NGLog.h"

DBCStorage<GemPropertyEntry> dbcGemProperty;
DBCStorage<ItemSetEntry> dbcItemSet;
DBCStorage<Lock> dbcLock;
DBCStorage<SpellEntry> dbcSpell;
DBCStorage<SpellDuration> dbcSpellDuration;
DBCStorage<SpellRange> dbcSpellRange;
DBCStorage<emoteentry> dbcEmoteEntry;
DBCStorage<SpellRadius> dbcSpellRadius;
DBCStorage<SpellCastTime> dbcSpellCastTime;
DBCStorage<AreaTable> dbcArea;
DBCStorage<FactionTemplateDBC> dbcFactionTemplate;
DBCStorage<FactionDBC> dbcFaction;
DBCStorage<EnchantEntry> dbcEnchant;
DBCStorage<RandomProps> dbcRandomProps;
DBCStorage<skilllinespell> dbcSkillLineSpell;
DBCStorage<skilllineentry> dbcSkillLine;
DBCStorage<DBCTaxiNode> dbcTaxiNode;
DBCStorage<DBCTaxiPath> dbcTaxiPath;
DBCStorage<DBCTaxiPathNode> dbcTaxiPathNode;
DBCStorage<AuctionHouseDBC> dbcAuctionHouse;
DBCStorage<TalentEntry> dbcTalent;
DBCStorage<CreatureSpellDataEntry> dbcCreatureSpellData;
DBCStorage<CreatureFamilyEntry> dbcCreatureFamily;
DBCStorage<CharClassEntry> dbcCharClass;
DBCStorage<CharRaceEntry> dbcCharRace;
DBCStorage<MapEntry> dbcMap;
DBCStorage<ItemExtendedCostEntry> dbcItemExtendedCost;

const char * ItemSetFormat = "uuxxxxxxxxxxxxxxxuuuuuuuuuxxxxxxxxxuuuuuuuuuuuuuuuuuu";
const char * LockFormat = "uuuuuuxxxuuuuuxxxuuuuuxxxxxxxxxxx";
const char * EmoteEntryFormat = "uxuuuuxuxuxxxxxxxxx";
const char * skilllinespellFormat = "uuuxxxxxuuuuxxu";
const char * EnchantEntrYFormat = "uuuuuuuuuuuuuxxxxxxxxxxxxxxxxxuuuu";
const char * GemPropertyEntryFormat = "uuuuu";
const char * skilllineentrYFormat = "uuuxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char * spellentrYFormat = "uuuuuuuuuuuuuuuuuuuuuuuuuuuuuiuuuuuuuuuufuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuffffffiiiiiiuuuuuuuuuuuuuuufffuuuuuuuuuuuufffuuuuuxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxuuuuuuuuuuifffuuuuuu";
const char * itemextendedcostFormat = "uuuuuuuuuuuuu";
const char * talententryFormat = "uuuuuuuuuxxxxuxxuxxxx";
const char * spellcasttimeFormat = "uuxx";
const char * spellradiusFormat = "ufxf";
const char * spellrangeFormat = "uffxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char * spelldurationFormat = "uuuu";
const char * randompropsFormat = "uxuuuxxxxxxxxxxxxxxxxxxx";
const char * areatableFormat = "uuuuuxxxuxuxxxxxxxxxxxxxxxxxuxxxxxx";
const char * factiontemplatedbcFormat = "uuuuuuuuuuuuuu";
const char * auctionhousedbcFormat = "uuuuxxxxxxxxxxxxxxxxx";
const char * factiondbcFormat = "uiuuuuxxxxiiiixxxxuxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char * dbctaxinodeFormat = "uufffxxxxxxxxxxxxxxxxxuu";
const char * dbctaxipathFormat = "uuuu";
const char * dbctaxipathnodeFormat = "uuuufffuuxx";
const char * creaturespelldataFormat = "uuuuuuuuu";
const char * charraceFormat = "uxuxxxxxuxxxxuxxxxxxxxxxxxxxxxxxxxx";
const char * charclassFormat = "uxuxxxxxxxxxxxxxxxxxxxxx";
const char * creaturefamilyFormat = "ufufuuuuxxxxxxxxxxxxxxxxxx";
const char * mapentryFormat = "uxuxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

template<class T>
bool loader_stub(const char * filename, const char * format, bool ind, T& l)
{
	Log.Notice("DBC", "Loading %s.", filename);
	return l.Load(filename, format, ind);
}

#define LOAD_DBC(filename, format, ind, stor) if(!loader_stub(filename, format, ind, stor)) { return false; } 

bool LoadDBCs()
{
	LOAD_DBC("DBC/ItemSet.dbc", ItemSetFormat, true, dbcItemSet);
	LOAD_DBC("DBC/Lock.dbc", LockFormat, true, dbcLock);
	LOAD_DBC("DBC/EmotesText.dbc", EmoteEntryFormat, true, dbcEmoteEntry);
	LOAD_DBC("DBC/SkillLineAbility.dbc", skilllinespellFormat, false, dbcSkillLineSpell);
	LOAD_DBC("DBC/SpellItemEnchantment.dbc", EnchantEntrYFormat, true, dbcEnchant);
	LOAD_DBC("DBC/GemProperties.dbc", GemPropertyEntryFormat, true, dbcGemProperty);
	LOAD_DBC("DBC/SkillLine.dbc", skilllineentrYFormat, true, dbcSkillLine);
	LOAD_DBC("DBC/Spell.dbc", spellentrYFormat, true, dbcSpell);
	LOAD_DBC("DBC/ItemExtendedCost.dbc", itemextendedcostFormat, true, dbcItemExtendedCost);
	LOAD_DBC("DBC/Talent.dbc", talententryFormat, true, dbcTalent);
	LOAD_DBC("DBC/SpellCastTimes.dbc", spellcasttimeFormat, true, dbcSpellCastTime);
	LOAD_DBC("DBC/SpellRadius.dbc", spellradiusFormat, true, dbcSpellRadius);
	LOAD_DBC("DBC/SpellRange.dbc", spellrangeFormat, true, dbcSpellRange);
	LOAD_DBC("DBC/SpellDuration.dbc", spelldurationFormat, true, dbcSpellDuration);
	LOAD_DBC("DBC/ItemRandomProperties.dbc", randompropsFormat, true, dbcRandomProps);
	LOAD_DBC("DBC/AreaTable.dbc", areatableFormat, true, dbcArea);
	LOAD_DBC("DBC/FactionTemplate.dbc", factiontemplatedbcFormat, true, dbcFactionTemplate);
	LOAD_DBC("DBC/Faction.dbc", factiondbcFormat, true, dbcFaction);
	LOAD_DBC("DBC/TaxiNodes.dbc", dbctaxinodeFormat, false, dbcTaxiNode);
	LOAD_DBC("DBC/TaxiPath.dbc", dbctaxipathFormat, false, dbcTaxiPath);
	LOAD_DBC("DBC/TaxiPathNode.dbc", dbctaxipathnodeFormat, false, dbcTaxiPathNode);
	LOAD_DBC("DBC/CreatureSpellData.dbc", creaturespelldataFormat, true, dbcCreatureSpellData);
	LOAD_DBC("DBC/CreatureFamily.dbc", creaturefamilyFormat, true, dbcCreatureFamily);
	LOAD_DBC("DBC/ChrRaces.dbc", charraceFormat, true, dbcCharRace);
	LOAD_DBC("DBC/ChrClasses.dbc", charclassFormat, true, dbcCharClass);
	LOAD_DBC("DBC/Map.dbc", mapentryFormat, true, dbcMap);
	return true;
}

