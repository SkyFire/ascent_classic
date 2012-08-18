/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2008 Ascent Team <http://www.ascentemu.com/>
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

#pragma comment(lib, "collision.lib")
#define MAX_MAP 600

struct CollisionMap
{
	uint32 m_loadCount;
	uint32 m_tileLoadCount[64][64];
	RWLock m_lock;
};

SERVER_DECL CCollideInterface CollideInterface;
IVMapManager * CollisionMgr;
CollisionMap *m_mapLocks[MAX_MAP];
Mutex m_mapCreateLock;

void CCollideInterface::Init()
{
	Log.Notice("CollideInterface", "Init");
	CollisionMgr = ((IVMapManager*)collision_init());
	memset(m_mapLocks, 0, sizeof(CollisionMap*)*MAX_MAP);
}

void CCollideInterface::ActivateMap(uint32 mapId)
{
	m_mapCreateLock.Acquire();
	if( m_mapLocks[mapId] == NULL )
	{
		m_mapLocks[mapId] = new CollisionMap;
		m_mapLocks[mapId]->m_loadCount = 1;
		memset(m_mapLocks[mapId]->m_tileLoadCount, 0, sizeof(uint32)*64*64);
	}
	else
		m_mapLocks[mapId]->m_loadCount++;

	m_mapCreateLock.Release();
}

void CCollideInterface::DeactivateMap(uint32 mapId)
{
	m_mapCreateLock.Acquire();
	ASSERT(m_mapLocks[mapId] != NULL);
	--m_mapLocks[mapId]->m_loadCount;
	if( m_mapLocks[mapId]->m_loadCount == 0 )
	{
		// no instances using this anymore
		delete m_mapLocks[mapId];
		CollisionMgr->unloadMap(mapId);
		m_mapLocks[mapId] = NULL;
	}
	m_mapCreateLock.Release();
}

void CCollideInterface::ActivateTile(uint32 mapId, uint32 tileX, uint32 tileY)
{
	ASSERT(m_mapLocks[mapId] != NULL);
	// acquire write lock
	m_mapLocks[mapId]->m_lock.AcquireWriteLock();
	if( m_mapLocks[mapId]->m_tileLoadCount[tileX][tileY] == 0 )
		CollisionMgr->loadMap("vmaps", mapId, tileY, tileX);

	// increment count
	m_mapLocks[mapId]->m_tileLoadCount[tileX][tileY]++;

	// release lock
	m_mapLocks[mapId]->m_lock.ReleaseWriteLock();
}

void CCollideInterface::DeactivateTile(uint32 mapId, uint32 tileX, uint32 tileY)
{
	ASSERT(m_mapLocks[mapId] != NULL);
	// get write lock
	m_mapLocks[mapId]->m_lock.AcquireWriteLock();
	if( (--m_mapLocks[mapId]->m_tileLoadCount[tileX][tileY]) == 0 )
		CollisionMgr->unloadMap(mapId, tileX, tileY);

	// release write lock
	m_mapLocks[mapId]->m_lock.ReleaseWriteLock();
}

bool CCollideInterface::CheckLOS(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2)
{
	ASSERT(m_mapLocks[mapId] != NULL);

	// get read lock
	m_mapLocks[mapId]->m_lock.AcquireReadLock();

	// get data
	bool res = CollisionMgr->isInLineOfSight(mapId, x1, y1, z1, x2, y2, z2);

	// release write lock
	m_mapLocks[mapId]->m_lock.ReleaseReadLock();

	// return
	return res;
}

bool CCollideInterface::GetFirstPoint(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2, float & outx, float & outy, float & outz, float distmod)
{
	ASSERT(m_mapLocks[mapId] != NULL);

	// get read lock
	m_mapLocks[mapId]->m_lock.AcquireReadLock();

	// get data
	bool res = CollisionMgr->getObjectHitPos(mapId, x1, y1, z1, x2, y2, z2, outx, outy, outz, distmod);

	// release write lock
	m_mapLocks[mapId]->m_lock.ReleaseReadLock();

	// return
	return res;
}

float CCollideInterface::GetHeight(uint32 mapId, float x, float y, float z)
{
	ASSERT(m_mapLocks[mapId] != NULL);

	// get read lock
	m_mapLocks[mapId]->m_lock.AcquireReadLock();

	// get data
	float res = CollisionMgr->getHeight(mapId, x, y, z);

	// release write lock
	m_mapLocks[mapId]->m_lock.ReleaseReadLock();

	// return
	return res;
}

bool CCollideInterface::IsIndoor(uint32 mapId, float x, float y, float z)
{
	ASSERT(m_mapLocks[mapId] != NULL);

	// get read lock
	m_mapLocks[mapId]->m_lock.AcquireReadLock();

	// get data
	bool res = CollisionMgr->isInDoors(mapId, x, y, z);

	// release write lock
	m_mapLocks[mapId]->m_lock.ReleaseReadLock();

	// return
	return res;
}

bool CCollideInterface::IsOutdoor(uint32 mapId, float x, float y, float z)
{
	ASSERT(m_mapLocks[mapId] != NULL);

	// get read lock
	m_mapLocks[mapId]->m_lock.AcquireReadLock();

	// get data
	bool res = CollisionMgr->isOutDoors(mapId, x, y, z);

	// release write lock
	m_mapLocks[mapId]->m_lock.ReleaseReadLock();

	// return
	return res;
}

void CCollideInterface::DeInit()
{
	// bleh.
}

#endif		// COLLISION
