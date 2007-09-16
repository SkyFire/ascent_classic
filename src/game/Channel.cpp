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

bool Channel::Join(Player *p, const char *pass)
{	
	m_Lock.Acquire();
	bool ret = false;
	WorldPacket data(100);
	if(IsOn(p))
	{
		MakeAlreadyOn(&data,p);
		SendToOne(&data,p);
	}
	else if(IsBanned(p->GetGUID()))
	{
		MakeYouAreBanned(&data);
		SendToOne(&data,p);
	}
	else if((password.length() > 0 && strcmp(pass,password.c_str())) || (strcmp(name.c_str(), sWorld.getGmClientChannel().c_str()) == 0 && !p->GetSession()->CanUseCommand('c')))
	{
		MakeWrongPass(&data);
		SendToOne(&data,p);
	}
	else
	{
		PlayerInfo pinfo;
		pinfo.player = p;
		pinfo.muted = false;
		if(GetNumPlayers() == 0)
			pinfo.owner = true;
		else
			pinfo.owner = false;
		pinfo.moderator = false;

		MakeJoined(&data,p);
		p->JoinedChannel(this);
		if(announce && !m_DefaultChannel)
			SendToAll(&data);

		data.clear();
		players[p] = pinfo;

		/*MakeYouJoined(&data,p);
		SendToOne(&data,p);*/

		if(owner == NULL)
		{
			if(!IsDefaultChannel())
			{
				SetOwner(p);
				players[p].moderator = false;
			}
			else if(p->GetSession()->CanUseCommand('c'))
			{
				SetOwner(p);
				players[p].moderator = true;
			}
		}
		ret = true;
	}
	m_Lock.Release();
	return ret;
}

void Channel::Leave(Player *p, bool send)
{
	m_Lock.Acquire();
	WorldPacket data(100);
	if(!IsOn(p))
	{
		MakeNotOn(&data);
		if(send) SendToOne(&data,p);
	}
	else
	{
		bool changeowner = (owner == p);

		//MakeYouLeft(&data);
		if(send)
		{
			//SendToOne(&data,p);
			p->LeftChannel(this);
		}
		//data.clear();

		players.erase(p);
		MakeLeft(&data,p);
		if(announce && !m_DefaultChannel)
			SendToAll(&data);

		if(changeowner)
		{
			Player *newowner = players.size() > 0 ? players.begin()->second.player : NULL;
			SetOwner(newowner);
		}
	}
	m_Lock.Release();
}

void Channel::KickOrBan(Player *good, const char *badname, bool ban)
{
	m_Lock.Acquire();
	WorldPacket data(100);
	if(!IsOn(good))
	{
		MakeNotOn(&data);
		SendToOne(&data,good);
	}
	else if(!players[good].moderator && !good->GetSession()->CanUseCommand('c'))
	{
		MakeNotModerator(&data);
		SendToOne(&data,good);
	}
	else
	{
		Player *bad = objmgr.GetPlayer(badname);
		if(bad == NULL || !IsOn(bad))
		{
			MakeNotOn(&data,badname);
			SendToOne(&data,good);
		}
		else if(!good->GetSession()->CanUseCommand('c') && bad == owner && good != owner)
		{
			MakeNotOwner(&data);
			SendToOne(&data,good);
		}
		else
		{
			bool changeowner = (owner == bad);

			if(ban && !IsBanned(bad->GetGUID()))
			{
				banned.push_back(bad->GetGUID());
				MakeBanned(&data,good,bad);
			}
			else
				MakeKicked(&data,good,bad);	

			SendToAll(&data);
			players.erase(bad);

			if(changeowner)
			{
				Player *newowner = players.size() > 0 ? good : NULL;
				SetOwner(newowner);
			}
		}
	}
	m_Lock.Release();
}

void Channel::UnBan(Player *good, const char *badname)
{
	m_Lock.Acquire();
	WorldPacket data(100);
	if(!IsOn(good))
	{
		MakeNotOn(&data);
		SendToOne(&data,good);
	}
	else if(!players[good].moderator && !good->GetSession()->CanUseCommand('c'))
	{
		MakeNotModerator(&data);
		SendToOne(&data,good);
	}
	else
	{
		Player *bad = objmgr.GetPlayer(badname);
		if(bad == NULL || !IsBanned(bad->GetGUID()))
		{
			MakeNotOn(&data,badname); // Change to <Not Banned> message. Not sure what it is.
			SendToOne(&data,good);
		}
		else
		{
			banned.remove(bad->GetGUID());
			MakeUnbanned(&data,good,bad);
			SendToAll(&data);
		}
	}
	m_Lock.Release();
}

void Channel::Password(Player *p, const char *pass)
{
	m_Lock.Acquire();
	WorldPacket data(100);
	if(!IsOn(p))
	{
		MakeNotOn(&data);
		SendToOne(&data,p);
	}
	else if(!players[p].moderator && !p->GetSession()->CanUseCommand('c'))
	{
		MakeNotModerator(&data);
		SendToOne(&data,p);
	}
	else
	{
		password = pass;
		MakeSetPassword(&data,p);
		SendToAll(&data);
	}
	m_Lock.Release();
}

void Channel::SetMode(Player *p, const char *p2n, bool mod, bool set)
{
	m_Lock.Acquire();
	WorldPacket data(100);
	if(!IsOn(p))
	{
		MakeNotOn(&data);
		SendToOne(&data,p);
	}
	else if(!players[p].moderator && !p->GetSession()->CanUseCommand('c'))
	{
		MakeNotModerator(&data);
		SendToOne(&data,p);
	}
	else 
	{
		Player *newp = objmgr.GetPlayer(p2n);
//		PlayerInfo inf = players[newp];
		if(p == owner && newp == owner && mod)
		{
			m_Lock.Release();
			return;
		}
		if(newp == NULL || !IsOn(newp))
		{
			MakeNotOn(&data,p2n);
			SendToOne(&data,p);
		}
		else if(owner == newp && owner != p)
		{
			MakeNotOwner(&data);
			SendToOne(&data,p);
		}
		else
		{
			if(mod)
				SetModerator(newp,set);
			else
				SetMute(newp,set);
		}
	}
	m_Lock.Release();
}

void Channel::SetOwner(Player *p, const char *newname)
{
	m_Lock.Acquire();
	WorldPacket data(100);
	if(!IsOn(p))
	{
		MakeNotOn(&data);
		SendToOne(&data,p);
	}
	else if(!p->GetSession()->CanUseCommand('c') && p != owner)
	{
		MakeNotOwner(&data);
		SendToOne(&data,p);
	}
	else 
	{
		Player *newp = objmgr.GetPlayer(newname);
		if(newp == NULL || !IsOn(newp))
		{
			MakeNotOn(&data,newname);
			SendToOne(&data,p);
		}
		else
		{
			MakeChangeOwner(&data,newp);
			if(!m_DefaultChannel) SendToAll(&data);

			SetModerator(newp,true);
			owner = newp;
		}
	}
	m_Lock.Release();
}

void Channel::GetOwner(Player *p)
{
	WorldPacket data(100);
	if(!IsOn(p))
	{
		MakeNotOn(&data);
		SendToOne(&data,p);
	}
	else
	{
		MakeWhoOwner(&data);
		SendToOne(&data,p);
	}
}

void Channel::List(Player *p)
{
	m_Lock.Acquire();
	WorldPacket data(100);
	if(!IsOn(p))
	{
		MakeNotOn(&data);
		SendToOne(&data,p);
	}
	else
	{
		data.Initialize(SMSG_CHANNEL_LIST);
		data << (uint8)3 << (uint32)players.size();

		PlayerList::iterator i;
		for(i = players.begin(); i!=players.end(); i++)
		{
			data << i->first->GetGUID();
			uint8 mode = 0x00;
			if(i->second.muted) 
				mode |= 0x04;
			if(i->second.moderator)
				mode |= 0x02;
			data << mode;
		}
		SendToOne(&data,p);
	}
	m_Lock.Release();
}

void Channel::Announce(Player *p)
{
	WorldPacket data(100);
	if(!IsOn(p))
	{
		MakeNotOn(&data);
		SendToOne(&data,p);
	}
	else if(!players[p].moderator && !p->GetSession()->CanUseCommand('c'))
	{
		MakeNotModerator(&data);
		SendToOne(&data,p);
	}
	else
	{
		announce = !announce;
		MakeAnnounce(&data,p,announce);
		SendToAll(&data);
	}
}

void Channel::Moderate(Player *p)
{
	WorldPacket data(100);
	if(!IsOn(p))
	{
		MakeNotOn(&data);
		SendToOne(&data,p);
	}
	else if(!players[p].moderator && !p->GetSession()->CanUseCommand('c'))
	{
		MakeNotModerator(&data);
		SendToOne(&data,p);
	}
	else
	{
		moderate = !moderate;
		MakeModerate(&data,p,moderate);
		SendToAll(&data);
	}
}

void Channel::Say(Player *p, const char *what, Player *t)
{
	if(!IsOn(p))
	{
        WorldPacket data(15);
		MakeNotOn(&data);
		SendToOne(&data,p);
	}
	else if(players[p].muted || (GetName() == "WorldDefense" && p->GetPVPRank() < 11))
	{
        WorldPacket data(15);
		MakeYouCantSpeak(&data);
		SendToOne(&data,p);
	}
	else if(moderate && !players[p].moderator && !p->GetSession()->CanUseCommand('c'))
	{
        WorldPacket data(15);
		MakeNotModerator(&data);
		SendToOne(&data,p);
	}
	else
	{
		//Packet    structure
		//uint8	    type;
		//uint32	language;
		//uint32	PVP rank
		//uint64	guid;
		//uint32	len_of_text;
		//char	    text[];
		//uint8	    afk_state;

		uint32 messageLength = strlen((char*)what) + 1;

        WorldPacket data(40 + messageLength);

		data.Initialize(SMSG_MESSAGECHAT);
		data << (uint8)CHAT_MSG_CHANNEL;      // CHAT_MESSAGE_CHANNEL
		data << (uint32)0;      // Universal lang
		data << p->GetGUID();
		data << uint32(0);		// pvp rank??
		data << name;
		data << p->GetGUID();
		data << messageLength;
		data << what;
		data << (uint8)(p->bGMTagOn ? 4 : 0);

		if(t == NULL)
			SendToAll(&data);
		else
			SendToOne(&data, t);
		// Send the actual talking stuff.
	}
}

void Channel::SayFromIRC(const char* msg)
{
	
	//Packet    structure
	//uint8	    type;
	//uint32	language;
	//uint32	PVP rank
	//uint64	guid;
	//uint32	len_of_text;
	//char	    text[];
	//uint8	    afk_state;

    
	uint32 messageLength = strlen((char*)msg) + 1;
    WorldPacket data(messageLength + 40);
//	uint8 afk = 0;

	data.Initialize(SMSG_MESSAGECHAT);
	data << (uint8)CHAT_MSG_CHANNEL; // CHAT_MESSAGE_CHANNEL
	data << (uint32)0; // Universal lang
	data << name.c_str();
	data << (uint32)0; // pvp ranks
	data << (uint64)0;  // was.. guid..
	data << messageLength;
	data << msg;
	data << (uint8)0;

	SendToAll(&data);
}

void Channel::Invite(Player *p, const char *newname)
{
	WorldPacket data(100);
	if(!IsOn(p))
	{
		MakeNotOn(&data);
		SendToOne(&data,p);
	}
	else 
	{
		Player *newp = objmgr.GetPlayer(newname);
		if(newp == NULL)
		{
			MakeNotOn(&data,newname);
			SendToOne(&data,p);
		}
		else if(IsOn(newp))
		{
			MakeAlreadyOn(&data,newp);
			SendToOne(&data,p);
		}
		else
		{
			MakeInvited(&data,p);
			SendToOne(&data,newp);
			data.clear();
			MakeYouInvited(&data,newp);
			SendToOne(&data,p);
		}
	}
}

bool Channel::IsDefaultChannel()
{
	const char* cName = name.c_str();

	if( !memcmp (cName, "General", 7)		  
		)
		return true;
	if( !memcmp(cName, "LookingForGroup", 15)  
		)
		return true;
	if( !strncmp(cName, "LocalDefense", 12)	 
		)
		return true;
	if( !strncmp(cName, "Trade", 5)			
		)
		return true;
	if( !strncmp(cName, "Guild Recruitment", 17)
		)
		return true;
	if( !strncmp(cName, "WorldDefense", 12)
		)
		return true;

	return false;
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


