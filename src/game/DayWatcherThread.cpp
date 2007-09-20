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

#include "StdAfx.h"

DayWatcherThread::DayWatcherThread() : m_cond(&m_mutex)
{
	m_running = true;
}

void DayWatcherThread::terminate()
{
	m_running = false;
	m_cond.BeginSynchronized();
	if(!m_busy)
		m_cond.Broadcast();
	m_cond.EndSynchronized();
}

void DayWatcherThread::dupe_tm_pointer(tm * returnvalue, tm * mypointer)
{
	memcpy(mypointer, returnvalue, sizeof(tm));
}

void DayWatcherThread::update_settings()
{
	CharacterDatabase.Execute("REPLACE INTO server_settings VALUES(\"last_honor_update_time\", %u)", last_honor_time);
	CharacterDatabase.Execute("REPLACE INTO server_settings VALUES(\"last_arena_update_time\", %u)", last_arena_time);
}

void DayWatcherThread::load_settings()
{
	string honor_timeout = Config.MainConfig.GetStringDefault("Periods", "HonorUpdate", "daily");
	string arena_timeout = Config.MainConfig.GetStringDefault("Periods", "ArenaUpdate", "weekly");

	honor_period = get_timeout_from_string(honor_timeout.c_str(), DAILY);
	arena_period = get_timeout_from_string(arena_timeout.c_str(), WEEKLY);

	QueryResult * result = CharacterDatabase.Query("SELECT * FROM server_settings WHERE setting_id = \"last_honor_update_time\"");
	if(result)
	{
		last_honor_time = result->Fetch()[0].GetUInt32();
		delete result;
	}
	else
		last_honor_time = 0;

	result = CharacterDatabase.Query("SELECT * FROM server_settings WHERE setting_id = \"last_arena_update_time\"");
	if(result)
	{
		last_arena_time = result->Fetch()[0].GetUInt32();
		delete result;
	}
	else
		last_arena_time = 0;
}

void DayWatcherThread::set_tm_pointers()
{
	dupe_tm_pointer(localtime(&last_arena_time), &local_last_arena_time);
	dupe_tm_pointer(localtime(&last_honor_time), &local_last_honor_time);
}

uint32 DayWatcherThread::get_timeout_from_string(const char * string, uint32 def)
{
	if(!stricmp(string, "weekly"))
		return WEEKLY;
	else if(!stricmp(string, "monthly"))
		return MONTHLY;
	else if(!stricmp(string, "daily"))
		return DAILY;
	else if(!stricmp(string, "hourly"))
		return HOURLY;
	else
		return def;
}

bool DayWatcherThread::has_timeout_expired(tm * now_time, tm * last_time, uint32 timeoutval)
{
	switch(timeoutval)
	{
	case WEEKLY:
		{
			if( (now_time->tm_mon != last_time->tm_mon) )
				return true;
            
			return ( (now_time->tm_mday / 7) != (last_time->tm_mday / 7) );
		}
		
	case MONTHLY:
		return (now_time->tm_mon != last_time->tm_mon);

	case HOURLY:
		return ((now_time->tm_hour != last_time->tm_hour) || (now_time->tm_mday != last_time->tm_mday) || (now_time->tm_mon != last_time->tm_mon));

	case DAILY:
		return ((now_time->tm_mday != last_time->tm_mday) || (now_time->tm_mday != last_time->tm_mday));
	}
	return false;
}

void DayWatcherThread::run()
{
	Log.Notice("DayWatcherThread", "Started.");
	currenttime = time(NULL);
	dupe_tm_pointer(localtime(&currenttime), &local_currenttime);
	load_settings();
	set_tm_pointers();
	
	while(ThreadState != THREADSTATE_TERMINATE)
	{
		m_cond.BeginSynchronized();

		currenttime = UNIXTIME;
		dupe_tm_pointer(localtime(&currenttime), &local_currenttime);
		if(has_timeout_expired(&local_currenttime, &local_last_honor_time, honor_period))
			update_honor();

		if(has_timeout_expired(&local_currenttime, &local_last_arena_time, arena_period))
			update_arena();
        
		if(m_dirty)
			update_settings();

		m_cond.Wait(timeout);
		if(!m_running)
		{
			m_cond.EndSynchronized();
			break;
		}

		m_cond.EndSynchronized();
	}
}

void DayWatcherThread::update_arena()
{
	QueryResult * result = CharacterDatabase.Query("SELECT ");

    //===========================================================================
	last_arena_time = UNIXTIME;
	dupe_tm_pointer(localtime(&last_arena_time), &local_last_arena_time);
	m_dirty = true;
}

void DayWatcherThread::update_honor()
{
	uint32 guid, killstoday, killsyesterday, killslifetime, honortoday, honoryesterday, honorpoints, points_to_add;
	QueryResult * result = CharacterDatabase.Query("SELECT guid, killsToday, killsYesterday, killsLifeTime, honorToday, honorYesterday, honorPoints, honorPointsToAdd");
	if(result)
	{
		Field * f = result->Fetch();
		guid = f[0].GetUInt32();
		killstoday = f[1].GetUInt32();
		killsyesterday = f[2].GetUInt32();
		killslifetime = f[3].GetUInt32();
		honortoday = f[4].GetUInt32();
		honoryesterday = f[5].GetUInt32();
		honorpoints = f[6].GetUInt32();
		points_to_add = f[7].GetUInt32();
	}

	//===========================================================================
	last_honor_time = UNIXTIME;
	dupe_tm_pointer(localtime(&last_honor_time), &local_last_arena_time);
	m_dirty = true;
}

