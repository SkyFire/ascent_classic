#include "vmap/IVMapManager.h"
#include "vmap/VMapManager.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "LocationVector.h"

#ifdef WIN32
#define EXPORTME __declspec(dllexport)
#else
#define EXPORTME
#endif

VMAP::VMapManager* vmgr;

EXPORTME void * collision_init()
{
	vmgr = new VMAP::VMapManager();
	return vmgr;
}

EXPORTME void collision_shutdown()
{
	vmgr->unloadMap(0);
	vmgr->unloadMap(1);
	vmgr->unloadMap(530);
}

