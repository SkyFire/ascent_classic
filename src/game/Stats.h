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

#ifndef __STATS_H
#define __STATS_H

#include "Unit.h"
#include "UpdateMask.h"
#include "ItemInterface.h"

enum Stats
{
	STAT_STRENGTH,
	STAT_AGILITY,
	STAT_STAMINA,
	STAT_INTELLECT,
	STAT_SPIRIT,
};

uint32 getConColor(uint16 AttackerLvl, uint16 VictimLvl);
uint32 CalculateXpToGive(Unit *pVictim, Unit *pAttacker);
uint32 CalculateStat(uint16 level,double a3, double a2, double a1, double a0);
uint32 CalculateDamage(Unit *pAttacker, Unit *pVictim, uint32 damage_type, uint32 spellgroup, SpellEntry *ability);
uint32 GainStat(uint16 level, uint8 playerclass,uint8 Stat);
bool isEven (int num);

#endif
