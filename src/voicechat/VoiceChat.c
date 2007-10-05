#include "VoiceChat.h"

// w00t! old-school linked-lists!
struct VoiceChatChannel * m_channelBegin;
struct VoiceChatChannel * m_channelEnd;
struct WoWServer * m_serverBegin;
struct WoWServer * m_serverEnd;

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

	memcpy(&ws->address, &address, sizeof(struct sockaddr_in));
	ws->fd = fd;

	m_serverEnd = ws;
	return ws;
}

void CloseServerConnection(struct WoWServer * srv)
{
	
}

void CloseChannelsOnServer(struct WoWServer * srv)
{

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
