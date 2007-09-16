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
void WorldSession::HandleAreaTriggerOpcode(WorldPacket & recv_data)
{
	 if(!_player->IsInWorld()) return;
	CHECK_PACKET_SIZE(recv_data, 4);
	uint32 id ;
	recv_data >> id;
	_HandleAreaTriggerOpcode(id);
}

void WorldSession::_HandleAreaTriggerOpcode(uint32 id)
{		
	sLog.outDebug("AreaTrigger: %u", id);

	AreaTrigger * pAreaTrigger = AreaTriggerStorage.LookupEntry(id);

	// Are we REALLY here?
	if(pAreaTrigger)
	{
		LocationVector l(pAreaTrigger->x, pAreaTrigger->y, pAreaTrigger->z);
		if((_player->GetMapId() == pAreaTrigger->Mapid) && _player->CalcDistance(l) > sWorld.GetUpdateDistance())
			return;
	}

    // Search quest log, find any exploration quests
	sQuestMgr.OnPlayerExploreArea(GetPlayer(),id);
	
	// if in BG handle is triggers
	if(_player->m_bg)
	{
		_player->m_bg->HookOnAreaTrigger(_player, id);
		return;
	}

	if(GetPermissionCount())
	{
		sChatHandler.BlueSystemMessage(this, "[%sSystem%s] |rEntered areatrigger: %s%u.", MSG_COLOR_WHITE, MSG_COLOR_LIGHTBLUE, 
			MSG_COLOR_SUBWHITE, id);
	}

	/* if we don't have an areatrigger, create one on the stack to use for gm scripts :p */
	if(!pAreaTrigger)
	{
		AreaTrigger tmpTrigger;
		tmpTrigger.AreaTriggerID = id;

		ScriptSystem->OnActivateAreaTrigger(&tmpTrigger, _player);
		return;
	}

	/* script prerequsites */
	if(ScriptSystem->OnActivateAreaTrigger(pAreaTrigger, _player) == false)
		return;

	if(pAreaTrigger->Type == ATTYPE_BATTLEGROUND)
	{
		if(pAreaTrigger->Mapid == 489)		// hack fix
			pAreaTrigger->Mapid = 2;
		else if(pAreaTrigger->Mapid == 529)
			pAreaTrigger->Mapid = 3;
		else if(pAreaTrigger->Mapid == 30)
			pAreaTrigger->Mapid = 1;
			
		/*WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundListPacket(GetPlayer()->GetGUID(), _player,
			pAreaTrigger->Mapid);
		SendPacket(pkt);
		delete pkt;
		return;*/
	}

	bool bFailedPre = false;
	std::string failed_reason;

	if(pAreaTrigger->required_level)
	{
		if(GetPlayer()->getLevel() < pAreaTrigger->required_level)
		{
			bFailedPre = true;
			if(failed_reason.size() > 0)
				failed_reason += ", and ";
			else
				failed_reason = "You must be ";

			// mm hacky
			char lvltext[30];
			snprintf(lvltext, 30, "at least level %d", (int)pAreaTrigger->required_level);
			failed_reason += lvltext;
		}
	}
	if(bFailedPre)
	{
		failed_reason += " before you're allowed through here.";
		WorldPacket msg(4 + 1 + 1 + failed_reason.size());
		msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
		msg << uint32(0) << failed_reason << uint8(0);
		SendPacket(&msg);
		sLog.outDebug("Player %s failed area trigger prereq - %s", GetPlayer()->GetName(), failed_reason.c_str());
		return;
	}
	switch(pAreaTrigger->Type)
	{
	case ATTYPE_INSTANCE:
		{
			if(GetPlayer()->GetPlayerStatus() != TRANSFER_PENDING) //only ports if player is out of pendings
			{
				GetPlayer()->SaveEntryPoint(pAreaTrigger->Mapid);

				//death system check.
				Corpse *pCorpse = NULL;
//					CorpseData *pCorpseData = NULL;
				MapInfo *pMapinfo = NULL;

				pMapinfo = WorldMapInfoStorage.LookupEntry(pAreaTrigger->Mapid);
				if(pMapinfo && !pMapinfo->HasFlag(WMI_INSTANCE_ENABLED))
				{
					WorldPacket msg(47);
					msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
					msg << uint32(0) << "This instance is currently unavailable." << uint8(0) << uint8(0);
					SendPacket(&msg);
					return;
				}

				if(pMapinfo && pMapinfo->HasFlag(WMI_INSTANCE_XPACK_01) && !HasFlag(ACCOUNT_FLAG_XPACK_01))
				{
					WorldPacket msg(72);
					msg.Initialize(SMSG_BROADCAST_MSG);
					msg << uint32(3) << "You must have The Burning Crusade Expansion to access this content." << uint8(0);
					SendPacket(&msg);
					return;
				}

                if(pMapinfo && pMapinfo->type != INSTANCE_MULTIMODE && GetPlayer()->iInstanceType == MODE_HEROIC && pMapinfo->type != INSTANCE_NULL)
                {
					WorldPacket msg(54);
					msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
					msg << uint32(0) << "Heroic mode is not available for this instance." << uint8(0) << uint8(0);
					SendPacket(&msg);
					return;
				}

				if(pMapinfo && pMapinfo->type == INSTANCE_RAID && GetPlayer()->InGroup() && GetPlayer()->GetGroup()->GetGroupType() != GROUP_TYPE_RAID)
				{
					WorldPacket msg(72);
					msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
					msg << uint32(0) << "You need to be in a raid group to be able to enter this instance." << uint8(0) << uint8(0);
					SendPacket(&msg);
					return;
				}

				if(pMapinfo && pMapinfo->type == INSTANCE_RAID && !GetPlayer()->InGroup())
				{
					WorldPacket msg(72);
					msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
					msg << uint32(0) << "You need to be in a raid group to be able to enter this instance." << uint8(0) << uint8(0);
					SendPacket(&msg);
					return;
				}

				if(pMapinfo && pMapinfo->required_quest && !_player->HasFinishedQuest(pMapinfo->required_quest))
				{
					WorldPacket msg( 68 );
					msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
					msg << uint32(0) << "You do not have the required attunement to enter this instance.";
					SendPacket(&msg);
					return;
				}

				if(pMapinfo && pMapinfo->required_item && !_player->GetItemInterface()->GetItemCount(pMapinfo->required_item, true))
				{
					WorldPacket msg(68);
					msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
					msg << uint32(0) << "You do not have the required attunement to enter this instance.";
					SendPacket(&msg);
					return;
				}

				if(!GetPlayer()->isAlive())
				{
					pCorpse = objmgr.GetCorpseByOwner(GetPlayer()->GetGUIDLow());
					if(pCorpse)
					{
						pMapinfo = WorldMapInfoStorage.LookupEntry(pCorpse->GetMapId());
						if(pMapinfo)
						{
                            if(GetPlayer()->InGroup())
							{
								MapMgr * groupinstance = sWorldCreator.GetInstanceByGroup(GetPlayer()->GetGroup(), GetPlayer(), pMapinfo);
								if (groupinstance)
								{
									if(groupinstance->GetPlayerCount() >= pMapinfo->playerlimit)
                                    {
                                        WorldPacket data(4);
                                        data.Initialize(SMSG_TRANSFER_ABORTED);
								        data << uint32(INSTANCE_ABORT_FULL);
								        _player->GetSession()->SendPacket(&data);
                                        GetPlayer()->RepopAtGraveyard(GetPlayer()->GetPositionX(),GetPlayer()->GetPositionY(),GetPlayer()->GetPositionZ(), GetPlayer()->GetMapId());
                                        GetPlayer()->ResurrectPlayer();
								        return;
                                    }
                                }
							}

                            //if its a raid instance and corpse is inside and player is not in a group, ressurect
							if(pMapinfo->type != INSTANCE_NULL && pMapinfo->type != INSTANCE_PVP  && pMapinfo->type != INSTANCE_NONRAID && pMapinfo->type != INSTANCE_MULTIMODE && GetPlayer()->GetMapId() != pCorpse->GetMapId() && pCorpse->GetMapId() == pAreaTrigger->Mapid  && !GetPlayer()->InGroup())
							{
								GetPlayer()->ResurrectPlayer();
								return;
							}
                            //if its a instance and player is trying to enter when corpse is on a diferent instance, repop back
                            else if(pMapinfo->type != INSTANCE_NULL && pMapinfo->type != INSTANCE_PVP &&  pCorpse->GetMapId() != pAreaTrigger->Mapid)
							{
								GetPlayer()->RepopAtGraveyard(GetPlayer()->GetPositionX(),GetPlayer()->GetPositionY(),GetPlayer()->GetPositionZ(), GetPlayer()->GetMapId());
								return;
							}
						}
					}
					else
					{
						GetPlayer()->RepopAtGraveyard(GetPlayer()->GetPositionX(),GetPlayer()->GetPositionY(),GetPlayer()->GetPositionZ(),GetPlayer()->GetMapId());
						return;
					}
				}
				bool result = sWorldCreator.CheckInstanceForObject(static_cast<Object*>(GetPlayer()), pMapinfo);
				if(result)
				{
					GetPlayer()->SaveEntryPoint(pAreaTrigger->Mapid);
					GetPlayer()->SafeTeleport(pAreaTrigger->Mapid, 0, LocationVector(pAreaTrigger->x, pAreaTrigger->y, pAreaTrigger->z, pAreaTrigger->o));
				}
			}
		}break;
	case ATTYPE_QUESTTRIGGER:
		{

		}break;
	case ATTYPE_INN:
		{
			// Inn
			if (!GetPlayer()->m_isResting) GetPlayer()->ApplyPlayerRestState(true);
		}break;
	case ATTYPE_TELEPORT:
		{
			if(GetPlayer()->GetPlayerStatus() != TRANSFER_PENDING) //only ports if player is out of pendings
			{
				GetPlayer()->SaveEntryPoint(pAreaTrigger->Mapid);
				GetPlayer()->SafeTeleport(pAreaTrigger->Mapid, 0, LocationVector(pAreaTrigger->x, pAreaTrigger->y, pAreaTrigger->z, pAreaTrigger->o));
			}
		}break;
	case ATTYPE_NULL:
		{
			MapInfo *pMapinfo = NULL;
			pMapinfo = WorldMapInfoStorage.LookupEntry(pAreaTrigger->Mapid);
			if(pMapinfo && pMapinfo->HasFlag(WMI_INSTANCE_XPACK_01) && !HasFlag(ACCOUNT_FLAG_XPACK_01))
			{
				WorldPacket msg(73);
				msg.Initialize(SMSG_BROADCAST_MSG);
				msg << uint32(3) << "You must have The Burning Crusade Expansion to access this content." << uint8(0);
				SendPacket(&msg);
				return;
			}
		}
	default:break;
	}
}
