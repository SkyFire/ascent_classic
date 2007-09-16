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

// You need a line like this for every DBC store. If you use createDBCStore (no Indexed), the lines will be ordered the way they are in the file
// SpellEntry is the file struct entry (for Spell.dbc here).
implementIndexedDBCStore(ItemSetStore,ItemSetEntry)
implementIndexedDBCStore(LockStore,Lock)
implementIndexedDBCStore(SpellStore,SpellEntry)
implementIndexedDBCStore(DurationStore,SpellDuration)
implementIndexedDBCStore(RangeStore,SpellRange)
implementIndexedDBCStore(EmoteStore,emoteentry)
implementIndexedDBCStore(RadiusStore,SpellRadius)
implementIndexedDBCStore(CastTimeStore,SpellCastTime)
implementIndexedDBCStore(AreaStore,AreaTable)
implementIndexedDBCStore(FactionTmpStore,FactionTemplateDBC)
implementIndexedDBCStore(RandomPropStore,RandomProps)
implementIndexedDBCStore(FactionStore,FactionDBC)
implementIndexedDBCStore(EnchantStore,EnchantEntry)
implementIndexedDBCStore(WorldMapAreaStore,WorldMapArea)
implementDBCStore(WorldMapOverlayStore,WorldMapOverlay)
implementDBCStore(SkillStore,skilllinespell)
implementIndexedDBCStore(SkillLineStore,skilllineentry)
implementDBCStore(TaxiPathStore,DBCTaxiPath)
implementDBCStore(TaxiNodeStore,DBCTaxiNode)
implementDBCStore(TaxiPathNodeStore,DBCTaxiPathNode)
implementDBCStore(WorldSafeLocsStore,GraveyardEntry)
implementIndexedDBCStore(TransportAnimationStore,TransportAnimation);
implementDBCStore(NameGenStore, NameGenEntry);
implementIndexedDBCStore(AuctionHouseStore,AuctionHouseDBC);
implementDBCStore(TalentStore, TalentEntry);
implementIndexedDBCStore(CreatureSpellDataStore, CreatureSpellDataEntry);
implementIndexedDBCStore(CreatureFamilyStore, CreatureFamilyEntry);
implementIndexedDBCStore(CharClassStore, CharClassEntry);
implementIndexedDBCStore(CharRaceStore, CharRaceEntry);
implementIndexedDBCStore(MapStore, MapEntry);
implementIndexedDBCStore(ItemExtendedCostStore,ItemExtendedCostEntry);
implementIndexedDBCStore(GemPropertiesStore,GemPropertyEntry);

