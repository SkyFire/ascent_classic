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

initialiseSingleton( LfgMgr );

LfgMgr::LfgMgr()
{
	
	
}

LfgMgr::~LfgMgr()
{
	
	
}

void LfgMgr::RemoveFromLfgQueue(Player *pl,uint32 LfgDungeonId)
{	
	if(!LfgDungeonId)
		return;
	
	std::set<Player*>::iterator itr;
	for(itr = GetLfgQueueBegin(LfgDungeonId); itr != GetLfgQueueEnd(LfgDungeonId) ; ++itr)
	{
		if((*itr) == pl)
		{
			m_LFGqueueMembers[LfgDungeonId].erase(itr);
			return;
		}
	}
}	

void LfgMgr::SetPlayerInLFGqueue(Player *pl,uint32 LfgDungeonId)
{
	m_LFGqueueMembers[LfgDungeonId].insert(pl);
}

uint32 LfgMgr::GetLfgQueueSize(uint32 LfgDungeonId)
{
	return m_LFGqueueMembers[LfgDungeonId].size();	
}

std::set<Player*>::iterator LfgMgr::GetLfgQueueBegin(uint32 LfgDungeonId)
{
	return m_LFGqueueMembers[LfgDungeonId].begin();
}

std::set<Player*>::iterator LfgMgr::GetLfgQueueEnd(uint32 LfgDungeonId)
{
	return m_LFGqueueMembers[LfgDungeonId].end();
}
