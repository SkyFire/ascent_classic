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

#ifndef __CHANNEL_H
#define __CHANNEL_H

using namespace std;

inline uint32 ahash(const char * str)
{
	register size_t len = strlen(str);
	register uint32 ret = 0;
	register size_t i = 0;
	for(; i < len; ++i)
		ret += 5 * ret + (tolower(str[i]));

	/*printf("%s : %u\n", str, ret);*/
	return ret;
}

class Channel
{
	Mutex m_lock;
	typedef map<Player*, uint32> MemberMap;
	MemberMap m_members;

public:
	Channel(string& name);
	~Channel();
};

#endif
