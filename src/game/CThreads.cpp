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

// Class CThread - Base class for all threads in the
// server, and allows for easy management by ThreadMgr.

#include "Common.h"
#include "NameTables.h"
#include "CThreads.h"
#include "ThreadMgr.h"

/* Name tables */
NameTableEntry g_threadStates[] = {
	{ THREADSTATE_TERMINATE, "STATE_TERM" },
	{ THREADSTATE_PAUSED, "TSTATE_PAUSE" },
	{ THREADSTATE_SLEEPING, "STATE_SLEEP" },
	{ THREADSTATE_BUSY, "STATE_BUSY" },
	{ THREADSTATE_AWAITING, "STATE_AWAIT" },
	{ 0, NULL },
};

NameTableEntry g_threadTypes[] = {
	{ THREADTYPE_UNASSIGNED, "UNASSIGNED" },
	{ THREADTYPE_OBJECTUPDATER, "OBJECTUPDATER" },
	{ THREADTYPE_MAPMGR, "MAPMGR" },
	{ THREADTYPE_WORLDRUNNABLE, "WORLDRUNNABLE" },
	{ THREADTYPE_MAILDELIVERY, "MAILDELIVERY" },
	{ THREADTYPE_CONSOLEINTERFACE, "CONSOLE" },
	{ THREADTYPE_IRCBOT, "IRCBOT" },
	{ THREADTYPE_AUCTIONHOUSE, "AUCTIONHOUSE" },
	{ THREADTYPE_NETWORK, "NETWORK" },
	{ THREADTYPE_SESSIONUPDATER, "SESSIONUPDATER" },
	{ THREADTYPE_SOCKETUPDATER, "SOCKETUPDATER" },
	{ THREADTYPE_DATABASE, "DATABASEQUEUE" },
	{ THREADTYPE_IOCPWORKER, "IOCPWORKER" },
	{ THREADTYPE_IOCPLISTENER, "IOCPLISTENER" },
	{ 0, NULL },
};

CThread::CThread() : ThreadBase()
{
	ThreadState = THREADSTATE_AWAITING;
	ThreadType  = THREADTYPE_UNASSIGNED;
	ThreadId	= sThreadMgr.GenerateThreadId();
	start_time  = time(NULL);
	sThreadMgr.AddThread(this);
}

CThread::~CThread()
{
	if(ThreadType == THREADTYPE_CONSOLEINTERFACE || !delete_after_use)
		return; // we're deleted manually
	sThreadMgr.RemoveThread(this);
}

void CThread::run()
{

}

void CThread::manual_start()
{
	launch_thread(this);
}
