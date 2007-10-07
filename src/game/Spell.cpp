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
#define SPELL_CHANNEL_UPDATE_INTERVAL 1000

/// externals for spell system
extern pSpellEffect SpellEffectsHandler[TOTAL_SPELL_EFFECTS];
extern pSpellTarget SpellTargetHandler[TOTAL_SPELL_TARGET];

void SpellCastTargets::read ( WorldPacket & data,uint64 caster )
{
	m_unitTarget = m_itemTarget = 0;m_srcX = m_srcY = m_srcZ = m_destX = m_destY = m_destZ = 0;
	m_strTarget = "";

	data >> m_targetMask;
	WoWGuid guid;

	if(m_targetMask == TARGET_FLAG_SELF)
	{
		m_unitTarget = caster;
	}

	if(m_targetMask & (TARGET_FLAG_OBJECT | TARGET_FLAG_UNIT | TARGET_FLAG_CORPSE | TARGET_FLAG_CORPSE2))
	{
		data >> guid;
		m_unitTarget = guid.GetOldGuid();
	}

	if(m_targetMask & (TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM))
	{
		data >> guid;
		m_itemTarget = guid.GetOldGuid();
	}

	if(m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
	{
		data >> m_srcX >> m_srcY >> m_srcZ;
	}

	if(m_targetMask & TARGET_FLAG_DEST_LOCATION)
	{
		data >> m_destX >> m_destY >> m_destZ;
	}

	if(m_targetMask & TARGET_FLAG_STRING)
	{
		data >> m_strTarget;
	}
}

void SpellCastTargets::write ( WorldPacket& data)
{
	data << m_targetMask;

	if(m_targetMask == TARGET_FLAG_SELF || m_targetMask & (TARGET_FLAG_UNIT | TARGET_FLAG_CORPSE | TARGET_FLAG_CORPSE2 | TARGET_FLAG_OBJECT))
    {
        FastGUIDPack(data,m_unitTarget);
    }

    if(m_targetMask & (TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM))
    {
        FastGUIDPack(data,m_itemTarget);
    }

	if(m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
		data << m_srcX << m_srcY << m_srcZ;

	if(m_targetMask & TARGET_FLAG_DEST_LOCATION)
		data << m_destX << m_destY << m_destZ;

	if(m_targetMask & TARGET_FLAG_STRING)
		data << m_strTarget;
}

Spell::Spell(Object* Caster, SpellEntry *info, bool triggered, Aura* aur)
{
	ASSERT( Caster != NULL && info != NULL );
  
	pushLastSpell(info->Id);
	m_spellInfo = info;
	m_caster = Caster;
	duelSpell = false;
	switch(Caster->GetTypeId())
	{
		case TYPEID_PLAYER:
        {
		    g_caster=NULL;
		    i_caster=NULL;
		    u_caster=(Unit*)Caster;
		    p_caster=(Player*)Caster;
		    if(p_caster->DuelingWith)
			    duelSpell = true;
        }break;

		case TYPEID_UNIT:
        {
		    g_caster=NULL;
		    i_caster=NULL;
		    p_caster=NULL;
		    u_caster=(Unit*)Caster;
		    if(u_caster->IsPet() && ((Pet*)u_caster)->GetPetOwner()->DuelingWith)
			    duelSpell = true;
        }break;

		case TYPEID_ITEM:
		case TYPEID_CONTAINER:
        {
		    g_caster=NULL;
		    u_caster=NULL;
		    p_caster=NULL;
		    i_caster=(Item*)Caster;
        }break;
		
		case TYPEID_GAMEOBJECT:
        {
		    u_caster=NULL;
		    p_caster=NULL;
		    i_caster=NULL;
		    g_caster=(GameObject*)Caster;
        }break;
        default:
            sLog.outDebug("[DEBUG][SPELL] Incompatible object type, please report this to the dev's");
        break;
	}

	m_spellState = SPELL_STATE_NULL;

	m_castPositionX = m_castPositionY = m_castPositionZ = 0;
//	TriggerSpellId = 0;
  //  TriggerSpellTarget = 0;
	m_triggeredSpell = triggered;
	m_AreaAura = false;
  
	m_triggeredByAura = aur;

	damageToHit = 0;
	castedItemId = 0;
	
	m_usesMana = false;
	failed = false;
	hadEffect = false;
	bDurSet=false;
	bRadSet[0]=false;
	bRadSet[1]=false;
	bRadSet[2]=false;
	
	cancastresult = -1;
	
	m_requiresCP=false;
	unitTarget = NULL;
	ModeratedTargets.clear();
	itemTarget = NULL;
	gameObjTarget = NULL;
	playerTarget = NULL;
	corpseTarget = NULL;
	judgement = false;
	add_damage = 0;
	m_delayed = false;
	pSpellId = 0;
	m_cancelled = false;
	ProcedOnSpell = 0;
}

Spell::~Spell()
{
	if(u_caster && u_caster->GetCurrentSpell() == this)
		u_caster->SetCurrentSpell(NULL); 
	
	if((cancastresult == -1 && !failed) || hadEffect)
		RemoveItems();
}

//i might forget conditions here. Feel free to add them
bool Spell::IsStealthSpell()
{
	//check if aura name is some stealth aura
	if( m_spellInfo->EffectApplyAuraName[0]==16 ||
		m_spellInfo->EffectApplyAuraName[1]==16 ||
		m_spellInfo->EffectApplyAuraName[2]==16 )
		return true;
	return false;
}

//i might forget conditions here. Feel free to add them
bool Spell::IsInvisibilitySpell()
{
	//check if aura name is some invisibility aura
	if( m_spellInfo->EffectApplyAuraName[0]==18 ||
		m_spellInfo->EffectApplyAuraName[1]==18 ||
		m_spellInfo->EffectApplyAuraName[2]==18 )
		return true;
	return false;
}

void Spell::FillSpecifiedTargetsInArea(float srcx,float srcy,float srcz,uint32 ind, uint32 specification)
{
    FillSpecifiedTargetsInArea(&m_targetUnits[ind],srcx,srcy,srcz,GetRadius(ind), specification);
}

// for the moment we do invisible targets
void Spell::FillSpecifiedTargetsInArea(TargetsList *tmpMap,float srcx,float srcy,float srcz, float range, uint32 specification)
{
    //IsStealth()
    float r = range * range;
    for(std::set<Object*>::iterator itr = m_caster->GetInRangeSetBegin(); itr != m_caster->GetInRangeSetEnd(); itr++ )
    {
        // don't add objects that are not units and that are dead
        if(!((*itr)->IsUnit()) || !((Unit*)(*itr))->isAlive())
            continue;
        
        //((Unit*)(*itr))->IsStealth()
        if(m_spellInfo->TargetCreatureType)
        {
            if((*itr)->GetTypeId()!= TYPEID_UNIT)
                continue;
            CreatureInfo *inf = ((Creature*)(*itr))->GetCreatureName();
            if(!inf || !(1<<(inf->Type-1) & m_spellInfo->TargetCreatureType))
                continue;
        }

        if(IsInrange(srcx,srcy,srcz,(*itr),r))
        {
            if(u_caster)
            {
                if(isAttackable(u_caster, (Unit*)(*itr)))
                {
                    if(DidHit((*itr)->GetGUID()))
                        tmpMap->push_back((*itr)->GetGUID());
                    else
                        ModeratedTargets.push_back(SpellTargetMod((*itr)->GetGUID(),2));
                }

            }
            else //cast from GO
            {
                if(g_caster && g_caster->GetUInt32Value(OBJECT_FIELD_CREATED_BY) && g_caster->m_summoner)
                {
                    //trap, check not to attack owner and friendly
                    if(isAttackable(g_caster->m_summoner,(Unit*)(*itr)))
                        tmpMap->push_back((*itr)->GetGUID());
                }
                else
                    tmpMap->push_back((*itr)->GetGUID());
            }
            if(m_spellInfo->MaxTargets)
            {
                if(m_spellInfo->MaxTargets >= tmpMap->size())
                    return;
            }
        }
    }
}
void Spell::FillAllTargetsInArea(LocationVector & location,uint32 ind)
{
    FillAllTargetsInArea(&m_targetUnits[ind],location.x,location.y,location.z,GetRadius(ind));
}

void Spell::FillAllTargetsInArea(float srcx,float srcy,float srcz,uint32 ind)
{
	FillAllTargetsInArea(&m_targetUnits[ind],srcx,srcy,srcz,GetRadius(ind));
}

void Spell::FillAllTargetsInArea(TargetsList *tmpMap,float srcx,float srcy,float srcz, float range)
{
	float r = range*range;
	for(std::set<Object*>::iterator itr = m_caster->GetInRangeSetBegin(); itr != m_caster->GetInRangeSetEnd(); itr++ )
	{
		if(!((*itr)->IsUnit()) || !((Unit*)(*itr))->isAlive())
			continue;
		if(m_spellInfo->TargetCreatureType)
		{
			if((*itr)->GetTypeId()!= TYPEID_UNIT)
				continue;
			CreatureInfo *inf = ((Creature*)(*itr))->GetCreatureName();
			if(!inf || !(1<<(inf->Type-1) & m_spellInfo->TargetCreatureType))
				continue;
		}
		if(IsInrange(srcx,srcy,srcz,(*itr),r))
		{
			if(u_caster)
			{
				if(isAttackable(u_caster, (Unit*)(*itr)))
				{
					if(DidHit((*itr)->GetGUID()))
						tmpMap->push_back((*itr)->GetGUID());
					else
						ModeratedTargets.push_back(SpellTargetMod((*itr)->GetGUID(),2));
				}
			}
			else //cast from GO
			{
				if(g_caster && g_caster->GetUInt32Value(OBJECT_FIELD_CREATED_BY) && g_caster->m_summoner)
				{
					//trap, check not to attack owner and friendly
					if(isAttackable(g_caster->m_summoner,(Unit*)(*itr)))
						tmpMap->push_back((*itr)->GetGUID());
				}
				else
					tmpMap->push_back((*itr)->GetGUID());
			}			
			if(m_spellInfo->MaxTargets)
				if(m_spellInfo->MaxTargets == tmpMap->size())
					return;
		}
	}	
}

uint64 Spell::GetSinglePossibleEnemy(float prange)
{
	float range;
	if(!prange)
		range = GetMaxRange(dbcSpellRange.LookupEntry(m_spellInfo->rangeIndex));
	else range = prange;
	float r = range*range;
	float srcx=m_caster->GetPositionX(),srcy=m_caster->GetPositionY(),srcz=m_caster->GetPositionZ();
	for(std::set<Object*>::iterator itr = m_caster->GetInRangeSetBegin(); itr != m_caster->GetInRangeSetEnd(); itr++ )
	{
		if(!((*itr)->IsUnit()) || !((Unit*)(*itr))->isAlive())
			continue;
		if(m_spellInfo->TargetCreatureType)
		{
			if((*itr)->GetTypeId()!= TYPEID_UNIT)
				continue;
			CreatureInfo *inf = ((Creature*)(*itr))->GetCreatureName();
			if(!inf || !(1<<(inf->Type-1) & m_spellInfo->TargetCreatureType))
				continue;
		}	
		if(IsInrange(srcx,srcy,srcz,(*itr),r))
		{
			if(u_caster)
			{
				if(isAttackable(u_caster, (Unit*)(*itr)) && DidHit((*itr)->GetGUID()))
					return (*itr)->GetGUID(); 			
			}
			else //cast from GO
			{
				if(g_caster && g_caster->GetUInt32Value(OBJECT_FIELD_CREATED_BY) && g_caster->m_summoner)
				{
					//trap, check not to attack owner and friendly
					if(isAttackable(g_caster->m_summoner,(Unit*)(*itr)))
						return (*itr)->GetGUID();
				}
			}			
		}
	}
	return 0;
}

uint64 Spell::GetSinglePossibleFriend(float prange)
{
	float range;
	if(!prange)
		range = GetMaxRange(dbcSpellRange.LookupEntry(m_spellInfo->rangeIndex));
	else range = prange;
	float r = range*range;
	float srcx=m_caster->GetPositionX(),srcy=m_caster->GetPositionY(),srcz=m_caster->GetPositionZ();
	for(std::set<Object*>::iterator itr = m_caster->GetInRangeSetBegin(); itr != m_caster->GetInRangeSetEnd(); itr++ )
	{
		if(!((*itr)->IsUnit()) || !((Unit*)(*itr))->isAlive())
			continue;
		if(m_spellInfo->TargetCreatureType)
		{
			if((*itr)->GetTypeId()!= TYPEID_UNIT)
				continue;
			CreatureInfo *inf = ((Creature*)(*itr))->GetCreatureName();
				if(!inf || !(1<<(inf->Type-1) & m_spellInfo->TargetCreatureType))
					continue;
		}	
		if(IsInrange(srcx,srcy,srcz,(*itr),r))
		{
			if(u_caster)
			{
				if(isFriendly(u_caster, (Unit*)(*itr)) && DidHit((*itr)->GetGUID()))
					return (*itr)->GetGUID(); 			
			}
			else //cast from GO
			{
				if(g_caster && g_caster->GetUInt32Value(OBJECT_FIELD_CREATED_BY) && g_caster->m_summoner)
				{
					//trap, check not to attack owner and friendly
					if(isFriendly(g_caster->m_summoner,(Unit*)(*itr)))
						return (*itr)->GetGUID();
				}
			}			
		}
	}
	return 0;
}

bool Spell::DidHit(uint64 target)
{
	//MEH return true for physical attacks they can not be resisted it's not !!!  spell

	if(m_spellInfo->School == 0)
		return true;
	//note resistchance is vise versa, is full hit chance
	Creature *u_victim = NULL;
	Player *p_victim = NULL;
	switch(UINT32_LOPART(GUID_HIPART(target)))
	{
		case HIGHGUID_UNIT:
			u_victim = m_caster->GetMapMgr()->GetCreature(target);
			break;
		case HIGHGUID_PLAYER:
			p_victim = m_caster->GetMapMgr()->GetPlayer(target);
			break;
		case HIGHGUID_PET:
			u_victim = m_caster->GetMapMgr()->GetPet(target);
			break;
	}

	float baseresist[3]={4.0f,5.0f,6.0f};
	int32 lvldiff;
	float resistchance ;
	if(!u_victim && !p_victim)
		return false;
	if(!p_caster)
		return true;		// Mob spells hit!


	bool pvp =(p_caster && p_victim);

	if(pvp)
		lvldiff = p_victim->getLevel() - p_caster->getLevel();
	else
		lvldiff = u_victim->getLevel() - p_caster->getLevel();
	if (lvldiff < 0)
	{
		resistchance = baseresist[0] +lvldiff;
	}
	else
	{
		if(lvldiff < 3)
		{ 
				resistchance = baseresist[lvldiff];
		}
		else
		{
			if(pvp)
				resistchance = baseresist[2] + ((lvldiff-2)*7.0f);
			else
				resistchance = baseresist[2] + ((lvldiff-2)*11.0f);
		}
	}
	//check mechanical resistance
	//i have no idea what is the best pace for this code
	if(m_spellInfo->MechanicsType<27)
	{
		if(p_victim)
			resistchance += p_victim->MechanicsResistancesPCT[m_spellInfo->MechanicsType];
		else 
			resistchance += u_victim->MechanicsResistancesPCT[m_spellInfo->MechanicsType];
	}
	//rating bonus
	if(p_caster)
	{
		resistchance -= p_caster->CalcRating(7);
		resistchance -= p_caster->GetHitFromSpell();
	}

	if(p_victim)
		resistchance += p_victim->m_resist_hit[2];


	if(resistchance >= 100.0)
		return false;
	else
	{
		if(resistchance<=1.0)//resist chance >=1
			return !Rand(1.0);
		else
			return !Rand(resistchance);
	}
 
}
//generate possible target list for a spell. Use as last resort since it is not acurate
//this function makes a rough estimation for possible target !
//!!!disabled parts that were not tested !!
void Spell::GenerateTargets(SpellCastTargets *store_buff)
{
	float range = GetMaxRange(dbcSpellRange.LookupEntry(m_spellInfo->rangeIndex));
	if(range==0)
	{
		float tr1=GetRadius(0);
		float tr2=GetRadius(1);
		float tr3=GetRadius(2);
		if(tr1>tr2)
			range=tr1;
		else range=tr2;
		if(tr3>range)
			range=tr3;
	}
	uint32 cur;
	for(uint32 i=0;i<3;i++)
		for(uint32 j=0;j<2;j++)
		{
			if(j==0)
				cur = m_spellInfo->EffectImplicitTargetA[i];
			else // if(j==1)
				cur = m_spellInfo->EffectImplicitTargetB[i];		
			switch(cur)
			{
				case EFF_TARGET_NONE:{
					//this is bad for us :(
					}break;
				case EFF_TARGET_SELF:{
						if(m_caster->IsUnit())
							store_buff->m_unitTarget = m_caster->GetGUID();
					}break;		
					// need more research
				case 4:{ // dono related to "Wandering Plague", "Spirit Steal", "Contagion of Rot", "Retching Plague" and "Copy of Wandering Plague"
					}break;			
				case EFF_TARGET_PET:
					{// Target: Pet
						if(p_caster && p_caster->GetSummon())
							store_buff->m_unitTarget = p_caster->GetSummon()->GetGUID();
					}break;
				case EFF_TARGET_SINGLE_ENEMY:// Single Target Enemy
				case 77:					// grep: i think this fits
				case 8: // related to Chess Move (DND), Firecrackers, Spotlight, aedm, Spice Mortar
				case EFF_TARGET_ALL_ENEMY_IN_AREA: // All Enemies in Area of Effect (TEST)
				case EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT: // All Enemies in Area of Effect instant (e.g. Flamestrike)
				case EFF_TARGET_ALL_ENEMIES_AROUND_CASTER:
				case EFF_TARGET_IN_FRONT_OF_CASTER:
				case EFF_TARGET_ALL_ENEMY_IN_AREA_CHANNELED:// All Enemies in Area of Effect(Blizzard/Rain of Fire/volley) channeled
				case 31:// related to scripted effects
				case 53:// Target Area by Players CurrentSelection()
				case 54:// Targets in Front of the Caster
					{
						if(p_caster)
							store_buff->m_unitTarget = p_caster->GetSelection();
						else if(u_caster)
						{
							if(isAttackable(u_caster,u_caster->GetAIInterface()->GetNextTarget()))
							{
								store_buff->m_unitTarget = u_caster->GetAIInterface()->GetNextTarget()->GetGUID();
							}
							if(u_caster->GetAIInterface()->getAITargetsCount())
							{
								//try to get most hated creature
								TargetMap *m_aiTargets = u_caster->GetAIInterface()->GetAITargets();
								TargetMap::iterator itr;
								float rsq=range*range;
								for(itr = m_aiTargets->begin(); itr != m_aiTargets->end();itr++)
								{
									if( /*m_caster->GetMapMgr()->GetUnit(itr->first->GetGUID()) &&*/ itr->first->GetMapMgr() == m_caster->GetMapMgr() && 
										itr->first->isAlive() &&
										m_caster->GetDistanceSq(itr->first) <= rsq &&
										isAttackable(u_caster,itr->first)
										)
									{
										store_buff->m_unitTarget=itr->first->GetGUID();
										break;
									}
								}
							}
						}
						//try to get a whatever target
						if(!store_buff->m_unitTarget)
						{
							store_buff->m_unitTarget=GetSinglePossibleEnemy(range);
						}
						//if we still couldn't get a target, check maybe we could use 
//						if(!store_buff->m_unitTarget)
//						{
//						}
					}break;
					// spells like 17278:Cannon Fire and 21117:Summon Son of Flame A
				case 17: // A single target at a xyz location or the target is a possition xyz
				case 18:// Land under caster.Maybe not correct
					{
						store_buff->m_srcX=m_caster->GetPositionX();
						store_buff->m_srcY=m_caster->GetPositionY();
						store_buff->m_srcZ=m_caster->GetPositionZ();
						store_buff->m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
					}break;
				case EFF_TARGET_ALL_PARTY_AROUND_CASTER:
					{// All Party Members around the Caster in given range NOT RAID!			
						Player *p=p_caster;
						if(!p)
						{
							if(((Creature*)u_caster)->IsTotem())
								p=(Player*)((Creature*)u_caster)->GetTotemOwner();
						}
						float r= range;
						r*=r;
						if(!p)
							break;

						if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),p,r))
						{
							store_buff->m_unitTarget = m_caster->GetGUID();
							break;
						}
						SubGroup * subgroup = p->GetGroup() ?
							p->GetGroup()->GetSubGroup(p->GetSubGroup()) : 0;

						if(subgroup)
						{
							p->GetGroup()->Lock();
							for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
							{
								if(!itr->player || m_caster == itr->player) 
									continue;
								if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),itr->player,r))
								{
									store_buff->m_unitTarget = itr->player->GetGUID();
									break;
								}
							}
							p->GetGroup()->Unlock();
						}
					}break;
				case EFF_TARGET_SINGLE_FRIEND:
				case 45:// Chain,!!only for healing!! for chain lightning =6 
				case 57:// Targeted Party Member
					{// Single Target Friend
						if(p_caster)
						{
							if(isFriendly(p_caster,p_caster->GetMapMgr()->GetUnit(p_caster->GetSelection())))
								store_buff->m_unitTarget = p_caster->GetSelection();
							else store_buff->m_unitTarget = p_caster->GetGUID();
						}
						else if(u_caster)
						{
							if(u_caster->GetUInt64Value(UNIT_FIELD_CREATEDBY))
								store_buff->m_unitTarget = u_caster->GetUInt64Value(UNIT_FIELD_CREATEDBY);
							else store_buff->m_unitTarget = u_caster->GetGUID();
						}
						else store_buff->m_unitTarget=GetSinglePossibleFriend(range);			
					}break;
				case EFF_TARGET_GAMEOBJECT:
					{
						if(p_caster && p_caster->GetSelection())
							store_buff->m_unitTarget = p_caster->GetSelection();
					}break;
				case EFF_TARGET_DUEL: 
					{// Single Target Friend Used in Duel
						if(p_caster && p_caster->DuelingWith && p_caster->DuelingWith->isAlive() && IsInrange(p_caster,p_caster->DuelingWith,range*range))
							store_buff->m_unitTarget = p_caster->GetSelection();
					}break;
				case EFF_TARGET_GAMEOBJECT_ITEM:{// Gameobject/Item Target
						//shit
					}break;
				case 27:{ // target is owner of pet
					// please correct this if not correct does the caster variablen need a Pet caster variable?
						if(u_caster && u_caster->IsPet())
							store_buff->m_unitTarget = ((Pet*)u_caster)->GetPetOwner()->GetGUID();
					}break;
				case EFF_TARGET_MINION:
				case 73:
					{// Minion Target
						if(m_caster->GetUInt64Value(UNIT_FIELD_SUMMON) == 0)
							store_buff->m_unitTarget = m_caster->GetGUID();
						else store_buff->m_unitTarget = m_caster->GetUInt64Value(UNIT_FIELD_SUMMON);
					}break;
				case 33://Party members of totem, inside given range
				case EFF_TARGET_SINGLE_PARTY:// Single Target Party Member
				case EFF_TARGET_ALL_PARTY: // all Members of the targets party
					{
						Player *p=NULL;
						if(p_caster)
								p=p_caster;
						else if(u_caster && u_caster->GetTypeId()==TYPEID_UNIT && ((Creature*)u_caster)->IsTotem())
								p=(Player*) ((Creature*)u_caster)->GetTotemOwner();
						if(p_caster)
						{
							float r =range;
							r*=r;
							if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),p,r))
							{
								store_buff->m_unitTarget = p->GetGUID();
								break;
							}
							SubGroup * pGroup = p_caster->GetGroup() ?
								p_caster->GetGroup()->GetSubGroup(p_caster->GetSubGroup()) : 0;

							if( pGroup )
							{
								p_caster->GetGroup()->Lock();
								for(GroupMembersSet::iterator itr = pGroup->GetGroupMembersBegin();
									itr != pGroup->GetGroupMembersEnd(); ++itr)
								{
									if(!itr->player || p == itr->player) 
										continue;
									if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),itr->player,r))
									{
										store_buff->m_unitTarget = itr->player->GetGUID();
										break;
									}
								}
								p_caster->GetGroup()->Unlock();
							}
						}
					}break;
				case 38:{//Dummy Target
					//have no idea
					}break;
				case EFF_TARGET_SELF_FISHING://Fishing
				case 46://Unknown Summon Atal'ai Skeleton
				case 47:// Portal
				case 52:	// Lightwells, etc
					{
						store_buff->m_unitTarget = m_caster->GetGUID();
					}break;
				case 40://Activate Object target(probably based on focus)
				case EFF_TARGET_TOTEM_EARTH:
				case EFF_TARGET_TOTEM_WATER:
				case EFF_TARGET_TOTEM_AIR:
				case EFF_TARGET_TOTEM_FIRE:// Totem
					{
						if(p_caster)
						{
							uint32 slot = m_spellInfo->Effect[i] - SPELL_EFFECT_SUMMON_TOTEM_SLOT1;
							if(p_caster->m_TotemSlots[slot] != 0)
								store_buff->m_unitTarget = p_caster->m_TotemSlots[slot]->GetGUID();
						}
					}break;
				case 61:{ // targets with the same group/raid and the same class
					//shit again
				}break;
			}//end switch
		}//end for
	if(store_buff->m_unitTarget)
		store_buff->m_targetMask |= TARGET_FLAG_UNIT;
	if(store_buff->m_srcX)
		store_buff->m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
	if(store_buff->m_destX)
		store_buff->m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}//end function

void Spell::prepare(SpellCastTargets * targets)
{
	chaindamage=0;
	m_targets = *targets;

	if(!m_triggeredSpell && p_caster && p_caster->CastTimeCheat)
		m_castTime = 0;
	else
	{
		m_castTime = GetCastTime(dbcSpellCastTime.LookupEntry(m_spellInfo->CastingTimeIndex));

		if (m_spellInfo->SpellGroupType && u_caster)
		{
			SM_FIValue(u_caster->SM_FCastTime,(int32*)&m_castTime,m_spellInfo->SpellGroupType);
			SM_PIValue(u_caster->SM_PCastTime,(int32*)&m_castTime,m_spellInfo->SpellGroupType);
		}

		// handle MOD_CAST_TIME
		if(u_caster)
		{
			m_castTime *= float2int32(u_caster->GetFloatValue(UNIT_MOD_CAST_SPEED));
		}
	}


	if(p_caster)
	{
		if(p_caster->cannibalize)
		{
			sEventMgr.RemoveEvents(p_caster, EVENT_CANNIBALIZE);
			p_caster->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
			p_caster->cannibalize = false;
		}
	}
	
	//let us make sure cast_time is within decent range

	if(m_castTime<0)
		m_castTime=0;
	//this is a hax but there is no spell that has more then 10 minutes cast time
	else if(m_castTime>60*10*1000)
		m_castTime=60*10*1000; //we should limit cast time to 10 minutes right ?

	m_timer = m_castTime;
   // if(p_caster)
	 //   m_castTime -= 100;	  // session update time

	SendSpellStart();

	if(!m_triggeredSpell && p_caster && p_caster->CooldownCheat)
		p_caster->ClearCooldownForSpell(m_spellInfo->Id);

	m_spellState = SPELL_STATE_PREPARING;
	
	if(m_triggeredSpell)
		   cancastresult = -1;
	  else
		   cancastresult = CanCast(false);

	if(cancastresult != -1)
	{
		SendCastResult(cancastresult);

		if(m_triggeredByAura)
		{
			SendChannelUpdate(0);
			if(u_caster)
				u_caster->RemoveAura(m_triggeredByAura);
		}
		else
		{
			// HACK, real problem is the way spells are handled
			// when a spell is channeling and a new spell is casted
			// that is a channeling spell, but not triggert by a aura
			// the channel bar/spell is bugged
			if(u_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) > 0 && u_caster->GetCurrentSpell())
			{
				u_caster->GetCurrentSpell()->cancel();
				SendChannelUpdate(0);
				cancel();
				return;
			}
		}
		finish();
		return;
	}
	else
	{
		// start cooldown handler
		if(p_caster && !m_triggeredSpell)
		{
			AddStartCooldown();
		}

		
		if(!i_caster)
		{
			if(p_caster && m_timer > 0 && !m_triggeredSpell)
				p_caster->delayAttackTimer(m_timer+1000);
//				p_caster->setAttackTimer(m_timer + 1000, false);
		}
	}

	//instant cast(or triggered) and not channeling
	if(u_caster && (m_castTime>0 || m_spellInfo->ChannelInterruptFlags) && !m_triggeredSpell)	
	{
		m_castPositionX = m_caster->GetPositionX();
		m_castPositionY = m_caster->GetPositionY();
		m_castPositionZ = m_caster->GetPositionZ();
	
		u_caster->castSpell( this );

		//remove Aurastates required for this spell from caster and target
		//not sure if this is the right spot for this
//		if(m_spellInfo->CasterAuraState)
//			u_caster->RemoveFlag(UNIT_FIELD_AURASTATE,m_spellInfo->CasterAuraState);
	}
	else
		cast(false);
}

void Spell::cancel()
{
	SendInterrupted(0);
	SendCastResult(SPELL_FAILED_INTERRUPTED);

	if(m_spellState == SPELL_STATE_CASTING)
	{
		if(u_caster)
			u_caster->RemoveAura(m_spellInfo->Id);
	
		if(m_timer > 0 || m_delayed)
		{
			if(p_caster && p_caster->IsInWorld())
			{
				Unit *pTarget = p_caster->GetMapMgr()->GetUnit(m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
				if(!pTarget)
					pTarget = p_caster->GetMapMgr()->GetUnit(p_caster->GetSelection());
				  
				if(pTarget)
				{
					pTarget->RemoveAura(m_spellInfo->Id, m_caster->GetGUID());
				}
				if(m_AreaAura)//remove of blizz and shit like this
				{
					
					DynamicObject* dynObj=m_caster->GetMapMgr()->GetDynamicObject(m_caster->GetUInt32Value(UNIT_FIELD_CHANNEL_OBJECT));
					if(dynObj)
					{
						dynObj->RemoveFromWorld(true);
						delete dynObj;
					}
				}

				if(p_caster->GetSummonedObject())
				{
					if(p_caster->GetSummonedObject()->IsInWorld())
						p_caster->GetSummonedObject()->RemoveFromWorld(true);
					// for now..
					ASSERT(p_caster->GetSummonedObject()->GetTypeId() == TYPEID_GAMEOBJECT);
					delete ((GameObject*)(p_caster->GetSummonedObject()));
					p_caster->SetSummonedObject(NULL);
				}
				if (m_timer > 0)
					p_caster->delayAttackTimer(-m_timer);
//				p_caster->setAttackTimer(1000, false);
			 }
		}
		SendChannelUpdate(0);
	}

	//m_spellState = SPELL_STATE_FINISHED;
	finish();
}

void Spell::AddCooldown()
{
	if(p_caster)
	{
		int32 cooltime = m_spellInfo->RecoveryTime;
		if(cooltime && m_spellInfo->SpellGroupType)
		{
			SM_FIValue(u_caster->SM_FCooldownTime, &cooltime, m_spellInfo->SpellGroupType);
			SM_PIValue(u_caster->SM_PCooldownTime, &cooltime, m_spellInfo->SpellGroupType);
		}

		if(cooltime > 0)
			p_caster->AddCooldown(m_spellInfo->Id,cooltime);

		cooltime = m_spellInfo->CategoryRecoveryTime;
		if(cooltime && m_spellInfo->SpellGroupType)
		{
			SM_FIValue(u_caster->SM_FCooldownTime, &cooltime, m_spellInfo->SpellGroupType);
			SM_PIValue(u_caster->SM_PCooldownTime, &cooltime, m_spellInfo->SpellGroupType);
		}

		if(cooltime > 0)
			p_caster->AddCategoryCooldown(m_spellInfo->Category,cooltime);
	}
}

// grep please speak with me about this function
// there are possible expliots
void Spell::AddStartCooldown()
{
	if(p_caster)
	{
		int32 cooltime = m_spellInfo->StartRecoveryTime;
		if(cooltime && m_spellInfo->SpellGroupType)
		{
			SM_FIValue(u_caster->SM_FCooldownTime, &cooltime, m_spellInfo->SpellGroupType);
			SM_PIValue(u_caster->SM_PCooldownTime, &cooltime, m_spellInfo->SpellGroupType);
		}

		if(cooltime > 0)
			p_caster->AddCooldown(m_spellInfo->Id,cooltime);

		cooltime = m_spellInfo->StartRecoveryCategory;
		if(cooltime && m_spellInfo->SpellGroupType)
		{
			SM_FIValue(u_caster->SM_FCooldownTime, &cooltime, m_spellInfo->SpellGroupType);
			SM_PIValue(u_caster->SM_PCooldownTime, &cooltime, m_spellInfo->SpellGroupType);
		}

		if(cooltime > 0)
			p_caster->AddCategoryCooldown(m_spellInfo->Category,cooltime);

		// add spell recover cooldown for visual stuff
		p_caster->AddRecoverCooldown(m_spellInfo);
	}
}

void Spell::cast(bool check)
{
	if(duelSpell && (
		(p_caster && p_caster->GetDuelState() != DUEL_STATE_STARTED) ||
		(u_caster && u_caster->IsPet() && ((Pet*)u_caster)->GetPetOwner()->GetDuelState() != DUEL_STATE_STARTED)))
	{
		// Can't cast that!
		SendInterrupted(SPELL_FAILED_TARGET_FRIENDLY);
		finish();
		return;
	}

	sLog.outDebug("Spell::cast %u, Unit: %u", m_spellInfo->Id, m_caster->GetGUIDLow());
	if(check)
	{
		/*if(m_caster->IsPet())
			castResult = -1;
		else*/
			cancastresult = CanCast(true);
	}
	else 
		cancastresult = -1;

	if(cancastresult == -1)
	{
		if(m_spellInfo->Attributes & ATTRIBUTE_ON_NEXT_ATTACK)
		{
			if(!m_triggeredSpell)
			{
				// on next attack - we don't take the mana till it actually attacks.
				if(!HasPower())
				{
					SendInterrupted(SPELL_FAILED_NO_POWER);
					finish();
					return; 
				}
			}
			else
			{
				// this is the actual spell cast
				if(!TakePower())	// shouldn't happen
				{
					SendInterrupted(SPELL_FAILED_NO_POWER);
					finish();
					return;
				}
			}
		}
		else
		{
			if(!m_triggeredSpell)
			{
				if(!TakePower()) //not enough mana
				{
					//sLog.outDebug("Spell::Not Enough Mana");
					SendInterrupted(SPELL_FAILED_NO_POWER);
					finish();
					return; 
				}
			}
		}
		SendCastResult(cancastresult);
		if(!m_triggeredSpell)
			AddCooldown();
		
		for(uint32 i=0;i<3;i++)
			if(m_spellInfo->Effect[i] && m_spellInfo->Effect[i]!=27)
				 FillTargetMap(i);

		if(p_caster)
		{
			if(m_spellInfo->NameHash == 0xD790D50B)
			{
				/* slam - reset attack timer */
				p_caster->setAttackTimer(0, true);
				p_caster->setAttackTimer(0, false);
			}
			if(p_caster->IsStealth() && !(m_spellInfo->AttributesEx & ATTRIBUTESEX_REMAIN_STEALTHED))
			{
				/* talents procing - don't remove stealth either */
				if(m_spellInfo->Attributes & 64 || (pSpellId && dbcSpell.LookupEntry(pSpellId)->Attributes & 64))
				{

				}
				else
				{
					uint32 stealthid = p_caster->m_stealth;
					p_caster->m_stealth = 0;
					p_caster->RemoveAura(stealthid);
				}
			}
		}

		/*if (m_spellInfo->RecoveryTime && m_caster->IsPlayer() && m_caster->IsInWorld())
		{
			WorldPacket data(SMSG_SPELL_COOLDOWN, 15);
			data << (uint32)m_spellInfo->Id << m_caster->GetNewGUID();
			data << (uint16)m_spellInfo->RecoveryTime; // from dbc
			((Player*)m_caster)->GetSession()->SendPacket (&data);
		}*/

		/*SpellExtraInfo* sp = objmgr.GetSpellExtraData(m_spellInfo->Id);
		if(sp)
		{
			Unit *Target = objmgr.GetUnit(m_targets.m_unitTarget);
			if(Target)
				Target->RemoveBySpecialType(sp->specialtype, p_caster->GetGUID());
		}*/

		if(!(m_spellInfo->Attributes & ATTRIBUTE_ON_NEXT_ATTACK  && !m_triggeredSpell))//on next attack
		{
			SendSpellGo();

			//******************** SHOOT SPELLS ***********************
			//* Flags are now 1,4,19,22 (4718610) //0x480012

			if (m_spellInfo->Flags4 & 0x8000 && m_caster->IsPlayer() && m_caster->IsInWorld())
			{
				WorldPacket data(SMSG_SPELL_COOLDOWN, 15);
				data << (uint32)m_spellInfo->Id << m_caster->GetNewGUID();
				if (m_spellInfo->RecoveryTime)
					data << (uint16)m_spellInfo->RecoveryTime;
				else
					data << (uint16)(2300); // hack fix for shoot spells, should be some other resource for it

				((Player*)m_caster)->GetSession()->SendPacket (&data);
				
				// Hack fix for the channel error on wands
				// this needs to be properly fixed!!!!
			}
			else
			{			
				if(m_spellInfo->ChannelInterruptFlags != 0 && !m_triggeredSpell)
				{
					m_spellState = SPELL_STATE_CASTING;
					SendChannelStart(GetDuration());
					if(p_caster)
					{
						//Use channel interrupt flags here
						if(m_targets.m_targetMask == TARGET_FLAG_DEST_LOCATION || m_targets.m_targetMask == TARGET_FLAG_SOURCE_LOCATION)
							u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSelection());					
						else if(p_caster->GetSelection() == m_caster->GetGUID())
						{
							if(p_caster->GetSummon())
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSummon()->GetGUID());
							else if(m_targets.m_unitTarget)
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, m_targets.m_unitTarget);
							else
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSelection());
						}
						else
						{
							if(p_caster->GetSelection())
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSelection());
							else if(p_caster->GetSummon())
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSummon()->GetGUID());
							else if(m_targets.m_unitTarget)
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, m_targets.m_unitTarget);
							else
							{
								cancel();
								return;
							}
						}
					}
				}
			}

			std::vector<uint64>::iterator i;
			
			// this is here to avoid double search in the unique list
			bool canreflect = false, reflected = false;
			for(int j=0;j<3;j++)
			{
				switch(m_spellInfo->EffectImplicitTargetA[j])
				{
					case 6:
					case 22:
					case 24:
					case 25:
						canreflect = true;
						break;
				}
				if(canreflect)
					continue;
				else
					break;
			}

			if(canreflect && m_caster->IsInWorld())
			{
				for(i= UniqueTargets.begin();i != UniqueTargets.end();i++)
				{
					Unit *Target = m_caster->GetMapMgr()->GetUnit(*i);
					if(Target)
					   reflected = reflect(Target);
					
					if(reflected)
						break;
				}
			}
			bool isDuelEffect = false;
			if(!reflected)
			{
				for(uint32 x=0;x<3;x++)
				{
					if(m_spellInfo->Effect[x])
					{
						isDuelEffect = isDuelEffect ||  m_spellInfo->Effect[x] == SPELL_EFFECT_DUEL;
						if(m_spellInfo->Effect[x] == SPELL_EFFECT_PERSISTENT_AREA_AURA)
							HandleEffects(m_caster->GetGUID(),x);
						else  
						{
	/*						if(m_spellInfo->TargetAuraState)
							{
								for(i= m_targetUnits[x].begin();i != m_targetUnits[x].end();i++)
								{
									HandleEffects((*i),x);
									//do not swap orders or unittarget will not be valid
									Unit *ut=GetUnitTarget();
									if(ut)
										ut->RemoveFlag(UNIT_FIELD_AURASTATE,m_spellInfo->TargetAuraState);
								}
							}
							else*/
							if (m_targetUnits[x].size()>0)
							{
								for(i= m_targetUnits[x].begin();i != m_targetUnits[x].end();i++)
									HandleEffects((*i),x);
							}
							else if(m_spellInfo->Effect[x] == SPELL_EFFECT_TELEPORT_UNITS)
									HandleEffects(m_caster->GetGUID(),x);
						}
					}
				}
	
				/* don't call HandleAddAura unless we actually have auras... - Burlex*/
				if(m_spellInfo->EffectApplyAuraName[0] != 0 || m_spellInfo->EffectApplyAuraName[1] != 0 ||
				   m_spellInfo->EffectApplyAuraName[2] != 0)
				{
					hadEffect = true; // spell has had an effect (for item removal & possibly other things)

					for(i= UniqueTargets.begin();i != UniqueTargets.end();i++)
					{
						HandleAddAura((*i));
					}
				}
				// spells that proc on spell cast, some talents
				if(p_caster && p_caster->IsInWorld() && !m_triggeredSpell)
				{
					for(i= UniqueTargets.begin();i != UniqueTargets.end();i++)
					{
						Unit * Target = p_caster->GetMapMgr()->GetUnit((*i));

						if(!Target)
							continue; //we already made this check, so why make it again ?

						p_caster->HandleProc(PROC_ON_CAST_SPECIFIC_SPELL | PROC_ON_CAST_SPELL,Target, m_spellInfo);
						p_caster->m_procCounter = 0; //this is required for to be able to count the depth of procs (though i have no idea where/why we use proc on proc)
					}
				}
			}
			// we're much better to remove this here, because otherwise spells that change powers etc,
			// don't get applied.

			if(u_caster && !m_triggeredSpell && !m_triggeredByAura)
				u_caster->RemoveAurasByInterruptFlagButSkip(AURA_INTERRUPT_ON_CAST_SPELL, m_spellInfo->Id);

			if(m_spellState != SPELL_STATE_CASTING)
				finish();
		} 
		else //this shit has nothing to do with instant, this only means it will be on NEXT melee hit
		{
			// we're much better to remove this here, because otherwise spells that change powers etc,
			// don't get applied.

			if(u_caster && !m_triggeredSpell && !m_triggeredByAura)
				u_caster->RemoveAurasByInterruptFlagButSkip(AURA_INTERRUPT_ON_CAST_SPELL, m_spellInfo->Id);

			//not sure if it must be there...
			/*if(p_caster)
			{
				if(p_caster->m_onAutoShot)
				{
					p_caster->GetSession()->OutPacket(SMSG_CANCEL_AUTO_REPEAT);
					p_caster->GetSession()->OutPacket(SMSG_CANCEL_COMBAT);
					p_caster->m_onAutoShot = false;
				}
			}*/
			
			SendCastResult(cancastresult);
			if(u_caster)
				if (u_caster->GetOnMeleeSpell() != m_spellInfo->Id)
					u_caster->SetOnMeleeSpell(m_spellInfo->Id);
			finish();
		}
	}
	else
	{
		SendCastResult(cancastresult);
		finish();
	}
}

void Spell::AddTime(uint32 type)
{
	if(u_caster && u_caster->IsPlayer())
	{
		if(m_spellInfo->InterruptFlags & CAST_INTERRUPT_ON_DAMAGE_TAKEN)
		{
			cancel();
			return;
		}
		if(m_spellInfo->SpellGroupType && u_caster)
		{
			float ch=0;
			SM_FFValue(u_caster->SM_PNonInterrupt,&ch,m_spellInfo->SpellGroupType);
			if(Rand(ch))
				return;
		}
		if(p_caster)
		{
			if(Rand(p_caster->SpellDelayResist[type]))
				return;
		}
		if(m_spellState==SPELL_STATE_PREPARING)
		{
			uint32 delay = m_castTime/4;
			m_timer+=delay;
			if(m_timer>m_castTime)
				m_timer=m_castTime;

			WorldPacket data(SMSG_SPELL_DELAYED, 13);
			data << u_caster->GetNewGUID();
			data << uint32(delay);
			u_caster->SendMessageToSet(&data, true);

			if(!p_caster)
			{
				if(m_caster->GetTypeId() == TYPEID_UNIT)
					u_caster->GetAIInterface()->AddStopTime(delay);
			}
			//in case cast is delayed, make sure we do not exit combat 
			else
			{
//				sEventMgr.ModifyEventTimeLeft(p_caster,EVENT_ATTACK_TIMEOUT,PLAYER_ATTACK_TIMEOUT_INTERVAL,true);
				// also add a new delay to offhand and main hand attacks to avoid cutting the cast short
				p_caster->delayAttackTimer(delay);
			}
		}
		else if(m_spellInfo->ChannelInterruptFlags != 48140)
		{
			int32 delay = GetDuration()/3;
			m_timer-=delay;
			p_caster->delayAttackTimer(-delay);
			m_delayed = true;
			if(m_timer>0)
				SendChannelUpdate(m_timer);

		}
	}
}

void Spell::update(uint32 difftime)
{
	// skip cast if we're more than 2/3 of the way through
	if(
		(((float)m_castTime / 1.5f) > (float)m_timer ) && 
//		float(m_castTime)/float(m_timer) >= 2.0f		&&
		(
		m_castPositionX != m_caster->GetPositionX() ||
		m_castPositionY != m_caster->GetPositionY() ||
		m_castPositionZ != m_caster->GetPositionZ()
		)
		)
	{	
		if(u_caster)
		{
			if(u_caster->HasNoInterrupt() == 0 && m_spellInfo->EffectMechanic[1] != 14)
			{
				cancel();
				return;
			}
		}
	}

	if(m_cancelled)
	{
		cancel();
		return;
	}

	switch(m_spellState)
	{
	case SPELL_STATE_PREPARING:
		{
			printf("spell::update m_timer %u, difftime %d, newtime %d\n", m_timer, difftime, m_timer-difftime);
			if((int32)difftime >= m_timer)
				cast(true);
			else 
			{
				m_timer -= difftime;
				if((int32)difftime >= m_timer)
				{
					m_timer = 0;
					cast(true);
				}
			}
			
			
		} break;
	case SPELL_STATE_CASTING:
		{
			if(m_timer > 0)
			{
				if((int32)difftime >= m_timer)
					m_timer = 0;
				else
					m_timer -= difftime;
			}
			if(m_timer <= 0)
			{
				SendChannelUpdate(0);
				finish();
			}
		} break;
	}
}

void Spell::finish()
{
	m_spellState = SPELL_STATE_FINISHED;
	if(u_caster)
	{
		u_caster->m_canMove = true;
 
		if(m_usesMana && !failed && u_caster->GetPowerType()==POWER_TYPE_MANA) 
		{
			u_caster->setPRegenTimer(5000); /* 5 Seconds */
			u_caster->setPIRegenTimer(2000);
		}
	}
	/* Mana Regenerates while in combat but not for 5 seconds after each spell */
	/* Only if the spell uses mana, will it cause a regen delay.
	   is this correct? is there any spell that doesn't use mana that does cause a delay?
	   this is for creatures as effects like chill (when they have frost armor on) prevents regening of mana	*/
	
	//moved to spellhandler.cpp -> remove item when click on it! not when it finishes 

	//enable pvp when attacking another player with spells
	if(p_caster)
	{
		if(m_requiresCP && !failed)
		{
			if(p_caster->m_spellcomboPoints)
			{
				p_caster->m_comboPoints = p_caster->m_spellcomboPoints;
				p_caster->UpdateComboPoints(); //this will make sure we do not use any wrong values here
			}
			else
				p_caster->NullComboPoints();
		}
		if(m_delayed)
		{
			Unit *pTarget = p_caster->GetMapMgr()->GetUnit(m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
			if(!pTarget)
				pTarget = p_caster->GetMapMgr()->GetUnit(p_caster->GetSelection());
			   
			if(pTarget)
			{
				pTarget->RemoveAura(m_spellInfo->Id, m_caster->GetGUID());
			}
		}
	}

	if(m_spellInfo->Effect[0] == SPELL_EFFECT_SUMMON_OBJECT ||
		m_spellInfo->Effect[1] == SPELL_EFFECT_SUMMON_OBJECT ||
		m_spellInfo->Effect[2] == SPELL_EFFECT_SUMMON_OBJECT)
		if(p_caster)
			p_caster->SetSummonedObject(NULL);		
	/*
	We set current spell only if this spell has cast time or is channeling spell
	otherwise it's instant spell and we delete it right after completion
	*/
	
	if(u_caster)
	{
		if(!m_triggeredSpell && (m_spellInfo->ChannelInterruptFlags || m_castTime>0))
			u_caster->SetCurrentSpell(NULL);

		delete this;
	}
}

void Spell::SendCastResult(int16 result)
{
	if(result != -1)
		failed = true;

	if(!m_caster->IsInWorld())
		return;

	if(result != -1)
	{
        Player * plr = p_caster;
        if(!plr && u_caster)
            plr = u_caster->m_redirectSpellPackets;
        if (plr)
        {
            plr->SendCastResult(m_spellInfo->Id, result,(result == SPELL_FAILED_REQUIRES_SPELL_FOCUS) ? m_spellInfo->RequiresSpellFocus : 0);
        }
	}
	//else
	//{
		// result packet sent in handleeffects()
	//}
}

// uint16 0xFFFF
enum SpellStartFlags
{
    //0x01
    SPELL_START_FLAG_DEFAULT = 0x02, // atm set as defailt flag
    //0x04
    //0x08
    //0x10
    SPELL_START_FLAG_RANGED = 0x20,
    //0x40
    //0x80
    //0x100
    //0x200
    //0x400
    //0x800
    //0x1000
    //0x2000
    //0x4000
    //0x8000
};

void Spell::SendSpellStart()
{
	// no need to send this on passive spells
	if(!m_caster->IsInWorld() || m_spellInfo->Attributes & 64 || m_triggeredSpell)
		return;

	WorldPacket data(150);

	uint16 cast_flags = 2;

	if(GetType() == SPELL_TYPE_RANGED)
		cast_flags |= 0x20;

    // hacky yeaaaa
    if (m_spellInfo->Id == 8326) // death
        cast_flags = 0x0F;


	data.SetOpcode(SMSG_SPELL_START);
	if(i_caster)
		data << i_caster->GetNewGUID() << u_caster->GetNewGUID();
	else
		data << m_caster->GetNewGUID() << m_caster->GetNewGUID();

	data << m_spellInfo->Id;
	data << cast_flags;
	data << (uint32)m_castTime;
		
	m_targets.write( data );

	if(GetType() == SPELL_TYPE_RANGED)
	{
		ItemPrototype* ip = NULL;
        if (m_spellInfo->Id == SPELL_RANGED_THROW) // throw
        {
			if(p_caster)
			{
				Item *itm = p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
				if(itm)
				{
	                ip = itm->GetProto();
					/* Throwing Weapon Patch by Supalosa
					p_caster->GetItemInterface()->RemoveItemAmt(it->GetEntry(),1);
					(Supalosa: Instead of removing one from the stack, remove one from durability)
					We don't need to check if the durability is 0, because you can't cast the Throw spell if the thrown weapon is broken, because it returns "Requires Throwing Weapon" or something.
					*/
	
					// burlex - added a check here anyway (wpe suckers :P)
					if(itm->GetDurability() > 0)
	                    itm->SetDurability( itm->GetDurability() - 1 );
				}
				else
				{
					ip = ItemPrototypeStorage.LookupEntry(2512);	/*rough arrow*/
				}
            }
        }
        else if(m_spellInfo->Flags4 & FLAGS4_PLAYER_RANGED_SPELLS)
        {
			if(p_caster)
				ip = ItemPrototypeStorage.LookupEntry(p_caster->GetUInt32Value(PLAYER_AMMO_ID));
			else
				ip = ItemPrototypeStorage.LookupEntry(2512);	/*rough arrow*/
        }
		
		if(ip)
			data << ip->DisplayInfoID << ip->InventoryType;
	}

	m_caster->SendMessageToSet(&data, true);
}

/************************************************************************/
/* General Spell Go Flags, for documentation reasons                    */
/************************************************************************/
enum SpellGoFlags
{
    //0x01
    //0x02
    //0x04
    //0x08
    //0x10
    SPELL_GO_FLAGS_RANGED           = 0x20,
    //0x40
    //0x80
    SPELL_GO_FLAGS_ITEM_CASTER      = 0x100,
    //0x200
    SPELL_GO_FLAGS_EXTRA_MESSAGE    = 0x400, //TARGET MISSES AND OTHER MESSAGES LIKE "Resist"
    //0x800
    //0x1000
    //0x2000
    //0x4000
    //0x8000
};

void Spell::SendSpellGo()
{
    

	// Fill UniqueTargets
	TargetsList::iterator i,j;
	for(uint32 x=0;x<3;x++)
	{
		if(m_spellInfo->Effect[x])
		{
            bool add = true;
			for ( i = m_targetUnits[x].begin(); i != m_targetUnits[x].end(); i++ )
			{
				add = true;
				for(j = UniqueTargets.begin(); j != UniqueTargets.end(); j++ )
				{
					if((*j) == (*i))
					{
						add = false;
						break;
					}
				}
				if(add && (*i) != 0)
					UniqueTargets.push_back((*i));
                //TargetsList::iterator itr = std::unique(m_targetUnits[x].begin(), m_targetUnits[x].end());
                //UniqueTargets.insert(UniqueTargets.begin(),));
                //UniqueTargets.insert(UniqueTargets.begin(), itr);
			}
		}
	}
	
    // no need to send this on passive spells
    if(!m_caster->IsInWorld() || m_spellInfo->Attributes & 64)
        return;

	// Start Spell
	WorldPacket data(200);
	data.SetOpcode(SMSG_SPELL_GO);
	uint16 flags = 0;

	if (GetType() == SPELL_TYPE_RANGED) //ranged
		flags |= 0x20;				    // 0x20 RANGED

	if(i_caster)
		flags |= SPELL_GO_FLAGS_ITEM_CASTER; // 0x100 ITEM CASTER

	if(ModeratedTargets.size() > 0)
		flags |= 0x400;				    // 0x400 TARGET MISSES AND OTHER MESSAGES LIKE "Resist"

    // hacky..
    if (m_spellInfo->Id == 8326)      // death
        flags = SPELL_GO_FLAGS_ITEM_CASTER | 0x0D;
    

	if(i_caster && u_caster)			// this is needed for correct cooldown on items
	{
		data << i_caster->GetNewGUID() << u_caster->GetNewGUID();
	} 
	else 
	{
		data << m_caster->GetNewGUID() << m_caster->GetNewGUID();
	}

	data << m_spellInfo->Id;
	data << flags;
	data << (uint8)(UniqueTargets.size()); //number of hits
	
	if (flags & 0x400)
		data << (uint8)(ModeratedTargets.size()); //number if misses
  
	writeSpellGoTargets(&data);
	
	if (flags & 0x400)
		writeSpellMissedTargets(&data);
	else
		data << uint8(0);

	m_targets.write( data ); // this write is included the target flag

	if (GetType() == SPELL_TYPE_RANGED && p_caster)//ranged
	{
		ItemPrototype* ip=NULL;
		if(m_spellInfo->Id == SPELL_RANGED_THROW)
		{
			if(p_caster)
			{
				Item * it=p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
				if(it)
					ip = it->GetProto();
			}
			else
			{
				ip = ItemPrototypeStorage.LookupEntry(2512);	/*rough arrow*/
			}
        }
		else
		{
			if(p_caster)
				ip = ItemPrototypeStorage.LookupEntry(p_caster->GetUInt32Value(PLAYER_AMMO_ID));
			else // HACK FIX
				ip = ItemPrototypeStorage.LookupEntry(2512);	/*rough arrow*/
		}
		if(ip)
			data << ip->DisplayInfoID << ip->InventoryType;
	}
   
	m_caster->SendMessageToSet(&data, true);

	// spell log execute is still send 2.08
	// as I see with this combination, need to test it more
	//if (flags != 0x120 && m_spellInfo->Attributes & 16) // not ranged and flag 5
	  //  SendLogExecute(0,m_targets.m_unitTarget);
}

void Spell::writeSpellGoTargets( WorldPacket * data )
{
	TargetsList::iterator i;
	for ( i = UniqueTargets.begin(); i != UniqueTargets.end(); i++ )
	{
		SendCastSuccess(*i);
		*data << (*i);
	}
}

void Spell::writeSpellMissedTargets( WorldPacket * data )
{
	/*
	 * The flags at the end known to us so far are.
	 * 1 = Miss
	 * 2 = Resist
	 * 3 = Dodge // mellee only
	 * 4 = Deflect
	 * 5 = Block // mellee only
	 * 6 = Evade
	 * 7 = Immune
	 */
	SpellTargetsList::iterator i;
	if(u_caster && u_caster->isAlive())
	{
		for ( i = ModeratedTargets.begin(); i != ModeratedTargets.end(); i++ )
		{
			*data << (*i).TargetGuid;       // uint64
			*data << (*i).TargetModType;    // uint8
			///handle proc on resist spell
			Unit* target = u_caster->GetMapMgr()->GetUnit((*i).TargetGuid);
			if(target && target->isAlive())
				u_caster->HandleProc(PROC_ON_RESIST_VICTIM,target,m_spellInfo/*,damage*/);		/** Damage is uninitialized at this point - burlex */
		}
	}
	else
		for ( i = ModeratedTargets.begin(); i != ModeratedTargets.end(); i++ )
		{
			*data << (*i).TargetGuid;       // uint64
			*data << (*i).TargetModType;    // uint8
		}
}

void Spell::SendLogExecute(uint32 damage, uint64 & targetGuid)
{
	WorldPacket data(SMSG_SPELLLOGEXECUTE, 37);
	data << m_caster->GetNewGUID();
	data << m_spellInfo->Id;
	data << uint32(1);
//	data << m_spellInfo->SpellVisual;
//	data << uint32(1);
	if (m_caster->GetGUID() != targetGuid)
		data << targetGuid;
	if (damage)
		data << damage;
	m_caster->SendMessageToSet(&data,true);
}

void Spell::SendInterrupted(uint8 result)
{
	if(result != (uint8)-1)
		failed = true;
	if(!m_caster->IsInWorld())
		return;

	WorldPacket data(SMSG_SPELL_FAILURE, 20);

	// send the failure to pet owner if we're a pet
	Player *plr = p_caster;
	if(!plr && m_caster->IsPet())
		plr = static_cast<Pet*>(m_caster)->GetPetOwner();
	if(!plr && u_caster)
		plr = u_caster->m_redirectSpellPackets;

	if(plr&&plr->IsPlayer())
	{
		data << m_caster->GetNewGUID();
		data << m_spellInfo->Id;
		data << uint8(result);
		plr->GetSession()->SendPacket(&data);
	}

	data.Initialize(SMSG_SPELL_FAILED_OTHER);
	data << m_caster->GetNewGUID();
	data << m_spellInfo->Id;
	m_caster->SendMessageToSet(&data, false);
}

void Spell::SendChannelUpdate(uint32 time)
{
	if(time == 0)
	{
		if(u_caster && u_caster->IsInWorld())
		{
			DynamicObject* dynObj=u_caster->GetMapMgr()->GetDynamicObject(u_caster->GetUInt32Value(UNIT_FIELD_CHANNEL_OBJECT));
			if(dynObj)
			{
				dynObj->RemoveFromWorld(true);
				delete dynObj;
			}
			u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT,0);
			u_caster->SetUInt32Value(UNIT_CHANNEL_SPELL,0);
		}
	}

	if (!p_caster)
		return;

	WorldPacket data(MSG_CHANNEL_UPDATE, 18);
	data << p_caster->GetNewGUID();
	data << time;

	p_caster->SendMessageToSet(&data, true);	
}

void Spell::SendChannelStart(uint32 duration)
{
	if (m_caster->GetTypeId() != TYPEID_GAMEOBJECT)
	{
		// Send Channel Start
		WorldPacket data(MSG_CHANNEL_START, 22);
		data << m_caster->GetNewGUID();
		data << m_spellInfo->Id;
		data << duration;
		m_caster->SendMessageToSet(&data, true);
	}

	m_castTime = m_timer = duration;

	if(u_caster)
		u_caster->SetUInt32Value(UNIT_CHANNEL_SPELL,m_spellInfo->Id);

	/*
	Unit* target = objmgr.GetCreature(((Player*)m_caster)->GetSelection());
	if(!target)
		target = objmgr.GetObject<Player>(((Player*)m_caster)->GetSelection());
	if(!target)
		return;
 
	m_caster->SetUInt32Value(UNIT_FIELD_CHANNEL_OBJECT,target->GetGUIDLow());
	m_caster->SetUInt32Value(UNIT_FIELD_CHANNEL_OBJECT+1,target->GetGUIDHigh());
	//disabled it can be not only creature but GO as well
	//and GO is not selectable, so this method will not work
	//these fields must be filled @ place of call
	*/
}

void Spell::SendResurrectRequest(Player* target)
{
	WorldPacket data(SMSG_RESURRECT_REQUEST, 13);
	data << m_caster->GetGUID();
	data << uint32(0) << uint8(0);

	target->GetSession()->SendPacket(&data);
}

bool Spell::HasPower()
{
	int32 powerField;
	if(u_caster)
		if(u_caster->HasFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_TRAINER))
			return true;

	if(p_caster && p_caster->PowerCheat)
		return true;

	switch(m_spellInfo->powerType)
	{
	case POWER_TYPE_HEALTH:{
		powerField = UNIT_FIELD_HEALTH;
						   }break;
	case POWER_TYPE_MANA:{
		powerField = UNIT_FIELD_POWER1;
		m_usesMana=true;
						 }break;
	case POWER_TYPE_RAGE:{
		powerField = UNIT_FIELD_POWER2;
						 }break;
	case POWER_TYPE_FOCUS:{
		powerField = UNIT_FIELD_POWER3;
						  }break;
	case POWER_TYPE_ENERGY:{
		powerField = UNIT_FIELD_POWER4;
						   }break;
	default:{
		sLog.outDebug("unknown power type");
		// we should'nt be here to return
		return false;
			}break;
	}
	

	//FIXME: add handler for UNIT_FIELD_POWER_COST_MODIFIER
	//UNIT_FIELD_POWER_COST_MULTIPLIER
	if(u_caster)
	{
		if( m_spellInfo->AttributesEx & 2 ) // Uses %100 mana
		{
			m_caster->SetUInt32Value(powerField, 0);
			return true;
		}
	}

	int32 currentPower = m_caster->GetUInt32Value(powerField);

	int32 cost;
	if(m_spellInfo->ManaCostPercentage)//Percentage spells cost % of !!!BASE!!! mana
	{
		if(m_spellInfo->powerType==POWER_TYPE_MANA)
			cost = (m_caster->GetUInt32Value(UNIT_FIELD_BASE_MANA)*m_spellInfo->ManaCostPercentage)/100;
		else
			cost = (m_caster->GetUInt32Value(UNIT_FIELD_BASE_HEALTH)*m_spellInfo->ManaCostPercentage)/100;
	}
	else 
	{
		cost = m_spellInfo->manaCost;
	}

	if(m_spellInfo->powerType==POWER_TYPE_HEALTH)
		cost -= m_spellInfo->baseLevel;//FIX for life tap	
	else if(u_caster)
	{
		if(m_spellInfo->powerType==POWER_TYPE_MANA)
			cost += u_caster->PowerCostMod[m_spellInfo->School];//this is not percent!
		else
			cost += u_caster->PowerCostMod[0];
		cost +=float2int32(cost*u_caster->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER+m_spellInfo->School));
	}

	//apply modifiers
	if(m_spellInfo->SpellGroupType && u_caster)
	{
		SM_FIValue(u_caster->SM_FCost,&cost,m_spellInfo->SpellGroupType);
		SM_PIValue(u_caster->SM_PCost,&cost,m_spellInfo->SpellGroupType);
	}

	if (cost <=0)
		return true;

	//FIXME:DK:if field value < cost what happens
	if(powerField == UNIT_FIELD_HEALTH)
	{
		return true;
	}
	else
	{
		if(cost <= currentPower) // Unit has enough power (needed for creatures)
		{
			return true;
		}
		else
			return false;	 
	} 
}

bool Spell::TakePower()
{
	int32 powerField;
	if(u_caster)
	if(u_caster->HasFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_TRAINER))
		return true;

	if(p_caster && p_caster->PowerCheat)
		return true;

	switch(m_spellInfo->powerType)
	{
		case POWER_TYPE_HEALTH:{
			powerField = UNIT_FIELD_HEALTH;
							   }break;
		case POWER_TYPE_MANA:{
			powerField = UNIT_FIELD_POWER1;
			m_usesMana=true;
							 }break;
		case POWER_TYPE_RAGE:{
			powerField = UNIT_FIELD_POWER2;
							 }break;
		case POWER_TYPE_FOCUS:{
			powerField = UNIT_FIELD_POWER3;
							  }break;
		case POWER_TYPE_ENERGY:{
			powerField = UNIT_FIELD_POWER4;
							  }break;
		default:{
			sLog.outDebug("unknown power type");
			// we should'nt be here to return
			return false;
				}break;
	}

	//FIXME: add handler for UNIT_FIELD_POWER_COST_MODIFIER
	//UNIT_FIELD_POWER_COST_MULTIPLIER
	if(u_caster)
	{
		if( m_spellInfo->AttributesEx & 2 ) // Uses %100 mana
		{
			m_caster->SetUInt32Value(powerField, 0);
			return true;
		}
	}
	   
	int32 currentPower = m_caster->GetUInt32Value(powerField);

	int32 cost;
	if(m_spellInfo->ManaCostPercentage)//Percentage spells cost % of !!!BASE!!! mana
	{
		if(m_spellInfo->powerType==POWER_TYPE_MANA)
			cost = (m_caster->GetUInt32Value(UNIT_FIELD_BASE_MANA)*m_spellInfo->ManaCostPercentage)/100;
		else
			cost = (m_caster->GetUInt32Value(UNIT_FIELD_BASE_HEALTH)*m_spellInfo->ManaCostPercentage)/100;
	}
	else 
	{
		cost = m_spellInfo->manaCost;
	}
	
	if(m_spellInfo->powerType==POWER_TYPE_HEALTH)
			cost -= m_spellInfo->baseLevel;//FIX for life tap	
	else if(u_caster)
	{
		if(m_spellInfo->powerType==POWER_TYPE_MANA)
			cost += u_caster->PowerCostMod[m_spellInfo->School];//this is not percent!
		else
			cost += u_caster->PowerCostMod[0];
		cost +=float2int32(cost*u_caster->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER+m_spellInfo->School));
	}

	//apply modifiers
	if(m_spellInfo->SpellGroupType && u_caster)
	{
		  SM_FIValue(u_caster->SM_FCost,&cost,m_spellInfo->SpellGroupType);
		  SM_PIValue(u_caster->SM_PCost,&cost,m_spellInfo->SpellGroupType);
	}
		 
/*
int spell_flat_modifers=0;
int spell_pct_modifers=0;
SM_FIValue(u_caster->SM_FCost,&spell_flat_modifers,m_spellInfo->SpellGroupType);
SM_FIValue(u_caster->SM_PCost,&spell_pct_modifers,m_spellInfo->SpellGroupType);
printf("!!!!!spell cost mod flat %d , spell value mod pct %d , spell dmg %d, spell group %u\n",spell_flat_modifers,spell_pct_modifers,cost,m_spellInfo->SpellGroupType);
*/

	if (cost <=0)
		return true;

	//FIXME:DK:if field value < cost what happens
	if(powerField == UNIT_FIELD_HEALTH)
	{
		m_caster->DealDamage(u_caster, cost, 0, 0, 0,true);
		return true;
	}
	else
	{
		if(cost <= currentPower) // Unit has enough power (needed for creatures)
		{
			m_caster->SetUInt32Value(powerField, currentPower - cost);
			return true;
		}
		else
			return false;	 
	} 
}

void Spell::HandleEffects(uint64 guid, uint32 i)
{   
	if(guid == m_caster->GetGUID())
	{
		unitTarget = u_caster;
		gameObjTarget = g_caster;
		playerTarget = p_caster;
		itemTarget = i_caster;
	}
	else
	{
		if(!m_caster->IsInWorld())
		{
			unitTarget = 0;
			playerTarget = 0;
			itemTarget = 0;
			gameObjTarget = 0;
			corpseTarget = 0;
		}
		else if(m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
		{
			if(p_caster)
			{
				Player * plr = p_caster->GetTradeTarget();
				if(plr)
					itemTarget = plr->getTradeItem(guid);
			}
		}
		else
		{
			unitTarget = NULL;
			switch(UINT32_LOPART(GUID_HIPART(guid)))
			{
			case HIGHGUID_UNIT:
				unitTarget = m_caster->GetMapMgr()->GetCreature(guid);
				break;
			case HIGHGUID_PET:
				unitTarget = m_caster->GetMapMgr()->GetPet(guid);
				break;
			case HIGHGUID_PLAYER:
				{
					unitTarget =  m_caster->GetMapMgr()->GetPlayer(guid);
					playerTarget = static_cast<Player*>(unitTarget);
				}break;
			case HIGHGUID_ITEM:
				if(p_caster)
					itemTarget = p_caster->GetItemInterface()->GetItemByGUID(guid);

				break;
			case HIGHGUID_GAMEOBJECT:
				gameObjTarget = m_caster->GetMapMgr()->GetGameObject(guid);
				break;
			case HIGHGUID_CORPSE:
				corpseTarget = objmgr.GetCorpse(guid);
				break;
			}
		}
	}	

	damage = CalculateEffect(i);  

	sLog.outDebug( "WORLD: Spell effect id = %u, damage = %d", m_spellInfo->Effect[i], damage); 
	
	if(m_spellInfo->Effect[i]<TOTAL_SPELL_EFFECTS)
	{
		/*if(unitTarget && p_caster && isAttackable(p_caster,unitTarget))
			sEventMgr.ModifyEventTimeLeft(p_caster,EVENT_ATTACK_TIMEOUT,PLAYER_ATTACK_TIMEOUT_INTERVAL);*/

		(*this.*SpellEffectsHandler[m_spellInfo->Effect[i]])(i);
	}
	else
		sLog.outError("SPELL: unknown effect %u spellid %u",m_spellInfo->Effect[i], m_spellInfo->Id);
}

void Spell::HandleAddAura(uint64 guid)
{
	Unit * Target = 0;
	if(guid == 0)
		return;

	if(u_caster && u_caster->GetGUID() == guid)
		Target = u_caster;
	else if(m_caster->IsInWorld())
		Target = m_caster->GetMapMgr()->GetUnit(guid);

	if(!Target)
		return;

	// Applying an aura to a flagged target will cause you to get flagged.
    // self casting doesnt flag himself.
	if(Target->IsPlayer() && p_caster && p_caster != static_cast<Player*>(Target))
	{
		if(static_cast<Player*>(Target)->IsPvPFlagged())
			p_caster->SetPvPFlag();
	}

	// remove any auras with same type
	if(m_spellInfo->buffType > 0)
		Target->RemoveAurasByBuffType(m_spellInfo->buffType, m_caster->GetGUID(),0);

	std::map<uint32,Aura*>::iterator itr=Target->tmpAura.find(m_spellInfo->Id);
	if(itr!=Target->tmpAura.end())
	{
		if(itr->second)
		{
			if(itr->second->GetSpellProto()->procCharges>0)
			{
				Aura *aur=NULL;
				for(int i=0;i<itr->second->GetSpellProto()->procCharges-1;i++)
				{
					aur = new Aura(itr->second->GetSpellProto(),itr->second->GetDuration(),itr->second->GetCaster(),itr->second->GetTarget());
					Target->AddAura(aur);
					aur=NULL;
				}
				if(!(itr->second->GetSpellProto()->procFlags & PROC_REMOVEONUSE))
				{
					SpellCharge charge;
					charge.count=itr->second->GetSpellProto()->procCharges;
					charge.spellId=itr->second->GetSpellId();
					charge.ProcFlag=itr->second->GetSpellProto()->procFlags;
					charge.lastproc = 0;charge.FromProc=false;
					Target->m_chargeSpells.insert(make_pair(itr->second->GetSpellId(),charge));
				}
			}
			Target->AddAura(itr->second); // the real spell is added last so the modifier is removed last
			Target->tmpAura.erase(itr);
		}
	}
	uint32 spellid = 0;

	if(m_spellInfo->MechanicsType == 25 && m_spellInfo->Id != 25771) // Cast spell Forbearance
		spellid = 25771;
	else if(m_spellInfo->MechanicsType == 16 && m_spellInfo->Id != 11196) // Cast spell Recently Bandaged
		spellid = 11196;
	else if(m_spellInfo->MechanicsType == 19 && m_spellInfo->Id != 6788) // Cast spell Weakened Soul
		spellid = 6788;
	else if(m_spellInfo->Id == 11958) // Cast spell Hypothermia
		spellid = 41425;
	else if(m_spellInfo->Id == 30451) // Cast spell Arcane Blast
		spellid = 36032;
	else if(m_spellInfo->Id == 20572 || m_spellInfo->Id == 33702 || m_spellInfo->Id == 33697) // Cast spell Blood Fury
		spellid = 23230;

	if(spellid && p_caster)
	{
		SpellEntry *spellInfo = dbcSpell.LookupEntry( spellid );
		if(!spellInfo) return;
		Spell *spell = new Spell(p_caster, spellInfo ,true, NULL);
		SpellCastTargets targets(Target->GetGUID());
		spell->prepare(&targets);	
	}
}

/*
void Spell::TriggerSpell()
{
	if(TriggerSpellId != 0)
	{
		// check for spell id
		SpellEntry *spellInfo = sSpellStore.LookupEntry(TriggerSpellId );

		if(!spellInfo)
		{
			sLog.outError("WORLD: unknown spell id %i\n", TriggerSpellId);
			return;
		}

		Spell *spell = new Spell(m_caster, spellInfo,false, NULL);
		WPAssert(spell);

		SpellCastTargets targets;
		if(TriggerSpellTarget)
			targets.m_unitTarget = TriggerSpellTarget;
		else
			targets.m_unitTarget = m_targets.m_unitTarget;

		spell->prepare(&targets);
	}
}*/
/*
void Spell::DetermineSkillUp()
{
	skilllinespell* skill = objmgr.GetSpellSkill(m_spellInfo->Id);
	if (m_caster->GetTypeId() == TYPEID_PLAYER)
	{
		if ((skill) && ((Player*)m_caster)->HasSkillLine(skill->skilline))
		{
			uint32 amt = ((Player*)m_caster)->GetBaseSkillAmt(skill->skilline);
			uint32 max = ((Player*)m_caster)->GetSkillMax(skill->skilline);
			if (amt >= skill->grey) //grey
			{
			}
			else if ((amt >= (((skill->grey - skill->green) / 2) + skill->green)) && (amt < max))
			{
				if (rand()%100 < 33) //green
					((Player*)m_caster)->AdvanceSkillLine(skill->skilline);	

			}
			else if ((amt >= skill->green) && (amt < max))
			{
				if (rand()%100 < 66) //yellow
					((Player*)m_caster)->AdvanceSkillLine(skill->skilline);		

			}
			else 
			{
				if (amt < max) //brown
					((Player*)m_caster)->AdvanceSkillLine(skill->skilline);
			}
		}
	}
}
*/

bool Spell::IsAspect()
{
	return (
		(m_spellInfo->Id ==  2596) || (m_spellInfo->Id ==  5118) || (m_spellInfo->Id == 14320) || (m_spellInfo->Id == 13159) || (m_spellInfo->Id == 13161) || (m_spellInfo->Id == 20190) || 
		(m_spellInfo->Id == 20043) || (m_spellInfo->Id == 14322) || (m_spellInfo->Id == 14321) || (m_spellInfo->Id == 13163) || (m_spellInfo->Id == 14319) || (m_spellInfo->Id == 14318) || (m_spellInfo->Id == 13165)); 
}

bool Spell::IsSeal()
{
	return (
		(m_spellInfo->Id == 13903) || (m_spellInfo->Id == 17177) || (m_spellInfo->Id == 20154) || (m_spellInfo->Id == 20162) || (m_spellInfo->Id == 20164) || (m_spellInfo->Id == 20165) || 
		(m_spellInfo->Id == 20166) || (m_spellInfo->Id == 20287) || (m_spellInfo->Id == 20288) || (m_spellInfo->Id == 20289) || (m_spellInfo->Id == 20290) || (m_spellInfo->Id == 20291) || 
		(m_spellInfo->Id == 20292) || (m_spellInfo->Id == 20293) || (m_spellInfo->Id == 20305) || (m_spellInfo->Id == 20306) || (m_spellInfo->Id == 20307) || (m_spellInfo->Id == 20308) || 
		(m_spellInfo->Id == 20347) || (m_spellInfo->Id == 20348) || (m_spellInfo->Id == 20349) || (m_spellInfo->Id == 20356) || (m_spellInfo->Id == 20357) || (m_spellInfo->Id == 20375) || 
		(m_spellInfo->Id == 20915) || (m_spellInfo->Id == 20918) || (m_spellInfo->Id == 20919) || (m_spellInfo->Id == 20920) || (m_spellInfo->Id == 21082) || (m_spellInfo->Id == 21084)); 
}

uint8 Spell::CanCast(bool rangetolerate)
{
	if(objmgr.IsSpellDisabled(m_spellInfo->Id))
		return SPELL_FAILED_ERROR;
	
	Unit *target = NULL;
    float maxr = 0;

	if(m_targets.m_unitTarget)
	{
        // checks for players
		if(p_caster)
		{
			if(!rangetolerate && !p_caster->CanCastDueToCooldown(m_spellInfo))
				return SPELL_FAILED_NOT_READY;
        }

		target = (m_caster->IsInWorld()) ? m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget) : NULL;
	 
        // checks that demand a target
		if(target)
		{
            // if the check requere's a player caster
            if (p_caster)
            {
				if(m_spellInfo->EffectApplyAuraName[0] == SPELL_AURA_MOUNTED ||
					m_spellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOUNTED ||
					m_spellInfo->EffectApplyAuraName[2] == SPELL_AURA_MOUNTED)
				{
					if(p_caster->IsMounted())
						return (uint8)SPELL_FAILED_NOT_ON_TRANSPORT;
				}

				if(m_spellInfo->Id == SPELL_RANGED_THROW)
				{
					Item * itm = p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
					if(!itm || itm->GetDurability() == 0)
						return (uint8)SPELL_FAILED_NO_AMMO;
				}

                // if the target should be a player
                if (target->IsPlayer())
                {
                    // disallow spell casting in sanctuary zones
                    // allow attacks in duels
                    if (p_caster->DuelingWith != target && !isFriendly(p_caster, target))
                    {
                        AreaTable * atCaster = dbcArea.LookupEntry(p_caster->GetAreaID());
                        AreaTable * atTarget = dbcArea.LookupEntry(((Player*)target)->GetAreaID());
                        if(atCaster->AreaFlags & 0x800 || atTarget->AreaFlags & 0x800)
                            return SPELL_FAILED_NOT_HERE;
                    }

                    // scripted spell stuff, target is player
                    switch (m_spellInfo->Id)
                    {
                        case 603: //curse of doom, can't be casted on players
                        case 30910:
                            {
                                return SPELL_FAILED_TARGET_IS_PLAYER;
                            }break;
                    }
                }
            }
			
			if(m_spellInfo->EffectApplyAuraName[0]==2)//mind control
			if(m_spellInfo->EffectBasePoints[0])//got level req;
				if((int32)target->getLevel() > m_spellInfo->EffectBasePoints[0]+1 + (p_caster->getLevel() - m_spellInfo->spellLevel))
					return SPELL_FAILED_HIGHLEVEL;
				else if(target->GetTypeId() == TYPEID_UNIT) 
					{ 
						Creature * c = (Creature*)(target);
						if (c&&c->GetCreatureName()&&c->GetCreatureName()->Rank == 3) //boss
							return SPELL_FAILED_HIGHLEVEL;
					} 

			// scripted spell stuff
			switch (m_spellInfo->Id)
			{			
            // disable spell
            case 25997: // Eye for an Eye
			case 38554: //Absorb Eye of Grillok
				{
					// do not allow spell to be cast
					return SPELL_FAILED_SPELL_UNAVAILABLE;
				}
			case 36314: //The Seer's Presence
				{
					// this spell can be cast only on socrethar. Otherwife cool exploit
					if(target->IsPlayer() || !target->IsUnit())
						return SPELL_FAILED_BAD_TARGETS;
					// this spell should be used only for a specific quest on specific monster = "Socrethar"
					if(target->GetEntry()!=20132) //nasty fixed numbers :(
						return SPELL_FAILED_BAD_TARGETS;
				}
			}

            // if the target is not the unit caster and not the masters pet
			if(target != u_caster && !m_caster->IsPet())
			{
                

				// Dummy spells check
				if (m_spellInfo->Id == 4130)// Banish Burning Exile
				{
					if (target->GetEntry()!= 2760) // target needs to be a Burning Exile
						return SPELL_FAILED_BAD_TARGETS;
				}
				if (m_spellInfo->Id == 4131)// Banish Cresting Exile
				{
					if (target->GetEntry()!= 2761) // target needs to be a Cresting Exile
						return SPELL_FAILED_BAD_TARGETS;
				}
				if (m_spellInfo->Id == 4132)// Banish Thundering Exile
				{
					if (target->GetEntry()!= 2762) // target needs to be a Thundering Exile
						return SPELL_FAILED_BAD_TARGETS;
				}
				if (m_spellInfo->Id == 38177) //Blackwhelp Net
				{
					// should only affect Wyrmcult Blackwhelps
					if(target->GetEntry()!= 21387)
						return SPELL_FAILED_BAD_TARGETS;
				}

				/***********************************************************
				* Inface checks, these are checked in 2 ways
				* 1e way is check for damage type, as 3 is always ranged
				* 2e way is trough the data in the extraspell db
				*
				**********************************************************/

				/* burlex: units are always facing the target! */
				if(p_caster && !(m_spellInfo->buffType & SPELL_TYPE_CURSE))
				{
					if (m_spellInfo->Spell_Dmg_Type == 3) // 3 is ranged so we do not need to check, we just need inface
					{   // our spell is a ranged spell
						if (!p_caster->isInFront(target))
							return (uint8)SPELL_FAILED_UNIT_NOT_INFRONT;
					}
					else
					{   // our spell is not a ranged spell

						// get spell extra data
						SpellExtraInfo* SpellExtra = SpellExtraStorage.LookupEntry(m_spellInfo->Id);

						if(SpellExtra)
						{
							// Spell extra infront check
							if (GetType() == SPELL_TYPE_MAGIC && SpellExtra->ExtraFlags & SPELL_EXTRA_INFRONT) // only do spells
								if (!u_caster->isInFront(target))
									return (uint8)SPELL_FAILED_UNIT_NOT_INFRONT;
							// Spell extra Behind check
							if(SpellExtra->ExtraFlags & SPELL_EXTRA_BEHIND)
								if(target->isInFront(u_caster))
									return SPELL_FAILED_NOT_BEHIND;
						}
					}
				}
			}

			// if target is already skinned, don't let it be skinned again
			if (m_spellInfo->Effect[0] == SPELL_EFFECT_SKINNING) // skinning
				if (target->IsUnit() && (((Creature*)target)->Skinned) )
					return SPELL_FAILED_TARGET_UNSKINNABLE;

			// all spells with target 61 need to be in group or raid
			// TODO: need to research this if its not handled by the client!!!
			if (
				m_spellInfo->EffectImplicitTargetA[0] == 61 || 
				m_spellInfo->EffectImplicitTargetA[1] == 61 || 
				m_spellInfo->EffectImplicitTargetA[2] == 61)
			{
				if (target->IsPlayer() && !((Player*)target)->InGroup())
					return SPELL_FAILED_NOT_READY;//return SPELL_FAILED_TARGET_NOT_IN_PARTY or SPELL_FAILED_TARGET_NOT_IN_PARTY;
			}

			// pet's owner stuff
			if (m_spellInfo->EffectImplicitTargetA[0] == 27 || 
				m_spellInfo->EffectImplicitTargetA[1] == 27 || 
				m_spellInfo->EffectImplicitTargetA[2] == 27)
			{
				if (!target->IsPlayer())
					return SPELL_FAILED_TARGET_NOT_PLAYER; //if you are there something is very wrong
			}

			// target 39 is fishing, all fishing spells are handled
			if (m_spellInfo->EffectImplicitTargetA[0] == 39 )//|| 
	//			m_spellInfo->EffectImplicitTargetA[1] == 39 || 
  //			  m_spellInfo->EffectImplicitTargetA[2] == 39)
			{
				uint32 entry = m_spellInfo->EffectMiscValue[0];
				if(entry == GO_FISHING_BOBBER)
				{
					//uint32 mapid = p_caster->GetMapId();
					float px=u_caster->GetPositionX();
					float py=u_caster->GetPositionY();
//					float pz=u_caster->GetPositionZ();
					float orient = m_caster->GetOrientation();
					float posx = 0,posy = 0,posz = 0;
					float co = cos(orient);
					float si = sin(orient);
					MapMgr * map = m_caster->GetMapMgr(); 

					float r;
					for(r=20; r>10; r--)
					{
						posx = px + r * co;
						posy = py + r * si;
						/*if(!(map->GetWaterType(posx,posy) & 1))//water 
							continue;*/
						posz = map->GetWaterHeight(posx,posy);
						if(posz > map->GetLandHeight(posx,posy))//water
							break;
					}
					if(r<=10)
						return SPELL_FAILED_NOT_FISHABLE;

					// if we are already fishing, dont cast it again
					if (p_caster->GetSummonedObject())
						if (p_caster->GetSummonedObject()->GetEntry() == GO_FISHING_BOBBER)
							return SPELL_FAILED_SPELL_IN_PROGRESS;
				}
			}
			
			maxr = GetMaxRange(dbcSpellRange.LookupEntry(m_spellInfo->rangeIndex));
			
			if(m_spellInfo->SpellGroupType && u_caster)
			{
				SM_FFValue(u_caster->SM_FRange,&maxr,m_spellInfo->SpellGroupType);
				SM_PFValue(u_caster->SM_PRange,&maxr,m_spellInfo->SpellGroupType);
			}

			if(rangetolerate)
				maxr *= 1.33f;		
			if(!IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),target, (maxr*maxr+2)))
				return SPELL_FAILED_OUT_OF_RANGE;
		
			if(p_caster)
			{
				if(m_spellInfo->NameHash==0xCCC8A100)// Gouge
					if(!target->isInFront(p_caster))
						return SPELL_FAILED_NOT_INFRONT;

				if(m_spellInfo->Category==1131)//Hammer of wrath, requires target to have 20- % of hp
				{
					if(target->GetUInt32Value(UNIT_FIELD_HEALTH) == 0)
						return SPELL_FAILED_BAD_TARGETS;

					if(target->GetUInt32Value(UNIT_FIELD_MAXHEALTH)/target->GetUInt32Value(UNIT_FIELD_HEALTH)<5)
						 return SPELL_FAILED_BAD_TARGETS;
					
				}
				else if(m_spellInfo->Category==672)//Conflagrate, requires immolation spell on victim
				{
					if(!target->HasAura(46))
						return SPELL_FAILED_BAD_TARGETS;
				}

				if(target->dispels[m_spellInfo->DispelType])
					return (uint8)SPELL_FAILED_PREVENTED_BY_MECHANIC;
				
				if(target->MechanicsDispels[m_spellInfo->MechanicsType])
					return (uint8)SPELL_FAILED_PREVENTED_BY_MECHANIC;
			}

			// if we're replacing a higher rank, deny it
			AuraCheckResponse acr = target->AuraCheck(m_spellInfo->NameHash, m_spellInfo->RankNumber);
			if(acr.Error == AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT)
				return (uint8)SPELL_FAILED_AURA_BOUNCED;

			//check if we are trying to stealth or turn invisible but it is not allowed right now
			if(IsStealthSpell() || IsInvisibilitySpell())
			{
				//if we have Faerie Fire, we cannot stealth or turn invisible
				if(u_caster->HasNegativeAuraWithNameHash(2787586002UL))
					return SPELL_FAILED_CANT_STEALTH;
			}

			/*SpellReplacement*rp=objmgr.GetReplacingSpells(m_spellInfo->Id);
			if(rp)
			{
				if(isAttackable(u_caster,target))//negative, replace only our own spell
				{
					for(uint32 x=0;x<rp->count;x++)
					{
						if(target->HasActiveAura(rp->spells[x],m_caster->GetGUID()))
						{
							return SPELL_FAILED_AURA_BOUNCED;
						}
					}
				}
				else
				{
					for(uint32 x=0;x<rp->count;x++)
					{
						if(target->HasActiveAura(rp->spells[x]))
						{
							return SPELL_FAILED_AURA_BOUNCED;
						}
					}
				}
			}	*/		
		}
	}	
	if(m_targets.m_targetMask==TARGET_FLAG_DEST_LOCATION)
	{
		maxr = GetMaxRange (dbcSpellRange.LookupEntry(m_spellInfo->rangeIndex));
		if(!IsInrange(m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,m_caster, (maxr*maxr+2)))
			return SPELL_FAILED_OUT_OF_RANGE;
	}

	if(p_caster)
	{
		if (p_caster->m_bgHasFlag)
		{
			switch(m_spellInfo->Id)
			{
			case 1784:
			case 1785:
			case 1786:
			case 1787:
			case 5215:
			case 6783:
			case 9913:
			case 1856:
			case 1857:
			case 26889:
				{
					// Player has the flag in WSG, he cant stealth now.
					return SPELL_FAILED_SPELL_UNAVAILABLE;
				}
				break;
			default:
				break;
			}
		}
		if( m_spellInfo->RequiresSpellFocus)
		{
			if(m_spellInfo->rangeIndex == 1) // somehow range that is needed for brazers for cooking is not in the range spell
				maxr = 3.0f;
			bool found = false;
			for(std::set<Object*>::iterator itr = p_caster->GetInRangeSetBegin(); itr != p_caster->GetInRangeSetEnd(); itr++ )
			{
				if((*itr)->GetTypeId() != TYPEID_GAMEOBJECT)
					continue;
				if((*itr)->GetUInt32Value( GAMEOBJECT_TYPE_ID) != GAMEOBJECT_TYPE_SPELL_FOCUS)
					continue;
				if(!IsInrange(p_caster->GetPositionX(),p_caster->GetPositionY(),
					p_caster->GetPositionZ(),(*itr),maxr*maxr))
					continue;
				GameObjectInfo *info = ((GameObject*)(*itr))->GetInfo();
			//	GameObjectNameStorage.LookupEntry((*itr)->GetEntry());
				if(!info)
				{
					sLog.outDebug("Warning: could not find info about game object %u",(*itr)->GetEntry());
					continue;
				}
				if(info->SpellFocus == m_spellInfo->RequiresSpellFocus)
				{
					found = true;
					break;
				}
			}
			if(!found)
				return SPELL_FAILED_REQUIRES_SPELL_FOCUS;
		}
	}
	
    // If the caster is a Unit
	if(u_caster)
	{
		if(u_caster->SchoolCastPrevent[m_spellInfo->School])
		{	
			uint32 now_ = getMSTime();
			if(now_ > u_caster->SchoolCastPrevent[m_spellInfo->School])//this limit has expired,remove
				u_caster->SchoolCastPrevent[m_spellInfo->School]=0;
			else 
			{
                // HACK FIX
				switch (m_spellInfo->NameHash)
				{
				case 0x768F3B4B: //Ice Block
				case 0x9840A1A6: //Divine Shield
					break;
				case 0x3DFA70E5: //Will of the Forsaken
					{
						if (u_caster->m_special_state & (UNIT_STATE_FEAR | UNIT_STATE_CHARM | UNIT_STATE_SLEEP))
							break;
					}break;
				case 0xF60291F4: //Death Wish
				case 0xD77038F4: //Fear Ward
				case 0x19700707: //Berserker Rage
					{
						if (u_caster->m_special_state & UNIT_STATE_FEAR)
							break;
					}break;
				default:
					return SPELL_FAILED_SILENCED;
				}
			}
		}

		if(u_caster->m_silenced && m_spellInfo->School != NORMAL_DAMAGE)// can only silence non-physical
			return SPELL_FAILED_SILENCED;

		if(target)
		{
			for(int i=0;i<3;i++) // if is going to cast a spell that breaks stun remove stun auras, looks a bit hacky but is the best way i can find
            {
				if(m_spellInfo->EffectApplyAuraName[i] == 77 && (m_spellInfo->EffectMiscValue[i] == 12 || m_spellInfo->EffectMiscValue[i] == 17))
				{
					for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
						if(target->m_auras[x])
							if(target->m_auras[x]->GetSpellProto()->MechanicsType == m_spellInfo->EffectMiscValue[i])
								target->m_auras[x]->Remove();
				}
            }
		}

		if(u_caster->IsPacified() && m_spellInfo->School == NORMAL_DAMAGE) // only affects physical damage
		{
            // HACK FIX
			switch (m_spellInfo->NameHash)
			{
			case 0x768F3B4B: //Ice Block
			case 0x9840A1A6: //Divine Shield
			case 0x3DFA70E5: //Will of the Forsaken
				{
					if (u_caster->m_special_state & (UNIT_STATE_FEAR | UNIT_STATE_CHARM | UNIT_STATE_SLEEP))
						break;
				}break;
			default:
				return SPELL_FAILED_PACIFIED;
			}
		}

		if(u_caster->IsStunned())
		{
            // HACK FIX
			switch (m_spellInfo->NameHash)
			{
			case 0x768F3B4B: //Ice Block
			case 0x9840A1A6: //Divine Shield
			case 0xCD4CDF55: //Barkskin
				break;
			default:
				return SPELL_FAILED_STUNNED;
			}
		}

        if(u_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) > 0)
        {
			SpellEntry * t_spellInfo = (u_caster->GetCurrentSpell() ? u_caster->GetCurrentSpell()->m_spellInfo : NULL);

            if(!t_spellInfo || !m_triggeredSpell)
                return SPELL_FAILED_SPELL_IN_PROGRESS;
            else if (t_spellInfo)
            {
                if (
                    t_spellInfo->EffectTriggerSpell[0] != m_spellInfo->Id &&
                    t_spellInfo->EffectTriggerSpell[1] != m_spellInfo->Id &&
                    t_spellInfo->EffectTriggerSpell[2] != m_spellInfo->Id
                    )
                {
                    return SPELL_FAILED_SPELL_IN_PROGRESS;
                }
            }
        }
    }

	if(i_caster) // if the caster is an item
	{
		// *** ITEM CHARGES CHECK - Partha ***
		if( i_caster->GetProto()->Spells[0].Charges != 0 && ((int32)i_caster->GetUInt32Value(ITEM_FIELD_SPELL_CHARGES)) == 0 )
			return SPELL_FAILED_NO_CHARGES_REMAIN; // Item has no charges left

		// *** SHAPESHIFT CHECK - Partha ***
		uint8 ss = p_caster->GetShapeShift();

		switch(ss)
		{
			case 0: // normal form
			case FORM_TREE:
			case FORM_BATTLESTANCE:
			case FORM_DEFENSIVESTANCE:
			case FORM_BERSERKERSTANCE:
			case FORM_SHADOW:
			case FORM_STEALTH:
			case FORM_MOONKIN:
				break;

			case FORM_SWIFT:
			case FORM_FLIGHT:
				return SPELL_FAILED_NO_ITEMS_WHILE_SHAPESHIFTED;

			//case FORM_CAT: 
			//case FORM_TRAVEL:
			//case FORM_AQUA:
			//case FORM_BEAR:
			//case FORM_AMBIENT:
			//case FORM_GHOUL:
			//case FORM_DIREBEAR:
			//case FORM_CREATUREBEAR:
			//case FORM_GHOSTWOLF:
			//case FORM_SPIRITOFREDEMPTION:

			default:  // only equipped items allowed in other forms
				if( i_caster->GetProto()->InventoryType == INVTYPE_NON_EQUIP )
					return SPELL_FAILED_NO_ITEMS_WHILE_SHAPESHIFTED;
		}
	}

    /// if we happen to get here we check if the caster is a player and apply check's for items 
	if(p_caster)
		return CheckItems();
	else 
		return uint8(-1);
}

/// we check player items
int8 Spell::CheckItems()
{
    /// we check so we are sure we "are" a player
	if (m_caster->GetTypeId() != TYPEID_PLAYER)
		return int8(-1);

    /// typecast Master to Player
	Player* p_caster = (Player*)m_caster;

	ItemInterface * t_PlayerItemInterface = p_caster->GetItemInterface();

	for(uint32 i=0;i<8;i++)
	{
		if((m_spellInfo->Reagent[i] == 0) || (m_spellInfo->ReagentCount[i] == 0))
			continue;

		if(i_caster && i_caster->GetProto()->ItemId == m_spellInfo->Reagent[i])
			usedItem = 1; // make sure the item being used doesn't get counted as an available reagent 
		else usedItem = 0;

		if (t_PlayerItemInterface->GetItemCount(m_spellInfo->Reagent[i]) - usedItem < m_spellInfo->ReagentCount[i])
			return int8(SPELL_FAILED_ITEM_NOT_FOUND);
	}

    if(m_spellInfo->Totem[0] != 0)
    {
        if(!p_caster->GetItemInterface()->GetItemCount(m_spellInfo->Totem[0]))
            return int8(SPELL_FAILED_ITEM_NOT_FOUND);
    }
    else if(m_spellInfo->Totem[1] != 0)
    {
        if(!p_caster->GetItemInterface()->GetItemCount(m_spellInfo->Totem[1]))
            return int8(SPELL_FAILED_ITEM_NOT_FOUND);
    }

	if(m_targets.m_itemTarget)
	{
		Item *it = 0;
		if(m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
			//only allow some item targetable spells to work in trade windows
			switch(m_spellInfo->Effect[0])
			{
			case SPELL_EFFECT_OPEN_LOCK://lock picking
			case SPELL_EFFECT_ENCHANT_ITEM://enchanting
			case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY://temporary enchanting
				{
					if(p_caster)
					{
						Player * plr = p_caster->GetTradeTarget();
                        if(plr)
							it = plr->getTradeItem(m_targets.m_itemTarget);
					}
				}break;
			default:
				return int8(SPELL_FAILED_BAD_TARGETS);
				break;
			}
		else
			it=((Player*)m_caster)->GetItemInterface()->GetItemByGUID(m_targets.m_itemTarget);

		if(it)
		{
			ItemPrototype *proto=it->GetProto();
			ASSERT(proto);

			if(m_spellInfo->Id==13262) // check for disenchantable item and high enough skill
			{
				if( proto->DisenchantReqSkill < 1 )
					return SPELL_FAILED_CANT_BE_DISENCHANTED;

				if( (int32)p_caster->_GetSkillLineCurrent(SKILL_ENCHANTING) < proto->DisenchantReqSkill )
					return SPELL_FAILED_CANT_BE_DISENCHANTED_SKILL;
			}
			else
			{
				if(m_spellInfo->EquippedItemClass != (uint32)-1)//all items
					if(m_spellInfo->Effect[0] == SPELL_EFFECT_ENCHANT_ITEM)
					{
						if(m_spellInfo->RequiredItemFlags == 0x10000)
						{
							if(proto->InventoryType != INVTYPE_CLOAK)
								return int8(SPELL_FAILED_BAD_TARGETS);
						}
						else if(m_spellInfo->RequiredItemFlags == 0x800)
						{
							if(proto->InventoryType != INVTYPE_FINGER)
								return int8(SPELL_FAILED_BAD_TARGETS);
						}
						else
						{
							if((m_spellInfo->EquippedItemClass != proto->Class)||
							!(m_spellInfo->EquippedItemSubClass & (1 << proto->SubClass)) ||
							(m_spellInfo->RequiredItemFlags && !(m_spellInfo->RequiredItemFlags & (1<<proto->InventoryType))))
								return int8(SPELL_FAILED_BAD_TARGETS);
						}
					}
					else
					{
						if((m_spellInfo->EquippedItemClass != proto->Class)||
							!(m_spellInfo->EquippedItemSubClass & (1 << proto->SubClass)) ||
							
							// replaced the dummy code with correct code
							//(m_spellInfo->dummy && !(m_spellInfo->dummy & (1<<proto->InventoryType))))		
							(m_spellInfo->RequiredItemFlags && !(m_spellInfo->RequiredItemFlags & (1<<proto->InventoryType))))		
								return int8(SPELL_FAILED_BAD_TARGETS);
					}
			}
		}
		else 
			return int8(SPELL_FAILED_ITEM_NOT_FOUND);
	}
	return int8(-1);
}

void Spell::RemoveItems()
{
    /// ITEM CHARGES REMOVAL, created by Partha
	if(i_caster)
	{
		if( i_caster->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > 1 )                  // STACKABLE ITEM
		{
			i_caster->ModUInt32Value(ITEM_FIELD_STACK_COUNT,-1);                    // remove 1 from stack
			i_caster->m_isDirty = true;
		}
		else if( i_caster->GetProto()->Spells[0].Charges < 0                        // EXPENDABLE ITEM
						|| i_caster->GetProto()->Spells[1].Category == 1153 )       // healthstones/mana gems <-- hackfix for wrong db values
		{
			if( ((int32)i_caster->GetUInt32Value(ITEM_FIELD_SPELL_CHARGES)) < -1 ) // Has Charges Remaining
			{
				i_caster->ModUInt32Value(ITEM_FIELD_SPELL_CHARGES,1);               // remove 1 charge
				i_caster->m_isDirty = true;
			}
			else                                                                    // No Charges Remaining
				i_caster->GetOwner()->GetItemInterface()->SafeFullRemoveItemByGuid(i_caster->GetGUID()); // delete item
		}
		else if( i_caster->GetProto()->Spells[0].Charges > 0 )                      // NON-EXPENDABLE ITEM
		{
			i_caster->ModUInt32Value(ITEM_FIELD_SPELL_CHARGES,-1);                  // remove 1 charge
			i_caster->m_isDirty = true;
		}
	} 
    // END -> ITEM CHARGES REMOVAL

	if (m_caster->GetTypeId() != TYPEID_PLAYER)
		return;

	Player* p_caster = (Player*)m_caster;

	if(m_spellInfo->Flags3 == FLAGS3_REQ_RANGED_WEAPON || m_spellInfo->Flags4 & 0x8000) //0x8000 //0x20000
	{
		p_caster->GetItemInterface()->RemoveItemAmt(p_caster->GetUInt32Value(PLAYER_AMMO_ID),1);
	}

	for(uint32 i=0;i<8;i++)
	{
		if(m_spellInfo->Reagent[i])
			p_caster->GetItemInterface()->RemoveItemAmt(m_spellInfo->Reagent[i],m_spellInfo->ReagentCount[i]);
	}
}

int32 Spell::CalculateEffect(uint32 i)
{
	// TODO: Add ARMOR CHECKS; Add npc that have ranged weapons use them;

	// Range checks
 /*   if (m_spellInfo->Id == SPELL_RANGED_GUN) // this includes bow and gun
	{
		if(!u_caster || !unitTarget)
			return 0;

		return ::CalculateDamage(u_caster,unitTarget,RANGED, m_spellInfo->SpellGroupType);//ranged
	}
*/
	int32 value = 0;

	float basePointsPerLevel    = m_spellInfo->EffectRealPointsPerLevel[i];
	float randomPointsPerLevel  = m_spellInfo->EffectDicePerLevel[i];
	int32 basePoints            = m_spellInfo->EffectBasePoints[i] + 1;

	/* Shady: it's so strange cause almost all spells has BPPL!=0 so at lvl70 Fireball takes +280 damage.
	I think it's imbalanced so committed and replaced with dirty fix.
	if (m_caster->IsUnit())
		basePoints += static_cast<Unit*>(m_caster)->getLevel()*basePointsPerLevel; */
	if (u_caster)
	{
		switch (m_spellInfo->NameHash)
		{
		case 0x7424D6B3: //Gift of the Naaru
		case 0xDE1C36C8: //Blood Fury
		case 0xEE91A232: //Mana Tap
		case 0x6632EB62: //Arcane Torrent
			basePoints += u_caster->getLevel()*basePointsPerLevel;
			break;
		}
	}

	int32 randomPoints = m_spellInfo->EffectDieSides[i];
	if(u_caster)
		randomPoints += u_caster->getLevel() * (int32)randomPointsPerLevel;
	
	if(randomPoints <= 1)
		value = basePoints;
	else
		value = basePoints + rand() % randomPoints;

    // druid passive forms
	if(m_spellInfo->Id ==3025 ||m_spellInfo->Id==9635 || m_spellInfo->Id == 1178 || m_spellInfo->Id == 24905)
	{
		if(u_caster)
			value += float2int32(m_spellInfo->EffectRealPointsPerLevel[i]*(u_caster->getLevel()-m_spellInfo->baseLevel));
	}
	//scripted shit
	else if(m_spellInfo->Id == 34120)
	{	//A steady shot that causes ${$RAP*0.3+$m1} damage. 
		if(i==0 && u_caster)
			value += (uint32)(u_caster->GetRAP()*0.3);
	}
    // HACK FIX
    else if(m_spellInfo->Id == 34428 || m_spellInfo->Id ==23881 ||m_spellInfo->Id == 23892 || m_spellInfo->Id==23893 ||m_spellInfo->Id == 23894||
		    m_spellInfo->Id == 25251 || m_spellInfo->Id == 30335)
	{//causing ${$AP*$m1/100} damage
		if(i==0 && u_caster)
			value = (value*u_caster->GetAP())/100;
	}
	else if(m_spellInfo->NameHash==0xF4D5F002)
	{
		//causes ${$RAP*0.2+$m1} Arcane damage." 
		if(i==0 && u_caster)
			value +=u_caster->GetRAP()*0.15f;
	}else if(m_spellInfo->NameHash == 0x93C04185)//rake
	{
		if(u_caster) {
			if(i==0)
				value+=u_caster->GetAP()/100;
			else if(i==1)
			{
				value = (uint32)(value*3+ u_caster->GetAP()*0.06);
			}
		}
	}else if(m_spellInfo->NameHash == 0x5F076E9E)//Mongoose Bite
	{// ${$AP*0.2+$m1} damage.
		if(u_caster)
			value+=u_caster->GetAP()/5;
	}
	
	if(p_caster)
	{	
		if(m_spellInfo->NameHash==0xCD88AA0D && i == 0 )//Envenom
		{
			value *= p_caster->m_comboPoints;
			value += (uint32)(p_caster->GetAP()*(0.03f*p_caster->m_comboPoints));
			m_requiresCP=true;
		}
		int32 comboDamage = (int32)m_spellInfo->EffectPointsPerComboPoint[i];
	
		if(comboDamage)
		{
			
			if(m_spellInfo->NameHash==0xC8146271) //Eviscerate
			value += (uint32)(p_caster->GetAP()*(0.03*p_caster->m_comboPoints));

			value += (comboDamage * p_caster->m_comboPoints);
			m_requiresCP=true;
			//this is ugly so i willexplain the case maybe someone ha a better idea :
			// while casting a spell talent will trigger uppon the spell prepare faze
			// the effect of the talent is to add 1 combo point but when triggering spell finishes it will clear the extra combo point
			if(p_caster)
				p_caster->m_spellcomboPoints = 0;
		}

		SpellOverrideMap::iterator itr = p_caster->mSpellOverrideMap.find(m_spellInfo->Id);
		if(itr != p_caster->mSpellOverrideMap.end())
		{
			ScriptOverrideList::iterator itrSO;
			for(itrSO = itr->second->begin(); itrSO != itr->second->end(); ++itrSO)
			{
				//DK:FIXME->yeni bir map oluþtur
                // Capt: WHAT THE FUCK DOES THIS MEAN....
				value += (sRand.randIntInternal(true)%(*itrSO)->damage);
			}
		}
	 }

	if(u_caster)
	{
		int32 spell_flat_modifers=0;
		int32 spell_pct_modifers=0;
		int32 spell_pct_modifers2=0;//separated from the other for debugging purpuses
		SM_FIValue(u_caster->SM_FSPELL_VALUE,&spell_flat_modifers,m_spellInfo->SpellGroupType);
		SM_FIValue(u_caster->SM_PSPELL_VALUE,&spell_pct_modifers,m_spellInfo->SpellGroupType);
		//it seems there is no Flat value only pct values for this mod. Maybe later ? Right now it is intended for testing
		SM_FIValue(u_caster->SM_PDOT,&spell_pct_modifers2,m_spellInfo->SpellGroupType);
	
//printf("!!!!spell value mod flat %d , spell value mod pct %d, spell value mod pct2 %d , spell dmg %d, spell group %u\n",spell_flat_modifers,spell_pct_modifers,spell_pct_modifers2,value,m_spellInfo->SpellGroupType);
	
		value = value + value*(spell_pct_modifers+spell_pct_modifers2)/100 + spell_flat_modifers;
	}

	return value;
}

void Spell::HandleTeleport(uint32 id, Unit* Target)
{
	if(Target->GetTypeId()!=TYPEID_PLAYER)
		return;

	Player* pTarget = (Player*)Target;
   
	float x,y,z;
	uint32 mapid;
	
	if (m_spellInfo->Id == 8690 ||m_spellInfo->Id == 556)//556- Astral Recall
	{
		x = pTarget->GetBindPositionX();
		y = pTarget->GetBindPositionY();
		z = pTarget->GetBindPositionZ();
		mapid = pTarget->GetBindMapId();
	}
	else
	{
		/*std::stringstream query;
		query << "SELECT * FROM recall where guid = " << m_spellInfo->Id;
		QueryResult *result = sDatabase.Query( query.str( ).c_str( ) );
		
		if( !result )
			return;

		Field *fields = result->Fetch();
		mapid = fields[3].GetUInt32();
		x = fields[4].GetUInt32();
		y = fields[5].GetUInt32();
		z = fields[6].GetUInt32();*/
		TeleportCoords* TC = TeleportCoordStorage.LookupEntry(id);
		if(!TC)
			return;
		 
		x=TC->x;
		y=TC->y;
		z=TC->z;
		mapid=TC->mapId;
	}   

	pTarget->EventAttackStop();
	pTarget->SetSelection(0);   
	  
	//pTarget->Relocate(mapid, x, y, z, pTarget->GetOrientation(), true, false);

	// We have to use a teleport event on this one. Reason being because of UpdateCellActivity,
	// the game object set of the updater thread WILL Get messed up if we teleport from a gameobject
	// caster.

	if(!sEventMgr.HasEvent(pTarget, EVENT_PLAYER_TELEPORT))
		sEventMgr.AddEvent(pTarget, &Player::EventTeleport, mapid, x, y, z, EVENT_PLAYER_TELEPORT, 1, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void Spell::CreateItem(uint32 itemId)
{
    if(!itemId)
        return;

	Player *    pUnit = (Player*)m_caster;
	Item *      newItem;
	Item *      add;
	SlotResult  slotresult;
	ItemPrototype *m_itemProto;

	m_itemProto = ItemPrototypeStorage.LookupEntry(itemId);
	if (!m_itemProto)
	    return;

	if (pUnit->GetItemInterface()->CanReceiveItem(m_itemProto, 1))
	{
		SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
		return;
	}

	add = pUnit->GetItemInterface()->FindItemLessMax(itemId, 1, false);
	if (!add)
	{
		slotresult = pUnit->GetItemInterface()->FindFreeInventorySlot(m_itemProto);
		if(!slotresult.Result)
		{
			 SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
			 return;
		}
		
		newItem = objmgr.CreateItem(itemId, pUnit);
		bool result = pUnit->GetItemInterface()->SafeAddItem(newItem, slotresult.ContainerSlot, slotresult.Slot);
		if(!result)
		{
			delete newItem;
			return;
		}

		newItem->SetUInt64Value(ITEM_FIELD_CREATOR,m_caster->GetGUID());
		newItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, damage);

		WorldPacket data(45);
		p_caster->GetSession()->BuildItemPushResult(&data, p_caster->GetGUID(), 1, 1, itemId ,0,0xFF,1,0xFFFFFFFF);
		p_caster->SendMessageToSet(&data, true);
		newItem->m_isDirty = true;

	} 
	else 
	{
		add->SetUInt32Value(ITEM_FIELD_STACK_COUNT,add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + damage);
		WorldPacket data(45);
		p_caster->GetSession()->BuildItemPushResult(&data, p_caster->GetGUID(), 1, 1, itemId ,0,0xFF,1,0xFFFFFFFF);
		p_caster->SendMessageToSet(&data, true);
		add->m_isDirty = true;
	}
}

/*void Spell::_DamageRangeUpdate()
{
	if(unitTarget)
	{
		if(unitTarget->isAlive())
		{
			m_caster->SpellNonMeleeDamageLog(unitTarget,m_spellInfo->Id, damageToHit);
		}
		else
		{	if(u_caster)
			if(u_caster->GetCurrentSpell() != this)
			{
					if(u_caster->GetCurrentSpell() != NULL)
					{
						u_caster->GetCurrentSpell()->SendChannelUpdate(0);
						u_caster->GetCurrentSpell()->cancel();
					}
			}
			SendChannelUpdate(0);
			cancel();
		}
		sEventMgr.RemoveEvents(this, EVENT_SPELL_DAMAGE_HIT);
		delete this;
	}
	else if(gameObjTarget)
	{
		sEventMgr.RemoveEvents(this, EVENT_SPELL_DAMAGE_HIT);
		delete this;
		//Go Support
	}
	else
	{
		sEventMgr.RemoveEvents(this, EVENT_SPELL_DAMAGE_HIT);
		delete this;
	}
}*/

void Spell::SendHealSpellOnPlayer(Object* caster, Object* target, uint32 dmg,bool critical)
{
	if(!caster || !target || !target->IsPlayer())
		return;
	WorldPacket data(SMSG_HEALSPELL_ON_PLAYER, 27);
	// Bur: I know it says obsolete, but I just logged this tonight and got this packet.
	
	data << target->GetNewGUID();
	data << caster->GetNewGUID();
	data << (pSpellId ? pSpellId : m_spellInfo->Id);
	data << uint32(dmg);	// amt healed
	data << uint8(critical);	 //this is critical message

	caster->SendMessageToSet(&data, true);
}

void Spell::SendHealManaSpellOnPlayer(Object * caster, Object * target, uint32 dmg, uint32 powertype)
{
	if(!caster || !target || !target->IsPlayer())
		return;

	WorldPacket data(SMSG_HEALMANASPELL_ON_PLAYER, 30);

	data << target->GetNewGUID();
	data << caster->GetNewGUID();
	data << (pSpellId ? pSpellId : m_spellInfo->Id);
	data << powertype;
	data << dmg;

	caster->SendMessageToSet(&data, true);
}

void Spell::Heal(int32 amount)
{
	if(!unitTarget || !unitTarget->isAlive())
		return;
	
	if(p_caster)
		p_caster->last_heal_spell=m_spellInfo;

    //self healing shouldnt flag himself
	if(p_caster && playerTarget && p_caster != playerTarget)
	{
		// Healing a flagged target will flag you.
		if(playerTarget->IsPvPFlagged())
			p_caster->SetPvPFlag();
	}

	//Make it critical
	bool critical=false;
	if(u_caster)
	{
		SpellCastTime *sd = dbcSpellCastTime.LookupEntry(m_spellInfo->CastingTimeIndex);

		// affect the plus damage by duration
		float castaff = GetCastTime(sd);
		if(castaff > 3500) 
            castaff = 3500;
		else if(castaff < 1500) 
            castaff = 1500;

		float healdoneaffectperc = castaff / 3500;
		
		amount += float2int32(u_caster->HealDoneMod[m_spellInfo->School] * healdoneaffectperc);
		amount += (amount*u_caster->HealDonePctMod[m_spellInfo->School])/100;
		amount += unitTarget->HealTakenMod[m_spellInfo->School];//amt of health that u RECIVE, not heal
		amount += float2int32(unitTarget->HealTakenPctMod[m_spellInfo->School]*amount);

        //Downranking
        if(m_spellInfo->baseLevel > 0 && m_spellInfo->maxLevel > 0 && p_caster)
        {
            float downrank1 = 1.0f;
            if (m_spellInfo->baseLevel < 20)
                downrank1 = 1.0f - (20.0f - float (m_spellInfo->baseLevel) ) * 0.0375f;
           float downrank2 = ( float(m_spellInfo->maxLevel + 5.0f) / float(p_caster->getLevel()) );
           if (downrank2 >= 1 || downrank2 < 0)
                downrank2 = 1.0f;
            healdoneaffectperc *= downrank1 * downrank2;
        }

		float spellCrit = u_caster->spellcritperc + u_caster->SpellCritChanceSchool[m_spellInfo->School];
		if(critical = Rand(spellCrit))
		{
			int32 critbonus = amount / 2;
			if(m_spellInfo->SpellGroupType)
					SM_PIValue(static_cast<Unit*>(u_caster)->SM_PCriticalDamage, &critbonus, m_spellInfo->SpellGroupType);
			amount += critbonus;
			//Shady: does it correct> caster casts heal and proc ..._VICTIM ? 
			// Or mb i'm completely wrong? So if true  - just replace with old string. 
			//u_caster->HandleProc(PROC_ON_SPELL_CRIT_HIT_VICTIM, unitTarget, m_spellInfo, amount);
			//Replaced with following one:
			
			unitTarget->HandleProc(PROC_ON_SPELL_CRIT_HIT_VICTIM, u_caster, m_spellInfo, amount);
			u_caster->HandleProc(PROC_ON_SPELL_CRIT_HIT, unitTarget, m_spellInfo, amount);
		}
		
	}
	if(amount < 0) amount = 0;

	if(p_caster)  
	{
		amount += float2int32(p_caster->SpellHealDoneByInt[m_spellInfo->School] * p_caster->GetUInt32Value(UNIT_FIELD_STAT3));
		amount += float2int32(p_caster->SpellHealDoneBySpr[m_spellInfo->School] * p_caster->GetUInt32Value(UNIT_FIELD_STAT4));

		if(unitTarget->IsPlayer())
		{
			SendHealSpellOnPlayer(p_caster, ((Player*)unitTarget), amount, critical);
		}
		p_caster->m_bgScore.HealingDone += amount;
		if(p_caster->m_bg)
			p_caster->m_bg->UpdatePvPData();
	}
	uint32 curHealth = unitTarget->GetUInt32Value(UNIT_FIELD_HEALTH);
	uint32 maxHealth = unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
	if((curHealth + amount) >= maxHealth)
		unitTarget->SetUInt32Value(UNIT_FIELD_HEALTH, maxHealth);
	else
		unitTarget->ModUInt32Value(UNIT_FIELD_HEALTH, amount);

	if(p_caster)
		p_caster->HandleProc(PROC_ON_CAST_SPECIFIC_SPELL | PROC_ON_CAST_SPELL,p_caster, m_spellInfo);
	
	int doneTarget = 0;

	// add threat
	if(u_caster)
	{
		uint32 base_threat=GetBaseThreat(amount);
		int count = 0;
		Unit *unit;
		std::vector<Unit*> target_threat;
		if(base_threat)
		{
			target_threat.reserve(u_caster->GetInRangeCount()); // this helps speed

			for(std::set<Object*>::iterator itr = u_caster->GetInRangeSetBegin(); itr != u_caster->GetInRangeSetEnd(); ++itr)
			{
				if((*itr)->GetTypeId() != TYPEID_UNIT)
					continue;
				unit = static_cast<Unit*>((*itr));
				if(unitTarget->IsBeingAttackedBy(unit) ||
					unitTarget->getAttackTarget() == unit->GetGUID() ||
					u_caster->IsBeingAttackedBy(unit) ||
					u_caster->getAttackTarget() == unit->GetGUID())
				{
					target_threat.push_back(unit);
					++count;
				}
			}
			if(count == 0)
				count = 1;  // division against 0 protection
			/* 
			When a tank hold multiple mobs, the threat of a heal on the tank will be split between all the mobs.
			The exact formula is not yet known, but it is more than the Threat/number of mobs.
			So if a tank holds 5 mobs and receives a heal, the threat on each mob will be less than Threat(heal)/5.
			Current speculation is Threat(heal)/(num of mobs *2)
			*/
			uint32 threat = base_threat / (count * 2);
				
			for(std::vector<Unit*>::iterator itr = target_threat.begin(); itr != target_threat.end(); ++itr)
			{
				// for now we'll just use heal amount as threat.. we'll prolly need a formula though
				((Unit*)(*itr))->GetAIInterface()->HealReaction(u_caster, unitTarget, threat);

				if((*itr)->GetGUID() == u_caster->getAttackTarget())
					doneTarget = 1;
			}
		}

		if(!u_caster->isInCombat() && unitTarget->isInCombat() && unitTarget->IsInWorld())
		{
			// "get" the caster in combat.
			Unit * add = 0;

			if(unitTarget->getAttackTarget())
				add = unitTarget->GetMapMgr()->GetUnit(unitTarget->getAttackTarget());

			if(!add)
			{
				// try and grab one from the list
				if(target_threat.size())
					add = *target_threat.begin();
			}

			if(add && !add->IsPlayer())
				u_caster->addAttacker(add);
		}
		if(!doneTarget && u_caster->getAttackTarget())
		{
			// this shouldn't happen..
			//printf("wtf in heal()");
			//u_caster->m_AttackTarget->GetAIInterface()->HealReaction(u_caster, unitTarget, threat);
		}
	}   
}

void Spell::DetermineSkillUp(uint32 skillid,uint32 targetlevel)
{
	if(!p_caster)return;
	if(p_caster->GetSkillUpChance(skillid)<0.01)return;//to preven getting higher skill than max
	int32 diff=p_caster->_GetSkillLineCurrent(skillid,false)/5-targetlevel;
	if(diff<0)diff=-diff;
	float chance;
	if(diff<=5)chance=95.0;
	else if(diff<=10)chance=66;
	else if(diff <=15)chance=33;
	else return;
	if(Rand(chance*sWorld.getRate(RATE_SKILLCHANCE)))
		p_caster->_AdvanceSkillLine(skillid, float2int32( 1.0f * sWorld.getRate(RATE_SKILLRATE)));
}

void Spell::DetermineSkillUp(uint32 skillid)
{
	//This code is wrong for creating items and disenchanting. 
	if(!p_caster)return;
	float chance = 0.0f;
	skilllinespell* skill = objmgr.GetSpellSkill(m_spellInfo->Id);
	if ((skill) && ((Player*)m_caster)->_HasSkillLine(skill->skilline))
	{
		uint32 amt = ((Player*)m_caster)->_GetSkillLineCurrent(skill->skilline);
		uint32 max = ((Player*)m_caster)->_GetSkillLineMax(skill->skilline);
		if(amt >= max)
			return;
		if (amt >= skill->grey) //grey
			chance = 0.0f;
		else if ((amt >= (((skill->grey - skill->green) / 2) + skill->green))) //green
			chance = 33.0f;
		else if (amt >= skill->green) //yellow
			chance = 66.0f;
		else //brown
			chance=100.0f;
	}
	if(Rand(chance*sWorld.getRate(RATE_SKILLCHANCE)))
		p_caster->_AdvanceSkillLine(skillid, float2int32( 1.0f * sWorld.getRate(RATE_SKILLRATE)));
}

void Spell::SafeAddTarget(TargetsList* tgt,uint64 guid)
{
	if(guid == 0)
		return;

	for(TargetsList::iterator i=tgt->begin();i!=tgt->end();i++)
		if((*i)==guid)
			return;
	
	tgt->push_back(guid);
}

void Spell::SafeAddMissedTarget(uint64 guid)
{
    for(SpellTargetsList::iterator i=ModeratedTargets.begin();i!=ModeratedTargets.end();i++)
        if((*i).TargetGuid==guid)
        {
            sLog.outDebug("[SPELL] Something goes wrong in spell target system");
            return;
        }

    ModeratedTargets.push_back(SpellTargetMod(guid,2));
}

void Spell::SafeAddModeratedTarget(uint64 guid, uint16 type)
{
	for(SpellTargetsList::iterator i=ModeratedTargets.begin();i!=ModeratedTargets.end();i++)
		if((*i).TargetGuid==guid)
        {
            sLog.outDebug("[SPELL] Something goes wrong in spell target system");
			return;
        }

	ModeratedTargets.push_back(SpellTargetMod(guid, type));
}

bool Spell::reflect(Unit *refunit)
{
	SpellEntry *refspell = NULL;

	// if the spell to reflect is a reflect spell, do nothing.
	for(int i=0; i<3; i++)
		if(m_spellInfo->Effect[i] == 6 && (m_spellInfo->EffectApplyAuraName[i] == 74 || m_spellInfo->EffectApplyAuraName[i] == 28))
			return false;
	for(std::list<struct ReflectSpellSchool*>::iterator i = refunit->m_reflectSpellSchool.begin();i != refunit->m_reflectSpellSchool.end();i++)
	{
		if((*i)->school == -1 || (*i)->school == (int32)m_spellInfo->School)
		{
			if(Rand((float)(*i)->chance))
			{
				//the god blessed special case : mage - Frost Warding = is an augmentation to frost warding
				if((*i)->require_aura_hash && u_caster && !u_caster->HasAurasWithNameHash((*i)->require_aura_hash))
					continue;
				refspell = m_spellInfo;
			}
		}
	}

	if(!refspell || m_caster == refunit) return false;

	Spell *spell = new Spell(refunit, refspell, true, NULL);
	SpellCastTargets targets;
	targets.m_unitTarget = m_caster->GetGUID();
	spell->prepare(&targets);
	return true;
}

void ApplyDiminishingReturnTimer(uint32 * Duration, Unit * Target, SpellEntry * spell)
{
	uint32 status = GetDiminishingGroup(spell->NameHash);
	uint32 Grp = status & 0xFFFF;   // other bytes are if apply to pvp
	uint32 PvE = (status >> 16) & 0xFFFF;

	// Make sure we have a group
	if(Grp == 0xFFFF) return;

	// Check if we don't apply to pve
	if(!PvE && Target->GetTypeId() != TYPEID_PLAYER && !Target->IsPet())
		return;

	// TODO: check for spells that should do this
	float Dur = *Duration;

	switch(Target->m_diminishCount[Grp])
	{
	case 0: // Full effect
		if (Target->IsPlayer() && Dur > 10000)
		{
			Dur = 10000;
		}
		break;
		
	case 1: // Reduced by 50%
		Dur *= 0.5f;
		if (Target->IsPlayer() && Dur > 5000)
		{
			Dur = 5000;
		}
		break;

	case 2: // Reduced by 75%
		Dur *= 0.25f;
		if (Target->IsPlayer() && Dur > 2500)
		{
			Dur = 2500;
		}
		break;

	default:// Target immune to spell
		{
			*Duration = 0;
			return;
		}break;
	}

	// Convert back
	*Duration = FL2UINT(Dur);

	// Reset the diminishing return counter, and add to the aura count (we don't decrease the timer till we
	// have no auras of this type left.
	++Target->m_diminishAuraCount[Grp];
	++Target->m_diminishCount[Grp];
}

void UnapplyDiminishingReturnTimer(Unit * Target, SpellEntry * spell)
{
	uint32 status = GetDiminishingGroup(spell->NameHash);
	uint32 Grp = status & 0xFFFF;   // other bytes are if apply to pvp
	uint32 PvE = (status >> 16) & 0xFFFF;

	// Make sure we have a group
	if(Grp == 0xFFFF) return;

	// Check if we don't apply to pve
	if(!PvE && Target->GetTypeId() != TYPEID_PLAYER && !Target->IsPet())
		return;

	Target->m_diminishAuraCount[Grp]--;

	// start timer decrease
	if(!Target->m_diminishAuraCount[Grp])
	{
		Target->m_diminishActive = true;
		Target->m_diminishTimer[Grp] = 15000;
	}
}

/// Calculate the Diminishing Group. This is based on a name hash.
/// this off course is very hacky, but as its made done in a proper way
/// I leave it here.
uint32 GetDiminishingGroup(uint32 NameHash)
{
	int32 grp = -1;
	bool pve = false;

	switch(NameHash)
	{
	case 0x4219BB33:		// Sap
	case 0xCCC8A100:		// Gouge
		grp = 0;
		break;

	case 0xC76EACCD:		// Cheap Shot
		{
			grp = 1;
			pve = true;
		}break;

	case 0x02663BE6:		// Kidney Shot
		{
			grp = 2;
			pve = true;
		}break;

	case 0x3F73F2FE:		// Bash
		grp = 3;
		break;

	case 0x34C50810:		// Entangling Roots
		grp = 4;
		break;

	case 0xD70F8418:		// Hammer of Justice
		{
			grp = 5;
			pve = true;
		}break;

	case 0xCC6D4182:		// Seal of Justice
		grp = 6;
		break;

	case 0xE30500A1:		// Charge
	case 0x6B84435E:		// Intercept
	case 0xEC828A40:		// Concussion Blow
		{
			grp = 7;
			pve = true;
		}break;

	case 0x328E44DC:		// Fear
	case 0xC264D626:		// Seduction
	case 0x482156A0:		// Howl of Terror
		grp = 8;
		break;

	case 0xB358A86B:		// Frost Nova
		grp = 9;
		break;

	case 0x2266F1F2:		// Polymorph
	case 0xB15D524E:		// Chicken
	case 0xA73086F1:		// Pig
	case 0x0128F8E9:		// Turtle
	case 0xBD6B76DC:		// Good ol' sheep
		{
			grp = 10;
			pve = true;   
		}break;

	case 0x678E2C2F:		// Psychic Scream
		grp = 11;
		break;

	case 0x0D885237:		// Mind Control
		grp = 12;
		break;

	case 0x24D5C95F:		// Frost Shock
		grp = 13;
		break;

	case 0x898B6207:        // Hibernate
		grp=14;
		break;

	case 0x73812928:        // Cyclone
		{
			grp = 15;
			pve = true;
		}break;

	case 0xAD3ABF60:        // Celestial Focus
		{
			grp = 16;
			pve = true;
		}break;

	case 0x72676492:        // Impact
		{
			grp = 17;
			pve = true;
		}break;

	case 0x9018F6EA:        // Blackout
		{
			grp = 18;
			pve = true;
		}break;

	case 0x3D46465A:        // Banish
		grp = 19;
		break;

	case 0x1895FD50:        // Freezing Trap Effect
		grp = 20;
		break;

	case 0x94675337:        // Scare Beast
		grp = 21;
		break;

	case 0x9B34BCDA:        // Enslave Demon
		grp = 22;
		break;

	}
	uint32 ret;
	if(pve)
		ret = grp | (1 << 16);
	else
		ret = grp;

	return ret;
}

void Spell::SendCastSuccess(Object * target)
{
	Player * plr = p_caster;
	if(!plr && u_caster)
		plr = u_caster->m_redirectSpellPackets;
	if(!plr||!plr->IsPlayer())
		return;

	WorldPacket data(SMSG_TARGET_CAST_RESULT, 13);
	data << ((target != 0) ? target->GetNewGUID() : uint8(0));
	data << m_spellInfo->Id;
	
	plr->GetSession()->SendPacket(&data);
}

void Spell::SendCastSuccess(const uint64& guid)
{
	Player * plr = p_caster;
	if(!plr && u_caster)
		plr = u_caster->m_redirectSpellPackets;
	if(!plr || !plr->IsPlayer())
		return;
    
	// fuck bytebuffers
	unsigned char buffer[13];
	uint32 c = FastGUIDPack(guid, buffer, 0);
#ifdef USING_BIG_ENDIAN
	*(uint32*)&buffer[c] = swap32(m_spellInfo->Id);         c += 4;
#else
	*(uint32*)&buffer[c] = m_spellInfo->Id;                 c += 4;
#endif

	plr->GetSession()->OutPacket(SMSG_TARGET_CAST_RESULT, c, buffer);
}
/*
bool IsBeneficSpell(SpellEntry *sp)
{
	uint32 cur;
	for(uint32 i=0;i<3;i++)
		for(uint32 j=0;j<2;j++)
		{
			if(j==0)
				cur = sp->EffectImplicitTargetA[i];
			else // if(j==1)
				cur = sp->EffectImplicitTargetB[i];		
			switch(cur)
			{
				case EFF_TARGET_SELF:
				case EFF_TARGET_PET:
				case EFF_TARGET_ALL_PARTY_AROUND_CASTER:
				case EFF_TARGET_SINGLE_FRIEND:
				case 45:// Chain,!!only for healing!! for chain lightning =6 
				case 57:// Targeted Party Member
				case 27: // target is owner of pet
				case EFF_TARGET_MINION:// Minion Target
				case 33://Party members of totem, inside given range
				case EFF_TARGET_SINGLE_PARTY:// Single Target Party Member
				case EFF_TARGET_ALL_PARTY: // all Members of the targets party
				case EFF_TARGET_SELF_FISHING://Fishing
				case 46://Unknown Summon Atal'ai Skeleton
				case 47:// Portal
				case 52:	// Lightwells, etc
				case 40://Activate Object target(probably based on focus)
				case EFF_TARGET_TOTEM_EARTH:
				case EFF_TARGET_TOTEM_WATER:
				case EFF_TARGET_TOTEM_AIR:
				case EFF_TARGET_TOTEM_FIRE:// Totem
				case 61: // targets with the same group/raid and the same class
				case 32:
				case 73:
					return true;
			}//end switch
		}//end for
	return false;
}

AI_SpellTargetType RecommandAISpellTargetType(SpellEntry *sp)
{
	uint32 cur;
	for(uint32 i=0;i<3;i++)
		for(uint32 j=0;j<2;j++)
		{
			if(j==0)
				cur = sp->EffectImplicitTargetA[i];
			else // if(j==1)
				cur = sp->EffectImplicitTargetB[i];		
			switch(cur)
			{
				case EFF_TARGET_NONE:
				case EFF_TARGET_GAMEOBJECT:
				case EFF_TARGET_GAMEOBJECT_ITEM:// Gameobject/Item Target
				case EFF_TARGET_SELF_FISHING://Fishing
				case 47:// Portal
				case 52:	// Lightwells, etc
				case 40://Activate Object target(probably based on focus)
					return TTYPE_NULL;

				case EFF_TARGET_SELF:
				case 38://Dummy Target
				case 32:
				case 73:
					return TTYPE_CASTER;

				case EFF_TARGET_ALL_ENEMY_IN_AREA: // All Enemies in Area of Effect (TEST)
				case EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT: // All Enemies in Area of Effect instant (e.g. Flamestrike)
				case EFF_TARGET_ALL_ENEMY_IN_AREA_CHANNELED:// All Enemies in Area of Effect(Blizzard/Rain of Fire/volley) channeled
					return TTYPE_DESTINATION;

				case 4: // dono related to "Wandering Plague", "Spirit Steal", "Contagion of Rot", "Retching Plague" and "Copy of Wandering Plague"
				case EFF_TARGET_PET:
				case EFF_TARGET_SINGLE_ENEMY:// Single Target Enemy
				case 77:					// grep: i think this fits
				case 8: // related to Chess Move (DND), Firecrackers, Spotlight, aedm, Spice Mortar
				case EFF_TARGET_IN_FRONT_OF_CASTER:
				case 31:// related to scripted effects
				case 53:// Target Area by Players CurrentSelection()
				case 54:// Targets in Front of the Caster
				case EFF_TARGET_ALL_PARTY_AROUND_CASTER:
				case EFF_TARGET_SINGLE_FRIEND:
				case 45:// Chain,!!only for healing!! for chain lightning =6 
				case 57:// Targeted Party Member
				case EFF_TARGET_DUEL: 
				case 27: // target is owner of pet
				case EFF_TARGET_MINION:// Minion Target
				case 33://Party members of totem, inside given range
				case EFF_TARGET_SINGLE_PARTY:// Single Target Party Member
				case EFF_TARGET_ALL_PARTY: // all Members of the targets party
				case EFF_TARGET_TOTEM_EARTH:
				case EFF_TARGET_TOTEM_WATER:
				case EFF_TARGET_TOTEM_AIR:
				case EFF_TARGET_TOTEM_FIRE:// Totem
				case 61: // targets with the same group/raid and the same class
					return TTYPE_SINGLETARGET;

					// spells like 17278:Cannon Fire and 21117:Summon Son of Flame A
				case 17: // A single target at a xyz location or the target is a possition xyz
				case 18:// Land under caster.Maybe not correct
				case 46://Unknown Summon Atal'ai Skeleton
				case EFF_TARGET_ALL_ENEMIES_AROUND_CASTER:
					return TTYPE_SOURCE;

			}//end switch
		}//end for
	return TTYPE_NULL;// this means a new spell :P
}
*/
