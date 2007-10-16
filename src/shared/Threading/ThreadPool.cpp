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

#else
	
	volatile int threadid_count = 0;
	int GenerateThreadId()
	{
		return ++threadid_count;
	}

#endif

#define THREAD_RESERVE 5
SERVER_DECL CThreadPool ThreadPool;

CThreadPool::CThreadPool()
{
	_threadsExitedSinceLastCheck = 0;
	_threadsRequestedSinceLastCheck = 0;
	_threadsEaten = 0;
	_threadsFreedSinceLastCheck = 0;
}

void CThreadPool::EnterSuspendPool(Thread * t)
{
	_mutex.Acquire();
	m_freeThreads.push(t);
	++_threadsEaten;
	Log.Debug("ThreadPool", "Thread %u entered the free pool.", t->ControlInterface.GetId());
	_mutex.Release();
	return;
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
		Log.Debug("ThreadPool", "Thread %u exiting.", t->ControlInterface.GetId());
		_mutex.Release();
		//delete t;
		return false;
	}

	_mutex.Release();
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
		t = m_freeThreads.front();
		m_freeThreads.pop();

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
		_mutex.Release();
		t = StartThread(ExecutionTarget);
		_mutex.Acquire();
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
	Sleep(500);
	IntegrityCheck();
}

void CThreadPool::ShowStats()
{
	_mutex.Acquire();
	Log.Debug("ThreadPool", "============ ThreadPool Status =============");
	Log.Debug("ThreadPool", "Active Threads: %u", m_activeThreads.size());
	Log.Debug("ThreadPool", "Suspended Threads: %u", m_freeThreads.size());
	Log.Debug("ThreadPool", "Requested-To-Freed Ratio: %.3f%% (%u/%u)", float( float(_threadsRequestedSinceLastCheck+1) / float(_threadsExitedSinceLastCheck+1) * 100.0f ), _threadsRequestedSinceLastCheck, _threadsExitedSinceLastCheck);
	Log.Debug("ThreadPool", "Eaten Count: %d (negative is bad!)", _threadsEaten);
	Log.Debug("ThreadPool", "============================================");
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
		_threadsEaten=0;

		for(uint32 i = 0; i < new_threads; ++i) {
			_mutex.Release();
			StartThread(NULL);
			_mutex.Acquire();
		}

		Log.Debug("ThreadPool", "IntegrityCheck: (gobbled < 0) Spawning %u threads.", new_threads);
	}
	else if(gobbled < THREAD_RESERVE)
	{
		// this means while we didn't run out of threads, we were getting damn low.
		// spawn enough threads to keep the reserve amount up.
		uint32 new_threads = (THREAD_RESERVE - gobbled);
		for(uint32 i = 0; i < new_threads; ++i) {
			_mutex.Release();
			StartThread(NULL);
			_mutex.Acquire();
		}

		Log.Debug("ThreadPool", "IntegrityCheck: (gobbled <= 5) Spawning %u threads.", new_threads);
	}
	else if(gobbled > THREAD_RESERVE)
	{
		// this means we had "excess" threads sitting around doing nothing.
		// lets kill some of them off.
		uint32 kill_count = (gobbled - THREAD_RESERVE);
		KillFreeThreads(kill_count);
		_threadsEaten -= kill_count;
		Log.Debug("ThreadPool", "IntegrityCheck: (gobbled > 5) Killing %u threads.", kill_count);
	}
	else
	{
		// perfect! we have the ideal number of free threads.
		Log.Debug("ThreadPool", "IntegrityCheck: Perfect!");
	}
	/*if(m_freeThreads.size() < 5)
	{
		uint32 j = 5 - m_freeThreads.size();
		Log.Debug("ThreadPool", "Spawning %u threads.", j);
		for(uint32 i = 0; i < j; ++i)
			StartThread(NULL);
	}*/

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

		t = m_freeThreads.front();
		m_freeThreads.pop();

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
	_mutex.Release();
	KillFreeThreads((uint32)m_freeThreads.size());
	_mutex.Acquire();

	_threadsToExit += (uint32)m_activeThreads.size();

	for(ThreadSet::iterator itr = m_activeThreads.begin(); itr != m_activeThreads.end(); ++itr)
	{
		if((*itr)->ExecutionTarget != NULL) {
			/* Allow the thread to clean up. */
			(*itr)->ExecutionTarget->OnShutdown();
		}
	}

	_mutex.Release();

	for(int i = 0; i < 4; i++) {
		if(m_activeThreads.size() || m_freeThreads.size())
		{
			Log.Debug("ThreadPool", "Remaining threads: %u active, %u free.",
				m_activeThreads.size(), m_freeThreads.size());
			Sleep(1000);
		}
		else
			break;
	}

#ifndef WIN32
#include <signal.h>
#endif
	Log.Notice("ThreadPool", "Killing any remaining threads.");
	/* Kill (nicely) any remaining threads. */
	for(ThreadSet::reverse_iterator itr = m_activeThreads.rbegin(); itr != m_activeThreads.rend(); ++itr)
	{
#ifdef WIN32
		/* TODO - Kill thread under Windows. */
#else /* Linux */
		if(pthread_kill((*itr)->ControlInterface.GetPthreadId(), SIGTERM) == 0)
		{
			m_activeThreads.erase(*itr);
			pthread_mutex_destroy(&(*itr)->setup_mutex);
			delete *itr;
		}
#endif
	}

	Sleep(2000);
	/* Kill (forcefully) any remaining threads. */
	for(ThreadSet::reverse_iterator itr = m_activeThreads.rbegin(); itr != m_activeThreads.rend(); ++itr)
	{
#ifdef WIN32
		/* TODO - Kill thread under Windows. */
#else /* Linux */
		pthread_kill((*itr)->ControlInterface.GetPthreadId(), SIGKILL);
		pthread_mutex_destroy(&(*itr)->setup_mutex);
		delete *itr;
#endif
	}
}

#ifdef WIN32

static unsigned long WINAPI thread_proc(void* param)
{
	Thread * t = (Thread*)param;
	uint32 tid = t->ControlInterface.GetId();
	bool ht = (t->ExecutionTarget != NULL);
	//Log.Debug("ThreadPool", "Thread %u started.", t->ControlInterface.GetId());

	for(;;)
	{
		if(t->ExecutionTarget != NULL)
		{
			if(t->ExecutionTarget->run())
				delete t->ExecutionTarget;

			t->ExecutionTarget = NULL;
			EnterSuspendPool(t);
		}

		if(!ThreadPool.ThreadExit(t))
			break;
		else
		{
			if(ht)
				Log.Debug("ThreadPool", "Thread %u waiting for a new task.", tid);
			// enter "suspended" state. when we return, the threadpool will either tell us to fuk off, or to execute a new task.
			t->ControlInterface.Suspend();
			// after resuming, this is where we will end up. start the loop again, check for tasks, then go back to the threadpool.
		}
	}

	delete t;
	ExitThread(0);

	// not reached
	return 0;
}

Thread * CThreadPool::StartThread(ThreadBase * ExecutionTarget)
{
	HANDLE h;
	Thread * t = new Thread;
	
	t->ExecutionTarget = ExecutionTarget;
	//h = (HANDLE)_beginthreadex(NULL, 0, &thread_proc, (void*)t, 0, NULL);
	
	/* FIXME - Ensure that the thread doesn't run until the
	 * ThreadController is setup. Otherwise the thread will access garbage
	 * data in the controller. */
	h = CreateThread(NULL, 0, &thread_proc, (LPVOID)t, 0, (LPDWORD)&t->ControlInterface.thread_id);
	t->ControlInterface.Setup(h);
	EnterSuspendPool(t);

	return t;
}

#else

static void * thread_proc(void * param)
{
	Thread * t = (Thread*)param;
	/* This is ugly, but the idea is that we can't run until StartThread()
	 * has finished setting us up. */
	pthread_mutex_lock(&t->setup_mutex);
	pthread_mutex_unlock(&t->setup_mutex);
	Log.Debug("ThreadPool", "Thread %u started.", t->ControlInterface.GetId());

	for(;;)
	{
		if(t->ExecutionTarget != NULL)
		{
			if(t->ExecutionTarget->run())
				delete t->ExecutionTarget;

			t->ExecutionTarget = NULL;
			ThreadPool.EnterSuspendPool(t);
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

	pthread_mutex_destroy(&t->setup_mutex);
	delete t;
	pthread_exit(0);
}

Thread * CThreadPool::StartThread(ThreadBase * ExecutionTarget)
{
	pthread_t target;
	Thread * t = new Thread;
	t->ExecutionTarget = ExecutionTarget;

	/* Use a mutex to "signal" the thread when it can start running. */
	pthread_mutex_init(&t->setup_mutex, NULL);
	pthread_mutex_lock(&t->setup_mutex);

	/* Create the thread.
	 * NB: We can't let this run until we setup the ThreadController,
	 * otherwise the thread will access garbage data. */
	pthread_create(&target, NULL, &thread_proc, (void*)t);
	t->ControlInterface.Setup(target);

	EnterSuspendPool(t);
	/* Signal the thread that setup is complete. */
	pthread_mutex_unlock(&t->setup_mutex);
	return t;
}

#endif
