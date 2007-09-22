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

#ifndef _COLLISION_COLLISION_H
#define _COLLISION_COLLISION_H

#pragma pack(push,1)
struct Vec3d
{
	float x;
	float y;
	float z;
};
#pragma pack(pop)

namespace Collision
{
	/** hash type - can be changed later if we get collisions. currently crc32
	*/
	typedef unsigned int MODELHASH;
};

#include "Model.h"
#include "ModelManager.h"
#include "Tile.h"
#include "TileManager.h"

#endif	// _COLLISION_COLLISION_H