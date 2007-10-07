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

#include "StdAfx.h"

void WorldSession::HandleEnableMicrophoneOpcode(WorldPacket & recv_data)
{
	uint8 enabled, unk;
	recv_data >> enabled;
	recv_data >> unk;

	if(!enabled)
	{
		/*
		{SERVER} Packet: (0x039F) UNKNOWN PacketSize = 16
		|------------------------------------------------|----------------|
		|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
		|------------------------------------------------|----------------|
		|F3 0D 13 01 00 00 00 00 4A C5 00 00 00 00 D1 E1 |........J.......|
		-------------------------------------------------------------------
		uint64 player_guid
		uint64 channel_id
		*/
	}
	else
	{
		/*
		{SERVER} Packet: (0x03D8) UNKNOWN PacketSize = 24
		|------------------------------------------------|----------------|
		|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
		|------------------------------------------------|----------------|
		|4B C5 00 00 00 00 D1 E1 00 6D 6F 6F 63 6F 77 00 |K........moocow.|
		|F3 0D 13 01 00 00 00 00                         |........        |
		-------------------------------------------------------------------
		uint8 channel_id?
		uint8 channel_type (00=custom channel, 03=party, 04=raid?)
		string channel_name (not applicable to party/raid)
		uint64 player_guid
		*/
	}
}

void WorldSession::HandleChannelVoiceQueryOpcode(WorldPacket & recv_data)
{
	string name;
	recv_data >> name;

	// custom channel
	Channel * chn = channelmgr.GetChannel(name.c_str(), _player);
	if(chn == NULL || chn->voice_enabled == false || chn->m_general)
		return;

	WorldPacket data(SMSG_CHANNEL_NOTIFY_AVAILABLE_VOICE_SESSION, 17+chn->m_name.size());
	data << chn->m_id << uint32(0);
	data << uint8(0);		// 00=custom,03=party,04=raid
	data << chn->m_name;
	data << _player->GetGUID();
	SendPacket(&data);
}

void WorldSession::HandleVoiceChatQueryOpcode(WorldPacket & recv_data)
{
	uint32 type;
	string name;
	recv_data >> type >> name;
	if(type == 0)
	{
		// custom channel
		Channel * chn = channelmgr.GetChannel(name.c_str(), _player);
		if(chn == NULL)
			return;

		if(chn->m_general || !chn->voice_enabled)
			return;

		chn->JoinVoiceChannel(_player);
	}
}



/************************************************************************/
/* Singleton Stuff                                                      */
/************************************************************************/
initialiseSingleton(VoiceChatHandler);

VoiceChatHandler::VoiceChatHandler()
{
	request_high=1;
	next_connect = 0;
	m_client = 0;
	port =0;
	ip = 0;
	enabled=false;
}

void VoiceChatHandler::OnRead(const uint8 * bytes, uint32 len)
{
	if(len<4) return;

	int cmd = *(int*)bytes;
	Log.Debug("VoiceChatHandler", "Got Packet %u", cmd);
	switch(cmd)
	{
	case VOICECHAT_SMSG_CHANNEL_CREATED:
		{
			uint32 request_id = *(uint32*)&bytes[4];
			uint32 channel_id = *(uint32*)&bytes[8];
			Log.Debug("VoiceChatHandler", "Request ID %u, channel id %u", request_id, channel_id);

			for(vector<VoiceChatChannelRequest>::iterator itr = m_requests.begin(); itr != m_requests.end(); ++itr)
			{
				if(itr->id == request_id)
				{
					Channel * chn = channelmgr.GetChannel(itr->channel_name.c_str(), itr->team);
					if(chn != NULL)
						chn->VoiceChannelCreated((uint16)channel_id);

					m_requests.erase(itr);
					break;
				}
			}
		}break;
	}
}

void VoiceChatHandler::SocketDisconnected()
{
	Log.Debug("VoiceChatHandler", "SocketDisconnected");
	m_client = NULL;
	m_requests.clear();
	channelmgr.VoiceDied();

	WorldPacket data(SMSG_VOICE_SYSTEM_STATUS, 2);
	data << uint8(2);
	data << uint8(0);
	sWorld.SendGlobalMessage(&data, NULL);
	next_connect = UNIXTIME + 5;
}

bool VoiceChatHandler::CanUseVoiceChat()
{
	return (enabled && m_client != NULL);
}

void VoiceChatHandler::CreateVoiceChannel(Channel * chn)
{
	if(m_client == NULL)
		return;

	Log.Debug("VoiceChatHandler", "CreateVoiceChannel %s", chn->m_name.c_str());
	ByteBuffer buf(50);
	VoiceChatChannelRequest req;

	m_lock.Acquire();

	req.id = request_high++;
	req.channel_name = chn->m_name;
	req.team = chn->m_team;
	m_requests.push_back(req);

	buf << uint32(VOICECHAT_CMSG_CREATE_CHANNEL);
	buf << uint32(0);
	buf << req.id;
	m_client->Send(buf.contents(), 12);
	m_lock.Release();
}

void VoiceChatHandler::DestroyVoiceChannel(Channel * chn)
{
	Log.Debug("VoiceChatHandler", "DestroyVoiceChannel %s", chn->m_name.c_str());

	if(chn->i_voice_channel_id != (uint16)-1 && m_client)
	{
		ByteBuffer buf(15);
		buf << uint32(VOICECHAT_CMSG_DELETE_CHANNEL);
		buf << uint32(chn->i_voice_channel_id);
		m_client->Send(buf.contents(), 8);
	}

	chn->VoiceDied();
}

void VoiceChatHandler::Startup()
{
	ip_s = Config.MainConfig.GetStringDefault("VoiceChat", "ServerIP", "127.0.0.1");
	port = Config.MainConfig.GetIntDefault("VoiceChat", "ServerPort", 3727);
	enabled = Config.MainConfig.GetBoolDefault("VoiceChat", "Enabled", false);
	if(!enabled)
		return;

	ip = inet_addr(ip_s.c_str());
	next_connect = 0;
	Update();
}

void VoiceChatHandler::Update()
{
	if(!enabled || m_client)
		return;

	if(UNIXTIME > next_connect)
	{
		Log.Notice("VoiceChatHandler", "Attempting to connect to voicechat server %s:%u", ip_s.c_str(), port);
		VoiceChatClientSocket * s = ConnectTCPSocket<VoiceChatClientSocket>(ip_s.c_str(), port);
		if(s != NULL)
		{
			// connected!
			m_client = s;
			Log.Notice("VoiceChatHandler", "Connected to %s:%u.", ip_s.c_str(), port);
			WorldPacket data(SMSG_VOICE_SYSTEM_STATUS, 2);
			data << uint8(2) << uint8(1);
			sWorld.SendGlobalMessage(&data, NULL);
		}
		else
		{
			Log.Notice("VoiceChatHandler", "Could not connect. Will try again later.");
			m_client = NULL;
			next_connect = UNIXTIME + 10;
		}
	}
}
