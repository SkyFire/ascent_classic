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
#define SWIMMING_TOLERANCE_LEVEL -0.08f
#define MOVEMENT_PACKET_TIME_DELAY 50

void WorldSession::HandleMoveWorldportAckOpcode( WorldPacket & recv_data )
{
	if(_player->IsInWorld())
	{
		// get outta here
		return;
	}
	sLog.outDebug( "WORLD: got MSG_MOVE_WORLDPORT_ACK." );
	
	GetPlayer()->SendInitialLogonPackets();
	GetPlayer()->clearAttackers(true);
	GetPlayer()->m_TeleportState = 2;
	GetPlayer()->AddToWorld();
	GetPlayer()->SetPlayerStatus(NONE);
}

void WorldSession::HandleMoveTeleportAckOpcode( WorldPacket & recv_data )
{
	uint64 guid;
	recv_data >> guid;
	if(guid == _player->GetGUID())
	{
		if(sWorld.antihack_teleport && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && _player->GetPlayerStatus() != TRANSFER_PENDING)
		{
			/* we're obviously cheating */
			sCheatLog.writefromsession(this, "Used teleport hack, disconnecting.");
			Disconnect();
			return;
		}

		if(sWorld.antihack_teleport && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && _player->m_position.Distance2DSq(_player->m_sentTeleportPosition) > 625.0f)	/* 25.0f*25.0f */
		{
			/* cheating.... :( */
			sCheatLog.writefromsession(this, "Used teleport hack {2}, disconnecting.");
			Disconnect();
			return;
		}

		sLog.outDebug( "WORLD: got MSG_MOVE_TELEPORT_ACK." );
		GetPlayer()->SetPlayerStatus(NONE);
		GetPlayer()->clearAttackers(true);
		GetPlayer()->SetMovement(MOVE_UNROOT,5);
		_player->ResetHeartbeatCoords();

		if(GetPlayer()->GetSummon() != NULL)		// move pet too
			GetPlayer()->GetSummon()->SetPosition((GetPlayer()->GetPositionX() + 2), (GetPlayer()->GetPositionY() + 2), GetPlayer()->GetPositionZ(), M_PI);
		_player->m_sentTeleportPosition.ChangeCoords(999999.0f,999999.0f,999999.0f);
	}

}

void WorldSession::HandleMovementOpcodes( WorldPacket & recv_data )
{
	if(_player == 0)
		return;

	if(GetPlayer()->GetPlayerStatus() == TRANSFER_PENDING) //dont update coords
		return;
	if(!_player->IsInWorld() || _player->m_uint32Values[UNIT_FIELD_CHARMEDBY])
		return;

	// spell cancel on movement, for now only fishing is added
	Object * t_go = GetPlayer()->m_SummonedObject;
	if (t_go)
		if (t_go->GetEntry() == GO_FISHING_BOBBER)
			((GameObject*)t_go)->EndFishing(GetPlayer(),true);

	movement_info.init(recv_data);
	GetPlayer()->m_isMoving = true;

	// check for bad coords
	if( !((movement_info.y >= _minY) && (movement_info.y <= _maxY)) ||
		!((movement_info.x >= _minX) && (movement_info.x <= _maxX)) )
	{
		sLog.outError("%s might be cheating, bad coords specified in movement packet.", _player->GetName());
		return;
	}

	if(GetPlayer()->cannibalize)
	{
		sEventMgr.RemoveEvents(GetPlayer(), EVENT_CANNIBALIZE);
		GetPlayer()->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
		GetPlayer()->cannibalize = false;
	}

	//Send packet to other players
	if(recv_data.size() > 80)
	{
		Disconnect();
		return;
	}

	if(sWorld.antihack_teleport && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && _player->m_position.Distance2DSq(movement_info.x, movement_info.y) > 2500.0f
		&& _player->m_runSpeed < 50.0f && !(movement_info.flags & MOVEFLAG_TAXI))	/*50*50*/
	{
		sCheatLog.writefromsession(this, "Used teleport hack {3}, speed was %f", _player->m_runSpeed);
		Disconnect();
		return;
	}

	if(sWorld.antihack_flight && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && !_player->FlyCheat &&
		(movement_info.flags & MOVEFLAG_FLYING || movement_info.flags & MOVEFLAG_AIR_SWIMMING) &&
		!(movement_info.flags & MOVEFLAG_FALLING) && !(movement_info.flags & MOVEFLAG_TAXI) &&
		_player->_delayAntiFlyUntil < UNIXTIME)
	{
		sCheatLog.writefromsession(this, "Used flying hack {1}, movement flags: %u", movement_info.flags);
		Disconnect();
		return;
	}

	if(movement_info.flags & MOVEFLAG_FALLING_FAR && !movement_info.FallTime && sWorld.antihack_falldmg && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && !_player->bSafeFall && !_player->GodModeCheat)
	{
		sCheatLog.writefromsession(this, "Used fall damage hack, falltime is 0 and flags are %u", movement_info.flags);
		Disconnect();
		return;
	}

	uint32 pos = m_MoverWoWGuid.GetNewGuidLen() + 1;
	uint32 mstime = getMSTime();
	int32 new_move_time = (MOVEMENT_PACKET_TIME_DELAY + (movement_info.time - mstime))+mstime;
	memcpy(&movement_packet[pos], recv_data.contents(), recv_data.size());

	for(set<Player*>::iterator itr = _player->m_inRangePlayers.begin(); itr != _player->m_inRangePlayers.end(); ++itr)
	{
#ifdef USING_BIG_ENDIAN
		*(uint32*)&movement_packet[pos+4] = swap32(new_move_time+(*itr)->GetSession()->m_moveDelayTime);
#else
		*(uint32*)&movement_packet[pos+4] = new_move_time+(*itr)->GetSession()->m_moveDelayTime;
#endif
		(*itr)->GetSession()->OutPacket(recv_data.GetOpcode(), recv_data.size() + pos, movement_packet);
	}

	//Falling Handler
	if (movement_info.flags & 0x2000) // Falling
	{
		if( GetPlayer()->m_fallTime < movement_info.FallTime)
			GetPlayer()->m_fallTime = movement_info.FallTime;
		_player->ResetHeartbeatCoords();
	}
	else //once we done falling lets do some damage
	{
		if(GetPlayer()->m_fallTime > 1000 && GetPlayer()->isAlive() && !_player->GodModeCheat)
		{
			//Check if we aren't falling in water
			if(!_player->bSafeFall)
			{
				if( !(movement_info.flags & 0x200000) && !_player->blinked)
				{
					uint8 type = DAMAGE_FALL;
					//10% dmg per sec after first 3 seconds
					//it rL a*t*t
					double coeff = 0.000000075*(_player->m_fallTime*_player->m_fallTime - _player->m_fallTime);
					if (coeff<0)
						coeff=0;
					uint32 damage = (uint32)(_player->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*coeff);
					if(damage > GetPlayer()->GetUInt32Value(UNIT_FIELD_MAXHEALTH)) // Can only deal 100% damage.
						damage = GetPlayer()->GetUInt32Value(UNIT_FIELD_MAXHEALTH);

					WorldPacket data(13);
					data.SetOpcode(SMSG_ENVIRONMENTALDAMAGELOG);
					data << GetPlayer()->GetGUID();
					data << type;
					data << damage;
					SendPacket(&data);
					_player->DealDamage(GetPlayer(), damage, 0, 0, 0);
				}
			}
			GetPlayer()->m_fallTime = 0;
			_player->blinked = false;
			_player->ResetHeartbeatCoords();
		}
		else
		{
			//player is dead, no need to keep increasing falltime
			GetPlayer()->m_fallTime = 0;
			/*_player->ResetHeartbeatCoords();*/
			_player->blinked = false;
		}
	}

	//Setup Transporter Positioning
	if(movement_info.transGuid != 0 && !_player->m_lockTransportVariables)
	{
		if(!_player->m_TransporterGUID)
		{
			_player->m_CurrentTransporter = objmgr.GetTransporter(movement_info.transGuid);
			if(_player->m_CurrentTransporter)
			{
				_player->m_CurrentTransporter->AddPlayer(_player);
			}
		}

		GetPlayer()->m_TransporterX = movement_info.transX;
		GetPlayer()->m_TransporterY = movement_info.transY;
		GetPlayer()->m_TransporterZ = movement_info.transZ;
		GetPlayer()->m_TransporterO = movement_info.transO;
		GetPlayer()->m_TransporterUnk = movement_info.transUnk;
		GetPlayer()->m_TransporterGUID = movement_info.transGuid;
		
		//float x = movement_info.x - movement_info.transX;
		//float y = movement_info.y - movement_info.transY;
		//float z = movement_info.z - movement_info.transZ;
		/*Transporter* trans = _player->m_CurrentTransporter;
		if(trans) sChatHandler.SystemMessageToPlr(_player, "Client t pos: %f %f\nServer t pos: %f %f   Diff: %f %f", x,y, trans->GetPositionX(), trans->GetPositionY(), trans->CalcDistance(x,y,z), trans->CalcDistance(movement_info.x, movement_info.y, movement_info.z));*/
	}
	else
	{
		if(_player->m_TransporterGUID && !_player->m_lockTransportVariables)
		{
			// remove us from the porter
			GetPlayer()->m_TransporterGUID = 0;
			GetPlayer()->m_TransporterX = 0.0f;
			GetPlayer()->m_TransporterY = 0.0f;
			GetPlayer()->m_TransporterZ = 0.0f;
			GetPlayer()->m_TransporterO = 0.0f;

			if(_player->m_CurrentTransporter)
				_player->m_CurrentTransporter->RemovePlayer(_player);

			GetPlayer()->m_CurrentTransporter = NULL;
			_player->ResetHeartbeatCoords();
		}

		//// speedhack protection
		if(sWorld.SpeedhackProtection && !_player->blinked)
			_SpeedCheck(movement_info);
	}
	_HandleBreathing(recv_data, movement_info);
	_player->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_MOVEMENT);

	if( _player->m_CurrentCharm )
	{
		_player->m_CurrentCharm->SetPosition(movement_info.x, movement_info.y, movement_info.z, movement_info.orientation);
	}
	else
	{
		if(!_player->m_CurrentTransporter) 
		{
			if( !_player->SetPosition(movement_info.x, movement_info.y, movement_info.z, movement_info.orientation) )
			{
				GetPlayer()->SetUInt32Value(UNIT_FIELD_HEALTH, 0);
				GetPlayer()->KillPlayer();
			}
		}
		else
		{
			_player->SetPosition(_player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), 
				movement_info.orientation + movement_info.transO, false);
		}
	}	
}

void WorldSession::HandleMoveStopOpcode( WorldPacket & recv_data )
{	
	HandleMovementOpcodes( recv_data );
	_player->m_isMoving = false;
}

void WorldSession::HandleMoveTimeSkippedOpcode( WorldPacket & recv_data )
{
	uint64 guid;
	recv_data >> guid >> m_moveDelayTime;
	//Log.Debug("MoveTimeSkipped", "Client %s is out of sync by %u ms", GetSocket()->GetRemoteIP().c_str(), m_moveDelayTime);
}

void WorldSession::HandleMoveNotActiveMoverOpcode( WorldPacket & recv_data )
{
/*	uint64 guid;
	recv_data >> guid;

	MovementInfo mi(recv_data);
	WorldPacket data;
	data.SetOpcode(MSG_MOVE_TELEPORT);	  // meh..
	FastGUIDPack(data, guid);			   // faaast.

	mi >> data;
	_player->SendMessageToSet(&data, false);*/
}


void WorldSession::HandleSetActiveMoverOpcode( WorldPacket & recv_data )
{
	// set current movement object
	uint64 guid;
	recv_data >> guid;

	if(guid != m_MoverWoWGuid.GetOldGuid())
	{
		// generate wowguid
		if(guid != 0)
			m_MoverWoWGuid.Init(guid);
		else
			m_MoverWoWGuid.Init(_player->GetGUID());

		// set up to the movement packet
		movement_packet[0] = m_MoverWoWGuid.GetNewGuidMask();
		memcpy(&movement_packet[1], m_MoverWoWGuid.GetNewGuid(), m_MoverWoWGuid.GetNewGuidLen());
	}
}

void WorldSession::HandleMoveSplineCompleteOpcode(WorldPacket &recvPacket)
{

}

void WorldSession::HandleBasicMovementOpcodes( WorldPacket & recv_data )
{
	if(GetPlayer()->GetPlayerStatus() == TRANSFER_PENDING) //don't update coords
		return;
	if(!_player->IsInWorld() || _player->m_uint32Values[UNIT_FIELD_CHARMEDBY])
		return;

    movement_info.init(recv_data);
	// check for bad coords
	if( !((movement_info.y >= _minY) && (movement_info.y <= _maxY)) ||
		!((movement_info.x >= _minX) && (movement_info.x <= _maxX)) )
	{
		sLog.outError("%s might be cheating, bad coords specified in movement packet.", _player->GetName());
		return;
	}

	GetPlayer()->m_isMoving = true;

	//Send packet to other players
	if(recv_data.size() > 80)
	{
		Disconnect();
		return;
	}

	if(sWorld.antihack_teleport && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && _player->m_position.Distance2DSq(movement_info.x, movement_info.y) > 2500.0f
		&& _player->m_runSpeed < 50.0f && !(movement_info.flags & MOVEFLAG_TAXI))	/*50*50*/
	{
		sCheatLog.writefromsession(this, "Used teleport hack {3}, speed was %f", _player->m_runSpeed);
		Disconnect();
		return;
	}

	if(sWorld.antihack_flight && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && !_player->FlyCheat &&
		(movement_info.flags & MOVEFLAG_FLYING || movement_info.flags & MOVEFLAG_AIR_SWIMMING) &&
		!(movement_info.flags & MOVEFLAG_FALLING) && !(movement_info.flags & MOVEFLAG_TAXI) &&
		_player->_delayAntiFlyUntil < UNIXTIME)
	{
		sCheatLog.writefromsession(this, "Used flying hack {1}, movement flags: %u", movement_info.flags);
		Disconnect();
		return;
	}

	if(movement_info.flags & MOVEFLAG_FALLING_FAR && !movement_info.FallTime && sWorld.antihack_falldmg && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && !_player->bSafeFall && !_player->GodModeCheat)
	{
		sCheatLog.writefromsession(this, "Used fall damage hack, falltime is 0 and flags are %u", movement_info.flags);
		Disconnect();
		return;
	}

	uint32 pos = m_MoverWoWGuid.GetNewGuidLen() + 1;
	uint32 mstime = getMSTime();
	int32 new_move_time = (MOVEMENT_PACKET_TIME_DELAY + (movement_info.time - mstime))+mstime;
	memcpy(&movement_packet[pos], recv_data.contents(), recv_data.size());

	for(set<Player*>::iterator itr = _player->m_inRangePlayers.begin(); itr != _player->m_inRangePlayers.end(); ++itr)
	{
#ifdef USING_BIG_ENDIAN
		*(uint32*)&movement_packet[pos+4] = swap32(new_move_time+(*itr)->GetSession()->m_moveDelayTime);
#else
		*(uint32*)&movement_packet[pos+4] = new_move_time+(*itr)->GetSession()->m_moveDelayTime;
#endif
		(*itr)->GetSession()->OutPacket(recv_data.GetOpcode(), recv_data.size() + pos, movement_packet);
	}

	//Setup Transporter Positioning
	if(movement_info.transGuid != 0 && !_player->m_lockTransportVariables)
	{
		if(!_player->m_TransporterGUID)
		{
			_player->m_CurrentTransporter = objmgr.GetTransporter(movement_info.transGuid);
			if(_player->m_CurrentTransporter)
			{
				_player->m_CurrentTransporter->AddPlayer(_player);
			}
		}

		GetPlayer()->m_TransporterX = movement_info.transX;
		GetPlayer()->m_TransporterY = movement_info.transY;
		GetPlayer()->m_TransporterZ = movement_info.transZ;
		GetPlayer()->m_TransporterO = movement_info.transO;
		GetPlayer()->m_TransporterUnk = movement_info.transUnk;
		GetPlayer()->m_TransporterGUID = movement_info.transGuid;

//		float x = movement_info.x - movement_info.transX;
 //	   float y = movement_info.y - movement_info.transY;
  //	  float z = movement_info.z - movement_info.transZ;
		/*Transporter* trans = _player->m_CurrentTransporter;
		if(trans) sChatHandler.SystemMessageToPlr(_player, "Client t pos: %f %f\nServer t pos: %f %f   Diff: %f %f", x,y, trans->GetPositionX(), trans->GetPositionY(), trans->CalcDistance(x,y,z), trans->CalcDistance(movement_info.x, movement_info.y, movement_info.z));*/
	}
	else
	{
		if(_player->m_TransporterGUID && !_player->m_lockTransportVariables)
		{
			// remove us from the porter
			GetPlayer()->m_TransporterGUID = 0;
			GetPlayer()->m_TransporterX = 0.0f;
			GetPlayer()->m_TransporterY = 0.0f;
			GetPlayer()->m_TransporterZ = 0.0f;
			GetPlayer()->m_TransporterO = 0.0f;

			if(_player->m_CurrentTransporter)
				_player->m_CurrentTransporter->RemovePlayer(_player);
			
			GetPlayer()->m_CurrentTransporter = NULL;
			_player->ResetHeartbeatCoords();
		}

		// speedhack protection
		if(sWorld.SpeedhackProtection && !_player->blinked)
			_SpeedCheck(movement_info);
	}
	_HandleBreathing(recv_data, movement_info);

	if( _player->m_CurrentCharm )
	{
		_player->m_CurrentCharm->SetPosition(movement_info.x, movement_info.y, movement_info.z, movement_info.orientation);
	}
	else
	{
		if( !_player->m_CurrentTransporter)
		{
			if(!GetPlayer( )->SetPosition(movement_info.x, movement_info.y, movement_info.z, movement_info.orientation) )
			{
				GetPlayer()->SetUInt32Value(UNIT_FIELD_HEALTH, 0);
				GetPlayer()->KillPlayer();
			}
		}
		else
		{
			_player->SetPosition(_player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), 
				movement_info.orientation + movement_info.transO, false);
		}
	}
}

void WorldSession::_HandleBreathing(WorldPacket &recv_data, MovementInfo &mi)
{
    //player swiming.

    if(movement_info.flags & 0x200000)
    {
		if(_player->m_MountSpellId)
			_player->RemoveAura(_player->m_MountSpellId);

		if(_player->FlyCheat)
		{
			if(_player->m_lastMoveType != 2)
			{
				_player->m_lastMoveType = 2;		// flying
				_player->ResetHeartbeatCoords();
			}
		}
		else
		{
			if(_player->m_lastMoveType != 1)
			{
				_player->m_lastMoveType = 1;		// swimming
				_player->ResetHeartbeatCoords();
			}
		}

        // get water level only if it was not set before
		if (!m_bIsWLevelSet)
		{
			// water level is somewhere below the nose of the character when entering water
			m_wLevel = movement_info.z + _player->m_noseLevel*0.95;
			m_bIsWLevelSet = true;
		}
		if(!(_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING))
			_player->m_UnderwaterState |= UNDERWATERSTATE_SWIMMING;
    }
	else
	{
		if(movement_info.flags & MOVEFLAG_AIR_SWIMMING)
		{
			if(_player->FlyCheat)
			{
				if(_player->m_lastMoveType != 2)
				{
					_player->m_lastMoveType = 2;		// flying
					_player->ResetHeartbeatCoords();
				}
			}
		}
		else
		{
			if(_player->m_lastMoveType)
			{
				_player->m_lastMoveType=0;
				_player->ResetHeartbeatCoords();
			}
		}
	}

    if(movement_info.flags & 0x2000 && _player->m_UnderwaterState)
    {
        //player jumped inside water but still underwater.
        if(m_bIsWLevelSet && (movement_info.z + _player->m_noseLevel) < m_wLevel)
        {
            return;
        }
        else
        {
            if(!sWorld.BreathingEnabled || _player->FlyCheat || _player->m_bUnlimitedBreath || !_player->isAlive() || _player->GodModeCheat)
            {
            }
            else
            {
                //only swiming and can breath, stop bar
                if(_player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER)
		        {
			        WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			        data << uint32(1) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << uint32(10) << uint32(0);
			        SendPacket(&data);

			        _player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
		        }
            }
        }
        return;
    }
    //player not swiming
    if(!(movement_info.flags & 0x200000) && _player->m_UnderwaterState)
    {
        if(_player->m_lastMoveType)
		{
			_player->m_lastMoveType = 0;
			_player->ResetHeartbeatCoords();
		}

		if(_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING)
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_SWIMMING;
			_player->RemoveAura(1066);//remove aquatic form on land
			m_bIsWLevelSet = false;
		}

		if(_player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER)
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;

		return;

    }
    if(m_bIsWLevelSet && (movement_info.z + _player->m_noseLevel) < m_wLevel)
	{
		if(!(_player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER))
		{
			// we only just entered the water
			_player->m_UnderwaterState |= UNDERWATERSTATE_UNDERWATER;

            if(!sWorld.BreathingEnabled || _player->FlyCheat || _player->m_bUnlimitedBreath || !_player->isAlive() || _player->GodModeCheat)
            {
            }
            else
            {
			    // send packet
			    WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			    data << uint32(1) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(-1) << uint32(0);
			    SendPacket(&data);
            }
		}
	}
    else
    {
        if(_player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER)
		{
            if(!sWorld.BreathingEnabled || _player->FlyCheat || _player->m_bUnlimitedBreath || !_player->isAlive() || _player->GodModeCheat)
            {
            }
            else
            {
			    WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			    data << uint32(1) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << uint32(10) << uint32(0);
			    SendPacket(&data);
            }
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
		}
    }
}

void WorldSession::_SpeedCheck(MovementInfo &mi)
{
	// beat!

	// calculate distance between last heartbeat and this
	if(_player->_lastHeartbeatTime && _player->_heartBeatDisabledUntil < UNIXTIME)
	{
		int32 time_diff = movement_info.time - _player->_lastHeartbeatTime;
		float distance_travelled = _player->m_position.Distance2D(_player->_lastHeartbeatX, _player->_lastHeartbeatY);

		// do our check calculation
		float speed = _player->m_runSpeed;

		// underwater or flying
		switch(_player->m_lastMoveType)
		{
		case 1:		// swimming
			speed = _player->m_swimSpeed;
			break;
		case 2:		// flying
			speed = _player->m_flySpeed;
			break;
		}

		uint32 move_time = (uint32)((float)distance_travelled / (float)(speed*0.001f));

		// check if we're in the correct bounds
		if(move_time > time_diff)
		{
			int32 difference = move_time - time_diff;
			if(difference > 350)	// say this for now
			{
				if(_player->m_speedhackChances)
				{
					sChatHandler.SystemMessage(this, "Speedhack detected. This has been logged for later processing by the server admins. If you are caught again, you will be kicked from the server. You will be unrooted in 5 seconds.");
					_player->SetMovement(MOVE_ROOT, 1);
					sEventMgr.AddEvent(_player, &Player::SetMovement, uint8(MOVE_UNROOT), uint32(1), EVENT_DELETE_TIMER, 5000, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
					_player->ResetHeartbeatCoords();
					_player->m_speedhackChances--;
					
					// TODO: replace with server plugin system later on
					sCheatLog.writefromsession(this, "Speedhack warning, time diff of %u", difference);
				}
				else if(_player->m_speedhackChances == 0)
				{
					sChatHandler.SystemMessage(this, "You will now be removed from the server for speed hacking. Your account has been flagged for further investigation by the admins.");

					// TODO: replace with server plugin system later on
					sCheatLog.writefromsession(this, "Kicked for speedhack, time diff of %u", difference);

					_player->m_KickDelay = 0;
					sEventMgr.AddEvent(_player, &Player::_Kick, EVENT_PLAYER_KICK, 10000, 1,0);

					// Root movement :p heheheh evil
					_player->SetMovement(MOVE_ROOT, 1);
				}
			}

			//printf("Move shit: %ums\n", abs(difference));
			sChatHandler.SystemMessage(this, "Move time : %d / %d, diff: %d", move_time, time_diff, difference);
		}
	}
	_player->_lastHeartbeatTime = movement_info.time;
	_player->_lastHeartbeatX = movement_info.x;
	_player->_lastHeartbeatY = movement_info.y;
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket &recvdata)
{
	WorldPacket data(SMSG_MOUNTSPECIAL_ANIM,8);
	data << _player->GetGUID();
	_player->SendMessageToSet(&data, true);
}

void WorldSession::HandleWorldportOpcode(WorldPacket & recv_data)
{
	uint32 mapid;
	float x,y,z,o;
	recv_data >> mapid >> x >> y >> z >> o;
	
	if(!_player->IsInWorld())
		return;

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	LocationVector vec(x,y,z,o);
	_player->SafeTeleport(mapid,0,vec);
}

void WorldSession::HandleTeleportToUnitOpcode(WorldPacket & recv_data)
{
	uint8 unk;
	Unit * target;
	recv_data >> unk;

	if(!_player->IsInWorld())
		return;

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	if( (target = _player->GetMapMgr()->GetUnit(_player->GetSelection())) == NULL )
		return;

	_player->SafeTeleport(_player->GetMapId(), _player->GetInstanceID(), target->GetPosition());
}

void WorldSession::HandleTeleportCheatOpcode(WorldPacket & recv_data)
{
	float x,y,z,o;
	LocationVector vec;

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	recv_data >> x >> y >> z >> o;
	vec.ChangeCoords(x,y,z,o);
	_player->SafeTeleport(_player->GetMapId(),_player->GetInstanceID(),vec);
}
