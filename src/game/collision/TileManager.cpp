/*
 * Ascent MMORPG Server
 *   - Collision System
 *
 * Copyright (C) 2005-2007 Burlex <burlex@ascentemu.com>
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
#ifdef COLLISION

Collision::TileManager::TileManager(uint32 mapid) : m_mapid(mapid)
{
	memset(m_tiles, 0, sizeof(Tile*) * 64 * 64);
	memset(m_triedload, 0, sizeof(bool) * 64 * 64);
}

Collision::TileManager::~TileManager()
{
	for(uint32 x = 0; x < 64; ++x)
	{
		for(uint32 y = 0; y < 64; ++y)
		{
			if(m_tiles[x][y])
				delete m_tiles[x][y];
		}
	}
}

Collision::Tile * Collision::TileManager::GetTile(uint32 x, uint32 y)
{
	Tile * ret = m_tiles[x][y];
    if(ret != NULL)
	{
		ret->AddRef();
		return ret;
	}
	else
	{
		if(!m_triedload[x][y])
		{
			// try to load
			Collision::Tile * load = new Collision::Tile(x,y);
            if(load->Load(m_mapid))
			{
				m_tiles[x][y] = load;
				load->AddRef();
				return load;
			}
			else
			{
				// oh well ,at least we tried
				m_triedload[x][y] = true;
			}
		}

		return NULL;
	}
}

void Collision::TileManager::RemoveReference(Tile * tile)
{
	tile->DecRef();
}

#define MAP_COUNT 600 // as of 2.1.0
static Collision::TileManager * m_tileManagers[MAP_COUNT];

void Collision::InitTileManagers()
{
	memset(m_tileManagers, 0, sizeof(Collision::TileManager*) * MAP_COUNT);
	Collision::ModelManager.Init();
}

Collision::TileManager * Collision::GetTileManager(uint32 MapId)
{
	if(m_tileManagers[MapId] == NULL)
		m_tileManagers[MapId] = new Collision::TileManager(MapId);

	return m_tileManagers[MapId];
}

#endif		// COLLISION
