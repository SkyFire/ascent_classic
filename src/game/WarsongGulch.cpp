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

WarsongGulch::WarsongGulch(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t) : CBattleground(mgr, id, lgroup, t)
{
	m_flagHolders[0] = m_flagHolders[1] = 0;
	
	/* create the buffs */
	for(int i = 0; i < 6; ++i)
		SpawnBuff(i);

	/* take note: these are swapped around for performance bonus */
	// warsong flag - horde base
	m_homeFlags[0] = SpawnGameObject(179831, 489, 915.367, 1433.78, 346.089, 3.17301, 0, 210, 2.5f);
	m_homeFlags[0]->SetUInt32Value(GAMEOBJECT_STATE, 1);
	m_homeFlags[0]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 24);
	m_homeFlags[0]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);

	// silverwing flag - alliance base
	m_homeFlags[1] = SpawnGameObject(179830, 489, 1540.29, 1481.34, 352.64, 3.17301, 0,1314, 2.5f);
	m_homeFlags[1]->SetUInt32Value(GAMEOBJECT_STATE, 1);
	m_homeFlags[1]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 24);
	m_homeFlags[1]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);

	// dropped flags
	m_dropFlags[1] = m_mapMgr->CreateGameObject();
	if(!m_dropFlags[1]->CreateFromProto(179785, 489, 0, 0, 0, 0))
		Log.Warning("WarsongGulch", "Could not create dropped flag 1");

	m_dropFlags[0] = m_mapMgr->CreateGameObject();
	if(!m_dropFlags[0]->CreateFromProto(179786, 489, 0, 0, 0, 0))
		Log.Warning("WarsongGulch", "Could not create dropped flag 0");

	for(int i = 0; i < 2; ++i)
	{
		m_dropFlags[i]->SetUInt32Value(GAMEOBJECT_DYN_FLAGS, 1);
		m_dropFlags[i]->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.5f);
	}

	m_scores[0] = m_scores[1] = 0;

	m_playerCountPerTeam = 10;
}

WarsongGulch::~WarsongGulch()
{

}

void WarsongGulch::HookOnAreaTrigger(Player * plr, uint32 id)
{
	int32 buffslot = -1;
	switch(id)
	{
	case 3686:	  // Speed
		buffslot = 0;
		break;
	case 3687:	  // Speed (Horde)
		buffslot = 1;
		break;
	case 3706:	  // Restoration
		buffslot = 2;
		break;
	case 3708:	  // Restoration (Horde)
		buffslot = 3;
		break;
	case 3707:	  // Berserking
		buffslot = 4;
		break;
	case 3709:	  // Berserking (Horde)
		buffslot = 5;
		break;
	}

	if(buffslot > 0)
	{
		if(m_buffs[buffslot] != 0 && m_buffs[buffslot]->IsInWorld())
		{
			/* apply the buff */
			SpellEntry * sp = sSpellStore.LookupEntry(m_buffs[buffslot]->GetInfo()->sound3);
			Spell * s = new Spell(plr, sp, true, 0);
			SpellCastTargets targets(plr->GetGUID());
			s->prepare(&targets);

			/* despawn the gameobject (not delete!) */
			m_buffs[buffslot]->Despawn(BUFF_RESPAWN_TIME);
		}
		return;
	}

	if(((id == 3646 && plr->GetTeam() == 0) || (id == 3647 && plr->GetTeam() == 1)) && (plr->m_bgHasFlag && m_flagHolders[plr->GetTeam()] == plr->GetGUIDLow()))
	{
		/* remove the bool from the player so the flag doesn't drop */
		m_flagHolders[plr->GetTeam()] = 0;
		plr->m_bgHasFlag = 0;

		/* remove flag aura from player */
		plr->RemoveAura(23333+(plr->GetTeam() * 2));

		/* capture flag points */
		plr->m_bgScore.Misc1++;

		PlaySoundToAll(plr->GetTeam() ? SOUND_HORDE_SCORES : SOUND_ALLIANCE_SCORES);
		SendChatMessage(CHAT_MSG_BATTLEGROUND_EVENT, plr->GetGUID(), "%s captured the %s flag!", plr->GetName(), plr->GetTeam() ? "Alliance" : "Horde");
		SetWorldState(plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 1);

		/* respawn the home flag */
		m_homeFlags[plr->GetTeam()]->PushToWorld(m_mapMgr);

		/* give each player on that team a bonus 82 honor - burlex: is this correct amount? */
		for(set<Player*>::iterator itr = m_players[plr->GetTeam()].begin(); itr != m_players[plr->GetTeam()].end(); ++itr)
		{
			plr->m_bgScore.BonusHonor += 82;
			HonorHandler::AddHonorPointsToPlayer(plr, 82);
		}

		m_scores[plr->GetTeam()]++;
		if(m_scores[plr->GetTeam()] == 3)
		{
			/* victory! */
			m_ended = true;
			m_winningteam = plr->GetTeam() ? 0 : 1;
			m_nextPvPUpdateTime = 0;

			sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_CLOSE);
			sEventMgr.AddEvent(((CBattleground*)this), &CBattleground::Close, EVENT_BATTLEGROUND_CLOSE, 120000, 1,0);

			/* add the marks of honor to all players */
            SpellEntry * a_sp = sSpellStore.LookupEntry((plr->GetTeam()) ? 24950 : 24951);
			SpellEntry * h_sp = sSpellStore.LookupEntry((plr->GetTeam()) ? 24951 : 24950);
			for(uint32 i = 0; i < 2; ++i) {
				for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) {
					plr->CastSpell(plr, i ? h_sp : a_sp, true);
				}
			}
		}

		/* increment the score world state */
		SetWorldState(plr->GetTeam() ? WSG_CURRENT_HORDE_SCORE : WSG_CURRENT_ALLIANCE_SCORE, m_scores[plr->GetTeam()]);

		UpdatePvPData();
	}

}

void WarsongGulch::DropFlag(Player * plr)
{
	if(!plr->m_bgHasFlag || m_dropFlags[plr->GetTeam()]->IsInWorld())
		return;

	/* drop the flag! */
	m_dropFlags[plr->GetTeam()]->SetPosition(plr->GetPosition());
	m_dropFlags[plr->GetTeam()]->SetFloatValue(GAMEOBJECT_POS_X, plr->GetPositionX());
	m_dropFlags[plr->GetTeam()]->SetFloatValue(GAMEOBJECT_POS_Y, plr->GetPositionY());
	m_dropFlags[plr->GetTeam()]->SetFloatValue(GAMEOBJECT_POS_Z, plr->GetPositionZ());
	m_dropFlags[plr->GetTeam()]->SetFloatValue(GAMEOBJECT_FACING, plr->GetOrientation());
	m_dropFlags[plr->GetTeam()]->PushToWorld(m_mapMgr);
	m_flagHolders[plr->GetTeam()] = 0;
	SetWorldState(plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 1);
	plr->m_bgHasFlag = false;

	SendChatMessage(CHAT_MSG_BATTLEGROUND_EVENT, plr->GetGUID(), "The %s flag was dropped by %s!", plr->GetTeam() ? "Alliance" : "Horde", plr->GetName());
}

void WarsongGulch::HookFlagDrop(Player * plr, GameObject * obj)
{
	/* picking up a dropped flag */
	if(m_dropFlags[plr->GetTeam()] != obj)
	{
		/* are we returning it? */
		if( (obj->GetEntry() == 179785 && plr->GetTeam() == 0) ||
			(obj->GetEntry() == 179786 && plr->GetTeam() == 1) )
		{
			uint32 x = plr->GetTeam() ? 0 : 1;
			m_dropFlags[x]->RemoveFromWorld();
			if(m_homeFlags[x]->IsInWorld() == false)
				m_homeFlags[x]->PushToWorld(m_mapMgr);

			plr->m_bgScore.Misc2++;
			UpdatePvPData();

			SendChatMessage(CHAT_MSG_BATTLEGROUND_EVENT, plr->GetGUID(), "The %s flag was returned to its base by %s!", plr->GetTeam() ? "Horde" : "Alliance", plr->GetName());
			SetWorldState(plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 1);
			PlaySoundToAll(plr->GetTeam() ? SOUND_HORDE_RETURNED : SOUND_ALLIANCE_RETURNED);
		}
		return;
	}

	m_dropFlags[plr->GetTeam()]->RemoveFromWorld();
	m_flagHolders[plr->GetTeam()] = plr->GetGUIDLow();
	plr->m_bgHasFlag = true;

	/* This is *really* strange. Even though the A9 create sent to the client is exactly the same as the first one, if
	 * you spawn and despawn it, then spawn it again it will not show. So we'll assign it a new guid, hopefully that
	 * will work.
	 * - Burlex
	 */
	m_dropFlags[plr->GetTeam()]->SetNewGuid(m_mapMgr->GenerateGameobjectGuid());
	
	SpellEntry * pSp = sSpellStore.LookupEntry(23333 + (plr->GetTeam() * 2));
	Spell * sp = new Spell(plr, pSp, true, 0);
	SpellCastTargets targets(plr->GetGUID());
	sp->prepare(&targets);
	SetWorldState(plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 2);
}

void WarsongGulch::HookFlagStand(Player * plr, GameObject * obj)
{
	if(m_flagHolders[plr->GetTeam()] || m_homeFlags[plr->GetTeam()] != obj)
	{
		// cheater!
		return;
	}

	SpellEntry * pSp = sSpellStore.LookupEntry(23333 + (plr->GetTeam() * 2));
	Spell * sp = new Spell(plr, pSp, true, 0);
	SpellCastTargets targets(plr->GetGUID());
	sp->prepare(&targets);

	/* set the flag holder */
	m_flagHolders[plr->GetTeam()] = plr->GetGUIDLow();
	m_homeFlags[plr->GetTeam()]->RemoveFromWorld();
	plr->m_bgHasFlag = true;

	SendChatMessage(CHAT_MSG_BATTLEGROUND_EVENT, 0, "The %s flag was picked up by %s!", plr->GetTeam() ? "Alliance" : "Horde", plr->GetName());
	PlaySoundToAll(plr->GetTeam() ? SOUND_HORDE_CAPTURE : SOUND_ALLIANCE_CAPTURE);
	SetWorldState(plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 2);
}

void WarsongGulch::HookOnPlayerKill(Player * plr, Unit * pVictim)
{
	if(pVictim->IsPlayer())
	{
		plr->m_bgScore.KillingBlows++;
		UpdatePvPData();
	}
}

void WarsongGulch::HookOnHK(Player * plr)
{
	plr->m_bgScore.HonorableKills++;
	UpdatePvPData();
}

void WarsongGulch::OnAddPlayer(Player * plr)
{
	/* do we actually need to do anything special to the player? */
}

void WarsongGulch::OnRemovePlayer(Player * plr)
{
	/* drop the flag if we have it */
	if(plr->m_bgHasFlag)
		HookOnMount(plr);
}

LocationVector WarsongGulch::GetStartingCoords(uint32 Team)
{
	if(Team)		// Horde
		return LocationVector(933.989685f, 1430.735840f, 345.537140f, 3.141593);
	else			// Alliance
		return LocationVector(1519.530273f, 1481.868408f, 352.023743f, 3.141593f);
}

void WarsongGulch::HookOnPlayerDeath(Player * plr)
{
	plr->m_bgScore.Deaths++;

	/* do we have the flag? */
	if(plr->m_bgHasFlag)
		plr->RemoveAura( 23333 + (plr->GetTeam() * 2) );

	UpdatePvPData();
}

void WarsongGulch::HookOnMount(Player * plr)
{
	/* do we have the flag? */
	if(plr->m_bgHasFlag)
		plr->RemoveAura( 23333 + (plr->GetTeam() * 2) );
}

bool WarsongGulch::HookHandleRepop(Player * plr)
{
    LocationVector dest;
	if(plr->GetTeam())
		dest.ChangeCoords(1032.644775f, 1388.316040f, 340.559937f, 0.043200f);
	else
		dest.ChangeCoords(1423.218872f, 1554.663574f, 342.833801f, 3.124139f);
	plr->SafeTeleport(plr->GetMapId(), plr->GetInstanceID(), dest);
	return true;
}

void WarsongGulch::SpawnBuff(uint32 x)
{
    switch(x)
	{
	case 0:
		m_buffs[x] = SpawnGameObject(179871, 489, 1449.9296875, 1470.70971679688, 342.634552001953, -1.64060950279236, 0, 114, 1);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.73135370016098);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.681998312473297);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_STATE, 1);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
		break;
	case 1:
		m_buffs[x] = SpawnGameObject(179899, 489, 1005.17071533203, 1447.94567871094, 335.903228759766, 1.64060950279236, 0, 114, 1);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.73135370016098);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_03,0.681998372077942);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_STATE, 1);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
		break;
	case 2:
		m_buffs[x] = SpawnGameObject(179904, 489, 1317.50573730469, 1550.85070800781, 313.234375, -0.26179963350296, 0, 114, 1);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.130526319146156);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.991444826126099);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_STATE, 1);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
		break;
	case 3:
		m_buffs[x] = SpawnGameObject(179906, 489, 1110.45129394531, 1353.65563964844, 316.518096923828, -0.68067866563797, 0, 114, 1);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.333806991577148);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.94264143705368);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_STATE, 1);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
		break;
	case 4:
		m_buffs[x] = SpawnGameObject(179905, 489, 1320.09375, 1378.78967285156, 314.753234863281, 1.18682384490967, 0, 114, 1);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.559192895889282);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_03,0.829037606716156);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_STATE, 1);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
		break;
	case 5:
		m_buffs[x] = SpawnGameObject(179907, 489, 1139.68774414063, 1560.28771972656, 306.843170166016, -2.4434609413147, 0, 114, 1);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.939692616462708);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.342020124197006);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_STATE, 1);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
		break;
	}
}

void WarsongGulch::OnCreate()
{
	/* add the buffs to the world */
	for(int i = 0; i < 6; ++i)
		m_buffs[i]->PushToWorld(m_mapMgr);

	// Alliance Gates
	GameObject *gate = SpawnGameObject(179921, 489, 1471.554688, 1458.778076, 362.633240, 0, 33, 114, 2.33271);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	gate = SpawnGameObject(179919, 489, 1492.477783, 1457.912354, 342.968933, 0, 33, 114, 2.68149);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	gate = SpawnGameObject(179918, 489, 1503.335327, 1493.465820, 352.188843, 0, 33, 114, 2.26);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	// Horde Gates
	gate = SpawnGameObject(179916, 489, 949.1663208, 1423.7717285, 345.6241455, -0.5756807, 32, 114, 0.900901);
	gate->SetFloatValue(GAMEOBJECT_ROTATION,-0.0167336);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_01,-0.004956);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_02,-0.283972);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_03,0.9586736);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	gate = SpawnGameObject(179917, 489, 953.0507202, 1459.8424072, 340.6525573, -1.9966197, 32, 114, 0.854700);   
	gate->SetFloatValue(GAMEOBJECT_ROTATION,-0.1971825);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_01,0.1575096);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_02,-0.8239487);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_03,0.5073640);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	/* set world states */
	SetWorldState(0x8D8, 0);
	SetWorldState(0x8D7, 0);
	SetWorldState(0x8D6, 0);
	SetWorldState(0x8D5, 0);
	SetWorldState(0x8D4, 0);
	SetWorldState(0x8D3, 0);
	SetWorldState(0x60B, 0);
	SetWorldState(0x60A, 0);
	SetWorldState(0x609, 0);
	SetWorldState(WSG_ALLIANCE_FLAG_CAPTURED, 1);
	SetWorldState(WSG_HORDE_FLAG_CAPTURED, 1);
	SetWorldState(WSG_MAX_SCORE, 3);
	SetWorldState(WSG_CURRENT_ALLIANCE_SCORE, 0);
	SetWorldState(WSG_CURRENT_HORDE_SCORE, 0);

	/* spawn spirit guides */
	// Alliance Spirit Guide
	SpawnSpiritGuide(1423.218872f, 1554.663574f, 342.833801f, 3.124139f, 0);

	// Horde Spirit Guide
	SpawnSpiritGuide(1032.644775f, 1388.316040f, 340.559937f, 0.043200f, 1);
}

void WarsongGulch::OnStart()
{
	/* open the gates */
	for(list<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
	{
		(*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
		(*itr)->SetUInt32Value(GAMEOBJECT_STATE, 0);
	}

	/* add the flags to the world */
	for(int i = 0; i < 2; ++i)
		m_homeFlags[i]->PushToWorld(m_mapMgr);

	SendChatMessage(CHAT_MSG_BATTLEGROUND_EVENT, 0, "The flags are now placed at their bases.");

	/* correct? - burlex */
	PlaySoundToAll(SOUND_BATTLEGROUND_BEGIN);
}