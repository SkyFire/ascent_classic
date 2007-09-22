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

#ifndef _COLLISION_TILE_H
#define _COLLISION_TILE_H

namespace Collision
{
	class Tile
	{
		/* Model Instances
		 */
		ModelInstance ** m_modelInstances;
		uint32 m_numModelInstances;

		/* data cache
		 */
		uint32 m_x, m_y, m_mapid;

		/** Tiles are reference-counted too.
		*/
		volatile uint32 m_refcount;

	public:

		inline uint32 GetX() { return m_x; }
		inline uint32 GetY() { return m_y; }
		inline uint32 GetMapId() { return m_mapid; }

		/** Creates a tile.
		 */
		Tile(uint32 x, uint32 y) : m_x(x), m_y(y), m_modelInstances(NULL), m_numModelInstances(0)
		{

		}

		~Tile()
		{
			if(m_modelInstances)
			{
				for(uint32 i = 0; i < m_numModelInstances; ++i)
					delete m_modelInstances[i];
				delete [] m_modelInstances;
			}
		}

		/** Loads from file
		 */
		bool Load(uint32 mapid);

		inline void AddRef() { ++m_refcount; }
		void DecRef();
		inline uint32 GetRefCount() { return m_refcount; }		
	};
}

#endif		// _COLLISION_TILE_H
