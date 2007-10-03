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
Item::Item( )//this is called when constructing as container
{
	m_itemProto = NULL;
	m_owner = NULL;
	loot = NULL;
	locked = false;
}

Item::Item(uint32 high,uint32 low)
{
	m_objectTypeId = TYPEID_ITEM;
	m_valuesCount = ITEM_END;
	m_uint32Values = _fields;
	memset(m_uint32Values, 0,(ITEM_END)*sizeof(uint32));
	m_updateMask.SetCount(ITEM_END);
	SetUInt32Value( OBJECT_FIELD_TYPE,TYPE_ITEM|TYPE_OBJECT);
	SetUInt32Value( OBJECT_FIELD_GUID,low);
	SetUInt32Value( OBJECT_FIELD_GUID+1,high);
	m_wowGuid.Init(GetGUID());

	SetFloatValue( OBJECT_FIELD_SCALE_X, 1 );//always 1

	m_itemProto = NULL;
	m_owner = NULL;
	loot = NULL;
	locked = false;
	m_isDirty = true;
}

Item::~Item()
{
	if(loot != NULL)
		delete loot;
	sEventMgr.RemoveEvents(this);

	EnchantmentMap::iterator itr;
	for(itr = Enchantments.begin(); itr != Enchantments.end(); ++itr)
	{
		if(itr->second.Enchantment->type == 0 && itr->second.Slot == 0 && itr->second.ApplyTime == 0 && itr->second.Duration == 0)
		{
			delete itr->second.Enchantment;
			itr->second.Enchantment = NULL;
		}
	}

	if(IsInWorld())
		RemoveFromWorld();
}

void Item::Create( uint32 itemid, Player *owner )
{
	SetUInt32Value( OBJECT_FIELD_ENTRY, itemid );
 
	SetUInt64Value( ITEM_FIELD_OWNER, owner->GetGUID() );
	SetUInt64Value( ITEM_FIELD_CONTAINED, owner->GetGUID() );
	SetUInt32Value( ITEM_FIELD_STACK_COUNT, 1 );

	m_itemProto = ItemPrototypeStorage.LookupEntry( itemid );
	ASSERT(m_itemProto);
	 
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES , m_itemProto->Spells[0].Charges );
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_01 , m_itemProto->Spells[1].Charges );
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_02 , m_itemProto->Spells[2].Charges );
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_03 , m_itemProto->Spells[3].Charges );
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_04 , m_itemProto->Spells[4].Charges );


	SetUInt32Value( ITEM_FIELD_MAXDURABILITY, m_itemProto->MaxDurability);
	SetUInt32Value( ITEM_FIELD_DURABILITY, m_itemProto->MaxDurability);

	m_owner = owner;
	if(m_itemProto->LockId > 1)
		locked = true;
	else
		locked = false;
}

void Item::LoadFromDB(	Field *fields, Player * plr, bool light)
{
	uint32 itemid=fields[2].GetUInt32();
	m_itemProto = ItemPrototypeStorage.LookupEntry( itemid );
	ASSERT(m_itemProto);
	
	if(m_itemProto->LockId > 1)
		locked = true;
	else
		locked = false;
	
	SetUInt32Value( OBJECT_FIELD_ENTRY, itemid );
	m_owner = plr;

  
	SetUInt32Value( ITEM_FIELD_CREATOR, fields[3].GetUInt32() );
	SetUInt32Value( ITEM_FIELD_STACK_COUNT,  fields[4].GetUInt32());
	for(uint32 x=0;x<5;x++)
	if(m_itemProto->Spells[x].Id)
	{
		SetUInt32Value( ITEM_FIELD_SPELL_CHARGES+x , fields[5].GetUInt32() );
		break;
	}
	
	SetUInt32Value( ITEM_FIELD_FLAGS, fields[6].GetUInt32());
	SetUInt32Value( ITEM_FIELD_RANDOM_PROPERTIES_ID, fields[7].GetUInt32());

	SetUInt32Value( ITEM_FIELD_ITEM_TEXT_ID, fields[8].GetUInt32());

	SetUInt32Value( ITEM_FIELD_MAXDURABILITY, m_itemProto->MaxDurability);
	SetUInt32Value( ITEM_FIELD_DURABILITY, fields[9].GetUInt32());

	if(light) return;

	string enchant_field = fields[12].GetString();
	vector<string> enchants = StrSplit(enchant_field, ";");
	uint32 enchant_id;
	EnchantEntry * entry;
	uint32 time_left;
	uint32 enchslot;

	for(vector<string>::iterator itr = enchants.begin(); itr != enchants.end(); ++itr)
	{
		if(sscanf((*itr).c_str(), "%u,%u,%u", (unsigned int*)&enchant_id, (unsigned int*)&time_left, (unsigned int*)&enchslot) == 3)
		{
			entry = dbcEnchant.LookupEntry(enchant_id);
			if(entry && entry->Id == enchant_id)
				AddEnchantment(entry, time_left,
				(time_left==0),false,false,enchslot);
				//(enchslot != 2) ? false : true, false);
			else
			{/*

				EnchantEntry *pEnchant = new EnchantEntry;
				memset(pEnchant,0,sizeof(EnchantEntry));

				pEnchant->Id = enchant_id;
				if(enchslot != 2)
					AddEnchantment(pEnchant,0,true, false);
				else
					AddEnchantment(pEnchant,0,false,false);*/
			}
		}
	}	

	ApplyRandomProperties();

	// Charter stuff
	if(m_uint32Values[OBJECT_FIELD_ENTRY] == ITEM_ENTRY_GUILD_CHARTER)
	{
		SetUInt32Value(ITEM_FIELD_FLAGS, 1);
		SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
		SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, 57813883);
		if(plr->m_charters[CHARTER_TYPE_GUILD])
			SetUInt32Value(ITEM_FIELD_ENCHANTMENT, plr->m_charters[CHARTER_TYPE_GUILD]->GetID());
	}

	if(m_uint32Values[OBJECT_FIELD_ENTRY] == ARENA_TEAM_CHARTER_2v2)
	{
		SetUInt32Value(ITEM_FIELD_FLAGS, 1);
		SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
		SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, 57813883);
		if(plr->m_charters[CHARTER_TYPE_ARENA_2V2])
			SetUInt32Value(ITEM_FIELD_ENCHANTMENT, plr->m_charters[CHARTER_TYPE_ARENA_2V2]->GetID());
	}

	if(m_uint32Values[OBJECT_FIELD_ENTRY] == ARENA_TEAM_CHARTER_3v3)
	{
		SetUInt32Value(ITEM_FIELD_FLAGS, 1);
		SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
		SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, 57813883);
		if(plr->m_charters[CHARTER_TYPE_ARENA_3V3])
			SetUInt32Value(ITEM_FIELD_ENCHANTMENT, plr->m_charters[CHARTER_TYPE_ARENA_3V3]->GetID());
	}

	if(m_uint32Values[OBJECT_FIELD_ENTRY] == ARENA_TEAM_CHARTER_5v5)
	{
		SetUInt32Value(ITEM_FIELD_FLAGS, 1);
		SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
		SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, 57813883);
		if(plr->m_charters[CHARTER_TYPE_ARENA_5V5])
			SetUInt32Value(ITEM_FIELD_ENCHANTMENT, plr->m_charters[CHARTER_TYPE_ARENA_5V5]->GetID());
	}
}

void Item::ApplyRandomProperties()
{
	// apply random properties
	if(m_uint32Values[ITEM_FIELD_RANDOM_PROPERTIES_ID] != 0)
	{
		RandomProps *rp= dbcRandomProps.LookupEntry(m_uint32Values[ITEM_FIELD_RANDOM_PROPERTIES_ID]);
		if(rp)
		{
			for (int k=0;k<3;k++)
			{
				if (rp->spells[k] != 0)
				{
					EnchantEntry * ee = dbcEnchant.LookupEntry(rp->spells[k]);
					if(HasEnchantment(ee->Id) < 0) 
					{
						uint32 Slot = FindFreeEnchantSlot(ee);
						AddEnchantment(ee, 0, false, false, true, Slot);
					}
				}
			}
		}
	}
}

void Item::SaveToDB(int8 containerslot, int8 slot, bool firstsave)
{
	if(!m_isDirty && !firstsave)
		return;

	std::stringstream ss;

	ss << "REPLACE INTO playeritems VALUES(";


	ss << m_uint32Values[ITEM_FIELD_OWNER] << ",";
    ss << GetGUIDLow() << ",";
	ss << m_uint32Values[OBJECT_FIELD_ENTRY] << ",";
	ss << GetUInt32Value(ITEM_FIELD_CREATOR) << ",";
	ss << GetUInt32Value(ITEM_FIELD_STACK_COUNT) << ",";
	ss << GetChargesLeft() << ",";
	ss << GetUInt32Value(ITEM_FIELD_FLAGS) << ",";
	ss << GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID) << ",";
	ss << GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID) << ",";
	ss << GetUInt32Value(ITEM_FIELD_DURABILITY) << ",";
	ss << static_cast<int>(containerslot) << ",";
	ss << static_cast<int>(slot) << ",'";

	// Pack together enchantment fields
	EnchantmentMap::iterator itr = Enchantments.begin();
	for(; itr != Enchantments.end(); ++itr)
	{
		 if(itr->second.RemoveAtLogout)
			   continue;

		uint32 elapsed_duration = time(NULL) - itr->second.ApplyTime;
		int32 remaining_duration = itr->second.Duration - elapsed_duration;
		if(remaining_duration < 0) remaining_duration = 0;
	  /*  if(!itr->second.RemoveAtLogout && 
			(remaining_duration > 5 && itr->second.Slot != 2) || itr->second.Slot == 2)  // no point saving stuff with < 5 seconds... unless is perm enchant
		{
			ss << itr->second.Enchantment->Id << ",";
			ss << remaining_duration << ",";
			ss << itr->second.Slot << ";";
		}*/

		  
		if (itr->second.Enchantment && (remaining_duration && remaining_duration >5) || (itr->second.Duration==0))
		{
			ss << itr->second.Enchantment->Id << ",";
			ss << remaining_duration << ",";
			ss << itr->second.Slot << ";";
		}


	}
	ss << "')";
	
	if(firstsave)
		CharacterDatabase.WaitExecute(ss.str().c_str());
	else
		CharacterDatabase.Execute( ss.str().c_str());

	m_isDirty = false;
}




void Item::DeleteFromDB()
{
	if(m_itemProto->ContainerSlots>0)
	{
		/* deleting a container */
		for(uint32 i = 0; i < m_itemProto->ContainerSlots; ++i)
		{
			if(((Container*)this)->GetItem(i) != NULL)
			{
				/* abort the delete */
				return;
			}
		}
	}

	CharacterDatabase.Execute("DELETE FROM playeritems WHERE guid = %u", GetGUIDLow());
}

uint32 GetSkillByProto(uint32 Class, uint32 SubClass)
{
	const static uint32 arm_skills[7]={0,SKILL_CLOTH,SKILL_LEATHER,SKILL_MAIL,SKILL_PLATE_MAIL,0,SKILL_SHIELD};
   
	const static uint32 weap_skills[21]={SKILL_AXES,SKILL_2H_AXES,SKILL_BOWS,SKILL_GUNS,SKILL_MACES,SKILL_2H_MACES,
				SKILL_POLEARMS,SKILL_SWORDS,SKILL_2H_SWORDS,0,SKILL_STAVES,0,0,SKILL_FIST_WEAPONS,0,//<-13
				SKILL_DAGGERS,SKILL_THROWN,SKILL_SPEARS,SKILL_CROSSBOWS,SKILL_WANDS,SKILL_FISHING};
	//14--used by professions
	if(Class == 4 && SubClass < 7)
	{
		return arm_skills[SubClass];

	} else 
	if(Class == 2)
	{
		if(SubClass < 20)//no skill for fishing
		{
			return weap_skills[SubClass];
		}
	}
	return 0;
}

//This map is used for profess.
//Prof packe strcut: {SMSG_SET_PROFICIENCY,(uint8)item_class,(uint32)1<<item_subclass}
//ie: for fishing (it's class=2--weapon, subclass ==20 -- fishing rod) permissive packet
// will have structure 0x2,524288
//this table is needed to get class/subclass by skill, valid classes are 2 and 4

const ItemProf * GetProficiencyBySkill(uint32 skill)
{
	const static ItemProf prof[22]={
		{4,2},{4,4},{4,8},{4,16},{4,64},
		{2,1},{2,2},{2,4},{2,8},{2,16},{2,32},{2,64},{2,128},{2,256},{2,1024},{2,8192},{2,32768},{2,65536},{2,131072},
		{2,262144},{2,524288},{2,1048576}};

	switch(skill)
	{
		case SKILL_CLOTH:
			return &prof[0];
		case SKILL_LEATHER:
			return &prof[1];
		case SKILL_MAIL:
			return &prof[2];
		case SKILL_PLATE_MAIL:
			return &prof[3];
		case SKILL_SHIELD:
			return &prof[4];

		case SKILL_AXES:
			return &prof[5];
		case SKILL_2H_AXES:
			return &prof[6];
		case SKILL_BOWS:
			return &prof[7];
		case SKILL_GUNS:
			return &prof[8];
		case SKILL_MACES:
			return &prof[9];
		case SKILL_2H_MACES:
			return &prof[10];
		case SKILL_POLEARMS:
			return &prof[11];
		case SKILL_SWORDS:
			return &prof[12];
		case SKILL_2H_SWORDS:
			return &prof[13];
		case SKILL_STAVES:
			return &prof[14];
		case SKILL_FIST_WEAPONS:
			return &prof[15];
		case SKILL_DAGGERS:
			return &prof[16];
		case SKILL_THROWN:
			return &prof[17];
		case SKILL_SPEARS:
			return &prof[18];
		case SKILL_CROSSBOWS:
			return &prof[19];
		case SKILL_WANDS:
			return &prof[20];
		case SKILL_FISHING:
			return &prof[21];
		default:
			return NULL;
	}
}

uint32 GetSellPriceForItem(ItemPrototype *proto, uint32 count)
{
	int32 cost;

	switch(proto->Class)
	{
		case ITEM_CLASS_WEAPON:
		case ITEM_CLASS_CONSUMABLE:
		case ITEM_CLASS_PROJECTILE: // as far as i can tell these can't be sold at all
		{
			cost = (proto->SellPrice * ((count < 1) ? 1 : count)) / proto->MaxCount;
		}break;
	default:
		{
			cost = ( proto->SellPrice * ((count < 1) ? 1 : count) );
		}break;
	}

	return cost;
}

uint32 GetBuyPriceForItem(ItemPrototype *proto, uint32 count, uint32 vendorcount)
{
	int32 cost;

	switch(proto->Class)
	{
		case ITEM_CLASS_WEAPON:
		case ITEM_CLASS_PROJECTILE:
		case ITEM_CLASS_CONSUMABLE:
		{
			cost = ( proto->BuyPrice * ((count < 1) ? 1 : count) ) / proto->MaxCount;
		}break;
	default:
		{
			cost = ( proto->BuyPrice * ((count < 1) ? 1 : count));
		}break;
	}

	return cost;
}

uint32 GetSellPriceForItem(uint32 itemid, uint32 count)
{
	if(ItemPrototype *proto = ItemPrototypeStorage.LookupEntry(itemid))
		return GetSellPriceForItem(proto, count);
	else
		return 1;
}

uint32 GetBuyPriceForItem(uint32 itemid, uint32 count, uint32 vendorcount)
{
	if(ItemPrototype *proto = ItemPrototypeStorage.LookupEntry(itemid))
		return GetBuyPriceForItem(proto, count, vendorcount);
	else
		return 1;
}

void Item::RemoveFromWorld()
{
	// if we have an owner->send destroy
	if(m_owner)
	{
		DestroyForPlayer(m_owner);
	}

	if(!IsInWorld()) return;

	mSemaphoreTeleport = true;
	m_mapMgr->RemoveObject(this, false);
	m_mapMgr = NULL;
  
	// update our event holder
	event_Relocate();
}

void Item::SetOwner(Player *owner)
{ 
	if(owner)
		SetUInt64Value(ITEM_FIELD_OWNER,((Object*)owner)->GetGUID());
	else SetUInt64Value(ITEM_FIELD_OWNER,0);

	m_owner = owner; 
}


int32 Item::AddEnchantment(EnchantEntry * Enchantment, uint32 Duration, bool Perm /* = false */, bool apply /* = true */, bool RemoveAtLogout /* = false */,uint32 Slot_)
{
	int32 Slot = Slot_;
	m_isDirty = true;

/*	if(Perm)
	{
		if(Slot_)
		{
			Slot=Slot_;
		}
		else
        {
			Slot = FindFreeEnchantSlot(Enchantment);
        }
	}
	else
	{
		if(Enchantment->EnchantGroups > 1) // replaceable temp enchants
		{
			Slot = 1;
			RemoveEnchantment(1);
		}
		else
		{
			Slot = FindFreeEnchantSlot(Enchantment);*/
			/*
			Slot = Enchantment->type ? 3 : 0;
			 //that's 's code
			for(uint32 Index = ITEM_FIELD_ENCHANTMENT_09; Index < ITEM_FIELD_ENCHANTMENT_32; Index += 3)
			{
				if(m_uint32Values[Index] == 0) break;;	
				++Slot;
			}

			//Slot = FindFreeEnchantSlot(Enchantment);
			// reach max of temp enchants
			if(Slot >= 11) return -1;
		*/
		/*}
	}   
*/

	// Create the enchantment struct.
	EnchantmentInstance Instance;
	Instance.ApplyTime = time(NULL);
	Instance.BonusApplied = false;
	Instance.Slot = Slot;
	Instance.Enchantment = Enchantment;
	Instance.Duration = Duration;
	Instance.RemoveAtLogout = RemoveAtLogout;

	// Set the enchantment in the item fields.
	uint32 EnchantBase = Slot * 3 + ITEM_FIELD_ENCHANTMENT;
	SetUInt32Value(EnchantBase	, Enchantment->Id);
	SetUInt32Value(EnchantBase + 1, Instance.ApplyTime);
	SetUInt32Value(EnchantBase + 2, 0); // charges

	// Add it to our map.
	Enchantments[Slot] = Instance;

	if(m_owner == 0) return Slot;

	// Add the removal event.
	if(Duration)
	{
		sEventMgr.AddEvent(this, &Item::RemoveEnchantment, uint32(Slot),
			EVENT_REMOVE_ENCHANTMENT1 + Slot, Duration * 1000, 1,0);
	}

	// No need to send the log packet, if the owner isn't in world (we're still loading)
	if(!m_owner->IsInWorld())
		return Slot;

	if(apply)
	{
		WorldPacket EnchantLog(SMSG_ENCHANTMENTLOG, 25);
		EnchantLog << m_owner->GetGUID();
		EnchantLog << m_owner->GetGUID();
		EnchantLog << m_uint32Values[OBJECT_FIELD_ENTRY];
		EnchantLog << Enchantment->Id;
		EnchantLog << uint8(0);
		m_owner->GetSession()->SendPacket(&EnchantLog);

		if(m_owner->GetTradeTarget())
		{
			m_owner->SendTradeUpdate();
		}
	
		/* Only apply the enchantment bonus if we're equipped */
		uint8 slot = m_owner->GetItemInterface()->GetInventorySlotByGuid(GetGUID());
		if(slot > EQUIPMENT_SLOT_START && slot < EQUIPMENT_SLOT_END)
            ApplyEnchantmentBonus(Slot, APPLY);
	}

	return Slot;
}

void Item::RemoveEnchantment(uint32 EnchantmentSlot)
{
	// Make sure we actually exist.
	EnchantmentMap::iterator itr = Enchantments.find(EnchantmentSlot);
	if(itr == Enchantments.end())
		return;

	m_isDirty = true;
	uint32 Slot = itr->first;
	if(itr->second.BonusApplied)
		ApplyEnchantmentBonus(EnchantmentSlot, REMOVE);

	// Unset the item fields.
	uint32 EnchantBase = Slot * 3 + ITEM_FIELD_ENCHANTMENT;
	SetUInt32Value(EnchantBase + 0, 0);
	SetUInt32Value(EnchantBase + 1, 0);
	SetUInt32Value(EnchantBase + 2, 0);

	// Remove the enchantment event for removal.
	event_RemoveEvents(EVENT_REMOVE_ENCHANTMENT1 + Slot);

	// Remove the enchantment instance.
	Enchantments.erase(itr);
}

void Item::ApplyEnchantmentBonus(uint32 Slot, bool Apply)
{
	EnchantmentMap::iterator itr = Enchantments.find(Slot);
	if(itr == Enchantments.end())
		return;

	EnchantEntry * Entry = itr->second.Enchantment;

	if(itr->second.BonusApplied == Apply)
		return;

	itr->second.BonusApplied = Apply;

	if(Apply)
	{
		// Send the enchantment time update packet.
		SendEnchantTimeUpdate(itr->second.Slot, itr->second.Duration);
	}

	// Apply the visual on the player.
	uint32 ItemSlot = m_owner->GetItemInterface()->GetInventorySlotByGuid(GetGUID()) * 16;
	uint32 VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + ItemSlot;
	m_owner->SetUInt32Value(VisibleBase + 1 + Slot, Apply ? Entry->Id : 0);

	for(uint32 c=0;c<3;c++)
	if(Entry->type[c])
	{
		// Depending on the enchantment type, take the appropriate course of action.
		switch(Entry->type[c])
		{
		case 1:		 // Trigger spell on melee attack.
			{
				if(Apply && Entry->spell[c] != 0)
				{
					// Create a proc trigger spell

					ProcTriggerSpell TS;
					TS.caster = m_owner->GetGUID();
					TS.origId = 0;
					TS.procFlags = PROC_ON_MELEE_ATTACK;
					TS.procCharges = 0;
					TS.procChance = Entry->min[c] ? Entry->min[c] : 35;
					TS.deleted = false;
					TS.spellId = Entry->spell[c];
					m_owner->m_procSpells.push_back(TS);
				}
				else
				{
					// Remove the proctriggerspell
					uint32 SpellId;
					list<struct ProcTriggerSpell>::iterator itr/*, itr2*/;
					for(itr = m_owner->m_procSpells.begin();
						itr != m_owner->m_procSpells.end();)
					{
						SpellId = itr->spellId;
						/*itr2 = itr++;*/
						
						if(SpellId == Entry->spell[c] )
						{
							//m_owner->m_procSpells.erase(itr2);
							itr->deleted = true;
						}
						itr++;
					}
				}
			}break;

		case 2:		 // Mod damage done.
			{
				if(Apply)
					m_owner->ModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, Entry->min[c]);
				else m_owner->ModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, -Entry->min[c]);
				m_owner->CalcDamage();
			}break;

		case 3:		 // Cast spell (usually means apply aura)
			{
				if(Apply)
				{
					SpellCastTargets targets(m_owner->GetGUID());
					SpellEntry * sp;
					Spell * spell;
					
					if(Entry->spell[c] != 0)
					{
						sp = dbcSpell.LookupEntry(Entry->spell[c]);
						if(!sp) continue;

						spell = new Spell(m_owner, sp, true, 0);
						spell->prepare(&targets);
					}
				}
				else
				{
					if(Entry->spell[c] != 0)
							m_owner->RemoveAura(Entry->spell[c]);
				}

			}break;

		case 4:		 // Modify physical resistance
			{
				if(Apply)
				{
					m_owner->FlatResistanceModifierPos[0] += Entry->min[c];
				}
				else
				{
					m_owner->FlatResistanceModifierPos[0] -= Entry->min[c];
				}
				m_owner->CalcResistance(0);
			}break;

		case 5:	 //Modify rating ...order is PLAYER_FIELD_COMBAT_RATING_1 and above
			{
				//spellid is enum ITEM_STAT_TYPE
				//min=max is amount
				m_owner->ModifyBonuses(Entry->spell[c],
					Apply ? Entry->min[c] : -Entry->min[c]);
				m_owner->UpdateStats();
			}break;

		case 6:	 // Rockbiter weapon (increase damage per second... how the hell do you calc that)
			{
				if(Apply)
				{
	//				m_owner->ModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, Entry->min[c]);
					//if i'm not wrong then we should apply DMPS formula for this. This will have somewhat a larger value 28->34
					int32 value=GetProto()->Delay*Entry->min[c]/1000;
					m_owner->ModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, value);
				}
				else
				{
					int32 value=-(int32)(GetProto()->Delay*Entry->min[c]/1000);
					m_owner->ModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, value);
				}
				m_owner->CalcDamage();
			}break;

		default:
			{
				sLog.outError("Unknown enchantment type: %u (%u)", Entry->type[c], Entry->Id);
			}break;
		}
	}
}

void Item::ApplyEnchantmentBonuses()
{
	EnchantmentMap::iterator itr;
	for(itr = Enchantments.begin(); itr != Enchantments.end(); ++itr)
	{
		ApplyEnchantmentBonus(itr->first, APPLY);
	}
}

void Item::RemoveEnchantmentBonuses()
{
	EnchantmentMap::iterator itr;
	for(itr = Enchantments.begin(); itr != Enchantments.end(); ++itr)
	{
		ApplyEnchantmentBonus(itr->first, REMOVE);
	}
}

void Item::EventRemoveEnchantment(uint32 Slot)
{
	// Remove the enchantment.
	RemoveEnchantment(Slot);
}

int32 Item::FindFreeEnchantSlot(EnchantEntry * Enchantment)
{	
	//if(!Enchantment) return -1;

   /* uint32 Slot = Enchantment->type ? 3 : 0;
	for(uint32 Index = ITEM_FIELD_ENCHANTMENT_09; Index < ITEM_FIELD_ENCHANTMENT_32; Index += 3)
	{
		if(m_uint32Values[Index] == 0) return Slot;	
		++Slot;
	}*/

	uint32 GemSlotsReserve=GetSocketsCount();
	if(GetProto()->SocketBonus)
		GemSlotsReserve++;
	
	for(uint32 Slot = GemSlotsReserve+2;Slot<11; Slot++)
	{
		if(m_uint32Values[ITEM_FIELD_ENCHANTMENT + Slot*3] == 0) return Slot;	
	}


	return -1;
}

int32 Item::HasEnchantment(uint32 Id)
{
	for(uint32 Slot = 0;Slot<11;Slot++)
	{
		if(m_uint32Values[ITEM_FIELD_ENCHANTMENT+Slot*3] == Id)
			return Slot;
	}

	return -1;
}

void Item::ModifyEnchantmentTime(uint32 Slot, uint32 Duration)
{
	EnchantmentMap::iterator itr = Enchantments.find(Slot);
	if(itr == Enchantments.end())
		return;

	// Reset the apply time.
	itr->second.ApplyTime = time(NULL);
	itr->second.Duration = Duration;

	// Change the event timer.
	event_ModifyTimeAndTimeLeft(EVENT_REMOVE_ENCHANTMENT1 + Slot, Duration * 1000);

	// Send update packet
	SendEnchantTimeUpdate(itr->second.Slot, Duration);
}

void Item::SendEnchantTimeUpdate(uint32 Slot, uint32 Duration)
{
	/*
	{SERVER} Packet: (0x01EB) SMSG_ITEM_ENCHANT_TIME_UPDATE Size = 24
	|------------------------------------------------|----------------|
	|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
	|------------------------------------------------|----------------|
	|69 32 F0 35 00 00 00 40 01 00 00 00 08 07 00 00 |i2.5...@........|
	|51 46 35 00 00 00 00 00						 |QF5.....		|
	-------------------------------------------------------------------

	uint64 item_guid
	uint32 count?
	uint32 time_in_seconds
	uint64 player_guid
	*/

	WorldPacket *data = new WorldPacket(SMSG_ITEM_ENCHANT_TIME_UPDATE, 24);
	*data << GetGUID();
	*data << Slot;
	*data << Duration;
	*data << m_owner->GetGUID();
	m_owner->delayedPackets.add(data);
}

void Item::RemoveAllEnchantments(bool OnlyTemporary)
{
	EnchantmentMap::iterator itr, it2;
	for(itr = Enchantments.begin(); itr != Enchantments.end();)
	{
		it2 = itr++;

		if(OnlyTemporary && it2->second.Duration == 0) 
			continue;
			
		RemoveEnchantment(it2->first);
	}
}

void Item::RemoveRelatedEnchants(EnchantEntry * newEnchant)
{
	EnchantmentMap::iterator itr,itr2;
	for(itr = Enchantments.begin(); itr != Enchantments.end();)
	{
		itr2 = itr++;
		
		if(itr2->second.Enchantment->Id == newEnchant->Id || 
		  (itr2->second.Enchantment->EnchantGroups > 1 && newEnchant->EnchantGroups > 1 ))
		{ 
			RemoveEnchantment(itr2->first);
		}
	}
}

void Item::RemoveProfessionEnchant()
{
	EnchantmentMap::iterator itr;
	for(itr = Enchantments.begin(); itr != Enchantments.end();itr++)
	{
		if(itr->second.Duration!=0)// not perm
			continue;
		if(IsGemRelated(itr->second.Enchantment))
			continue;

		RemoveEnchantment(itr->first);
		return;
	}

}

void Item::RemoveSocketBonusEnchant()
{
	EnchantmentMap::iterator itr;
	
	for(itr = Enchantments.begin(); itr != Enchantments.end();itr++)
	{
		if(itr->second.Enchantment->Id == GetProto()->SocketBonus)
		{
			RemoveEnchantment(itr->first);
			return;
		}	
	}
}

EnchantmentInstance * Item::GetEnchantment(uint32 slot)
{
	EnchantmentMap::iterator itr = Enchantments.find(slot);
	if(itr != Enchantments.end())
		return &itr->second;
	else
		return NULL;
}

bool Item::IsGemRelated(EnchantEntry * Enchantment)
{
	if(GetProto()->SocketBonus == Enchantment->Id)
		return true;
	
	return (Enchantment->GemEntry!=0);
}

uint32 Item::GetSocketsCount()
{
	uint32 c=0;
	for(uint32 x=0;x<3;x++)
	if(GetProto()->Sockets[x].SocketColor)
		c++;
	return c;
}
