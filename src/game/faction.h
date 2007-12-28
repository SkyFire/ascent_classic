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

#ifndef __FACTION_H
#define __FACTION_H

#include "Unit.h"

bool isHostile(Object* objA, Object* objB); // B is hostile for A?
bool isAttackable(Object* objA, Object* objB, bool CheckStealth = true); // A can attack B?
bool isCombatSupport(Object* objA, Object* objB); // B combat supports A?;
bool isAlliance(Object* objA); // A is alliance?

ASCENT_INLINE bool isFriendly(Object* objA, Object* objB)// B is friendly to A if its not hostile
{
	return !isHostile(objA, objB);
}

ASCENT_INLINE bool isSameFaction(Object* objA, Object* objB)
{
	return (objB->m_faction->Faction == objA->m_faction->Faction);
}

#endif
