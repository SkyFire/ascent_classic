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

#include "RStdAfx.h"

#include "../game/NameTables.h"
#include "../game/CThreads.h"
#include "../game/ThreadMgr.h"

using namespace std;

initialiseSingleton(ThreadMgr);

ThreadMgr::ThreadMgr()
{
	ThreadIdCounter = 0;
	m_SetBusy = false;
}

ThreadMgr::~ThreadMgr()
{
	sLog.outString("*** ThreadMgr Destroyed.\n");
}

void ThreadMgr::Initialize()
{

}

void ThreadMgr::AddThread(CThread *thread)
{
	_mutex.Acquire();
	mThreads.insert(thread);
	_mutex.Release();
}

void ThreadMgr::RemoveThread(CThread *thread)
{
	// dont add/remove while searching a set. ;)
	//while(m_SetBusy);
	_mutex.Acquire();
	mThreads.erase(thread);
	_mutex.Release();
}

#define SETTHREADCOUNT threadcount = mThreads.size()

void ThreadMgr::Shutdown()
{
	uint32 threadcount;
	uint32 timer = getMSTime() + 500;
	uint32 start = getMSTime();
	SETTHREADCOUNT;

	sLog.outString("*** ThreadMgr: Shutdown requested. Requesting termination from %u threads.", threadcount);

	for(ThreadSet::iterator itr = mThreads.begin(); itr != mThreads.end(); ++itr)
		(*itr)->SetThreadState(THREADSTATE_TERMINATE);

	SETTHREADCOUNT;
	sLog.outString("*** ThreadMgr: %u threads remaining.", threadcount);

	while(mThreads.size())
	{
		SETTHREADCOUNT;
		if(getMSTime() > timer)
		{
			sLog.outString("*** ThreadMgr: %u threads remaining.", threadcount);
			timer = getMSTime() + 1000;
		}
		Sleep(20);
	}

	sLog.outString("*** ThreadMgr: Shutdown completed in %u msec.", getMSTime() - start);


}

std::string ThreadMgr::ShowStatus()
{
	std::stringstream s;
	s << endl;
	s << "*** ThreadMgr: " << mThreads.size() << " threads active." << endl;
	s <<	"=============================================================================" << endl;
	s <<	"|  ID  |	  Type		|	 State	  |			Start Time		  |" << endl;
	s <<	"=============================================================================" << endl;
	for(ThreadSet::iterator itr = mThreads.begin(); itr != mThreads.end(); ++itr)
	{
		AppendThreadStatus((*itr), s);
		s << endl;
	}
	s <<	"=============================================================================" << endl;
	s << endl;

	return s.str();
}

void ThreadMgr::AppendThreadStatus(CThread* thread, std::stringstream &stream)
{
	char line[400];
	time_t stm = thread->GetStartTime();
	char tmp[40];
	strcpy(tmp, ctime(&stm));
	//strncpy(tmp, tmp, strlen(tmp)-2);
	tmp[strlen(tmp)-1] = 0;
	snprintf(line, 400, "| %4u | %16s | %14s | %30s |", thread->GetThreadId(),
		LookupName(thread->GetThreadType(), g_threadTypes),
		LookupName(thread->GetThreadState(), g_threadStates),
		tmp);

	stream.write(line, strlen(line));	
}

int ThreadMgr::GenerateThreadId()
{
	ThreadIdCounter++;
	return ThreadIdCounter;
}

#undef SETTHREADCOUNT

void ThreadMgr::PauseAllThreads()
{
	sLog.outString("*** ThreadMgr: Pausing %u threads.", mThreads.size());
	for(ThreadSet::iterator itr = mThreads.begin(); itr != mThreads.end(); ++itr)
		(*itr)->SetThreadState(THREADSTATE_PAUSED);

	// give them time to kick in
	Sleep(1000);
}

void ThreadMgr::ResumeAllThreads()
{
	sLog.outString("*** ThreadMgr: Resuming %u threads.", mThreads.size());
	for(ThreadSet::iterator itr = mThreads.begin(); itr != mThreads.end(); ++itr)
		if((*itr)->GetThreadState() == THREADSTATE_PAUSED)
			(*itr)->SetThreadState(THREADSTATE_PAUSED);

	// give them time to kick in
	Sleep(1000);
}
