/*
 * Thread Pool Class
 * Copyright (C) Burlex <burlex@gmail.com>
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

#include "../Common.h"
#include "Mutex.h"
#include "ThreadStarter.h"

#ifndef __THREADPOOL_H
#define __THREADPOOL_H

#ifdef WIN32

class SERVER_DECL ThreadController
{
	HANDLE hThread;
	uint32 thread_id;
public:

	void Setup(HANDLE h)
	{
		hThread = h;
		// whoops! GetThreadId is for windows 2003 and up only! :<		 - Burlex
		//thread_id = (uint32)GetThreadId(h);
	}

	void Setup2()
	{
		thread_id = GetCurrentThreadId();
	}

	void Suspend()
	{
		// We can't be suspended by someone else. That is a big-no-no and will lead to crashes.
		ASSERT(GetCurrentThreadId() == thread_id);
		SuspendThread(hThread);
	}

	void Resume()
	{
		// This SHOULD be called by someone else.
		ASSERT(GetCurrentThreadId() != thread_id);
		if(!ResumeThread(hThread))
		{
			DWORD le = GetLastError();
			printf("lasterror: %u\n", le);
		}
	}

	void Join()
	{
		WaitForSingleObject(hThread, INFINITE);
	}

	uint32 GetId() { return thread_id; }
};

#else

class ThreadController
{
	sem_t sem;
	pthread_t handle;
public:
	void Setup(pthread_t h);
	{
		handle = h;
		sem_init(&sem);
	}
	~ThreadController()
	{
		sem_destroy(&sem);
	}

	void Suspend()
	{
		ASSERT(pthread_self() == handle);
		sem_wait(&sem);
	}

	void Resume()
	{
		ASSERT(pthread_self() != handle);
		sem_post(&sem);
	}

	void Join()
	{
		// waits until the thread finishes then returns
		pthread_join(&handle);
	}

	inline uint32 GetId() { return (uint32)handle; }
};

#endif

struct SERVER_DECL Thread
{
	ThreadBase * ExecutionTarget;
	ThreadController ControlInterface;
};

typedef std::set<Thread*> ThreadSet;

class CThreadPool
{
	int GetNumCpus();

	uint32 _threadsRequestedSinceLastCheck;
	uint32 _threadsFreedSinceLastCheck;
	uint32 _threadsExitedSinceLastCheck;
	uint32 _threadsToExit;
	int32 _threadsEaten;
	Mutex _mutex;

    ThreadSet m_activeThreads;
	ThreadSet m_freeThreads;

public:
	CThreadPool();

	// call every 2 minutes or so.
	void IntegrityCheck();

	// call at startup
	void Startup();

	// shutdown all threads
	void Shutdown();
	
	// return true - suspend ourselves, and wait for a future task.
	// return false - exit, we're shutting down or no longer needed.
	bool ThreadExit(Thread * t);

	// creates a thread, returns a handle to it.
	Thread * StartThread(ThreadBase * ExecutionTarget);

	// grabs/spawns a thread, and tells it to execute a task.
	void ExecuteTask(ThreadBase * ExecutionTarget);

	// prints some neat debug stats
	void ShowStats();

	// kills x free threads
	void KillFreeThreads(uint32 count);

	// resets the gobble counter
	inline void Gobble() { _threadsEaten=0; }
};

extern CThreadPool ThreadPool;

#endif
