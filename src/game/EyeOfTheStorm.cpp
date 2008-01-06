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

EyeOfTheStorm::EyeOfTheStorm(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t) : CBattleground(mgr,id,lgroup,t)
{

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

}

void EyeOfTheStorm::OnCreate()
{

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
	return LocationVector(0,0,0,0);
}

void EyeOfTheStorm::OnStart()
{

}
