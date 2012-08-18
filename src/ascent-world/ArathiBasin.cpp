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

#define BASE_RESOURCES_GAIN 10
#define RESOURCES_WARNING_THRESHOLD 1800
#define RESOURCES_WINVAL 2000
#define RESOURCES_TO_GAIN_BH 200
#define BASE_BH_GAIN 14
uint32 buffentrys[3] = {180380,180362,180146};

/* AB Battleground Data */

	static float GraveyardLocations[AB_NUM_CONTROL_POINTS][3] = {
		{ 1201.869507f, 1163.130615f, -56.285969f },												// STABLES
		{ 834.726379f, 784.978699f, -57.081944f },													// FARM
		{ 1016.588318f, 955.184692f, -42.828693f },													// BLACKSMITH
		{ 1211.523682f, 781.556946f, -82.709511f },													// MINE
		{ 772.755676f, 1213.113770f, 15.797392f },													// LUMBERMILL
	};

	static float NoBaseGYLocations[2][3] = {
		{ 1354.699951f, 1270.270020f, -11.129100f },												// ALLIANCE
		{ 713.710022f, 638.364014f, -10.599900f },													// HORDE
	};

	static const char * ControlPointNames[AB_NUM_CONTROL_POINTS] = {
		"Stable",																					// STABLE
		"Farm",																						// FARM
		"Blacksmith",																				// BLACKSMITH
		"Mine",																						// MINE
		"Lumber Mill",																				// LUMBERMILL
	};

	static uint32 ControlPointGoIds[AB_NUM_CONTROL_POINTS][AB_NUM_SPAWN_TYPES] = {
		  // NEUTRAL    ALLIANCE-ATTACK    HORDE-ATTACK    ALLIANCE-CONTROLLED    HORDE_CONTROLLED
		{ 180087,       180085,            180086,         180076,                180078 },			// STABLE	
		{ 180089,       180085,            180086,         180076,                180078 },			// FARM
		{ 180088,       180085,            180086,         180076,                180078 },			// BLACKSMITH
		{ 180091,       180085,            180086,         180076,                180078 },			// MINE
		{ 180090,       180085,            180086,         180076,                180078 },			// LUMBERMILL
	};

	static float ControlPointCoordinates[AB_NUM_CONTROL_POINTS][4] = {
		{ 1166.779541f, 1200.147583f, -56.701763f, -2.251474f },									// STABLE
		{ 806.2484741f, 874.2167358f, -55.9936981f, 0.8377580f },									// FARM
		{ 977.0503540f, 1046.5208740f, -44.8276138f, 0.5410520f },									// BLACKSMITH
		{ 1146.9224854f, 848.1899414f, -110.9200210f, 2.4260077f },									// MINE
		{ 856.141907f, 1148.902100f, 11.184692f, -2.303835f },										// LUMBERMILL
	};

	static float ControlPointRotations[AB_NUM_CONTROL_POINTS][2] = {
		{ 0.9025853f, -0.4305111f },																// STABLE
		{ 0.4067366f, 0.9135454f },																	// FARM
		{ 0.2672384f, 0.9636304f },																	// BLACKSMITH
		{ 0.9366722f, 0.3502073f },																	// MINE
		{ 0.9135455f, -0.4067366f },																// LUMBERMILL
	};

	static float BuffCoordinates[AB_NUM_CONTROL_POINTS][4] = {
		{ 1185.56616210938f, 1184.62854003906f, -56.3632850646973f, 2.30383467674255f },			// STABLE
		{ 990.113098144531f, 1008.73028564453f, -42.6032752990723f, 0.820304811000824f },			// FARM
		{ 816.906799f, 842.339844f, -56.538746f, 3.272740f },										// BLACKSMITH
		{ 808.846252441406f, 1185.41748046875f, 11.9216051101685f, -0.663225054740906f },			// MINE
		{ 1147.09057617188f, 816.836242675781f, -98.3989562988281f, -0.226892784237862f },			// LUMBERMILL
	};

	static float BuffRotations[AB_NUM_CONTROL_POINTS][2] = {
		{ 0.913545489311218f, 0.406736612319946f },													// STABLE
		{ 0.39874908328056f, 0.917060077190399f },													// FARM
		{ 0.913545489311218f, 0.406736612319946f },													// BLACKSMITH
		{ 0.325568109750748f, -0.945518612861633f },												// MINE
		{ 0.113203197717667f, -0.993571877479553f },												// LUMBERMILL
	};

	static uint32 AssaultFields[AB_NUM_CONTROL_POINTS][2] = {
		{ WORLDSTATE_AB_CAPTURING_STABLES_ALLIANCE, WORLDSTATE_AB_CAPTURING_STABLES_HORDE },		// STABLE
		{ WORLDSTATE_AB_CAPTURING_FARM_ALLIANCE, WORLDSTATE_AB_CAPTURING_FARM_HORDE },				// FARM
		{ WORLDSTATE_AB_CAPTURING_BLACKSMITH_ALLIANCE, WORLDSTATE_AB_CAPTURING_BLACKSMITH_HORDE },	// BLACKSMITH
		{ WORLDSTATE_AB_CAPTURING_GOLDMINE_ALLIANCE, WORLDSTATE_AB_CAPTURING_GOLDMINE_HORDE },		// MINE
		{ WORLDSTATE_AB_CAPTURING_LUMBERMILL_ALLIANCE, WORLDSTATE_AB_CAPTURING_LUMBERMILL_HORDE },	// LUMBERMILL
	};

	static uint32 OwnedFields[AB_NUM_CONTROL_POINTS][2] = {
		{ WORLDSTATE_AB_CAPTURED_STABLES_ALLIANCE, WORLDSTATE_AB_CAPTURED_STABLES_HORDE },			// STABLE
		{ WORLDSTATE_AB_CAPTURED_FARM_ALLIANCE, WORLDSTATE_AB_CAPTURED_FARM_HORDE },				// FARM
		{ WORLDSTATE_AB_CAPTURED_BLACKSMITH_ALLIANCE, WORLDSTATE_AB_CAPTURED_BLACKSMITH_HORDE },	// BLACKSMITH
		{ WORLDSTATE_AB_CAPTURED_GOLDMINE_ALLIANCE, WORLDSTATE_AB_CAPTURED_GOLDMINE_HORDE },		// MINE
		{ WORLDSTATE_AB_CAPTURED_LUMBERMILL_ALLIANCE, WORLDSTATE_AB_CAPTURED_LUMBERMILL_HORDE },	// LUMBERMILL
	};

	static uint32 NeutralFields[AB_NUM_CONTROL_POINTS] = {
		WORLDSTATE_AB_SHOW_STABLE_ICON,
		WORLDSTATE_AB_SHOW_FARM_ICON,
		WORLDSTATE_AB_SHOW_BACKSMITH_ICON,
		WORLDSTATE_AB_SHOW_GOLDMINE_ICON,
		WORLDSTATE_AB_SHOW_LUMBERMILL_ICON,
	};

	static uint32 ResourceUpdateIntervals[6] = {
		0,
		12000,
		9000,
		6000,
		3000,
		1000,
	};

	static uint32 PointBonusPerUpdate[6] = {
		0,
		10,
		10,
		10,
		10,
		30,
	};

/* End BG Data */

void ArathiBasin::SpawnBuff(uint32 x)
{
	uint32 chosen_buffid = buffentrys[RandomUInt(2)];
	GameObjectInfo * goi = GameObjectNameStorage.LookupEntry(chosen_buffid);
	if(goi == NULL)
		return;

	if(m_buffs[x] == NULL)
	{
		m_buffs[x] = SpawnGameObject(chosen_buffid, BuffCoordinates[x][0], BuffCoordinates[x][1], BuffCoordinates[x][2],
			BuffCoordinates[x][3], 0, 114, 1);

		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_02, BuffRotations[x][0]);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_ROTATION_03, BuffRotations[x][1]);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_STATE, 1);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
		m_buffs[x]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
		m_buffs[x]->PushToWorld(m_mapMgr);
	}
	else
	{
		// only need to reassign guid if the entry changes.
		if(m_buffs[x]->IsInWorld())
			m_buffs[x]->RemoveFromWorld(false);

		if(chosen_buffid != m_buffs[x]->GetEntry())
		{
			m_buffs[x]->SetNewGuid(m_mapMgr->GenerateGameobjectGuid());
			m_buffs[x]->SetUInt32Value(OBJECT_FIELD_ENTRY, chosen_buffid);
			m_buffs[x]->SetInfo(goi);
		}

		m_buffs[x]->PushToWorld(m_mapMgr);
	}
}

void ArathiBasin::SpawnControlPoint(uint32 Id, uint32 Type)
{
	GameObjectInfo * gi, * gi_aura;
	gi = GameObjectNameStorage.LookupEntry(ControlPointGoIds[Id][Type]);
	if(gi == NULL)
		return;

	gi_aura = gi->sound3 ? GameObjectNameStorage.LookupEntry(gi->sound3) : NULL;

	if(m_controlPoints[Id] == NULL)
	{
		m_controlPoints[Id] = SpawnGameObject(gi->ID, ControlPointCoordinates[Id][0], ControlPointCoordinates[Id][1],
			ControlPointCoordinates[Id][2], ControlPointCoordinates[Id][3], 0, 35, 1.0f);

		m_controlPoints[Id]->SetFloatValue(GAMEOBJECT_ROTATION_02, ControlPointRotations[Id][0]);
		m_controlPoints[Id]->SetFloatValue(GAMEOBJECT_ROTATION_03, ControlPointRotations[Id][1]);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_STATE, 1);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_TYPE_ID, gi->Type);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_DYN_FLAGS, 1);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_DISPLAYID, gi->DisplayID);

		switch(Type)
		{
		case AB_SPAWN_TYPE_ALLIANCE_ASSAULT:
		case AB_SPAWN_TYPE_ALLIANCE_CONTROLLED:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 2);
			break;

		case AB_SPAWN_TYPE_HORDE_ASSAULT:
		case AB_SPAWN_TYPE_HORDE_CONTROLLED:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 1);
			break;

		default:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 35);		// neutral
			break;
		}

		m_controlPoints[Id]->bannerslot = Id;
		m_controlPoints[Id]->PushToWorld(m_mapMgr);
	}
	else
	{
		if(m_controlPoints[Id]->IsInWorld())
			m_controlPoints[Id]->RemoveFromWorld(false);

		// assign it a new guid (client needs this to see the entry change?)
		m_controlPoints[Id]->SetNewGuid(m_mapMgr->GenerateGameobjectGuid());
		m_controlPoints[Id]->SetUInt32Value(OBJECT_FIELD_ENTRY, gi->ID);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_DISPLAYID, gi->DisplayID);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_TYPE_ID, gi->Type);

		switch(Type)
		{
		case AB_SPAWN_TYPE_ALLIANCE_ASSAULT:
		case AB_SPAWN_TYPE_ALLIANCE_CONTROLLED:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 2);
			break;

		case AB_SPAWN_TYPE_HORDE_ASSAULT:
		case AB_SPAWN_TYPE_HORDE_CONTROLLED:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 1);
			break;

		default:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 35);		// neutral
			break;
		}

		m_controlPoints[Id]->SetInfo(gi);
		m_controlPoints[Id]->PushToWorld(m_mapMgr);
	}

	if(gi_aura==NULL)
	{
		// remove it if it exists
		if(m_controlPointAuras[Id]!=NULL && m_controlPointAuras[Id]->IsInWorld())
			m_controlPointAuras[Id]->RemoveFromWorld(false);
			
		return;
	}

	if(m_controlPointAuras[Id] == NULL)
	{
		m_controlPointAuras[Id] = SpawnGameObject(gi_aura->ID, ControlPointCoordinates[Id][0], ControlPointCoordinates[Id][1],
			ControlPointCoordinates[Id][2], ControlPointCoordinates[Id][3], 0, 35, 1.0f);

		m_controlPointAuras[Id]->SetFloatValue(GAMEOBJECT_ROTATION_02, ControlPointRotations[Id][0]);
		m_controlPointAuras[Id]->SetFloatValue(GAMEOBJECT_ROTATION_03, ControlPointRotations[Id][1]);
		m_controlPointAuras[Id]->SetUInt32Value(GAMEOBJECT_STATE, 1);
		m_controlPointAuras[Id]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
		m_controlPointAuras[Id]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
		m_controlPointAuras[Id]->bannerauraslot = Id;
		m_controlPointAuras[Id]->PushToWorld(m_mapMgr);
	}
	else
	{
		if(m_controlPointAuras[Id]->IsInWorld())
			m_controlPointAuras[Id]->RemoveFromWorld(false);

		// re-spawn the aura
		m_controlPointAuras[Id]->SetNewGuid(m_mapMgr->GenerateGameobjectGuid());
		m_controlPointAuras[Id]->SetUInt32Value(OBJECT_FIELD_ENTRY, gi_aura->ID);
		m_controlPointAuras[Id]->SetUInt32Value(GAMEOBJECT_DISPLAYID, gi_aura->DisplayID);
		m_controlPointAuras[Id]->SetInfo(gi_aura);
		m_controlPointAuras[Id]->PushToWorld(m_mapMgr);
	}	
}

void ArathiBasin::OnCreate()
{
	// Alliance Gate
	GameObject *gate = SpawnGameObject(180255, 1284.597290f, 1281.166626f, -15.977916f, 0.706859f, 32, 114, 1.5799990f);
	gate->SetFloatValue(GAMEOBJECT_ROTATION,0.0129570f);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_01,-0.0602880f);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_02,0.3449600f);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_03,0.9365900f);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	// horde gate
	gate = SpawnGameObject(180256, 708.0902710f, 708.4479370f, -17.3898964f, -2.3910990f, 32, 114, 1.5699990f);
	gate->SetFloatValue(GAMEOBJECT_ROTATION,0.0502910f);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_01,0.0151270f);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_02,0.9292169f);
	gate->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.3657840f);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	// spawn (default) control points
	SpawnControlPoint(AB_CONTROL_POINT_STABLE,		AB_SPAWN_TYPE_NEUTRAL);
	SpawnControlPoint(AB_CONTROL_POINT_BLACKSMITH,	AB_SPAWN_TYPE_NEUTRAL);
	SpawnControlPoint(AB_CONTROL_POINT_LUMBERMILL,	AB_SPAWN_TYPE_NEUTRAL);
	SpawnControlPoint(AB_CONTROL_POINT_MINE,		AB_SPAWN_TYPE_NEUTRAL);
	SpawnControlPoint(AB_CONTROL_POINT_FARM,		AB_SPAWN_TYPE_NEUTRAL);

	// spawn buffs
	SpawnBuff(AB_BUFF_STABLES);
	SpawnBuff(AB_BUFF_BLACKSMITH);
	SpawnBuff(AB_BUFF_LUMBERMILL);
	SpawnBuff(AB_BUFF_MINE);
	SpawnBuff(AB_BUFF_FARM);

	// spawn the h/a base spirit guides
	AddSpiritGuide(SpawnSpiritGuide(NoBaseGYLocations[0][0],NoBaseGYLocations[0][1],NoBaseGYLocations[0][2], 0.0f, 0));
	AddSpiritGuide(SpawnSpiritGuide(NoBaseGYLocations[1][0],NoBaseGYLocations[1][1],NoBaseGYLocations[1][2], 0.0f, 1));

	// w00t
	WorldStateManager &sm = m_mapMgr->GetStateManager();

	// urrrgh worldstates
	sm.CreateWorldState(0x8D8, 0x00);
	sm.CreateWorldState(0x8D7, 0x00);
	sm.CreateWorldState(0x8D6, 0x00);
	sm.CreateWorldState(0x8D5, 0x00);
	sm.CreateWorldState(0x8D4, 0x00);
	sm.CreateWorldState(0x8D3, 0x00);

	// AB world state's
	// unknowns, need more research
	sm.CreateWorldState(0x7A3, 1800); // unknown
	sm.CreateWorldState(0x745, 0x02); // unknown

	// Icon stuff for on the map
	sm.CreateWorldState(WORLDSTATE_AB_SHOW_BACKSMITH_ICON, 			 0x01);
	sm.CreateWorldState(WORLDSTATE_AB_SHOW_FARM_ICON, 				  0x01);
	sm.CreateWorldState(WORLDSTATE_AB_SHOW_LUMBERMILL_ICON, 			0x01);
	sm.CreateWorldState(WORLDSTATE_AB_SHOW_GOLDMINE_ICON, 			 0x01);
	sm.CreateWorldState(WORLDSTATE_AB_SHOW_STABLE_ICON, 			0x01);

	// LumberMill
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_LUMBERMILL_HORDE, 	   0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_LUMBERMILL_ALLIANCE, 	0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_LUMBERMILL_HORDE, 		0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_LUMBERMILL_ALLIANCE, 	 0x00);

	// GoldMine
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_GOLDMINE_HORDE, 		 0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_GOLDMINE_ALLIANCE, 	  0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_GOLDMINE_HORDE, 		  0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_GOLDMINE_ALLIANCE, 	   0x00);

	// BlackSmith
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_BLACKSMITH_HORDE, 	   0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_BLACKSMITH_ALLIANCE, 	0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_BLACKSMITH_HORDE, 		0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_BLACKSMITH_ALLIANCE, 	 0x00);

	sm.CreateWorldState(WORLDSTATE_AB_MAX_SCORE, 						RESOURCES_WINVAL);
	sm.CreateWorldState(WORLDSTATE_AB_ALLIANCE_CAPTUREBASE, 			 0x00);
	sm.CreateWorldState(WORLDSTATE_AB_HORDE_CAPTUREBASE, 				0x00);
	sm.CreateWorldState(WORLDSTATE_AB_HORDE_RESOURCES, 				  0x00);
	sm.CreateWorldState(WORLDSTATE_AB_ALLIANCE_RESOURCES, 			   0x00);

	// Farm
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_FARM_ALLIANCE, 		 0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_FARM_HORDE, 			 0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_FARM_HORDE, 			  0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_FARM_ALLIANCE, 		   0x00);

	// Stables
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_STABLES_HORDE, 		  0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURING_STABLES_ALLIANCE, 	   0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_STABLES_HORDE, 		   0x00);
	sm.CreateWorldState(WORLDSTATE_AB_CAPTURED_STABLES_ALLIANCE, 		0x00);
}

void ArathiBasin::OnStart()
{
	for(uint32 i = 0; i < 2; ++i) {
		for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) {
			(*itr)->RemoveAura(BG_PREPARATION);
		}
	}

	// open gates
	for(list<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
	{
		(*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
		(*itr)->SetUInt32Value(GAMEOBJECT_STATE, 0);
	}

	/* correct? - burlex */
	PlaySoundToAll(SOUND_BATTLEGROUND_BEGIN);

	m_started = true;
}

ArathiBasin::ArathiBasin(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t) : CBattleground(mgr,id,lgroup,t)
{
	uint32 i;
	m_playerCountPerTeam=15;

	for(i = 0; i < AB_NUM_CONTROL_POINTS; ++i)
	{
		m_buffs[i] = NULL;
		m_controlPointAuras[i] = NULL;
		m_controlPoints[i] = NULL;
		m_spiritGuides[i] = NULL;
		m_basesAssaultedBy[i] = -1;
		m_basesOwnedBy[i] = -1;
		m_basesLastOwnedBy[i] = -1;
		m_flagIsVirgin[i] = true;
	}

	for(i = 0; i < 2; ++i)
	{
		m_resources[i] = 0;
		m_capturedBases[i] = 0;
		m_lastHonorGainResources[i] = 0;
		m_nearingVictory[i] = 0;
		m_reprewards[i] = 330;
	}
}

ArathiBasin::~ArathiBasin()
{
	// gates are always spawned, so mapmgr will clean them up
	for(uint32 i = 0; i < AB_NUM_CONTROL_POINTS; ++i)
	{
		// buffs may not be spawned, so delete them if they're not
		if(m_buffs[i] != NULL)
		{
			m_buffs[i]->m_battleground = NULL;
			if( !m_buffs[i]->IsInWorld() )
				delete m_buffs[i];
		}

		if(m_controlPoints[i] != NULL)
		{
			m_controlPoints[i]->m_battleground = NULL;
			if( !m_controlPoints[i]->IsInWorld() )
				delete m_controlPoints[i];
		}

		if(m_controlPointAuras[i])
		{
			m_controlPointAuras[i]->m_battleground = NULL;
			if( !m_controlPointAuras[i]->IsInWorld() )
				delete m_controlPointAuras[i];
		}
	}
}

void ArathiBasin::EventUpdateResources(uint32 Team)
{
	static uint32 resource_fields[2] = { WORLDSTATE_AB_ALLIANCE_RESOURCES, WORLDSTATE_AB_HORDE_RESOURCES };

	uint32 current_resources = m_resources[Team];
	uint32 current_bases = m_capturedBases[Team];

	if(current_bases>5)
		current_bases=5;

	// figure out how much resources we have to add to that team based on the number of captured bases.
	current_resources += (PointBonusPerUpdate[current_bases]);

	// did it change?
	if(current_resources == m_resources[Team])
		return;

	m_mainLock.Acquire();

	// check for overflow
	if(current_resources > RESOURCES_WINVAL)
		current_resources = RESOURCES_WINVAL;

	m_resources[Team] = current_resources;
	if((current_resources - m_lastHonorGainResources[Team]) >= RESOURCES_TO_GAIN_BH)
	{
		for(set<Player*>::iterator itr = m_players[Team].begin(); itr != m_players[Team].end(); ++itr)
			(*itr)->m_bgScore.BonusHonor += BASE_BH_GAIN;
	}

	// update the world states
	m_mapMgr->GetStateManager().UpdateWorldState(resource_fields[Team], current_resources);

	if( current_resources >= m_reprewards[Team] )
	{
		for(set<Player*>::iterator itx = m_players[Team].begin(); itx != m_players[Team].end(); itx++)
		{
			Player * plr = (*itx);
			if(!plr) continue;

			HonorHandler::AddHonorPointsToPlayer( plr, 20 );
		}

		m_reprewards[Team] += 330;
	}

	if(current_resources >= RESOURCES_WARNING_THRESHOLD && !m_nearingVictory[Team])
	{
		m_nearingVictory[Team] = true;
		SendChatMessage(Team ? CHAT_MSG_BG_EVENT_HORDE : CHAT_MSG_BG_EVENT_ALLIANCE, (uint64)0, "The %s has gathered %u resources and is nearing victory!", Team ? "Horde" : "Alliance", current_resources);
		uint32 sound = SOUND_ALLIANCE_BGALMOSTEND - Team;
		PlaySoundToAll(sound);
	}

	// check for winning condition
	if(current_resources == RESOURCES_WINVAL)
	{
		m_ended = true;
		m_losingteam = (Team) ? 0 : 1;
		m_nextPvPUpdateTime = 0;

		sEventMgr.RemoveEvents(this);
		sEventMgr.AddEvent(((CBattleground*)this), &CBattleground::Close, EVENT_BATTLEGROUND_CLOSE, 120000, 1,0);

		/* add the marks of honor to all players */
		SpellEntry * winner_spell = dbcSpell.LookupEntry(24953);
		SpellEntry * loser_spell = dbcSpell.LookupEntry(24952);
		for(uint32 i = 0; i < 2; ++i)
		{
			for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
			{
				(*itr)->Root();

				if( (*itr)->HasFlag(PLAYER_FLAGS, 0x2) )
					continue;

				if(i == m_losingteam)
					(*itr)->CastSpell((*itr), loser_spell, true);
				else
				{
					(*itr)->CastSpell((*itr), winner_spell, true);
					HonorHandler::AddHonorPointsToPlayer( (*itr), 20 );
				}
			}
		}
	}

	UpdatePvPData();
	m_mainLock.Release();
}

void ArathiBasin::HookOnPlayerDeath(Player * plr)
{
	// nothing in this BG
	plr->m_bgScore.Deaths++;
	UpdatePvPData();
}

void ArathiBasin::HookOnMount(Player * plr)
{
	// nothing in this BG
}

void ArathiBasin::HookOnPlayerKill(Player * plr, Unit * pVictim)
{
	if(pVictim->IsPlayer())
	{
		plr->m_bgScore.KillingBlows++;
		UpdatePvPData();
	}
}

void ArathiBasin::HookOnHK(Player * plr)
{
	plr->m_bgScore.HonorableKills++;
	UpdatePvPData();
}

void ArathiBasin::OnAddPlayer(Player * plr)
{
	if(!m_started)
		plr->CastSpell(plr, BG_PREPARATION, true);
}

void ArathiBasin::OnRemovePlayer(Player * plr)
{
	plr->RemoveAura(BG_PREPARATION);
}

void ArathiBasin::HookFlagDrop(Player * plr, GameObject * obj)
{
	// nothing?
}

void ArathiBasin::HookFlagStand(Player * plr, GameObject * obj)
{
	// nothing?
}

LocationVector ArathiBasin::GetStartingCoords(uint32 Team)
{
	if(Team)
		return LocationVector(684.75629f, 681.945007f, -12.915456f, 0.881211f);
	else
		return LocationVector(1314.932495f, 1311.246948f, -9.00952f,3.802896f);
}

void ArathiBasin::HookOnAreaTrigger(Player * plr, uint32 id)
{
	uint32 spellid=0;
	int32 buffslot = -1;
	switch(id)
	{
	case 3866:			// stables
		buffslot=AB_BUFF_STABLES;
		break;

	case 3867:			// farm
		buffslot=AB_BUFF_FARM;		
		break;

	case 3870:			// blacksmith
		buffslot=AB_BUFF_BLACKSMITH;
		break;

	case 3869:			// mine
		buffslot=AB_BUFF_MINE;
		break;

	case 3868:			// lumbermill
		buffslot=AB_BUFF_LUMBERMILL;
		break;

	case 3948:			// alliance/horde exits
	case 3949:
		{
			RemovePlayer(plr,false);
			return;
		}break;

	default:
		Log.Error("ArathiBasin", "Encountered unhandled areatrigger id %u", id);
		return;
		break;
	}

	if(plr->isDead())		// dont apply to dead players... :P
		return;	

	uint32 x = (uint32)buffslot;
	if(m_buffs[x] && m_buffs[x]->IsInWorld())
	{
		// apply the spell
		spellid = m_buffs[x]->GetInfo()->sound3;
		m_buffs[x]->RemoveFromWorld(false);

		// respawn it in buffrespawntime
		sEventMgr.AddEvent(this,&ArathiBasin::SpawnBuff,x,EVENT_AB_RESPAWN_BUFF,AB_BUFF_RESPAWN_TIME,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

		// cast the spell on the player
		SpellEntry * sp = dbcSpell.LookupEntryForced(spellid);
		if(sp)
		{
			Spell * pSpell = new Spell(plr, sp, true, NULL);
			SpellCastTargets targets(plr->GetGUID());
			pSpell->prepare(&targets);
		}
	}
}

bool ArathiBasin::HookHandleRepop(Player * plr)
{
	/* our uber leet ab graveyard handler */
	LocationVector dest( NoBaseGYLocations[plr->m_bgTeam][0], NoBaseGYLocations[plr->m_bgTeam][1], NoBaseGYLocations[plr->m_bgTeam][2], 0.0f );
	float current_distance = 999999.0f;
	float dist;

	for(uint32 i = 0; i < AB_NUM_CONTROL_POINTS; ++i)
	{
		if(m_basesOwnedBy[i] == (int32)plr->m_bgTeam)
		{
			dist = plr->GetPositionV()->Distance2DSq(GraveyardLocations[i][0], GraveyardLocations[i][1]);
			if(dist < current_distance)
			{
				current_distance = dist;
				dest.ChangeCoords(GraveyardLocations[i][0], GraveyardLocations[i][1], GraveyardLocations[i][2]);
			}
		}
	}

	// port us there.
	plr->SafeTeleport(plr->GetMapId(),plr->GetInstanceID(),dest);
	return true;
}

void ArathiBasin::CaptureControlPoint(uint32 Id, uint32 Team)
{
	if(m_basesOwnedBy[Id] != -1)
	{
		// there is a very slim chance of this happening, 2 teams evnets could clash..
		// just in case...
		return;
	}

	// anticheat, not really necessary because this is a server method but anyway
	if(m_basesAssaultedBy[Id] != (int32)Team)
		return;

	
	m_basesLastOwnedBy[Id] = Team;

	m_flagIsVirgin[Id] = false;

	m_basesOwnedBy[Id] = Team;
	m_basesAssaultedBy[Id]=-1;

	// remove the other spirit guide (if it exists) // burlex: shouldnt' happen
	if(m_spiritGuides[Id] != NULL)
	{
		RemoveSpiritGuide(m_spiritGuides[Id]);
		m_spiritGuides[Id]->Despawn(0,0);
	}

	// spawn the spirit guide for our faction
	m_spiritGuides[Id] = SpawnSpiritGuide(GraveyardLocations[Id][0], GraveyardLocations[Id][1], GraveyardLocations[Id][2], 0.0f, Team);
	AddSpiritGuide(m_spiritGuides[Id]);

	// send the chat message/sounds out
	PlaySoundToAll(Team ? SOUND_HORDE_SCORES : SOUND_ALLIANCE_SCORES);
	SendChatMessage(Team ? CHAT_MSG_BG_EVENT_HORDE : CHAT_MSG_BG_EVENT_ALLIANCE, 0, "The %s has taken the %s!", Team ? "Horde" : "Alliance", ControlPointNames[Id]);
	
	// update the overhead display on the clients (world states)
	m_capturedBases[Team]++;
	m_mapMgr->GetStateManager().UpdateWorldState(Team ? WORLDSTATE_AB_HORDE_CAPTUREBASE : WORLDSTATE_AB_ALLIANCE_CAPTUREBASE, m_capturedBases[Team]);

	// respawn the control point with the correct aura
	SpawnControlPoint(Id, Team ? AB_SPAWN_TYPE_HORDE_CONTROLLED : AB_SPAWN_TYPE_ALLIANCE_CONTROLLED);

	// update the map
	m_mapMgr->GetStateManager().UpdateWorldState(AssaultFields[Id][Team], 0);
	m_mapMgr->GetStateManager().UpdateWorldState(OwnedFields[Id][Team], 1);

	// resource update event. :)
	if(m_capturedBases[Team]==1)
	{
		// first
		sEventMgr.AddEvent(this,&ArathiBasin::EventUpdateResources, (uint32)Team, EVENT_AB_RESOURCES_UPDATE_TEAM_0+Team, ResourceUpdateIntervals[1], 0,
			EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
	else
	{
		// not first
		event_ModifyTime(EVENT_AB_RESOURCES_UPDATE_TEAM_0+Team, ResourceUpdateIntervals[m_capturedBases[Team]]);
	}
}

void ArathiBasin::AssaultControlPoint(Player * pPlayer, uint32 Id)
{
#if defined(BG_ANTI_CHEAT) && !defined(_DEBUG)
	if(!m_started)
	{
		SendChatMessage(CHAT_MSG_BG_EVENT_NEUTRAL, pPlayer->GetGUID(), "%s has been removed from the game for cheating.", pPlayer->GetName());
		pPlayer->SoftDisconnect();
		return;
	}
#endif

	bool isVirgin = false;

	uint32 Team = pPlayer->m_bgTeam;
	uint32 Owner;

	if(m_basesOwnedBy[Id]==-1 && m_basesAssaultedBy[Id]==-1)
	{
		isVirgin = true;
		// omgwtfbbq our flag is a virgin?
		m_mapMgr->GetStateManager().UpdateWorldState(NeutralFields[Id], 0);
	}

	if(m_basesOwnedBy[Id] != -1)
	{
		Owner = m_basesOwnedBy[Id];

		// set it to uncontrolled for now
		m_basesOwnedBy[Id] = -1;

		// this control point just got taken over by someone! oh noes!
		if( m_spiritGuides[Id] != NULL )
		{
			map<Creature*,set<uint32> >::iterator itr = m_resurrectMap.find(m_spiritGuides[Id]);
			if( itr != m_resurrectMap.end() )
			{
				for( set<uint32>::iterator it2 = itr->second.begin(); it2 != itr->second.end(); ++it2 )
				{
					Player* r_plr = m_mapMgr->GetPlayer( *it2 );
					if( r_plr != NULL && r_plr->isDead() )
					{
						HookHandleRepop( r_plr );
						QueueAtNearestSpiritGuide(r_plr, itr->first);
					}
				}
			}
			m_resurrectMap.erase( itr );
			m_spiritGuides[Id]->Despawn( 0, 0 );
			m_spiritGuides[Id] = NULL;
		}

		// detract one from the teams controlled points
		m_capturedBases[Owner] -= 1;
		m_mapMgr->GetStateManager().UpdateWorldState(Owner ? WORLDSTATE_AB_HORDE_CAPTUREBASE : WORLDSTATE_AB_ALLIANCE_CAPTUREBASE, m_capturedBases[Owner]);

		// reset the world states
		m_mapMgr->GetStateManager().UpdateWorldState(OwnedFields[Id][Owner], 0);

		// modify the resource update time period
		if(m_capturedBases[Owner]==0)
			this->event_RemoveEvents(EVENT_AB_RESOURCES_UPDATE_TEAM_0+Owner);
		else
			this->event_ModifyTime(EVENT_AB_RESOURCES_UPDATE_TEAM_0 + Owner, ResourceUpdateIntervals[m_capturedBases[Owner]]);
	}

	// Contested Flag, not ours, and is not virgin
	uint32 otherTeam = Team ? 0 : 1;
	if( !isVirgin && m_basesLastOwnedBy[Id] == Team && m_basesOwnedBy[Id] == -1 )
	{
		m_mapMgr->GetStateManager().UpdateWorldState(AssaultFields[Id][Team ? 0 : 1], 0);
		this->event_RemoveEvents(EVENT_AB_CAPTURE_CP_1 + Id);
		SendChatMessage(Team ? CHAT_MSG_BG_EVENT_HORDE : CHAT_MSG_BG_EVENT_ALLIANCE, pPlayer->GetGUID(), "$N has defended the %s!", ControlPointNames[Id]);
		m_basesAssaultedBy[Id] = Team;
		CaptureControlPoint( Id, Team );
		return;
	}

	// nigga stole my flag!
	if(m_basesAssaultedBy[Id] != -1)
	{
		Owner = m_basesAssaultedBy[Id];

		// woah! vehicle hijack!
		m_basesAssaultedBy[Id] = Team;
		m_mapMgr->GetStateManager().UpdateWorldState(AssaultFields[Id][Owner], 0);

		// make sure the event does not trigger
		sEventMgr.RemoveEvents(this, EVENT_AB_CAPTURE_CP_1 + Id);

		// no need to remove the spawn, SpawnControlPoint will do this.
	} 

	m_basesAssaultedBy[Id] = Team;

	// spawn the new control point gameobject
	SpawnControlPoint(Id, Team ? AB_SPAWN_TYPE_HORDE_ASSAULT : AB_SPAWN_TYPE_ALLIANCE_ASSAULT);

	// send out the chat message and sound
	SendChatMessage(Team ? CHAT_MSG_BG_EVENT_HORDE : CHAT_MSG_BG_EVENT_ALLIANCE, pPlayer->GetGUID(), "$N claims the %s! If left unchallenged, the %s will control it in 1 minute!", ControlPointNames[Id],
		Team ? "Horde" : "Alliance");

	// guessed
	PlaySoundToAll(Team ? SOUND_ALLIANCE_CAPTURE : SOUND_HORDE_CAPTURE);

	// update the client's map with the new assaulting field
	m_mapMgr->GetStateManager().UpdateWorldState(AssaultFields[Id][Team], 1);

	// create the 60 second event.
	sEventMgr.AddEvent(this, &ArathiBasin::CaptureControlPoint, Id, Team, EVENT_AB_CAPTURE_CP_1 + Id, 60000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

	// update players info
	pPlayer->m_bgScore.MiscData[BG_SCORE_AB_BASE_ASSAULTED]++;
	UpdatePvPData();
}

bool ArathiBasin::HookSlowLockOpen(GameObject * pGo, Player * pPlayer, Spell * pSpell)
{
	if( pPlayer->m_bgFlagIneligible )
		return false;

	// burlex todo: find a cleaner way to do this that doesnt waste memory.
	if(pGo->bannerslot >= 0 && pGo->bannerslot < AB_NUM_CONTROL_POINTS)
	{
		// TODO: anticheat here
		AssaultControlPoint(pPlayer,pGo->bannerslot);
		return true;
	}

	return false;
}

void ArathiBasin::HookGenerateLoot(Player *plr, Corpse *pCorpse)
{
	// add some money
	float gold = ((float(plr->getLevel()) / 2.5f)+1) * 100.0f;			// fix this later
	gold *= sWorld.getRate(RATE_MONEY);

	// set it
	pCorpse->m_loot.gold = float2int32(gold);
}
