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

#include "ThreadPool.h"
#include "../NGLog.h"

#ifdef WIN32
#include <process.h>
#endif

#define THREAD_RESERVE 5

CThreadPool ThreadPool;

CThreadPool::CThreadPool()
{
	_threadsExitedSinceLastCheck = 0;
	_threadsRequestedSinceLastCheck = 0;
	_threadsEaten = 0;
	_threadsFreedSinceLastCheck = 0;
}

bool CThreadPool::ThreadExit(Thread * t)
{
	_mutex.Acquire();
	
	// we're definitely no longer active
	m_activeThreads.erase(t);

	// do we have to kill off some threads?
	if(_threadsToExit > 0)
	{
		// kill us.
		--_threadsToExit;
		++_threadsExitedSinceLastCheck;
		_mutex.Release();
		delete t;
		return false;
	}

	// enter the "suspended" pool
	++_threadsExitedSinceLastCheck;
	m_freeThreads.insert(t);
	_mutex.Release();
	Log.Debug("ThreadPool", "Thread %u entered the free pool.", t->ControlInterface.GetId());
	return true;
}

void CThreadPool::ExecuteTask(ThreadBase * ExecutionTarget)
{
	Thread * t;
	_mutex.Acquire();
	++_threadsRequestedSinceLastCheck;
	--_threadsEaten;

	// grab one from the pool, if we have any.
	if(m_freeThreads.size())
	{
		t = *m_freeThreads.begin();
		m_freeThreads.erase(m_freeThreads.begin());

		// execute the task on this thread.
		t->ExecutionTarget = ExecutionTarget;

		// resume the thread, and it should start working.
		t->ControlInterface.Resume();
		Log.Debug("ThreadPool", "Thread %u left the thread pool.", t->ControlInterface.GetId());
	}
	else
	{
		// creating a new thread means it heads straight to its task.
		// no need to resume it :)
		t = StartThread(ExecutionTarget);
	}

	// add the thread to the active set
	Log.Debug("ThreadPool", "Thread %u is now executing task at 0x%p.", t->ControlInterface.GetId(), ExecutionTarget);
	m_activeThreads.insert(t);
	_mutex.Release();
}

void CThreadPool::Startup()
{
	int i;
	int tcount = THREAD_RESERVE;

	for(i=0; i < tcount; ++i)
		StartThread(NULL);

	Log.Debug("ThreadPool", "Startup, launched %u threads.", tcount);
	IntegrityCheck();
}

void CThreadPool::ShowStats()
{
	_mutex.Acquire();
	Log.Debug("ThreadPool", "============ ThreadPool Status =============");
	Log.Debug("ThreadPool", "Active Threads: %u", m_activeThreads.size());
	Log.Debug("ThreadPool", "Suspended Threads: %u", m_freeThreads.size());
	Log.Debug("ThreadPool", "Requested-To-Freed Ratio: %.3f%% (%u/%u)", float( float(_threadsRequestedSinceLastCheck) / float(_threadsExitedSinceLastCheck) * 100.0f ), _threadsRequestedSinceLastCheck, _threadsExitedSinceLastCheck);
	Log.Debug("ThreadPool", "Eaten Count: %d (negative is bad!)", _threadsEaten);
	Log.Debug("TrheadPool", "============================================");
	_mutex.Release();
}

void CThreadPool::IntegrityCheck()
{
	_mutex.Acquire();
	int32 gobbled = _threadsEaten;

    if(gobbled < 0)
	{
		// this means we requested more threads than we had in the pool last time.
        // spawn "gobbled" + THREAD_RESERVE extra threads.
		uint32 new_threads = abs(gobbled) + THREAD_RESERVE;
		for(uint32 i = 0; i < new_threads; ++i)
			StartThread(NULL);

		_threadsEaten = new_threads;
	}
	else if(gobbled <= THREAD_RESERVE)
	{
        // this means while we didn't run out of threads, we were getting damn low.
		// spawn enough threads to keep the reserve amount up.
		uint32 new_threads = (THREAD_RESERVE - gobbled);
		for(uint32 i = 0; i < new_threads; ++i)
			StartThread(NULL);

		_threadsEaten = new_threads;
	}
	else if(gobbled > THREAD_RESERVE)
	{
		// this means we had "excess" threads sitting around doing nothing.
		// lets kill some of them off.
		uint32 kill_count = (gobbled - THREAD_RESERVE);
		KillFreeThreads(kill_count);
		_threadsEaten -= kill_count;
	}
	else
	{
		// perfect! we have the ideal number of free threads.
	}

	_threadsExitedSinceLastCheck = 0;
	_threadsRequestedSinceLastCheck = 0;
	_threadsFreedSinceLastCheck = 0;

	_mutex.Release();
}

void CThreadPool::KillFreeThreads(uint32 count)
{
	Log.Debug("ThreadPool", "Killing %u excess threads.", count);
	_mutex.Acquire();
	Thread * t;
	for(uint32 i = 0; i < count; ++i)
	{
		if(m_freeThreads.size() == 0)
		{
			_mutex.Release();
			return;
		}

		t = *m_freeThreads.begin();
		m_freeThreads.erase(m_freeThreads.begin());

		t->ExecutionTarget = NULL; 
		++_threadsToExit;
		t->ControlInterface.Resume();
	}
	_mutex.Release();
}

void CThreadPool::Shutdown()
{
	_mutex.Acquire();
	size_t tcount = m_activeThreads.size() + m_freeThreads.size();		// exit all
	Log.Debug("ThreadPool", "Shutting down %u threads.", tcount);
	KillFreeThreads((uint32)m_freeThreads.size());
	_threadsToExit += m_activeThreads.size();

	for(ThreadSet::iterator itr = m_activeThreads.begin(); itr != m_activeThreads.end(); ++itr)
	{
		if((*itr)->ExecutionTarget)
			(*itr)->ExecutionTarget->OnShutdown();
	}
	_mutex.Release();

	for(;;)
	{
		_mutex.Acquire();
		if(m_activeThreads.size() || m_freeThreads.size())
		{
			Log.Debug("ThreadPool", "%u threads remaining...",m_activeThreads.size() + m_freeThreads.size() );
			_mutex.Release();
			Sleep(1000);
			continue;
		}

		break;
	}
}

/* this is the only platform-specific code. neat, huh! */
#ifdef WIN32

static unsigned int __stdcall thread_proc(void* param)
{
	Thread * t = (Thread*)param;
	Log.Debug("ThreadPool", "Thread %u started.", t->ControlInterface.GetId());

	for(;;)
	{
		if(t->ExecutionTarget != NULL)
		{
			t->ExecutionTarget->run();
			if(t->ExecutionTarget->delete_after_use)
				delete t->ExecutionTarget;

			t->ExecutionTarget = NULL;
		}

		if(!ThreadPool.ThreadExit(t))
			break;
		else
		{
			// enter "suspended" state. when we return, the threadpool will either tell us to fuk off, or to execute a new task.
			t->ControlInterface.Suspend();
			// after resuming, this is where we will end up. start the loop again, check for tasks, then go back to the threadpool.
		}
	}

	// at this point the t pointer has already been freed, so we can just cleanly exit.
	ExitThread(0);
}

Thread * CThreadPool::StartThread(ThreadBase * ExecutionTarget)
{
	HANDLE h;
	Thread * t = new Thread;
	
	t->ExecutionTarget = ExecutionTarget;
	h = (HANDLE)_beginthreadex(NULL, 0, &thread_proc, (void*)t, 0, NULL);
	t->ControlInterface.Setup(h);

	return t;
}

#else

static void * thread_proc(void * param)
{
	Thread * t = (Thread*)param;
	t->ControlInterface.Setup();
	Log.Debug("Thread %u started.", t->ControlInterface.GetId());

	for(;;)
	{
		if(t->ExecutionTarget != NULL)
		{
			t->ExecutionTarget->run();
			if(t->ExecutionTarget->delete_after_use)
				delete t->ExecutionTarget;

			t->ExecutionTarget = NULL
		}

		if(!ThreadPool.ThreadExit(t))
			break;
		else
		{
			// enter "suspended" state. when we return, the threadpool will either tell us to fuk off, or to execute a new task.
			t->ControlInterface.Suspend();
			// after resuming, this is where we will end up. start the loop again, check for tasks, then go back to the threadpool.
		}
	}

	pthread_exit(0);
}

Thread * CThreadPool::StartThread(ThreadBase * ExecutionTarget)
{
	pthread_t target;
	Thread * t = new Thread;
	t->ExecutionTarget = ExecutionTarget;

	pthread_create(&target, NULL, &thread_proc, (void*)t);
	t->ControlInterface.Setup(target);
	return t;
}

#endif