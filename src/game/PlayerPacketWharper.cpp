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