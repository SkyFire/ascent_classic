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

Channel::Channel(const char * name)
{
	// flags (0x01 = custom?, 0x04 = trade?, 0x20 = city?, 0x40 = lfg?, , 0x80 = voice?,
	m_flags = 0;
	m_announce = true;
	m_muted = false;
	m_general = false;
}

void Channel::AttemptJoin(Player * plr, const char * password)
{
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	uint32 flags = CHANNEL_FLAG_NONE;
	if(!m_password.empty() && strcmp(m_password.c_str(), password) != 0)
	{
        data << uint8(CHANNEL_NOTIFY_FLAG_WRONGPASS) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_bannedMembers.find(plr->GetGUIDLow()) != m_bannedMembers.end())
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_YOURBANNED) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_members.find(plr) != m_members.end())
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_ALREADY_ON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_members.empty())
		flags |= CHANNEL_FLAG_OWNER;

	plr->JoinedChannel(this);
	m_members.insert(make_pair(plr, flags));

	data << uint8(CHANNEL_NOTIFY_FLAG_YOUJOINED) << m_name << m_id << uint32(0) << uint8(0);
	plr->GetSession()->SendPacket(&data);

	if(!m_announce)
		return;

	if(m_general)
		data.Initialize(SMSG_PLAYER_JOINED_CHANNEL);
	else
		data.Initialize(SMSG_PLAYER_JOINED_CUSTOM_CHANNEL);

	data << plr->GetGUID() << m_flags << m_id << uint8(0) << m_name;
	SendToAll(&data);
}

void Channel::Part(Player * plr)
{
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	uint32 flags;
	MemberMap::iterator itr = m_members.find(plr);
	if(itr == m_members.end())
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}
    
	flags = itr->second;
	m_members.erase(itr);

	if(flags & CHANNEL_FLAG_OWNER)
	{
		// we need to find a new owner
		SetOwner(NULL);
	}

	if(!m_announce)
		return;

	data << uint8(CHANNEL_NOTIFY_FLAG_YOULEFT) << m_name << m_id << uint32(0) << uint8(0);
	plr->GetSession()->SendPacket(&data);
}

void Channel::SetOwner(Player * plr)
{
	Player * pOwner = plr;
	if(pOwner == NULL)
	{
		for(MemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
		{
			pOwner = itr->first;
			itr->second |= CHANNEL_FLAG_OWNER;
			break;
		}
	}

	if(pOwner == NULL)
		return;		// obviously no members

	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	data << uint8(CHANNEL_NOTIFY_FLAG_CHGOWNER) << m_name << pOwner->GetGUID();
	plr->GetSession()->SendPacket(&data);
}

void Channel::Invite(Player * plr, Player * new_player)
{
	if(m_members.find(plr) == m_members.end())
	{
		SendNotOn(plr);
		return;
	}

	if(m_members.find(new_player) != m_members.end())
	{
		SendAlreadyOn(plr, new_player);
		return;
	}

	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	data << uint8(CHANNEL_NOTIFY_FLAG_INVITED) << m_name << plr->GetGUID();
	new_player->GetSession()->SendPacket(&data);

	data.clear();
	data << uint8(CHANNEL_NOTIFY_FLAG_YOU_INVITED) << m_name << new_player->GetGUID();
	plr->GetSession()->SendPacket(&data);
}

void Channel::Moderate(Player * plr)
{
	MemberMap::iterator itr = m_members.find(plr);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	m_muted = !m_muted;
	data << uint8(m_muted ? CHANNEL_NOTIFY_FLAG_MODERATED : CHANNEL_NOTIFY_FLAG_UNMODERATED) << m_name << plr->GetGUID();
	SendToAll(&data);
}

void Channel::Say(Player * plr, const char * message)
{
	MemberMap::iterator itr = m_members.find(plr);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 200);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(itr->second & CHANNEL_FLAG_MUTED)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_YOUCANTSPEAK) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_muted && !(itr->second & CHANNEL_FLAG_VOICED) && !(itr->second & CHANNEL_FLAG_MODERATOR) && !(itr->second & CHANNEL_FLAG_OWNER))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_YOUCANTSPEAK) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;	
	}

	data.SetOpcode(SMSG_MESSAGECHAT);
	data << uint8(CHAT_MSG_CHANNEL);
	data << uint32(0);		// language
	data << plr->GetGUID();	// guid
	data << uint32(0);		// rank?
	data << m_name;			// channel name
	data << plr->GetGUID();	// guid again?
	data << uint32(strlen(message));
	data << message;
	data << (uint8)(plr->bGMTagOn ? 4 : 0);
	SendToAll(&data);
}

void Channel::SendNotOn(Player * plr)
{
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
	plr->GetSession()->SendPacket(&data);
}

void Channel::SendAlreadyOn(Player * plr, Player* plr2)
{
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	data << uint8(CHANNEL_NOTIFY_FLAG_ALREADY_ON) << m_name << plr2->GetGUID();
	plr->GetSession()->SendPacket(&data);
}

void Channel::Kick(Player * plr, Player * die_player, bool ban)
{
	MemberMap::iterator itr = m_members.find(die_player);
	MemberMap::iterator me_itr = m_members.find(plr);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	uint32 flags;

	if(me_itr == m_members.end())
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(itr == m_members.end())
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOT_ON_2) << m_name << die_player->GetGUID();
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(me_itr->second & CHANNEL_FLAG_OWNER || me_itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

    flags = itr->second;
	data << uint8(CHANNEL_NOTIFY_FLAG_KICKED) << m_name << die_player->GetGUID();
	SendToAll(data);

	if(ban)
	{
		data.clear();
		data << uint8(CHANNEL_NOTIFY_FLAG_BANNED) << m_name << die_player->GetGUID();
		SendToAll(data);
	}

	m_members.erase(itr);
	if(flags & CHANNEL_FLAG_OWNER)
		SetOwner(NULL);

	if(ban)
		m_bannedMembers.insert(die_player->GetGUIDLow());

	data.clear();
	data << uint8(CHANNEL_NOTIFY_FLAG_YOULEFT) << m_name << m_id << uint32(0) << uint8(0);
	die_player->GetSession()->SendPacket(&data);
}

void Channel::Unban(Player * plr, PlayerInfo * bplr)
{
	MemberMap::iterator itr = m_members.find(plr);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	set<uint32>::iterator it2 = m_bannedMembers.find(bplr->guid);
	if(it2 == m_bannedMembers.end())
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOT_ON_2) << m_name << uint64(bplr->guid);
		plr->GetSession()->SendPacket(&data);
		return;
	}

	data << uint8(CHANNEL_NOTIFY_FLAG_UNBANNED) << m_name << uint64(bplr->guid);
	SendToAll(&data);
	m_bannedMembers.erase(it2);
}

void Channel::Voice(Player * plr, Player * v_player)
{
	MemberMap::iterator itr = m_members.find(plr);
	MemberMap::iterator itr2 = m_members.find(v_player);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_members.end() == itr2)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOT_ON_2) << m_name << v_player->GetGUID();
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

    itr2->second |= CHANNEL_FLAG_VOICED;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << v_player->GetGUID() << uint8(0) << uint8(0x04);
	SendToAll(&data);
}

void Channel::Devoice(Player * plr, Player * v_player)
{
	MemberMap::iterator itr = m_members.find(plr);
	MemberMap::iterator itr2 = m_members.find(v_player);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_members.end() == itr2)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOT_ON_2) << m_name << v_player->GetGUID();
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	itr2->second &= ~CHANNEL_FLAG_VOICED;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << v_player->GetGUID() << uint8(0x04) << uint8(0x00);
	SendToAll(&data);
}

void Channel::Mute(Player * plr, Player * die_player)
{
	MemberMap::iterator itr = m_members.find(plr);
	MemberMap::iterator itr2 = m_members.find(die_player);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_members.end() == itr2)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOT_ON_2) << m_name << die_player->GetGUID();
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	itr2->second |= CHANNEL_FLAG_MUTED;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << die_player->GetGUID() << uint8(0x04) << uint8(0x00);
	SendToAll(&data);
}

void Channel::Unmute(Player * plr, Player * die_player)
{
	MemberMap::iterator itr = m_members.find(plr);
	MemberMap::iterator itr2 = m_members.find(die_player);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_members.end() == itr2)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOT_ON_2) << m_name << die_player->GetGUID();
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	itr2->second &= ~CHANNEL_FLAG_MUTED;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << die_player->GetGUID() << uint8(0x00) << uint8(0x04);
	SendToAll(&data);
}

void Channel::GiveModerator(Player * plr, Player * new_player)
{
	MemberMap::iterator itr = m_members.find(plr);
	MemberMap::iterator itr2 = m_members.find(new_player);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_members.end() == itr2)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOT_ON_2) << m_name << new_player->GetGUID();
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	itr2->second |= CHANNEL_FLAG_MODERATOR;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << new_player->GetGUID() << uint8(0x00) << uint8(0x02);
	SendToAll(&data);
}

void Channel::TakeModerator(Player * plr, Player * new_player)
{
	MemberMap::iterator itr = m_members.find(plr);
	MemberMap::iterator itr2 = m_members.find(new_player);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(m_members.end() == itr2)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOT_ON_2) << m_name << new_player->GetGUID();
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	itr2->second &= ~CHANNEL_FLAG_MODERATOR;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << new_player->GetGUID() << uint8(0x02) << uint8(0x00);
	SendToAll(&data);
}

void Channel::Announce(Player * plr)
{
	MemberMap::iterator itr = m_members.find(plr);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	m_announce = !m_announce;
	data << uint8(m_announce ? CHANNEL_NOTIFY_FLAG_ENABLE_ANN : CHANNEL_NOTIFY_FLAG_DISABLE_ANN) << m_name << plr->GetGUID();
	SendToAll(&data);
}

void Channel::Password(Player * plr, const char * pass)
{
	MemberMap::iterator itr = m_members.find(plr);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(m_members.end() == itr)
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	if(!(itr->second & CHANNEL_FLAG_OWNER || itr->second & CHANNEL_FLAG_MODERATOR))
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTMOD) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	m_password = string(pass);
	data << uint8(CHANNEL_NOTIFY_FLAG_SETPASS) << m_name << plr->GetGUID();
	SendToAll(&data);	
}

ChannelMgr::~ChannelMgr()
{
	for(int i = 0; i < 2; ++i)
	{
		ChannelList::iterator itr = this->Channels[i].begin();
		for(; itr != this->Channels[i].end(); ++itr)
		{
			delete itr->second;
		}
		Channels[i].clear();
	}
}


