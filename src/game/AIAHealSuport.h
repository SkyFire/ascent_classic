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

#ifndef WOWSERVER_AIAGENTHEALSUPPORT_H
#define WOWSERVER_AIAGENTHEALSUPPORT_H
#ifdef USE_SPECIFIC_AIAGENTS
class Object;
class Creature;
class Unit;
class Player;
class WorldSession;
class SpellCastTargets;

struct AI_Support_Spell
{
	SpellEntry  *m_spellinfo;
	uint32		cooldown_remain;
};

typedef std::map<uint32, uint32> CooldownMap;

class SERVER_DECL AiAgentHealSupport : public AIInterface
{

public:
	void Init(Unit *un, AIType at, MovementType mt, Unit *owner);
	void _UpdateCombat(uint32 p_time);

private:
	list<SpellEntry*>	m_HealSpells;		//used to take actions on target
	list<SpellEntry*>	m_HealGroupSpells;	//used to take actions on target
	list<SpellEntry*>	m_DefendSpells;		//in case this is not a combat unit it should try to escape battle using these spells
	list<SpellEntry*>	m_AugmentSpells;	//low priority spells	
	SpellEntry			*revive_spell;
	CooldownMap			spell_cooldown_map;
	uint32				DifficultyLevel; //spell values should scale up with the level of the support unit 
	SpellEntry			*m_castingSpell;
};

#endif
#endif
