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

Mutex m_confSettingLock;
vector<string> m_bannedChannels;
vector<string> m_generalChannels;

void Channel::LoadConfSettings()
{
	string BannedChannels = Config.MainConfig.GetStringDefault("Channels", "BannedChannels", "");
	string GeneralChannels = Config.MainConfig.GetStringDefault("Channels", "GeneralChannels", "General -;Trade -;LookingForGroup;GuildRecruitment;LocalDefense;WorldDefense");
	m_confSettingLock.Acquire();
	m_bannedChannels = StrSplit(BannedChannels, ";");
	m_generalChannels = StrSplit(GeneralChannels, ";");
	m_confSettingLock.Release();
}

Channel::Channel(const char * name, uint32 team)
{
	// flags (0x01 = custom?, 0x04 = trade?, 0x20 = city?, 0x40 = lfg?, , 0x80 = voice?,
	m_flags = 0;
	m_announce = true;
	m_muted = false;
	m_general = false;
	m_name = string(name);
	m_team = team;

	m_confSettingLock.Acquire();
	for(vector<string>::iterator itr = m_generalChannels.begin(); itr != m_generalChannels.end(); ++itr)
	{
		if(!strnicmp(m_name.c_str(), (*itr).c_str(), (*itr).size()))
		{
			m_general = true;
			m_announce = false;
			break;
		}
	}
	m_confSettingLock.Release();
}

void Channel::AttemptJoin(Player * plr, const char * password)
{
	Guard mGuard(m_lock);
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

	if(m_members.empty() && !m_general)
		flags |= CHANNEL_FLAG_OWNER;

	plr->JoinedChannel(this);
	m_members.insert(make_pair(plr, flags));

	data << uint8(CHANNEL_NOTIFY_FLAG_YOUJOINED) << m_name << m_id << uint32(0) << uint8(0);
	plr->GetSession()->SendPacket(&data);

	if(!m_announce)
		return;

	data.clear();
	data << uint8(CHANNEL_NOTIFY_FLAG_JOINED) << m_name << plr->GetGUID();
	SendToAll(&data);

	data.Initialize(SMSG_PLAYER_JOINED_CHANNEL);
	data << plr->GetGUID() << uint8(0x14) << m_flags << m_id << m_name;
	SendToAll(&data);
}

void Channel::Part(Player * plr)
{
	Guard mGuard(m_lock);
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
	plr->LeftChannel(this);

	if(flags & CHANNEL_FLAG_OWNER)
	{
		// we need to find a new owner
		SetOwner(NULL, NULL);
	}

	data << uint8(CHANNEL_NOTIFY_FLAG_YOULEFT) << m_name << m_id << uint32(0) << uint8(0);
	plr->GetSession()->SendPacket(&data);

	if(m_announce)
	{
		data.clear();
		data << uint8(CHANNEL_NOTIFY_FLAG_LEFT) << m_name << plr->GetGUID();
		SendToAll(&data);
	
		data.Initialize(SMSG_PLAYER_LEFT_CHANNEL);
		data << plr->GetGUID() << m_flags << m_id << m_name;
		SendToAll(&data);
	}

	if(m_members.size() == 0)
		channelmgr.RemoveChannel(this);
}

void Channel::SetOwner(Player * oldpl, Player * plr)
{
	Guard mGuard(m_lock);
	Player * pOwner = NULL;
	uint32 oldflags, oldflags2;
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(oldpl != NULL)
	{
		MemberMap::iterator itr = m_members.find(oldpl);
		if(m_members.end() == itr)
		{
			data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
			plr->GetSession()->SendPacket(&data);
			return;
		}

		if(!(itr->second & CHANNEL_FLAG_OWNER))
		{
			data << uint8(CHANNEL_NOTIFY_FLAG_NOT_OWNER) << m_name;
			plr->GetSession()->SendPacket(&data);
			return;
		}
	}

	if(plr == NULL)
	{
		for(MemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
		{
			if(itr->second & CHANNEL_FLAG_OWNER)
			{
				// remove the old owner
				oldflags2 = itr->second;
				itr->second &= ~CHANNEL_FLAG_OWNER;
				data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << itr->first->GetGUID() << uint8(oldflags2) << uint8(itr->second);				
				SendToAll(&data);
			}
			else
			{
				if(pOwner == NULL)
				{
					pOwner = itr->first;
					oldflags = itr->second;
					itr->second |= CHANNEL_FLAG_OWNER;
				}
			}				
		}
	}
	else
	{
		for(MemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
		{
			if(itr->second & CHANNEL_FLAG_OWNER)
			{
				// remove the old owner
				oldflags2 = itr->second;
				itr->second &= ~CHANNEL_FLAG_OWNER;
				data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << itr->first->GetGUID() << uint8(oldflags2) << uint8(itr->second);	
				SendToAll(&data);
			}
			else
			{
				if(plr == itr->first)
				{
					pOwner = itr->first;
					oldflags = itr->second;
					itr->second |= CHANNEL_FLAG_OWNER;
				}
			}				
		}
	}

	if(pOwner == NULL)
		return;		// obviously no members

	data.clear();
	data << uint8(CHANNEL_NOTIFY_FLAG_CHGOWNER) << m_name << pOwner->GetGUID();
	SendToAll(&data);

	// send the mode changes
	data.clear();
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << pOwner->GetGUID() << uint8(oldflags) << uint8(oldflags | CHANNEL_FLAG_OWNER);
	SendToAll(&data);
}

void Channel::Invite(Player * plr, Player * new_player)
{
	Guard mGuard(m_lock);
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
	Guard mGuard(m_lock);
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

void Channel::Say(Player * plr, const char * message, Player * for_gm_client, bool forced)
{
	Guard mGuard(m_lock);
	MemberMap::iterator itr = m_members.find(plr);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, strlen(message)+100);
	if(!forced)
	{
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
	}

	data.SetOpcode(SMSG_MESSAGECHAT);
	data << uint8(CHAT_MSG_CHANNEL);
	data << uint32(0);		// language
	data << plr->GetGUID();	// guid
	data << uint32(0);		// rank?
	data << m_name;			// channel name
	data << plr->GetGUID();	// guid again?
	data << uint32(strlen(message)+1);
	data << message;
	data << (uint8)(plr->bGMTagOn ? 4 : 0);
	if(for_gm_client != NULL)
		for_gm_client->GetSession()->SendPacket(&data);
	else
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
	Guard mGuard(m_lock);
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
	SendToAll(&data);

	if(ban)
	{
		data.clear();
		data << uint8(CHANNEL_NOTIFY_FLAG_BANNED) << m_name << die_player->GetGUID();
		SendToAll(&data);
	}

	m_members.erase(itr);
	if(flags & CHANNEL_FLAG_OWNER)
		SetOwner(NULL, NULL);

	if(ban)
		m_bannedMembers.insert(die_player->GetGUIDLow());

	data.clear();
	data << uint8(CHANNEL_NOTIFY_FLAG_YOULEFT) << m_name << m_id << uint32(0) << uint8(0);
	die_player->GetSession()->SendPacket(&data);
}

void Channel::Unban(Player * plr, PlayerInfo * bplr)
{
	Guard mGuard(m_lock);
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
	Guard mGuard(m_lock);
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

	uint32 oldflags = itr2->second;
    itr2->second |= CHANNEL_FLAG_VOICED;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << v_player->GetGUID() << uint8(oldflags) << uint8(itr2->second);
	SendToAll(&data);
}

void Channel::Devoice(Player * plr, Player * v_player)
{
	Guard mGuard(m_lock);
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

	uint32 oldflags = itr2->second;
	itr2->second &= ~CHANNEL_FLAG_VOICED;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << v_player->GetGUID() << uint8(oldflags) << uint8(itr2->second);
	SendToAll(&data);
}

void Channel::Mute(Player * plr, Player * die_player)
{
	Guard mGuard(m_lock);
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

	uint32 oldflags = itr2->second;
	itr2->second |= CHANNEL_FLAG_MUTED;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << die_player->GetGUID() << uint8(oldflags) << uint8(itr2->second);
	SendToAll(&data);
}

void Channel::Unmute(Player * plr, Player * die_player)
{
	Guard mGuard(m_lock);
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

	uint32 oldflags = itr2->second;
	itr2->second &= ~CHANNEL_FLAG_MUTED;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << die_player->GetGUID() << uint8(oldflags) << uint8(itr2->second);
	SendToAll(&data);
}

void Channel::GiveModerator(Player * plr, Player * new_player)
{
	Guard mGuard(m_lock);
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

	uint32 oldflags = itr2->second;
	itr2->second |= CHANNEL_FLAG_MODERATOR;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << new_player->GetGUID() << uint8(oldflags) << uint8(itr2->second);
	SendToAll(&data);
}

void Channel::TakeModerator(Player * plr, Player * new_player)
{
	Guard mGuard(m_lock);
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

	uint32 oldflags = itr2->second;
	itr2->second &= ~CHANNEL_FLAG_MODERATOR;
	data << uint8(CHANNEL_NOTIFY_FLAG_MODE_CHG) << m_name << new_player->GetGUID() << uint8(oldflags) << uint8(itr2->second);
	SendToAll(&data);
}

void Channel::Announce(Player * plr)
{
	Guard mGuard(m_lock);
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
	Guard mGuard(m_lock);
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

void Channel::List(Player * plr)
{
	Guard mGuard(m_lock);
	WorldPacket data(SMSG_CHANNEL_LIST, 50 + (m_members.size()*9));
	MemberMap::iterator itr = m_members.find(plr);
	if(itr == m_members.end())
	{
		data.Initialize(SMSG_CHANNEL_NOTIFY);
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	uint8 flags;
	data << uint8(1) << m_name;
	data << uint8(m_flags);
	data << uint32(m_members.size());
	for(MemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		data << itr->first->GetGUID();
		flags = 0;
		if(!(itr->second & CHANNEL_FLAG_MUTED))
			flags |= 0x04;		// voice flag

		if(itr->second & CHANNEL_FLAG_OWNER)
			flags |= 0x01;		// owner flag

		if(itr->second & CHANNEL_FLAG_MODERATOR)
			flags |= 0x02;		// moderator flag

		if(!m_general)
			flags |= 0x10;

		data << flags;
	}

	plr->GetSession()->SendPacket(&data);
}

void Channel::GetOwner(Player * plr)
{
	Guard mGuard(m_lock);
	MemberMap::iterator itr = m_members.find(plr);
	WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
	if(itr == m_members.end())
	{
		data << uint8(CHANNEL_NOTIFY_FLAG_NOTON) << m_name;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if(itr->second & CHANNEL_FLAG_OWNER)
		{
			data << uint8(CHANNEL_NOTIFY_FLAG_WHO_OWNER) << m_name << itr->first->GetGUID();
			plr->GetSession()->SendPacket(&data);
			return;
		}
	}
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

Channel::~Channel()
{

}

void Channel::SendToAll(WorldPacket * data)
{
	Guard guard(m_lock);
	for(MemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
		itr->first->GetSession()->SendPacket(data);
}

Channel * ChannelMgr::GetCreateChannel(const char *name, Player * p)
{
	ChannelList::iterator itr;
	ChannelList * cl = &Channels[0];
	Channel * chn;
	if(seperatechannels)
		cl = &Channels[p->GetTeam()];

	lock.Acquire();
	for(itr = cl->begin(); itr != cl->end(); ++itr)
	{
		if(!stricmp(name, itr->first.c_str()))
		{
			lock.Release();
			return itr->second;
		}
	}

	// make sure the name isn't banned
	m_confSettingLock.Acquire();
	for(vector<string>::iterator itr = m_bannedChannels.begin(); itr != m_bannedChannels.end(); ++itr)
	{
		if(!strnicmp( name, itr->c_str(), itr->size() ) )
		{
			lock.Release();
			return NULL;
		}
	}
	m_confSettingLock.Release();

	chn = new Channel(name, seperatechannels ? p->GetTeam() : 0);
	cl->insert(make_pair(chn->m_name, chn));
	lock.Release();
	return chn;
}

Channel * ChannelMgr::GetChannel(const char *name, Player * p)
{
	ChannelList::iterator itr;
	ChannelList * cl = &Channels[0];
	if(seperatechannels)
		cl = &Channels[p->GetTeam()];

	lock.Acquire();
	for(itr = cl->begin(); itr != cl->end(); ++itr)
	{
		if(!stricmp(name, itr->first.c_str()))
		{
			lock.Release();
			return itr->second;
		}
	}

	lock.Release();
	return NULL;
}

void ChannelMgr::RemoveChannel(Channel * chn)
{
	ChannelList::iterator itr;
	ChannelList * cl = &Channels[0];
	if(seperatechannels)
		cl = &Channels[chn->m_team];

	lock.Acquire();
	for(itr = cl->begin(); itr != cl->end(); ++itr)
	{
		if(itr->second == chn)
		{
			cl->erase(itr);
			lock.Release();
			return;
		}
	}

	lock.Release();
}

ChannelMgr::ChannelMgr()
{

}
