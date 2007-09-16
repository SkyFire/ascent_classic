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

#include "StdAfx.h"

uint32 GetGOReqSkill(GameObject * gameObjTarget)  
{
	if(gameObjTarget->GetEntry()==180215)return 300;
	GameObjectInfo*gi= GameObjectNameStorage.LookupEntry(gameObjTarget->GetEntry());
	if(!gi)
	{
		sLog.outError("Unknown go entry=%u",gameObjTarget->GetEntry());
		return 0;
	}

	Lock *lock = sLockStore.LookupEntry( gi->SpellFocus );
	if(!lock) return 0;
	for(uint32 i=0;i<5;i++)
		if(lock->locktype[i] == 2 && lock->minlockskill[i])
		{
			return lock->minlockskill[i];
		}
	return 0;
}

void AddItemFromDisenchant(ItemPrototype *proto,Player*owner)
{
	uint32 count = 0,item = 0;
	uint32 l=proto->ItemLevel;
	if(proto->Quality ==4 && l>=51)
	{
		count=1;
		if (l<61) item=20725;
		else item=22450;
	}
	else
	{
		if(proto->Quality ==2)//green -> dust for armor, essence for weapon
		{
			if(proto->Class==4)//armor->dust
			{
				if(l<=20)item=10940;
				else if(l<=30)item=11083;
				else if(l<=40)item=11137;
				else if(l<=50)item=11176;
				else if(l<=60)item=16204;
				else item=22445;
			}
			else
			{
				if(l<=10)item=10938;
				else if(l<=15)item=10939;
				else if(l<=20)item=10998;
				else if(l<=25)item=11082;
				else if(l<=30)item=11134;
				else if(l<=35)item=11135;
				else if(l<=40)item=11174;
				else if(l<=45)item=11175;
				else if(l<=50)item=16202;
				else if(l<=60)item=16203;
				else if(l<=66) item=22447;
				else item=22446;
			
			}
			count =1+rand()%3; 
		}
		else if(proto->Quality >=3)//blue -> shards
		{
				if(l<=20)item=10978;
				else if(l<=25)item=11084;
				else if(l<=30)item=11138;
				else if(l<=35)item=11139;
				else if(l<=40)item=11177;
				else if(l<=45)item=11178;
				else if(l<=50)item=14343;
				else if(l<=60)item=14344;
				else if(l<=65)item=22448;
				else item=22449;
		
			count = proto->Quality-2+rand()%3;
		}
	}
	Item *add;
	SlotResult slotresult;
	add = owner->GetItemInterface()->FindItemLessMax(item, count, false);
	if (!add)
	{
		slotresult = owner->GetItemInterface()->FindFreeInventorySlot(proto);
		if(!slotresult.Result)
		{
			owner->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
			return;
		}
		Item * it=objmgr.CreateItem(item,owner);  
		it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
		owner->GetItemInterface()->SafeAddItem(it,slotresult.ContainerSlot, slotresult.Slot);
	}
	else
	{
		add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + count);
		add->m_isDirty = true;
	}
}

/*
Dusts usually are extracted from armors, but also occasionally found from weapons. 

Strange Dust - Disenchanted from level 1 to 20 items [10940]
Soul Dust - Disenchanted from level 21 to 30 items [11083]
Vision Dust - Disenchanted from level 31 to 40 items [11137]
Dream Dust - Disenchanted from level 41 to 50 items [11176]
Illusion Dust - Disenchanted from level 51 to 60 items [16204]


Essences usually are extracted from weapons, but also occasionally found from armors. 

Lesser Magic Essence - Disenchanted from level 1 to 10 items [10938]
Greater Magic Essence - Disenchanted from level 11 to 15 items [10939]
Lesser Astral Essence - Disenchanted from level 16 to 20 items [10998]
Greater Astral Essence - Disenchanted from level 21 to 25 items [11082]
Lesser Mystic Essence - Disenchanted from level 26 to 30 items [11134]
Greater Mystic Essence - Disenchanted from level 31 to 35 items [11135]
Lesser Nether Essence - Disenchanted from level 36 to 40 items [11174]
Greater Nether Essence - Disenchanted from level 41 to 45 items [11175]
Lesser Eternal Essence - Disenchanted from level 46 to 50 items [16202]
Greater Eternal Essence - Disenchanted from level 51 to 60 items [16203]


Shards are normally disenchanted from blue or better items, but have a small chance of coming from green items. 

Small Glimmering Shard - Disenchanted from level 1 to 20 items [10978]
Large Glimmering Shard - Disenchanted from level 21 to 25 items [11084]
Small Glowing Shard - Disenchanted from level 26 to 30 items [11138]
Large Glowing Shard - Disenchanted from level 31 to 35 items [11139]
Small Radiant Shard - Disenchanted from level 36 to 40 items [11177]
Large Radiant Shard - Disenchanted from level 41 to 45 items [11178]
Small Brilliant Shard - Disenchanted from level 46 to 50 items [14343]
Large Brilliant Shard - Disenchanted from level 51 to 60 items [14344]

Nexus Crystals are obtained by disenchanting Epic (purple) items, and sometimes come from Rare (blue) items also. 

Nexus Crystal - Disenchanted from level 51-60 items [20725]
*/

void AddItemFromProspecting(uint32 loot_id,Player*owner)
{
	LootStore::iterator tab =lootmgr.ProspectingLoot.find(loot_id);
	if( lootmgr.ProspectingLoot.end()==tab)
		return;
	StoreLootList *list=&(tab->second);

	for(uint32 x =0,pass=0; x<list->count; x++,pass++)
	{
		if(list->items[x].item.itemid)// this check is needed until loot DB is fixed
		{
			if(Rand(list->items[x].chance)) 
		    {
				ItemPrototype *itemproto = ItemPrototypeStorage.LookupEntry(list->items[x].item.itemid);
				if(!itemproto)
					return;
				uint32 count = 1;
				if(list->count != (x - 1))
				{
					for(uint32 z = (x + 1); z < list->count; z++)
						if(itemproto->MaxCount && (count == itemproto->MaxCount))
							break;
						else if(list->items[x].item.itemid == list->items[z].item.itemid && Rand(list->items[x].chance))
							count++;
				}
				Item *add;
				SlotResult slotresult;
				add = owner->GetItemInterface()->FindItemLessMax(list->items[x].item.itemid, count, false);
				if (!add)
				{
					slotresult = owner->GetItemInterface()->FindFreeInventorySlot(itemproto);
					if(!slotresult.Result)
					{
						owner->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
						return;
					}
					Item * it=objmgr.CreateItem(list->items[x].item.itemid,owner);  
					it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
					owner->GetItemInterface()->SafeAddItem(it,slotresult.ContainerSlot, slotresult.Slot);
				}
				else
				{
					add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + count);
					add->m_isDirty = true;
				}
			}
		}
 	}
}
