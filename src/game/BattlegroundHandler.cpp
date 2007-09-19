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

void WorldSession::HandleBattlefieldPortOpcode(WorldPacket &recv_data)
{
	uint16 mapinfo, unk;
	uint8 action;
	uint32 bgtype;

	if(!_player->IsInWorld()) return;
	recv_data >> unk >> bgtype >> mapinfo >> action;

	if(action == 0)
	{
		BattlegroundManager.RemovePlayerFromQueues(_player);
	}
	else
	{
		/* Usually the fields in the packet would've been used to check what instance we're porting into, however since we're not
		 * doing "queue multiple battleground types at once" we can just use our cached pointer in the player class. - Burlex
		 */

		if(_player->m_pendingBattleground)
			_player->m_pendingBattleground->PortPlayer(_player);
	}
}

void WorldSession::HandleBattlefieldStatusOpcode(WorldPacket &recv_data)
{
	/* This is done based on whether we are queued, inside, or not in a battleground.
	 */
	if(_player->m_pendingBattleground)		// Ready to port
		BattlegroundManager.SendBattlefieldStatus(_player, 2, _player->m_pendingBattleground->GetType(), _player->m_pendingBattleground->GetId(), 120000, 0, _player->m_pendingBattleground->Rated());
	else if(_player->m_bg)					// Inside a bg
		BattlegroundManager.SendBattlefieldStatus(_player, 3, _player->m_bg->GetType(), _player->m_bg->GetId(), World::UNIXTIME - _player->m_bg->GetStartTime(), _player->GetMapId(), _player->m_bg->Rated());
	else									// None
		BattlegroundManager.SendBattlefieldStatus(_player, 0, 0, 0, 0, 0, 0);	
}

void WorldSession::HandleBattlefieldListOpcode(WorldPacket &recv_data)
{
	// TODO implement this
	sLog.outString("Received CMSG_BATTLEFIELD_LIST");
}

void WorldSession::SendBattlegroundList(Creature* pCreature, uint32 mapid)
{
	if(!pCreature)
		return;

	/* we should have a bg id selection here. */
	uint32 t = BATTLEGROUND_WARSUNG_GULCH;
	if(pCreature->GetCreatureName())
	{
		if(strstr(pCreature->GetCreatureName()->SubName, "Arena") != NULL)
			t = BATTLEGROUND_ARENA_2V2;
		else if(strstr(pCreature->GetCreatureName()->SubName, "Warsong") != NULL)
			t = BATTLEGROUND_WARSUNG_GULCH;
	}

    BattlegroundManager.HandleBattlegroundListPacket(this, t);
}

void WorldSession::HandleBattleMasterHelloOpcode(WorldPacket &recv_data)
{
	sLog.outString("Received CMSG_BATTLEMASTER_HELLO");
}

void WorldSession::HandleLeaveBattlefieldOpcode(WorldPacket &recv_data)
{
	if(_player->m_bg && _player->IsInWorld())
		_player->m_bg->RemovePlayer(_player, false);
}

void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket &recv_data)
{
	if(!_player->IsInWorld() || !_player->m_bg) return;
	uint64 guid;
	recv_data >> guid;
	
	uint32 restime = (_player->m_bg->GetLastResurrect() + 30) - World::UNIXTIME;
	WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
	data << guid << restime;
	SendPacket(&data);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket &recv_data)
{
	if(!_player->IsInWorld() || !_player->m_bg) return;
	uint64 guid;
	recv_data >> guid;
	_player->m_bg->QueuePlayerForResurrect(_player);
}

void WorldSession::HandleBattlegroundPlayerPositionsOpcode(WorldPacket &recv_data)
{
	/* This packet doesn't appear to be used anymore... 
	 * - Burlex
	 */
}

void WorldSession::HandleBattleMasterJoinOpcode(WorldPacket &recv_data)
{
	if(_player->GetGroup() && _player->GetGroup()->m_isqueued)
	{
		SystemMessage("You are in a group that is already queued for a battleground or inside a battleground. Leave this first.");
		return;
	}

	/* are we already in a queue? */
	if(_player->m_bgIsQueued)
		BattlegroundManager.RemovePlayerFromQueues(_player);

	if(_player->IsInWorld())
		BattlegroundManager.HandleBattlegroundJoin(this, recv_data);
}

void WorldSession::HandleArenaJoinOpcode(WorldPacket &recv_data)
{
	if(_player->GetGroup() && _player->GetGroup()->m_isqueued)
	{
		SystemMessage("You are in a group that is already queued for a battleground or inside a battleground. Leave this first.");
		return;
	}

	/* are we already in a queue? */
    if(_player->m_bgIsQueued)
		BattlegroundManager.RemovePlayerFromQueues(_player);

	uint32 bgtype=0;
	uint64 guid;
	uint8 arenacategory;
	uint8 as_group;
	uint8 rated_match;
    recv_data >> guid >> arenacategory >> as_group >> rated_match;
	switch(arenacategory)
	{
	case 0:		// 2v2
		bgtype = BATTLEGROUND_ARENA_2V2;
		break;

	case 1:		// 3v3
		bgtype = BATTLEGROUND_ARENA_3V3;
		break;

	case 2:		// 5v5
		bgtype = BATTLEGROUND_ARENA_5V5;
		break;
	}

	if(bgtype != 0)
		BattlegroundManager.HandleArenaJoin(this, bgtype, as_group, rated_match);
}

void WorldSession::HandleInspectHonorStatsOpcode(WorldPacket &recv_data)
{
	/* This belongs in HonorHandler. :P
	 * - Burlex
	 */

	sLog.outString("Received MSG_INSPECT_HONOR_STATS");
}

void WorldSession::HandlePVPLogDataOpcode(WorldPacket &recv_data)
{
	if(_player->m_bg)
		_player->m_bg->SendPVPData(_player);
}

