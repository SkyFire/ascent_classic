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

#ifndef __CHANNEL_H
#define __CHANNEL_H

using namespace std;

class Channel
{
	Mutex m_Lock;
	struct PlayerInfo
	{
		Player *player;
		bool owner, moderator, muted;
	};
	typedef map<Player*,PlayerInfo> PlayerList;
	PlayerList      players;
	list<uint64>    banned;
	string          name;
	bool            announce, constant, moderate;
	Player *        owner;
	string          password;
	uint32          team;

private:
	/*Packets! Woohoo!*/
	inline WorldPacket *MakeNotifyPacket(WorldPacket *data, uint8 code)
	{
		data->Initialize(SMSG_CHANNEL_NOTIFY);
		*data << code << name.c_str();
		return data;
	}
	inline void MakeJoined(WorldPacket *data, Player *joined) { *MakeNotifyPacket(data,0x00) << joined->GetGUID(); }
	inline void MakeLeft(WorldPacket *data, Player *left) { *MakeNotifyPacket(data,0x01) << left->GetGUID(); }
	inline void MakeYouJoined(WorldPacket *data, Player *p ) { *MakeNotifyPacket(data,0x02) << p->GetGUID();  }
	inline void MakeYouLeft(WorldPacket *data) { MakeNotifyPacket(data,0x03); }
	inline void MakeWrongPass(WorldPacket *data) { MakeNotifyPacket(data,0x04); }
	inline void MakeNotOn(WorldPacket *data) { MakeNotifyPacket(data,0x05); }
	inline void MakeNotModerator(WorldPacket *data) { MakeNotifyPacket(data,0x06); }
	inline void MakeSetPassword(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x07) << who->GetGUID(); }
	inline void MakeChangeOwner(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x08) << who->GetGUID(); }
	inline void MakeNotOn(WorldPacket *data, const char *who) { *MakeNotifyPacket(data,0x09) << who; }
	inline void MakeNotOwner(WorldPacket *data) { MakeNotifyPacket(data,0x0A); }
	inline void MakeWhoOwner(WorldPacket *data) { *MakeNotifyPacket(data,0x0B) << ((constant || owner == NULL) ? "Nobody" : owner->GetName()); }
	inline void MakeModeChange(WorldPacket *data, Player *who, char moderator, char voice) // 0 - no change, 1 - lost, 2 - got
	{
		MakeNotifyPacket(data,0x0C);
		*data << who->GetGUID();
		uint8 byte1 = 0x00, byte2 = 0x00;

		if(moderator == 1)  byte1 |= 0x02;
		if(voice == 1)      byte1 |= 0x04;

		if(moderator == 2)  byte2 |= 0x02;
		if(voice == 2)      byte2 |= 0x04;
		*data << byte1 << byte2;
	}
	inline void MakeEnabledAnnounce(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x0D) << who->GetGUID(); }
	inline void MakeDisabledAnnounce(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x0E) << who->GetGUID(); }
	inline void MakeAnnounce(WorldPacket *data, Player *who, bool set) { set ? MakeEnabledAnnounce(data,who) : MakeDisabledAnnounce(data,who); }
	inline void MakeModerated(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x0F) << who->GetGUID(); }
	inline void MakeUnmoderated(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x10) << who->GetGUID(); }
	inline void MakeModerate(WorldPacket *data, Player *who, bool set) { set ? MakeModerated(data,who) : MakeUnmoderated(data,who); }
	inline void MakeYouCantSpeak(WorldPacket *data) { MakeNotifyPacket(data,0x11); }
	inline void MakeKicked(WorldPacket *data, Player *good, Player *bad) { *MakeNotifyPacket(data,0x12) << bad->GetGUID() << good->GetGUID(); }
	inline void MakeYouAreBanned(WorldPacket *data) { MakeNotifyPacket(data,0x13); }
	inline void MakeBanned(WorldPacket *data, Player *good, Player *bad) { *MakeNotifyPacket(data,0x14) << bad->GetGUID() << good->GetGUID(); }
	inline void MakeUnbanned(WorldPacket *data, Player *good, Player *bad) { *MakeNotifyPacket(data,0x15) << bad->GetGUID() << good->GetGUID(); }
	// 16 unk
	inline void MakeAlreadyOn(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x17) << who->GetGUID(); }
	inline void MakeInvited(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x18) << who->GetGUID(); }
	inline void MakeWrongAlliance(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x19) << who->GetGUID(); }
	//....
	inline void MakeYouInvited(WorldPacket *data, Player *who) { *MakeNotifyPacket(data,0x1D) << who->GetName(); }

	void SendToAll(WorldPacket *data)
	{
		m_Lock.Acquire();
		PlayerList::iterator i;
		for(i = players.begin(); i!=players.end(); i++)
		{
			i->first->GetSession()->SendPacket(data);
		}
		m_Lock.Release();
	}

	void SendToAllButOne(WorldPacket *data, Player *who)
	{
		m_Lock.Acquire();
		PlayerList::iterator i;
		for(i = players.begin(); i!=players.end(); i++)
		{
			if(i->first != who)
				i->first->GetSession()->SendPacket(data);
		}
		m_Lock.Release();
	}

	inline void SendToOne(WorldPacket *data, Player *who)
	{
		who->GetSession()->SendPacket(data);
	}

	inline bool IsOn(Player *who)
	{
		return players.count(who) > 0;
	}

	bool IsBanned(const uint64 guid)
	{
		m_Lock.Acquire();
		bool ret = false;

		list<uint64>::iterator i;
		for(i = banned.begin(); i!=banned.end(); i++)
		{
			if(*i == guid)
				ret = true;
		}
		m_Lock.Release();
		return ret;
	}

	inline bool IsFirst()
	{
		m_Lock.Acquire();
		bool res = !(players.size() > 1);
		m_Lock.Release();
		return res;
	}

	void SetOwner(Player *p)
	{
		if(m_DefaultChannel)return;
		owner = p;
		if(owner != NULL)
		{
		   WorldPacket data(10);
		   MakeChangeOwner(&data,p);
		   SendToAll(&data);
		}
	}

	void SetModerator(Player *p, bool set)
	{
		if(players[p].moderator != set)
		{
			players[p].moderator = set;
			WorldPacket data(10);
			MakeModeChange(&data,p,set ? 2 : 1,0);
			SendToAll(&data);
		}
	}

	void SetMute(Player *p, bool set)
	{
		if(players[p].muted != set)
		{
			players[p].muted = set;
			set = !set;
			WorldPacket data(10);
			MakeModeChange(&data,p,0,set ? 2 : 1);
			SendToAll(&data);
		}
	}

public:

	Channel(const char* newname,Player*p)
	{ 
		name = newname;
		m_DefaultChannel = IsDefaultChannel();
		
		if(m_DefaultChannel)
			owner=NULL;
		else owner = p;

		password="";
		announce =false;
		moderate =false;
		constant =false;
		
		
	}
	/*
	void SetName(string newname,uint32 team)
	{
		name = newname;
	   
	}*/

	string GetName() { return name; }
	bool IsConstant() { return m_DefaultChannel; }
	bool IsAnnounce() { return announce; }
	string GetPassword() { return password; }
	void SetPassword(string npassword) { password = npassword; }
	void SetAnnounce(bool nannounce) { announce = nannounce; }
	uint32 GetNumPlayers() { return players.size(); }

	bool Join(Player *p, const char *pass);
	void Leave(Player *p, bool send = true);
	void KickOrBan(Player *good, const char *badname, bool ban);
	void Kick(Player *good, const char *badname) { KickOrBan(good,badname,false); }
	void Ban(Player *good, const char *badname) { KickOrBan(good,badname,true); }
	void UnBan(Player *good, const char *badname);
	void Password(Player *p, const char *pass);
	void SetMode(Player *p, const char *p2n, bool mod, bool set);
	void SetOwner(Player *p, const char *newname);
	void GetOwner(Player *p);
	void SetModerator(Player *p, const char *newname) { SetMode(p,newname,true,true); }
	void UnsetModerator(Player *p, const char *newname) { SetMode(p,newname,true,false); }
	void SetMute(Player *p, const char *newname) { SetMode(p,newname,false,true); }
	void UnsetMute(Player *p, const char *newname) { SetMode(p,newname,false,false); }
	void List(Player *p);
	void Announce(Player *p);
	void Moderate(Player *p);
	void Say(Player *p, const char *what, Player *t = NULL);
	void SayFromIRC(const char *msg);
	void Invite(Player *p, const char *newp);
	bool IsDefaultChannel();
	bool m_DefaultChannel;
};

#endif
