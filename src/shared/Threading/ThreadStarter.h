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

#ifndef _THREADING_STARTER_H
#define _THREADING_STARTER_H

class SERVER_DECL ThreadBase
{
public:
	ThreadBase() : delete_after_use(true) {}
	virtual ~ThreadBase() {}
	virtual void run() = 0;
	bool delete_after_use;
};

#ifdef WIN32

#include <process.h>

static unsigned int __stdcall thread_func(void * args)
{
	ThreadBase * ptr = (ThreadBase*)args;
	bool delete_it = ptr->delete_after_use;
	ptr->run();

	// delete the thread to call the destructor, our threads remove themselves here.
	if(delete_it)
		delete ptr;

	return 0;
}

// Use _beginthreadex to start the thread (MT runtime lib needed)
inline void launch_thread(ThreadBase * thread)
{
	_beginthreadex(0, 0, &thread_func, (void*)thread, 0, 0);
}

#else

static void * thread_func(void * args)
{
	ThreadBase * ptr = (ThreadBase*)args;
	bool delete_it = ptr->delete_after_use;
	ptr->run();

	// delete the thread to call the destructor, our threads remove themselves here.
	if(delete_it)
		delete ptr;
	
	return 0;
}

// Use pthread_create to start the thread
inline void launch_thread(ThreadBase * thread)
{
	pthread_t t;
	pthread_create(&t, 0, thread_func, (void*)thread);
}

#endif

#endif

