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

enum AreaTriggerFailures
{
	AREA_TRIGGER_FAILURE_OK				= 0,
	AREA_TRIGGER_FAILURE_UNAVAILABLE	= 1,
	AREA_TRIGGER_FAILURE_NO_BC			= 2,
	AREA_TRIGGER_FAILURE_NO_HEROIC		= 3,
	AREA_TRIGGER_FAILURE_NO_RAID		= 4,
	AREA_TRIGGER_FAILURE_NO_ATTUNE		= 5,
	AREA_TRIGGER_FAILURE_LEVEL			= 6,
};

const char * AreaTriggerFailureMessages[] = {
	"-",
	"This instance is unavailable",
	"You must have The Burning Crusade Expansion to access this content.",
	"Heroic mode unavailable for this instance.",
	"You must be in a raid group to pass through here.",
	"You do not have the required attunement to pass through here.",
	"You must be at least level %u to pass through here.",
};

inline uint32 CheckTriggerPrerequsites(AreaTrigger * pAreaTrigger, WorldSession * pSession, Player * pPlayer, MapInfo * pMapInfo)
{
	if(pAreaTrigger->required_level && pPlayer->getLevel() < pAreaTrigger->required_level)
		return AREA_TRIGGER_FAILURE_LEVEL;

	if(!pMapInfo || !pMapInfo->HasFlag(WMI_INSTANCE_ENABLED))
		return AREA_TRIGGER_FAILURE_UNAVAILABLE;

	if(!pSession->HasFlag(ACCOUNT_FLAG_XPACK_01) && pMapInfo->HasFlag(WMI_INSTANCE_XPACK_01))
		return AREA_TRIGGER_FAILURE_NO_BC;

	// These can be overridden by cheats/GM
	if(pPlayer->triggerpass_cheat)
		return AREA_TRIGGER_FAILURE_OK;

	if(pPlayer->iInstanceType == MODE_HEROIC && pMapInfo->type != INSTANCE_MULTIMODE)
		return AREA_TRIGGER_FAILURE_NO_HEROIC;

	if((pMapInfo->type == INSTANCE_MULTIMODE || pMapInfo->type == INSTANCE_RAID) && (!pPlayer->GetGroup() || (pPlayer->GetGroup() && pPlayer->GetGroup()->GetGroupType() != GROUP_TYPE_RAID)))
		return AREA_TRIGGER_FAILURE_NO_RAID;

	if(pMapInfo && pMapInfo->required_quest && !pPlayer->HasFinishedQuest(pMapInfo->required_quest))
		return AREA_TRIGGER_FAILURE_NO_ATTUNE;

	if(pMapInfo && pMapInfo->required_item && !pPlayer->GetItemInterface()->GetItemCount(pMapInfo->required_item, true))
		return AREA_TRIGGER_FAILURE_NO_ATTUNE;

	return AREA_TRIGGER_FAILURE_OK;
}

void WorldSession::_HandleAreaTriggerOpcode(uint32 id)
{		
	sLog.outDebug("AreaTrigger: %u", id);

	AreaTrigger * pAreaTrigger = AreaTriggerStorage.LookupEntry(id);

	// Are we REALLY here?
	if(!pAreaTrigger)
		return;

	LocationVector l(pAreaTrigger->x, pAreaTrigger->y, pAreaTrigger->z);
	if((_player->GetMapId() == pAreaTrigger->Mapid) && _player->CalcDistance(l) > sWorld.GetUpdateDistance())
		return;

    // Search quest log, find any exploration quests
	sQuestMgr.OnPlayerExploreArea(GetPlayer(),id);
	
	// if in BG handle is triggers
	if(_player->m_bg)
	{
		_player->m_bg->HookOnAreaTrigger(_player, id);
		return;
	}

	switch(pAreaTrigger->Type)
	{
	case ATTYPE_INSTANCE:
		{
			if(GetPlayer()->GetPlayerStatus() != TRANSFER_PENDING) //only ports if player is out of pendings
			{
				uint32 reason = CheckTriggerPrerequsites(pAreaTrigger, this, _player, WorldMapInfoStorage.LookupEntry(pAreaTrigger->Mapid));
				if(reason != AREA_TRIGGER_FAILURE_OK)
				{
					const char * pReason = AreaTriggerFailureMessages[reason];
					WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 50);
					data << uint32(0);
                    
					if(reason == AREA_TRIGGER_FAILURE_LEVEL)
					{
						// special case
						char msg[50];
						snprintf(msg,50,pReason,pAreaTrigger->required_level);
						data << msg;
					}
					else
						data << pReason;

					data << uint8(0);
					SendPacket(&data);
					return;
				}

				GetPlayer()->SaveEntryPoint(pAreaTrigger->Mapid);
				GetPlayer()->SafeTeleport(pAreaTrigger->Mapid, 0, LocationVector(pAreaTrigger->x, pAreaTrigger->y, pAreaTrigger->z, pAreaTrigger->o));
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
	default:break;
	}
}
