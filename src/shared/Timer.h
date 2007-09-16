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

#ifndef WOW_TIMER_H
#define WOW_TIMER_H

#if PLATFORM == PLATFORM_WIN32
#   include <windows.h>
#   include <mmsystem.h>
#else
#   include <sys/timeb.h>
#endif

inline uint32 getMSTime()
{
	uint32 time_in_ms = 0;
#if PLATFORM == PLATFORM_WIN32
	time_in_ms = timeGetTime();
#else
	struct timeb tp;
	ftime(&tp);

	time_in_ms = tp.time * 1000 + tp.millitm;
#endif

	return time_in_ms;
}

class IntervalTimer
{
public:
	IntervalTimer() : _interval(0), _current(0) {}

	void Update(time_t diff) { _current += diff; if(_current<0) _current=0;}
	bool Passed() { return _current >= _interval; }
	void Reset() { if(_current >= _interval) _current = _current%_interval;  }
	
	void SetCurrent(time_t current) { _current = current; }
	void SetInterval(time_t interval) {
		_current = 0;
		_interval = interval; 
	}
	time_t GetInterval() const { return _interval; }
	time_t GetCurrent() const { return _current; }

private:
	time_t _interval;
	time_t _current;
};

#endif


