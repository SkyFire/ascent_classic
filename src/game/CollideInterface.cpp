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

#include "StdAfx.h"

#ifdef COLLISION

#define MAX_MAP 600

CCollideInterface CollideInterface;
Mutex m_loadLock;
uint32 m_tilesLoaded[MAX_MAP][64][64];

#define COLLISION_DEBUG

#ifdef WIN32
#ifdef COLLISION_DEBUG

uint64 c_GetTimerValue()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter( &li );
	return li.QuadPart;
}

uint32 c_GetNanoSeconds(uint64 t1, uint64 t2)
{
	LARGE_INTEGER li;
	double val;
	QueryPerformanceFrequency( &li );
	val = double( t1 - t2 ) * 1000000;
	val /= li.QuadPart;
    return long2int32( val );	
}

#define COLLISION_BEGINTIMER uint64 v1 = c_GetTimerValue();

#endif	// COLLISION_DEBUG
#endif	// WIN32

#ifdef WIN32
#pragma comment(lib, "collision.lib")
#endif

void log_normal(const char * msg)
{
	Log.Notice("Collision", msg);
}

void log_warning(const char * msg)
{
	Log.Warning("Collision", msg);
}

void log_error(const char* msg)
{
	Log.Error("Collision", msg);
}

void log_debug(const char* msg)
{
	Log.Debug("Collision", msg);
}

// Debug functions
#ifdef COLLISION_DEBUG

void CCollideInterface::Init()
{
	Log.Notice("CollideInterface", "Init");
	COLLISION_BEGINTIMER;
	collision_init(log_normal, log_warning, log_error, log_debug);
	printf("[%u ns] collision_init\n", c_GetNanoSeconds(c_GetTimerValue(), v1));
}

void CCollideInterface::ActivateTile(uint32 mapId, uint32 tileX, uint32 tileY)
{
	m_loadLock.Acquire();
	if(m_tilesLoaded[mapId][tileX][tileY] == 0)
	{
		COLLISION_BEGINTIMER;
		collision_activate_cell(mapId, tileX, tileY);
		printf("[%u ns] collision_activate_cell %u %u %u\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, tileX, tileY);
	}

	++m_tilesLoaded[mapId][tileX][tileY];
	m_loadLock.Release();
}

void CCollideInterface::DeactivateTile(uint32 mapId, uint32 tileX, uint32 tileY)
{
	m_loadLock.Acquire();
	if(!(--m_tilesLoaded[mapId][tileX][tileY]))
	{
		COLLISION_BEGINTIMER;
		collision_deactivate_cell(mapId, tileX, tileY);
		printf("[%u ns] collision_deactivate_cell %u %u %u\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, tileX, tileY);
	}

	m_loadLock.Release();
}

void CCollideInterface::DeInit()
{
	Log.Notice("CollideInterface", "DeInit");
	COLLISION_BEGINTIMER;
	collision_shutdown();
	printf("[%u ns] collision_shutdown\n", c_GetNanoSeconds(c_GetTimerValue(), v1));
}

float CCollideInterface::GetHeight(uint32 mapId, float x, float y, float z)
{
	COLLISION_BEGINTIMER;
	float v = collision_get_height(mapId, x, y, z);
	printf("[%u ns] GetHeight Map:%u %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, x, y, z);
	return v;
}

float CCollideInterface::GetHeight(uint32 mapId, LocationVector * pos)
{
	COLLISION_BEGINTIMER;
	float v = collision_get_height(mapId, pos);
	printf("[%u ns] GetHeight Map:%u %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, pos->x, pos->y, pos->z);
	return v;
}

bool CCollideInterface::IsIndoor(uint32 mapId, LocationVector * pos)
{
	bool r;
	COLLISION_BEGINTIMER;
	printf("[%u ns] IsIndoor Map:%u %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, pos->x, pos->y, pos->z);
	r = collision_check_indoor(mapId, pos);
	return r;
}

bool CCollideInterface::IsIndoorMod(uint32 mapId, LocationVector * pos)
{
	bool r;
	COLLISION_BEGINTIMER;
	printf("[%u ns] IsIndoorMod Map:%u %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, pos->x, pos->y, pos->z);
	r = collision_check_indoor_mod(mapId, pos);
	return r;
}

bool CCollideInterface::IsIndoor(uint32 mapId, float x, float y, float z)
{
	bool r;
	COLLISION_BEGINTIMER;
	printf("[%u ns] IsIndoor Map:%u %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, x, y, z);
	r = collision_check_indoor(mapId, x, y, z);
	return r;
}

bool CCollideInterface::CheckLOS(uint32 mapId, LocationVector * pos1, LocationVector * pos2)
{
	bool r;
	COLLISION_BEGINTIMER;
	printf("[%u ns] CheckLOS Map:%u %f %f %f -> %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, pos1->x, pos1->y, pos1->z, pos2->x, pos2->y, pos2->z);
	r = collision_check_los(mapId, pos1, pos2);
	return r;
}

bool CCollideInterface::CheckLOSMod(uint32 mapId, LocationVector * pos1, LocationVector * pos2)
{
	bool r;
	COLLISION_BEGINTIMER;
	printf("[%u ns] CheckLOSMod Map:%u %f %f %f -> %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, pos1->x, pos1->y, pos1->z, pos2->x, pos2->y, pos2->z);
	r = collision_check_losmod(mapId, pos1, pos2);
	return r;
}

bool CCollideInterface::CheckLOS(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2)
{
	bool r;
	COLLISION_BEGINTIMER;
	printf("[%u ns] CheckLOS Map:%u %f %f %f -> %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, x1, y1, z1, x2, y2, z2);
	r = collision_check_los(mapId, x1, y1, z1, x2, y2, z2);
	return r;
}

bool CCollideInterface::GetFirstPoint(uint32 mapId, LocationVector * pos1, LocationVector * pos2, LocationVector * outvec, float distmod)
{
	bool r;
	COLLISION_BEGINTIMER;
	printf("[%u ns] GetFirstPoint Map:%u %f %f %f -> %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, pos1->x, pos1->y, pos1->z, pos2->x, pos2->y, pos2->z);
	r = collision_get_first_object_point(mapId, pos1, pos2, outvec, distmod);
	return r;
}

bool CCollideInterface::GetFirstPoint(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2, float * outx, float * outy, float * outz, float distmod)
{
	bool r;
	COLLISION_BEGINTIMER;
	printf("[%u ns] GetFirstPoint Map:%u %f %f %f -> %f %f %f\n", c_GetNanoSeconds(c_GetTimerValue(), v1), mapId, x1, y1, z1, x2, y2, z2);
	r = collision_get_first_object_point(mapId, x1, y1, z1, x2, y2, z2, outx, outy, outz, distmod);
	return r;
}

#else

void CCollideInterface::Init()
{
	Log.Notice("CollideInterface", "Init");
	collision_init(log_normal, log_warning, log_error, log_debug);
}

void CCollideInterface::ActivateTile(uint32 mapId, uint32 tileX, uint32 tileY)
{
	m_loadLock.Acquire();
	if(m_tilesLoaded[mapId][tileX][tileY] == 0)
		collision_activate_cell(mapId, tileX, tileY);

	++m_tilesLoaded[mapId][tileX][tileY];
	m_loadLock.Release();
}

void CCollideInterface::DeactivateTile(uint32 mapId, uint32 tileX, uint32 tileY)
{
	m_loadLock.Acquire();
	if(!(--m_tilesLoaded[mapId][tileX][tileY]))
		collision_deactivate_cell(mapId, tileX, tileY);

	m_loadLock.Release();
}

void CCollideInterface::DeInit()
{
	Log.Notice("CollideInterface", "DeInit");
	collision_shutdown();
}

float CCollideInterface::GetHeight(uint32 mapId, float x, float y, float z)
{
	return collision_get_height(mapId, x, y, z);
}

float CCollideInterface::GetHeight(uint32 mapId, LocationVector * pos)
{
	return collision_get_height(mapId, pos);
}

bool CCollideInterface::IsIndoor(uint32 mapId, LocationVector * pos)
{
	return collision_check_indoor(mapId, pos);
}

bool CCollideInterface::IsIndoorMod(uint32 mapId, LocationVector * pos)
{
	return collision_check_indoor_mod(mapId, pos);
}

bool CCollideInterface::IsIndoor(uint32 mapId, float x, float y, float z)
{
	return collision_check_indoor(mapId, x, y, z);
}

bool CCollideInterface::CheckLOS(uint32 mapId, LocationVector * pos1, LocationVector * pos2)
{
	return collision_check_los(mapId, pos1, pos2);
}

bool CCollideInterface::CheckLOSMod(uint32 mapId, LocationVector * pos1, LocationVector * pos2)
{
	return collision_check_losmod(mapId, pos1, pos2);
}

bool CCollideInterface::CheckLOS(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2)
{
	return collision_check_los(mapId, x1, y1, z1, x2, y2, z2);
}

bool CCollideInterface::GetFirstPoint(uint32 mapId, LocationVector * pos1, LocationVector * pos2, LocationVector * outvec, float distmod)
{
	return collision_get_first_object_point(mapId, pos1, pos2, outvec, distmod);
}

bool CCollideInterface::GetFirstPoint(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2, float * outx, float * outy, float * outz, float distmod)
{
	return collision_get_first_object_point(mapId, x1, y1, z1, x2, y2, z2, outx, outy, outz, distmod);
}

#endif		// COLLISION_DEBUG
#endif		// COLLISION
