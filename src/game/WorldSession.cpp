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

//
// WorldSession.cpp
//

#include "StdAfx.h"

#define WORLDSOCKET_TIMEOUT		 80

OpcodeHandler WorldPacketHandlers[NUM_MSG_TYPES];

WorldSession::WorldSession(uint32 id, string Name, WorldSocket *sock) : _player(0), _socket(sock), _accountId(id), _accountName(Name),
_logoutTime(0), permissions(NULL), permissioncount(0), _loggingOut(false), instanceId(0)
{
	memset(movement_packet, 0, sizeof(movement_packet));
	m_currMsTime = getMSTime();
	bDeleted = false;
	m_bIsWLevelSet = false;
	floodLines = 0;
	floodTime = World::UNIXTIME;
	_updatecount = 0;
	m_moveDelayTime=0;

	for(uint32 x=0;x<8;x++)
		sAccountData[x].data=NULL;	
}

WorldSession::~WorldSession()
{
	deleteMutex.Acquire();

	if(HasFlag(ACCOUNT_FLAG_XTEND_INFO))
		sWorld.RemoveExtendedSession(this);

	if(HasGMPermissions())
		sWorld.gmList.erase(this);

	if(_player)
	{
		sLog.outError("warning: logged out player in worldsession destructor");
		LogoutPlayer(true);
	}

	if(permissions)
		delete [] permissions;

	WorldPacket *packet;

	while((packet = _recvQueue.Pop()))
		delete packet;

	for(uint32 x=0;x<8;x++)
	{
		if(sAccountData[x].data)
			delete [] sAccountData[x].data;
	}

#ifndef CLUSTERING
	if(_socket)
		_socket->SetSession(0);
#endif

	deleteMutex.Release();
}

int WorldSession::Update(uint32 InstanceID)
{
	m_currMsTime = getMSTime();
	WorldPacket *packet;
	OpcodeHandler * Handler;

	if(InstanceID != instanceId)
	{
		// We're being updated by the wrong thread.
		// "Remove us!" - 2
		return 2;
	}

	// Socket disconnection.
	if(!_socket)
	{
		// Check if the player is in the process of being moved. We can't delete him
		// if we are.
		if(_player && _player->m_beingPushed)
		{
			// Abort..
			return 0;
		}

		if(_player && _player->DuelingWith)
			_player->EndDuel(DUEL_WINNER_RETREAT);

		bDeleted = true;
		LogoutPlayer(true);
		// 1 - Delete session completely.
		return 1;
	}

	while ((packet = _recvQueue.Pop()))
	{
		ASSERT(packet);

		if(packet->GetOpcode() >= NUM_MSG_TYPES)
			sLog.outDebug("[Session] Received out of range packet with opcode 0x%.4X", packet->GetOpcode());
		else
		{
			Handler = &WorldPacketHandlers[packet->GetOpcode()];
			if(Handler->status == STATUS_LOGGEDIN && !_player && Handler->handler != 0)
			{
				sLog.outDebug("[Session] Received unexpected/wrong state packet with opcode %s (0x%.4X)",
					LookupName(packet->GetOpcode(), g_worldOpcodeNames), packet->GetOpcode());
			}
			else
			{
				// Valid Packet :>
				if(Handler->handler == 0)
				{
					sLog.outDebug("[Session] Received unhandled packet with opcode %s (0x%.4X)",
						LookupName(packet->GetOpcode(), g_worldOpcodeNames), packet->GetOpcode());
				}
				else
				{
					(this->*Handler->handler)(*packet);
				}
			}
		}

		delete packet;

		if(InstanceID != instanceId)
		{
			// If we hit this -> means a packet has changed our map.
			return 2;
		}
	}

	if(InstanceID != instanceId)
	{
		// If we hit this -> means a packet has changed our map.
		return 2;
	}

	if( _logoutTime && (m_currMsTime >= _logoutTime) && instanceId == InstanceID)
	{
		// Check if the player is in the process of being moved. We can't delete him
		// if we are.
		if(_player && _player->m_beingPushed)
		{
			// Abort..
			return 0;
		}

		if(_player && _player->m_Group)
			_player->m_Group->RemovePlayer(_player->m_playerInfo, _player, true);

		LogoutPlayer(true);
	}

	if(m_lastPing + WORLDSOCKET_TIMEOUT < (uint32)time(NULL))
	{
		// Check if the player is in the process of being moved. We can't delete him
		// if we are.
		if(_player && _player->m_beingPushed)
		{
			// Abort..
			return 0;
		}

		// ping timeout!
		Disconnect();
		bDeleted = true;
		LogoutPlayer(true);

		// 1 - Complete deletion
		return 1;
	}

#ifndef CLUSTERING
	// 0 - OK!
	if(!((++_updatecount) % 2) && _socket)
		_socket->UpdateQueuedPackets();
#endif

	return 0;
}


void WorldSession::LogoutPlayer(bool Save)
{
	if(_loggingOut)
		return;

	_loggingOut = true;
	if (_player)
	{
		if(_player->m_currentLoot && _player->IsInWorld())
		{
			Object * obj = _player->GetMapMgr()->_GetObject(_player->m_currentLoot);
			if(obj)
			{
				switch(obj->GetTypeId())
				{
				case TYPEID_UNIT:
					((Creature*)obj)->loot.looters.erase(_player->GetGUID());
					break;
				case TYPEID_GAMEOBJECT:
					((GameObject*)obj)->loot.looters.erase(_player->GetGUID());
					break;
				}
			}
		}

		if(_player->m_CurrentTransporter)
			_player->m_CurrentTransporter->RemovePlayer(_player);
		
	  

		// cancel current spell
		if(_player->m_currentSpell)
			_player->m_currentSpell->cancel();

		sSocialMgr.LoggedOut(_player);

		// messages
		if(HasGMPermissions())
		{
			sWorld.BroadcastExtendedMessage(this, "[SM:GMLOGOUT]%s", _player->GetName());
		}

		if(_player->GetTeam() == 1)
			sWorld.HordePlayers--;
		else
			sWorld.AlliancePlayers--;

		if(_player->m_bg)
			_player->m_bg->RemovePlayer(_player, true);

		if(_player->m_bgIsQueued)
			BattlegroundManager.RemovePlayerFromQueues(_player);

		// send info
		sWorld.BroadcastExtendedMessage(0, "[SM:INFO:%u:%u]", sWorld.HordePlayers, sWorld.AlliancePlayers);

		//Duel Cancel on Leave
		if(_player->DuelingWith != NULL)
			_player->EndDuel(DUEL_WINNER_RETREAT);

		// wipe our attacker list and target list
		_player->clearAttackers(true);

		//Issue a message telling all guild members that this player signed off
		if(_player->IsInGuild())
		{
			Guild *pGuild = objmgr.GetGuild( _player->GetGuildId() );
			if(pGuild)
			{
				//Update Offline info
				pGuild->GuildMemberLogoff(_player);
				WorldPacket data(SMSG_GUILD_EVENT, 11+strlen(_player->GetName()));
				data << uint8(GUILD_EVENT_HASGONEOFFLINE);
				data << uint8(0x01);
				data << _player->GetName();
				data << _player->GetGUID();
				pGuild->SendMessageToGuild(0,&data,G_MSGTYPE_ALL);
			}
		}

		_player->GetItemInterface()->EmptyBuyBack();
		
		// Remove ourself from a group
		Group * group = _player->GetGroup();
		
		for(int i=0;i<3;++i)
		{
			if(_player->LfgDungeonId[i] != 0)
				sLfgMgr.RemoveFromLfgQueue(_player,_player->LfgDungeonId[i]);	   
		}
		
		// Save HP/Mana
		_player->load_health = _player->GetUInt32Value(UNIT_FIELD_HEALTH);
		_player->load_mana = _player->GetUInt32Value(UNIT_FIELD_POWER1);
		
		objmgr.RemovePlayer(_player);		
		_player->ok_to_remove = true;

		if(_player->GetSummon() != NULL)
			_player->GetSummon()->Remove(false, true, false);

   //	 _player->SaveAuras();
		if(Save)
			_player->SaveToDB(false);
		
		_player->RemoveAllAuras();
		  if(_player->IsInWorld())
			_player->RemoveFromWorld();
		
		  if(group)
		  {
			  group->RemovePlayer(_player->m_playerInfo, NULL, false);
			  _player->m_Group =NULL;
		  }
	  
		// Remove the "player locked" flag, to allow movement on next login
		if ( GetPlayer( )->GetUInt32Value(UNIT_FIELD_FLAGS) & U_FIELD_FLAG_LOCK_PLAYER )
			GetPlayer( )->RemoveFlag( UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER );

		// Save Honor Points
		_player->SaveHonorFields();

		// Update any dirty account_data fields.
		/*bool dirty=false;
		std::stringstream ss;
		ss << "UPDATE accounts SET ";
		for(uint32 ui=0;ui<8;ui++)
		{
			if(sAccountData[ui].bIsDirty)
			{
				if(dirty)
					ss <<",";
				ss << "uiconfig"<< ui <<"=\"";
				if(sAccountData[ui].data)
					ss.write(sAccountData[ui].data,sAccountData[ui].sz);
				ss << "\"";
				dirty=true;
			}
		}			
		if(dirty)
		{
			ss	<<" WHERE acct="<< _accountId <<";";
			sLogonDatabase.Execute(ss.str().c_str());
		}*/

		delete _player;
		_player = NULL;

		OutPacket(SMSG_LOGOUT_COMPLETE, 0, NULL);
		sLog.outDebug( "SESSION: Sent SMSG_LOGOUT_COMPLETE Message" );
	}
	_loggingOut = false;

	SetLogoutTimer(0);
}

void WorldSession::BuildItemPushResult(WorldPacket *data, uint64 guid, uint32 type, uint32 count, uint32 itemid, uint32 randomprop, uint8 unk, uint32 unk2, uint32 unk3, uint32 count_have)
{
    ItemPrototype *proto = ItemPrototypeStorage.LookupEntry(itemid);
    if(!proto) return;

    data->Initialize(SMSG_ITEM_PUSH_RESULT);
    *data << guid;
    *data << type << unk2;
    *data << uint32(1);      // show msg
    *data << unk;
    *data << unk3;
    *data << itemid;
	//not sure if random property. 
	//For quest items also sent at loot list but it's changing in time.
	//in SMSG_ITEM_PUSH_RESULT packet it's used as sent in first occurance of lootlist packet
    *data << randomprop; 
    *data << proto->Quality;
    *data << count;
    *data << count_have;
}

void WorldSession::SendBuyFailed(uint64 guid, uint32 itemid, uint8 error)
{
	WorldPacket data(13);
	data.SetOpcode(SMSG_BUY_FAILED);
	data << guid << itemid << error;
	_socket->SendPacket(&data);
}

void WorldSession::SendSellItem(uint64 vendorguid, uint64 itemid, uint8 error)
{
	WorldPacket data(17);
	data.SetOpcode(SMSG_SELL_ITEM);
	data << vendorguid << itemid << error;
	_socket->SendPacket(&data);
}

void WorldSession::LoadSecurity(std::string securitystring)
{
	std::list<char> tmp;
	bool hasa = false;
	for(uint32 i = 0; i < securitystring.length(); ++i)
	{
		char c = securitystring.at(i);
		c = towlower(c);
		if(c == '4' || c == '3')
			c = 'a';	// for the lazy people

		if(c == 'a')
		{
			// all permissions
			tmp.push_back('a');
			hasa = true;
		}
		else if(!hasa && (c == '0') && i == 0)
			break;
		else if(!hasa || (hasa && (c == 'z')))
		{
			tmp.push_back(c);
		}
	}

	permissions = new char[tmp.size()+1];
	memset(permissions, 0, tmp.size()+1);
	permissioncount = tmp.size();
	int k = 0;
	for(std::list<char>::iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
		permissions[k++] = (*itr);
	
	if(permissions[tmp.size()] != 0)
		permissions[tmp.size()] = 0;

	sLog.outDebug("Loaded permissions for %u. (%u) : [%s]", this->GetAccountId(), permissioncount, securitystring.c_str());
}

void WorldSession::SetSecurity(std::string securitystring)
{
	delete [] permissions;
	LoadSecurity(securitystring);

	// update db
	CharacterDatabase.Execute("UPDATE accounts SET gm=\"%s\" WHERE acct=%u", CharacterDatabase.EscapeString(string(permissions)).c_str(), _accountId);
}

bool WorldSession::CanUseCommand(char cmdstr)
{
	if(permissioncount == 0)
		return false;
	if(cmdstr == 0)
		return true;
	if(permissions[0] == 'a' && cmdstr != 'z')   // all
		return true;

	for(int i = 0; i < permissioncount; ++i)
		if(permissions[i] == cmdstr)
			return true;

	return false;
}

void WorldSession::SendNotification(const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	WorldPacket data(SMSG_NOTIFICATION, strlen(msg1) + 1);
	data << msg1;
	SendPacket(&data);
}

void WorldSession::InitPacketHandlerTable()
{
	// Nullify Everything, default to STATUS_LOGGEDIN
	for(uint32 i = 0; i < NUM_MSG_TYPES; ++i)
	{
		WorldPacketHandlers[i].status = STATUS_LOGGEDIN;
		WorldPacketHandlers[i].handler = 0;
	}
	// Login
	WorldPacketHandlers[CMSG_CHAR_ENUM].handler								 = &WorldSession::HandleCharEnumOpcode;
	WorldPacketHandlers[CMSG_CHAR_ENUM].status								  = STATUS_AUTHED;

	WorldPacketHandlers[CMSG_CHAR_CREATE].handler							   = &WorldSession::HandleCharCreateOpcode;
	WorldPacketHandlers[CMSG_CHAR_CREATE].status								= STATUS_AUTHED;

	WorldPacketHandlers[CMSG_CHAR_DELETE].handler							   = &WorldSession::HandleCharDeleteOpcode;
	WorldPacketHandlers[CMSG_CHAR_DELETE].status								= STATUS_AUTHED;

	WorldPacketHandlers[CMSG_CHAR_RENAME].handler							   = &WorldSession::HandleCharRenameOpcode;
	WorldPacketHandlers[CMSG_CHAR_RENAME].status								= STATUS_AUTHED;

	WorldPacketHandlers[CMSG_PLAYER_LOGIN].handler							  = &WorldSession::HandlePlayerLoginOpcode; 
	WorldPacketHandlers[CMSG_PLAYER_LOGIN].status							   = STATUS_AUTHED;

	// Queries
	WorldPacketHandlers[MSG_CORPSE_QUERY].handler							   = &WorldSession::HandleCorpseQueryOpcode;
	WorldPacketHandlers[CMSG_NAME_QUERY].handler								= &WorldSession::HandleNameQueryOpcode;
	WorldPacketHandlers[CMSG_QUERY_TIME].handler								= &WorldSession::HandleQueryTimeOpcode;
	WorldPacketHandlers[CMSG_CREATURE_QUERY].handler							= &WorldSession::HandleCreatureQueryOpcode;
	WorldPacketHandlers[CMSG_GAMEOBJECT_QUERY].handler						  = &WorldSession::HandleGameObjectQueryOpcode;
	WorldPacketHandlers[CMSG_PAGE_TEXT_QUERY].handler						   = &WorldSession::HandlePageTextQueryOpcode;
	WorldPacketHandlers[CMSG_ITEM_NAME_QUERY].handler						   = &WorldSession::HandleItemNameQueryOpcode;

	// Movement
	WorldPacketHandlers[MSG_MOVE_HEARTBEAT].handler							 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_WORLDPORT_ACK].handler						 = &WorldSession::HandleMoveWorldportAckOpcode;
	WorldPacketHandlers[MSG_MOVE_JUMP].handler								  = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[CMSG_FLY_PITCH_UP_Z].handler							= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[CMSG_FLY_PITCH_DOWN_AFTER_UP].handler				   = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_FORWARD].handler						 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_BACKWARD].handler						= &WorldSession::HandleMovementOpcodes;
    WorldPacketHandlers[MSG_MOVE_SET_FACING].handler                            = &WorldSession::HandleMovementOpcodes;
    WorldPacketHandlers[MSG_MOVE_START_STRAFE_LEFT].handler					 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_STRAFE_RIGHT].handler					= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_STRAFE].handler						   = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_TURN_LEFT].handler					   = &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_TURN_RIGHT].handler					  = &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_TURN].handler							 = &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_PITCH_UP].handler						= &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_PITCH_DOWN].handler					  = &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_PITCH].handler							= &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_SET_RUN_MODE].handler						  = &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_SET_WALK_MODE].handler						 = &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_SET_PITCH].handler							 = &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_SWIM].handler							= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_SWIM].handler							 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_FALL_LAND].handler							 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP].handler								  = &WorldSession::HandleMoveStopOpcode;
	WorldPacketHandlers[CMSG_MOVE_FLY_START_AND_END].handler					= &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[CMSG_FLY_PITCH_UP_Z].handler							= &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[CMSG_FLY_PITCH_DOWN_AFTER_UP].handler				   = &WorldSession::HandleBasicMovementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_TIME_SKIPPED].handler						 = &WorldSession::HandleMoveTimeSkippedOpcode;
	WorldPacketHandlers[CMSG_MOVE_NOT_ACTIVE_MOVER].handler					 = &WorldSession::HandleMoveNotActiveMoverOpcode;
	WorldPacketHandlers[CMSG_SET_ACTIVE_MOVER].handler						  = &WorldSession::HandleSetActiveMoverOpcode;
    WorldPacketHandlers[CMSG_TRANSPORT_STEP_IN_OUT].handler                         = &WorldSession::HandleMovementOpcodes;
	// ACK
	WorldPacketHandlers[MSG_MOVE_TELEPORT_ACK].handler						  = &WorldSession::HandleMoveTeleportAckOpcode;
	WorldPacketHandlers[CMSG_FORCE_WALK_SPEED_CHANGE_ACK].handler			   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_FEATHER_FALL_ACK].handler					 = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_WATER_WALK_ACK].handler					   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK].handler		  = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_TURN_RATE_CHANGE_ACK].handler				= &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_RUN_SPEED_CHANGE_ACK].handler				= &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK].handler		   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_SWIM_SPEED_CHANGE_ACK].handler			   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_MOVE_ROOT_ACK].handler					   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_MOVE_UNROOT_ACK].handler					 = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_KNOCK_BACK_ACK].handler					   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_HOVER_ACK].handler							= &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[MSG_MOVE_FLY_DOWN_UNK].handler							= &WorldSession::HandleMovementOpcodes;
	
	// Action Buttons
	WorldPacketHandlers[CMSG_SET_ACTION_BUTTON].handler						 = &WorldSession::HandleSetActionButtonOpcode;
	WorldPacketHandlers[CMSG_REPOP_REQUEST].handler							 = &WorldSession::HandleRepopRequestOpcode;
		
	// Loot
	WorldPacketHandlers[CMSG_AUTOSTORE_LOOT_ITEM].handler					   = &WorldSession::HandleAutostoreLootItemOpcode;
	WorldPacketHandlers[CMSG_LOOT_MONEY].handler								= &WorldSession::HandleLootMoneyOpcode;
	WorldPacketHandlers[CMSG_LOOT].handler									  = &WorldSession::HandleLootOpcode;
	WorldPacketHandlers[CMSG_LOOT_RELEASE].handler							  = &WorldSession::HandleLootReleaseOpcode;
	WorldPacketHandlers[CMSG_LOOT_ROLL].handler								 = &WorldSession::HandleLootRollOpcode;
	WorldPacketHandlers[CMSG_LOOT_MASTER_GIVE].handler						  = &WorldSession::HandleLootMasterGiveOpcode;
	
	// Player Interaction
	WorldPacketHandlers[CMSG_WHO].handler									   = &WorldSession::HandleWhoOpcode;
	WorldPacketHandlers[CMSG_LOGOUT_REQUEST].handler							= &WorldSession::HandleLogoutRequestOpcode;
	WorldPacketHandlers[CMSG_PLAYER_LOGOUT].handler							 = &WorldSession::HandlePlayerLogoutOpcode;
	WorldPacketHandlers[CMSG_LOGOUT_CANCEL].handler							 = &WorldSession::HandleLogoutCancelOpcode;
	WorldPacketHandlers[CMSG_ZONEUPDATE].handler								= &WorldSession::HandleZoneUpdateOpcode;
	WorldPacketHandlers[CMSG_SET_TARGET_OBSOLETE].handler					   = &WorldSession::HandleSetTargetOpcode;
	WorldPacketHandlers[CMSG_SET_SELECTION].handler							 = &WorldSession::HandleSetSelectionOpcode;
	WorldPacketHandlers[CMSG_STANDSTATECHANGE].handler						  = &WorldSession::HandleStandStateChangeOpcode;
	WorldPacketHandlers[CMSG_DISMOUNT].handler								= &WorldSession::HandleDismountOpcode;
	
	// Friends
	WorldPacketHandlers[CMSG_FRIEND_LIST].handler							   = &WorldSession::HandleFriendListOpcode;
	WorldPacketHandlers[CMSG_ADD_FRIEND].handler								= &WorldSession::HandleAddFriendOpcode;
	WorldPacketHandlers[CMSG_DEL_FRIEND].handler								= &WorldSession::HandleDelFriendOpcode;
	WorldPacketHandlers[CMSG_ADD_IGNORE].handler								= &WorldSession::HandleAddIgnoreOpcode;
	WorldPacketHandlers[CMSG_DEL_IGNORE].handler								= &WorldSession::HandleDelIgnoreOpcode;
	WorldPacketHandlers[CMSG_BUG].handler									   = &WorldSession::HandleBugOpcode;
	
	// Areatrigger
	WorldPacketHandlers[CMSG_AREATRIGGER].handler							   = &WorldSession::HandleAreaTriggerOpcode;
	
	// Account Data
	WorldPacketHandlers[CMSG_UPDATE_ACCOUNT_DATA].handler					   = &WorldSession::HandleUpdateAccountData;
	WorldPacketHandlers[CMSG_REQUEST_ACCOUNT_DATA].handler					  = &WorldSession::HandleRequestAccountData;
	WorldPacketHandlers[CMSG_SET_FACTION_ATWAR].handler						 = &WorldSession::HandleSetAtWarOpcode;
	WorldPacketHandlers[CMSG_SET_WATCHED_FACTION_INDEX].handler				 = &WorldSession::HandleSetWatchedFactionIndexOpcode;
	WorldPacketHandlers[CMSG_TOGGLE_PVP].handler								= &WorldSession::HandleTogglePVPOpcode;
	
	// Player Interaction
	WorldPacketHandlers[CMSG_GAMEOBJ_USE].handler							   = &WorldSession::HandleGameObjectUse;
	WorldPacketHandlers[CMSG_PLAYED_TIME].handler							   = &WorldSession::HandlePlayedTimeOpcode;
	WorldPacketHandlers[CMSG_SETSHEATHED].handler							   = &WorldSession::HandleSetSheathedOpcode;
	WorldPacketHandlers[CMSG_MESSAGECHAT].handler							   = &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_TEXT_EMOTE].handler								= &WorldSession::HandleTextEmoteOpcode;
	
	// Channels
	WorldPacketHandlers[CMSG_JOIN_CHANNEL].handler							  = &WorldSession::HandleChannelJoin;
	WorldPacketHandlers[CMSG_LEAVE_CHANNEL].handler							 = &WorldSession::HandleChannelLeave;
	WorldPacketHandlers[CMSG_CHANNEL_LIST].handler							  = &WorldSession::HandleChannelList;
	WorldPacketHandlers[CMSG_CHANNEL_PASSWORD].handler						  = &WorldSession::HandleChannelPassword;
	WorldPacketHandlers[CMSG_CHANNEL_SET_OWNER].handler						 = &WorldSession::HandleChannelSetOwner;
	WorldPacketHandlers[CMSG_CHANNEL_OWNER].handler							 = &WorldSession::HandleChannelOwner;
	WorldPacketHandlers[CMSG_CHANNEL_MODERATOR].handler						 = &WorldSession::HandleChannelModerator;
	WorldPacketHandlers[CMSG_CHANNEL_UNMODERATOR].handler					   = &WorldSession::HandleChannelUnmoderator;
	WorldPacketHandlers[CMSG_CHANNEL_MUTE].handler							  = &WorldSession::HandleChannelMute;
	WorldPacketHandlers[CMSG_CHANNEL_UNMUTE].handler							= &WorldSession::HandleChannelUnmute;
	WorldPacketHandlers[CMSG_CHANNEL_INVITE].handler							= &WorldSession::HandleChannelInvite;
	WorldPacketHandlers[CMSG_CHANNEL_KICK].handler							  = &WorldSession::HandleChannelKick;
	WorldPacketHandlers[CMSG_CHANNEL_BAN].handler							   = &WorldSession::HandleChannelBan;
	WorldPacketHandlers[CMSG_CHANNEL_UNBAN].handler							 = &WorldSession::HandleChannelUnban;
	WorldPacketHandlers[CMSG_CHANNEL_ANNOUNCEMENTS].handler					 = &WorldSession::HandleChannelAnnounce;
	WorldPacketHandlers[CMSG_CHANNEL_MODERATE].handler						  = &WorldSession::HandleChannelModerate;
	
	// Groups / Raids
	WorldPacketHandlers[CMSG_GROUP_INVITE].handler							  = &WorldSession::HandleGroupInviteOpcode;
	WorldPacketHandlers[CMSG_GROUP_CANCEL].handler							  = &WorldSession::HandleGroupCancelOpcode;
	WorldPacketHandlers[CMSG_GROUP_ACCEPT].handler							  = &WorldSession::HandleGroupAcceptOpcode;
	WorldPacketHandlers[CMSG_GROUP_DECLINE].handler							 = &WorldSession::HandleGroupDeclineOpcode;
	WorldPacketHandlers[CMSG_GROUP_UNINVITE].handler							= &WorldSession::HandleGroupUninviteOpcode;
	WorldPacketHandlers[CMSG_GROUP_UNINVITE_GUID].handler					   = &WorldSession::HandleGroupUninviteGuildOpcode;
	WorldPacketHandlers[CMSG_GROUP_SET_LEADER].handler						  = &WorldSession::HandleGroupSetLeaderOpcode;
	WorldPacketHandlers[CMSG_GROUP_DISBAND].handler							 = &WorldSession::HandleGroupDisbandOpcode;
	WorldPacketHandlers[CMSG_LOOT_METHOD].handler							   = &WorldSession::HandleLootMethodOpcode;
	WorldPacketHandlers[MSG_MINIMAP_PING].handler							   = &WorldSession::HandleMinimapPingOpcode;
	WorldPacketHandlers[CMSG_GROUP_RAID_CONVERT].handler						= &WorldSession::HandleConvertGroupToRaidOpcode;
	WorldPacketHandlers[CMSG_GROUP_CHANGE_SUB_GROUP].handler					= &WorldSession::HandleGroupChangeSubGroup;
	WorldPacketHandlers[CMSG_GROUP_ASSISTANT_LEADER].handler					= &WorldSession::HandleGroupAssistantLeader;
	WorldPacketHandlers[CMSG_REQUEST_RAID_INFO].handler						 = &WorldSession::HandleRequestRaidInfoOpcode;
	WorldPacketHandlers[CMSG_RAID_READYCHECK].handler						   = &WorldSession::HandleReadyCheckOpcode;
	WorldPacketHandlers[MSG_GROUP_SET_PLAYER_ICON].handler					  = &WorldSession::HandleSetPlayerIconOpcode;
	WorldPacketHandlers[CMSG_REQUEST_PARTY_MEMBER_STATS].handler				= &WorldSession::HandlePartyMemberStatsOpcode;

	// LFG System
	WorldPacketHandlers[CMSG_SET_LOOKING_FOR_GROUP_COMMENT].handler			 = &WorldSession::HandleSetLookingForGroupComment;
	WorldPacketHandlers[MSG_LOOKING_FOR_GROUP].handler						  = &WorldSession::HandleMsgLookingForGroup;
	WorldPacketHandlers[CMSG_SET_LOOKING_FOR_GROUP].handler					 = &WorldSession::HandleSetLookingForGroup;
	WorldPacketHandlers[CMSG_ENABLE_AUTOJOIN].handler						   = &WorldSession::HandleEnableAutoJoin;
	WorldPacketHandlers[CMSG_DISABLE_AUTOJOIN].handler						  = &WorldSession::HandleDisableAutoJoin;
	WorldPacketHandlers[CMSG_ENABLE_AUTOADD_MEMBERS].handler					= &WorldSession::HandleEnableAutoAddMembers;
	WorldPacketHandlers[CMSG_DISABLE_AUTOADD_MEMBERS].handler				   = &WorldSession::HandleDisableAutoAddMembers;
	
	// Taxi / NPC Interaction
	WorldPacketHandlers[CMSG_TAXINODE_STATUS_QUERY].handler					 = &WorldSession::HandleTaxiNodeStatusQueryOpcode;
	WorldPacketHandlers[CMSG_TAXIQUERYAVAILABLENODES].handler				   = &WorldSession::HandleTaxiQueryAvaibleNodesOpcode;
	WorldPacketHandlers[CMSG_ACTIVATETAXI].handler							  = &WorldSession::HandleActivateTaxiOpcode;
	WorldPacketHandlers[MSG_TABARDVENDOR_ACTIVATE].handler					  = &WorldSession::HandleTabardVendorActivateOpcode;
	WorldPacketHandlers[CMSG_BANKER_ACTIVATE].handler						   = &WorldSession::HandleBankerActivateOpcode;
	WorldPacketHandlers[CMSG_BUY_BANK_SLOT].handler							 = &WorldSession::HandleBuyBankSlotOpcode;
	WorldPacketHandlers[CMSG_TRAINER_LIST].handler							  = &WorldSession::HandleTrainerListOpcode;
	WorldPacketHandlers[CMSG_TRAINER_BUY_SPELL].handler						 = &WorldSession::HandleTrainerBuySpellOpcode;
	WorldPacketHandlers[CMSG_PETITION_SHOWLIST].handler						 = &WorldSession::HandleCharterShowListOpcode;
	WorldPacketHandlers[MSG_AUCTION_HELLO].handler							  = &WorldSession::HandleAuctionHelloOpcode;
	WorldPacketHandlers[CMSG_GOSSIP_HELLO].handler							  = &WorldSession::HandleGossipHelloOpcode;
	WorldPacketHandlers[CMSG_GOSSIP_SELECT_OPTION].handler					  = &WorldSession::HandleGossipSelectOptionOpcode;
	WorldPacketHandlers[CMSG_SPIRIT_HEALER_ACTIVATE].handler					= &WorldSession::HandleSpiritHealerActivateOpcode;
	WorldPacketHandlers[CMSG_NPC_TEXT_QUERY].handler							= &WorldSession::HandleNpcTextQueryOpcode;
	WorldPacketHandlers[CMSG_BINDER_ACTIVATE].handler						   = &WorldSession::HandleBinderActivateOpcode;
	WorldPacketHandlers[CMSG_ACTIVATE_MULTIPLE_TAXI].handler					= &WorldSession::HandleMultipleActivateTaxiOpcode;
	
	// Item / Vendors
	WorldPacketHandlers[CMSG_SWAP_INV_ITEM].handler							 = &WorldSession::HandleSwapInvItemOpcode;
	WorldPacketHandlers[CMSG_SWAP_ITEM].handler								 = &WorldSession::HandleSwapItemOpcode;
	WorldPacketHandlers[CMSG_DESTROYITEM].handler							   = &WorldSession::HandleDestroyItemOpcode;
	WorldPacketHandlers[CMSG_AUTOEQUIP_ITEM].handler							= &WorldSession::HandleAutoEquipItemOpcode;
	WorldPacketHandlers[CMSG_ITEM_QUERY_SINGLE].handler						 = &WorldSession::HandleItemQuerySingleOpcode;
	WorldPacketHandlers[CMSG_SELL_ITEM].handler								 = &WorldSession::HandleSellItemOpcode;
	WorldPacketHandlers[CMSG_BUY_ITEM_IN_SLOT].handler						  = &WorldSession::HandleBuyItemInSlotOpcode;
	WorldPacketHandlers[CMSG_BUY_ITEM].handler								  = &WorldSession::HandleBuyItemOpcode;
	WorldPacketHandlers[CMSG_LIST_INVENTORY].handler							= &WorldSession::HandleListInventoryOpcode;
	WorldPacketHandlers[CMSG_AUTOSTORE_BAG_ITEM].handler						= &WorldSession::HandleAutoStoreBagItemOpcode;
	WorldPacketHandlers[CMSG_SET_AMMO].handler								  = &WorldSession::HandleAmmoSetOpcode;
	WorldPacketHandlers[CMSG_BUYBACK_ITEM].handler							  = &WorldSession::HandleBuyBackOpcode;
	WorldPacketHandlers[CMSG_SPLIT_ITEM].handler								= &WorldSession::HandleSplitOpcode;
	WorldPacketHandlers[CMSG_READ_ITEM].handler								 = &WorldSession::HandleReadItemOpcode;
	WorldPacketHandlers[CMSG_REPAIR_ITEM].handler							   = &WorldSession::HandleRepairItemOpcode;
	WorldPacketHandlers[CMSG_AUTOBANK_ITEM].handler							 = &WorldSession::HandleAutoBankItemOpcode;
	WorldPacketHandlers[CMSG_AUTOSTORE_BANK_ITEM].handler					   = &WorldSession::HandleAutoStoreBankItemOpcode;
	WorldPacketHandlers[CMSG_CANCEL_TEMPORARY_ENCHANTMENT].handler			  = &WorldSession::HandleCancelTemporaryEnchantmentOpcode;
	WorldPacketHandlers[CMSG_SOCKET_GEMS].handler								= &WorldSession::HandleInsertGemOpcode;
	
	// Spell System / Talent System
	WorldPacketHandlers[CMSG_USE_ITEM].handler								  = &WorldSession::HandleUseItemOpcode;
	WorldPacketHandlers[CMSG_CAST_SPELL].handler								= &WorldSession::HandleCastSpellOpcode;
	WorldPacketHandlers[CMSG_CANCEL_CAST].handler							   = &WorldSession::HandleCancelCastOpcode;
	WorldPacketHandlers[CMSG_CANCEL_AURA].handler							   = &WorldSession::HandleCancelAuraOpcode;
	WorldPacketHandlers[CMSG_CANCEL_CHANNELLING].handler						= &WorldSession::HandleCancelChannellingOpcode;
	WorldPacketHandlers[CMSG_CANCEL_AUTO_REPEAT_SPELL].handler				  = &WorldSession::HandleCancelAutoRepeatSpellOpcode;
	WorldPacketHandlers[CMSG_LEARN_TALENT].handler							  = &WorldSession::HandleLearnTalentOpcode;
	WorldPacketHandlers[CMSG_UNLEARN_TALENTS].handler						   = &WorldSession::HandleUnlearnTalents;
	WorldPacketHandlers[MSG_TALENT_WIPE_CONFIRM].handler						= &WorldSession::HandleUnlearnTalents;
	
	// Combat / Duel
	WorldPacketHandlers[CMSG_ATTACKSWING].handler							   = &WorldSession::HandleAttackSwingOpcode;
	WorldPacketHandlers[CMSG_ATTACKSTOP].handler								= &WorldSession::HandleAttackStopOpcode;
	WorldPacketHandlers[CMSG_DUEL_ACCEPTED].handler							 = &WorldSession::HandleDuelAccepted;
	WorldPacketHandlers[CMSG_DUEL_CANCELLED].handler							= &WorldSession::HandleDuelCancelled;
	
	// Trade
	WorldPacketHandlers[CMSG_INITIATE_TRADE].handler							= &WorldSession::HandleInitiateTrade;
	WorldPacketHandlers[CMSG_BEGIN_TRADE].handler							   = &WorldSession::HandleBeginTrade;
	WorldPacketHandlers[CMSG_BUSY_TRADE].handler								= &WorldSession::HandleBusyTrade;
	WorldPacketHandlers[CMSG_IGNORE_TRADE].handler							  = &WorldSession::HandleIgnoreTrade;
	WorldPacketHandlers[CMSG_ACCEPT_TRADE].handler							  = &WorldSession::HandleAcceptTrade;
	WorldPacketHandlers[CMSG_UNACCEPT_TRADE].handler							= &WorldSession::HandleUnacceptTrade;
	WorldPacketHandlers[CMSG_CANCEL_TRADE].handler							  = &WorldSession::HandleCancelTrade;
	WorldPacketHandlers[CMSG_SET_TRADE_ITEM].handler							= &WorldSession::HandleSetTradeItem;
	WorldPacketHandlers[CMSG_CLEAR_TRADE_ITEM].handler						  = &WorldSession::HandleClearTradeItem;
	WorldPacketHandlers[CMSG_SET_TRADE_GOLD].handler							= &WorldSession::HandleSetTradeGold;
	
	// Quest System
	WorldPacketHandlers[CMSG_QUESTGIVER_STATUS_QUERY].handler				   = &WorldSession::HandleQuestgiverStatusQueryOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_HELLO].handler						  = &WorldSession::HandleQuestgiverHelloOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_ACCEPT_QUEST].handler				   = &WorldSession::HandleQuestgiverAcceptQuestOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_CANCEL].handler						 = &WorldSession::HandleQuestgiverCancelOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_CHOOSE_REWARD].handler				  = &WorldSession::HandleQuestgiverChooseRewardOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_REQUEST_REWARD].handler				 = &WorldSession::HandleQuestgiverRequestRewardOpcode;
	WorldPacketHandlers[CMSG_QUEST_QUERY].handler							   = &WorldSession::HandleQuestQueryOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_QUERY_QUEST].handler					= &WorldSession::HandleQuestGiverQueryQuestOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_COMPLETE_QUEST].handler				 = &WorldSession::HandleQuestgiverCompleteQuestOpcode;
	WorldPacketHandlers[CMSG_QUESTLOG_REMOVE_QUEST].handler					 = &WorldSession::HandleQuestlogRemoveQuestOpcode;
	WorldPacketHandlers[CMSG_RECLAIM_CORPSE].handler							= &WorldSession::HandleCorpseReclaimOpcode;
	WorldPacketHandlers[CMSG_RESURRECT_RESPONSE].handler						= &WorldSession::HandleResurrectResponseOpcode;
	WorldPacketHandlers[CMSG_PUSHQUESTTOPARTY].handler						  = &WorldSession::HandlePushQuestToPartyOpcode;
	WorldPacketHandlers[MSG_QUEST_PUSH_RESULT].handler						  = &WorldSession::HandleQuestPushResult;
	
	// Auction System
	WorldPacketHandlers[CMSG_AUCTION_LIST_ITEMS].handler						= &WorldSession::HandleAuctionListItems;
	WorldPacketHandlers[CMSG_AUCTION_LIST_BIDDER_ITEMS].handler				 = &WorldSession::HandleAuctionListBidderItems;
	WorldPacketHandlers[CMSG_AUCTION_SELL_ITEM].handler						 = &WorldSession::HandleAuctionSellItem;
	WorldPacketHandlers[CMSG_AUCTION_LIST_OWNER_ITEMS].handler				  = &WorldSession::HandleAuctionListOwnerItems;
	WorldPacketHandlers[CMSG_AUCTION_PLACE_BID].handler						 = &WorldSession::HandleAuctionPlaceBid;
	WorldPacketHandlers[CMSG_AUCTION_REMOVE_ITEM].handler					   = &WorldSession::HandleCancelAuction;
	
	// Mail System
	WorldPacketHandlers[CMSG_GET_MAIL_LIST].handler							 = &WorldSession::HandleGetMail;
	WorldPacketHandlers[CMSG_ITEM_TEXT_QUERY].handler						   = &WorldSession::HandleItemTextQuery;
	WorldPacketHandlers[CMSG_SEND_MAIL].handler								 = &WorldSession::HandleSendMail;
	WorldPacketHandlers[CMSG_MAIL_TAKE_MONEY].handler						   = &WorldSession::HandleTakeMoney;
	WorldPacketHandlers[CMSG_MAIL_TAKE_ITEM].handler							= &WorldSession::HandleTakeItem;
	WorldPacketHandlers[CMSG_MAIL_MARK_AS_READ].handler						 = &WorldSession::HandleMarkAsRead;
	WorldPacketHandlers[CMSG_MAIL_RETURN_TO_SENDER].handler					 = &WorldSession::HandleReturnToSender;
	WorldPacketHandlers[CMSG_MAIL_DELETE].handler							   = &WorldSession::HandleMailDelete;
	WorldPacketHandlers[MSG_QUERY_NEXT_MAIL_TIME].handler					   = &WorldSession::HandleMailTime;
	WorldPacketHandlers[CMSG_MAIL_CREATE_TEXT_ITEM].handler					 = &WorldSession::HandleMailCreateTextItem;
	
	// Guild Query (called when not logged in sometimes)
	WorldPacketHandlers[CMSG_GUILD_QUERY].handler							   = &WorldSession::HandleGuildQuery;
	WorldPacketHandlers[CMSG_GUILD_QUERY].status								= STATUS_AUTHED;

	// Guild System
	WorldPacketHandlers[CMSG_GUILD_CREATE].handler							  = &WorldSession::HandleCreateGuild;
	WorldPacketHandlers[CMSG_GUILD_INVITE].handler							  = &WorldSession::HandleInviteToGuild;
	WorldPacketHandlers[CMSG_GUILD_ACCEPT].handler							  = &WorldSession::HandleGuildAccept;
	WorldPacketHandlers[CMSG_GUILD_DECLINE].handler							 = &WorldSession::HandleGuildDecline;
	WorldPacketHandlers[CMSG_GUILD_INFO].handler								= &WorldSession::HandleGuildInfo;
	WorldPacketHandlers[CMSG_GUILD_ROSTER].handler							  = &WorldSession::HandleGuildRoster;
	WorldPacketHandlers[CMSG_GUILD_PROMOTE].handler							 = &WorldSession::HandleGuildPromote;
	WorldPacketHandlers[CMSG_GUILD_DEMOTE].handler							  = &WorldSession::HandleGuildDemote;
	WorldPacketHandlers[CMSG_GUILD_LEAVE].handler							   = &WorldSession::HandleGuildLeave;
	WorldPacketHandlers[CMSG_GUILD_REMOVE].handler							  = &WorldSession::HandleGuildRemove;
	WorldPacketHandlers[CMSG_GUILD_DISBAND].handler							 = &WorldSession::HandleGuildDisband;
	WorldPacketHandlers[CMSG_GUILD_LEADER].handler							  = &WorldSession::HandleGuildLeader;
	WorldPacketHandlers[CMSG_GUILD_MOTD].handler								= &WorldSession::HandleGuildMotd;
	WorldPacketHandlers[CMSG_GUILD_RANK].handler								= &WorldSession::HandleGuildRank;
	WorldPacketHandlers[CMSG_GUILD_ADD_RANK].handler							= &WorldSession::HandleGuildAddRank;
	WorldPacketHandlers[CMSG_GUILD_DEL_RANK].handler							= &WorldSession::HandleGuildDelRank;
	WorldPacketHandlers[CMSG_GUILD_SET_PUBLIC_NOTE].handler					 = &WorldSession::HandleGuildSetPublicNote;
	WorldPacketHandlers[CMSG_GUILD_SET_OFFICER_NOTE].handler					= &WorldSession::HandleGuildSetOfficerNote;
	WorldPacketHandlers[CMSG_PETITION_BUY].handler							  = &WorldSession::HandleCharterBuy;
	WorldPacketHandlers[CMSG_PETITION_SHOW_SIGNATURES].handler				  = &WorldSession::HandleCharterShowSignatures;
	WorldPacketHandlers[CMSG_TURN_IN_PETITION].handler						  = &WorldSession::HandleCharterTurnInCharter;
	WorldPacketHandlers[CMSG_PETITION_QUERY].handler							= &WorldSession::HandleCharterQuery;
	WorldPacketHandlers[CMSG_OFFER_PETITION].handler							= &WorldSession::HandleCharterOffer;
	WorldPacketHandlers[CMSG_PETITION_SIGN].handler							 = &WorldSession::HandleCharterSign;
	WorldPacketHandlers[MSG_PETITION_RENAME].handler							= &WorldSession::HandleCharterRename;
	WorldPacketHandlers[MSG_SAVE_GUILD_EMBLEM].handler						  = &WorldSession::HandleSaveGuildEmblem;
	WorldPacketHandlers[CMSG_SET_GUILD_INFORMATION].handler					 = &WorldSession::HandleSetGuildInformation;
	
	// Tutorials
	WorldPacketHandlers[CMSG_TUTORIAL_FLAG].handler							 = &WorldSession::HandleTutorialFlag;
	WorldPacketHandlers[CMSG_TUTORIAL_CLEAR].handler							= &WorldSession::HandleTutorialClear;
	WorldPacketHandlers[CMSG_TUTORIAL_RESET].handler							= &WorldSession::HandleTutorialReset;
	
	// Pets
	WorldPacketHandlers[CMSG_PET_ACTION].handler								= &WorldSession::HandlePetAction;
	WorldPacketHandlers[CMSG_REQUEST_PET_INFO].handler						  = &WorldSession::HandlePetInfo;
	WorldPacketHandlers[CMSG_PET_NAME_QUERY].handler							= &WorldSession::HandlePetNameQuery;
	WorldPacketHandlers[CMSG_BUY_STABLE_SLOT].handler						   = &WorldSession::HandleBuyStableSlot;
	WorldPacketHandlers[CMSG_STABLE_PET].handler								= &WorldSession::HandleStablePet;
	WorldPacketHandlers[CMSG_UNSTABLE_PET].handler							  = &WorldSession::HandleUnstablePet;
	WorldPacketHandlers[MSG_LIST_STABLED_PETS].handler						  = &WorldSession::HandleStabledPetList;
	WorldPacketHandlers[CMSG_PET_SET_ACTION].handler							= &WorldSession::HandlePetSetActionOpcode;
	WorldPacketHandlers[CMSG_PET_RENAME].handler								= &WorldSession::HandlePetRename;
	WorldPacketHandlers[CMSG_PET_ABANDON].handler							   = &WorldSession::HandlePetAbandon;
	
	// Battlegrounds
	WorldPacketHandlers[CMSG_BATTLEFIELD_PORT].handler						  = &WorldSession::HandleBattlefieldPortOpcode;
	WorldPacketHandlers[CMSG_BATTLEFIELD_STATUS].handler						= &WorldSession::HandleBattlefieldStatusOpcode;
	WorldPacketHandlers[CMSG_BATTLEFIELD_LIST].handler						  = &WorldSession::HandleBattlefieldListOpcode;
	WorldPacketHandlers[CMSG_BATTLEMASTER_HELLO].handler						= &WorldSession::HandleBattleMasterHelloOpcode;
	WorldPacketHandlers[CMSG_ARENA_JOIN].handler								= &WorldSession::HandleArenaJoinOpcode;
	WorldPacketHandlers[CMSG_BATTLEMASTER_JOIN].handler						 = &WorldSession::HandleBattleMasterJoinOpcode;
	WorldPacketHandlers[CMSG_LEAVE_BATTLEFIELD].handler						 = &WorldSession::HandleLeaveBattlefieldOpcode;
	WorldPacketHandlers[CMSG_AREA_SPIRIT_HEALER_QUERY].handler				  = &WorldSession::HandleAreaSpiritHealerQueryOpcode;
	WorldPacketHandlers[CMSG_AREA_SPIRIT_HEALER_QUEUE].handler				  = &WorldSession::HandleAreaSpiritHealerQueueOpcode;
	WorldPacketHandlers[MSG_BATTLEGROUND_PLAYER_POSITIONS].handler			  = &WorldSession::HandleBattlegroundPlayerPositionsOpcode;
	WorldPacketHandlers[MSG_PVP_LOG_DATA].handler							   = &WorldSession::HandlePVPLogDataOpcode;
	WorldPacketHandlers[MSG_INSPECT_HONOR_STATS].handler						= &WorldSession::HandleInspectHonorStatsOpcode;
	WorldPacketHandlers[CMSG_SET_ACTIONBAR_TOGGLES].handler					 = &WorldSession::HandleSetActionBarTogglesOpcode;
	WorldPacketHandlers[CMSG_MOVE_SPLINE_DONE].handler						  = &WorldSession::HandleMoveSplineCompleteOpcode;
	
	// GM Ticket System
	WorldPacketHandlers[CMSG_GMTICKET_CREATE].handler						   = &WorldSession::HandleGMTicketCreateOpcode;
	WorldPacketHandlers[CMSG_GMTICKET_UPDATETEXT].handler					   = &WorldSession::HandleGMTicketUpdateOpcode;
	WorldPacketHandlers[CMSG_GMTICKET_DELETETICKET].handler					 = &WorldSession::HandleGMTicketDeleteOpcode;
	WorldPacketHandlers[CMSG_GMTICKET_GETTICKET].handler						= &WorldSession::HandleGMTicketGetTicketOpcode;
	WorldPacketHandlers[CMSG_GMTICKET_SYSTEMSTATUS].handler					 = &WorldSession::HandleGMTicketSystemStatusOpcode;
	WorldPacketHandlers[CMSG_GMTICKETSYSTEM_TOGGLE].handler					 = &WorldSession::HandleGMTicketToggleSystemStatusOpcode;
	WorldPacketHandlers[CMSG_UNLEARN_SKILL].handler							 = &WorldSession::HandleUnlearnSkillOpcode;
	
	// Meeting Stone / Instances
	WorldPacketHandlers[CMSG_SUMMON_RESPONSE].handler							= &WorldSession::HandleSummonResponseOpcode;
	WorldPacketHandlers[CMSG_RESET_INSTANCE].handler							= &WorldSession::HandleResetInstanceOpcode;
	WorldPacketHandlers[CMSG_SELF_RES].handler								  = &WorldSession::HandleSelfResurrectOpcode;
	WorldPacketHandlers[MSG_RANDOM_ROLL].handler								= &WorldSession::HandleRandomRollOpcode;
	WorldPacketHandlers[CMSG_DUNGEON_DIFFICULTY].handler                        = &WorldSession::HandleDungeonDifficultyOpcode;

	// Misc
	WorldPacketHandlers[CMSG_OPEN_ITEM].handler								 = &WorldSession::HandleOpenItemOpcode;
	WorldPacketHandlers[CMSG_COMPLETE_CINEMATIC].handler						= &WorldSession::HandleCompleteCinematic;
	WorldPacketHandlers[CMSG_MOUNTSPECIAL_ANIM].handler						 = &WorldSession::HandleMountSpecialAnimOpcode;
	WorldPacketHandlers[CMSG_TOGGLE_CLOAK].handler							  = &WorldSession::HandleToggleCloakOpcode;
	WorldPacketHandlers[CMSG_TOGGLE_HELM].handler							   = &WorldSession::HandleToggleHelmOpcode;
	WorldPacketHandlers[CMSG_SET_VISIBLE_RANK].handler							= &WorldSession::HandleSetVisibleRankOpcode;
	WorldPacketHandlers[CMSG_REPORT_SPAM].handler								= &WorldSession::HandleReportSpamOpcode;

	WorldPacketHandlers[MSG_ADD_DYNAMIC_TARGET_OBSOLETE].handler				= &WorldSession::HandleAddDynamicTargetOpcode;


	// Arenas
	WorldPacketHandlers[CMSG_ARENA_TEAM_QUERY].handler = &WorldSession::HandleArenaTeamQueryOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_ROSTER].handler = &WorldSession::HandleArenaTeamRosterOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_ADD_MEMBER].handler = &WorldSession::HandleArenaTeamAddMemberOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_INVITE_ACCEPT].handler = &WorldSession::HandleArenaTeamInviteAcceptOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_INVITE_DECLINE].handler = &WorldSession::HandleArenaTeamInviteDenyOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_LEAVE].handler = &WorldSession::HandleArenaTeamLeaveOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_REMOVE_PLAYER].handler = &WorldSession::HandleArenaTeamRemoveMemberOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_DISBAND].handler = &WorldSession::HandleArenaTeamDisbandOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_PROMOTE].handler = &WorldSession::HandleArenaTeamPromoteOpcode;

#ifdef CLUSTERING
	WorldPacketHandlers[CMSG_PING].handler = &WorldSession::HandlePingOpcode;
#endif
}

void WorldSession::CHECK_PACKET_SIZE(WorldPacket& data, uint32 size)
{
	if(data.size() < size && size)
	{
		// write to file
		sCheatLog.writefromsession(this, "kicked for invalid packet (opcode %u), size %u less than %u", data.GetOpcode(), data.size(), size);
		
		// disconnect
		Disconnect();
	}
}

void SessionLogWriter::writefromsession(WorldSession* session, const char* format, ...)
{
	if(!IsOpen())
		return;

	va_list ap;
	va_start(ap, format);
	char out[32768];

	time_t t = time(NULL);
	tm* aTm = localtime(&t);
	snprintf(out, 32768, "[%-4d-%02d-%02d %02d:%02d:%02d] ",aTm->tm_year+1900,aTm->tm_mon+1,aTm->tm_mday,aTm->tm_hour,aTm->tm_min,aTm->tm_sec);
	int l = strlen(out);

	snprintf(&out[l], 32768 - l, "Account %u [%s], IP %s, Player %s :: ", (unsigned int)session->GetAccountId(), session->GetAccountName().c_str(),
		session->GetSocket() ? session->GetSocket()->GetRemoteIP().c_str() : "NOIP", 
		session->GetPlayer() ? session->GetPlayer()->GetName() : "nologin");

	l = strlen(out);
	vsnprintf(&out[l], 32768 - l, format, ap);

	fprintf(m_file, "%s\n", out);
	fflush(m_file);
	va_end(ap);
}

#ifdef CLUSTERING
void WorldSession::HandlePingOpcode(WorldPacket& recvPacket)
{
	uint32 pong;
	recvPacket >> pong;
	WorldPacket data(SMSG_PONG, 4);
	data << pong;
	SendPacket(&data);
}

#endif

void WorldSession::SystemMessage(const char * format, ...)
{
	WorldPacket * data;
	char buffer[1024];
	va_list ap;
	va_start(ap,buffer);
	vsnprintf(buffer,1024,format,ap);
	va_end(ap);

	data = sChatHandler.FillSystemMessageData(buffer);
	SendPacket(data);
	delete data;
}
