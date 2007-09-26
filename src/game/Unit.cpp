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

Unit::Unit()
{

	m_attackTimer = 0;
	m_attackTimer_1 = 0;
	m_duelWield = false;

	m_state = 0;
	m_special_state = 0;
	m_deathState = ALIVE;
	m_currentSpell = NULL;
	m_meleespell = 0;
	m_addDmgOnce = 0;
	m_TotemSlots[0] = NULL;
	m_TotemSlots[1] = NULL;
	m_TotemSlots[2] = NULL;
	m_TotemSlots[3] = NULL;
	m_ObjectSlots[0] = 0;
	m_ObjectSlots[1] = 0;
	m_ObjectSlots[2] = 0;
	m_ObjectSlots[3] = 0;
	m_silenced = false;
	disarmed   = false;

	// Pet
	m_isPet = false;
	
	//DK:modifiers
	PctRegenModifier = 0;
	for (uint32 x=0;x<4;x++)
	{
		PctPowerRegenModifier[x] = 1;
	}
	m_speedModifier = 0;
	m_slowdown = 0;
	m_mountedspeedModifier=0;
	VampTchCaster=0;
	for(uint32 x=0;x<27;x++)
	{
		MechanicsDispels[x]=0;
		MechanicsResistancesPCT[x]=0;
	}

	//SM
	SM_CriticalChance=0;
	SM_FDur=0;//flat
	SM_PDur=0;//pct
	SM_FRadius=0;
	SM_FRange=0;
	SM_PCastTime=0;
	SM_FCastTime=0;
	SM_PCriticalDamage=0;
	SM_FDOT=0;
	SM_PDOT=0;
	SM_FEffectBonus=0;
	SM_PEffectBonus=0;
	SM_FDamageBonus=0;
	SM_PDamageBonus=0;
	SM_PDummy=0;
	SM_FDummy=0;
	SM_FResist=0;
	SM_PRange=0;//pct
	SM_PRadius=0;
	SM_PAPBonus=0;
	SM_PCost=0;
	SM_FCost=0;
	SM_FAdditionalTargets=0;
	SM_PJumpReduce=0;
	SM_FSpeedMod=0;
	SM_PNonInterrupt=0;
	SM_FPenalty=0;
	SM_PPenalty=0;
	SM_FCooldownTime = 0;
	SM_PCooldownTime = 0;
	SM_FChanceOfSuccess = 0;

	m_pacified = 0;
	m_interruptRegen = 0;
	m_resistChance = 0;
	m_powerRegenPCT = 0;
	RAPvModifier=0;
	APvModifier=0;
	stalkedby=0;

	m_invisible = false;
	m_extraattacks = 0;
	m_stunned = 0;
	m_manashieldamt=0;
	m_rooted = 0;
	m_triggerSpell = 0;
	m_triggerDamage = 0;
	m_canMove = 0;
	m_noInterrupt = 0;
	m_modlanguage = -1;
	
	critterPet = NULL;
	summonPet = NULL;

	m_useAI = false;
	for(uint32 x=0;x<10;x++)
	{
		dispels[x]=0;
		CreatureAttackPowerMod[x] = 0;
		CreatureRangedAttackPowerMod[x] = 0;
	}
	//REMIND:Update these if you make any changes
	CreatureAttackPowerMod[UNIT_TYPE_MISC] = 0;
	CreatureRangedAttackPowerMod[UNIT_TYPE_MISC] = 0;
	CreatureAttackPowerMod[11] = 0;
	CreatureRangedAttackPowerMod[11] = 0;

	m_stealthLevel = 0;
	m_stealthDetectBonus = 0;
	m_stealth = 0;
	m_sleep = 0;
	
	for(uint32 x=0;x<5;x++)
		BaseStats[x]=0;

	m_attackTimer = 0;
	m_H_regenTimer = 2000;
	m_P_regenTimer = 2000;
	m_P_I_regenTimer = 2000;


	//	if(GetTypeId() == TYPEID_PLAYER) //only player for now
	//		CalculateActualArmor();

	m_aiInterface = new AIInterface();
	m_aiInterface->Init(this, AITYPE_AGRO, MOVEMENTTYPE_NONE);

	m_emoteState = 0;
	m_oldEmote = 0;	
	
	BaseDamage[0]=0;
	BaseOffhandDamage[0]=0;
	BaseRangedDamage[0]=0;
	BaseDamage[1]=0;
	BaseOffhandDamage[1]=0;
	BaseRangedDamage[1]=0;

	m_attackTarget = 0;
	m_CombatUpdateTimer = 0;
	for(uint32 x=0;x<7;x++)
	{
		SchoolImmunityList[x] = 0;
		BaseResistance[x] = 0;
		HealDoneMod[x] = 0;
		HealDonePctMod[x] = 0;
		HealTakenMod[x] = 0;
		HealTakenPctMod[x] = 0;
		DamageTakenMod[x] = 0;
		DamageDoneModPCT[x]= 0;
		SchoolCastPrevent[x]=0;
		DamageTakenPctMod[x] = 1;
		SpellCritChanceSchool[x] = 0;
		PowerCostMod[x] = 0;
		PowerCostPctMod[x] = 0;
		AttackerSpellCritChanceMod[x]=0;
	}
	DamageTakenPctModOnHP = 1;
	RangedDamageTaken = 0;

	for(int i = 0; i < 5; i++)
	{
		m_detectRangeGUID[i] = 0;
		m_detectRangeMOD[i] = 0;
		InvisibilityDetectBonus[i] = 0;
	}
	//REMIND:Update these if you make any changes
	InvisibilityDetectBonus[INVISIBILTY_FLAG_TRAP] = 180;//MaxLevel*3
	InvisibilityDetectBonus[INVISIBILTY_FLAG_GHOSTS] = 0;

	trackStealth = false;
	modAttackTimeIncreasePCT = 0;

	m_threatModifyer = 0;
	m_generatedThreatModifyer = 0;
	memset(m_auras, 0, (MAX_AURAS+MAX_PASSIVE_AURAS)*sizeof(Aura*));
	
	// diminishing return stuff
	memset(m_diminishAuraCount, 0, 23);
	memset(m_diminishCount, 0, 23*2);
	memset(m_diminishTimer, 0, 23*2);
	memset(m_auraStackCount, 0, MAX_AURAS);
	m_diminishActive = false;
	dynObj = 0;
	pLastSpell = 0;
	m_flyspeedModifier = 0;
	bInvincible = false;
	m_redirectSpellPackets = 0;
	can_parry = false;
	bProcInUse = false;
	spellcritperc = 0;

	polySpell = 0;
	RangedDamageTaken = 0;
	m_procCounter = 0;
	m_invisibityFlag = 0;
	m_extrastriketargets = 0;
//	fearSpell = 0;
}

Unit::~Unit()
{  
	RemoveAllAuras();

	if(SM_CriticalChance != 0) delete [] SM_CriticalChance ;
	if(SM_FDur != 0) delete [] SM_FDur ;//flat
	if(SM_PDur != 0) delete [] SM_PDur ;//pct
	if(SM_FRadius != 0) delete [] SM_FRadius ;
	if(SM_FRange != 0) delete [] SM_FRange ;
	if(SM_PCastTime != 0) delete [] SM_PCastTime ;
	if(SM_FCastTime != 0) delete [] SM_FCastTime ;
	if(SM_PCriticalDamage != 0) delete [] SM_PCriticalDamage ;
	if(SM_FDOT != 0) delete [] SM_FDOT ;
	if(SM_PDOT != 0) delete [] SM_PDOT ;
	if(SM_PEffectBonus != 0) delete [] SM_PEffectBonus ;
    if(SM_FEffectBonus != 0) delete [] SM_FEffectBonus ;
	if(SM_FDamageBonus != 0) delete [] SM_FDamageBonus ;
	if(SM_PDamageBonus != 0) delete [] SM_PDamageBonus ;
	if(SM_PDummy != 0) delete [] SM_PDummy ;
	if(SM_FDummy != 0) delete [] SM_FDummy ;
	if(SM_FResist != 0) delete [] SM_FResist ;
	if(SM_PRange != 0) delete [] SM_PRange ;//pct
	if(SM_PRadius != 0) delete [] SM_PRadius ;
	if(SM_PAPBonus != 0) delete [] SM_PAPBonus ;
	if(SM_PCost != 0) delete [] SM_PCost ;
	if(SM_FCost != 0) delete [] SM_FCost ;
	if(SM_FAdditionalTargets != 0) delete [] SM_FAdditionalTargets ;
	if(SM_PJumpReduce != 0) delete [] SM_PJumpReduce ;
	if(SM_FSpeedMod != 0) delete [] SM_FSpeedMod ;
	if(SM_PNonInterrupt != 0) delete [] SM_PNonInterrupt ;
	if(SM_FPenalty != 0) delete [] SM_FPenalty ;
	if(SM_PPenalty != 0) delete [] SM_PPenalty ;
	if(SM_FCooldownTime != 0) delete [] SM_FCooldownTime ;
	if(SM_PCooldownTime != 0) delete [] SM_PCooldownTime ;
	if(SM_FChanceOfSuccess != 0) delete [] SM_FChanceOfSuccess ;

	delete m_aiInterface;

	/*for(int i = 0; i < 4; i++)
		if (m_ObjectSlots[i])
			delete m_ObjectSlots[i];*/

	if(m_currentSpell)
		m_currentSpell->cancel();
}

void Unit::Update( uint32 p_time )
{
	_UpdateSpells( p_time );

	if(m_attackers.size() == 0 && m_attackTarget == 0)
	{
		RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_ATTACK_ANIMATION);
		if(hasStateFlag(UF_ATTACKING)) clearStateFlag(UF_ATTACKING);
	}
	else
	{
		if (m_attackTarget==0)
		{
			RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_ATTACK_ANIMATION);
			if(hasStateFlag(UF_ATTACKING)) clearStateFlag(UF_ATTACKING);

			for(AttackerSet::iterator itr = m_attackers.begin(); itr != m_attackers.end(); ++itr)
			{
				Unit* pUnit = GetMapMgr()->GetUnit(*itr);
				if(pUnit && pUnit->isAlive())
				{
					SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_ATTACK_ANIMATION);
					if(!hasStateFlag(UF_ATTACKING)) addStateFlag(UF_ATTACKING);
					break;
				}
			}
		}
	}

	if(!isDead())
	{
        if(p_time >= m_H_regenTimer)
		    RegenerateHealth();	
	    else
		    m_H_regenTimer -= p_time;
	    //most of the times the 2 timers will be the same (except on spell casts)
	    if(p_time >= m_P_regenTimer)
		    RegeneratePower(false);	
	    else
		{
			if (p_time >= m_P_I_regenTimer&&this->IsPlayer())
				RegeneratePower(true);
			else
				m_P_I_regenTimer -= p_time;
		    m_P_regenTimer -= p_time;
		}

		if(m_aiInterface != NULL && m_useAI)
			m_aiInterface->Update(p_time);

		if(m_diminishActive)
		{
			uint32 count = 0;
			for(uint32 x = 0; x < 16; ++x)
			{
				// diminishing return stuff
				if(m_diminishTimer[x] && !m_diminishAuraCount[x])
				{
					if(p_time >= m_diminishTimer[x])
					{
						// resetting after 15 sec
						m_diminishTimer[x] = 0;
						m_diminishCount[x] = 0;
					}
					else
					{
						// reducing, still.
						m_diminishTimer[x] -= p_time;
						++count;
					}
				}
			}
			if(!count)
				m_diminishActive = false;
		}

/*		//if health changed since last time. Would be perfect if it would work for creatures too :)
		if(m_updateMask.GetBit(UNIT_FIELD_HEALTH))
			EventHealthChangeSinceLastUpdate();*/
	}
}

bool Unit::canReachWithAttack(Unit *pVictim)
{
//	float targetreach = pVictim->GetFloatValue(UNIT_FIELD_COMBATREACH);
	float selfreach = GetFloatValue(UNIT_FIELD_COMBATREACH);
	float targetradius = pVictim->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
	float selfradius = GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
	float targetscale = pVictim->GetFloatValue(OBJECT_FIELD_SCALE_X);
	float selfscale = GetFloatValue(OBJECT_FIELD_SCALE_X);

	if( GetMapId() != pVictim->GetMapId() )
		return false;
	float distance = sqrt(GetDistanceSq(pVictim));

	float attackreach = (((targetradius*targetscale) + selfreach) + ((pow(selfradius,2)*selfscale)+1.50));
	//formula adjustment for player side.
	if(this->IsPlayer())
	{
		if (attackreach <= 8 && attackreach >= 5 && targetradius >= 1.80f) attackreach = 11; //giant type units
		if (attackreach > 11) attackreach = 11; //distance limited to max 11 yards attack range //max attack distance
		if (attackreach < 5 ) attackreach = 5; //normal units with too small reach.
		//range can not be less than 5 yards - this is normal combat range, SCALE IS NOT SIZE
	}

	return (distance <= attackreach);
}

void Unit::GiveGroupXP(Unit *pVictim, Player *PlayerInGroup)
{
	if(!PlayerInGroup) 
		return;
	if(!pVictim) 
		return;
	if(!PlayerInGroup->InGroup()) 
		return;
	Group *pGroup = PlayerInGroup->GetGroup();
	uint32 xp;
	if(!pGroup) 
		return;

	//Get Highest Level Player, Calc Xp and give it to each group member
	Player *pHighLvlPlayer = NULL;
	Player *pGroupGuy = NULL;
	  int active_player_count=0;
	Player *active_player_list[MAX_GROUP_SIZE_RAID];//since group is small we can afford to do this ratehr then recheck again the whole active player set
	int total_level=0;
	float xp_mod = 1.0f;

/*	if(pGroup->GetGroupType() == GROUP_TYPE_RAID)
	{   //needs to change
		//Calc XP
		xp = CalculateXpToGive(pVictim, PlayerInGroup);
		xp /= pGroup->MemberCount();

		GroupMembersSet::iterator itr;
		for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		{
			for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			{
				if((*itr)->getLevel() < sWorld.LevelCap)
					(*itr)->GiveXP(xp, pVictim->GetGUID(), true);
			}
		}
	}
	else if(pGroup->GetGroupType() == GROUP_TYPE_PARTY) */
	//change on 2007 04 22 by Zack
	//we only take into count players that are near us, on same map
	GroupMembersSet::iterator itr;
	pGroup->Lock();
	for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++) {
		for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
		{
			pGroupGuy = itr->player;
			if( pGroupGuy && 
				pGroupGuy->isAlive() && 
//				PlayerInGroup->GetInstanceID()==pGroupGuy->GetInstanceID() &&
				pVictim->GetMapMgr() == pGroupGuy->GetMapMgr() && 
				pGroupGuy->GetDistanceSq(pVictim)<100*100
				)
			{
				active_player_list[active_player_count]=pGroupGuy;
				active_player_count++;
				total_level += pGroupGuy->getLevel();
				if(pHighLvlPlayer)
				{
					if(pGroupGuy->getLevel() > pHighLvlPlayer->getLevel())
						pHighLvlPlayer = pGroupGuy;
				}
				else 
					pHighLvlPlayer = pGroupGuy;
			}
		}
	}
	pGroup->Unlock();
	if(active_player_count<1) //killer is always close to the victim. This should never execute
	{
		if(PlayerInGroup == 0) PlayerInGroup = pGroup->GetLeader();
		xp = CalculateXpToGive(pVictim, PlayerInGroup);
		PlayerInGroup->GiveXP(xp, pVictim->GetGUID(), true);
	}
	else
	{
		if( pGroup->GetGroupType() == GROUP_TYPE_PARTY)
		{
			if(active_player_count==3)
				xp_mod=1.1666f;
			else if(active_player_count==4)
				xp_mod=1.3f;
			else if(active_player_count==5)
				xp_mod=1.4f;
			else xp_mod=1;//in case we have only 2 members ;)
		}
		else if(pGroup->GetGroupType() == GROUP_TYPE_RAID)
			xp_mod=0.5f;
		if(pHighLvlPlayer == 0) pHighLvlPlayer = pGroup->GetLeader();

		xp = CalculateXpToGive(pVictim, pHighLvlPlayer);
		//i'm not sure about this formula is correct or not. Maybe some brackets are wrong placed ?
		for(int i=0;i<active_player_count;i++)
			active_player_list[i]->GiveXP( float2int32(((xp*active_player_list[i]->getLevel()) / total_level)*xp_mod), pVictim->GetGUID(), true );
	}
		/* old code start before 2007 04 22
		GroupMembersSet::iterator itr;
		for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		{
			for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			{
				pGroupGuy = (*itr);
				if(pGroupGuy)
				{
					if(pHighLvlPlayer)
					{
						if(pGroupGuy->getLevel() > pHighLvlPlayer->getLevel())
						{
							pHighLvlPlayer = pGroupGuy;
						}
					}
					else
						pHighLvlPlayer = pGroupGuy;
				}
			}
		}

		//Calc XP
		xp = CalculateXpToGive(pVictim, pHighLvlPlayer);
		uint32 giveXP = 0;

		for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		{
			for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			{
				pGroupGuy = (*itr);
				giveXP = xp * pGroupGuy->getLevel() / (pHighLvlPlayer->getLevel() + pGroupGuy->getLevel());
				if(pGroupGuy->getLevel() < sWorld.LevelCap)
					pGroupGuy->GiveXP(giveXP, pVictim->GetGUID(), true);
			}
		}
	}*/
}

void Unit::HandleProc(uint32 flag, Unit* victim, SpellEntry* CastingSpell,uint32 dmg)
{
	++m_procCounter;
	bool can_delete = !bProcInUse;
	bProcInUse = true;

	std::list<uint32> remove;
	std::list<struct ProcTriggerSpell>::iterator itr,itr2;
	for( itr = m_procSpells.begin();itr != m_procSpells.end();)  // Proc Trigger Spells for Victim
	{
		itr2= itr;
		++itr;
		if(itr2->deleted)
		{
			if(can_delete) m_procSpells.erase(itr2);
			continue;
		}

		uint32 origId = itr2->origId;
		if(CastingSpell)
		{
			//this is to avoid spell proc on spellcast loop. We use dummy that is same for both spells
			if(CastingSpell->Id == itr2->origId || CastingSpell->Id == itr2->spellId)
			{
				//printf("WOULD CRASH HERE ON PROC: CastingId: %u, OrigId: %u, SpellId: %u\n", CastingSpell->Id, itr2->origId, itr2->spellId);
				continue;
			}
		}
		SpellEntry *ospinfo = sSpellStore.LookupEntry(origId );//no need to check if exists or not since we were not able to register this trigger if it would not exist :P
		//this requires some specific spell check,not yet implemented
		if(itr2->procFlags & flag)
		{
			uint32 spellId = itr2->spellId;
			if(itr2->procFlags & PROC_ON_CAST_SPECIFIC_SPELL)
			{

				if(!CastingSpell)
					continue;
				
				//this is wrong, dummy is too common to be based on this, we should use spellgroup or something
				SpellEntry *sp=sSpellStore.LookupEntry(spellId);
				if(sp->dummy != CastingSpell->dummy)
				{
					if(!ospinfo->School)
						continue;
					if(ospinfo->School != CastingSpell->School)
						continue;
					if(CastingSpell->EffectImplicitTargetA[0] == 1 || 
						CastingSpell->EffectImplicitTargetA[1] == 1 || 
						CastingSpell->EffectImplicitTargetA[2] == 1) //Prevents school based procs affecting caster when self buffing
						continue;
				}
				else
					if(sp->dummy == 1)
						continue;
			}			
			uint32 proc_Chance = itr2->procChance;
			SM_FIValue(SM_FChanceOfSuccess, (int32*)&proc_Chance, ospinfo->SpellGroupType);
			if(spellId && Rand(proc_Chance))
			{
				/* hmm whats a reasonable value here */
				if(m_procCounter > 40)
				{
					/* something has proceed over 10 times in a loop :/ dump the spellids to the crashlog, as the crashdump will most likely be useless. */
					OutputCrashLogLine("HandleProc %u SpellId %u (%s) %u", flag, spellId, sSpellStore.LookupString(sSpellStore.LookupEntry(spellId)->Name), m_procCounter);
					return;
				}

				//check if we can trigger due to time limitation
				if(ospinfo->proc_interval)
				{
					uint32 now_in_ms=getMSTime();
					if(itr2->LastTrigger+ospinfo->proc_interval>now_in_ms)
						continue; //we can't trigger it yet.
					itr2->LastTrigger = now_in_ms; // consider it triggered
				}
				//since we did not allow to remove auras like these with interrupt flag we have to remove them manually.
				if(itr2->procFlags & PROC_REMOVEONUSE)
					RemoveAura(origId);
				//these are player talents. Fuckem they pull the emu speed down 
				if(IsPlayer())
				{
					uint32 talentlevel=0;
					switch(origId)
					{
						//mace specialization
						case 12284:	{talentlevel=1;}break;
						case 12701:	{talentlevel=2;}break;
						case 12702:	{talentlevel=3;}break;
						case 12703:	{talentlevel=4;}break;
						case 12704:	{talentlevel=5;}break;
						//Unbridled Wrath
						case 12999:	{talentlevel=1;}break;
						case 13000:	{talentlevel=2;}break;
						case 13001:	{talentlevel=3;}break;
						case 13002:	{talentlevel=4;}break;
					}
					switch(spellId)
					{
						case 31616:
						{
							//yep, another special case: Nature's grace
							if(GetHealthPct()>30)
								continue;
						}break;
						case 5530:
						{
							//warrior mace specialization can trigger only when using maces
							Item *it;
							if(static_cast<Player*>(this)->GetItemInterface())
							{
								it = static_cast<Player*>(this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
								if(it && it->GetProto())
								{
									uint32 reqskill=GetSkillByProto(it->GetProto()->Class,it->GetProto()->SubClass);
									if(reqskill!=SKILL_MACES && reqskill!=SKILL_2H_MACES)
										continue;
								}
								else continue; //no weapon no joy
							}
							else continue; //no weapon no joy
							//let's recalc chance to cast since we have a full 100 all time on this one
							//how lame to get talentpointlevel for this spell :(
//							float chance=it->GetProto()->Delay*100*talentlevel/60000;
							float chance=it->GetProto()->Delay*talentlevel/600;
							if(!Rand(chance))
								continue;
						}break;
						case 4350:
						{
							//sword specialization
							if(static_cast<Player*>(this)->GetItemInterface())
							{
								Item *it;
								it = static_cast<Player*>(this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
								if(it && it->GetProto())
								{
									uint32 reqskill=GetSkillByProto(it->GetProto()->Class,it->GetProto()->SubClass);
									if(reqskill!=SKILL_SWORDS && reqskill!=SKILL_2H_SWORDS)
										continue;
								}
								else continue; //no weapon no joy
							}
							else continue; //no weapon no joy
						}break;
						case 12721:
						{
							//deep wound requires a melee weapon
							if(static_cast<Player*>(this)->GetItemInterface())
							{
								Item *it;
								it = static_cast<Player*>(this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
								if(it && it->GetProto())
								{
									//class 2 means weapons ;)
									if(it->GetProto()->Class!=2)
										continue;
								}
								else continue; //no weapon no joy
							}
							else continue; //no weapon no joy
						}break;
						//Unbridled Wrath
						case 12964:
						{
							//let's recalc chance to cast since we have a full 100 all time on this one
							Item *it;
							if(static_cast<Player*>(this)->GetItemInterface())
							{
								it = static_cast<Player*>(this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
								if(!(it && it->GetProto()))
									continue; //no weapon no joy
							}
							else continue; //no weapon no joy
//							float chance=it->GetProto()->Delay*100*talentlevel/60000;
							float chance=it->GetProto()->Delay*talentlevel/600;
							if(!Rand(chance))
								continue;
						}break;
						// Mage ignite talent only for fire dmg
						case 12654:
						{
							if(!CastingSpell)
								continue;
							if(CastingSpell->School!=SCHOOL_FIRE)
								continue;
						}break;
						//druid - Primal Fury
						case 37116:
						case 37117:
						{
							if (!this->IsPlayer())
								continue;
							Player* mPlayer = (Player*)this;
							if (!mPlayer->IsInFeralForm() || 
								(mPlayer->GetShapeShift() != FORM_CAT &&
								mPlayer->GetShapeShift() != FORM_BEAR &&
								mPlayer->GetShapeShift() != FORM_DIREBEAR))
								continue;
						}break;
						//rogue - blade twisting
						case 31125:
							{
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								//only trigger effect for specified spells
								if( CastingSpell->NameHash!=0xD3D32C05 && //backstab
									CastingSpell->NameHash!=0xE876878A && //sinister strike
									CastingSpell->NameHash!=0x92253E33 && //shiv
									CastingSpell->NameHash!=0xCCC8A100 ) //gouge
									continue;
							}break;
						//warlock - Improved Shadow Bolt
						case 17794:
						case 17798:
						case 17797:
						case 17799:
						case 17800:
							{
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								//only trigger effect for specified spells
								if( CastingSpell->NameHash!=0x7A7B6753)//shadow bolt								
									continue;
							}break;
						//warlock - Shadow Embrace
						case 32386:
						case 32388:
						case 32389:
						case 32390:
						case 32391:
							{
								if (!CastingSpell)
									continue;
								else
								{
									if (CastingSpell->NameHash!=0x2ADC25D7 &&// Corruption
										CastingSpell->NameHash!=0x34C208A2 &&//CoA
										CastingSpell->NameHash!=0x2979DFBA &&//Siphon Life
										CastingSpell->NameHash!=0xFD712ED2)  //SoC
										continue;
								}
							}break;
						//warlock - Aftermath
						case 18118:
							{
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								//only trigger effect for specified spells
								skilllinespell* skillability = objmgr.GetSpellSkill(CastingSpell->Id);
								if (!skillability)
									continue;
								if(skillability->skilline!=SKILL_DESTRUCTION)
									continue;
							}break;
						//warlock - Nether Protection
						case 30300:
							{
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								//only trigger effect for specified spells
								if( CastingSpell->School!=SCHOOL_FIRE &&
									CastingSpell->School!=SCHOOL_SHADOW)
									continue;
							}break;
						//warlock - Soul Leech
						//this whole spell should get rewriten. Uses bad formulas, bad trigger method, spell is rewriten ...
						case 30294:
							{
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								//only trigger effect for specified spells
								uint32 amount;
								switch(CastingSpell->NameHash)
								{
									case 0x7A7B6753: //Shadow Bolt
									case 0x9B1BA9E0: //Soul Fire
									case 0x2BC0AE00: //Incinerate
									case 0xD6269851: //Searing Pain
									case 0xB767620F: //Conflagrate
									{
										amount = CastingSpell->EffectBasePoints[0]+1;
									}break;
									case 0x1066956C: //Shadowburn
									{
										amount = CastingSpell->EffectBasePoints[1]+1;
									}break;
									default:
										amount=0;
								}
								if(!amount)
									continue;
								SpellEntry *spellInfo = sSpellStore.LookupEntry(spellId );
								if(!spellInfo)
									continue;
								Spell *spell = new Spell(this, spellInfo ,true, NULL);
								spell->SetUnitTarget(this);
								spell->Heal(amount*(ospinfo->EffectBasePoints[0]+1)/100);
								delete spell;
								continue;
							}break;
						//warlock - pyroclasm
						case 18093:
							{
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								//only trigger effect for specified spells
								if( CastingSpell->NameHash!=0xBA051C13 && //Rain of Fire
									CastingSpell->NameHash!=0x799839A4 && //Hellfire
									CastingSpell->NameHash!=0x9B1BA9E0 ) //Soul Fire
									continue;
							}break;
						//mage - Improved Scorch
						case 22959:
							{
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								//only trigger effect for specified spells
								if( CastingSpell->NameHash!=1828847009) //Rain of Fire
									continue;
							}break;
						//priest - Misery
						case 33200:
						case 33199:
						case 33198:
						case 33197:
						case 33196:
							{
								if (!CastingSpell)
									continue;
								else
								{
									if (CastingSpell->NameHash!=0xC3D9ACEB &&// Mind Flay
										CastingSpell->NameHash!=0x7335D7AF &&//SW:P
										CastingSpell->NameHash!=0x34D76539)  //SoC
										continue;
								}
							}break;
						//priest - Shadow Weaving
						case 15258:
							{
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								if(CastingSpell->School!=SCHOOL_SHADOW || !IsDamagingSpell(CastingSpell)) //we need damaging spells for this, so we suppose all shadow spells casted on target are dmging spells = Wrong
									continue;
							}break;
						//shaman - windfurry weapon
						case 8232:
						case 8235:
						case 10486:
						case 16362:
						case 25505:
							{
								if(!IsPlayer())
									continue;
								//!! The wierd thing is that we need the spell thet trigegred this enchant spell in order to output logs ..we are using oldspell info too 
								//we have to recalc the value of this spell
								SpellEntry *spellInfo = sSpellStore.LookupEntry(origId);
								uint32 AP_owerride=GetAP() + spellInfo->EffectBasePoints[0]+1;
								float dmg = static_cast<Player*>(this)->GetMainMeleeDamage(AP_owerride);
								SpellEntry *sp_for_the_logs = sSpellStore.LookupEntry(spellId);
								Strike(victim,MELEE,sp_for_the_logs,dmg,0,0,true);
								Strike(victim,MELEE,sp_for_the_logs,dmg,0,0,true);
								//nothing else to be done for this trigger
								continue;
							}break;
						//rogue - Ruthlessness
						case 14157:
							{
								//we need a finishing move for this 
								if(CastingSpell->buffType!=SPELL_TYPE_FINISHING_MOVE || victim==this)
									continue;
							}break;
						//rogue - Relentless Strikes
						case 14181:
							{
								int32 proc_Chance;
								//chance is based actually on combopoint count and not 100% always 
								if(CastingSpell->buffType==SPELL_TYPE_FINISHING_MOVE && IsPlayer())
									proc_Chance = static_cast<Player*>(this)->m_comboPoints*ospinfo->EffectBasePoints[1];
								else continue;
								if(!Rand(proc_Chance))
									continue;
							}break;
						//rogue - Initiative
						case 13977:
							{
								//we need a Ambush, Garrote, or Cheap Shot
								if (CastingSpell->NameHash!=3345919181 && //Cheap Shot
									CastingSpell->NameHash!=566770777 && //Ambush
									CastingSpell->NameHash!=891349384)  //Garrote
									continue;
							}break;
						//Blackout
						case 15269:
							{
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								if(CastingSpell->School!=SCHOOL_SHADOW && !IsDamagingSpell(CastingSpell))
									continue;
							}break;
						//warrior - improved berserker rage 
						case 23690:
						case 23691:
							{
								if(!HasActiveAura(18499))
									continue;
							}break;
/*						//paladin - illumination
						case 18350:
							{
								continue; //disabled until finished
								if(!CastingSpell)
									continue;//this should not ocur unless we made a fuckup somewhere
								//only trigger effect for specified spells
								if( CastingSpell->NameHash!=666 && //Holy light
									CastingSpell->NameHash!=666 && //Flash of light
									CastingSpell->NameHash!=666 ) //Holy shock
									continue;
							}break;*/
					}
				}
				if(spellId==22858 && isInBack(victim)) //retatliation needs target to be not in front. Can be casted by creatures too
					continue;
				SpellEntry *spellInfo = sSpellStore.LookupEntry(spellId );
				if(!spellInfo)
				{
					continue;
				}
				Spell *spell = new Spell(this, spellInfo ,true, NULL);
				//Spell *spell = new Spell(this,spellInfo,false,0,true,false);
				if(spellId==974||spellId==32593||spellId==32594) // Earth Shield handler
				{
					spell->pSpellId=itr2->spellId;
					spell->SpellEffectDummy(0);
					spell->ProcedOnSpell = CastingSpell;
					delete spell;
					continue;
				}
				SpellCastTargets targets;
				if(itr2->procFlags & PROC_TAGRGET_SELF)
					targets.m_unitTarget = GetGUID();
				else 
					targets.m_unitTarget = victim->GetGUID();
	
				spell->pSpellId=origId;
				spell->prepare(&targets);
			}//not always we have a spell to cast
		}
	}

	std::map<uint32,struct SpellCharge>::iterator iter,iter2;
	iter=m_chargeSpells.begin();
	while(iter!= m_chargeSpells.end())
	{
		iter2=iter++;
		if(iter2->second.count)
		{
			if((iter2->second.ProcFlag&flag))
			{
				//Fixes for spells that dont lose charges when dmg is absorbd
				if(iter2->second.ProcFlag==680&&dmg==0) continue;
				if(CastingSpell)
				{

					SpellCastTime *sd = sCastTime.LookupEntry(CastingSpell->CastingTimeIndex);
					if(!sd) continue; // this shouldnt happen though :P
					switch(iter2->second.spellId)
					{
					case 12043:
						{
							//Presence of Mind and Nature's Swiftness should only get removed
							//when a non-instant and bellow 10 sec. Also must be nature :>
//							if(!sd->CastTime||sd->CastTime>10000) continue;
							if(sd->CastTime==0)
								continue;
						}break;
					case 17116: //Shaman - Nature's Swiftness
					case 16188:
						{
//							if(CastingSpell->School!=SCHOOL_NATURE||(!sd->CastTime||sd->CastTime>10000)) continue;
							if(CastingSpell->School!=SCHOOL_NATURE||(sd->CastTime==0)) continue;
						}break;
					case 16166:
						{
							if(!(CastingSpell->School==SCHOOL_FIRE||CastingSpell->School==SCHOOL_FROST||CastingSpell->School==SCHOOL_NATURE))
								continue;
						}break;
					case 14177: //cold blood will get removed on offensive spell
						{
							if(victim==this || isFriendly(this, victim))
								continue;
						}break;
					}
				}
				if(iter2->second.lastproc!=0)
				{
					if(iter2->second.procdiff>3000)
					{
						--(iter2->second.count);
						iter2->second.FromProc=true;
						RemoveAura(iter2->second.spellId);
						iter2->second.FromProc=false;
					}
				}
				else
				{
					--(iter2->second.count);
					iter2->second.FromProc=true;
					this->RemoveAura(iter2->second.spellId);
					iter2->second.FromProc=false;
				}
			}
		}
		if(!iter2->second.count)
		{
			m_chargeSpells.erase(iter2);
		}
	}
	if(can_delete)
		bProcInUse = false;
}

void Unit::HandleProcDmgShield(uint32 flag, Unit* victim)
{
	//make sure we do not loop dmg procs
	if(this==victim || !victim)
		return;
	//charges are already removed in handleproc
	WorldPacket data(24);
	std::list<DamageProc>::iterator i;
	std::list<DamageProc>::iterator i2;
	for(i = victim->m_damageShields.begin();i != victim->m_damageShields.end();)     // Deal Damage to Attacker
	{
		i2 = i++; //we should not proc on proc.. not get here again.. not needed.Better safe then sorry.
		if(	(flag & (*i2).m_flags) )
		{
			if(PROC_MISC & (*i2).m_flags)
			{
				data.Initialize(SMSG_SPELLDAMAGESHIELD);
				data << victim->GetGUID();
				data << this->GetGUID();
				data << (*i2).m_damage;
				data << (*i2).m_school;
				SendMessageToSet(&data,true);
				victim->DealDamage(this,(*i2).m_damage,0,0,(*i2).m_spellId);
			}
			else
			{
				SpellEntry	*ability=sSpellStore.LookupEntry((*i2).m_spellId);
//				victim->Strike(this,(*i2).m_school,ability,0,0,(*i2).m_damage, true);
				victim->Strike(this,RANGED,ability,0,0,(*i2).m_damage, true);
			}
		}
	}
}

/*
void Unit::HandleProcSpellOnSpell(Unit* Victim,uint32 damage,bool critical)
{
}
*/
bool Unit::isCasting()
{
	return (m_currentSpell != NULL);
}

void Unit::RegenerateHealth()
{
	m_H_regenTimer = 2000;//set next regen time 

	if (!isAlive())
		return;

	// player regen
	if(this->IsPlayer())
	{
		// These only NOT in combat
		if(!static_cast<Player*>(this)->isInCombat())
		{
			static_cast<Player*>(this)->RegenerateHealth(false);
		}
		else
			static_cast<Player*>(this)->RegenerateHealth(true);
	}
	else
	{
		// Only regen health out of combat
		if(!isInCombat())
			static_cast<Creature*>(this)->RegenerateHealth();
	}
}

void Unit::RegeneratePower(bool isinterrupted)
{
        // This is only 2000 IF the power is not rage
    m_P_regenTimer = 2000;//set next regen time 
	m_P_I_regenTimer = 2000;//set next interrupted regen time

	if (!isAlive())
		return;

	// player regen
	if(this->IsPlayer())
	{
		uint32 powertype = GetPowerType();
		float RegenPct = 1.0f;
		switch(powertype)
		{
		case POWER_TYPE_MANA:
			if (isinterrupted)
				RegenPct = static_cast<Player*>(this)->m_ModInterrMRegenPCT/100.0f;
			static_cast<Player*>(this)->RegenerateMana(RegenPct);
			break;
		case POWER_TYPE_ENERGY:
			if (isinterrupted)
				RegenPct = 0.0f;
			static_cast<Player*>(this)->RegenerateEnergy(RegenPct);
			break;
		}
		
		/*

		There is a problem here for druids.
		Druids when shapeshifted into bear have 2 power with different regen timers
		a) Mana (which regenerates while shapeshifted
		b) Rage

		Mana has a regen timer of 2 seconds
		Rage has a regen timer of 3 seconds

		I think the only viable way of fixing this is to have 2 different timers
		to check each individual power.

		Atm, mana is being regen at 3 seconds while shapeshifted...

		*/


		// druids regen mana when shapeshifted
		if(getClass() == DRUID && powertype != POWER_TYPE_MANA)
			static_cast<Player*>(this)->RegenerateMana(RegenPct);

		// These only NOT in combat
		if(!static_cast<Player*>(this)->isInCombat())
		{
	        // Rage timer is 3 seconds not 2
	        if(powertype == POWER_TYPE_RAGE)
			{
		        m_P_regenTimer = 3000;
				m_P_I_regenTimer = 3000;
		        static_cast<Player*>(this)->LooseRage();
			}
		}
	}
	else
	{
		uint32 powertype = GetPowerType();
		switch(powertype)
		{
		case POWER_TYPE_MANA:
			static_cast<Creature*>(this)->RegenerateMana();
			break;
		case POWER_TYPE_FOCUS:
			static_cast<Creature*>(this)->RegenerateFocus();
			break;
		}
	}
}

void Unit::CalculateResistanceReduction(Unit *pVictim,dealdamage * dmg)
{
	float AverageResistance = 0.0f;

	if((*dmg).damage_type == 0)//physical
	{		
//		double Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)+400+(85*getLevel()));
		//dmg reduction formula from xinef
		double Reduction = 0;
		if (getLevel() < 60) Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)+400+(85*getLevel()));
		else if (getLevel() > 59 && getLevel() < 70) Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)-22167.5+(467.5*getLevel()));
		//
		else Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)+10557.5);
		if(Reduction > 0.75f) Reduction = 0.75f;
		else if(Reduction < 0) Reduction = 0;
		if(Reduction) dmg[0].resisted_damage = (uint32)(dmg[0].full_damage*Reduction);	  // no multiply by 0
	}
	else
	{
		// applying resistance to other type of damage 
		AverageResistance = ((float)pVictim->GetResistance( (*dmg).damage_type)- PowerCostPctMod[(*dmg).damage_type]) / (float)(getLevel() * 5) * 0.75f;
		  if(AverageResistance > 0.75f)
			AverageResistance = 0.75f;
		if(AverageResistance>0)
			(*dmg).resisted_damage = (uint32)(((*dmg).full_damage)*AverageResistance);
		else 
			(*dmg).resisted_damage=0; 
	}
	// Checks for random bugs on spells
	if ( (*dmg).full_damage > 10000 && GetTypeId() == TYPEID_PLAYER && ((Player*)this)->GetSession()->GetPermissionCount() == 0) // hits higher then 5.5k must be bugged
	{
		sCheatLog.writefromsession(static_cast<Player*>(this)->GetSession(),"some how caused %u damage using spell %u",(*dmg).full_damage,GetCurrentSpell());
		(*dmg).resisted_damage = (*dmg).full_damage;
		sChatHandler.RedSystemMessage(static_cast<Player*>(this)->GetSession(),"Your actions have been logged, please tell a GM how to re-produce this damage bug or risk getting banned.");
	}
	//sLog.outDebug("calc resistance - damage: %d , dmg type: %d , dmg abs: %d\n",*damage,damage_type,*dmgabs);
}

void Unit::Strike(Unit *pVictim, uint32 damage_type, SpellEntry *ability, int32 add_damage, int32 pct_dmg_mod, uint32 exclusive_damage, bool disable_proc)
{
//==========================================================================================
//==============================Unacceptable Cases Processing===============================
//==========================================================================================
	if (!pVictim->isAlive() || !isAlive()  || IsStunned() || IsPacified())
		return;
	if(!isInFront(pVictim))
		if(IsPlayer())
		{
			static_cast<Player*>(this)->GetSession()->OutPacket(SMSG_ATTACKSWING_BADFACING);
			return;
		}
//==========================================================================================
//==============================Variables Initialization====================================
//==========================================================================================
	dealdamage dmg			  = {0,0,0};
	
	Item * it = NULL;
	
	float hitchance          = 0.0f;
	float dodge				 = 0.0f;
	float parry				 = 0.0f;
	float glanc              = 0.0f;
	float block				 = 0.0f;
	float crit				 = 0.0f;
	float crush              = 0.0f;

	uint32 targetEvent		 = 0;
	uint32 hit_status		 = 0;

	uint32 blocked_damage	 = 0;
	int32  realdamage		 = 0;

	uint32 vstate			 = 1;
	uint32 aproc			 = 0;
	uint32 vproc			 = 0;
	   
	float hitmodifier		 = 0;
	int32 self_skill;
	int32 victim_skill;
	uint32 SubClassSkill	 = SKILL_UNARMED;

	bool backAttack			 = isInBack( pVictim );
	uint32 vskill            = 0;
	
//==========================================================================================
//==============================Victim Skill Base Calculation===============================
//==========================================================================================
	if(pVictim->IsPlayer())
	{
		vskill = ((Player*)pVictim)->_GetSkillLineCurrent(SKILL_DEFENSE);
		if((damage_type != RANGED) && !backAttack)
		{
//--------------------------------block chance----------------------------------------------
				block = pVictim->GetFloatValue(PLAYER_BLOCK_PERCENTAGE); //shield check already done in Update chances
//--------------------------------dodge chance----------------------------------------------
			if (pVictim->m_stunned<=0) 
			{
				dodge = pVictim->GetFloatValue(PLAYER_DODGE_PERCENTAGE);
			}
//--------------------------------parry chance----------------------------------------------
			if(pVictim->can_parry && !disarmed)
			{
				parry = pVictim->GetFloatValue(PLAYER_PARRY_PERCENTAGE);
			}
		}
		victim_skill = float2int32(vskill+((Player*)pVictim)->CalcRating(1));
	}
//--------------------------------mob defensive chances-------------------------------------
	else
	{
		if(damage_type != RANGED && !backAttack)
			dodge = pVictim->GetUInt32Value(UNIT_FIELD_STAT1) / 14.5f; // what is this value?
		victim_skill = pVictim->getLevel() * 5;
		if(pVictim->m_objectTypeId == TYPEID_UNIT) 
		{ 
			Creature * c = (Creature*)(pVictim);
			if (c&&c->GetCreatureName()&&c->GetCreatureName()->Rank == 3) //boss
			{
				victim_skill = max(victim_skill,((int32)this->getLevel()+3)*5); //used max to avoid situation when lowlvl hits boss.
			}
		} 
	}
//==========================================================================================
//==============================Attacker Skill Base Calculation=============================
//==========================================================================================
	if(this->IsPlayer())
	{	  
		self_skill=0;
		Player *pr = ((Player*)this);
		hitmodifier = (uint32)pr->GetHitFromMeleeSpell();  
		
		switch(damage_type)
		{
		case MELEE://melee,
			it = (disarmed) ? NULL : pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
			hitmodifier+=pr->CalcRating(5);
			self_skill = float2int32(pr->CalcRating(20));
			break;
		case DUALWIELD://dual wield
			it = (disarmed) ? NULL : pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
			hitmodifier+=pr->CalcRating(5);
			self_skill = float2int32(pr->CalcRating(21));
			hit_status |= HITSTATUS_DUALWIELD;//animation
			break;
		case RANGED: //ranged
			it = (disarmed) ? NULL : pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
			hitmodifier+=pr->CalcRating(6);
			self_skill = float2int32(pr->CalcRating(0));
			break;
		}

		if (it && it->GetProto())
			SubClassSkill = GetSkillByProto(it->GetProto()->Class,it->GetProto()->SubClass);
		else
			SubClassSkill = SKILL_UNARMED;

		if (SubClassSkill==SKILL_FIST_WEAPONS) 
			SubClassSkill = SKILL_UNARMED;

		//chances in feral form don't depend on weapon skill
		if (static_cast<Player*>(this)->IsInFeralForm()) 
		{
			uint8 form = static_cast<Player*>(this)->GetShapeShift();
			if (form == FORM_CAT || form == FORM_BEAR || form == FORM_DIREBEAR)
			{
				SubClassSkill = SKILL_FERAL_COMBAT;
				// Adjust skill for Level * 5 for Feral Combat
				self_skill += pr->getLevel() * 5;
			}
		}

		self_skill += pr->_GetSkillLineCurrent(SubClassSkill);
		crit = GetFloatValue(PLAYER_CRIT_PERCENTAGE);
	}
	else
	{
		self_skill = this->getLevel() * 5;
		if(m_objectTypeId == TYPEID_UNIT) 
		{ 
			Creature * c = (Creature*)(this);
			if (c&&c->GetCreatureName()&&c->GetCreatureName()->Rank == 3) //boss
				self_skill = max(self_skill,((int32)pVictim->getLevel()+3)*5);//used max to avoid situation when lowlvl hits boss.
		} 
		crit = 5.0f; //will be modified later
	}
//==========================================================================================
//==============================Special Chances Base Calculation============================
//==========================================================================================
//<THE SHIT> to avoid Linux bug. 
float diffVcapped = (float)self_skill;
if (int32(pVictim->getLevel()*5)>victim_skill)
	diffVcapped -=(float)victim_skill;
else
	diffVcapped -=(float)(pVictim->getLevel()*5);

float diffAcapped = (float)victim_skill;
if (int32(this->getLevel()*5)>self_skill)
	diffAcapped -=(float)self_skill;
else
	diffAcapped -=(float)(this->getLevel()*5);
//<SHIT END>

//--------------------------------crushing blow chance--------------------------------------
	if(pVictim->IsPlayer()&&!this->IsPlayer()&&!ability)
	{
		if (diffVcapped>=15.0f)
			crush = -15.0f+2.0f*diffVcapped; 
		else 
			crush = 0.0f;
	}
//--------------------------------glancing blow chance--------------------------------------
	if (this->IsPlayer()&&!pVictim->IsPlayer()&&!ability)
	{
		glanc = 10.0f + diffAcapped;
		if (glanc<0)
			glanc = 0.0f;
	}
//==========================================================================================
//==============================Advanced Chances Modifications==============================
//==========================================================================================
//--------------------------------by talents------------------------------------------------
	if(pVictim->IsPlayer())
	{
		if((damage_type != RANGED))
		{
			crit += static_cast<Player*>(pVictim)->res_M_crit_get();
			hitmodifier += static_cast<Player*>(pVictim)->m_resist_hit[0];
		}
		else 
		{
			crit += static_cast<Player*>(pVictim)->res_R_crit_get(); //this could be ability but in that case we overwrite the value
			hitmodifier += static_cast<Player*>(pVictim)->m_resist_hit[1];
		}
	}
//--------------------------------by victim state-------------------------------------------
	if (pVictim->IsPlayer()&&pVictim->GetStandState()) //every not standing state is >0
	{
		hitchance = 100.0f;
		dodge = 0.0f;
		parry = 0.0f;
		block = 0.0f;
		crush = 0.0f;
		crit = 100.0f;
	}
//--------------------------------by damage type and by weapon type-------------------------
	if (damage_type==RANGED) 
	{
		dodge=0.0f;
		parry=0.0f;
		glanc=0.0f;
	}
	else
		if (this->IsPlayer())
		{
			it = ((Player*)this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
			if(it && it->GetProto()->InventoryType==INVTYPE_WEAPON && !ability)//dualwield to-hit penalty
				hitmodifier -= 19.0f;
			else
			{
				it = ((Player*)this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
				if(it && it->GetProto()->InventoryType==INVTYPE_2HWEAPON)//2 handed weapon to-hit penalty
  					hitmodifier -= 4.0f;
			}
		}
//--------------------------------by skill difference---------------------------------------
	float vsk = (float)self_skill-(float)victim_skill;
	dodge = max(0.0,dodge-vsk*0.04);
	if (parry)
		parry = max(0.0,parry-vsk*0.04);
	if (block)
		block = max(0.0,block-vsk*0.04);

	crit += pVictim->IsPlayer() ? vsk*0.04 : min(vsk*0.2,0.0) ; 
	crit -= pVictim->IsPlayer() ? static_cast<Player*>(pVictim)->CalcRating(14) : 0.0f;
	if (crit<0) crit=0.0f;

	if (vsk>0)
			hitchance = max(hitchance,95.0f+vsk*0.02+hitmodifier);
	else
	{
		if (pVictim->IsPlayer())
			hitchance = max(hitchance,95.0f+vsk*0.1+hitmodifier); //wowwiki multiplier - 0.04 but i think 0.1 more balanced
		else
			hitchance = max(hitchance,100.0f+vsk*0.6+hitmodifier); //not wowwiki but more balanced
	}

	if(ability && ability->SpellGroupType)
	{
		SM_FFValue(SM_CriticalChance,&crit,ability->SpellGroupType);
		SM_FFValue(SM_FResist,&hitchance,ability->SpellGroupType);
	}
//==========================================================================================
//==============================One Roll Processing=========================================
//==========================================================================================
//--------------------------------cummulative chances generation----------------------------
	float chances[7];
	chances[0]=max(0.0f,100.0f-hitchance);
	chances[1]=chances[0]+dodge;
	chances[2]=chances[1]+parry;
	chances[3]=chances[2]+glanc;
	chances[4]=chances[3]+block;
	chances[5]=chances[4]+crit;
	chances[6]=chances[5]+crush;
//--------------------------------roll------------------------------------------------------
	float Roll = (float)sRand.rand(100);
	uint32 r = 0;
	while (r<7&&Roll>chances[r])
	{
		r++;
	}
//--------------------------------postroll processing---------------------------------------
	uint32 abs = 0;
	switch(r)
	{ 
//--------------------------------miss------------------------------------------------------
	case 0:
		hit_status |= HITSTATUS_MISS;

		// dirty ai agro fix
		// make mob aggro when u miss it
		// grep: dirty fix for this
		if(pVictim->GetTypeId() == TYPEID_UNIT)
			pVictim->GetAIInterface()->AttackReaction(this, 1, 0);
		break;
//--------------------------------dodge-----------------------------------------------------
	case 1:
		CALL_SCRIPT_EVENT(pVictim, OnTargetDodged)(this);
		CALL_SCRIPT_EVENT(this, OnDodged)(this);
		targetEvent = 1;
		vstate = DODGE;
		vproc |= PROC_ON_DODGE_VICTIM;
		pVictim->Emote(EMOTE_ONESHOT_PARRYUNARMED);			// Animation
		if(pVictim->IsPlayer())
		{
			pVictim->SetFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_DODGE_BLOCK);	//SB@L: Enables spells requiring dodge
			if(!sEventMgr.HasEvent(pVictim,EVENT_DODGE_BLOCK_FLAG_EXPIRE))
				sEventMgr.AddEvent(pVictim,&Unit::EventAurastateExpire,(uint32)AURASTATE_FLAG_DODGE_BLOCK,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000,1,0);
			else sEventMgr.ModifyEventTimeLeft(pVictim,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000,0);
		}
		break;
//--------------------------------parry-----------------------------------------------------
	case 2:
		CALL_SCRIPT_EVENT(pVictim, OnTargetParried)(this);
		CALL_SCRIPT_EVENT(this, OnParried)(this);
		targetEvent = 3;
		vstate = PARRY;
		pVictim->Emote(EMOTE_ONESHOT_PARRYUNARMED);			// Animation
		if(pVictim->IsPlayer())
		{
			pVictim->SetFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_PARRY);	//SB@L: Enables spells requiring parry
			if(!sEventMgr.HasEvent(pVictim,EVENT_PARRY_FLAG_EXPIRE))
				sEventMgr.AddEvent(pVictim,&Unit::EventAurastateExpire,(uint32)AURASTATE_FLAG_PARRY,EVENT_PARRY_FLAG_EXPIRE,5000,1,0);
			else 
				sEventMgr.ModifyEventTimeLeft(pVictim,EVENT_PARRY_FLAG_EXPIRE,5000);
			if( ((Player*)pVictim)->getClass()==1 || ((Player*)pVictim)->getClass()==4 )//warriors for 'revenge' and rogues for 'riposte'
			{
				pVictim->SetFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_DODGE_BLOCK);	//SB@L: Enables spells requiring dodge
				if(!sEventMgr.HasEvent(pVictim,EVENT_DODGE_BLOCK_FLAG_EXPIRE))
					sEventMgr.AddEvent(pVictim,&Unit::EventAurastateExpire,(uint32)AURASTATE_FLAG_DODGE_BLOCK,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000,1,0);
				else 
					sEventMgr.ModifyEventTimeLeft(pVictim,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000);
			}
		}
		break;
//--------------------------------not miss,dodge or parry-----------------------------------
	default:
		hit_status |= HITSTATUS_HITANIMATION;//hit animation on victim
		if(pVictim->SchoolImmunityList[0])
			vstate = IMMUNE;		
		else
		{
//--------------------------------state proc initialization---------------------------------
			vproc |= PROC_ON_ANY_DAMAGE_VICTIM;			
			if(damage_type != RANGED)
			{
				aproc |= PROC_ON_MELEE_ATTACK;
				vproc |= PROC_ON_MELEE_ATTACK_VICTIM;
			}
			else
			{
				aproc |= PROC_ON_RANGED_ATTACK;
				vproc |= PROC_ON_RANGED_ATTACK_VICTIM;
				if(ability && ability->Id==3018 && IsPlayer() && getClass()==HUNTER)
					aproc |= PROC_ON_AUTO_SHOT_HIT;
			}
//--------------------------------base damage calculation-----------------------------------
			if(exclusive_damage)
				dmg.full_damage = exclusive_damage;
			else
			{
				if(damage_type == MELEE && ability)
					dmg.full_damage = CalculateDamage(this, pVictim, damage_type, ability->SpellGroupType, ability);
				else			
					dmg.full_damage = CalculateDamage(this, pVictim, damage_type, 0, ability);
			}

			if(ability && ability->SpellGroupType)
			{	
				SM_FIValue(((Unit*)this)->SM_FDamageBonus,&dmg.full_damage,ability->SpellGroupType);
				SM_PIValue(((Unit*)this)->SM_PDamageBonus,&dmg.full_damage,ability->SpellGroupType);
			}

			dmg.full_damage += pVictim->DamageTakenMod[0]+add_damage;
			if (dmg.damage_type==RANGED)
			{
				dmg.full_damage+=pVictim->RangedDamageTaken;
			}
					
			float summaryPCTmod = pVictim->DamageTakenPctMod[0]+this->DamageDoneModPCT[0];
			if (pct_dmg_mod)
				summaryPCTmod += pct_dmg_mod/100.0f - 1;
			if (dmg.damage_type == RANGED)
			{
				summaryPCTmod+=pVictim->RangedDamageTakenPct/100;	
			}

			dmg.full_damage = (dmg.full_damage < 0) ? 0 : float2int32(dmg.full_damage*summaryPCTmod);

			if(dmg.full_damage < 0)
				dmg.full_damage = 0;
//--------------------------------check for special hits------------------------------------
			switch(r)
			{
//--------------------------------glancing blow---------------------------------------------
			case 3:
				{
					float low_dmg_mod = 1.5 - (0.05 * diffAcapped);
					if (this->getClass() == MAGE || this->getClass() == PRIEST || this->getClass() == WARLOCK) //casters = additional penalty.
					{
						low_dmg_mod -= 0.7f;
					}
					if (low_dmg_mod<0.01)
						low_dmg_mod = 0.01f;
					if (low_dmg_mod>0.91)
						low_dmg_mod = 0.91f;
					float high_dmg_mod = 1.2 - (0.03 * diffAcapped);
					if (this->getClass() == MAGE || this->getClass() == PRIEST || this->getClass() == WARLOCK) //casters = additional penalty.
					{
						high_dmg_mod -= 0.3f;
					}
					if (high_dmg_mod>0.99)
						high_dmg_mod = 0.99f;
					if (high_dmg_mod<0.2)
						high_dmg_mod = 0.2f;

					float damage_reduction = (high_dmg_mod + low_dmg_mod) / 2;
					if(damage_reduction > 0)
					{
							dmg.full_damage = (damage_reduction * dmg.full_damage);
					}
					hit_status |= HITSTATUS_GLANCING;
				}
				break;
//--------------------------------block-----------------------------------------------------
			case 4:
				{
					Item * shield = ((Player*)pVictim)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
					if(shield)
					{
						targetEvent = 2;
						pVictim->Emote(EMOTE_ONESHOT_PARRYSHIELD);// Animation
						blocked_damage = uint32((shield->GetProto()->Block + ((Player*)pVictim)->m_modblockvalue)*(1.0+((Player*)pVictim)->GetBlockFromSpell()/100)+pVictim->GetUInt32Value(UNIT_FIELD_STAT0)/20);

						if(dmg.full_damage <= (int32)blocked_damage)
						{
							CALL_SCRIPT_EVENT(pVictim, OnTargetBlocked)(this, blocked_damage);
							CALL_SCRIPT_EVENT(this, OnBlocked)(pVictim, blocked_damage);
							vstate = BLOCK;
							vproc |= PROC_ON_BLOCK_VICTIM;
						}
						if(pVictim->IsPlayer())//not necessary now but we'll have blocking mobs in future
						{                                                          
							pVictim->SetFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_DODGE_BLOCK);	//SB@L: Enables spells requiring dodge
							if(!sEventMgr.HasEvent(pVictim,EVENT_DODGE_BLOCK_FLAG_EXPIRE))
								sEventMgr.AddEvent(pVictim,&Unit::EventAurastateExpire,(uint32)AURASTATE_FLAG_DODGE_BLOCK,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000,1,0);
							else 
								sEventMgr.ModifyEventTimeLeft(pVictim,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000);
						}
					}
				}
				break;
//--------------------------------critical hit----------------------------------------------
			case 5:
				{
					hit_status |= HITSTATUS_CRICTICAL;
					int32 dmgbonus = dmg.full_damage;
					if(ability && ability->SpellGroupType)
						SM_FIValue(SM_PCriticalDamage,&dmgbonus,ability->SpellGroupType);
					dmg.full_damage += dmgbonus;
					if(IsPlayer())
					{
						if(damage_type != RANGED && !ability)
						{
							float critextra=static_cast<Player*>(this)->m_modphyscritdmgPCT;
							dmg.full_damage += int32((dmg.full_damage*critextra/100.0f));
						}
						if(!pVictim->IsPlayer())
							dmg.full_damage += float2int32(dmg.full_damage*static_cast<Player*>(this)->IncreaseCricticalByTypePCT[((Creature*)pVictim)->GetCreatureName() ? ((Creature*)pVictim)->GetCreatureName()->Type : 0]);
					}
					if (pVictim->IsPlayer())
					{
//						dmg.full_damage=float2int32(float(dmg.full_damage)*(1.0f-2.0f*static_cast<Player*>(pVictim)->CalcRating(14)));

						//Resilience is a special new rating which was created to reduce the effects of critical hits against your character.
						//It has two components; it reduces the chance you will be critically hit by x%, 
						//and it reduces the damage dealt to you by critical hits by 2x%. x is the percentage resilience granted by a given resilience rating. 
						//It is believed that resilience also functions against spell crits, 
						//though it's worth noting that NPC mobs cannot get critical hits with spells.
						float dmg_reduction_pct=2*static_cast<Player*>(pVictim)->CalcRating(14)/100;
						if(dmg_reduction_pct>1.0f)
							dmg_reduction_pct = 1.0f; //we cannot resist more then he is criticalling us, there is no point of the critical then :P
						dmg.full_damage=float2int32(dmg.full_damage - dmg.full_damage*dmg_reduction_pct);
					}
					
					pVictim->Emote(EMOTE_ONESHOT_WOUNDCRITICAL);
					vproc |= PROC_ON_CRIT_HIT_VICTIM;
					aproc |= PROC_ON_CRIT_ATTACK;

					if(this->IsPlayer())
					{
						this->SetFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_CRITICAL);	//SB@L: Enables spells requiring critical strike
						if(!sEventMgr.HasEvent(this,EVENT_CRIT_FLAG_EXPIRE))
							sEventMgr.AddEvent((Unit*)this,&Unit::EventAurastateExpire,(uint32)AURASTATE_FLAG_CRITICAL,EVENT_CRIT_FLAG_EXPIRE,5000,1,0);
						else sEventMgr.ModifyEventTimeLeft(this,EVENT_CRIT_FLAG_EXPIRE,5000);
					}

					CALL_SCRIPT_EVENT(pVictim, OnTargetCritHit)(this, dmg.full_damage);
					CALL_SCRIPT_EVENT(this, OnCritHit)(pVictim, dmg.full_damage);
				}
				break;
//--------------------------------crushing blow---------------------------------------------
			case 6:
				hit_status |= HITSTATUS_CRUSHINGBLOW;
				dmg.full_damage = (dmg.full_damage*3)/2;
				break;
//--------------------------------regular hit-----------------------------------------------
			default:
				break;	
			}
//==========================================================================================
//==============================Post Roll Damage Processing=================================
//==========================================================================================
//--------------------------absorption------------------------------------------------------
			uint32 dm = dmg.full_damage;
			abs = pVictim->AbsorbDamage(0,(uint32*)&dm);
		
			if(dmg.full_damage > (int32)blocked_damage)
			{
				uint32 sh = pVictim->ManaShieldAbsorb(dmg.full_damage);
//--------------------------armor reducing--------------------------------------------------
				if(sh)
				{
					dmg.full_damage -= sh;
					if(dmg.full_damage)
						CalculateResistanceReduction(pVictim,&dmg);
					dmg.full_damage += sh;
					dmg.resisted_damage += sh;
				}
				else
					CalculateResistanceReduction(pVictim,&dmg);
			}
			dmg.resisted_damage += abs;
			realdamage = dmg.full_damage-dmg.resisted_damage-blocked_damage;
			if(realdamage < 0)
			{
				realdamage = 0;
				vstate = IMMUNE;
				hit_status |= HITSTATUS_ABSORBED;
			}
		}
		break;
	}

//==========================================================================================
//==============================Post Roll Special Cases Processing==========================
//==========================================================================================
//--------------------------special states processing---------------------------------------
	if(pVictim->GetTypeId() == TYPEID_UNIT)
	{
		if(pVictim->GetAIInterface() && pVictim->GetAIInterface()->getAIState()== STATE_EVADE)
		{
			vstate = EVADE;
			realdamage = 0;
			dmg.full_damage = 0;
			dmg.resisted_damage = 0;
		}
	}
	if(pVictim->GetTypeId() == TYPEID_PLAYER && static_cast<Player*>(pVictim)->GodModeCheat == true)
	{
		dmg.resisted_damage = dmg.full_damage; //godmode
	}

	if (this->IsPlayer() && static_cast<Player*>(this)->IsInFeralForm()) 
	{ //Players in feral forms can't proc anything on hit
		switch (static_cast<Player*>(this)->GetShapeShift())
		{
		case FORM_CAT:	
		case FORM_BEAR: 
		case FORM_DIREBEAR:
			disable_proc = true;
			break;
		default:
			break;
		}
	}
//--------------------------dirty fixes-----------------------------------------------------
	//vstate=1-wound,2-dodge,3-parry,4-interrupt,5-block,6-evade,7-immune,8-deflect	
	// hack fix for stormstirke loop here.
	if(damage_type != DUALWIELD && !disable_proc)
    {
		if( !(ability && ability->NameHash == 0x2535ed19) )
		{
			this->HandleProc(aproc,pVictim, ability,realdamage);
			m_procCounter = 0;
		}

		pVictim->HandleProc(vproc,this, ability,realdamage);
		m_procCounter = 0;
	}
//--------------------------spells triggering-----------------------------------------------
	if(realdamage > 0 && ability == 0)
	{
		if(IsPlayer() && ((Player*)this)->m_onStrikeSpells.size())
		{
			SpellCastTargets targets;
			targets.m_unitTarget = pVictim->GetGUID();
			targets.m_targetMask = 0x2;
			Spell *cspell;

			// Loop on hit spells, and strike with those.
			for(map< SpellEntry*, pair<uint32, uint32> >::iterator itr = ((Player*)this)->m_onStrikeSpells.begin();
				itr != ((Player*)this)->m_onStrikeSpells.end(); ++itr)
			{
				//Strike(pVictim, 1, (*itr), add_damage, pct_dmg_mod, exclusive_damage);
				if( itr->second.first )
				{
					// We have a *periodic* delayed spell.
					uint32 t = getMSTime();
					if( t > itr->second.second )  // Time expired
					{
						// Set new time
						itr->second.second = t + itr->second.first;
					}

					// Cast.
					cspell = new Spell(this, itr->first, true, NULL);
					cspell->prepare(&targets);
				}
				else
				{
					cspell = new Spell(this, itr->first, true, NULL);
					cspell->prepare(&targets);
				}			
			}
		}

		if(IsPlayer() && ((Player*)this)->m_onStrikeSpellDmg.size())
		{
			map<uint32, OnHitSpell>::iterator itr = ((Player*)this)->m_onStrikeSpellDmg.begin();
			uint32 min_dmg, max_dmg, range, dmg;
			for(; itr != ((Player*)this)->m_onStrikeSpellDmg.end(); ++itr)
			{
				min_dmg = itr->second.mindmg;
				max_dmg = itr->second.maxdmg;
				range = min_dmg - max_dmg;
				dmg = min_dmg;
				if(range) range += sRand.randInt(range);

				SpellNonMeleeDamageLog(pVictim, itr->second.spellid, dmg, true);
			}
		}
	}
	
//==========================================================================================
//==============================Data Sending================================================
//==========================================================================================
	WorldPacket data(SMSG_ATTACKERSTATEUPDATE, 70);
	//0x4--dualwield,0x10 miss,0x20 absorbed,0x80 crit,0x4000 -glancing,0x8000-crushing
	//only for melee!

	if(!ability)
	{
		if(dmg.full_damage)
			if(dmg.full_damage == (int32)dmg.resisted_damage)
				hit_status |= HITSTATUS_ABSORBED;

		data << (uint32)hit_status;   
		data << GetNewGUID();
		data << pVictim->GetNewGUID();
		
		data << (uint32)realdamage;		 // Realdamage;
		data << (uint8)1;				   // Damage type counter / swing type

		data << (uint32)0;				  // Damage school
		data << (float)dmg.full_damage;	 // Damage float
		data << (uint32)dmg.full_damage;	// Damage amount
		data << (uint32)dmg.resisted_damage;// Damage absorbed
		data << (uint32)0;				  // Damage resisted

		data << (uint32)vstate;			 // new victim state
		data << (int32)0;					// can be 0,1000 or -1
		data << (uint32)0;				  // unknown
		data << (uint32)blocked_damage;	 // Damage amount blocked
		
		data << (uint32) 0;

		SendMessageToSet(&data, this->IsPlayer());
	}
	else
	{
		if(realdamage)//FIXME: add log for miss,block etc for ability and ranged
		{
			SendSpellNonMeleeDamageLog(this,pVictim,ability->Id,realdamage,0,dmg.resisted_damage,0,false,blocked_damage,((hit_status & HITSTATUS_CRICTICAL) ? true : false),true);
		}
		//FIXME: add log for miss,block etc for ability and ranged
		//example how it works
		//SendSpellLog(this,pVictim,ability->Id,SPELL_LOG_MISS);
	}

	if(ability && realdamage==0)
	{	
		SendSpellLog(this,pVictim,ability->Id,SPELL_LOG_RESIST);
	}
//==========================================================================================
//==============================Damage Dealing==============================================
//==========================================================================================
	if(realdamage)
	{
		DealDamage(pVictim, realdamage, 0, targetEvent, 0);
		//pVictim->HandleProcDmgShield(PROC_ON_MELEE_ATTACK_VICTIM,this);
		HandleProcDmgShield(PROC_ON_MELEE_ATTACK_VICTIM,pVictim);

		if (pVictim->GetCurrentSpell())
			pVictim->GetCurrentSpell()->AddTime(0);
	}
	else
	{		
		// have to set attack target here otherwise it wont be set
		// because dealdamage is not called.
		setAttackTarget(pVictim);
	}
//==========================================================================================
//==============================Post Damage Dealing Processing==============================
//==========================================================================================
//--------------------------durability processing-------------------------------------------
	if(pVictim->IsPlayer())
	{
		static_cast<Player*>(pVictim)->GetItemInterface()->ReduceItemDurability();
		if (!this->IsPlayer())
		{
			Player *pr = ((Player*)pVictim);
			if (Rand(pr->GetSkillUpChance(SKILL_DEFENSE)*sWorld.getRate(RATE_SKILLCHANCE)))
			{
				pr->_AdvanceSkillLine(SKILL_DEFENSE, float2int32( 1.0f * sWorld.getRate(RATE_SKILLRATE)));
				pr->UpdateChances();
			}
		}
		else
		{
			 static_cast<Player*>(this)->GetItemInterface()->ReduceItemDurability();
		}
	}
	else
	{
		if (this->IsPlayer())//not pvp
		{
			static_cast<Player*>(this)->GetItemInterface()->ReduceItemDurability();
			Player *pr = ((Player*)this);
			if (Rand(pr->GetSkillUpChance(SubClassSkill)*sWorld.getRate(RATE_SKILLCHANCE)))
			{
				pr->_AdvanceSkillLine(SubClassSkill, float2int32( 1.0f * sWorld.getRate(RATE_SKILLRATE)));
				//pr->UpdateChances();
			}
		}
	}
//--------------------------rage processing-------------------------------------------------
	uint32 val;
	if(IsPlayer())
	{
		if(this->GetPowerType() == POWER_TYPE_RAGE && !ability)
		{
		  // It only regens rage if in combat, don't know why but this is making
		  // the player to regen 1 rage every 3 secs.....
		  // and the formula is wrong also ... TODO
		  if(isInCombat()) {
			val = GetUInt32Value(UNIT_FIELD_POWER2)+(realdamage*20)/getLevel();
			val += (static_cast<Player *>(this)->rageFromDamageDealt*val)/100;
			SetUInt32Value(UNIT_FIELD_POWER2, val>=1000?1000:val);
		  }
		}
	}
		
	RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_START_ATTACK);
//--------------------------extra strikes processing----------------------------------------
	while(m_extraattacks > 0)
	{
		m_extraattacks--;
		Strike(pVictim,damage_type,ability,add_damage,pct_dmg_mod,exclusive_damage, false);
	}

	if(m_extrastriketargets)
	{
		int32 m_extra = m_extrastriketargets;
		int32 m_temp = m_extrastriketargets;
		m_extrastriketargets = 0;

		for(set<Object*>::iterator itr = m_objectsInRange.begin(); itr != m_objectsInRange.end() && m_extra; ++itr)
		{
			if(m_extra == 0)
				break;

			if((*itr) != pVictim && (*itr)->IsUnit() && CalcDistance(*itr) < 10.0f && isAttackable(this, (*itr)) && (*itr)->isInFront(this))
			{
				Strike( ((Unit*)*itr), damage_type, ability, add_damage, pct_dmg_mod, exclusive_damage, false );
				--m_extra;
			}
		}
		m_extrastriketargets = m_temp;
	}
}	

void Unit::smsg_AttackStop(Unit* pVictim)
{
	if(!pVictim)
		return;

	if(m_attackTarget == pVictim->GetGUID())
		setAttackTarget(NULL);

	WorldPacket data(SMSG_ATTACKSTOP, 24);
	data << GetNewGUID();
	data << pVictim->GetNewGUID();
	data << uint32( pVictim->isDead() ? 1 : 0 );
	SendMessageToSet(&data, IsPlayer() );
	/*if(IsPlayer())
		((Player*)this)->GetSession()->OutPacket(SMSG_CANCEL_COMBAT);*/
	if(pVictim->isDead())
	{
		// this seems to be the way to stop combat when the victim is dead
		data.Initialize(SMSG_ATTACKSTOP);
		data << GetNewGUID() << uint8(0) << uint32(0);
		SendMessageToSet(&data, IsPlayer());
	}
}

void Unit::smsg_AttackStop(uint64 victimGuid)
{
	WorldPacket data(20);
	data.Initialize( SMSG_ATTACKSTOP );
	data << GetNewGUID();
	FastGUIDPack(data, victimGuid);
	data << uint32( 0 );
	SendMessageToSet(&data, IsPlayer());
}

void Unit::smsg_AttackStart(Unit* pVictim)
{
	if(GetTypeId() != TYPEID_PLAYER) 
		return;

	Player* pThis = (Player*) this;

	// Prevent user from ignoring attack speed and stopping and start combat really really fast
	/*if(!isAttackReady())
		setAttackTimer(uint32(0));
	else if(!canReachWithAttack(pVictim))
	{
		setAttackTimer(uint32(500));
		//pThis->GetSession()->OutPacket(SMSG_ATTACKSWING_NOTINRANGE);
	}
	else if(!isInFront(pVictim))
	{
		setAttackTimer(uint32(500));
		//pThis->GetSession()->OutPacket(SMSG_ATTACKSWING_NOTINRANGE);
	}*/

	// Send out ATTACKSTART
	WorldPacket data(SMSG_ATTACKSTART, 16);
	data << GetGUID();
	data << pVictim->GetGUID();
	SendMessageToSet(&data, true);
	sLog.outDebug( "WORLD: Sent SMSG_ATTACKSTART" );

	// FLAGS changed so other players see attack animation
	//	addUnitFlag(0x00080000);
	//	setUpdateMaskBit(UNIT_FIELD_FLAGS );
	if(pThis->cannibalize)
	{
		sEventMgr.RemoveEvents(pThis, EVENT_CANNIBALIZE);
		pThis->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
		pThis->cannibalize = false;
	}
}

void Unit::RemoveBySpecialType(uint32 id, uint64 caster)
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			SpellExtraInfo* aur = SpellExtraStorage.LookupEntry(m_auras[x]->GetSpellId());
			if(aur)
			{
				uint32 type = aur->specialtype;
				if(type == id)
				{
					if(((type == STING) || (type == BLESSING) || (type == CURSE)) && (m_auras[x]->m_casterGuid==caster))
					{
						if (RemoveAura(m_auras[x]->GetSpellId(),caster))
							return;
					}
					else
					{
						if (RemoveAura(m_auras[x]))
							return;
					}
				}
			}
		}
	}
}

void Unit::AddAura(Aura *aur)
{
	if(m_mapId != 530)
	{
		for(uint32 i = 0; i < 3; ++i)
		{
			// Can't use flying auras in non-outlands.
			if(aur->GetSpellProto()->EffectApplyAuraName[i] == 208 || aur->GetSpellProto()->EffectApplyAuraName[i] == 207)
			{
				delete aur;
				return;
			}
		}
	}

	if(aur->GetSpellProto()->School && SchoolImmunityList[aur->GetSpellProto()->School])
	{
		delete aur;
		return;
	}

    if(!aur->IsPassive())
	{
		//uint32 aurName = aur->GetSpellProto()->Name;
		//uint32 aurRank = aur->GetSpellProto()->Rank;
		uint32 maxStack = aur->GetSpellProto()->maxstack;
		if(aur->GetSpellProto()->procCharges>0)
			maxStack=aur->GetSpellProto()->procCharges;
		if(IsPlayer() && ((Player*)this)->stack_cheat)
			maxStack = 999;

		SpellEntry * info = aur->GetSpellProto();
		//uint32 flag3 = aur->GetSpellProto()->Flags3;
		AuraCheckResponse acr;
		bool deleteAur = false;
		//check if we already have this aura by this caster -> update duration
		uint32 f=0;
		for(uint32 x=0;x<MAX_AURAS;x++)
		{
			if(m_auras[x])
			{
				// Nasty check for Blood Fury debuff (spell system based on namehashes is bs anyways)
				if(info->NameHash == 0xDE1C36C8)
					continue;

				if(m_auras[x]->GetSpellProto()->Id != aur->GetSpellId())
				{
					// Check for auras by specific type.
					// Check for auras with the same name and a different rank.
					
					if(info->buffType > 0 && m_auras[x]->GetSpellProto()->buffType & info->buffType && maxStack == 0)
						deleteAur = HasAurasOfBuffType(info->buffType, aur->m_casterGuid,0);
					else
					{
						acr = AuraCheck(info->NameHash, info->RankNumber, m_auras[x]);
						if(acr.Error == AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT)
							deleteAur = true;
						else if(acr.Error == AURA_CHECK_RESULT_LOWER_BUFF_PRESENT)
						{
							// remove the lower aura
							m_auras[x]->Remove();

							// no more checks on bad ptr
							continue;
						}
					}					   
				}
				else if(m_auras[x]->GetSpellId() == aur->GetSpellId()) // not the best formula to test this I know, but it works until we find a solution
				{
					if(!aur->IsPositive() && m_auras[x]->m_casterGuid != aur->m_casterGuid)
						continue;
					f++;
					//if(maxStack > 1)
					{
						//update duration,the same aura (update the whole stack whenever we cast a new one)
						m_auras[x]->SetDuration(aur->GetDuration());
						sEventMgr.ModifyEventTimeLeft(m_auras[x], EVENT_AURA_REMOVE, aur->GetDuration());
						if(this->IsPlayer())
						{
							WorldPacket data(SMSG_UPDATE_AURA_DURATION, 5);
							data << (uint8)m_auras[x]->m_visualSlot <<(uint32) aur->GetDuration();
							((Player*)this)->GetSession()->SendPacket(&data);
						}
					}
					if(maxStack <= f)
					{
						deleteAur = true;
						break;
					}
				}
			}
		}
		if(deleteAur)
		{
			sEventMgr.RemoveEvents(aur);
			delete aur;
			return;
		}
	}
			
	////////////////////////////////////////////////////////
	if(aur->GetSpellProto()->SpellGroupType && m_objectTypeId == TYPEID_PLAYER)
	{
		int32 speedmod=0;
		SM_FIValue(SM_FSpeedMod,&speedmod,aur->GetSpellProto()->SpellGroupType);
		if(speedmod)
		{
			m_speedModifier += speedmod;
			UpdateSpeed();
		}
	}
	////////////////////////////////////////////////////////

	aur->m_auraSlot=255;
	aur->ApplyModifiers(true);
	
	if(!aur->IsPassive())
	{	
		aur->AddAuraVisual();
		if(aur->m_auraSlot==255)
		{
			//add to invisible slot
			for(uint32 x=MAX_AURAS;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
			{
				if(!m_auras[x])
				{
					m_auras[x]=aur;
					aur->m_auraSlot=x;
					break;
				}
			}
			if(aur->m_auraSlot == 255)
			{
				sLog.outError("Aura error in active aura. ");
				// for next loop.. lets kill the fucker
				aur->Remove();
				return;
			} 

			// add visual
			AddAuraVisual(aur->GetSpellId(), 1, aur->IsPositive());
		}
		else
		{
			m_auras[aur->m_auraSlot]=aur;
		}
	}
	else
	{
        if((aur->m_spellProto->AttributesEx & 1024))
        {
            aur->AddAuraVisual();
        }
		for(uint32 x=MAX_AURAS;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
		{
			if(!m_auras[x])
			{
				m_auras[x]=aur;
				aur->m_auraSlot=x;
				break;
			}
		}
		if(aur->m_auraSlot==255)
		{
			sLog.outError("Aura error in passive aura. removing. SpellId: %u", aur->GetSpellProto()->Id);
			// for next loop.. lets kill the fucker
			aur->Remove();
			return;
		}
	}

	if(aur->GetDuration() > 0)
		sEventMgr.AddEvent(aur, &Aura::Remove, EVENT_AURA_REMOVE, aur->GetDuration(), 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

	aur->RelocateEvents();

	// Reaction from enemy AI
	if(GetTypeId() == TYPEID_UNIT && !aur->IsPositive())	  // Creature
	{
		if(isAlive() && CanAgroHash(aur->m_spellProto->NameHash)) //no threat for hunter's mark
		{
			Unit * pCaster = aur->GetUnitCaster();
			if(!pCaster) return;

			addAttacker(pCaster);

			GetAIInterface()->AttackReaction(pCaster, 1, aur->GetSpellId());
		}
	}
}

bool Unit::RemoveAura(Aura *aur)
{
	aur->Remove();
	return true;
}

bool Unit::RemoveAura(uint32 spellId)
{//this can be speed up, if we know passive \pos neg
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellId()==spellId)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAuras(uint32 * SpellIds)
{
	if(!SpellIds || *SpellIds == 0)
		return false;

	uint32 x=0,y;
	bool res = false;
	for(;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			for(y=0;SpellIds[y] != 0;++y)
			{
				if(m_auras[x]->GetSpellId()==SpellIds[y])
				{
					m_auras[x]->Remove();
					res = true;
				}
			}
		}
	}
	return res;
}

bool Unit::RemoveAura(uint32 spellId, uint64 guid)
{   
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellId()==spellId && m_auras[x]->m_casterGuid == guid)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAuraByNameHash(uint32 namehash)
{
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAuraPosByNameHash(uint32 namehash)
{
	for(uint32 x=0;x<MAX_POSITIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAuraNegByNameHash(uint32 namehash)
{
	for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAllAuraByNameHash(uint32 namehash)
{
	bool res = false;
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				res=true;
			}
		}
	}
	return res;
}

bool Unit::RemoveAllPosAuraByNameHash(uint32 namehash)
{
	bool res = false;
	for(uint32 x=0;x<MAX_POSITIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				res=true;
			}
		}
	}
	return res;
}

bool Unit::RemoveAllNegAuraByNameHash(uint32 namehash)
{
	bool res = false;
	for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				res=true;
			}
		}
	}
	return res;
}

void Unit::RemoveNegativeAuras()
{
	for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
            if(m_auras[x]->GetSpellProto()->Flags4 & CAN_PERSIST_AND_CASTED_WHILE_DEAD)
                continue;
            else
            {
			    m_auras[x]->Remove();
            }
		}
	}
}

void Unit::RemoveAllAuras()
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			m_auras[x]->Remove();
		}
	}
}

//ex:to remove morph spells
void Unit::RemoveAllAuraType(uint32 auratype)
{
    for(uint32 x=0;x<MAX_AURAS;x++)
    {
		if(m_auras[x])
		{
			SpellEntry *proto=m_auras[x]->GetSpellProto();
			if(proto->EffectApplyAuraName[0]==auratype || proto->EffectApplyAuraName[1]==auratype || proto->EffectApplyAuraName[2]==auratype)
				RemoveAura(m_auras[x]->GetSpellId());//remove all morph auras containig to this spell (like wolf motph also gives speed)
		}
    }
}

bool Unit::SetAurDuration(uint32 spellId,Unit* caster,uint32 duration)
{
	Aura*aur=FindAura(spellId,caster->GetGUID());
	if(!aur)
		return false;
	aur->SetDuration(duration);
	sEventMgr.ModifyEventTimeLeft(aur, EVENT_AURA_REMOVE, duration);
			
	if(this->IsPlayer())
	{
		WorldPacket data(5);
		data.SetOpcode(SMSG_UPDATE_AURA_DURATION);
		data << (uint8)(aur)->GetAuraSlot() << duration;
		((Player*)this)->GetSession()->SendPacket(&data);
	}
			
	return true;
}

bool Unit::SetAurDuration(uint32 spellId,uint32 duration)
{
	Aura*aur=FindAura(spellId);

	if(!aur) 
		return false;
	

	aur->SetDuration(duration);
	sEventMgr.ModifyEventTimeLeft(aur, EVENT_AURA_REMOVE, duration);
		
	if(this->IsPlayer())
	{
		WorldPacket data(5);
		data.SetOpcode(SMSG_UPDATE_AURA_DURATION);
		data << (uint8)(aur)->GetAuraSlot() << duration;
		((Player*)this)->GetSession()->SendPacket(&data);
	}

	return true;
}

Aura* Unit::FindAura(uint32 spellId)
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellId()==spellId)
			{
				return m_auras[x];
			}
		}
	}
	return NULL;
}

Aura* Unit::FindAura(uint32 spellId, uint64 guid)
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellId() == spellId && m_auras[x]->m_casterGuid == guid)
			{
				return m_auras[x];
			}
		}
	}
	return NULL;
}

void Unit::_UpdateSpells( uint32 time )
{
	if(m_currentSpell != NULL)
	{
		m_currentSpell->update(time);
	}
}

void Unit::castSpell( Spell * pSpell )
{
	// check if we have a spell already casting etc
	if(m_currentSpell)
	{
		m_currentSpell->cancel();
	}

	m_currentSpell = pSpell;
	pLastSpell = pSpell->m_spellInfo;
}

void Unit::InterruptSpell()
{
	if(m_currentSpell)
	{
		//ok wtf is this
		//m_currentSpell->SendInterrupted(SPELL_FAILED_INTERRUPTED);
		m_currentSpell->cancel();
	}
}

void Unit::DeMorph()
{
	// hope it solves it :)
	uint32 displayid = this->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID);
	this->SetUInt32Value(UNIT_FIELD_DISPLAYID, displayid);
}

void Unit::Emote (EmoteType emote)
{
	if(emote == 0)
		return;

	WorldPacket data(SMSG_EMOTE, 12);
	data << uint32(emote);
	data << this->GetGUID();
	SendMessageToSet (&data, true);
}

void Unit::SendChatMessageAlternateEntry(uint32 entry, uint8 type, uint32 lang, const char * msg)
{
	uint32 UnitNameLength = 0, MessageLength = 0;
	const char *UnitName = "";
	CreatureInfo *ci;

	ci = CreatureNameStorage.LookupEntry(entry);
	if(!ci)
		return;

	UnitName = ci->Name;
	UnitNameLength = strlen((char*)UnitName) + 1;
	MessageLength = strlen((char*)msg) + 1;

	switch(type)
	{
	case CHAT_MSG_MONSTER_SAY:
		{
			for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); i++)
			{
				if((*i)->GetTypeId() == TYPEID_PLAYER)
				{
					WorldPacket data(SMSG_MESSAGECHAT, 35 + UnitNameLength + MessageLength);
					data << type;
					data << lang;
					data << GetGUID();
					data << uint32(0);			// new in 2.1.0
					data << UnitNameLength;
					data << UnitName;
					data << ((Player*)(*i))->GetGUID();
					data << MessageLength;
					data << msg;
					data << uint8(0x00);

					WorldSession *session = ((Player*)(*i))->GetSession();
					session->SendPacket(&data);
				}
			}
		}break;
	case CHAT_MSG_MONSTER_YELL:
		{
			for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); i++)
			{
				if((*i)->GetTypeId() == TYPEID_PLAYER)
				{
					WorldPacket data(SMSG_MESSAGECHAT, 35 + UnitNameLength + MessageLength);
					data << type;
					data << lang;
					data << GetGUID();
					data << uint32(0);			// new in 2.1.0
					data << UnitNameLength;
					data << UnitName;
					data << ((Player*)(*i))->GetGUID();
					data << MessageLength;
					data << msg;
					data << uint8(0x00);

					WorldSession *session = ((Player*)(*i))->GetSession();
					session->SendPacket(&data);
				}
			}
		}break;
	case CHAT_MSG_MONSTER_EMOTE:
		{
			for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); i++)
			{
				if((*i)->GetTypeId() == TYPEID_PLAYER)
				{
					WorldPacket data(SMSG_MESSAGECHAT, 35 + UnitNameLength + MessageLength);
					data << type;
					data << lang;
					data << GetGUID();
					data << uint32(0);			// new in 2.1.0
					data << UnitNameLength;
					data << UnitName;
					data << ((Player*)(*i))->GetGUID();
					data << MessageLength;
					data << msg;
					data << uint8(0x00);

					WorldSession *session = ((Player*)(*i))->GetSession();
					session->SendPacket(&data);
				}
			}
		}break;
	}
}

void Unit::SendChatMessage(uint8 type, uint32 lang, const char *msg)
{
	uint32 UnitNameLength = 0, MessageLength = 0;
	const char *UnitName = "";
	CreatureInfo *ci;

	ci = CreatureNameStorage.LookupEntry(GetEntry());
	if(!ci)
		return;

	UnitName = ci->Name;
	UnitNameLength = strlen((char*)UnitName) + 1;
	MessageLength = strlen((char*)msg) + 1;

	switch(type)
	{
	case CHAT_MSG_MONSTER_SAY:
		{
			for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); i++)
			{
				if((*i)->GetTypeId() == TYPEID_PLAYER)
				{
					WorldPacket data(SMSG_MESSAGECHAT, 35 + UnitNameLength + MessageLength);
					data << type;
					data << lang;
					data << GetGUID();
					data << uint32(0);			// new in 2.1.0
					data << UnitNameLength;
					data << UnitName;
					data << ((Player*)(*i))->GetGUID();
					data << MessageLength;
					data << msg;
					data << uint8(0x00);

					WorldSession *session = ((Player*)(*i))->GetSession();
					session->SendPacket(&data);
				}
			}
		}break;
	case CHAT_MSG_MONSTER_YELL:
		{
			for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); i++)
			{
				if((*i)->GetTypeId() == TYPEID_PLAYER)
				{
					WorldPacket data(SMSG_MESSAGECHAT, 35 + UnitNameLength + MessageLength);
					data << type;
					data << lang;
					data << GetGUID();
					data << uint32(0);			// new in 2.1.0
					data << UnitNameLength;
					data << UnitName;
					data << ((Player*)(*i))->GetGUID();
					data << MessageLength;
					data << msg;
					data << uint8(0x00);

					WorldSession *session = ((Player*)(*i))->GetSession();
					session->SendPacket(&data);
				}
			}
		}break;
	case CHAT_MSG_MONSTER_EMOTE:
		{
			for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); i++)
			{
				if((*i)->GetTypeId() == TYPEID_PLAYER)
				{
					WorldPacket data(SMSG_MESSAGECHAT, 35 + UnitNameLength + MessageLength);
					data << type;
					data << lang;
					data << GetGUID();
					data << uint32(0);			// new in 2.1.0
					data << UnitNameLength;
					data << UnitName;
					data << ((Player*)(*i))->GetGUID();
					data << MessageLength;
					data << msg;
					data << uint8(0x00);

					WorldSession *session = ((Player*)(*i))->GetSession();
					session->SendPacket(&data);
				}
			}
		}break;
	}
}

void Unit::WipeHateList()
{ 
	GetAIInterface()->WipeHateList(); 
}

void Unit::WipeTargetList() 
{ 
	GetAIInterface()->WipeTargetList(); 
}

void Unit::AddInRangeObject(Object* pObj)
{
	if ((pObj->GetTypeId() == TYPEID_UNIT) || (pObj->GetTypeId() == TYPEID_PLAYER))
	{
		if (isHostile(this, (Unit*)pObj))
			m_oppFactsInRange.insert(pObj);
		if (GetTypeId()==TYPEID_PLAYER)
		{
			if (((Player*)this)->InGroup())
			{
				if(pObj->GetTypeId() == TYPEID_UNIT)
				{
					if (((Creature*)pObj)->Tagged)
					{
						if(((Player*)this)->GetGroup()->HasMember(pObj->GetMapMgr()->GetPlayer(((Creature*)pObj)->TaggerGuid)))
						{
							uint32 Flags;
							Flags = ((Creature*)pObj)->m_uint32Values[UNIT_DYNAMIC_FLAGS];
							Flags |= U_DYN_FLAG_TAPPED_BY_PLAYER;
							ByteBuffer buf1(500);
							((Creature*)pObj)->BuildFieldUpdatePacket(&buf1, UNIT_DYNAMIC_FLAGS, Flags);
							((Player*)this)->PushUpdateData(&buf1, 1);
						}
					}
				}
			}
		}		
	}

	Object::AddInRangeObject(pObj);
}//427

void Unit::OnRemoveInRangeObject(Object* pObj)
{
	m_oppFactsInRange.erase(pObj);

	if(pObj->GetTypeId() == TYPEID_UNIT || pObj->GetTypeId() == TYPEID_PLAYER)
	{
		/*if (m_useAI)*/
		Unit *pUnit = static_cast<Unit*>(pObj);
		GetAIInterface()->CheckTarget(pUnit);

		/* === ATTACKER CHECKS === */

		// if we're being attacked by him, remove from our set.
		removeAttacker(pUnit);

		// check that our target is not him (shouldn't happen!)
		if(m_attackTarget == pUnit->GetGUID())
			setAttackTarget(NULL);

		/* === END ATTACKER CHECKS === */

		if(GetUInt64Value(UNIT_FIELD_CHARM) == pObj->GetGUID())
			if(m_currentSpell)
				m_currentSpell->cancel();

        Object::OnRemoveInRangeObject(pObj);
        if(critterPet == pObj)
		{
			critterPet->SafeDelete();
			critterPet = 0;
		}
	}
    else
    {
        Object::OnRemoveInRangeObject(pObj);
    }
}

void Unit::ClearInRangeSet()
{
	Object::ClearInRangeSet();
}

//Events
void Unit::EventAddEmote(EmoteType emote, uint32 time)
{
	m_oldEmote = GetUInt32Value(UNIT_NPC_EMOTESTATE);
	SetUInt32Value(UNIT_NPC_EMOTESTATE,emote);
	sEventMgr.AddEvent(this, &Creature::EmoteExpire, EVENT_UNIT_EMOTE, time, 1,0);
}

void Unit::EmoteExpire()
{
	SetUInt32Value(UNIT_NPC_EMOTESTATE, m_oldEmote);
	sEventMgr.RemoveEvents(this, EVENT_UNIT_EMOTE);
}

void Unit::RegisterPeriodicChatMessage(uint32 delay, uint32 msgid, std::string message,bool sendnotify)
{
	EventMgr::getSingleton().AddEvent(this, &Unit::DelayedChatMessage, uint32(0), msgid, message, sendnotify, EVENT_UNIT_REPEAT_MSG, delay, 0,0);
}

void Unit::DelayedChatMessage(uint32 delay, uint32 msgid, std::string message, bool sendnotify)
{
	if(delay == 0)	  // Send instantly
	{
		SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message.c_str());
		if(sendnotify && msgid) SendNotifyToNearby(msgid);
	}
	else
	{
		if(delay > 0)
			EventMgr::getSingleton().AddEvent(this, &Unit::DelayedChatMessage, uint32(0), msgid, message, sendnotify, EVENT_UNIT_CHAT_MSG, delay, 1,0);
		else
			// We shouldn't get here; Repeat this function again, but with a delay of 0 ;)
			DelayedChatMessage(0, msgid, message, sendnotify);
	}
}

void Unit::SendNotifyToNearby(uint32 msgid)
{
	//Zehamster: commented the full code, since it does nothing but wasting cpu cycles
	/*
	InRangeSet::iterator itr = GetInRangeSetBegin();
	for(;itr!=GetInRangeSetEnd();++itr)
	{
		if((*itr)->GetTypeId() == TYPEID_UNIT)
		{
			// Send lua event
			Creature *crt = ((Creature*)(*itr));
			//Log::getSingleton().outDebug("LUA: Sending \"NOTIFY\" Msg %d to mob %s", msgid, crt->GetCreatureName()->Name.c_str());
			//crt->LUA_SendEvent(ON_UNIT_NOTIFYMSG, msgid);
		}
	}
	*/
}

void Unit::SendNotifyToNearbyCreature(uint32 msgid, uint32 entryid)
{
	//Zehamster: commented the full code since it does nothing but wasting cpu cycles
	/*
	InRangeSet::iterator itr = GetInRangeSetBegin();
	for(;itr!=GetInRangeSetEnd();++itr)
	{
		if((*itr)->GetTypeId() == TYPEID_UNIT && (*itr)->GetEntry() == entryid)
		{
			// Send lua event
			Creature *crt = ((Creature*)(*itr));
 //		   Log::getSingleton().outDebug("LUA: Sending \"NOTIFY\" Msg %d to mob %s", msgid, GetCreatureName()->Name.c_str());
			//crt->LUA_SendEvent(ON_UNIT_NOTIFYMSG, msgid);
		}
	}
	*/
}

uint32 Unit::GetResistance(uint32 type)
{
	return GetUInt32Value(UNIT_FIELD_RESISTANCES+type);
}

// grep: note to self.. this should be moved to creature.
/*
void Unit::InitializeEscortQuest(uint32 questid, bool stopatend, bool returnondie)
{
	this->bEscortActive = false;
	this->bHasEscortQuest = true;
	this->bStopAtEndOfWaypoints = stopatend;
	this->m_escortquestid = questid;
	this->m_escortupdatetimer = 0;
	if(this->m_useAI && this->GetAIInterface() != NULL)
		GetAIInterface()->setMoveType(10);				// Quest
}*/

void Unit::MoveToWaypoint(uint32 wp_id)
{
	if(this->m_useAI && this->GetAIInterface() != NULL)
	{
		AIInterface *ai = GetAIInterface();
		WayPoint *wp = ai->getWayPoint(wp_id);
		if(!wp)
		{
			sLog.outString("WARNING: Invalid WP specified in MoveToWaypoint.");
			return;
		}

		ai->m_currentWaypoint = wp_id;
		if(wp->flags!=0)
			ai->m_moveRun = true;
		ai->MoveTo(wp->x, wp->y, wp->z, 0);
	}
}

/*
void Unit::StartEscortQuest()
{
	this->bEscortActive = true;
	this->m_escortupdatetimer = 1000;
}

void Unit::PauseEscortQuest()
{
	this->bEscortActive = false;
}

void Unit::EndEscortQuest()
{
	// Return to spawn
	Creature *crt = ((Creature*)this);
	GetAIInterface()->MoveTo(crt->respawn_cord[0], crt->respawn_cord[1], crt->respawn_cord[2], crt->respawn_cord[3]);
	this->bEscortActive = false;
	this->m_escortupdatetimer = 0;
}

void Unit::EscortSetStartWP(uint32 wp)
{
	this->m_escortStartWP = wp;
}

void Unit::EscortSetEndWP(uint32 wp)
{
	this->m_escortEndWP = wp;
}*/

int32 Unit::GetDamageDoneMod(uint32 school)
{
	if(this->IsPlayer())
	   return (int32)(GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS+school)-GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+school));
	else
	   return ((Creature*)this)->ModDamageDone[school];
}
	
float Unit::GetDamageDonePctMod(uint32 school)
{
   if(this->IsPlayer())
	   return GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+school);
	else
	   return ((Creature*)this)->ModDamageDonePct[school];
}

void Unit::CalcDamage()
{
	if(IsPlayer())
		((Player*)this)->CalcDamage();
	else
	{
	float r;
	float delta;
	float mult;
	
	float ap_bonus = GetAP()/14000.0;

		float bonus = ap_bonus*GetUInt32Value(UNIT_FIELD_BASEATTACKTIME);
	
		delta = ((Creature*)this)->ModDamageDone[0];
		mult = ((Creature*)this)->ModDamageDonePct[0];
		r = BaseDamage[0]*mult+delta+bonus;
		SetFloatValue(UNIT_FIELD_MINDAMAGE,r>0?r:0);
		r = BaseDamage[1]*mult+delta+bonus;
		SetFloatValue(UNIT_FIELD_MAXDAMAGE,r>0?r:0);
	
	//	SetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE,BaseRangedDamage[0]*mult+delta);
	//	SetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE,BaseRangedDamage[1]*mult+delta);
  
	}	
}

//returns absorbed dmg
uint32 Unit::ManaShieldAbsorb(uint32 dmg)
{
	if(!m_manashieldamt)
		return 0;
	//mana shield group->16. the only

	uint32 mana = GetUInt32Value(UNIT_FIELD_POWER1);
	int32 effectbonus = SM_PEffectBonus ? SM_PEffectBonus[16] : 0;

	int32 potential = (mana*50)/((100+effectbonus));
	if(potential>m_manashieldamt)
		potential = m_manashieldamt;

	if((int32)dmg<potential)
		potential = dmg;

	uint32 cost = (potential*(100+effectbonus))/50;

	SetUInt32Value(UNIT_FIELD_POWER1,mana-cost);
	m_manashieldamt -= potential;
	if(!m_manashieldamt)
		RemoveAura(m_manaShieldId);
	return potential;	
}

void Unit::VampiricEmbrace(uint32 dmg,Unit* tgt)
{
	if(!IsPlayer())
		return;//just in case
	int32 perc = 20;
	SM_FIValue(SM_FDummy,&perc,4);
	uint32 heal = (dmg*perc) / 100;

	WorldPacket data(25);
	data.SetOpcode(SMSG_HEALSPELL_ON_PLAYER);
	
	data << this->GetNewGUID();
	data << this->GetNewGUID();
	data << uint32(15286);  //Vampiric embrace
	data << uint32(heal);   // amt healed
	data << uint8(0);	   //this is crical message
	uint32 ch=this->GetUInt32Value(UNIT_FIELD_HEALTH);
	uint32 mh=this->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
	if(mh!=ch)
	{
		ch += heal;
		if(ch > mh)
			this->SetUInt32Value(UNIT_FIELD_HEALTH, mh);
		else 
			this->SetUInt32Value(UNIT_FIELD_HEALTH, ch);
	
	}
	this->SendMessageToSet(&data, true);

	SubGroup * pGroup = ((Player*)this)->GetGroup() ?
		((Player*)this)->GetGroup()->GetSubGroup(((Player*)this)->GetSubGroup()) : 0;

	if(pGroup)
	{
		GroupMembersSet::iterator itr;
		((Player*)this)->GetGroup()->Lock();
		for(itr = pGroup->GetGroupMembersBegin(); itr != pGroup->GetGroupMembersEnd(); ++itr)
		{
			Player *p = itr->player;
			if(!p || p==this || !p->isAlive())
				continue;
			data.clear();
			data << p->GetNewGUID();
			data << this->GetNewGUID();
			data << uint32(15286);//Vampiric embrace
			data << uint32(heal);	// amt healed
			data << uint8(0);	 //this is crical message
			
			ch = p->GetUInt32Value(UNIT_FIELD_HEALTH);
			mh = p->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
			if(mh != ch)
			{
				ch += heal;
				if(ch > mh)
					p->SetUInt32Value(UNIT_FIELD_HEALTH,mh);
				else 
					p->SetUInt32Value(UNIT_FIELD_HEALTH,ch);			
			}
			this->SendMessageToSet(&data, true);
		}
		((Player*)this)->GetGroup()->Unlock();
	}

}

// based of vampiric embrace code
void Unit::VampiricTouch(uint32 dmg,Unit* tgt)
{
        if(!IsPlayer() || this->getClass() == WARRIOR || this->getClass() == ROGUE)
                return;//just in case
        
        int32 perc = 5;
        SM_FIValue(SM_FDummy,&perc,4); // need fixing if required
        uint32 man = (dmg*perc) / 100;
        
        uint32 cm=this->GetUInt32Value(UNIT_FIELD_POWER1);
        uint32 mm=this->GetUInt32Value(UNIT_FIELD_MAXPOWER1);
        
        if(mm!=cm)
        {
                cm += man;
                if(cm > mm)
                        this->SetUInt32Value(UNIT_FIELD_POWER1, mm);
                else 
                        this->SetUInt32Value(UNIT_FIELD_POWER1, cm);
        }
    
		SubGroup * pGroup = ((Player*)this)->GetGroup() ?
			((Player*)this)->GetGroup()->GetSubGroup(((Player*)this)->GetSubGroup()) : 0;

		if(pGroup)
        {
                GroupMembersSet::iterator itr;
				((Player*)this)->GetGroup()->Lock();
                for(itr = pGroup->GetGroupMembersBegin(); itr != pGroup->GetGroupMembersEnd(); ++itr)
                {
                        if(!itr->player || itr->player == this)
                                continue;
                        Player *p = itr->player;
                        if(!p->isAlive() || this->getClass()==WARRIOR || this->getClass() == ROGUE)
                                continue;
                        
                        uint32 cm=p->GetUInt32Value(UNIT_FIELD_POWER1);
                        uint32 mm=p->GetUInt32Value(UNIT_FIELD_MAXPOWER1);
            
                        if(mm!=cm)
                        {
                                cm += man;
                                if(cm > mm)
                                        this->SetUInt32Value(UNIT_FIELD_POWER1, mm);
                                else 
                                        this->SetUInt32Value(UNIT_FIELD_POWER1, cm);
                        }
                }
				((Player*)this)->GetGroup()->Unlock();
        }
}

// grep: Remove any AA spells that aren't owned by this player.
//		 Otherwise, if he logs out and in and joins another group,
//		 he'll apply it to them.
/*
void Unit::RemoveAllAreaAuras()
{
	AuraList::iterator itr,it1;
	for(itr = m_auras.begin();itr!=m_auras.end();)
	{
		it1 = itr++;
		if(((*it1)->m_spellProto->Effect[0] == SPELL_EFFECT_APPLY_AREA_AURA ||
			(*it1)->m_spellProto->Effect[1] == SPELL_EFFECT_APPLY_AREA_AURA ||
			(*it1)->m_spellProto->Effect[2] == SPELL_EFFECT_APPLY_AREA_AURA) && (*it1)->GetCaster() != this)
		{
			(*it1)->Remove();
			m_auras.erase(it1);
		} 
	}
}
*/
uint32 Unit::AbsorbDamage(uint32 School,uint32 * dmg)
{
	SchoolAbsorb::iterator i,j;
	uint32 abs=0;
	for(i=Absorbs[School].begin();i!=Absorbs[School].end();)
	{
		if ((int32)(*dmg) >= (*i)->amt)//remove this absorb
		{
			*dmg -= (*i)->amt;
			abs += (*i)->amt;
			j = i++;

			if(i != Absorbs[School].end())
			{
				while((*i)->spellid == (*j)->spellid)
				{
					++i;
					if(i == Absorbs[School].end())
						break;
				}
			}
			
			this->RemoveAura((*j)->spellid); //,(*j)->caster);
			if(!*dmg)//absorbed all dmg
				break;		
		}
		else //absorb full dmg
		{
			abs += *dmg;
			(*i)->amt -= *dmg;
			*dmg=0;
			break;
		}
	}
	
	return abs;
}

bool Unit::setDetectRangeMod(uint64 guid, int32 amount)
{
	int next_free_slot = -1;
	for(int i = 0; i < 5; i++)
	{
		if(m_detectRangeGUID[i] == 0 && next_free_slot == -1)
		{
			next_free_slot = i;
		}
		if(m_detectRangeGUID[i] == guid)
		{
			m_detectRangeMOD[i] = amount;
			return true;
		}
	}
	if(next_free_slot != -1)
	{
		m_detectRangeGUID[next_free_slot] = guid;
		m_detectRangeMOD[next_free_slot] = amount;
		return true;
	}
	return false;
}

void Unit::unsetDetectRangeMod(uint64 guid)
{
	for(int i = 0; i < 5; i++)
	{
		if(m_detectRangeGUID[i] == guid)
		{
			m_detectRangeGUID[i] = 0;
			m_detectRangeMOD[i] = 0;
		}
	}
}

int32 Unit::getDetectRangeMod(uint64 guid)
{
	for(int i = 0; i < 5; i++)
	{
		if(m_detectRangeGUID[i] == guid)
		{
			return m_detectRangeMOD[i];
		}
	}
	return 0;
}

void Unit::SetStandState(uint8 standstate)
{
	SetByte(UNIT_FIELD_BYTES_1,0,standstate);
	if(standstate == STANDSTATE_STAND)//standup
		RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_STAND_UP);

	if(m_objectTypeId == TYPEID_PLAYER)
		((Player*)this)->GetSession()->OutPacket(SMSG_STANDSTATE_CHANGE_ACK, 1, &standstate);
}

void Unit::RemoveAurasByInterruptFlag(uint32 flag)
{
	Aura * a;
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		a = m_auras[x];
		if(a == NULL)
			continue;
		
		//some spells do not get removed all the time only at specific intervals
		if((a->m_spellProto->AuraInterruptFlags & flag) && !(a->m_spellProto->procFlags & PROC_REMOVEONUSE))
		{
			a->Remove();
			m_auras[x] = NULL;
		}
	}
}

bool Unit::HasAura(uint32 visualid)
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	if(m_auras[x] && m_auras[x]->GetSpellProto()->SpellVisual==visualid)
	{
		return true;
	}

	return false;
}

void Unit::DropAurasOnDeath()
{
	for(uint32 x=0;x<MAX_AURAS;x++)
    {
        if(m_auras[x])
        {
            if(m_auras[x] && m_auras[x]->GetSpellProto()->Flags4 & CAN_PERSIST_AND_CASTED_WHILE_DEAD)
                continue;
            else
	        {
		        m_auras[x]->Remove();
	        }
        }
    }
}

void Unit::UpdateSpeed(bool delay /* = false */)
{
	if(!HasFlag( UNIT_FIELD_FLAGS , U_FIELD_FLAG_MOUNT_SIT ))
	{
		if(IsPlayer())
			m_runSpeed = m_base_runSpeed*(1.0f + ((float)m_speedModifier)/100.0f);
		else
			m_runSpeed = m_base_runSpeed*(1.0f + ((float)m_speedModifier)/100.0f);
	}
	else
	{
		if(IsPlayer())
		{
			m_runSpeed = m_base_runSpeed*(1.0f + ((float)m_mountedspeedModifier)/100.0f);
			m_runSpeed += (m_speedModifier<0) ? (m_base_runSpeed*((float)m_speedModifier)/100.0f) : 0;
		}
		else
		{
			m_runSpeed = m_base_runSpeed*(1.0f + ((float)m_mountedspeedModifier)/100.0f);
			m_runSpeed += (m_speedModifier<0) ? (m_base_runSpeed*((float)m_speedModifier)/100.0f) : 0;
		}
	}

	m_flySpeed = PLAYER_NORMAL_FLIGHT_SPEED*(1.0f + ((float)m_flyspeedModifier)/100.0f);

	if(IsPlayer())
	{
		if(delay)
			static_cast<Player*>(this)->resend_speed = delay;
		else
		{
			static_cast<Player*>(this)->SetPlayerSpeed(RUN, m_runSpeed);
			static_cast<Player*>(this)->SetPlayerSpeed(FLY, m_flySpeed);
		}
	}
}

bool Unit::HasActiveAura(uint32 spellid)
{
	for(uint32 x=0;x<MAX_AURAS;x++)
	if(m_auras[x] && m_auras[x]->GetSpellId()==spellid)
	{
		return true;
	}

	return false;
}

bool Unit::HasActiveAura(uint32 spellid,uint64 guid)
{
	for(uint32 x=0;x<MAX_AURAS;x++)
	if(m_auras[x] && m_auras[x]->GetSpellId()==spellid && m_auras[x]->m_casterGuid==guid)
	{
		return true;
	}

	return false;
}

void Unit::addAttacker(Unit *pUnit)
{
	if(!pUnit->isAlive() || !isAlive())
		return;

	// this shouldn't happen
	ASSERT(pUnit != NULL);

	// being attacked by this unit
	if(m_attackers.count(pUnit->GetGUID()) == 0)
		m_attackers.insert(pUnit->GetGUID());
}

void Unit::removeAttacker(Unit *pUnit)
{
	// this should't happen
	ASSERT(pUnit != NULL);

	// crashfix at shutdown
//	if(m_attackers.size() == 0) return;

	// no longer being attacked by unit
	m_attackers.erase(pUnit->GetGUID());
}

void Unit::setAttackTarget(Unit* pUnit)
{
	if(!isAlive())
		pUnit = 0;

	if(pUnit == NULL)
	{
		// check if we have a current target
		// if so, remove from their attacker set
		if(m_attackTarget)
		{
			Unit *pTarget = m_mapMgr ? m_mapMgr->GetUnit(m_attackTarget) : NULL;
			if(pTarget)
				pTarget->removeAttacker(this);
		}

		// now, clear ourselves and abort
		m_attackTarget = 0;
		return;		
	}

	// can't attack self
	if(pUnit == this)
		return;

	// already done -> skip
	if(m_attackTarget == pUnit->GetGUID())
		return;

	// remove from our old target, if we have one
	// this will stop attackers getting "lost" and people
	// "stuck" in combat.

	if(m_attackTarget != 0)
	{
		Unit *pTarget = m_mapMgr ? m_mapMgr->GetUnit(m_attackTarget) : NULL;
		if(pTarget)
			pTarget->removeAttacker(this);
	}

	// add to victim's attacker set
	pUnit->addAttacker(this);

	// set our value
	m_attackTarget = pUnit->GetGUID();
}

void Unit::clearAttackers(bool bFromOther)
{
	// this will _usually_ be called on death.
	// so, we will remove ourselves from our target's set,
	// and clear our target set

	// clear our target
	if(bFromOther)
		setAttackTarget(NULL);

	// skip this shit if we don't have any attackers
	if(m_attackers.size() == 0)
		return;
	
	if(bFromOther && IsInWorld())
	{
		// push our existing attackers into a temp vector
		// because they will erase from our attacker set when they have their target
		// reset, we need to have another place to store them
		std::vector<Unit*> tmp_vec;
		tmp_vec.reserve(m_attackers.size());
		Unit *pUnit;

		for(AttackerSet::iterator itr = m_attackers.begin(); itr != m_attackers.end(); ++itr)
		{
			pUnit = GetMapMgr()->GetUnit(*itr);
			if(pUnit)
				tmp_vec.push_back(pUnit);
		}

		// clear any attackers from us
		for(std::vector<Unit*>::iterator itr = tmp_vec.begin(); itr != tmp_vec.end(); ++itr)
		{
			pUnit = (*itr);
			if(pUnit->getAttackTarget() == GetGUID())
				pUnit->setAttackTarget(NULL);
		}
	}

	// wipe our attacker set (we shouldnt have any unknowns)
	m_attackers.clear();
}

void Unit::EventSummonPetExpire()
{
	if(summonPet)
	{
		if(summonPet->GetEntry() == 7915)//Goblin Bomb
		{
			SpellEntry *spInfo = sSpellStore.LookupEntry(13259);
			if(!spInfo)
				return;

			Spell*sp=new Spell(summonPet,spInfo,true,NULL);
			SpellCastTargets tgt;
			tgt.m_unitTarget=summonPet->GetGUID();
			sp->prepare(&tgt);
		}
		else
		{
			summonPet->RemoveFromWorld(false);
			delete summonPet;
			summonPet = NULL;
		}
	}
	sEventMgr.RemoveEvents(this, EVENT_SUMMON_PET_EXPIRE);
}

void Unit::CastSpell(Unit* Target, SpellEntry* Sp, bool triggered)
{
	SpellCastTargets targets(Target->GetGUID());
	Spell *newSpell = new Spell(this, Sp, triggered, 0);
	newSpell->prepare(&targets);
}

void Unit::CastSpell(Unit* Target, uint32 SpellID, bool triggered)
{
	SpellEntry * ent = sSpellStore.LookupEntry(SpellID);
	if(ent == 0) return;

	CastSpell(Target, ent, triggered);
}

void Unit::CastSpell(uint64 targetGuid, SpellEntry* Sp, bool triggered)
{
	SpellCastTargets targets(targetGuid);
	Spell *newSpell = new Spell(this, Sp, triggered, 0);
	newSpell->prepare(&targets);
}

void Unit::CastSpell(uint64 targetGuid, uint32 SpellID, bool triggered)
{
	SpellEntry * ent = sSpellStore.LookupEntry(SpellID);
	if(ent == 0) return;

	CastSpell(targetGuid, ent, triggered);
}
void Unit::CastSpellAoF(float x,float y,float z,SpellEntry* Sp, bool triggered)
{
	SpellCastTargets targets;
	targets.m_destX = x;
	targets.m_destY = y;
	targets.m_destZ = z;
	targets.m_targetMask=TARGET_FLAG_DEST_LOCATION;
	Spell *newSpell = new Spell(this, Sp, triggered, 0);
	newSpell->prepare(&targets);
}

void Unit::PlaySoundToSet(uint32 sound_entry)
{
	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << sound_entry;
	SendMessageToSet(&data, true);
}

void Unit::PlaySpellVisual(uint64 target, uint32 spellVisual)
{
	WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 12);
	data << target << spellVisual;
	SendMessageToSet(&data, true);
}

void Unit::Root()
{
	if(m_objectTypeId == TYPEID_PLAYER)
	{
		static_cast<Player*>(this)->SetMovement(MOVE_ROOT, 1);
	}
	else
	{
		m_aiInterface->m_canMove = false;
	}
}

void Unit::Root(uint32 time)
{
	this->m_special_state |= UNIT_STATE_ROOT;
	if(m_objectTypeId == TYPEID_PLAYER)
	{
		static_cast<Player*>(this)->SetMovement(MOVE_ROOT, 1);
	}
	else
	{
		m_aiInterface->m_canMove = false;
		m_aiInterface->StopMovement(time);
	}
}

void Unit::Unroot()
{
	this->m_special_state &= ~(UNIT_STATE_ROOT);
	if(m_objectTypeId == TYPEID_PLAYER)
	{
		static_cast<Player*>(this)->SetMovement(MOVE_UNROOT, 5);
	}
	else
	{
		m_aiInterface->m_canMove = true;
	}
}

void Unit::RemoveAurasByBuffType(uint32 buff_type, const uint64 &guid, uint32 skip)
{
	uint64 sguid = buff_type >= SPELL_TYPE_BLESSING ? guid : 0;

	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->buffType & buff_type && m_auras[x]->GetSpellId()!=skip)
			if(!sguid || (sguid && m_auras[x]->m_casterGuid == sguid))
				m_auras[x]->Remove();
	}
}

void Unit::RemoveAurasByBuffIndexType(uint32 buff_index_type, const uint64 &guid)
{
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->buffIndexType == buff_index_type)
			if(!guid || (guid && m_auras[x]->m_casterGuid == guid))
				m_auras[x]->Remove();
	}
}

bool Unit::HasAurasOfBuffType(uint32 buff_type, const uint64 &guid,uint32 skip)
{
	uint64 sguid = buff_type >= SPELL_TYPE_BLESSING ? guid : 0;

	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->buffType & buff_type && m_auras[x]->GetSpellId()!=skip)
			if(!sguid || (sguid && m_auras[x]->m_casterGuid == sguid))
				return true;
	}

	return false;
}

AuraCheckResponse Unit::AuraCheck(uint32 name_hash, uint32 rank)
{
	AuraCheckResponse resp;

	// no error for now
	resp.Error = AURA_CHECK_RESULT_NONE;
	resp.Misc  = 0;

	// look for spells with same namehash
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash)
		{
			// we've got an aura with the same name as the one we're trying to apply
			resp.Misc = m_auras[x]->GetSpellProto()->Id;

			// compare the rank to our applying spell
			if(m_auras[x]->GetSpellProto()->RankNumber > rank)
				resp.Error = AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT;
			else
				resp.Error = AURA_CHECK_RESULT_LOWER_BUFF_PRESENT;

			// we found something, save some loops and exit
			break;
		}
	}

	// return it back to our caller
	return resp;
}

AuraCheckResponse Unit::AuraCheck(uint32 name_hash, uint32 rank, Aura* aur)
{
	AuraCheckResponse resp;

	// no error for now
	resp.Error = AURA_CHECK_RESULT_NONE;
	resp.Misc  = 0;

	// look for spells with same namehash
	if(aur->GetSpellProto()->NameHash == name_hash)
	{
		// we've got an aura with the same name as the one we're trying to apply
		resp.Misc = aur->GetSpellProto()->Id;

		// compare the rank to our applying spell
		if(aur->GetSpellProto()->RankNumber > rank)
			resp.Error = AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT;
		else
			resp.Error = AURA_CHECK_RESULT_LOWER_BUFF_PRESENT;
	}

	// return it back to our caller
	return resp;
}

void Unit::AddToWorld()
{
	Object::AddToWorld();
}

void Unit::OnPushToWorld()
{
	for(uint32 x = 0; x < MAX_AURAS+MAX_PASSIVE_AURAS; ++x)
	{
		if(m_auras[x] != 0)
			m_auras[x]->RelocateEvents();
	}
}

void Unit::RemoveFromWorld()
{
	if(critterPet != 0)
	{
		critterPet->RemoveFromWorld(false);
		delete critterPet;
		critterPet = 0;
	}

	if(dynObj != 0)
		dynObj->Remove();

	for(uint32 i = 0; i < 4; ++i)
	{
		if(m_ObjectSlots[i] != 0)
		{
			GameObject * obj = m_mapMgr->GetGameObject(m_ObjectSlots[i]);
			if(obj)
				obj->ExpireAndDelete();

			m_ObjectSlots[i] = 0;
		}
	}

	Object::RemoveFromWorld();


	for(uint32 x = 0; x < MAX_AURAS+MAX_PASSIVE_AURAS; ++x)
	{
		if(m_auras[x] != 0)
			m_auras[x]->RelocateEvents();
	}
	m_aiInterface->WipeReferences();
}

void Unit::RemoveAurasByInterruptFlagButSkip(uint32 flag, uint32 skip)
{
	Aura * a;
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		a = m_auras[x];
		if(a == 0)
			continue;

		//some spells do not get removed all the time only at specific intervals
		if((a->m_spellProto->AuraInterruptFlags & flag) && (a->m_spellProto->Id != skip) && a->m_spellProto->proc_interval==0)
		{
			//the black sheeps of sociaty
			if(a->m_spellProto->AuraInterruptFlags & AURA_INTERRUPT_ON_CAST_SPELL)
			{
				switch(a->GetSpellProto()->Id)
				{
					//priest - surge of light
					case 33151:
						{
							//our luck. it got trigered on smite..we do not remove it just yet
							if(m_currentSpell && m_currentSpell->m_spellInfo->NameHash==2272412495)
								continue;
							//this spell gets removed only when caasting smite
						    SpellEntry *spi = sSpellStore.LookupEntry(skip);
							if(spi && spi->NameHash!=2272412495)
								continue;
						}
				}
			}
			a->Remove();
		}
	}
}

bool Unit::HasAurasWithNameHash(uint32 name_hash)
{
	for(uint32 x = 0; x < MAX_AURAS; ++x)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash)
			return true;
	}

	return false;
}

bool Unit::HasNegativeAuraWithNameHash(uint32 name_hash)
{
	for(uint32 x = MAX_POSITIVE_AURAS; x < MAX_AURAS; ++x)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash)
			return true;
	}

	return false;
}

bool Unit::HasNegativeAura(uint32 spell_id)
{
	for(uint32 x = MAX_POSITIVE_AURAS; x < MAX_AURAS; ++x)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->Id == spell_id)
			return true;
	}

	return false;
}

uint32 Unit::AddAuraVisual(uint32 spellid, uint32 count, bool positive)
{
	int32 free = -1;
	uint32 start = positive ? 0 : MAX_POSITIVE_AURAS;
	uint32 end_  = positive ? MAX_POSITIVE_AURAS : MAX_AURAS;

	for(uint32 x = start; x < end_; ++x)
	{
		if(free == -1 && m_uint32Values[UNIT_FIELD_AURA+x] == 0)
			free = x;

		if(m_uint32Values[UNIT_FIELD_AURA+x] == spellid)
		{
			// Increase count of this aura.
			ModAuraStackCount(x, count);
			return free;
		}
	}

	if(free == -1) return 0xFF;

	uint8 flagslot = free >> 3;
	uint32 value = GetUInt32Value((uint16)(UNIT_FIELD_AURAFLAGS + flagslot));

	uint8 aurapos = (free & 7) << 2;
	uint32 setflag = AFLAG_SET;
	if(positive)
		setflag = 0xD;

	uint32 value1 = (uint32)setflag << aurapos;
	value |= value1;

	SetUInt32Value((uint16)(UNIT_FIELD_AURAFLAGS + flagslot), value);
	SetUInt32Value(UNIT_FIELD_AURA + free, spellid);
	ModAuraStackCount(free, 1);
	SetAuraSlotLevel(free, positive);
	
	return free;
}

void Unit::SetAuraSlotLevel(uint32 slot, bool positive)
{
	uint32 index = slot / 4;
	uint32 val = m_uint32Values[UNIT_FIELD_AURALEVELS + index];
	uint32 bit = (slot % 4) * 8;
	val &= ~(0xFF << bit);
	if(positive)
		val |= (0x11 << bit);

	SetUInt32Value(UNIT_FIELD_AURALEVELS + index, val);
}

void Unit::RemoveAuraVisual(uint32 spellid, uint32 count)
{
	for(uint32 x = 0; x < MAX_AURAS; ++x)
	{
		if(m_uint32Values[UNIT_FIELD_AURA+x] == spellid)
		{
			// Decrease count of this aura.
			int test = ModAuraStackCount(x, -(int32)count);
			if(test == 0)
			{
				// Aura has been removed completely.
				uint8 flagslot = x >> 3;
				uint32 value = GetUInt32Value((uint16)(UNIT_FIELD_AURAFLAGS + flagslot));
				uint8 aurapos = (x & 7) << 2;
				uint32 value1 = ~( (uint32)0xF << aurapos );
				value &= value1;
				SetUInt32Value(UNIT_FIELD_AURAFLAGS + flagslot,value);
				SetUInt32Value(UNIT_FIELD_AURA + x, 0);
				SetAuraSlotLevel(x, false);
			}			
		}
	}
}

uint32 Unit::ModAuraStackCount(uint32 slot, int32 count)
{
	uint32 index = (slot / 4);
	uint32 byte  = (slot % 4);
	uint32 val   = m_uint32Values[UNIT_FIELD_AURAAPPLICATIONS+index];

	// shouldn't happen
	uint32 ac;

	if(count < 0 && m_auraStackCount[slot] < abs(count))
	{
		m_auraStackCount[slot] = ac = 0;
	}
	else
	{
		m_auraStackCount[slot] += count;
		ac = (m_auraStackCount[slot] > 0) ? m_auraStackCount[slot] - 1 : 0;
	}

	val &= ~(0xFF << byte * 8);
	val |= (ac << byte * 8);

	SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS+index, val);
	return m_auraStackCount[slot];
}

void Unit::RemoveAurasOfSchool(uint32 School, bool Positive)
{
	for(uint32 x = 0; x < MAX_AURAS; ++x)
	{
		if(m_auras[x]  && m_auras[x]->GetSpellProto()->School == School && (!m_auras[x]->IsPositive() || Positive))
			m_auras[x]->Remove();
	}
}

void Unit::EnableFlight(bool delay /* = false */)
{
	if(!delay || m_objectTypeId != TYPEID_PLAYER)
	{
		WorldPacket data(SMSG_MOVE_SET_FLY, 13);
		data << GetNewGUID();
		data << uint32(2);
		SendMessageToSet(&data, true);

		if(IsPlayer())
			((Player*)this)->_delayAntiFlyUntil = UNIXTIME + 3;
	}
	else
	{
		WorldPacket * data = new WorldPacket(SMSG_MOVE_SET_FLY, 13);
		*data << GetNewGUID();
		*data << uint32(2);
		SendMessageToSet(data, false);
		static_cast<Player*>(this)->delayedPackets.add(data);
		static_cast<Player*>(this)->_delayAntiFlyUntil=UNIXTIME+3;
	}
}

void Unit::DisableFlight(bool delay /* = false */)
{
	if(!delay || m_objectTypeId != TYPEID_PLAYER)
	{
		WorldPacket data(SMSG_MOVE_SET_UNFLY, 13);
		data << GetNewGUID();
		data << uint32(5);
		SendMessageToSet(&data, true);

		if(IsPlayer())
			((Player*)this)->_delayAntiFlyUntil = UNIXTIME + 3;
	}
	else
	{
		WorldPacket * data = new WorldPacket(SMSG_MOVE_SET_UNFLY, 13);
		*data << GetNewGUID();
		*data << uint32(5);
		SendMessageToSet(data, false);
		static_cast<Player*>(this)->delayedPackets.add(data);
		static_cast<Player*>(this)->_delayAntiFlyUntil=UNIXTIME+3;
	}
}

bool Unit::IsDazed()
{
	for(uint32 x = 0; x < MAX_AURAS; ++x)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ENSNARED)
			return true;
			for(uint32 y=0;y<3;y++)
			if(m_auras[x]->GetSpellProto()->EffectMechanic[y]==MECHANIC_ENSNARED)
				return true;
		}
	}

	return false;

}

void Unit::UpdateVisibility()
{
	ByteBuffer buf(2500);
	InRangeSet::iterator itr, it3;
	uint32 count;
	bool can_see;
	bool is_visible;
	Player *pl;
	Object * pObj;
	Player * plr;

	if(m_objectTypeId == TYPEID_PLAYER)
	{
		plr = ((Player*)this);
		for (Object::InRangeSet::iterator itr = m_objectsInRange.begin(); itr != m_objectsInRange.end();)
		{
			pObj = (*itr);
			++itr;

			can_see = plr->CanSee(pObj);
			is_visible = plr->GetVisibility(pObj, &it3);
			if(can_see)
			{
				if(!is_visible)
				{
					buf.clear();
					count = pObj->BuildCreateUpdateBlockForPlayer( &buf, plr );
					plr->PushCreationData(&buf, count);
					plr->AddVisibleObject(pObj);
				}
			}
			else
			{
				if(is_visible)
				{
					pObj->DestroyForPlayer(plr);
					plr->RemoveVisibleObject(it3);
				}
			}

			if (pObj->GetTypeId() == TYPEID_PLAYER)
			{
				pl = ((Player*)pObj);
				can_see = pl->CanSee(plr);
				is_visible = pl->GetVisibility(plr, &it3);
				if(can_see)
				{
					if(!is_visible)
					{
						buf.clear();
						count = plr->BuildCreateUpdateBlockForPlayer( &buf, pl );
						pl->PushCreationData(&buf, count);
						pl->AddVisibleObject(plr);
					}
				}
				else
				{
					if(is_visible)
					{
						plr->DestroyForPlayer(pl);
						pl->RemoveVisibleObject(it3);
					}
				}
			}
		}
	}
	else			// For units we can save a lot of work
	{
		for(set<Player*>::iterator it2 = GetInRangePlayerSetBegin(); it2 != GetInRangePlayerSetEnd(); ++it2)
		{
			can_see = (*it2)->CanSee(this);
			is_visible = (*it2)->GetVisibility(this, &itr);
			if(!can_see)
			{
				if(is_visible)
				{
					DestroyForPlayer(*it2);
					(*it2)->RemoveVisibleObject(itr);
				}
			}
			else
			{
				if(!is_visible)
				{
					buf.clear();
					count = BuildCreateUpdateBlockForPlayer(&buf, *it2);
					(*it2)->PushCreationData(&buf, count);
					(*it2)->AddVisibleObject(this);
				}
			}
		}
	}
}

void Unit::RemoveSoloAura(uint32 type)
{
	switch(type)
	{
		case 1:// Polymorph
		{
			if(!polySpell) return;
			if(HasActiveAura(polySpell))
				RemoveAura(polySpell);
		}break;
/*		case 2:// Sap
		{
			if(!sapSpell) return;
			if(HasActiveAura(sapSpell))
				RemoveAura(sapSpell);
		}break;
		case 3:// Fear (Warlock)
		{
			if(!fearSpell) return;
			if(HasActiveAura(fearSpell))
				RemoveAura(fearSpell);
		}break;*/
		default:
			{
			sLog.outDebug("Warning: we are trying to remove a soloauratype that has no handle");
			}break;
	}
}

void Unit::EventHealthChangeSinceLastUpdate()
{
	int pct = GetHealthPct();
	if(pct<35)
	{
		uint32 toset=AURASTATE_FLAG_HEALTH35;
		if(pct<20)
			toset |= AURASTATE_FLAG_HEALTH20;
		else
			RemoveFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_HEALTH20);
		SetFlag(UNIT_FIELD_AURASTATE,toset);
	}
	else
		RemoveFlag(UNIT_FIELD_AURASTATE , AURASTATE_FLAG_HEALTH35 | AURASTATE_FLAG_HEALTH20);
}

int32 Unit::GetAP()
{
    int32 baseap = GetUInt32Value(UNIT_FIELD_ATTACK_POWER) + GetUInt32Value(UNIT_FIELD_ATTACK_POWER_MODS);
    float totalap = float(baseap)*(GetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER)+1);
	if(totalap>=0)
		return float2int32(totalap);
	return	0;
}

int32 Unit::GetRAP()
{
    int32 baseap = GetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER) + GetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MODS);
    float totalap = float(baseap)*(GetFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER)+1);
	if(totalap>=0)
		return float2int32(totalap);
	return	0;
}

bool Unit::GetSpeedDecrease()
{
	int32 before=m_speedModifier;
	m_speedModifier -= m_slowdown;
	m_slowdown = 0;
	map< uint32, int32 >::iterator itr = speedReductionMap.begin();
	for(; itr != speedReductionMap.end(); ++itr)
		m_slowdown = min(m_slowdown, itr->second);

	if(m_slowdown<-100)
		m_slowdown = 100; //do not walk backwards !

	m_speedModifier += m_slowdown;
	//save bandwidth :P
	if(m_speedModifier!=before)
		return true;
	return false;
}

void Unit::EventCastSpell(Unit * Target, SpellEntry * Sp)
{
	Spell * pSpell = new Spell(Target, Sp, true, NULL);
	SpellCastTargets targets(Target->GetGUID());
	pSpell->prepare(&targets);
}

void Unit::SetFacing(float newo)
{
	SetOrientation(newo);
	/*WorldPacket data(40);
	data.SetOpcode(MSG_MOVE_SET_FACING);
	data << GetNewGUID();
	data << (uint32)0; //flags
	data << (uint32)0; //time
	data << GetPositionX() << GetPositionY() << GetPositionZ() << newo;
	data << (uint32)0; //unk
	SendMessageToSet( &data, false );*/

	/*WorldPacket data(SMSG_MONSTER_MOVE, 60);
	data << GetNewGUID();
	data << m_position << getMSTime();
	data << uint8(4) << newo;
	data << uint32(0x00000000);		// flags
	data << uint32(0);				// time
	data << m_position;				// position
	SendMessageToSet(&data, true);*/
	m_aiInterface->SendMoveToPacket(m_position.x,m_position.y,m_position.z,m_position.o,1,1);
}

//guardians are temporary spawn that will inherit master faction and will folow them. Apart from that they have their own mind
Unit* Unit::create_guardian(uint32 guardian_entry,uint32 duration,float angle)
{
	CreatureProto * proto = CreatureProtoStorage.LookupEntry(guardian_entry);
	CreatureInfo * info = CreatureNameStorage.LookupEntry(guardian_entry);
	if(!proto || !info)
	{
		sLog.outDetail("Warning : Missing summon creature template %u !",guardian_entry);
		return NULL;
	}
	float m_fallowAngle=angle;
	float x = GetPositionX()+(3*(cosf(m_fallowAngle+GetOrientation())));
	float y = GetPositionY()+(3*(sinf(m_fallowAngle+GetOrientation())));
	float z = GetPositionZ();
	Creature * p = GetMapMgr()->CreateCreature();
	p->SetInstanceID(GetMapMgr()->GetInstanceID());
	p->Load(proto, x, y, z);
	p->SetUInt64Value(UNIT_FIELD_SUMMONEDBY, GetGUID());
    p->SetUInt64Value(UNIT_FIELD_CREATEDBY, GetGUID());
    p->SetZoneId(GetZoneId());
	p->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE,GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
	p->_setFaction();

	p->GetAIInterface()->Init(p,AITYPE_PET,MOVEMENTTYPE_NONE,this);
	p->GetAIInterface()->SetUnitToFollow(this);
	p->GetAIInterface()->SetUnitToFollowAngle(m_fallowAngle);
	p->GetAIInterface()->SetFollowDistance(3.0f);

	p->PushToWorld(GetMapMgr());

	sEventMgr.AddEvent(p, &Creature::SummonExpire, EVENT_SUMMON_EXPIRE, duration, 1,0);

	return p;//lol, will compiler make the pointer conversion ?

}

float Unit::get_chance_to_daze(Unit *target)
{
//	if(GetTypeId()!=TYPEID_UNIT)
//		return 0.0f;
	float attack_skill=getLevel()*5;
	float defense_skill;
	if(target->IsPlayer())
		defense_skill=((Player*)target)->_GetSkillLineCurrent(SKILL_DEFENSE,false);
	else defense_skill=target->getLevel()*5;
	if(!defense_skill)
		defense_skill=1;
	float chance_to_daze=attack_skill*20/defense_skill;//if level is equal then we get a 20% chance to daze
	chance_to_daze = chance_to_daze*min(target->getLevel()/30.0f,1);//for targets below level 30 the chance decreses
	if(chance_to_daze>40)
		return 40.0f;
	else return chance_to_daze;
}
