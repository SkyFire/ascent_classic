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

uint32 chash(const char * str);

class ChannelMgr :  public Singleton < ChannelMgr >
{
 
public:
	ChannelMgr()
	{
		
	}

	~ChannelMgr();

	Channel *GetCreateChannel(const char *name, Player * p)
	{ 
		uint32 team = 0;
		ChannelList* cl=&Channels[team];
		if(seperatechannels)
		{
			cl = &Channels[p->GetTeam()];
			team = p->GetTeam();
		}

		uint32 h = chash(name);
		lock.Acquire();
		ChannelList::iterator i=cl->find(h);
		lock.Release();
		if(i!=cl->end())
			return i->second;
		else 
		{
			Channel *nchan = new Channel(name, team);
			cl->insert(make_pair(nchan->m_hash, nchan));
			return nchan;
		}

	}
	Channel *GetChannel(const char *name, Player * p)
	{ 
		ChannelList* cl = &Channels[0];
		lock.Acquire();
		if(seperatechannels)
			cl = &Channels[p->GetTeam()];

		uint32 h = chash(name);
		ChannelList::iterator i=cl->find(h);
		lock.Release();

		if(i!=cl->end())
			return i->second;
		else 
			return NULL;
	}

	void RemoveChannel(Channel * chn)
	{
		lock.Acquire();
		ChannelList * cl = &Channels[chn->m_team];
		ChannelList::iterator i = cl->find(chn->m_hash);
		if(i != cl->end())
		{
			cl->erase(i);
		}
		lock.Release();
		delete chn;
	}

	bool seperatechannels;
private:
	//team 0: aliance, team 1 horde
	typedef map<uint32,Channel *> ChannelList;
	ChannelList Channels[2];
	Mutex lock;

};

#define channelmgr ChannelMgr::getSingleton()
