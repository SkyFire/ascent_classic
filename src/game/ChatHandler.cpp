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

extern std::string LogFileName;
extern bool bLogChat;


void WorldSession::HandleMessagechatOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 9);
	WorldPacket *data;
	if(!_player->IsInWorld())
		return;

	uint32 type;
	int32 lang;

	const char * pMisc = 0;
	const char * pMsg = 0;
	recv_data >> type;
	recv_data >> lang;

	if(GetPlayer()->IsBanned())
	{
		GetPlayer()->BroadcastMessage("You cannot do that when banned.");
		return;
	}

	if(lang != -1 && !GetPermissionCount() && sWorld.flood_lines)
	{
		/* flood detection, wheeee! */
		if(UNIXTIME >= floodTime)
		{
			floodLines = 0;
			floodTime = UNIXTIME + sWorld.flood_seconds;
		}

		if((++floodLines) > sWorld.flood_lines)
		{
			if(sWorld.flood_message)
				_player->BroadcastMessage("Your message has triggered serverside flood protection. You can speak again in %u seconds.", floodTime - UNIXTIME);

			return;
		}
	}

	std::stringstream irctext;
	irctext.rdbuf()->str("");
	std::string msg;
	msg.reserve(256);

	//arghhh STFU. I'm not giving you gold or items NOOB
	switch(type)
	{
	case CHAT_MSG_EMOTE:
	case CHAT_MSG_SAY:
	case CHAT_MSG_YELL:
	case CHAT_MSG_WHISPER:
	case CHAT_MSG_CHANNEL:
		{
			if(_player->HasMuteOnPlayer())
			{
				std::stringstream Reply; 
				Reply << "Your voice has been taken away for "<< (_player->HasMuteOnPlayer()/1000/60) << " minutes by a GM.";
				data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, LANG_UNIVERSAL, Reply.str().c_str(), _player->GetGUID(), 3);
			}
		}
	}

	switch(type)
	{
	case CHAT_MSG_EMOTE:
		{
			recv_data >> msg;

			if(sWorld.getReqGmClient() == false)
				if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
					break;

			if(GetPlayer()->m_modlanguage >=0)
				data = sChatHandler.FillMessageData( CHAT_MSG_EMOTE, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0 );
			else	
				data = sChatHandler.FillMessageData( CHAT_MSG_EMOTE, CanUseCommand('c') ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0 );
			GetPlayer()->SendMessageToSet( data, true ,true );

			sLog.outString("[emote] %s: %s", _player->GetName(), msg.c_str());
			delete data;
			
			pMsg=msg.c_str();
			pMisc=0;

		}break;
	case CHAT_MSG_SAY:
		{
			recv_data >> msg;
			if(sWorld.getReqGmClient() == false)
				if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
					break;

			if(sWordFilter.UsesBannedKeyword(msg, this))
				return;

			if(GetPlayer()->m_modlanguage >=0)
				data = sChatHandler.FillMessageData( CHAT_MSG_SAY, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0 );
			else 
				data = sChatHandler.FillMessageData( CHAT_MSG_SAY, (CanUseCommand('c') && lang != -1) ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0 );

			GetPlayer()->SendMessageToSet( data, true );
			sLog.outString("[say] %s: %s", _player->GetName(), msg.c_str());
			delete data;
			pMsg=msg.c_str();
			pMisc=0;
		} break;
	case CHAT_MSG_PARTY:
	case CHAT_MSG_RAID:
	case CHAT_MSG_RAIDLEADER:
	case CHAT_MSG_RAIDWARNING:
		{
			recv_data >> msg;

			if(sWorld.getReqGmClient() == false)
				if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
					break;
			
			if(sWordFilter.UsesBannedKeyword(msg, this))
				return;

			Group *pGroup = _player->GetGroup();
			if(pGroup == NULL) break;
			
			if(GetPlayer()->m_modlanguage >=0)
				data=sChatHandler.FillMessageData( type, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0 );
			else
				data=sChatHandler.FillMessageData( type, (CanUseCommand('c') && lang != -1) ? LANG_UNIVERSAL : lang, msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0);
			if(type == CHAT_MSG_PARTY && pGroup->GetGroupType() == GROUP_TYPE_RAID)
			{
				// only send to that subgroup
				SubGroup * sgr = _player->GetGroup() ?
					_player->GetGroup()->GetSubGroup(_player->GetSubGroup()) : 0;

				if(sgr)
				{
					_player->GetGroup()->Lock();
					for(GroupMembersSet::iterator itr = sgr->GetGroupMembersBegin(); itr != sgr->GetGroupMembersEnd(); ++itr)
					{
						if(itr->player)
							itr->player->GetSession()->SendPacket(data);
					}
					_player->GetGroup()->Unlock();
				}
			}
			else
			{
				pGroup->SendPacketToAll(data);
			}
			//sLog.outString("[party] %s: %s", _player->GetName(), msg.c_str());
			delete data;
			pMsg=msg.c_str();
			pMisc=0;
		} break;
	case CHAT_MSG_GUILD:
		{
			recv_data >> msg;
			if(sWorld.getReqGmClient() == false)
				if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
				{
					break;
				}

				if(sWordFilter.UsesBannedKeyword(msg, this))
					return;

			if (GetPlayer()->IsInGuild())
			{
				Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );
				if(pGuild)
				{
					if(pGuild->HasRankRight(GetPlayer()->GetGuildRank(), GR_RIGHT_GCHATSPEAK))
						pGuild->BroadCastToGuild(this, msg);
					else
					{
                        
						WorldPacket data2(SMSG_GUILD_COMMAND_RESULT, 8 + pGuild->GetGuildName().size() + 1);
						data2 << uint32(GUILD_MEMBER_S);
						data2 << pGuild->GetGuildName();
						data2 << uint32(C_R_DONT_HAVE_PERMISSION);
						SendPacket(&data2);
					}
				}
			}
			pMsg=msg.c_str();
			pMisc=0;
		} break;
	case CHAT_MSG_OFFICER:
		{
			recv_data >> msg;

			if(sWorld.getReqGmClient() == false)
				if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
					break;
			if(sWordFilter.UsesBannedKeyword(msg, this))
				return;

			if (GetPlayer()->IsInGuild())
			{
				Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );
				if(pGuild)
				{
					if(pGuild->HasRankRight(GetPlayer()->GetGuildRank(), GR_RIGHT_OFFCHATSPEAK))
						pGuild->OfficerChannelChat(this, msg);
					else
					{
						WorldPacket data2(SMSG_GUILD_COMMAND_RESULT, 8 + pGuild->GetGuildName().size() + 1);
						data2 << uint32(GUILD_MEMBER_S);
						data2 << pGuild->GetGuildName();
						data2 << uint32(C_R_DONT_HAVE_PERMISSION);
						SendPacket(&data2);
					}
				}
			}
			pMsg=msg.c_str();
			pMisc=0;
		} break;
	case CHAT_MSG_YELL:
		{
			recv_data >> msg;

			if(sWorld.getReqGmClient() == false)
				if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
					break;
			if(sWordFilter.UsesBannedKeyword(msg, this))
				return;

			if(GetPlayer()->m_modlanguage >=0)
				data = sChatHandler.FillMessageData( CHAT_MSG_YELL, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0 );
			else
				data = sChatHandler.FillMessageData( CHAT_MSG_YELL, (CanUseCommand('c') && lang != -1) ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0 );

			//SendPacket(data);
			//sWorld.SendZoneMessage(data, GetPlayer()->GetZoneId(), this);
			_player->GetMapMgr()->SendMessageToCellPlayers(_player, data, 2);
			delete data;
			sLog.outString("[yell] %s: %s", _player->GetName(), msg.c_str());
			pMsg=msg.c_str();
			pMisc=0;
		} break;
	case CHAT_MSG_WHISPER:
		{
			std::string to = "",tmp;
			recv_data >> to >> msg;

			if(sWordFilter.UsesBannedKeyword(msg, this))
				return;
		 
			Player *player = objmgr.GetPlayer(to.c_str(), false);
			if(!player)
			{
				/*tmp = "Player \"";
				tmp += to.c_str();
				tmp += "\" is not online.";
				data = sChatHandler.FillSystemMessageData(  tmp.c_str() );
				SendPacket(data);			  
				delete data;*/
				data = new WorldPacket(SMSG_CHAT_PLAYER_NOT_FOUND, to.length() + 1);
				*data << to;
				SendPacket(data);
				delete data;
				break;
			}

			// Check that the player isn't a gm with his status on
			if(!_player->GetSession()->GetPermissionCount() && player->bGMTagOn && player->gmTargets.count(_player) == 0)
			{
				// Build automated reply
				string Reply = "This Game Master does not currently have an open ticket from you and did not receive your whisper. Please submit a new GM Ticket request if you need to speak to a GM. This is an automatic message.";
				data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, LANG_UNIVERSAL, Reply.c_str(), player->GetGUID(), 3);
				SendPacket(data);
				delete data;
				break;
			}

			if( sSocialMgr.HasIgnore(player, GetPlayer()) )
				return;

			if(GetPlayer()->m_modlanguage >=0)
				data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0 );
			else
				data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, (CanUseCommand('c') && lang != -1) ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 4 : 0 );

			player->GetSession()->SendPacket(data);
			delete data;
			//Sent the to Users id as the channel, this should be fine as it's not used for wisper
		  
			data = sChatHandler.FillMessageData(CHAT_MSG_WHISPER_INFORM, LANG_UNIVERSAL,msg.c_str(), player->GetGUID(), player->bGMTagOn ? 4 : 0  );
			SendPacket(data);
			delete data;

			if(player->HasFlag(PLAYER_FLAGS, 0x02))
			{
				// Has AFK flag, autorespond.
				data = sChatHandler.FillMessageData(CHAT_MSG_AFK, LANG_UNIVERSAL,  player->m_afk_reason.c_str(),player->GetGUID(), _player->bGMTagOn ? 4 : 0);
				SendPacket(data);
				delete data;
			}
			else if(player->HasFlag(PLAYER_FLAGS, 0x04))
			{
				// Has AFK flag, autorespond.
				data = sChatHandler.FillMessageData(CHAT_MSG_DND, LANG_UNIVERSAL, player->m_afk_reason.c_str(),player->GetGUID(), _player->bGMTagOn ? 4 : 0);
				SendPacket(data);
				delete data;
			}

			sLog.outString("[whisper] %s to %s: %s", _player->GetName(), to.c_str(), msg.c_str());
			pMsg=msg.c_str();
			pMisc=to.c_str();
		} break;
	case CHAT_MSG_CHANNEL:
		{
			std::string channel = "";
			recv_data >> channel;
			recv_data >> msg;
		 
			if(strcmp(channel.c_str(), sWorld.getGmClientChannel().c_str()) == 0)
				if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
					break;

			if(sWordFilter.UsesBannedKeyword(msg, this))
				return;

			Channel *chn = channelmgr.GetChannel(channel.c_str(),GetPlayer()); 
			if(chn) 
				chn->Say(GetPlayer(),msg.c_str());

			//sLog.outString("[%s] %s: %s", channel.c_str(), _player->GetName(), msg.c_str());
			pMsg=msg.c_str();
			pMisc=channel.c_str();

		} break;
	case CHAT_MSG_AFK:
		{
			std::string reason;
			recv_data >> reason;
			GetPlayer()->SetAFKReason(reason);

			if(sWordFilter.UsesBannedKeyword(reason, this))
				return;

			/* WorldPacket *data, WorldSession* session, uint32 type, uint32 language, const char *channelName, const char *message*/
			if(GetPlayer()->HasFlag(PLAYER_FLAGS, 0x02))
			{
				GetPlayer()->RemoveFlag(PLAYER_FLAGS, 0x02);
				if(sWorld.GetKickAFKPlayerTime())
					sEventMgr.RemoveEvents(GetPlayer(),EVENT_PLAYER_SOFT_DISCONNECT);
			}
			else
			{
				GetPlayer()->SetFlag(PLAYER_FLAGS, 0x02);
				if(sWorld.GetKickAFKPlayerTime())
					sEventMgr.AddEvent(GetPlayer(),&Player::SoftDisconnect,EVENT_PLAYER_SOFT_DISCONNECT,sWorld.GetKickAFKPlayerTime(),1,0);
			}			
		} break;
	case CHAT_MSG_DND:
		{
			std::string reason;
			recv_data >> reason;
			GetPlayer()->SetAFKReason(reason);

			if(sWordFilter.UsesBannedKeyword(reason, this))
				return;

			if(GetPlayer()->HasFlag(PLAYER_FLAGS, 0x04))
				GetPlayer()->RemoveFlag(PLAYER_FLAGS, 0x04);
			else
			{
				GetPlayer()->SetFlag(PLAYER_FLAGS, 0x04);
			}		  
		} break;
	default:
		sLog.outError("CHAT: unknown msg type %u, lang: %u", type, lang);
	}

	if(pMsg)
		sHookInterface.OnChat(_player, type, lang, pMsg, pMisc);
}

void WorldSession::HandleTextEmoteOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 16);
	if(!_player->IsInWorld())
		return;

	uint64 guid;
	uint32
		text_emote,
		unk,
		namelen =1;
	const char* name =" ";

	recv_data >> text_emote;
	recv_data >> unk;
	recv_data >> guid;

	Unit * pUnit = _player->GetMapMgr()->GetUnit(guid);
	if(pUnit)
	{
		if(pUnit->IsPlayer())
		{
			name = static_cast<Player*>(pUnit)->GetName();
			namelen = strlen(name) + 1;
		}
		else if(pUnit->GetTypeId() == TYPEID_UNIT)
		{
			Creature * p = static_cast<Creature*>(pUnit);
			if(p->GetCreatureName())
			{
				name = p->GetCreatureName()->Name;
				namelen = strlen(name) + 1;
			}
			else
			{
				name = 0;
				namelen = 0;
			}
		}
	}

	emoteentry *em = sEmoteStore.LookupEntry(text_emote);
	if(em)
	{
		WorldPacket data(SMSG_EMOTE, 28 + namelen);

		sHookInterface.OnEmote(_player, (EmoteType)em->textid);
        switch(em->textid)
        {
            case EMOTE_STATE_SLEEP:
            case EMOTE_STATE_SIT:
            case EMOTE_STATE_KNEEL:
                break;
            default:
				data << (uint32)em->textid;
				data << (uint64)GetPlayer()->GetGUID();
				GetPlayer()->SendMessageToSet(&data, true); //If player receives his own emote, his animation stops.
                break;
        }

		data.Initialize(SMSG_TEXT_EMOTE);
		data << (uint64)GetPlayer()->GetGUID();
		data << (uint32)text_emote;
		data << unk;
		data << (uint32)namelen;
		if( namelen > 1 )   data.append(name, namelen);
		else				data << (uint8)0x00;

		GetPlayer()->SendMessageToSet(&data, true);
	}
}

void WorldSession::HandleReportSpamOpcode(WorldPacket & recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 29);

    // the 0 in the out packet is unknown
    GetPlayer()->GetSession()->OutPacket(SMSG_REPORT_SPAM_RESPONSE, 1, 0 );

	/* This whole thing is guess-work */
	uint8 unk1;
	uint64 reportedGuid;
	uint32 unk2;
	uint32 messagetype;
	uint32 unk3;
	uint32 unk4;
	std::string message;
	recvPacket >> unk1 >> reportedGuid >> unk2 >> messagetype >> unk3 >> unk4 >> message;

	Player * rPlayer = objmgr.GetPlayer(reportedGuid);
	if(!rPlayer)
		return;

	sSocialMgr.AddIgnore(GetPlayer(), rPlayer->GetName());
}
