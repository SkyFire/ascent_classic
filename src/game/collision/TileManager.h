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

#ifndef _COLLISION_TILEMANAGER_H
#define _COLLISION_TILEMANAGER_H

#include "Tile.h"

namespace Collision
{
	class TileManager
	{
		Tile * m_tiles[64][64];
		bool m_triedload[64][64];
		uint32 m_mapid;
	public:

		TileManager(uint32 mapid);
		~TileManager();

		/** Gets tile, adds reference.
		 */
		Tile * GetTile(uint32 x, uint32 y);

		/** removes a reference
		 */
		void RemoveReference(Tile * tile);
	};

	Collision::TileManager * GetTileManager(uint32 MapId);
	void InitTileManagers();

	inline uint32 GetXFromCellX(uint32 cell_x) { return cell_x/8; }
	inline uint32 GetYFromCellY(uint32 cell_y) { return cell_y/8; }
}

#endif		// _COLLISION_TILEMANAGER_H
