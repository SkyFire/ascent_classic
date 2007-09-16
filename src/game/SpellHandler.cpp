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

void WorldSession::HandleUseItemOpcode(WorldPacket& recvPacket)
{
	if(!_player->IsInWorld()) return;
	typedef std::list<Aura*> AuraList;
	
	Player* p_User = GetPlayer();
	sLog.outDetail("WORLD: got use Item packet, data length = %i",recvPacket.size());
	int8 tmp1,slot,tmp3;
	uint32 spellId = 0;

	recvPacket >> tmp1 >> slot >> tmp3;
	Item* tmpItem = NULL;
	tmpItem = p_User->GetItemInterface()->GetInventoryItem(tmp1,slot);
	if (!tmpItem)
		tmpItem = p_User->GetItemInterface()->GetInventoryItem(slot);
	if (!tmpItem)
		return;
	ItemPrototype *itemProto = tmpItem->GetProto();
	if(!itemProto)
		return;

	if(itemProto->Bonding == ITEM_BIND_ON_USE)
		tmpItem->SoulBind();

	if(itemProto->QuestId)
	{
		// Item Starter
		Quest *qst = QuestStorage.LookupEntry(itemProto->QuestId);
		if(!qst) 
			return;

        WorldPacket data;
        sQuestMgr.BuildQuestDetails(&data, qst, tmpItem, 0);
		SendPacket(&data);
	}
	
	SpellCastTargets targets(recvPacket, _player->GetGUID());
	for(int x = 0; x < 5; x++)
	{
		if(itemProto->Spells[x].Trigger == USE)
		{
			if(itemProto->Spells[x].Id)
			{
				spellId = itemProto->Spells[x].Id;
				break;
			}
		}
	}
	if(!spellId)
		return;
	// check for spell id
	SpellEntry *spellInfo = sSpellStore.LookupEntry( spellId );

	if(!spellInfo)
	{
		sLog.outError("WORLD: unknown spell id %i\n", spellId);
		return;
	}

	if (spellInfo->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP)
	{
		if (p_User->isInCombat())
		{
			_player->GetItemInterface()->BuildInventoryChangeError(tmpItem,NULL,INV_ERR_CANT_DO_IN_COMBAT);
			return;
		}
		if(p_User->GetStandState()!=1)
		p_User->SetStandState(STANDSTATE_SIT);
		// loop through the auras and removing existing eating spells
	}
	
	if (spellInfo->Category > 1) // 0 and -1 a.k.a. infinite not included
	{
		ItemCooldown * item = new ItemCooldown;
	   
		for(uint32 i = 0; i < 5; i++)
		{
			if(itemProto->Spells[i].Id)
			{
				if(itemProto->Spells[i].Trigger == USE)
				{
					item->ItemEntry = itemProto->ItemId;
					item->SpellID = spellId;
					item->SpellCategory = itemProto->Spells[i].Category;
					if(itemProto->Spells[i].Category)
					{
						item->Cooldown = itemProto->Spells[i].CategoryCooldown;
						item->CooldownTimeStamp = now() + itemProto->Spells[i].CategoryCooldown;
					}
					else
					{
						item->Cooldown = itemProto->Spells[i].Cooldown;
						item->CooldownTimeStamp = now() + itemProto->Spells[i].Cooldown;
					}
				}
			}
		}
		_player->m_itemcooldown.insert(item);
	}



	Spell *spell = new Spell(_player, spellInfo, false, NULL);
	spell->i_caster = tmpItem;
	//GetPlayer()->setCurrentSpell(spell);
	spell->prepare(&targets);
}

void WorldSession::HandleCastSpellOpcode(WorldPacket& recvPacket)
{
	if(!_player->IsInWorld()) return;
	uint32 spellId;

	recvPacket >> spellId;

	sLog.outDetail("WORLD: got cast spell packet, spellId - %i, data length = %i",
		spellId, recvPacket.size());
	
	// Cheat Detection only if player and not from an item
	// this could fuck up things but meh it's needed ALOT of the newbs are using WPE now
	// WPE allows them to mod the outgoing packet and basicly choose what ever spell they want :(

	if( !GetPlayer()->HasSpell(spellId) )
	{
		sLog.outDetail("WORLD: Spell isn't casted because player \"%s\" is cheating", GetPlayer()->GetName());
		return;
	}

	if (GetPlayer()->GetOnMeleeSpell() != spellId)
	{
		// check for spell id
		SpellEntry *spellInfo = sSpellStore.LookupEntry(spellId );

		if(!spellInfo || !sHookInterface.OnCastSpell(_player, spellInfo))
		{
			sLog.outError("WORLD: unknown spell id %i\n", spellId);
			return;
		}
		//autoshot 75
		if((spellInfo->Flags3 & FLAGS3_ACTIVATE_AUTO_SHOT) /*spellInfo->Attributes == 327698*/)	// auto shot..
		{
			Item *weapon = GetPlayer()->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
			if(!weapon) 
				return;
			uint32 spellid;
			switch(weapon->GetProto()->SubClass)
			{
			case 2:			 // bows
			case 3:			 // guns
            case 18:		 // crossbow
				spellid = SPELL_RANGED_GENERAL;
				break;
			case 16:			// thrown
				spellid = SPELL_RANGED_THROW;
				break;
			case 19:			// wands
				spellid = SPELL_RANGED_WAND;
				break;
			default:
				spellid = 0;
				break;
			}
		   
			if(!spellid) 
				spellid = spellInfo->Id;
			
			if(!_player->m_onAutoShot)
			{
				if(_player->IsMounted())
				{
                    _player->SendCastResult(spellInfo->Id, SPELL_FAILED_NOT_MOUNTED, 0);
					return;
				}
				_player->m_AutoShotStartX = _player->GetPositionX();
				_player->m_AutoShotStartY = _player->GetPositionY();
				_player->m_AutoShotStartZ = _player->GetPositionZ();
				_player->m_AutoShotTarget = _player->GetSelection();
				uint32 duration = _player->GetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME);
				SpellCastTargets targets(recvPacket,GetPlayer()->GetGUID());
				if(!targets.m_unitTarget)
					return;
				SpellEntry *sp = sSpellStore.LookupEntry(spellid);
			
				_player->m_AutoShotSpell = sp;
				_player->m_AutoShotDuration = duration;
				//This will fix fast clicks
				if(_player->m_AutoShotAttackTimer < 500)
					_player->m_AutoShotAttackTimer = 500;
				_player->m_onAutoShot = true;
			}

			return;
		}
		/*const char * name = sSpellStore.LookupString(spellInfo->Name);
		if(name)
			sChatHandler.SystemMessageToPlr(_player, "%sSpell Cast:%s %s %s[Group %u, family %u]", MSG_COLOR_LIGHTBLUE,
			MSG_COLOR_SUBWHITE, name, MSG_COLOR_YELLOW, spellInfo->SpellGroupType, spellInfo->SpellFamilyName);*/

        if(GetPlayer()->m_currentSpell && GetCastTime(sCastTime.LookupEntry(spellInfo->CastingTimeIndex)))
        {
            _player->SendCastResult(spellInfo->Id, SPELL_FAILED_SPELL_IN_PROGRESS, 0);
            return;
        }
		Spell *spell = new Spell(GetPlayer(), spellInfo, false, NULL);
	
		SpellCastTargets targets(recvPacket,GetPlayer()->GetGUID());
		spell->prepare(&targets);
	}
}

void WorldSession::HandleCancelCastOpcode(WorldPacket& recvPacket)
{
	uint32 spellId;
	recvPacket >> spellId;

	if(GetPlayer()->m_currentSpell)
		GetPlayer()->m_currentSpell->cancel();
}

void WorldSession::HandleCancelAuraOpcode( WorldPacket& recvPacket)
{
	uint32 spellId;
	recvPacket >> spellId;
	
	for(uint32 x = 0; x < MAX_AURAS+MAX_POSITIVE_AURAS; ++x)
	{
		if(_player->m_auras[x] && _player->m_auras[x]->IsPositive() && _player->m_auras[x]->GetSpellId() == spellId)
			_player->m_auras[x]->Remove();
	}
	sLog.outDebug("removing aura %u",spellId);
}

void WorldSession::HandleCancelChannellingOpcode( WorldPacket& recvPacket)
{
	uint32 spellId;
	recvPacket >> spellId;

	Player *plyr = GetPlayer();
	if(!plyr)
		return;
	if(plyr->m_currentSpell)
	{		
		plyr->m_currentSpell->cancel();
	}
}

void WorldSession::HandleCancelAutoRepeatSpellOpcode(WorldPacket& recv_data)
{
	//sLog.outString("Received CMSG_CANCEL_AUTO_REPEAT_SPELL message.");
	//on original we automatically enter combat when creature got close to us
//	GetPlayer()->GetSession()->OutPacket(SMSG_CANCEL_COMBAT);
	GetPlayer()->m_onAutoShot = false;
}

void WorldSession::HandleAddDynamicTargetOpcode(WorldPacket & recvPacket)
{
	uint64 guid;
	uint32 spellid;
	uint8 flags;
	recvPacket >> guid >> spellid >> flags;
	
	SpellEntry * sp = sSpellStore.LookupEntry(spellid);
	// Summoned Elemental's Freeze
    if (spellid == 33395)
    {
        if (!_player->m_Summon)
            return;
    }
    else if (!_player->m_CurrentCharm || guid != _player->m_CurrentCharm->GetGUID())
    {
        return;
    }
	
	/* burlex: this is.. strange */
	SpellCastTargets targets;
	targets.m_targetMask = flags;

	if(flags == 0)
		targets.m_unitTarget = guid;
	else if(flags == 0x02)
	{
		WoWGuid guid;
		recvPacket >> flags;		// skip one byte
		recvPacket >> guid;
		targets.m_unitTarget = guid.GetOldGuid();
	}
	else if(flags == 0x40)
	{
		recvPacket >> flags;		// skip one byte
		recvPacket >> targets.m_destX >> targets.m_destY >> targets.m_destZ;
	}
	if(spellid == 33395)	// Summoned Water Elemental's freeze
	{
		Spell * pSpell = new Spell(_player->m_Summon, sp, false, 0);
		pSpell->prepare(&targets);
	}
	else			// trinket?
	{
		Spell * pSpell = new Spell(_player->m_CurrentCharm, sp, false, 0);
		pSpell->prepare(&targets);
	}
}
