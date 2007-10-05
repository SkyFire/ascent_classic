
#ifndef _VOICECHAT_H
#define _VOICECHAT_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
#endif

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

struct VoiceChatChannelMember
{
	struct sockaddr_in address;
	uint8 encryption_key[16];
};

struct VoiceChatChannel
{
	uint32 channel_id;
	uint32 member_count;
	uint32 member_slots;
	struct VoiceChatChannelMember * members;
	struct VoiceChatChannel * next;
	struct VoiceChatChannel * prev;
};

struct VoiceChatChannel * GetChannel(uint16 channel_id);
struct VoiceChatChannelMember * GetChannelMember(uint8 user_id, struct VoiceChatChannel * channel);
struct WoWServer * CreateServer(int fd, struct sockaddr_in* address);
struct WoWServer * GetServer(int fd);
void CloseChannelsOnServer(struct WoWServer * srv);
void CloseServerConnection(struct WoWServer * srv);

struct WoWServer
{
	struct VoiceChatChannel * channels[1000];
	struct sockaddr_in address;
	int fd;
	struct WoWServer * next;
	struct WoWServer * prev;
};

extern struct VoiceChatChannel * m_channelBegin;
extern struct VoiceChatChannel * m_channelEnd;
extern struct WoWServer * m_serverBegin;
extern struct WoWServer * m_serverEnd;

#endif	//_VOICECHAT_H
