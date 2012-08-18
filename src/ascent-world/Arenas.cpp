/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2008 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

#define ARENA_PREPARATION 32727
#define GOLD_TEAM 0
#define GREEN_TEAM 1

Arena::Arena(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t, uint32 players_per_side) : CBattleground(mgr, id, lgroup, t)
{
	m_started = false;
	m_playerCountPerTeam = players_per_side;
	switch(t)
	{
	case BATTLEGROUND_ARENA_5V5:
		m_arenateamtype=2;
		break;

	case BATTLEGROUND_ARENA_3V3:
		m_arenateamtype=1;
		break;
		
	case BATTLEGROUND_ARENA_2V2:
		m_arenateamtype=0;
		break;

	default:
		m_arenateamtype=0;
		break;
	}
	rated_match=false;
	m_ratedTeams[0] = m_ratedTeams[1] = 0;
}

Arena::~Arena()
{

}

void Arena::OnAddPlayer(Player * plr)
{
	if( !m_started )
	{
		/* cast arena readyness buff */
		if(plr->isDead())
			plr->ResurrectPlayer(NULL);

		plr->SetUInt32Value(UNIT_FIELD_HEALTH, plr->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
		plr->SetUInt32Value(UNIT_FIELD_POWER1, plr->GetUInt32Value(UNIT_FIELD_MAXPOWER1));
		plr->SetUInt32Value(UNIT_FIELD_POWER4, plr->GetUInt32Value(UNIT_FIELD_MAXPOWER4));
		sEventMgr.AddEvent(plr, &Player::FullHPMP, EVENT_PLAYER_UPDATE, 500, 1, 0);
	}

	plr->m_deathVision = true;

	// remove all buffs (exclude talents, include flasks)
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(plr->m_auras[x])
		{
			if(plr->m_auras[x] && !plr->m_auras[x]->GetSpellProto()->DurationIndex && plr->m_auras[x]->GetSpellProto()->Flags4 & CAN_PERSIST_AND_CASTED_WHILE_DEAD)
				continue;
			else
			{
				plr->m_auras[x]->Remove();
			}
		}
	}
	plr->GetItemInterface()->RemoveAllConjured();
	plr->ResetAllCooldowns();

	if( !m_started )
		plr->CastSpell(plr, ARENA_PREPARATION, true);

	UpdatePlayerCounts();

	/* Add the green/gold team flag */
	Aura * aura = new Aura(dbcSpell.LookupEntry(32724+plr->m_bgTeam), -1, plr, plr);
	plr->AddAura(aura, NULL);
	
	/* Set FFA PvP Flag */
	if(!plr->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_FREE_FOR_ALL_PVP))
		plr->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_FREE_FOR_ALL_PVP);
}

void Arena::OnRemovePlayer(Player * plr)
{
	/* remove arena readyness buff */
	plr->m_deathVision = false;
	plr->RemoveAura(ARENA_PREPARATION);
	UpdatePlayerCounts();
	
	plr->RemoveAura(32724+plr->m_bgTeam);
	if(plr->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_FREE_FOR_ALL_PVP))
		plr->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_FREE_FOR_ALL_PVP);
}

void Arena::HookOnPlayerKill(Player * plr, Unit * pVictim)
{
	if( !pVictim->IsPlayer() )
		return;

	plr->m_bgScore.KillingBlows++;
	UpdatePlayerCounts();
}

void Arena::HookOnHK(Player * plr)
{
	plr->m_bgScore.HonorableKills++;
}

void Arena::HookOnPlayerDeath(Player * plr)
{
	
}

void Arena::OnCreate()
{
	GameObject * obj;
	WorldStateManager &sm = m_mapMgr->GetStateManager();

	switch(m_mapMgr->GetMapId())
	{
		/* loraedeon */
	case 572: {
		obj = SpawnGameObject(185917, 1278.647705f, 1730.556641f, 31.605574f, 1.684245f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.746058f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.665881f);
		m_gates.insert(obj);

		obj = SpawnGameObject(185918, 1293.560791f, 1601.937988f, 31.605574f, -1.457349f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, -0.665881f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.746058f);
		m_gates.insert(obj);

		m_pcWorldStates[GREEN_TEAM] = WORLDSTATE_ARENA_LORDAERON_GREEN_PLAYER_COUNT;
		m_pcWorldStates[GOLD_TEAM] = WORLDSTATE_ARENA_LORDAERON_GOLD_PLAYER_COUNT;
		sm.CreateWorldState(WORLDSTATE_ARENA_LORDAERON_SCORE_SHOW, 1);
		
	  }break;

		/* blades edge arena */
	case 562: {
		obj = SpawnGameObject(183972, 6177.707520f, 227.348145f, 3.604374f, -2.260201f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.90445f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, -0.426569f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183973, 6189.546387f, 241.709854f, 3.101481f, 0.881392f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.426569f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.904455f);
		m_gates.insert(obj);

		obj = SpawnGameObject(183970, 6299.115723f, 296.549438f, 3.308032f, 0.881392f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.426569f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.904455f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183971, 6287.276855f, 282.187714f, 3.810925f, -2.260201f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.904455f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, -0.426569f);
		m_gates.insert(obj);

		m_pcWorldStates[GREEN_TEAM] = WORLDSTATE_ARENA_BLADESEDGE_GREEN_PLAYER_COUNT;
		m_pcWorldStates[GOLD_TEAM] = WORLDSTATE_ARENA_BLADESEDGE_GOLD_PLAYER_COUNT;
		sm.CreateWorldState(WORLDSTATE_ARENA_BLADESEDGE_SCORE_SHOW, 1);

			  }break;

		/* nagrand arena */
	case 559: {
		obj = SpawnGameObject(183979, 4090.064453f, 2858.437744f, 10.236313f, 0.492805f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.243916f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.969796f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183980, 4081.178955f, 2874.970459f, 12.391714f, 0.492805f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.243916f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.969796f);
		m_gates.insert(obj);

		obj = SpawnGameObject(183977, 4023.709473f, 2981.776611f, 10.701169f, -2.648788f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.969796f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, -0.243916f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183978, 4031.854248f, 2966.833496f, 12.646200f, -2.648788f, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.969796f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, -0.243916f);
		m_gates.insert(obj);

		m_pcWorldStates[GREEN_TEAM] = WORLDSTATE_ARENA_NAGRAND_GREEN_PLAYER_COUNT;
		m_pcWorldStates[GOLD_TEAM] = WORLDSTATE_ARENA_NAGRAND_GOLD_PLAYER_COUNT;
		sm.CreateWorldState(WORLDSTATE_ARENA_NAGRAND_SCORE_SHOW, 1);

			  }break;
	}

	/* push gates into world */
	for(set<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
		(*itr)->PushToWorld(m_mapMgr);

	
	// known world states
	sm.CreateWorldState(m_pcWorldStates[GREEN_TEAM], 0);
	sm.CreateWorldState(m_pcWorldStates[GOLD_TEAM], 0);

	// unknown world states
	sm.CreateWorldState(0x08D4	,0x0000);
	sm.CreateWorldState(0x08D8	,0x0000);
	sm.CreateWorldState(0x08D7	,0x0000);
	sm.CreateWorldState(0x08D6	,0x0000);
	sm.CreateWorldState(0x08D5	,0x0000);
	sm.CreateWorldState(0x08D3	,0x0000);
	sm.CreateWorldState(0x0C0D	,0x017B);
}

void Arena::OnStart()
{
	/* remove arena readyness buff */
	for(uint32 i = 0; i < 2; ++i) {
		for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) {
			(*itr)->RemoveAura(ARENA_PREPARATION);
		}
	}

	/* open gates */
	for(set<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
	{
		(*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
		(*itr)->SetUInt32Value(GAMEOBJECT_STATE, 0);
	}

	m_started = true;

	/* Incase all players left */
	UpdatePlayerCounts();

	// soundz!
	PlaySoundToAll(SOUND_BATTLEGROUND_BEGIN);
}

void Arena::UpdatePlayerCounts()
{
	if(m_ended)
		return;

	uint32 players[2] = {0,0};
	for(uint32 i = 0; i < 2; ++i) {
		for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) {
			if((*itr)->isAlive())
				players[i]++;
		}
	}

	m_mapMgr->GetStateManager().UpdateWorldState(m_pcWorldStates[GOLD_TEAM], players[GOLD_TEAM]);
	m_mapMgr->GetStateManager().UpdateWorldState(m_pcWorldStates[GREEN_TEAM], players[GREEN_TEAM]);

	if(!m_started)
		return;

	if(players[GOLD_TEAM] == 0)
		m_losingteam = GOLD_TEAM;
	else if(players[GREEN_TEAM] == 0)
		m_losingteam = GREEN_TEAM;
	else
		return;

	Finish();
}

void Arena::RecalculateArenaTeams(ArenaTeam * pWinner, ArenaTeam * pLoser)
{
	// Get the winner's chance to win
	int32 ourRating = (int32)pWinner->m_stat_rating;
	int32 otherRating = (int32)pLoser->m_stat_rating;

	float expo = (ourRating - otherRating) / 400.0f;
	float chanceToWin = 1.0f / ( 1.0f + pow(10.0f, expo));

	int32 teamBonus = (int32)(32.0f*(chanceToWin));

	if((int32)pLoser->m_stat_rating + teamBonus < 0)
		pLoser->m_stat_rating = 0;
	else
		pLoser->m_stat_rating -= (uint32)teamBonus;

	pWinner->m_stat_rating += (uint32)teamBonus;

	if(pWinner->m_stat_rating > 3000)
		pWinner->m_stat_rating = 3000;

	pWinner->m_stat_gameswonseason++;
	pWinner->m_stat_gameswonweek++;
	pWinner->m_stat_gamesplayedweek++;
	pLoser->m_stat_gamesplayedweek++;

	objmgr.UpdateArenaTeamRankings();
}

void Arena::Finish()
{
	m_ended = true;
	m_nextPvPUpdateTime = 0;
	UpdatePvPData();
	PlaySoundToAll(m_losingteam ? SOUND_ALLIANCEWINS : SOUND_HORDEWINS);

	sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_CLOSE);
	sEventMgr.RemoveEvents(this, EVENT_ARENA_ANTISTEALTH);
	sEventMgr.AddEvent(((CBattleground*)this), &CBattleground::Close, EVENT_BATTLEGROUND_CLOSE, 120000, 1,0);

	/* update arena team stats */
	doneteams.clear();
	if(rated_match)
	{
		ArenaTeam *pTeams[2];
		pTeams[0] = m_ratedTeams[0] ? objmgr.GetArenaTeamById(m_ratedTeams[0]) : NULL;
		pTeams[1] = m_ratedTeams[1] ? objmgr.GetArenaTeamById(m_ratedTeams[1]) : NULL;
		if( pTeams[0] != NULL && pTeams[1]!= NULL )
		{
			ArenaTeam * pWinningTeam = m_losingteam == 0 ? pTeams[1] : pTeams[0];
			ArenaTeam * pLosingTeam = m_losingteam == 0 ? pTeams[0] : pTeams[1];

			RecalculateArenaTeams(pWinningTeam, pLosingTeam);	

			for(uint32 i = 0; i < 2; i++)
			{
				for(std::set<PlayerInfo*>::iterator itr = m_RatedPlayers[i].begin(); itr != m_RatedPlayers[i].end(); itr++)
				{
					PlayerInfo * pi = (*itr);
					if(pTeams[i]->GetMember(pi))
					{
						ArenaTeamMember * member = pTeams[i]->GetMember(pi);
						if(member != NULL)
						{
							if(pTeams[i] == pWinningTeam)
							{
								member->Won_ThisSeason++;
								member->Won_ThisWeek++;
							}
						}
					}
				}
			}

			pTeams[0]->SaveToDB();
			pTeams[1]->SaveToDB();
		}
	}
	for(int i = 0; i < 2; i++)
	{
		bool victorious = (i != m_losingteam);
		set<Player*>::iterator itr = m_players[i].begin();
		for(; itr != m_players[i].end(); itr++)
		{
			Player * plr = (Player *)(*itr);
			plr->Root();

			if( plr->m_bgScore.DamageDone == 0 && plr->m_bgScore.HealingDone == 0 )
				continue;

			sHookInterface.OnArenaFinish(plr, m_arenateamtype, plr->m_playerInfo->arenaTeam[m_arenateamtype], victorious, rated_match);
		}
	}
}

LocationVector Arena::GetStartingCoords(uint32 Team)
{
	// 559, 562, 572
	/*
	A start
	H start
	Repop
	572 1295.322388 1585.953369 31.605387
	572 1277.105103 1743.956177 31.603209
	572 1286.112061 1668.334961 39.289127

	562 6184.806641 236.643463 5.037095
	562 6292.032227 287.570343 5.003577
	562 6241.171875 261.067322 0.891833

	559 4085.861328 2866.750488 12.417445
	559 4027.004883 2976.964844 11.600499
	559 4057.042725 2918.686523 13.051933
	*/
	switch(m_mapMgr->GetMapId())
	{
		/* loraedeon */
	case 572:
		{
			if(Team)
				return LocationVector(1277.105103f, 1743.956177f, 31.603209f);
			else
				return LocationVector(1295.322388f, 1585.953369f, 31.605387f);
		}break;

		/* blades edge arena */
	case 562:
		{
			if(Team)
				return LocationVector(6292.032227f, 287.570343f, 5.003577f);
			else
				return LocationVector(6184.806641f, 236.643463f, 5.037095f);
		}break;

		/* nagrand arena */
	case 559:
		{
			if(Team)
				return LocationVector(4027.004883f, 2976.964844f, 11.600499f);
			else
				return LocationVector(4085.861328f, 2866.750488f, 12.417445f);
		}break;
	}

	return LocationVector(0,0,0,0);
}

bool Arena::HookHandleRepop(Player * plr)
{
	// 559, 562, 572
	/*
	A start
	H start
	Repop
	572 1295.322388 1585.953369 31.605387
	572 1277.105103 1743.956177 31.603209
	572 1286.112061 1668.334961 39.289127

	562 6184.806641 236.643463 5.037095
	562 6292.032227 287.570343 5.003577
	562 6241.171875 261.067322 0.891833

	559 4085.861328 2866.750488 12.417445
	559 4027.004883 2976.964844 11.600499
	559 4057.042725 2918.686523 13.051933
	*/
	LocationVector dest(0,0,0,0);
	switch(m_mapMgr->GetMapId())
	{
		/* loraedeon */
	case 572: {
			dest.ChangeCoords(1286.112061f, 1668.334961f, 39.289127f);
		}break;

		/* blades edge arena */
	case 562: {
			dest.ChangeCoords(6241.171875f, 261.067322f, 0.891833f);
		}break;

		/* nagrand arena */
	case 559: {
			dest.ChangeCoords(4057.042725f, 2918.686523f, 13.051933f);
		}break;
	}

	plr->SafeTeleport(m_mapMgr->GetMapId(), m_mapMgr->GetInstanceID(), dest);
	return true;
}

void Arena::HookOnAreaTrigger(Player * plr, uint32 id)
{

}

void Player::FullHPMP()
{
	if( isDead() )
		ResurrectPlayer(NULL);

    SetUInt32Value(UNIT_FIELD_HEALTH, GetUInt32Value(UNIT_FIELD_MAXHEALTH));
    SetUInt32Value(UNIT_FIELD_POWER1, GetUInt32Value(UNIT_FIELD_MAXPOWER1));
    SetUInt32Value(UNIT_FIELD_POWER4, GetUInt32Value(UNIT_FIELD_MAXPOWER4));
}
