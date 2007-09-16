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

class ChannelMgr :  public Singleton < ChannelMgr >
{
 
public:
	ChannelMgr()
	{
		
	}

	~ChannelMgr();

	Channel *GetJoinChannel(const char *name, Player *p)
	{ 
		ChannelList* cl=&Channels[0];
		if(seperatechannels)
			cl = &Channels[p->GetTeam()];

		ChannelList::iterator i=cl->find(string(name));
		if(i!=cl->end())
			return i->second;
		else 
		{
			Channel *nchan = new Channel(name,p);
			(*cl)[name]=nchan;
			return nchan;
		}

	}
	Channel *GetChannel(const char *name, Player *p)
	{ 
		ChannelList* cl=&Channels[0];
		if(seperatechannels)
			cl = &Channels[p->GetTeam()];

		ChannelList::iterator i=cl->find(string(name));
		if(i!=cl->end())
			return i->second;
		else 
		{
            string n_name = name;
			WorldPacket data(1 + 1 + n_name.size());
			data.SetOpcode (SMSG_CHANNEL_NOTIFY);
			data << (uint8)0x05 << n_name;
			p->GetSession()->SendPacket(&data);
			return NULL;
		}
	}
	bool LeftChannel(const char *name,Player*p)
	{ 
		ChannelList* cl=&Channels[0];
		if(seperatechannels)
			cl = &Channels[p->GetTeam()];

		ChannelList::iterator i=cl->find(string(name));
		if(i!=cl->end())
		{
			Channel *ch=i->second;

			ch->Leave(p);
			if(!ch->IsConstant()  && ch->GetNumPlayers() == 0 )
			{
				cl->erase(i);
				delete ch;
			}
			return true;
		}
		return false;
	}
	bool seperatechannels;
private:
	//team 0: aliance, team 1 horde
	typedef map<string,Channel *> ChannelList;
	ChannelList Channels[2];

};

#define channelmgr ChannelMgr::getSingleton()
