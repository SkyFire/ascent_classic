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

/// collection of client packets in a warper to keep the player.cpp clean

#pragma pack(push,1) 
struct Levelup_Info_Packet
{
    uint32 level;
    uint32 Hp;
    uint32 Mana;
    uint32 unk0;
    uint32 unk1;
    uint32 unk2;
    uint32 unk3;
    uint32 Stat0;
    uint32 Stat1;
    uint32 Stat2;
    uint32 Stat3;
    uint32 Stat4;
};

struct LogXPGainPacket
{
    uint64 guid;                                    // Player guid
    uint32 xp;                                      // Normal XP
    uint8  type;                                    // 0 for xp gained from killing creature's and 1 for xp gained from quests
    uint32 restxp;                                  // "Rest XP", is equal to XP for no rest xp message
    float  unk2; //1.0f                             // static data.. Seems to always be 1.0f
};

struct LogQuestXPGainPacket
{
    uint64 guid;                                    // Always 0
    uint32 xp;                                      // Normal XP
    uint8  type;                                    // Unknown.. seems to always be 0		
};

#pragma pack(pop)

    
void Player::SendLevelupInfo(uint32 level, uint32 Hp, uint32 Mana, uint32 Stat0, uint32 Stat1, uint32 Stat2, uint32 Stat3, uint32 Stat4)
{
    Levelup_Info_Packet packet;
    packet.level = level;
    packet.Hp = Hp;
    packet.Mana = Mana;

    // grep: these are probably the other powers :)
    packet.unk0 = 0;
    packet.unk1 = 0;
    packet.unk2 = 0;
    packet.unk3 = 0;

    // Append stat differences
    packet.Stat0 = Stat0;
    packet.Stat1 = Stat1;
    packet.Stat2 = Stat2;
    packet.Stat3 = Stat3;
    packet.Stat4 = Stat4;
    GetSession()->OutPacket(SMSG_LEVELUP_INFO, sizeof(Levelup_Info_Packet),(const char*)&packet);
}

void Player::SendSpellCoolDown(uint32 SpellID, uint16 Time)
{
    if (!IsInWorld())
        return;

    WorldPacket data(SMSG_SPELL_COOLDOWN, 14);
    data << SpellID;
    data << GetNewGUID();
    data << Time;
    GetSession()->SendPacket(&data);
}

void Player::SendCastResult(uint32 SpellId, uint8 ErrorMessage, uint32 Extra)
{
#ifndef USING_BIG_ENDIAN
    StackWorldPacket<9> data(SMSG_CAST_RESULT);
#else
    WorldPacket data(SMSG_CAST_RESULT, 9);
#endif
    data << SpellId;
    data << ErrorMessage;
    if (Extra)
        data << Extra;
    GetSession()->SendPacket(&data);
}

void Player::SendLogXPGain(uint64 guid, uint32 NormalXP, uint32 RestedXP, uint8 type)
{
    if (!type)
    {
        LogXPGainPacket packet;
        packet.guid     = guid;
        packet.xp       = NormalXP;
        packet.type     = type;
        packet.restxp   = RestedXP;
        packet.unk2     = 1.0f;
        GetSession()->OutPacket(SMSG_LOG_XPGAIN, sizeof(LogXPGainPacket),(const char*)&packet);
    }
    else if (type == 1)
    {
        LogQuestXPGainPacket packet;
        packet.guid = 0; // does not need to be set for quest xp
        packet.xp = NormalXP;
        packet.type = type;
        GetSession()->OutPacket(SMSG_LOG_XPGAIN, sizeof(LogQuestXPGainPacket),(const char*)&packet);
    }
}