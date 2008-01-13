/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _COLLIDEINTERFACE_H
#define _COLLIDEINTERFACE_H

/* imports */
#define NO_WMO_HEIGHT -100000.0f

#ifdef _GAME

#ifdef WIN32
#define COLLISION_IMPORT __declspec(dllimport)
#else
#define COLLISION_IMPORT 
#endif

typedef void(*collision_log_func)(const char * msg);
COLLISION_IMPORT void collision_init(collision_log_func normal_log, collision_log_func warning_log, collision_log_func error_log, collision_log_func debug_log);
COLLISION_IMPORT void collision_shutdown();
COLLISION_IMPORT void collision_activate_cell(uint32 mapid, uint32 x, uint32 y);
COLLISION_IMPORT void collision_deactivate_cell(uint32 mapid, uint32 x, uint32 y);

COLLISION_IMPORT bool collision_check_indoor(uint32 mapid, LocationVector * pos);
COLLISION_IMPORT bool collision_check_los(uint32 mapid, LocationVector * pos1, LocationVector * pos2);
COLLISION_IMPORT bool collision_check_losmod(uint32 mapid, LocationVector * pos1, LocationVector * pos2);
COLLISION_IMPORT float collision_get_height(uint32 mapid, LocationVector * pos);
COLLISION_IMPORT bool collision_get_first_object_point(uint32 mapid, LocationVector * pos1, LocationVector * pos2, LocationVector * outvec, float distmod);

COLLISION_IMPORT bool collision_check_indoor(uint32 mapid, float x, float y, float z);
COLLISION_IMPORT bool collision_check_los(uint32 mapid, float x1, float y1, float z1, float x2, float y2, float z2);
COLLISION_IMPORT float collision_get_height(uint32 mapid, float x, float y, float z);
COLLISION_IMPORT bool collision_get_first_object_point(uint32 mapid, float x1, float y1, float z1, float x2, float y2, float z2, float * outx, float * outy, float * outz, float distmod);

#endif

class CCollideInterface
{
public:
	void Init();
	void DeInit();

	void ActivateTile(uint32 mapId, uint32 tileX, uint32 tileY);
	void DeactivateTile(uint32 mapId, uint32 tileX, uint32 tileY);

	bool CheckLOS(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2);
	bool GetFirstPoint(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2, float * outx, float * outy, float * outz, float distmod);
	bool IsIndoor(uint32 mapId, float x, float y, float z);
	float GetHeight(uint32 mapId, float x, float y, float z);

	bool CheckLOS(uint32 mapId, LocationVector * pos1, LocationVector * pos2);
	bool CheckLOSMod(uint32 mapId, LocationVector * pos1, LocationVector * pos2);
	bool GetFirstPoint(uint32 mapId, LocationVector * pos1, LocationVector * pos2, LocationVector * outvec, float distmod);
	bool IsIndoor(uint32 mapId, LocationVector * pos);
	float GetHeight(uint32 mapId, LocationVector * pos);
};

extern CCollideInterface CollideInterface;

#endif
