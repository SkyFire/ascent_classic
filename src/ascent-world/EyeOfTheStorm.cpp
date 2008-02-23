/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
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

const float EOTSGraveyardLocations[EOTS_TOWER_COUNT][3] = {
	{ 2009.388428f, 1455.857910f, 1172.221802f },			// BE Tower
	{ 2008.504028f, 1674.463867f, 1182.099487f },			// Fel Reaver Ruins
	{ 2357.358887f, 1681.148438f, 1173.154785f },			// Mage Tower
	{ 2353.309326f, 1455.936768f, 1185.333740f },			// Draenei Ruins
};

const float EOTSCPLocations[EOTS_TOWER_COUNT][3] = {
	{ 2048.290039f, 1393.757690f, 1194.363525f },			// BE Tower
	{ 2043.571533f, 1729.117310f, 1189.911865f },			// Fel Reaver Ruins
	{ 2284.430664f, 1731.128488f, 1189.874512f },			// Mage Tower
	{ 2285.848877f, 1402.939575f, 1197.128540f },			// Draenei Ruins
};

const float EOTSFlagLocation[3] = { 2174.718750f, 1568.766113f, 1159.958740f };
const float EOTSStartLocations[2][3] = {
	{ 2520.728027f, 1592.083984f, 1268.558228f },
	{ 1808.166138f, 1533.971924f, 1267.698975f },
};

const float EOTSBubbleLocations[2][4] = {
	{ 2527.596924f, 1596.906494f, 1262.128052f, -3.124139f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
};

const float EOTSBubbleRotations[2][4] = {
	{ -0.173642f, -0.001515f, 0.984770f, -0.008594f },
	{ -0.173642f, -0.001515f, 0.984770f, -0.008594f },
};

//===================================================
// 184083 - Draenei Tower Cap Pt, 184082 - Human Tower Cap Pt, 184081 - Fel Reaver Cap Pt, 184080 - BE Tower Cap Pt
#define EOTS_GO_BE_TOWER 184080
#define EOTS_GO_FELREAVER 184081
#define EOTS_GO_MAGE_TOWER 184082
#define EOTS_GO_DRAENEI_TOWER 184083

#define EOTS_TOWER_BE 1
#define EOTS_TOWER_FELREAVER 2
#define EOTS_TOWER_MAGE 3
#define EOTS_TOWER_DRAENEI 4

#define EOTS_BANNER_NEUTRAL 184382
#define EOTS_BANNER_ALLIANCE 184381
#define EOTS_BANNER_HORDE 184380

#define EOTS_CAPTURE_DISTANCE 900 /*30*/
const uint32 EOTSTowerIds[EOTS_TOWER_COUNT] = { EOTS_GO_BE_TOWER, EOTS_GO_FELREAVER, EOTS_GO_MAGE_TOWER, EOTS_GO_DRAENEI_TOWER };

/**
 * WorldStates
 */
#define EOTS_WORLDSTATE_DISPLAYON 2718
#define EOTS_WORLDSTATE_DISPLAYVALUE 2719
#define EOTS_WORLDSTATE_ALLIANCE_VICTORYPOINTS 2749
#define EOTS_WORLDSTATE_HORDE_VICTORYPOINTS 2750
#define EOTS_WORLDSTATE_ALLIANCE_BASES 2752
#define EOTS_WORLDSTATE_HORDE_BASES 2753


#define EOTS_CAPTURE_RATE 20

EyeOfTheStorm::EyeOfTheStorm(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t) : CBattleground(mgr,id,lgroup,t)
{
	uint32 i;

	for(i = 0; i < EOTS_TOWER_COUNT; ++i)
	{
		m_CPStatus[i] = 50;
		m_CPBanner[i] = NULL;
		m_CPStatusGO[i] = NULL;
	}

	m_flagHolder = 0;
}

EyeOfTheStorm::~EyeOfTheStorm()
{

}

bool EyeOfTheStorm::HookHandleRepop(Player * plr)
{
	return false;
}

void EyeOfTheStorm::HookOnAreaTrigger(Player * plr, uint32 id)
{

}

void EyeOfTheStorm::HookOnPlayerDeath(Player * plr)
{
	plr->m_bgScore.Deaths++;
	UpdatePvPData();
}

void EyeOfTheStorm::HookFlagDrop(Player * plr, GameObject * obj)
{

}

void EyeOfTheStorm::HookFlagStand(Player * plr, GameObject * obj)
{

}

void EyeOfTheStorm::HookOnMount(Player * plr)
{

}

void EyeOfTheStorm::OnAddPlayer(Player * plr)
{

}

void EyeOfTheStorm::OnRemovePlayer(Player * plr)
{
	uint32 i;

	for( i = 0; i < EOTS_TOWER_COUNT; ++i )
	{
		m_CPDisplay[i].erase( plr );
	}
}

void EyeOfTheStorm::OnCreate()
{
	GameObjectInfo* goi;
	uint32 i;


	/* eww worldstates */
	SetWorldState(2565, 142);
	SetWorldState(2720, 0);
	SetWorldState(2719, 0);
	SetWorldState(2718, 0);
	SetWorldState(2260, 0);
	SetWorldState(2264, 0);
	SetWorldState(2263, 0);
	SetWorldState(2262, 0);
	SetWorldState(2261, 0);
	SetWorldState(2259, 0);
	SetWorldState(2742, 0);
	SetWorldState(2741, 0);
	SetWorldState(2740, 0);
	SetWorldState(2739, 0);
	SetWorldState(2738, 0);
	SetWorldState(2737, 0);
	SetWorldState(2736, 0);
	SetWorldState(2735, 0);
	SetWorldState(2733, 0);
	SetWorldState(2732, 0);
	SetWorldState(2731, 1);
	SetWorldState(2730, 0);
	SetWorldState(2729, 0);
	SetWorldState(2728, 1);
	SetWorldState(2727, 0);
	SetWorldState(2726, 0);
	SetWorldState(2725, 1);
	SetWorldState(2724, 0);
	SetWorldState(2723, 0);
	SetWorldState(2722, 1);
	SetWorldState(2757, 1);
	SetWorldState(2753, 0);
	SetWorldState(2752, 0);
	SetWorldState(2770, 1);
	SetWorldState(2769, 1);
	SetWorldState(2750, 0);
	SetWorldState(2749, 0);
	SetWorldState(3191, 2);
	SetWorldState(3218, 0);
	SetWorldState(3217, 0);
	SetWorldState(3085, 379);

	/* create gameobjects */
	for(i = 0; i < EOTS_TOWER_COUNT; ++i)
	{
		goi = GameObjectNameStorage.LookupEntry( EOTSTowerIds[i] );
		if( goi == NULL )
		{
			Log.LargeErrorMessage(LARGERRORMESSAGE_ERROR, "EOTS is being created and you are missing gameobjects. Terminating.");
			abort();
			return;
		}

		m_CPStatusGO[i] = m_mapMgr->CreateGameObject();
		m_CPStatusGO[i]->CreateFromProto( goi->ID, m_mapMgr->GetMapId(), EOTSCPLocations[i][0], EOTSCPLocations[i][1], EOTSCPLocations[i][2], 0);
		m_CPStatusGO[i]->PushToWorld( m_mapMgr );

		goi = GameObjectNameStorage.LookupEntry( EOTS_BANNER_NEUTRAL );
		if( goi == NULL )
		{
			Log.LargeErrorMessage(LARGERRORMESSAGE_ERROR, "EOTS is being created and you are missing gameobjects. Terminating.");
			abort();
			return;
		}

		m_CPBanner[i] = m_mapMgr->CreateGameObject();
		m_CPBanner[i]->CreateFromProto( goi->ID, m_mapMgr->GetMapId(), EOTSCPLocations[i][0], EOTSCPLocations[i][1], EOTSCPLocations[i][2], 0);
		m_CPBanner[i]->PushToWorld( m_mapMgr );
	}

	/* BUBBLES! */
	for( i = 0; i < 2; ++i )
	{
		m_bubbles[i] = m_mapMgr->CreateGameObject();
		if( !m_bubbles[i]->CreateFromProto( 184719, m_mapMgr->GetMapId(), EOTSBubbleLocations[i][0], EOTSBubbleLocations[i][1], EOTSBubbleLocations[i][2], EOTSBubbleLocations[i][3] ) )
		{
			Log.LargeErrorMessage(LARGERRORMESSAGE_ERROR, "EOTS is being created and you are missing gameobjects. Terminating.");
			abort();
			return;
		}

		m_bubbles[i]->PushToWorld( m_mapMgr );
	}
}

void EyeOfTheStorm::RespawnCPFlag(uint32 i, uint32 id)
{
	m_CPBanner[i]->RemoveFromWorld(false);
	m_CPBanner[i]->SetNewGuid( m_mapMgr->GenerateGameobjectGuid() );
	m_CPBanner[i]->CreateFromProto( id, m_mapMgr->GetMapId(), m_CPBanner[i]->GetPositionX(), m_CPBanner[i]->GetPositionY(), m_CPBanner[i]->GetPositionZ(), m_CPBanner[i]->GetOrientation() );
	m_CPBanner[i]->PushToWorld( m_mapMgr );
}

void EyeOfTheStorm::RespawnFlag()
{

}

void EyeOfTheStorm::UpdateCPs()
{
	uint32 i;
	set<Player*>::iterator itr, itrend;
	Player * plr;
	GameObject * go;
	int32 delta;
	uint32 playercounts[2];
	EOTSCaptureDisplayList::iterator eitr, eitr2, eitrend;
	EOTSCaptureDisplayList * disp;

	for(i = 0; i < EOTS_TOWER_COUNT; ++i)
	{
		/* loop players inrange, add any that arent in the set to the set */
		playercounts[0] = playercounts[1] = 0;
		go = m_CPStatusGO[i];
		disp = &m_CPDisplay[i];
		itr = go->GetInRangePlayerSetBegin();
		itrend = go->GetInRangePlayerSetEnd();

		for( ; itr != itrend; ++itr )
		{
			plr = *itr;
			if( plr->GetDistance2dSq( go ) <= EOTS_CAPTURE_DISTANCE )
			{
				playercounts[plr->GetTeam()]++;

				if( disp->find( plr ) == disp->end() )
				{
					disp->insert( plr );
					plr->SendWorldStateUpdate(EOTS_WORLDSTATE_DISPLAYON, 1);
				}
			}
		}

		/* score diff calculation */
		printf("EOTS: Playercounts = %u %u\n", playercounts[0], playercounts[1]);
		if(playercounts[0] != playercounts[1])
		{
			if(playercounts[0] > playercounts[1])
				delta = playercounts[0];
			else if(playercounts[1] > playercounts[0])
				delta = -(int32)playercounts[1];

			m_CPStatus[i] += delta;
			if( m_CPStatus[i] > 100 )
				m_CPStatus[i] = 100;
			else if( m_CPStatus[i] < 0 )
				m_CPStatus[i] = 0;

			// change the flag depending on cp status
			if( m_CPStatus[i] == 0 )
			{
				if( m_CPBanner[i]->GetEntry() != EOTS_BANNER_HORDE )
					RespawnCPFlag(i, EOTS_BANNER_HORDE);
			}
			else if( m_CPStatus[i] == 100 )
			{
				if( m_CPBanner[i]->GetEntry() != EOTS_BANNER_ALLIANCE )
					RespawnCPFlag(i, EOTS_BANNER_ALLIANCE);
			}
			else
			{
				if( m_CPBanner[i]->GetEntry() != EOTS_BANNER_NEUTRAL )
					RespawnCPFlag(i, EOTS_BANNER_NEUTRAL);
			}
		}

		/* update the players with the new value */
		eitr = disp->begin();
		eitrend = disp->end();

		for( ; eitr != eitrend; )
		{
			plr = *eitr;
			eitr2 = eitr;
			++eitr;

			if( plr->GetDistance2dSq( go ) > EOTS_CAPTURE_DISTANCE )
			{
				disp->erase( eitr2 );
				plr->SendWorldStateUpdate(EOTS_WORLDSTATE_DISPLAYON, 0);			// hide the cp bar
			}
			else
				plr->SendWorldStateUpdate(EOTS_WORLDSTATE_DISPLAYVALUE, m_CPStatus[i]);
		}
	}
}

void EyeOfTheStorm::GeneratePoints()
{
	uint32 i;
	uint32 towers[2] = {0,0};

	/*
	#  Unlike Arathi Basin, points are always generated in 2 seconds intervals no matter how many towers are controlled by both teams.
	# Each claimed tower generates victory points for the controlling team. The more towers your team owns, the faster your team gains points

	* 1 tower controlled = 1 point/tick (0.5 points per second)
	* 2 towers controlled = 2 points/tick (1 point per second)
	* 3 towers controlled = 5 points/tick (2.5 points per second)
	* 4 towers controlled = 10 points/tick (5 points per second) 

	*/
	uint32 pointspertick[5] = { 0, 1, 2, 5, 10 };

	for(i = 0; i < EOTS_TOWER_COUNT; ++i)
	{
		if(m_CPStatus[i] == 100)
			towers[0]++;
		else if(m_CPStatus[i] == 0)
			towers[1]++;
	}

	for( i = 0; i < 2; ++i )
	{
		if( towers[i] == 0 )
		{
			printf("EOTS: No points on team %u\n", i);
			continue;
		}

		if( GivePoints( i, pointspertick[towers[i]] ) )
			return;
	}
}

bool EyeOfTheStorm::GivePoints(uint32 team, uint32 points)
{
	printf("EOTS: Give team %u %u points.\n", team, points);
	return true;
}

void EyeOfTheStorm::HookOnPlayerKill(Player * plr, Unit * pVictim)
{
	plr->m_bgScore.KillingBlows++;
	UpdatePvPData();
}

void EyeOfTheStorm::HookOnHK(Player * plr)
{
	plr->m_bgScore.HonorableKills++;
	UpdatePvPData();
}

void EyeOfTheStorm::SpawnBuff(uint32 x)
{

}

LocationVector EyeOfTheStorm::GetStartingCoords(uint32 Team)
{
	return LocationVector( EOTSStartLocations[Team][0], 
		EOTSStartLocations[Team][1],
		EOTSStartLocations[Team][2] );
}

void EyeOfTheStorm::OnStart()
{
	uint32 i;

	/* start the events */
	sEventMgr.AddEvent(this, &EyeOfTheStorm::GeneratePoints, EVENT_EOTS_GIVE_POINTS, 2000, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	sEventMgr.AddEvent(this, &EyeOfTheStorm::UpdateCPs, EVENT_EOTS_CHECK_CAPTURE_POINT_STATUS, 5000, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

	/* remove the bubbles */
	for( i = 0; i < 2; ++i )
	{
		m_bubbles[i]->RemoveFromWorld(false);
		delete m_bubbles[i];
		m_bubbles[i] = NULL;
	}
}
