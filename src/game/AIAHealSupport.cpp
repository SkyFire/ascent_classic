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

#ifdef USE_SPECIFIC_AIAGENTS

void AiAgentHealSupport::Init(Unit *un, AIType at, MovementType mt, Unit *owner)
{
	AIInterface::Init(un,at,mt,owner);//run the original init and we add our custom stuff too
	//later will load/use these from ai_agent
	SpellEntry *sp;
	sp = dbcSpell.LookupEntry( 2054 ); // heal rank 1
	m_HealSpells.push_back( sp ); 
	sp = dbcSpell.LookupEntry( 122 ); // frost nova 1
	m_DefendSpells.push_back( sp ); 
	sp = dbcSpell.LookupEntry( 2006 ); // resurrection 1
	revive_spell = sp;
}

void AiAgentHealSupport::_UpdateCombat(uint32 p_time)
{
	m_nextTarget = m_PetOwner;

printf("we have just overwritten updatecombat\n");

	if( !m_nextTarget )
		return; //oh noez, our master has abandoned us ! Where is te luv ?

printf("we do have a target\n");

	if( m_Unit->isCasting() )
		return; // we are already supporting someone ...get back later

printf("we are not casting\n");

	uint32 Time_Now = getMSTime();

	SpellCastTargets targets;
	m_castingSpell = NULL;

	//poor thing died. Res him
	if( !m_nextTarget->isAlive() 
		&& m_Unit->GetUInt32Value(UNIT_FIELD_POWER1) >= revive_spell->manaCost
		&& spell_cooldown_map[ revive_spell->Id ] < Time_Now
		)
	{
		m_castingSpell = revive_spell;
		setSpellTargets(m_castingSpell, m_PetOwner);
printf("master died, we are going to resurect him\n");
	}

	if ( m_PetOwner->GetHealthPct() < 100 )
	{
printf("master is injured, will try to heal him\n");
		uint32 now_health = m_PetOwner->GetUInt32Value(UNIT_FIELD_HEALTH);
		uint32 max_health = m_PetOwner->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
		uint32 health_to_heal = max_health - now_health;

		//get best spell to match our needs :D

// right now let's just use heal 1 :P
m_castingSpell = dbcSpell.LookupEntry( 2054 ); // heal rank 1
if(	!( spell_cooldown_map[ m_castingSpell->Id ] < Time_Now
	&& m_Unit->GetUInt32Value(UNIT_FIELD_POWER1) >= revive_spell->manaCost )
	)
	m_castingSpell = NULL;
else setSpellTargets(m_castingSpell, m_PetOwner);
	}

	if( !m_castingSpell )
		return; //sorry but we are out of juice

printf("we have a spell to cast\n");

	SpellCastTime *sd = dbcSpellCastTime.LookupEntry(m_castingSpell->CastingTimeIndex);

	//do not stop for instant casts
	if(GetCastTime(sd) != 0)
	{
		StopMovement(0);
printf("spell is not instant so we are going to stop movement \n");
	}

	float distance = m_Unit->GetDistanceSq(m_nextTarget);
	if(	distance <= m_castingSpell->base_range_or_radius_sqr || m_castingSpell->base_range_or_radius_sqr == 0 )
	{

printf("we are in range and going to cast spell \n");
		m_AIState = STATE_CASTING;
		
		Spell *nspell = new Spell(m_Unit, m_castingSpell, false, NULL);
		nspell->prepare(&targets);

		CastSpell(m_Unit, m_castingSpell, targets);

		if( m_castingSpell->RecoveryTime )
			spell_cooldown_map[ m_castingSpell->Id ] = Time_Now + m_castingSpell->RecoveryTime;
	}
	else // Target out of Range -> Run to it
	{
printf("we are going to move closer \n");
		m_moveRun = true;
		_CalcDestinationAndMove(m_nextTarget, sqrt( m_castingSpell->base_range_or_radius_sqr ) );
	}

// check if pets regenrate mana, If not then we should implement that too
}

#endif