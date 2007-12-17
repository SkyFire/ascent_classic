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

void WorldSession::HandleGuildQuery(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 4);

	uint32 guildId;
	recv_data >> guildId;

	// we can skip some searches here if this is our guild
	if(_player && _player->GetGuildId() == guildId && _player->m_playerInfo->guild) {
		_player->m_playerInfo->guild->SendGuildQuery(this);
		return;
	}
	
	Guild * pGuild = objmgr.GetGuild( guildId );
	if(!pGuild)
		return;

	pGuild->SendGuildQuery(this);
}

void WorldSession::HandleCreateGuild(WorldPacket & recv_data)
{
}

/*void WorldSession::SendGuildCommandResult(uint32 typecmd,const char *  str,uint32 cmdresult)
{
	WorldPacket data;
	data.SetOpcode(SMSG_GUILD_COMMAND_RESULT);
	data << typecmd;
	data << str;
	data << cmdresult;
	SendPacket(&data);
}*/

void WorldSession::HandleInviteToGuild(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);

	std::string inviteeName;
	recv_data >> inviteeName;

	Player *plyr = objmgr.GetPlayer( inviteeName.c_str() , false);
	Guild *pGuild = _player->m_playerInfo->guild;
	
	if(!plyr)
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,inviteeName.c_str(),GUILD_PLAYER_NOT_FOUND);
		return;
	}
	else if(!pGuild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if( plyr->GetGuildId() )
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,plyr->GetName(),ALREADY_IN_GUILD);
		return;
	}
	else if( plyr->GetGuildInvitersGuid())
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,plyr->GetName(),ALREADY_INVITED_TO_GUILD);
		return;
	}
	else if(!_player->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_INVITE))
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,"",GUILD_PERMISSIONS);
		return;
	}
	else if(plyr->GetTeam()!=_player->GetTeam() && _player->GetSession()->GetPermissionCount() == 0)
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,"",GUILD_NOT_ALLIED);
		return;
	}
	Guild::SendGuildCommandResult(this, GUILD_INVITE_S,inviteeName.c_str(),GUILD_U_HAVE_INVITED);
	//41
  
	WorldPacket data(SMSG_GUILD_INVITE, 100);
	data << _player->GetName();
	data << pGuild->GetGuildName();
	plyr->GetSession()->SendPacket(&data);

	plyr->SetGuildInvitersGuid( _player->GetGUIDLow() );	
}

void WorldSession::HandleGuildAccept(WorldPacket & recv_data)
{
	Player *plyr = GetPlayer();

	if(!plyr)
		return;

	Player *inviter = objmgr.GetPlayer( plyr->GetGuildInvitersGuid() );
	plyr->UnSetGuildInvitersGuid();

	if(!inviter)
	{
		return;
	}

	Guild *pGuild = inviter->m_playerInfo->guild;
	if(!pGuild)
	{
		return;
	}

	pGuild->AddGuildMember(plyr->m_playerInfo, NULL);
}

void WorldSession::HandleGuildDecline(WorldPacket & recv_data)
{
	WorldPacket data;

	Player *plyr = GetPlayer();

	if(!plyr)
		return;

	Player *inviter = objmgr.GetPlayer( plyr->GetGuildInvitersGuid() );
	plyr->UnSetGuildInvitersGuid(); 

	if(!inviter)
		return;

	data.Initialize(SMSG_GUILD_DECLINE);
	data << plyr->GetName();
	inviter->GetSession()->SendPacket(&data);
}

void WorldSession::HandleSetGuildInformation(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);
	std::string NewGuildInfo;
	recv_data >> NewGuildInfo;

	Guild *pGuild = _player->m_playerInfo->guild;
	if(!pGuild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	pGuild->SetGuildInformation(NewGuildInfo.c_str(), this);
}

void WorldSession::HandleGuildInfo(WorldPacket & recv_data)
{
	/*WorldPacket data;

	Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );

	if(!pGuild)
		return;

	data.Initialize(SMSG_GUILD_INFO);//not sure
	data << pGuild->GetGuildName();
	data << pGuild->GetCreatedYear();
	data << pGuild->GetCreatedMonth();
	data << pGuild->GetCreatedDay();
	data << uint32(pGuild->GetGuildMembersCount());
	data << uint32(pGuild->GetGuildMembersCount());//accountcount

	SendPacket(&data);*/
}

void WorldSession::HandleGuildRoster(WorldPacket & recv_data)
{
	if(!_player->m_playerInfo->guild)
		return;

	_player->m_playerInfo->guild->SendGuildRoster(this);
}

void WorldSession::HandleGuildPromote(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);

	std::string name;
	recv_data >> name;

	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	PlayerInfo * dstplr = objmgr.GetPlayerInfoByName(name);
	if(dstplr==NULL)
		return;

	_player->m_playerInfo->guild->PromoteGuildMember(dstplr, this);
}

void WorldSession::HandleGuildDemote(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);

	std::string name;
	recv_data >> name;

	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	PlayerInfo * dstplr = objmgr.GetPlayerInfoByName(name);
	if(dstplr==NULL)
		return;

	_player->m_playerInfo->guild->DemoteGuildMember(dstplr, this);
}

void WorldSession::HandleGuildLeave(WorldPacket & recv_data)
{
	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	_player->m_playerInfo->guild->RemoveGuildMember(_player->m_playerInfo, this);
}

void WorldSession::HandleGuildRemove(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);

	std::string name;
	recv_data >> name;

	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	PlayerInfo * dstplr = objmgr.GetPlayerInfoByName(name);
	if(dstplr==NULL)
		return;

	_player->m_playerInfo->guild->RemoveGuildMember(dstplr, this);
}

void WorldSession::HandleGuildDisband(WorldPacket & recv_data)
{
	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if(_player->m_playerInfo->guild->GetGuildLeader() != _player->GetGUIDLow())
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S, "", GUILD_PERMISSIONS);
		return;
	}

	_player->m_playerInfo->guild->Disband();
}

void WorldSession::HandleGuildLeader(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);

	std::string name;
	recv_data >> name;

	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	PlayerInfo * dstplr = objmgr.GetPlayerInfoByName(name);
	if(dstplr==NULL)
		return;

	_player->m_playerInfo->guild->ChangeGuildMaster(dstplr, this);
}

void WorldSession::HandleGuildMotd(WorldPacket & recv_data)
{
	std::string motd;
	if(recv_data.size())
		recv_data >> motd;

	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	_player->m_playerInfo->guild->SetMOTD(motd.c_str(), this);
}

void WorldSession::HandleGuildRank(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 9);

	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if(GetPlayer()->GetGUIDLow() != _player->m_playerInfo->guild->GetGuildLeader())
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,"",GUILD_PERMISSIONS);
		return;
	}

	uint32 rankId;
	string newName;
	uint32 i;
	GuildRank * pRank;

	recv_data >> rankId;
	pRank = _player->m_playerInfo->guild->GetGuildRank(rankId);
	if(pRank == NULL)
		return;

	recv_data >> pRank->iRights;
	recv_data >> newName;
	
	if(strcmp(newName.c_str(), pRank->szRankName))
	{
		// name changed
		char * pTmp = pRank->szRankName;
		pRank->szRankName = strdup(newName.c_str());
		free(pTmp);

		CharacterDatabase.Execute("UPDATE guild_ranks SET rankName = \"%s\" WHERE guildId = %u AND rankId = %u", 
			CharacterDatabase.EscapeString(newName).c_str(), _player->m_playerInfo->guild->GetGuildId(), rankId);
	}

	recv_data >> pRank->iGoldLimitPerDay;
	recv_data >> pRank->iBankTabFlags;
	recv_data >> pRank->iItemStacksPerDay;

	for(i = 0; i < 10; ++i)
		recv_data >> pRank->iExtraRights[i];

	CharacterDatabase.Execute("UPDATE guild_ranks SET rankRights = %u, goldLimitPerDay = %u, bankTabFlags = %u, itemStacksPerDay = %u WHERE rankId = %u AND guildId = %u",
		pRank->iRights, pRank->iGoldLimitPerDay, pRank->iBankTabFlags, pRank->iItemStacksPerDay, pRank->iId, _player->m_playerInfo->guild->GetGuildId());
}

void WorldSession::HandleGuildAddRank(WorldPacket & recv_data)
{
	/*CHECK_PACKET_SIZE(recv_data, 1);
	WorldPacket data;
	Guild *pGuild;
	std::string rankname;

	pGuild = objmgr.GetGuild(GetPlayer()->GetGuildId());
	if(!pGuild)
	{
		//not in Guild
		SendGuildCommandResult(GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if(GetPlayer()->GetGUID() != pGuild->GetGuildLeaderGuid())
	{
		//not GuildMaster
		SendGuildCommandResult(GUILD_INVITE_S,"",GUILD_PERMISSIONS);
		return;
	}

	recv_data >> rankname;

	pGuild->CreateRank(rankname,GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);

	pGuild->FillQueryData(&data);
	pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

	data.clear();
	//Maybe theres an event for this
	pGuild->FillGuildRosterData(&data);
	pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

	pGuild->SaveRanksToDb();*/
}

void WorldSession::HandleGuildDelRank(WorldPacket & recv_data)
{
	/*WorldPacket data;
	Guild *pGuild;

	pGuild = objmgr.GetGuild(GetPlayer()->GetGuildId());	
	if(!pGuild)
	{
		SendGuildCommandResult(GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if(GetPlayer()->GetGUID() != pGuild->GetGuildLeaderGuid())
	{
		SendGuildCommandResult(GUILD_INVITE_S,"",GUILD_PERMISSIONS);
		return;
	}

	pGuild->DelRank();

	pGuild->FillQueryData(&data);
	pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
	data.clear();

	//Maybe theres an event for this
	pGuild->FillGuildRosterData(&data);
	pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

	pGuild->SaveRanksToDb();*/
}

void WorldSession::HandleGuildSetPublicNote(WorldPacket & recv_data)
{
	string target, newnote;
	recv_data >> target >> newnote;

	PlayerInfo * pTarget = objmgr.GetPlayerInfoByName(target);
	if(pTarget == NULL)
		return;

	if(!pTarget->guild)
		return;

	pTarget->guild->SetPublicNote(pTarget, newnote.c_str(), this);
}

void WorldSession::HandleGuildSetOfficerNote(WorldPacket & recv_data)
{
	string target, newnote;
	recv_data >> target >> newnote;

	PlayerInfo * pTarget = objmgr.GetPlayerInfoByName(target);
	if(pTarget == NULL)
		return;

	if(!pTarget->guild)
		return;

	pTarget->guild->SetOfficerNote(pTarget, newnote.c_str(), this);
}

void WorldSession::HandleSaveGuildEmblem(WorldPacket & recv_data)
{
/*	CHECK_PACKET_SIZE(recv_data, 20);
	WorldPacket data;

	Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );

	if(!pGuild)
	{
		data.Initialize(MSG_SAVE_GUILD_EMBLEM);
		data <<	uint32(ERR_GUILDEMBLEM_NOGUILD);
		SendPacket(&data);
		return;
	}

	Player *plyr = GetPlayer();

	if(!plyr)
		return;

	if(plyr->GetGuildRank() != GUILDRANK_GUILD_MASTER)
	{
		data.Initialize(MSG_SAVE_GUILD_EMBLEM);
		data <<	uint32(ERR_GUILDEMBLEM_NOTGUILDMASTER);
		SendPacket(&data);
		return;
	}
	uint32 Money = plyr->GetUInt32Value(PLAYER_FIELD_COINAGE);
	uint32 Cost = 100000; //10 Gold
	if(Money < Cost)
	{
		data.Initialize(MSG_SAVE_GUILD_EMBLEM);
		data << uint32(ERR_GUILDEMBLEM_NOTENOUGHMONEY);
		SendPacket(&data);
		return;
	}
	plyr->SetUInt32Value(PLAYER_FIELD_COINAGE,(Money-Cost));

	uint64 DesignerGuid;
	recv_data >> DesignerGuid;

	uint32 emblemPart;
	recv_data >> emblemPart;
	pGuild->SetGuildEmblemStyle( emblemPart );
	recv_data >> emblemPart;
	pGuild->SetGuildEmblemColor( emblemPart );
	recv_data >> emblemPart;
	pGuild->SetGuildBorderStyle( emblemPart );
	recv_data >> emblemPart;
	pGuild->SetGuildBorderColor( emblemPart );
	recv_data >> emblemPart;
	pGuild->SetGuildBackgroundColor( emblemPart );

	data.Initialize(MSG_SAVE_GUILD_EMBLEM);
	data <<	uint32(ERR_GUILDEMBLEM_SUCCESS);
	SendPacket(&data);

	data.clear();
	pGuild->FillQueryData(&data);
	pGuild->SendMessageToGuild(plyr->GetGUID(), &data, G_MSGTYPE_ALL);

	//data.Initialize(SMSG_GUILD_EVENT);
	//data << uint8(GUILD_EVENT_TABARDCHANGE);
	//data << uint8(0x00);
	//data << pGuild->GetGuildMotd();
	//pGuild->SendMessageToGuild(NULL, &data, G_MSGTYPE_ALL);

	pGuild->UpdateGuildToDb();*/
}

// Charter part
void WorldSession::HandleCharterBuy(WorldPacket & recv_data)
{
	/*
	{CLIENT} Packet: (0x01BD) CMSG_PETITION_BUY PacketSize = 85
	|------------------------------------------------|----------------|
	|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
	|------------------------------------------------|----------------|
	|50 91 00 00 6E 13 01 F0 00 00 00 00 00 00 00 00 |P...n...........|
	|00 00 00 00 53 74 6F 72 6D 62 72 69 6E 67 65 72 |....Stormbringer|
	|73 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |s...............|
	|00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
	|00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 |................|
	|00 00 00 00 00								  |.....		   |
	-------------------------------------------------------------------
	*/

	uint64 creature_guid;
	uint64 crap;
	uint32 crap2;
	string name;
    uint8 error;
	uint32 crap3,crap4,crap5,crap6,crap7,crap8,crap9,crap10,crap11,arena_index,crap12;
	uint16 crap13;
	uint8 crap14;
	uint32 crap15;
	recv_data >> creature_guid >> crap >> crap2 >> name;
	recv_data >> crap3 >> crap4 >> crap5 >> crap6 >> crap7 >> crap8 >> crap9 >> crap10 >> crap11
		>> crap12 >> crap13 >> crap14 >> arena_index >> crap15;

	Creature * crt = _player->GetMapMgr()->GetCreature((uint32)creature_guid);
	if(!crt)
	{
		Disconnect();
		return;
	}

	if(crt->GetEntry()==19861 || crt->GetEntry()==18897 || crt->GetEntry()==19856)		/* i am lazy! */
	{
		uint32 arena_type = arena_index - 1;
		if(arena_type > 2)
			return;

		if(_player->m_arenaTeams[arena_type] || _player->m_charters[arena_index])
		{
			SendNotification("You are already in an arena team.");
			return;
		}

		ArenaTeam * t = objmgr.GetArenaTeamByName(name, arena_type);
		if(t != NULL)
		{
			sChatHandler.SystemMessage(this,"That name is already in use.");
			return;
		}

		if(objmgr.GetCharterByName(name, (CharterTypes)arena_index))
		{
			sChatHandler.SystemMessage(this,"That name is already in use.");
			return;
		}

		if(_player->m_charters[arena_type])
		{
			SendNotification("You already have an arena charter.");
			return;
		}

		static uint32 item_ids[] = {ARENA_TEAM_CHARTER_2v2, ARENA_TEAM_CHARTER_3v3, ARENA_TEAM_CHARTER_5v5};
		static uint32 costs[] = {ARENA_TEAM_CHARTER_2v2_COST,ARENA_TEAM_CHARTER_3v3_COST,ARENA_TEAM_CHARTER_5v5_COST};

		ItemPrototype * ip = ItemPrototypeStorage.LookupEntry(item_ids[arena_type]);
		ASSERT(ip);
		SlotResult res = _player->GetItemInterface()->FindFreeInventorySlot(ip);
		if(res.Result == 0)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
			return;
		}

		error = _player->GetItemInterface()->CanReceiveItem(ip,1);
		if(error)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL,NULL,error);
		}
		else
		{
			// Create the item and charter
			Item * i = objmgr.CreateItem(item_ids[arena_type], _player);
			Charter * c = objmgr.CreateCharter(_player->GetGUIDLow(), (CharterTypes)arena_index);
			c->GuildName = name;
			c->ItemGuid = i->GetGUID();

			i->SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
			i->SetUInt32Value(ITEM_FIELD_FLAGS, 1);
			i->SetUInt32Value(ITEM_FIELD_ENCHANTMENT, c->GetID());
			i->SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, 57813883);
			_player->GetItemInterface()->AddItemToFreeSlot(i);
			c->SaveToDB();

			/*WorldPacket data(45);
			BuildItemPushResult(&data, _player->GetGUID(), ITEM_PUSH_TYPE_RECEIVE, 1, item_ids[arena_type], 0);
			SendPacket(&data);*/
			SendItemPushResult(i, false, true, false, true, _player->GetItemInterface()->LastSearchItemBagSlot(), _player->GetItemInterface()->LastSearchItemSlot(), 1);

			_player->m_charters[arena_index] = c;
			_player->SaveToDB(false);
		}
	}
	else
	{
		Guild * g = objmgr.GetGuildByGuildName(name);
		Charter * c = objmgr.GetCharterByName(name, CHARTER_TYPE_GUILD);
		if(g != 0 || c != 0)
		{
			SendNotification("A guild with that name already exists.");
			return;
		}

		if(_player->m_charters[CHARTER_TYPE_GUILD])
		{
			SendNotification("You already have a guild charter.");
			return;
		}

		ItemPrototype * ip = ItemPrototypeStorage.LookupEntry(ITEM_ENTRY_GUILD_CHARTER);
		assert(ip);
		SlotResult res = _player->GetItemInterface()->FindFreeInventorySlot(ip);
		if(res.Result == 0)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
			return;
		}

		error = _player->GetItemInterface()->CanReceiveItem(ItemPrototypeStorage.LookupEntry(ITEM_ENTRY_GUILD_CHARTER),1);
		if(error)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL,NULL,error);
		}
		else
		{
			// Meh...
			WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 12);
			data << uint32(0x000019C2);
			data << creature_guid;
			SendPacket(&data);

			// Create the item and charter
			Item * i = objmgr.CreateItem(ITEM_ENTRY_GUILD_CHARTER, _player);
			c = objmgr.CreateCharter(_player->GetGUIDLow(), CHARTER_TYPE_GUILD);
			c->GuildName = name;
			c->ItemGuid = i->GetGUID();


			i->SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
			i->SetUInt32Value(ITEM_FIELD_FLAGS, 1);
			i->SetUInt32Value(ITEM_FIELD_ENCHANTMENT, c->GetID());
			i->SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, 57813883);
			_player->GetItemInterface()->AddItemToFreeSlot(i);
			c->SaveToDB();

			/*data.clear();
			data.resize(45);
			BuildItemPushResult(&data, _player->GetGUID(), ITEM_PUSH_TYPE_RECEIVE, 1, ITEM_ENTRY_GUILD_CHARTER, 0);
			SendPacket(&data);*/
			SendItemPushResult(i, false, true, false, true, _player->GetItemInterface()->LastSearchItemBagSlot(), _player->GetItemInterface()->LastSearchItemSlot(), 1);

			_player->m_charters[CHARTER_TYPE_GUILD] = c;
			_player->SaveToDB(false);
		}
	}
}

void SendShowSignatures(Charter * c, uint64 i, Player * p)
{
	WorldPacket data(SMSG_PETITION_SHOW_SIGNATURES, 100);
	data << i;
	data << (uint64)c->GetLeader();
	data << c->GetID();
	data << uint8(c->SignatureCount);
	for(uint32 i = 0; i < c->Slots; ++i)
	{
		if(c->Signatures[i] == 0) continue;
		data << uint64(c->Signatures[i]) << uint32(1);
	}
	data << uint8(0);
	p->GetSession()->SendPacket(&data);
}

void WorldSession::HandleCharterShowSignatures(WorldPacket & recv_data)
{
	Charter * pCharter;
	uint64 item_guid;
	recv_data >> item_guid;
	pCharter = objmgr.GetCharterByItemGuid(item_guid);
	
	if(pCharter)
		SendShowSignatures(pCharter, item_guid, _player);
}

void WorldSession::HandleCharterQuery(WorldPacket & recv_data)
{
	/*
	{SERVER} Packet: (0x01C7) SMSG_PETITION_QUERY_RESPONSE PacketSize = 77
	|------------------------------------------------|----------------|
	|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
	|------------------------------------------------|----------------|
	|20 08 00 00 28 32 01 00 00 00 00 00 53 74 6F 72 | ...(2......Stor|
	|6D 62 72 69 6E 67 65 72 73 00 00 09 00 00 00 09 |mbringers.......|
	|00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
	|00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
	|00 00 00 00 00 00 00 00 00 00 00 00 00		  |.............   |
	-------------------------------------------------------------------
	
	uint32 charter_id
	uint64 leader_guid
	string guild_name
	uint8  0			<-- maybe subname? or some shit.. who knows
	uint32 9
	uint32 9
	uint32[9] signatures
	uint8  0
	uint8  0
	*/
//this is wrong there are 42 bytes after 9 9, 4*9+2=38 not 42,
	//moreover it can't signature, blizz uses always fullguid so it must be uin64
	//moreover this does not show ppl who signed this, for this purpose there is another opcde
	uint32 charter_id;
	uint64 item_guid;
	recv_data >> charter_id;
	recv_data >> item_guid;
	/*Charter * c = objmgr.GetCharter(charter_id,CHARTER_TYPE_GUILD);
	if(c == 0)
		c = objmgr.GetCharter(charter_id, CHARTER_TYPE_ARENA_2V2);
	if(c == 0)
		c = objmgr.GetCharter(charter_id, CHARTER_TYPE_ARENA_3V3);
	if(c == 0)
		c = objmgr.GetCharter(charter_id, CHARTER_TYPE_ARENA_5V5);*/

	Charter * c = objmgr.GetCharterByItemGuid(item_guid);
	if(c == 0)
		return;

	WorldPacket data(SMSG_PETITION_QUERY_RESPONSE, 100);
	data << charter_id;
	data << (uint64)c->LeaderGuid;
	data << c->GuildName << uint8(0);
	if(c->CharterType == CHARTER_TYPE_GUILD)
	{
		data << uint32(9) << uint32(9);
	}
	else
	{
		/*uint32 v = c->CharterType;
		if(c->CharterType == CHARTER_TYPE_ARENA_3V3)
			v=2;
		else if(c->CharterType == CHARTER_TYPE_ARENA_5V5)
			v=4;

		data << v << v;*/
		data << uint32(c->Slots) << uint32(c->Slots);
	}

	data << uint32(0);                                      // 4
    data << uint32(0);                                      // 5
    data << uint32(0);                                      // 6
    data << uint32(0);                                      // 7
    data << uint32(0);                                      // 8
    data << uint16(0);                                      // 9 2 bytes field
    data << uint32(0x46);                                      // 10
    data << uint32(0);                                      // 11
    data << uint32(0);                                      // 13 count of next strings?
    data << uint32(0);                                      // 14

	if (c->CharterType == CHARTER_TYPE_GUILD)
	{
		data << uint32(0);
	}
	else
	{
		data << uint32(1);
	}

	SendPacket(&data);
}

void WorldSession::HandleCharterOffer( WorldPacket & recv_data )
{
	uint32 shit;
	uint64 item_guid, target_guid;
	Charter * pCharter;
	recv_data >> shit >> item_guid >> target_guid;
	
	if(!_player->IsInWorld()) return;
	Player * pTarget = _player->GetMapMgr()->GetPlayer((uint32)target_guid);
	pCharter = objmgr.GetCharterByItemGuid(item_guid);

	if(pTarget == 0 || pTarget->GetTeam() != _player->GetTeam() || pTarget == _player)
	{
		SendNotification("Target is of the wrong faction.");
		return;
	}

	if(!pTarget->CanSignCharter(pCharter, _player))
	{
		SendNotification("Target player cannot sign your charter for one or more reasons.");
		return;
	}

	SendShowSignatures(pCharter, item_guid, pTarget);
}

void WorldSession::HandleCharterSign( WorldPacket & recv_data )
{
	uint64 item_guid;
	recv_data >> item_guid;

	Charter * c = objmgr.GetCharterByItemGuid(item_guid);
	if(c == 0)
		return;

	for(uint32 i = 0; i < 9; ++i)
	{
		if(c->Signatures[i] == _player->GetGUID())
		{
			SendNotification("You have already signed that charter.");
			return;
		}
	}

	if(c->IsFull())
		return;

	c->AddSignature(_player->GetGUIDLow());
	c->SaveToDB();
	_player->m_charters[c->CharterType] = c;
	_player->SaveToDB(false);

	Player * l = _player->GetMapMgr()->GetPlayer(c->GetLeader());
	if(l == 0)
		return;

	WorldPacket data(SMSG_PETITION_SIGN_RESULTS, 100);
	data << item_guid << _player->GetGUID() << uint32(0);
	l->GetSession()->SendPacket(&data);
	data.clear();
	data << item_guid << (uint64)c->GetLeader() << uint32(0);
	SendPacket(&data);
}

void WorldSession::HandleCharterTurnInCharter(WorldPacket & recv_data)
{
	uint64 mooguid;
	recv_data >> mooguid;
	Charter * pCharter = objmgr.GetCharterByItemGuid(mooguid);
	if(!pCharter) return;

	if(pCharter->CharterType == CHARTER_TYPE_GUILD)
	{
		Charter * gc = _player->m_charters[CHARTER_TYPE_GUILD];
		if(gc == 0)
			return;
		if(gc->SignatureCount < 9 && Config.MainConfig.GetBoolDefault("Server", "RequireAllSignatures", false))
		{
			SendNotification("You don't have the required amount of signatures to turn in this petition.");
			return;
		}

		// dont know hacky or not but only solution for now
		// If everything is fine create guild

		Guild *pGuild = Guild::Create();
		pGuild->CreateFromCharter(gc, this);

/*		uint32 guildId = pGuild->GetFreeGuildIdFromDb();

		if(guildId == 0)
		{
			printf("Error Getting Free Guild ID");
			delete pGuild;
			return;
		}

		//Guild Setup
		pGuild->SetGuildId( guildId );
		pGuild->SetGuildName( gc->GuildName );
		pGuild->CreateRank("Guild Master", GR_RIGHT_ALL);
		pGuild->CreateRank("Officer", GR_RIGHT_ALL);
		pGuild->CreateRank("Veteran", GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);  
		pGuild->CreateRank("Member", GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
		pGuild->CreateRank("Initiate", GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
		pGuild->SetGuildEmblemStyle( 0xFFFF );
		pGuild->SetGuildEmblemColor( 0xFFFF );
		pGuild->SetGuildBorderStyle( 0xFFFF );
		pGuild->SetGuildBorderColor( 0xFFFF );
		pGuild->SetGuildBackgroundColor( 0xFFFF );

		objmgr.AddGuild(pGuild);

		//Guild Leader Setup
		_player->SetGuildId( pGuild->GetGuildId() );
		_player->SetGuildRank(GUILDRANK_GUILD_MASTER);
		pGuild->SetGuildLeaderGuid( _player->GetGUID() );
		pGuild->AddNewGuildMember( _player );

		//Other Guild Members Setup
		Player *pMember;  

		for(uint32 x=0;x<9;x++)
		{	   
			pMember = objmgr.GetPlayer(gc->Signatures[x]);
			if(pMember)//online
			{
				pMember->SetGuildId( pGuild->GetGuildId() );
				pMember->SetGuildRank(GUILDRANK_MEMBER);		
				//Charters
				pMember->m_charters[gc->CharterType] = 0;
				pMember->SaveToDB(false);
			}
			else
			{
				CharacterDatabase.Execute("UPDATE characters SET guildid = %u WHERE guid = %u", pGuild->GetGuildId(), gc->Signatures[x]);
				CharacterDatabase.Execute("UPDATE characters SET guildRank = %u WHERE guid = %u", GUILDRANK_MEMBER, gc->Signatures[x]);
			}

			PlayerInfo *p=objmgr.GetPlayerInfo(gc->Signatures[x]);
			if(!p)continue;//this should not ever happen though
			p->Rank = GUILDRANK_MEMBER;
			pGuild->AddGuildMember(p);			
		}	

		pGuild->SaveToDb();
		pGuild->SaveAllGuildMembersToDb();
		pGuild->SaveRanksToDb();*/

		// Destroy the charter
		_player->m_charters[CHARTER_TYPE_GUILD] = 0;
		gc->Destroy();

		_player->GetItemInterface()->RemoveItemAmt(ITEM_ENTRY_GUILD_CHARTER, 1);
		sHookInterface.OnGuildCreate(_player, pGuild);
	}
	else
	{
		/* Arena charter - TODO: Replace with correct messages */
		ArenaTeam * team;
		uint32 type;
		uint32 i;
		uint32 icon, iconcolor, bordercolor, border, background;
		recv_data >> iconcolor >>icon >> bordercolor >> border >> background;

		switch(pCharter->CharterType)
		{
		case CHARTER_TYPE_ARENA_2V2:
			type = ARENA_TEAM_TYPE_2V2;
			break;

		case CHARTER_TYPE_ARENA_3V3:
			type = ARENA_TEAM_TYPE_3V3;
			break;

		case CHARTER_TYPE_ARENA_5V5:
			type = ARENA_TEAM_TYPE_5V5;
			break;

		default:
			SendNotification("Internal Error");
			return;
		}

		if(_player->m_arenaTeams[pCharter->CharterType-1] != NULL)
		{
			sChatHandler.SystemMessage(this, "You are already in an arena team.");
			return;
		}

		if(pCharter->SignatureCount < pCharter->GetNumberOfSlotsByType() && Config.MainConfig.GetBoolDefault("Server", "RequireAllSignatures", false))
		{
			sChatHandler.SystemMessage(this, "You don't have the required amount of signatures to turn in this petition.");
			return;
		}

		team = new ArenaTeam(type, objmgr.GenerateArenaTeamId());
		team->m_name = pCharter->GuildName;
		team->m_emblemColour = iconcolor;
		team->m_emblemStyle = icon;
		team->m_borderColour = bordercolor;
		team->m_borderStyle = border;
		team->m_backgroundColour = background;
		team->m_leader=_player->GetGUIDLow();
		team->m_stat_rating=1500;
        
		objmgr.AddArenaTeam(team);
		objmgr.UpdateArenaTeamRankings();
		team->AddMember(_player->m_playerInfo);
		

		/* Add the members */
		for(i = 0; i < pCharter->SignatureCount; ++i)
		{
			PlayerInfo * info = objmgr.GetPlayerInfo(pCharter->Signatures[i]);
			if(info)
			{
				team->AddMember(info);
			}
		}

		_player->GetItemInterface()->SafeFullRemoveItemByGuid(mooguid);
		_player->m_charters[pCharter->CharterType] = NULL;
		pCharter->Destroy();
	}

	WorldPacket data(4);
	data.SetOpcode(SMSG_TURN_IN_PETITION_RESULTS);
	data << uint32(0);
	SendPacket( &data );
}

void WorldSession::HandleCharterRename(WorldPacket & recv_data)
{
	uint64 guid;
	string name;
	recv_data >> guid >> name;

	Charter * pCharter = objmgr.GetCharterByItemGuid(guid);
	if(pCharter == 0)
		return;

	Guild * g = objmgr.GetGuildByGuildName(name);
	Charter * c = objmgr.GetCharterByName(name, (CharterTypes)pCharter->CharterType);
	if(c || g)
	{
		SendNotification("That name is in use by another guild.");
		return;
	}

	c = pCharter;
	c->GuildName = name;
	c->SaveToDB();
	WorldPacket data(MSG_PETITION_RENAME, 100);
	data << guid << name;
	SendPacket(&data);
}

void WorldSession::HandleGuildLog(WorldPacket & recv_data)
{
	if(!_player->m_playerInfo->guild)
		return;

	_player->m_playerInfo->guild->SendGuildLog(this);
}

void WorldSession::HandleGuildBankBuyTab(WorldPacket & recv_data)
{
	uint64 guid;
	recv_data >> guid;

	if(!_player->IsInWorld())
		return;

	if(!_player->m_playerInfo->guild)
		return;

	if(_player->m_playerInfo->guild->GetGuildLeader() != _player->GetGUIDLow())
	{
		Guild::SendGuildCommandResult(this, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	_player->m_playerInfo->guild->BuyBankTab(this);
	_player->m_playerInfo->guild->LogGuildEvent(GUILD_EVENT_BANKTABBOUGHT, 1, "");
}

void WorldSession::HandleGuildBankGetAvailableAmount(WorldPacket & recv_data)
{
	// calculate using the last withdrawl blablabla
	if(_player->m_playerInfo->guildMember == NULL)
		return;

	uint32 money = _player->m_playerInfo->guild->GetBankBalance();
	uint32 avail = _player->m_playerInfo->guildMember->CalculateAvailableAmount();

	/* pls gm mi hero poor give 1 gold coin pl0x */
	WorldPacket data(MSG_GUILD_BANK_GET_AVAILABLE_AMOUNT, 4);
	data << uint32(money>avail ? avail : money);
	SendPacket(&data);
}

void WorldSession::HandleGuildBankModifyTab(WorldPacket & recv_data)
{
	GuildBankTab * pTab;
	uint64 guid;
	uint8 slot;
	string tabname;
	string tabicon;
	char * ptmp;

	recv_data >> guid;
	recv_data >> slot;
	recv_data >> tabname;
	recv_data >> tabicon;

	if(_player->m_playerInfo->guild==NULL)
		return;

	pTab = _player->m_playerInfo->guild->GetBankTab((uint32)slot);
	if(pTab==NULL)
		return;

	if(tabname.size())
	{
		if( !(pTab->szTabName && strcmp(pTab->szTabName, tabname.c_str()) == 0) )
		{
			ptmp = pTab->szTabName;
			pTab->szTabName = strdup(tabname.c_str());
			if(ptmp)
				free(ptmp);
	
			CharacterDatabase.Execute("UPDATE guild_banktabs SET tabName = \"%s\" WHERE guildId = %u AND tabId = %u", 
				CharacterDatabase.EscapeString(tabname).c_str(), _player->m_playerInfo->guild->GetGuildId(), (uint32)slot);
		}
	}
	else
	{
		if(pTab->szTabName)
		{
			ptmp = pTab->szTabName;
			pTab->szTabName = NULL;
			if(ptmp)
				free(ptmp);

			CharacterDatabase.Execute("UPDATE guild_banktabs SET tabName = '' WHERE guildId = %u AND tabId = %u", 
				_player->m_playerInfo->guild->GetGuildId(), (uint32)slot);
		}
	}

	if(tabicon.size())
	{
		if( !(pTab->szTabIcon && strcmp(pTab->szTabIcon, tabicon.c_str()) == 0) )
		{
			ptmp = pTab->szTabIcon;
			pTab->szTabIcon = strdup(tabname.c_str());
			if(ptmp)
				free(ptmp);

			CharacterDatabase.Execute("UPDATE guild_banktabs SET tabIcon = \"%s\" WHERE guildId = %u AND tabId = %u", 
				CharacterDatabase.EscapeString(tabicon).c_str(), _player->m_playerInfo->guild->GetGuildId(), (uint32)slot);
		}
	}
	else
	{
		if(pTab->szTabIcon)
		{
			ptmp = pTab->szTabIcon;
			pTab->szTabIcon = NULL;
			if(ptmp)
				free(ptmp);

			CharacterDatabase.Execute("UPDATE guild_banktabs SET tabIcon = '' WHERE guildId = %u AND tabId = %u", 
				_player->m_playerInfo->guild->GetGuildId(), (uint32)slot);
		}
	}
}

void WorldSession::HandleGuildBankWithdrawMoney(WorldPacket & recv_data)
{
	uint64 guid;
	uint32 money;

	recv_data >> guid;
	recv_data >> money;

	if(_player->m_playerInfo->guild==NULL)
		return;

	_player->m_playerInfo->guild->WithdrawMoney(this, money);
}

void WorldSession::HandleGuildBankDepositMoney(WorldPacket & recv_data)
{
	uint64 guid;
	uint32 money;

	recv_data >> guid;
	recv_data >> money;

	if(_player->m_playerInfo->guild==NULL)
		return;

	_player->m_playerInfo->guild->DepositMoney(this, money);
}

void WorldSession::HandleGuildBankDepositItem(WorldPacket & recv_data)
{
	uint64 guid;
	uint8 source_isfrombank;
	uint8 source_bank;
	uint8 source_bankslot;
	uint8 source_bagslot;
	uint8 source_slot;
	uint8 dest_bank;
	uint8 dest_bankslot;
	uint32 wtf;
	uint8 wtf2;
	GuildBankTab * pTab;
	GuildBankTab * pTab2;
	Item * pItem;
	Item * pItem2;
	Guild * pGuild = _player->m_playerInfo->guild;
	GuildMember * pMember = _player->m_playerInfo->guildMember;
	
	if(pGuild==NULL || pMember==NULL)
		return;

	recv_data >> guid >> source_isfrombank;
	if(source_isfrombank)
	{
		/* read packet */
		recv_data >> source_bank;
		recv_data >> source_bankslot;
		recv_data >> wtf;
		recv_data >> dest_bank;
		recv_data >> dest_bankslot;

		/* sanity checks to avoid overflows */
		if(source_bankslot >= MAX_GUILD_BANK_SLOTS ||
			dest_bankslot >= MAX_GUILD_BANK_SLOTS ||
			source_bank >= MAX_GUILD_BANK_TABS ||
			dest_bank >= MAX_GUILD_BANK_TABS)
		{
			return;
		}

		/* locate the tabs */
		pTab = pGuild->GetBankTab((uint32)dest_bank);
		pTab2 = pGuild->GetBankTab((uint32)source_bank);
		if(pTab==NULL || pTab2==NULL)
			return;

		/* swapping items - this will be kinda difficult in sql.. */
		pItem = pTab->pSlots[source_bankslot];
		pItem2 = pTab2->pSlots[dest_bankslot];

		pTab->pSlots[source_bankslot] = pItem2;
		pTab2->pSlots[dest_bankslot] = pItem;

		/* resend it to the client to update him */
		pGuild->SendGuildBank(this);

		/* update it in the database */
		if(pItem)
		{
			// this one goes in the destination slot
			CharacterDatabase.Execute("UPDATE guild_bankitems SET slotId = %u, tabId = %u WHERE itemGuid = %u AND guildId = %u",
				(uint32)dest_bankslot, (uint32)dest_bank, pItem->GetGUIDLow(), pGuild->GetGuildId());
		}

		if(pItem2)
		{
			// this one is the destination item coming into the source slot
			CharacterDatabase.Execute("UPDATE guild_bankitems SET slotId = %u, tabId = %u WHERE itemGuid = %u AND guildId = %u",
				(uint32)source_bankslot, (uint32)source_bank, pItem2->GetGUIDLow(), pGuild->GetGuildId());
		}
	}
	else
	{
		/* read packet */
		recv_data >> dest_bank;
		recv_data >> dest_bankslot;
		recv_data >> wtf;
		recv_data >> wtf2;

		recv_data >> source_bagslot;
		recv_data >> source_slot;

		/* sanity checks to avoid overflows */
		if(dest_bankslot >= MAX_GUILD_BANK_SLOTS ||
			dest_bank >= MAX_GUILD_BANK_TABS)
		{
			return;
		}

		/* get tab */
		pTab = pGuild->GetBankTab((uint32)dest_bank);
		if(pTab==NULL)
			return;

		if(wtf==0)		// depositing an item
		{
			// get the item
			pItem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(source_bagslot, source_slot, false);
			if(pItem==NULL)
				return;

			// remove owner association
			pItem->SetOwner(NULL);
			pItem->SetUInt32Value(ITEM_FIELD_OWNER, 0);
			pItem->SaveToDB(0, 0, false);

			// insert into the guild bank
			pTab->pSlots[dest_bankslot] = pItem;
			CharacterDatabase.Execute("INSERT INTO guild_bankitems VALUES(%u, %u, %u, %u)", pGuild->GetGuildId(),
				(uint32)dest_bank, (uint32)dest_bankslot, pItem->GetGUIDLow());

			pGuild->SendGuildBank(this);
		}
		else			// withdrawing an item
		{
			pItem = pTab->pSlots[dest_bankslot];
			if(pItem==NULL)
				return;

			if(pMember->pRank->iItemStacksPerDay)
				if(pMember->CalculateAllowedItemWithdraws() == 0)
					return;

			
			pItem2 = _player->GetItemInterface()->GetInventoryItem(source_bagslot, source_slot);
			if(pItem2 != NULL)
			{
				_player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(source_bagslot, source_slot);

				// this actually means we're swapping.. urgh
				// remove owner association
				pItem2->SetOwner(NULL);
				pItem2->SetUInt32Value(ITEM_FIELD_OWNER, 0);
				pItem2->SaveToDB(0, 0, false);

				// insert into the guild bank
				pTab->pSlots[dest_bankslot] = pItem2;
				CharacterDatabase.Execute("INSERT INTO guild_bankitems VALUES(%u, %u, %u, %u)", pGuild->GetGuildId(),
					(uint32)dest_bank, (uint32)dest_bankslot, pItem->GetGUIDLow());
			}

			// set new owner of the item
			pItem->SetOwner(_player);
			pItem->SetUInt32Value(ITEM_FIELD_OWNER, _player->GetGUIDLow());
			
			// attempt to add it onto that slot
			if(_player->GetItemInterface()->SafeAddItem(pItem, source_bagslot, source_slot))
			{
				// remove it from the bank in the database (having the same item in 2 places would be a disaster)
				CharacterDatabase.Execute("DELETE FROM guild_bankitems WHERE guildId = %u AND tabId = %u AND slotId = %u AND itemGuid = %u", 
					pGuild->GetGuildId(), (uint32)dest_bank, (uint32)dest_bankslot, pItem->GetGUIDLow());
	
				pItem->SaveToDB(source_bagslot, source_slot);
				if(!pItem2)
					pTab->pSlots[dest_bankslot] = NULL;

				// mi banks mi bankz!
				pMember->OnItemWithdraw();
			}
			else
			{
				// reverse what we did, for some reason we can't add it :<
				pItem->SetOwner(NULL);
				pItem->SetUInt32Value(ITEM_FIELD_OWNER, 0);
			}

			pGuild->SendGuildBank(this);
		}
	}		
}

void WorldSession::HandleGuildBankOpenVault(WorldPacket & recv_data)
{
	GameObject * pObj;
	uint64 guid;

	if(!_player->IsInWorld() || _player->m_playerInfo->guild==NULL)
		return;

	recv_data >> guid;
	pObj = _player->GetMapMgr()->GetGameObject((uint32)guid);
	if(pObj==NULL)
		return;

	_player->m_playerInfo->guild->SendGuildBank(this);
}

void Guild::SendGuildBank(WorldSession * pClient)
{
	size_t pos;
	uint32 count=0;
	WorldPacket data(0x03E7, 3000);
	GuildMember * pMember = pClient->GetPlayer()->m_playerInfo->guildMember;

	if(pMember==NULL)
		return;

	data << uint64(m_bankBalance);			// amount you have deposited
	data << uint8(0);				// unknown
	data << uint32(pMember->CalculateAllowedItemWithdraws());		// remaining stacks for this day
	data << uint8(1);		// some sort of view flag?
	data << uint8(GetBankTabCount());		// tab count?

	for(uint32 i = 0; i < GetBankTabCount(); ++i)
	{
		GuildBankTab * pTab = GetBankTab(i);
		if(pTab==NULL)
		{
			data << uint16(0);		// shouldn't happen
			continue;
		}

		if(pTab->szTabName)
			data << pTab->szTabName;
		else
			data << uint8(0);

		if(pTab->szTabIcon)
			data << pTab->szTabIcon;
		else
			data << uint8(0);
	}

	pos = data.wpos();
	data << uint8(0);

	for(uint32 i = 0; i < GetBankTabCount(); ++i)
	{
		GuildBankTab * pTab = GetBankTab(i);
		if(pTab)
		{
			for(uint32 j = 0; j < MAX_GUILD_BANK_SLOTS; ++j)
			{
				if(pTab->pSlots[j] != NULL)
				{
					++count;

					// what i don't understand here, where is the field for random properties? :P - Burlex
					data << uint8(j);
					data << pTab->pSlots[j]->GetEntry();
					data << uint32(0);			// this is an enchant
					data << pTab->pSlots[j]->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
					data << uint16(i);
				}
			}
		}
	}

#ifdef USING_BIG_ENDIAN
	*(uint8*)&data.contents()[pos] = swap16(count);
#else
	*(uint8*)&data.contents()[pos] = (uint8)count;
#endif

	// uint8 view_flag
	// if view_flag = 0
	// uint8 item count
	// for each item
	//     uint8 slot
	//     uint32 entry
	//     uint32 unk
	//     uint32 stack_count
	//     uint16 unk


	//data << uint8(0);
	// uint64 amount_you_have_deposited
	/*data << uint64(1000000);

	data << uint8(1);
	data << uint8(0);
	data << uint8(0);
	data << uint8(0);

	data << uint8(0);
	data << uint8(1);
	data << uint8(0x4C);
	data << uint8(0);
	data << uint8(0x11);
	data << uint8(0x29);

	data << uint8(0);
	data << uint8(1);
	data << uint8(1);
	data << uint8(0);
	data << uint8(0);
	data << uint8(0);*/

	pClient->SendPacket(&data);
}