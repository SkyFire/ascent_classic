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
	uint8 key[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	if(chn == NULL)
		return;

	if(chn->m_general || !chn->voice_enabled)
		return;

	WorldPacket data(SMSG_CHANNEL_NOTIFY_AVAILABLE_VOICE_SESSION, 17+chn->m_name.size());
	data << chn->voice_channel_id;
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
		uint8 key[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		if(chn == NULL)
			return;

		if(chn->m_general || !chn->voice_enabled)
			return;

		WorldPacket data(SMSG_VOICE_SESSION, 100);
		data.Initialize(0x039E);
		data << chn->voice_channel_id;
		data << uint16(0x0001);
		data << uint8(type);
		data << chn->m_name;
		data.append(key, 16);
		data << uint32(htonl(inet_addr("0.0.0.0")));
		data << uint16(htons(3725));
		data << uint8(1);
		data << _player->GetGUID();
		data << uint8(1);
		data << uint8(40);
		data << uint8(6);
		SendPacket(&data);
	}
}

