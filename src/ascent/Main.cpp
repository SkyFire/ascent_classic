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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Config/ConfigEnv.h"
#include "Log.h"
#include "Master.h"
//#include <vld.h>
//#include <vldapi.h>

#ifdef WIN32
#include "../shared/CrashHandler.h"
#endif

#ifndef WIN32
#include <sys/resource.h>
#endif

uint8 loglevel = DEFAULT_LOG_LEVEL;
int main(int argc, char ** argv)
{
#ifndef WIN32
	rlimit rl;
	if (getrlimit(RLIMIT_CORE, &rl) == -1)
		printf("getrlimit failed. This could be problem.\n");
	else
	{
		rl.rlim_cur = rl.rlim_max;
		if (setrlimit(RLIMIT_CORE, &rl) == -1)
			printf("setrlimit failed. Server may not save core.dump files.\n");
	}
#endif

	//VLDEnable();
	SetThreadName("Main Thread");

#ifdef WIN32
	// Activate Crash Handler
	StartCrashHandler();

	THREAD_TRY_EXECUTION
	{
#endif
		if(!sMaster.Run(argc, argv))
		{
			sLog.outString("\nStartup failed for some reason. Please review the error messages above.");
		} else {
			sLog.outString("Server exited without failure.");
		}
#ifdef WIN32
	}
	THREAD_HANDLE_CRASH;
#endif

	exit(0);
}
