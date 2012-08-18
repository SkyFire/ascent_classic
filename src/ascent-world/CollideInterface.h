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

#ifndef _COLLIDEINTERFACE_H
#define _COLLIDEINTERFACE_H

/* imports */
#define NO_WMO_HEIGHT -100000.0f

#ifdef WIN32
#define COLLISION_IMPORT __declspec(dllimport)
#else
#define COLLISION_IMPORT 
#endif

/* The class the dll sends back. */
class SERVER_DECL IVMapManager
{
public:
	IVMapManager() {}

	virtual ~IVMapManager(void) {}

	virtual int loadMap(const char* pBasePath, unsigned int pMapId, int x, int y) = 0;

	virtual void unloadMap(unsigned int pMapId, int x, int y) = 0;
	virtual void unloadMap(unsigned int pMapId) = 0;

	// LOS
	virtual bool isInLineOfSight(unsigned int pMapId, float x1, float y1, float z1, float x2, float y2, float z2) = 0;

	// Height
	virtual float getHeight(unsigned int pMapId, float x, float y, float z) = 0;

	// Indoors
	virtual bool isInDoors(unsigned int mapid, float x, float y, float z) = 0;

	// Outdoors
	virtual bool isOutDoors(unsigned int mapid, float x, float y, float z) = 0;

	// Closest Point
	virtual bool getObjectHitPos(unsigned int pMapId, float x1, float y1, float z1, float x2, float y2, float z2, float& rx, float &ry, float& rz, float pModifyDist) = 0;
	virtual std::string getDirFileName(unsigned int pMapId, int x, int y) const =0;

	/*// debugging
	virtual void setDebugPoint(float x, float y, float z, float o);*/
};

COLLISION_IMPORT void * collision_init();
COLLISION_IMPORT void collision_shutdown();

class SERVER_DECL CCollideInterface
{
public:
	void Init();
	void DeInit();

	void ActivateTile(uint32 mapId, uint32 tileX, uint32 tileY);
	void DeactivateTile(uint32 mapId, uint32 tileX, uint32 tileY);
	void ActivateMap(uint32 mapId);
	void DeactivateMap(uint32 mapId);

	bool CheckLOS(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2);
	bool GetFirstPoint(uint32 mapId, float x1, float y1, float z1, float x2, float y2, float z2, float & outx, float & outy, float & outz, float distmod);
	bool IsIndoor(uint32 mapId, float x, float y, float z);
	bool IsOutdoor(uint32 mapId, float x, float y, float z);
	float GetHeight(uint32 mapId, float x, float y, float z);
};

extern SERVER_DECL CCollideInterface CollideInterface;

#endif
