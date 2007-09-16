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

#include "../shared/CallBack.h"

template<class Type>
class PeriodicFunctionCaller : public ThreadBase
{
public:
	template<class T>
		PeriodicFunctionCaller(T * callback, void (T::*method)(), uint32 Interval)
	{
		cb = new CallbackP0<T>(callback, method);
		interval = Interval;
		running = true;
		delete_after_use=false;
	}

	~PeriodicFunctionCaller()
	{
		delete cb;
	}

	void run()
	{
#ifndef WIN32
		uint32 start = getMSTime();
		uint32 end;
		uint32 etime;
		// initial rest
		Sleep(interval);

		while(running && mrunning)
		{
			start = getMSTime();
			
			// execute the callback
			cb->execute();

			end = getMSTime();
			etime = end - start;
			if(etime < interval)
				Sleep(interval - etime);
		}
#else
		thread_active=true;
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		for(;;)
		{
			WaitForSingleObject(hEvent, interval);

			if(!running)
				break;	/* we got killed */

			/* times up */
			ResetEvent(hEvent);
			cb->execute();
		}
		thread_active=false;
#endif

	}

	void kill()
	{
		running = false;
#ifdef WIN32
		/* push the event */
		SetEvent(hEvent);
		printf("Waiting for PFC thread to exit...");
		/* wait for the thread to exit */
		while(thread_active)
		{
			Sleep(100);
		}
		printf(" done.\n");
#endif
	}

private:
	CallbackBase * cb;
	uint32 interval;
	bool running;
#ifdef WIN32
	bool thread_active;
	HANDLE hEvent;
#endif
};

#define SpawnPeriodicCallThread(otype, ptr, method, interval) \
	launch_thread(new PeriodicFunctionCaller<otype>(ptr, method, interval));
