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

#ifndef WOWSERVER_ITEM_H
#define WOWSERVER_ITEM_H

#include "Skill.h"

enum Item_quality_names
{
	ITEM_QUALITY_POOR_GREY				= 0, 
	ITEM_QUALITY_NORMAL_WHITE			= 1,
	ITEM_QUALITY_UNCOMMON_GREEN			= 2, 
	ITEM_QUALITY_RARE_BLUE				= 3,
	ITEM_QUALITY_EPIC_PURPLE			= 4, 
	ITEM_QUALITY_LEGENDARY_ORANGE		= 5, 
	ITEM_QUALITY_ARTIFACT_LIGHT_YELLOW	= 6,
};

struct EnchantmentInstance
{
	EnchantEntry * Enchantment;
	bool BonusApplied;
	uint32 Slot;
	time_t ApplyTime;
	uint32 Duration;
	bool RemoveAtLogout;
};

enum ITEM_FLAGS
{
    ITEM_SOULBOUND = 0x01,
    ITEM_QUESTITEM = 0x800,
};

typedef map<uint32, EnchantmentInstance> EnchantmentMap;

#define APPLY true
#define REMOVE false

class SERVER_DECL Item : public Object
{
public:
	Item ( );
	Item(uint32 high,uint32 low);
	virtual ~Item();

	void Create( uint32 itemid, Player* owner );

	inline ItemPrototype* GetProto() const { return m_itemProto; }

	inline Player* GetOwner() const { return m_owner; }
	void SetOwner(Player *owner);

	inline bool IsContainer(){return (m_objectTypeId == TYPEID_CONTAINER) ? true : false; }
	
	//! DB Serialization
	void LoadFromDB(Field *fields, Player* plr, bool light);
	void SaveToDB(int8 containerslot, int8 slot, bool firstsave = false);
	bool LoadAuctionItemFromDB(uint64 guid);
	void DeleteFromDB();
	
	inline void SoulBind()
	{
		this->SetFlag(ITEM_FIELD_FLAGS,1);
	}
	inline bool IsSoulbound()
	{
		return HasFlag(ITEM_FIELD_FLAGS, 1);
	}

	inline uint32 GetChargesLeft()
	{
		for(uint32 x=0;x<5;x++)
			if(m_itemProto->Spells[x].Id)
				return GetUInt32Value(ITEM_FIELD_SPELL_CHARGES+x);

		return 0;
	}

	inline uint32 GetEnchantmentApplytime(uint32 slot)
	{
		EnchantmentMap::iterator itr = Enchantments.find(slot);
		if(itr == Enchantments.end()) return 0;
		else return itr->second.ApplyTime;
	}

	//! Adds an enchantment to the item.
	int32 AddEnchantment(EnchantEntry * Enchantment, uint32 Duration, bool Perm = false,
		bool apply = true, bool RemoveAtLogout = false,uint32 Slot_ = 0);
	uint32 GetSocketsCount();


	//! Removes an enchantment from the item.
	void RemoveEnchantment(uint32 EnchantmentSlot);
	
	// Removes related temporary enchants
	void RemoveRelatedEnchants(EnchantEntry * newEnchant);

	//! Adds the bonus on an enchanted item.
	void ApplyEnchantmentBonus(uint32 Slot, bool Apply);

	//! Applies all enchantment bonuses (use on equip)
	void ApplyEnchantmentBonuses();

	//! Removes all enchantment bonuses (use on dequip)
	void RemoveEnchantmentBonuses();

	//! Event to remove an enchantment.
	void EventRemoveEnchantment(uint32 Slot);

	//! Check if we have an enchantment of this id?
	int32 HasEnchantment(uint32 Id);

	//! Modify the time of an existing enchantment.
	void ModifyEnchantmentTime(uint32 Slot, uint32 Duration);

	//! Find free enchantment slot.
	int32 FindFreeEnchantSlot(EnchantEntry * Enchantment);

	//! Removes all enchantments.
	void RemoveAllEnchantments(bool OnlyTemporary);

	//! Sends SMSG_ITEM_UPDATE_ENCHANT_TIME
	void SendEnchantTimeUpdate(uint32 Slot, uint32 Duration);

	//! Applies any random properties the item has.
	void ApplyRandomProperties();

	void RemoveProfessionEnchant();
	void RemoveSocketBonusEnchant();

	inline void SetCount(uint32 amt) { SetUInt32Value(ITEM_FIELD_STACK_COUNT,amt); }
	inline void SetDurability(uint32 Value) { SetUInt32Value(ITEM_FIELD_DURABILITY,Value); };
	inline void SetDurabilityToMax() { SetUInt32Value(ITEM_FIELD_DURABILITY,GetUInt32Value(ITEM_FIELD_MAXDURABILITY)); }
	inline uint32 GetDurability() { return GetUInt32Value(ITEM_FIELD_DURABILITY); }
	inline uint32 GetDurabilityMax() { return GetUInt32Value(ITEM_FIELD_MAXDURABILITY); }
	inline bool IsAmmoBag() { return (m_itemProto->Class==ITEM_CLASS_QUIVER); }

	void RemoveFromWorld();

	Loot *loot;
	bool locked;
	bool m_isDirty;

	EnchantmentInstance * GetEnchantment(uint32 slot);
	bool IsGemRelated(EnchantEntry * Enchantment);

protected:

	ItemPrototype *m_itemProto;
	EnchantmentMap Enchantments;
	uint32 _fields[ITEM_END];//this mem is wasted in case of container... but this will be fixed in future

	Player *m_owner; // let's not bother the manager with unneeded requests
};

uint32 GetSkillByProto(uint32,uint32);

uint32 GetSellPriceForItem(ItemPrototype *proto, uint32 count);
uint32 GetBuyPriceForItem(ItemPrototype *proto, uint32 count, uint32 vendorcount);

uint32 GetSellPriceForItem(uint32 itemid, uint32 count);
uint32 GetBuyPriceForItem(uint32 itemid, uint32 count, uint32 vendorcount);

#endif
