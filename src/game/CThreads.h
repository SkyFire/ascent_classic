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

#ifndef _MAPUPDATER_H
#define _MAPUPDATER_H

class MapMgr;
class Object;
class Player;
class WorldSession;
class Creature;
class GameObject;

#define MAPMGR_UPDATEOBJECT_LOOP_DELAY 100
#define MAPMGR_SESSION_UPDATE_DELAY	50

#define MAPMGR_UPDATE_DELAY			100

enum CThreadState
{
	THREADSTATE_TERMINATE = 0,
	THREADSTATE_PAUSED	= 1,
	THREADSTATE_SLEEPING  = 2,
	THREADSTATE_BUSY	  = 3,
	THREADSTATE_AWAITING  = 4,
};

enum CThreadType
{
	THREADTYPE_UNASSIGNED,	  
	THREADTYPE_OBJECTUPDATER,
	THREADTYPE_MAPMGR,
	THREADTYPE_WORLDRUNNABLE,
	THREADTYPE_MAILDELIVERY,
	THREADTYPE_CONSOLEINTERFACE,
	THREADTYPE_IRCBOT,
	THREADTYPE_AUCTIONHOUSE,
	THREADTYPE_NETWORK,
	THREADTYPE_SESSIONUPDATER,
	THREADTYPE_SOCKETUPDATER,
	THREADTYPE_DATABASE,
	THREADTYPE_IOCPLISTENER,
	THREADTYPE_IOCPWORKER,
};

struct NameTableEntry;
extern NameTableEntry g_threadStates[];
extern NameTableEntry g_threadTypes[];

class SERVER_DECL CThread : public ThreadBase
{
	friend class ThreadMgr;
public:
	CThread();
	~CThread();

	inline void SetThreadState(CThreadState thread_state) { ThreadState = thread_state; }
	inline CThreadState GetThreadState() { return ThreadState; }
	inline CThreadType GetThreadType() { return ThreadType; }
	int GetThreadId() { return ThreadId; }
	time_t GetStartTime() { return start_time; }
	virtual void run();
	void manual_start();

protected:
	CThreadState ThreadState;
	CThreadType  ThreadType;
	time_t start_time;
	int ThreadId;
};

#endif
