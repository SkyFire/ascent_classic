/*
 * Ascent MMORPG Server
 * Voice Chat Engine
 * Copyright (C) 2005-2007 Burlex <burlex@gmail.com>
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

#include "VoiceChat.h"

// w00t! old-school linked-lists!
struct VoiceChatChannel * m_channelBegin;
struct VoiceChatChannel * m_channelEnd;
struct WoWServer * m_serverBegin;
struct WoWServer * m_serverEnd;
int m_channelIdHigh = 1;

struct VoiceChatChannel * GetChannel(uint16 channel_id)
{
	struct VoiceChatChannel * chn = m_channelBegin;
	while(chn != NULL)
	{
		if(chn->channel_id == channel_id)
			return chn;

		chn = chn->next;
	}

	return NULL;
}

struct VoiceChatChannelMember * GetChannelMember(uint8 user_id, struct VoiceChatChannel * channel)
{
	if(user_id > channel->member_slots)
		return NULL;

	return &channel->members[user_id];
}

struct VoiceChatChannel * CreateChannel(uint16 channel_id, uint8 channel_type)
{
	struct VoiceChatChannel * chn;
	struct VoiceChatChannel * chn_prev;
	struct VoiceChatChannel * chn_loop;

	chn = (struct VoiceChatChannel*)malloc(sizeof(struct VoiceChatChannel));
	assert(chn);

	chn->channel_id = channel_id;

	// update the linked list
	chn_prev = NULL;
    chn_loop = m_channelBegin;

	while(chn_loop != NULL)
	{
		chn_prev = chn_loop;
		chn_loop = chn_loop->next;
	}

	chn->next = NULL;
	if(chn_prev == NULL)		// first element in the list
	{
		m_channelBegin = chn;
		chn->prev = chn->next = NULL;
	}
	else
	{
		chn_prev->next = chn;
		chn->prev = chn_prev;
		chn->next = NULL;
	}

	switch(channel_type)
	{
	case 0:		// channel, allocate 250 members
		chn->member_slots = 250;
		break;

	case 2:		// party, allocate 5 members
		chn->member_slots = 5;
		break;

	case 3:		// raid, allocate 40 members
		chn->member_slots = 40;
		break;

	default:
		assert(0);
		break;
	}

	chn->members = (struct VoiceChatChannelMember*)malloc(sizeof(struct VoiceChatChannelMember) * chn->member_slots);
	chn->member_count = 0;
	m_channelEnd = chn;

	return chn;
}


struct WoWServer * CreateServer(int fd, struct sockaddr_in* address)
{
	struct WoWServer * ws;
	struct WoWServer * prev;
	struct WoWServer * current;

	ws = (struct WoWServer*)malloc(sizeof(struct WoWServer));
	memset(ws, 0, sizeof(struct WoWServer));
	assert(ws);

	prev = NULL;
	current = m_serverBegin;
	while(current != NULL)
	{
		prev = current;
		current = current->next;
	}

	if(prev == NULL)
	{
		m_serverBegin = ws;
		ws->prev = ws->next = NULL;
	}
	else
	{
		prev->next = ws;
		ws->prev = prev;
		ws->next = NULL;
	}

	memcpy(&ws->address, address, sizeof(struct sockaddr_in));
	ws->fd = fd;

	m_serverEnd = ws;
	return ws;
}

void CloseServerConnection(struct WoWServer * srv)
{
	printf("server connection: %s:%u is now closed.\n", inet_ntoa(srv->address.sin_addr), ntohs(srv->address.sin_port));
	CloseChannelsOnServer(srv);

	if(srv->prev == NULL)
		m_serverBegin = srv->next;
	else
		srv->prev->next = srv->next;

	if(srv->next != NULL)
		srv->next->prev = srv->prev;

	if(m_serverEnd == srv)
		m_serverEnd = srv->prev;

	free(srv);
}

void CloseChannel(struct VoiceChatChannel* chn)
{
	if(chn->prev == NULL)
		m_channelBegin = chn->next;
	else
		chn->prev->next = chn->next;

	if(chn->next != NULL)
		chn->next->prev = chn->prev;

	if(m_channelEnd == chn)
		m_channelEnd = chn->prev;

	free(chn->members);
	free(chn);
}

void CloseChannelsOnServer(struct WoWServer * srv)
{
	int i;
	for(i = 0; i < 1000; ++i)
	{
		if(srv->channels[i].channel_id != 0)
			CloseChannel(&srv->channels[i]);
	}
}

struct WoWServer * GetServer(int fd)
{
	struct WoWServer * current = m_serverBegin;
	while(current != NULL)
	{
		if(current->fd == fd)
			return current;

		current = current->next;
	}

	return NULL;
}

int GenerateChannelId()
{
	return m_channelIdHigh++;
}
