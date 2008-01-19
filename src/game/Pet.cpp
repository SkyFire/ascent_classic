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

#define GROWL_RANK_1 2649
#define GROWL_RANK_2 14916
#define WATER_ELEMENTAL 510

uint32 GetAutoCastTypeForSpell(SpellEntry * ent)
{
	switch(ent->NameHash)
	{

	/************************************************************************/
	/* Warlock Pet Spells													*/
	/************************************************************************/

	case SPELL_HASH_FIREBOLT:			// Firebolt
	case SPELL_HASH_LASH_OF_PAIN:		// Lash of Pain
	case SPELL_HASH_TORMENT:			// Torment
		return AUTOCAST_EVENT_ATTACK;
		break;

	case SPELL_HASH_BLOOD_PACT:			// Blood Pact
		return AUTOCAST_EVENT_ON_SPAWN;
		break;

	case SPELL_HASH_FIRE_SHIELD:		// Fire Shield
		return AUTOCAST_EVENT_OWNER_ATTACKED;
		break;
		
	case SPELL_HASH_PHASE_SHIFT:		// Phase Shift
		return AUTOCAST_EVENT_LEAVE_COMBAT;
		break;

	/************************************************************************/
	/* Hunter Pet Spells													*/
	/************************************************************************/

	case SPELL_HASH_BITE:				// Bite
	case SPELL_HASH_CHARGE:				// Charge
	case SPELL_HASH_CLAW:				// Claw
	case SPELL_HASH_COWER:				// Cower
	case SPELL_HASH_DASH:				// Dash
	case SPELL_HASH_DIVE:				// Dive 
	case SPELL_HASH_FIRE_BREATH:		// Fire Breath
	case SPELL_HASH_FURIOUS_HOWL:		// Furious Howl
	case SPELL_HASH_GORE:				// Gore
	case SPELL_HASH_GROWL:				// Growl
	case SPELL_HASH_LIGHTNING_BREATH:	// Lightning Breath
	case SPELL_HASH_POISON_SPIT:		// Poison Spit 
	case SPELL_HASH_PROWL:				// Prowl
	case SPELL_HASH_SCORPID_POISON:		// Scorpid Poison
	case SPELL_HASH_SCREECH:			// Screech
	case SPELL_HASH_SHELL_SHIELD:		// Shell Shield
	case SPELL_HASH_THUNDERSTOMP:		// Thunderstomp
	case SPELL_HASH_WARP:				// Warp 
		return AUTOCAST_EVENT_ATTACK;
		break;

	/************************************************************************/
	/* Mage Pet Spells														*/
	/************************************************************************/

	case SPELL_HASH_WATERBOLT:			// Waterbolt
		return AUTOCAST_EVENT_ATTACK;
		break;
	}

	return AUTOCAST_EVENT_NONE;
}

void Pet::CreateAsSummon(uint32 entry, CreatureInfo *ci, Creature* created_from_creature, Unit *owner, SpellEntry* created_by_spell, uint32 type, uint32 expiretime)
{
	SetIsPet(true);

	//std::string myname = sWorld.GenerateName();

	if(!ci) return;
	m_Owner = static_cast<Player*>(owner);
	m_OwnerGuid = m_Owner->GetGUID();
	creature_info = ci;
	myFamily = dbcCreatureFamily.LookupEntry(creature_info->Family);
	//m_name = objmgr.GetCreatureFamilyName(myFamily->ID);
	if( myFamily->name == NULL )
		m_name = "Pet";
	else
		m_name.assign( myFamily->name );

	// Create ourself	
	Create(m_name.c_str(), owner->GetMapId(), owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), owner->GetOrientation());
	SetUInt32Value(OBJECT_FIELD_ENTRY, entry);
	SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);   // better set this one

	// Fields common to both lock summons and pets
	if(type & 0x2 && created_from_creature != NULL)
		SetUInt32Value(UNIT_FIELD_LEVEL, created_from_creature->getLevel());
	else
		SetUInt32Value(UNIT_FIELD_LEVEL,owner->GetUInt32Value(UNIT_FIELD_LEVEL));

	SetUInt32Value(UNIT_FIELD_DISPLAYID,  ci->Male_DisplayID);
	SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, ci->Male_DisplayID);
	SetUInt64Value(UNIT_FIELD_SUMMONEDBY, owner->GetGUID());
	SetUInt64Value(UNIT_FIELD_CREATEDBY, owner->GetGUID());
	
	if(type & 0x1 && created_by_spell != NULL)
		SetUInt64Value(UNIT_CREATED_BY_SPELL, created_by_spell->Id);

	if(type & 0x1 || created_from_creature == NULL)
	{
		Summon = true;
		SetUInt32Value(UNIT_FIELD_BYTES_0, 2048 | (0 << 24));
		SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
		SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 2000);
		SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, 2000);
		SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.5f);
		SetFloatValue(UNIT_FIELD_COMBATREACH, 0.75f);
		SetUInt32Value(UNIT_FIELD_BYTES_2, (0x01 | (0x28 << 8) | (0x2 << 24)));
		SetUInt32Value(UNIT_FIELD_PETNUMBER, GetGUIDLow());
		SetPowerType(POWER_TYPE_MANA);
		if(entry == WATER_ELEMENTAL)
			m_name = "Water Elemental";
		else
			m_name = sWorld.GenerateName();
		ApplyDemonTalents();

	} else {
		SetUInt32Value(UNIT_FIELD_BYTES_0, 2048 | (0 << 24));
		SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 2000);
		SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, 2000); // Supalosa: 2.00 normalised attack speed
		SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, created_from_creature->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS));
		SetFloatValue(UNIT_FIELD_COMBATREACH, created_from_creature->GetFloatValue(UNIT_FIELD_COMBATREACH));

		// These need to be checked.
		SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED | UNIT_FLAG_COMBAT); // why combat ??
		SetUInt32Value(UNIT_FIELD_POWER5, PET_HAPPINESS_UPDATE_VALUE >> 1);//happiness
		SetUInt32Value(UNIT_FIELD_MAXPOWER5, 1000000);
		SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
		SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, GetNextLevelXP(getLevel()));
		SetUInt32Value(UNIT_FIELD_BYTES_1, 0 | (REBELIOUS << 8));//loyalty level
		SetUInt32Value(UNIT_TRAINING_POINTS, 0);				//training points

		// Focus
		SetUInt32Value(UNIT_FIELD_POWER3, 100);
		SetUInt32Value(UNIT_FIELD_MAXPOWER3, 100);

		// 0x3 -> Enable pet rename.
		SetUInt32Value(UNIT_FIELD_BYTES_2, 1 | (0x28 << 8) | (0x3 << 16));

		// Change the power type to FOCUS
		SetPowerType(POWER_TYPE_FOCUS);

		// create our spellz
		SetDefaultSpells();
		ApplyBeastTalents();
	}

	// Apply stats.
	ApplyStatsForLevel();

	BaseDamage[0]=GetFloatValue(UNIT_FIELD_MINDAMAGE);
	BaseDamage[1]=GetFloatValue(UNIT_FIELD_MAXDAMAGE);
	BaseOffhandDamage[0]=GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE);
	BaseOffhandDamage[1]=GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE);
	BaseRangedDamage[0]=GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE);
	BaseRangedDamage[1]=GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE);

	SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, owner->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
	m_PartySpellsUpdateTimer = 0;

	m_PetNumber = static_cast<Player*>(owner)->GeneratePetNumber();
	SetUInt32Value(UNIT_FIELD_PETNUMBER, GetGUIDLow());

	m_ExpireTime = expiretime;
	bExpires = m_ExpireTime > 0 ? true : false;

	if(!bExpires)
	{
		// Create PlayerPet struct (Rest done by UpdatePetInfo)
		PlayerPet *pp = new PlayerPet;
		pp->number = m_PetNumber;
		pp->stablestate = STABLE_STATE_ACTIVE;
		static_cast<Player*>(owner)->AddPlayerPet(pp, pp->number);
	}	

	//maybe we should use speed from the tempalte we created the creature ?
	m_base_runSpeed = m_runSpeed = owner->m_base_runSpeed; //should we be able to keep up with master ?
	m_base_walkSpeed = m_walkSpeed = owner->m_base_walkSpeed; //should we be able to keep up with master ?

	InitializeMe(true);
}


Pet::Pet(uint32 high, uint32 low) : Creature(high, low)
{
	m_PetXP = 0;
	Summon = false;
	memset(ActionBar, 0, sizeof(uint32)*10);

	m_AutoCombatSpell = 0;

	m_PartySpellsUpdateTimer = 0;
	m_HappinessTimer = PET_HAPPINESS_UPDATE_TIMER;
	m_LoyaltyTimer = PET_LOYALTY_UPDATE_TIMER;
	m_PetNumber = 0;

	m_State = PET_STATE_DEFENSIVE;
	m_Action = PET_ACTION_FOLLOW;
	bHasLoyalty = false;
	m_ExpireTime = 0;
	bExpires = false;
	m_Diet = 0;
	m_Action = PET_ACTION_FOLLOW;
	m_State = PET_STATE_DEFENSIVE;
	TP = 0;
	LoyaltyPts=0;
}

Pet::~Pet()
{
	for(std::map<uint32, AI_Spell*>::iterator itr = m_AISpellStore.begin(); itr != m_AISpellStore.end(); ++itr)
		delete itr->second;

	if(IsInWorld())
		this->Remove(false, true, true);
}

void Pet::Update(uint32 time)
{
	if(!m_Owner)
		return;

	Creature::Update(time); // passthrough
	this->m_base_runSpeed = this->m_runSpeed = m_Owner->m_runSpeed; //should be able to keep up with master
	this->m_base_walkSpeed = this->m_walkSpeed = m_Owner->m_runSpeed; //should be able to keep up with master

	if(bHasLoyalty && !bExpires)
	{
		//Happiness
		if(m_HappinessTimer == 0)
		{	
			int32 val = GetUInt32Value(UNIT_FIELD_POWER5);
			//amount of burned happiness is loyalty_lvl depended
			int32 burn = HappinessTicks[GetLoyaltyLevel()-1]*125; //(ticks are 70/35/17/8/4/2 *1000 [per minute] ) /8 [per 7.5 sec]
			if((val - burn)<0)
			{	
				val = 0;
				bExpires=true; 
				m_ExpireTime=10000;	//avoid loosing pet right after calling it			
			}
			else
				val -= burn;
			SetUInt32Value(UNIT_FIELD_POWER5, val);// Set the value
			m_HappinessTimer = PET_HAPPINESS_UPDATE_TIMER;// reset timer
		} 
	else 
		{
			if(time > m_HappinessTimer)
				m_HappinessTimer = 0;
			else
				m_HappinessTimer -= time;
		}
		//Loyalty
		if(m_LoyaltyTimer==0 && GetHappinessState()!=NULL)
		{
			UpdateLoyalty(LoyaltyTicks[GetHappinessState()-1]);//loyalty tick is happiness state dependent
			m_LoyaltyTimer = PET_LOYALTY_UPDATE_TIMER;
		} 
		else 
		{
			if(time > m_LoyaltyTimer)
				m_LoyaltyTimer = 0;
			else
				m_LoyaltyTimer -= time;
		}
	}

	if(bExpires)
	{
		if(m_ExpireTime == 0)
		{
			// remove
			Dismiss(false);
			return;
		} else {
			if(time > m_ExpireTime)
			{
				m_ExpireTime = 0;
			} else {
				m_ExpireTime -= time;
			}
		}
	}
}

void Pet::SendSpellsToOwner()
{
	int packetsize = (m_uint32Values[OBJECT_FIELD_ENTRY] != WATER_ELEMENTAL) ? ((int)mSpells.size() * 4 + 20) : 64;
	WorldPacket * data = new WorldPacket(SMSG_PET_SPELLS, packetsize);
	*data << GetGUID();
	*data << uint32(0x00000000);//unk1
	*data << uint32(0x00000101);//unk2

	// Send the actionbar
	for(uint32 i = 0; i < 10; ++i)
	{
		if(ActionBar[i] & 0x4000000)		// Command
			*data << uint32(ActionBar[i]);
		else
		{
			if(uint16(ActionBar[i]))
				*data << uint16(ActionBar[i]) << GetSpellState(ActionBar[i]);
			else
				*data << uint16(0) << uint8(0) << uint8(i+5);
		}
	}

	// we don't send spells for the water elemental so it doesn't show up in the spellbook
	if(m_uint32Values[OBJECT_FIELD_ENTRY] != WATER_ELEMENTAL)	
	{
		// Send the rest of the spells.
		*data << uint8(mSpells.size());
		for(PetSpellMap::iterator itr = mSpells.begin(); itr != mSpells.end(); ++itr)
			*data << uint16(itr->first->Id) << uint16(itr->second);
	}
	*data << uint64(0);
	m_Owner->delayedPackets.add(data);
}

void Pet::SendNullSpellsToOwner()
{
	WorldPacket data(8);
	data.SetOpcode(SMSG_PET_SPELLS);
	data << uint64(0);
	m_Owner->GetSession()->SendPacket(&data);
}

//Implementation of some beast mastery talents to pet
void Pet::ApplyBeastTalents()
{
	InheritSMMods(m_Owner);
	Aura *pAura;
	SpellEntry *sp;
	int val;
	
	RemoveAura( 8875 );
	RemoveAura( 19580 );
	RemoveAura( 19581 );
	RemoveAura( 19582 );
	RemoveAura( 19589 );
	RemoveAura( 19591 );
	RemoveAura( 20784 );
	RemoveAura( 34457 );
	RemoveAura( 34666 );
	RemoveAura( 34667 );
	RemoveAura( 34675 );
	CastSpell( this, 8875, false );//Unleashed fury
	CastSpell( this, 19580, false );//Thick hide
	CastSpell( this, 19581, false );//Endurance training
	CastSpell( this, 19582, false );//Bestial swiftness
	CastSpell( this, 19589, false );//Bestial discipline
	CastSpell( this, 19591, false );//Ferocity

	//Frenzy
	sp = dbcSpell.LookupEntry( 20784 );
	pAura = new Aura( sp, -1, this, this );
	val = 0;
	if( this->m_Owner->HasAura( 19621 ) )
		val = 20;
	if( this->m_Owner->HasAura( 19622 ) )
		val = 40;
	if( this->m_Owner->HasAura( 19623 ) )
		val = 60;
	if( this->m_Owner->HasAura( 19624 ) )
		val = 80;
	if( this->m_Owner->HasAura( 19625 ) )
		val = 100;
	pAura->AddMod( sp->EffectApplyAuraName[0], val, 0, 19615 );
	this->AddAura( pAura );
	CastSpell( this, 20784, false );

	//Ferocious inspiration
	sp = dbcSpell.LookupEntry( 34457 );
	pAura = new Aura( sp, -1, this, this );
	val = 0;
	if( this->m_Owner->HasAura( 34455 ) )
		val = 1;
	if( this->m_Owner->HasAura( 34459 ) )
		val = 2;
	if( this->m_Owner->HasAura( 34460 ) )
		val = 3;
	pAura->AddMod( sp->EffectApplyAuraName[0], val, 0, 34456 );
	this->AddAura( pAura );

	//Animal handler
	sp = dbcSpell.LookupEntry( 34666 );
	pAura = new Aura(sp, -1, this, this );
	val = 0;
	if( this->m_Owner->HasAura( 34453 ) )
		val = 2;
	if( this->m_Owner->HasAura( 34454 ) )
		val = 4;
	pAura->AddMod( sp->EffectApplyAuraName[0], val, 0, 0 );
	pAura->AddMod( sp->EffectApplyAuraName[1], val, 0, 0 );
	this->AddAura( pAura );

	//Catlike reflexes
	sp = dbcSpell.LookupEntry( 34667 );
	pAura = new Aura( sp, -1, this, this);
	val = 0;
	if( this->m_Owner->HasAura( 34462 ) )
		val = 3;
	if( this->m_Owner->HasAura( 34464 ) )
		val = 6;
	if( this->m_Owner->HasAura( 34465 ) )
		val = 9;
	pAura->AddMod( sp->EffectApplyAuraName[0], val, 0, 0 );
	this->AddAura( pAura );

	//Serpent's swiftness
	sp = dbcSpell.LookupEntry(34675);
	pAura = new Aura( sp, -1, this, this );
	val = 0;
	if(this->m_Owner->HasAura( 34466 ))
		val = -4;
	if(this->m_Owner->HasAura( 34467 ))
		val = -8;
	if(this->m_Owner->HasAura( 34468 ) )
		val = -12;
	if(this->m_Owner->HasAura( 34469 ) )
		val = -16;
	if(this->m_Owner->HasAura( 34470 ) )
		val = -20;
	pAura->AddMod( sp->EffectApplyAuraName[0], val, 0, 0 );
	this->AddAura( pAura );

	//Spirit Bond
	val = 0;
	if( this->m_Owner->HasAura( 19578 ) )
		val = 19579;
	if( this->m_Owner->HasAura( 20895 ) )
		val = 24529;
	if( val > 0 )
	{
		CastSpell( this,(uint32)val, false ); //cast on self
		this->m_Owner->CastSpell( this->m_Owner, (uint32)val, false ); //cast on owner
	}
}

//Implementation of some demonology talents to pet
void Pet::ApplyDemonTalents()
{
	InheritSMMods( m_Owner );
	SpellEntry *sp;
	Aura *pAura;
	int val;
	
	RemoveAura( 35695 );//Demonic tactics
	RemoveAura( 35697 );//Demonic resilience

	sp = dbcSpell.LookupEntry( 35695 );
	pAura=new Aura( sp, -1, this, this );//Demonic tactics
	val = 0;
	if( this->m_Owner->HasAura( 30242 ) )
		val = 1;
	if( this->m_Owner->HasAura( 30245 ) )
		val = 2;
	if( this->m_Owner->HasAura( 30246 ) )
		val = 3;
	if( this->m_Owner->HasAura( 30247 ) )
		val = 4;
	if( this->m_Owner->HasAura( 30248 ) )
		val = 5;
	pAura->AddMod( sp->EffectApplyAuraName[0], val, 0,0 );
	this->AddAura( pAura );
	
	sp =dbcSpell.LookupEntry( 35697 );
	pAura=new Aura( sp, -1, this, this );//Demonic resilience
	val = 0;
	if( this->m_Owner->HasAura( 30319 ) )
		val = -5;
	if( this->m_Owner->HasAura( 30320 ) )
		val = -10;
	if( this->m_Owner->HasAura( 30321 ) )
		val = -15;
	pAura->AddMod( sp->EffectApplyAuraName[0], val, 0,0 );
	this->AddAura( pAura );
	
	switch( this->GetEntry() )
	{
	//imp
	case 416:
		this->RemoveAura( 18728 );
		this->RemoveAura( 18737 );
		this->RemoveAura( 18740 );
		this->CastSpell( this, 18728, false );	// Unholy power
		this->CastSpell( this, 18737, false );	// Fel Stamina
		this->CastSpell( this, 18740, false );	// Fel Intellect
		break;
	//voidvalker
	case 1860 :
		this->RemoveAura( 18727 );
		this->RemoveAura( 18735 );
		this->RemoveAura( 18742 );
		this->CastSpell( this, 18727, false );	// Unholy power
		this->CastSpell( this, 18735, false );	// Fel Stamina
		this->CastSpell( this, 18742, false );	// Fel Intellect
		break;
	//succubus
	case 1863:
		this->RemoveAura( 18729 );
		this->RemoveAura( 18736 );
		this->RemoveAura( 18741 );
		this->CastSpell( this, 18729, false );	// Unholy power
		this->CastSpell( this, 18736, false );	// Fel Stamina
		this->CastSpell( this, 18741, false );	// Fel Intellect
		break;
	//felhunter
	case 417:
		this->RemoveAura( 18730);
		this->RemoveAura( 18738);
		this->RemoveAura( 18739);
		this->CastSpell( this, 18730, false );	// Unholy power
		this->CastSpell( this, 18738, false );	// Fel Stamina
		this->CastSpell( this, 18739, false );	// Fel Intellect
		break;
	//felguard
	case 17252:
		this->RemoveAura( 30147 );
		this->RemoveAura( 30148 );
		this->RemoveAura( 30149 );
		this->CastSpell( this, 30147, false );	// Unholy power
		this->CastSpell( this, 30148, false );	// Fel Stamina
		this->CastSpell( this, 30149, false );	// Fel Intellect
		break;
	default: break;
	}

	val = 0;
	if( this->m_Owner->HasAura( 23785 ) )// Master Demonologist talent apply
		switch( this->GetEntry() )
		{
		case 416: val = 23759; break;	// imp
		case 1860: val = 23760; break;	// voidwalker
		case 1863: val = 23761; break;	// succubus
		case 417: val = 23762; break;	// felhunter
		case 17252: val = 35702; break;	// felguard
		default: break;
		}
		if( this->m_Owner->HasAura( 23822 ) )
		switch( this->GetEntry() )
		{
		case 416: val = 23826; break;	// imp
		case 1860: val = 23841; break;	// voidwalker
		case 1863: val = 23833; break;	// succubus
		case 417: val = 23837; break;	// felhunter
		case 17252: val = 35703; break;	// felguard
		default: break;
		}
		if( this->m_Owner->HasAura( 23823 ) )
		switch( this->GetEntry() )
		{
		case 416: val = 23827; break;	// imp
		case 1860: val = 23842; break;	// voidwalker
		case 1863: val = 23834; break;	// succubus
		case 417: val = 23838; break;	// felhunter
		case 17252: val = 35704; break;	// felguard
		default: break;
		}
		if( this->m_Owner->HasAura( 23824 ) )
		switch(this->GetEntry())
		{
		case 416: val = 23828; break;	// imp
		case 1860: val = 23843; break;	// voidwalker
		case 1863: val = 23835; break;	// succubus
		case 417: val = 23839; break;	// felhunter
		case 17252: val = 35705; break;	// felguard
		default: break;
		}
		if( this->m_Owner->HasAura( 23825 ) )
		switch( this->GetEntry() )
		{
		case 416: val = 23829; break;	// imp
		case 1860: val = 23844; break;	// voidwalker
		case 1863: val = 23836; break;	// succubus
		case 417: val = 23840; break;	// felhunter
		case 17252: val = 35706; break;	// felguard
		default: break;
		}
		if( val > 0)
		{
			CastSpell(this,(uint32)val,false); //cast on pet
			this->m_Owner->CastSpell(this->m_Owner,(uint32)val,false); //cast on owner
		}
}

void Pet::InitializeSpells()
{
	for(PetSpellMap::iterator itr = mSpells.begin(); itr != mSpells.end(); ++itr)
	{
		SpellEntry *info = itr->first;

		// Check that the spell isn't passive
		if(info->Attributes & 64)
		{
			// Cast on self..
			Spell * sp = new Spell(this, info, true, false);
			SpellCastTargets targets(this->GetGUID());
			sp->prepare(&targets);

			continue;
		}

		AI_Spell * sp = CreateAISpell(info);
		if(itr->second == AUTOCAST_SPELL_STATE)
			SetAutoCast(sp, true);
		else
			SetAutoCast(sp,false);
	}
}

AI_Spell * Pet::CreateAISpell(SpellEntry * info)
{
	// Create an AI_Spell
	map<uint32,AI_Spell*>::iterator itr = m_AISpellStore.find(info->Id);
	if(itr != m_AISpellStore.end())
		return itr->second;

	AI_Spell *sp = new AI_Spell;
	sp->agent = AGENT_SPELL;
	sp->entryId = GetEntry();
	sp->floatMisc1 = 0;
	sp->maxrange = GetMaxRange(dbcSpellRange.LookupEntry(info->rangeIndex));
	sp->minrange = GetMinRange(dbcSpellRange.LookupEntry(info->rangeIndex));
	sp->Misc2 = 0;
	sp->procChance = 0;
	sp->spell = info;
	sp->spellType = STYPE_DAMAGE;
	sp->spelltargetType = TTYPE_SINGLETARGET;
	sp->cooldown = objmgr.GetPetSpellCooldown(info->Id);
	sp->cooldowntime = 0;
	if(info->Effect[0] == SPELL_EFFECT_APPLY_AURA || info->Effect[0] == SPELL_EFFECT_APPLY_AREA_AURA)
		sp->spellType = STYPE_BUFF;

	if(info->EffectImplicitTargetA[0] == 24)
	{
		float radius = ::GetRadius(dbcSpellRadius.LookupEntry(info->EffectRadiusIndex[0]));
		sp->maxrange = radius;
		sp->spelltargetType = TTYPE_SOURCE;
	}

	sp->autocast_type = GetAutoCastTypeForSpell(info);
	sp->custom_pointer = false;
	sp->procCount=0;
	m_AISpellStore[info->Id] = sp;
	return sp;
}

void Pet::LoadFromDB(Player* owner, PlayerPet * pi)
{
	m_Owner = owner;
	m_OwnerGuid = m_Owner->GetGUID();
	mPi = pi;
	creature_info = CreatureNameStorage.LookupEntry(mPi->entry);

	Create(pi->name.c_str(), owner->GetMapId(), owner->GetPositionX() + 2 , owner->GetPositionY() +2, owner->GetPositionZ(), owner->GetOrientation());

	LoadValues(mPi->fields.c_str());
	m_PetNumber = mPi->number;
	m_PetXP = mPi->xp;
	m_name = mPi->name;
	Summon = mPi->summon;
	SetIsPet(true);

	m_HappinessTimer = mPi->happinessupdate;
	m_LoyaltyTimer = mPi->loyaltyupdate;
	LoyaltyPts = mPi->loyaltypts;

	SetIsPet(true);
	bExpires = false;   

	// Setup actionbar
	if(mPi->actionbar.size() > 2)
	{
		char * ab = strdup(mPi->actionbar.c_str());
		char * p = strchr(ab, ',');
		char * q = ab;
		uint32 spellid;
		uint32 spstate;
		uint32 i = 0;

		while(p && i<10)
		{
			*p = 0;

			if(sscanf(q, "%u %u", &spellid, &spstate) != 2)
				break;

			ActionBar[i] = spellid;
			//SetSpellState(dbcSpell.LookupEntry(spellid), spstate);
			if(!(ActionBar[i] & 0x4000000) && spellid)
				mSpells[dbcSpell.LookupEntry(spellid)] = spstate;

			i++;

			q = p+1;
			p = strchr(q, ',');
		}

		free(ab);
	}

	InitializeMe(false);

	if(m_Owner && getLevel() > m_Owner->getLevel())
	{
		SetUInt32Value(UNIT_FIELD_LEVEL, m_Owner->getLevel());
		SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
		SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, GetNextLevelXP(m_Owner->getLevel()));
	}
	ApplyStatsForLevel();//it is neccesary to recalculate pet stats on every call\summon to apply pet scaling
	if( this->Summon )
		ApplyDemonTalents();
	else
		ApplyBeastTalents();
	
	// Nuke auras
	for(uint32 x = UNIT_FIELD_AURA_01; x <= UNIT_FIELD_AURA_55; x++)
		SetUInt32Value(x, 0);
}

void Pet::OnPushToWorld()
{
	//before we initialize pet spells so we can apply spell mods on them 
	if( m_Owner && m_Owner->IsPlayer() )
		static_cast<Player*>( m_Owner )->EventSummonPet( this );

	Creature::OnPushToWorld();
}

void Pet::InitializeMe(bool first)
{
	// set up ai and shit
	GetAIInterface()->Init(this,AITYPE_PET,MOVEMENTTYPE_NONE,m_Owner);
	GetAIInterface()->SetUnitToFollow(m_Owner);
	GetAIInterface()->SetFollowDistance(3.0f);

	SetCreatureName(CreatureNameStorage.LookupEntry(GetEntry()));
	proto=CreatureProtoStorage.LookupEntry(GetEntry());
	m_Owner->SetSummon(this);
	m_Owner->SetUInt64Value(UNIT_FIELD_SUMMON, this->GetGUID());
	SetUInt32Value(UNIT_FIELD_PETNUMBER, GetGUIDLow());
	SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, (uint32)UNIXTIME);
	myFamily = dbcCreatureFamily.LookupEntry(creature_info->Family);
	bHasLoyalty = m_Owner->getClass() == HUNTER ? true : false;
	SetPetDiet();
	_setFaction();
	m_State = 1;		// dont set agro on spawn

	if(GetEntry() == 416)
		m_aiInterface->disable_melee = true;

	// Load our spells
	if(Summon)
	{
		SetDefaultSpells();
		// Adds parent +frost spell damage
		if(m_uint32Values[OBJECT_FIELD_ENTRY] == WATER_ELEMENTAL)
		{
			float parentfrost = (float)m_Owner->GetDamageDoneMod(SCHOOL_FROST);
			parentfrost *= 0.40f;
			ModDamageDone[SCHOOL_FROST] = (uint32)parentfrost;
		}
	}
	else
	{
		// Pull from database... :/
		QueryResult * query = CharacterDatabase.Query("SELECT * FROM playerpetspells WHERE ownerguid=%u and petnumber=%u",
			m_Owner->GetGUIDLow(), m_PetNumber);
		if(query)
		{
			do 
			{
				Field * f = query->Fetch();
				SpellEntry* spell = dbcSpell.LookupEntry(f[2].GetUInt32());
				uint16 flags = f[3].GetUInt16();
				if(mSpells.find(spell) == mSpells.end())
					mSpells.insert ( make_pair( spell, flags ) );

			} while(query->NextRow());
		}
		delete query;
		UpdateTP();
	}

	InitializeSpells(); 
	PushToWorld(m_Owner->GetMapMgr());

	if(first)
	{
		// Set up default actionbar
		SetDefaultActionbar();
	}

	SendSpellsToOwner();

	// set to active
	if(!bExpires)
		UpdatePetInfo(false);

	sEventMgr.AddEvent(this, &Pet::HandleAutoCastEvent, uint32(AUTOCAST_EVENT_ON_SPAWN), EVENT_UNK, 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	sEventMgr.AddEvent(this, &Pet::HandleAutoCastEvent, uint32(AUTOCAST_EVENT_LEAVE_COMBAT), EVENT_UNK, 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void Pet::UpdatePetInfo(bool bSetToOffline)
{
	if(bExpires || m_Owner==NULL)
		return;	// don't update expiring pets

	PlayerPet *pi = m_Owner->GetPlayerPet(m_PetNumber);
	pi->active = !bSetToOffline;
	pi->entry = GetEntry();
	std::stringstream ss;
	for( uint32 index = 0; index < UNIT_END; index ++ )
	{
		if(index >= UNIT_FIELD_AURA_01 && index <= UNIT_FIELD_AURA_55)
			ss << "0 ";
		else
			ss << GetUInt32Value(index) << " ";
	}
	pi->fields = ss.str();
	pi->name = GetName();
	pi->number = m_PetNumber;
	pi->xp = m_PetXP;
	pi->level = GetUInt32Value(UNIT_FIELD_LEVEL);
	pi->happiness = GetUInt32Value(UNIT_FIELD_POWER5);
	pi->happinessupdate = m_HappinessTimer;
	pi->loyaltypts = LoyaltyPts;
	pi->loyaltyupdate = m_LoyaltyTimer;
	pi->loyaltylvl = GetLoyaltyLevel();

	// save actionbar
	ss.rdbuf()->str("");
	for(uint32 i = 0; i < 10; ++i)
	{
		if(ActionBar[i] & 0x4000000)
			ss << ActionBar[i] << " 0";
		else if(ActionBar[i])
			ss << ActionBar[i] << " " << uint32(GetSpellState(ActionBar[i]));
		else
			ss << "0 0";

		ss << ",";
	}

	pi->actionbar = ss.str();
	pi->summon = Summon;
}

void Pet::Dismiss(bool bSafeDelete)//Abandon pet
{
	// Delete any petspells for us.
	if(!Summon && m_Owner)
	{
		CharacterDatabase.Execute("DELETE FROM playerpetspells WHERE ownerguid=%u AND petnumber=%u",
			m_Owner->GetGUIDLow(), m_PetNumber);
	}

	if(m_Owner)
	{
		m_Owner->RemovePlayerPet( m_PetNumber );

		//remove demonic resilience from owner warlock
		m_Owner->RemoveAura( 35695 );

		//remove master demonology from owner warlock
		switch( this->GetEntry() )
		{
		case 416: 
			m_Owner->RemoveAura( 23759 );
			m_Owner->RemoveAura( 23826 );
			m_Owner->RemoveAura( 23827 );
			m_Owner->RemoveAura( 23828 );
			m_Owner->RemoveAura( 23829 );
			break;	// imp
		case 1860:
			m_Owner->RemoveAura( 23760 );
			m_Owner->RemoveAura( 23841 );
			m_Owner->RemoveAura( 23842 );
			m_Owner->RemoveAura( 23843 );
			m_Owner->RemoveAura( 23844 );
			break;	// voidwalker
		case 1863:
			m_Owner->RemoveAura( 23761 );
			m_Owner->RemoveAura( 23833 );
			m_Owner->RemoveAura( 23834 );
			m_Owner->RemoveAura( 23835 );
			m_Owner->RemoveAura( 23836 );
			break;	// succubus
		case 417:
			m_Owner->RemoveAura( 23762 );
			m_Owner->RemoveAura( 23837 );
			m_Owner->RemoveAura( 23838 );
			m_Owner->RemoveAura( 23839 );
			m_Owner->RemoveAura( 23840 );
			break;	// felhunter
		case 17252:
			m_Owner->RemoveAura( 35702 );
			m_Owner->RemoveAura( 35703 );
			m_Owner->RemoveAura( 35704 );
			m_Owner->RemoveAura( 35705 );
			m_Owner->RemoveAura( 35706 );
			break;	// felguard
		default: break;
		}		

		//remove demonic tactics from owner warlock
		m_Owner->RemoveAura( (uint32)35697 );

		//remove owner warlock soul link from caster
		m_Owner->RemoveAura( (uint32)19028 );

		//remove owner warlock Demonic Knowledge from caster
		m_Owner->RemoveAura( (uint32)39576 );
	}

	// find out playerpet entry, delete it
	Remove(bSafeDelete, false, true);
}

void Pet::Remove(bool bSafeDelete, bool bUpdate, bool bSetOffline)
{
	RemoveAllAuras(); // Prevent pet overbuffing
	
	if(m_Owner)
	{
		m_Owner->RemoveAura( (uint32)19579 );	//remove spirit bond from owner hunter at pet remove
		m_Owner->RemoveAura( (uint32)24529 );
		
		// remove association with player
		m_Owner->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
	

		if(bUpdate) 
		{
			if(!bExpires) 
				UpdatePetInfo(bSetOffline);
			if(!IsSummon())
				m_Owner->_SavePet(NULL);//not perfect but working
		}
		m_Owner->SetSummon(NULL);
		SendNullSpellsToOwner();
	}
	ClearPetOwner();
/*	if(bSafeDelete)
		
	else*/
/*		PetSafeDelete();*/

	// has to be next loop - reason because of RemoveFromWorld, iterator gets broke.
	if(IsInWorld() && Active) Deactivate(m_mapMgr);
	sEventMgr.AddEvent(this, &Pet::PetSafeDelete, EVENT_CREATURE_SAFE_DELETE, 1, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void Pet::PetSafeDelete()
{
	if(this->IsInWorld())
	{
		// remove from world, and delete
		RemoveFromWorld(false, false);
	}

	//sEventMgr.AddEvent(World::getSingletonPtr(), &World::DeleteObject, ((Object*)this), EVENT_CREATURE_SAFE_DELETE, 1000, 1);
	Creature::SafeDelete();
}

void Pet::DelayedRemove(bool bTime, bool bDeath)
{
	m_Owner = objmgr.GetPlayer((uint32)m_OwnerGuid);
	if(bTime)
	{
		if(GetUInt32Value(UNIT_CREATED_BY_SPELL) > 0 || bDeath)
			Dismiss(true);  // remove us..
		else
			Remove(true, true, true);
	}
	else
		sEventMgr.AddEvent(this, &Pet::DelayedRemove, true, bDeath, EVENT_PET_DELAYED_REMOVE, PET_DELAYED_REMOVAL_TIME, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void Pet::GiveXP(uint32 xp)
{
	if(!m_Owner || getLevel() >= myFamily->maxlevel || getLevel() > m_Owner->getLevel()) return;
	
	xp += m_uint32Values[UNIT_FIELD_PETEXPERIENCE];
	uint32 nxp = m_uint32Values[UNIT_FIELD_PETNEXTLEVELEXP];
	bool changed = false;

	while(xp >= nxp)
	{
		ModUInt32Value(UNIT_FIELD_LEVEL, 1);
		xp -= nxp;
		nxp = GetNextLevelXP(m_uint32Values[UNIT_FIELD_LEVEL]);
		changed = true;
	}

	SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, xp);
	SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, nxp);
	if(changed) ApplyStatsForLevel();
}

uint32 Pet::GetNextLevelXP(uint32 currentlevel)
{
	uint32 level = currentlevel + 1;
	uint32 nextLvlXP = 0;
	if( level > 0 && level <= 30 )
	{
		nextLvlXP = ((int)((((double)(8 * level * ((level * 5) + 45)))/100)+0.5))*100;
	}
	else if( level == 31 )
	{
		nextLvlXP = ((int)((((double)(((8 * level) + 3) * ((level * 5) + 45)))/100)+0.5))*100;
	}
	else if( level == 32 )
	{
		nextLvlXP = ((int)((((double)(((8 * level) + 6) * ((level * 5) + 45)))/100)+0.5))*100;
	}
	else
	{
		nextLvlXP = ((int)((((double)(((8 * level) + ((level - 30) * 5)) * ((level * 5) + 45)))/100)+0.5))*100;
	}
	double xp = double(nextLvlXP) / 4.0;
	return FL2UINT(xp);
}

void Pet::SetDefaultSpells()
{
	if(Summon)
	{
		// this one's easy :p we just pull em from the owner.
		map<uint32, set<uint32> >::iterator it1;
		set<uint32>::iterator it2;
		it1 = m_Owner->SummonSpells.find(GetEntry());
		if(it1 != m_Owner->SummonSpells.end())
		{
			it2 = it1->second.begin();
			for(; it2 != it1->second.end(); ++it2)
			{
				AddSpell(dbcSpell.LookupEntry(*it2), false);
			}
		}
	}
	else
	{

		uint32 Line = GetCreatureName()->SpellDataID;
		if(Line)
		{
			CreatureSpellDataEntry * SpellData = dbcCreatureSpellData.LookupEntry(Line);
			if(SpellData)
				for(uint32 i = 0; i < 3; ++i)
					if(SpellData->Spells[i] != 0)
						AddSpell(dbcSpell.LookupEntry(SpellData->Spells[i]), false); //add spell to pet
		}
	}
}

void Pet::AddSpell(SpellEntry * sp, bool learning)
{
	// Cast on self if we're a passive spell
	if(sp->Attributes & 64)
	{
		if(IsInWorld())
		{
			Spell * spell = new Spell(this, sp, true, false);
			SpellCastTargets targets(this->GetGUID());
			spell->prepare(&targets);
			mSpells[sp] = 0x0100;
		}	
	}
	else
	{
	   // Active spell add to the actionbar.
		bool has=false;
		for(int i = 0; i < 10; ++i)
		{
			if(ActionBar[i] == sp->Id)
			{
				has=true;
				break;
			}
		}

		if(!has)
		{
			for(int i = 0; i < 10; ++i)
			{
				if(ActionBar[i] == 0)
				{
					ActionBar[i] = sp->Id;
					break;
				}
			}
		}

		bool done=false;
		if(learning)
		{
			for(PetSpellMap::iterator itr = mSpells.begin(); itr != mSpells.end(); ++itr)
			{
				if(sp->NameHash == itr->first->NameHash)
				{
					// replace the action bar
					for(int i = 0; i < 10; ++i)
					{
						if(ActionBar[i] == itr->first->Id)
						{
							ActionBar[i] = sp->Id;
							break;
						}
					}

					// Create the AI_Spell
					AI_Spell * asp = CreateAISpell(sp);

					// apply the spell state
					uint16 ss = GetSpellState(itr->first);
					mSpells[sp] = ss;
					if(ss==AUTOCAST_SPELL_STATE)
						SetAutoCast(asp, true);

					if(asp->autocast_type==AUTOCAST_EVENT_ON_SPAWN)
						CastSpell(this, sp, false);

					RemoveSpell(itr->first);
					done=true;
					break;
				}
			}
		}

		if(done==false)
		{
			if(mSpells.find(sp) != mSpells.end())
				return;

			if(learning)
			{
				AI_Spell * asp = CreateAISpell(sp);
				uint16 ss = (asp->autocast_type > 0) ? AUTOCAST_SPELL_STATE : DEFAULT_SPELL_STATE;
				mSpells[sp] = ss;
				if(ss==AUTOCAST_SPELL_STATE)
					SetAutoCast(asp,true);

				if(asp->autocast_type==AUTOCAST_EVENT_ON_SPAWN)
					CastSpell(this, sp, false);
			}
			else
				mSpells[sp] = DEFAULT_SPELL_STATE;
		}
	}
	if(IsInWorld())
		SendSpellsToOwner();
	UpdateTP();
}

void Pet::SetSpellState(SpellEntry* sp, uint16 State)
{
	PetSpellMap::iterator itr = mSpells.find(sp);
	if(itr == mSpells.end())
		return;

	uint16 oldstate = itr->second;
	itr->second = State;

	if(State == AUTOCAST_SPELL_STATE || oldstate == AUTOCAST_SPELL_STATE)
	{
		AI_Spell * sp2 = GetAISpellForSpellId(sp->Id);
		if(sp2)
		{
			if(State == AUTOCAST_SPELL_STATE)
				SetAutoCast(sp2, true);
			else
				SetAutoCast(sp2,false);
		}
	}		
}

uint16 Pet::GetSpellState(SpellEntry* sp)
{
	PetSpellMap::iterator itr = mSpells.find(sp);
	if(itr == mSpells.end())
		return DEFAULT_SPELL_STATE;

	return itr->second;
}

void Pet::SetDefaultActionbar()
{
	// Set up the default actionbar.
	ActionBar[0] = PET_SPELL_ATTACK;
	ActionBar[1] = PET_SPELL_FOLLOW;
	ActionBar[2] = PET_SPELL_STAY;

	// Fill up 4 slots with our spells
	if(mSpells.size() > 0)
	{
		PetSpellMap::iterator itr = mSpells.begin();
		uint32 pos = 0;
		for(; itr != mSpells.end() && pos < 4; ++itr, ++pos)
			ActionBar[3+pos] = itr->first->Id;
	}

	ActionBar[7] = PET_SPELL_AGRESSIVE;
	ActionBar[8] = PET_SPELL_DEFENSIVE;
	ActionBar[9] = PET_SPELL_PASSIVE;
}

void Pet::RemoveSpell(SpellEntry * sp)
{
	mSpells.erase(sp);
	map<uint32, AI_Spell*>::iterator itr = m_AISpellStore.find(sp->Id);
	if(itr != m_AISpellStore.end())
	{
		for(list<AI_Spell*>::iterator it = m_aiInterface->m_spells.begin(); it != m_aiInterface->m_spells.end(); ++it)
		{
			if((*it) == itr->second)
			{
				/*if((*it)->autocast_type > 0)
					m_autoCastSpells[(*it)->autocast_type].remove((*it));*/
				if((*it)->autocast_type > 0)
				{
					for(list<AI_Spell*>::iterator i3 = m_autoCastSpells[(*it)->autocast_type].begin();
						i3 != m_autoCastSpells[(*it)->autocast_type].end(); ++i3)
					{
						if( (*i3) == itr->second )
						{
							m_autoCastSpells[(*it)->autocast_type].erase(i3);
							break;
						}
					}
				}

				m_aiInterface->m_spells.erase(it);
				m_aiInterface->CheckNextSpell(itr->second);
				break;
			}
		}

		delete itr->second;
		m_AISpellStore.erase(itr);
	}
	else
	{
		for(list<AI_Spell*>::iterator it = m_aiInterface->m_spells.begin(); it != m_aiInterface->m_spells.end(); ++it)
		{
			if((*it)->spell == sp)
			{
				// woot?
				AI_Spell * spe = *it;
				m_aiInterface->m_spells.erase(it);
				delete spe;
				break;
			}
		}
	}
	UpdateTP();
}

void Pet::Rename(string NewName)
{
	m_name = NewName;
	// update petinfo
	UpdatePetInfo(false);

	// update timestamp to force a re-query
	SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, (uint32)UNIXTIME);
}

void Pet::ApplySummonLevelAbilities()
{
	uint32 level = m_uint32Values[UNIT_FIELD_LEVEL];
	double pet_level = (double)level;

	// Determine our stat index.
	int stat_index = -1;
	//float scale = 1;
	bool has_mana = true;

	switch(m_uint32Values[OBJECT_FIELD_ENTRY])
	{
	case 416: //Imp
		stat_index = 0;
		m_aiInterface->disable_melee = true;
		break;
	case 1860: //VoidWalker
		stat_index = 1;
		break;
	case 1863: //Succubus
		stat_index = 2;
		break;
	case 417: //Felhunter
		stat_index = 3;
		break;
	case 11859: // Doomguard
	case 89:	// Infernal
	case 17252: // Felguard
		stat_index = 4;
		break;
	/*case 11859: // Doomguard
		stat_index = 4;
		break;
	case 89:	// Infernal
		stat_index = 5;
		has_mana = false;
		break;
	case 17252: // Felguard
		stat_index = 6;
		break;*/
	case 510:	// Mage's water elemental
		stat_index = 5;
		m_aiInterface->disable_melee = true;
		break;
	}
	if(m_uint32Values[OBJECT_FIELD_ENTRY] == 89)
		has_mana = false;

	if(stat_index < 0)
	{
		sLog.outError("PETSTAT: No stat index found for entry %u, `%s`!", GetEntry(), creature_info->Name);
		return;
	}

	/*
	----------------------------
	------- Warlock Pets -------
	----------------------------
	pet_str = base_str + pet_lvl * mod_str
	pet_agi = base_agi + pet_lvl * mod_agi
	pet_sta = base_sta + pet_lvl * mod_sta + pet_sta_bonus
	pet_int = base_int + pet_lvl * mod_int + pet_int_bonus
	pet_spr = base_spr + pet_lvl * mod_spr
	pet_pwr = base_pwr + pet_lvl * mod_pwr
	pet_arm = base_arm + pet_lvl * mod_arm + pet_arm_bonus

	pet_sta_bonus = 0.3 * plyr_sta + sta_buffs
	pet_int_bonus = 0.3 * plyr_int + int_buffs
	pet_arm_bonus = 0.35 * plyr_arm + arm_buffs

	--[Imp]--
	base_str = 18.0	
	mod_str = 2.0
	base_agi = 20.2	
	mod_agi = 0.2
	base_sta = 19.5	
	mod_sta = 0.5
	base_int = 21.0	
	mod_int = 3.0
	base_spr = 19.0	
	mod_spr = 4.0
	base_pwr = 9.0	
	mod_pwr = 1.0
	base_arm = 47.0	
	mod_arm = 13.0

	--[Voidwalker]--
	base_str = 19.0	
	mod_str = 2.0
	base_agi = 15.0	
	mod_agi = 1.0
	base_sta = -13.0	
	mod_sta = 4.0
	base_int = 17.3	
	mod_int = 1.7
	base_spr = 9.0	
	mod_spr = 2.0
	base_pwr = -16.0
	mod_pwr = 4.0
	base_arm = 177.0	
	mod_arm = 50.0

	--[Succubus]--
	base_str = 2.0	
	mod_str = 2.0
	base_agi = 16.0	
	mod_agi = 1.0
	base_sta = -17.0	
	mod_sta = 4.5
	base_int = 9.0	
	mod_int = 2.0
	base_spr = 12.0	
	mod_spr = 1.3
	base_pwr = -26.0	
	mod_pwr = 4.0
	base_arm = 125.0	
	mod_arm = 23.0

	--[Felhunter]--
	base_str = -3.0	
	mod_str = 2.0
	base_agi = 16.0	
	mod_agi = 1.0
	base_sta = 36.0	
	mod_sta = 3.0
	base_int = 9.0	
	mod_int = 2.0
	base_spr = 21.0	
	mod_spr = 1.0
	base_pwr = -26.0	
	mod_pwr = 4.0
	base_arm = -910.0
	mod_arm = 57.0

	--[Doomguard]--
	base_str = 9.0	
	mod_str = 2.0
	base_agi = -5.0	
	mod_agi = 1.5
	base_sta = -66.0
	mod_sta = 5.0
	base_int = 10.0
	mod_int = 1.0
	base_spr = 30.0
	mod_spr = 2.0
	base_pwr = -62.0
	mod_pwr = 5.0
	base_arm = -187.0
	mod_arm = 65.0

	--[Infernal]--
	base_str = -14.0
	mod_str = 2.5
	base_agi = -20.0
	mod_agi = 2.0
	base_sta = -44.0
	mod_sta = 5.0
	base_int = 20.0	
	mod_int = 0.5
	base_spr = -40.0
	mod_spr = 2.0
	base_pwr = -48.0
	mod_pwr = 5.0
	base_arm = 1605.0
	mod_arm = 50.0

	--[Felguard]--
	base_str = -39.0
	mod_str = 3.0
	base_agi = 7.0	
	mod_agi = 1.5
	base_sta = -14.0	
	mod_sta = 6.0
	base_int = 82.0	
	mod_int = 2.0
	base_spr = -12.0	
	mod_spr = 2.0
	base_pwr = -68.0	
	mod_pwr = 5.5
	base_arm = 1208.0
	mod_arm = 47.0
	*/

	// Imp,Voidwalker,Succubus,Felhunter,Doomguard,Infernal,Felguard
	/*static double R_base_str[7] = { 18.1884058,-15,-15,-15,-15, -14, -39 };
	static double R_mod_str[7] = { 1.811594203,2.4,2.4,2.4,2.4, 2.5, 3 };
	static double R_base_agi[7] = { 20.2, 15, 16, 16, -5, -20, 7 };
	static double R_mod_agi[7] = { 0.2, 1, 1, 1, 1.5, 2, 1.5 };
	static double R_base_sta[7] = { 19.5, -13, -17, 36, -66, -44, -14 };
	static double R_mod_sta[7] = { 0.5, 4, 4.5, 3, 5, 5, 6 };
	static double R_base_int[7] = { 21, 17.3, 9, 9, 10, 20, 82 };
	static double R_mod_int[7] = { 3, 1.7, 2, 2, 1, 0.5, 2 };
	static double R_base_spr[7] = { 19, 9, 12, 21, 30, -40, -12 };
	static double R_mod_spr[7] = { 4, 2, 1.3, 1, 2, 2, 2 };
	static double R_base_pwr[7] = { 9, -16, -26, -26, -62, -48, -68 };
	static double R_mod_pwr[7] = { 1, 4, 4, 4, 5, 5, 5.5 };
	static double R_base_arm[7] = { 47, 177, 125, -910, -187, 1605, 1208 };
	static double R_mod_arm[7] = { 13, 50, 23, 57, 65, 50, 47 };

	double base_str = R_base_str[stat_index];
	double mod_str = R_mod_str[stat_index];
	double base_agi = R_base_agi[stat_index];
	double mod_agi = R_mod_agi[stat_index];
	double base_sta = R_base_sta[stat_index];
	double mod_sta = R_mod_sta[stat_index];
	double base_int = R_base_int[stat_index];
	double mod_int = R_mod_int[stat_index];
	double base_spr = R_base_spr[stat_index];
	double mod_spr = R_mod_spr[stat_index];
	double base_pwr = R_base_pwr[stat_index];
	double mod_pwr = R_mod_pwr[stat_index];
	double base_arm = R_base_arm[stat_index];
	double mod_arm = R_mod_arm[stat_index];

	// Calculate bonuses
	double pet_sta_bonus = 0.3 * (double)m_Owner->BaseStats[STAT_STAMINA];
	double pet_int_bonus = 0.3 * (double)m_Owner->BaseStats[STAT_INTELLECT];
	double pet_arm_bonus = 0.35 * (double)m_Owner->BaseResistance[0];

	double pet_str = base_str + pet_level * mod_str;
	double pet_agi = base_agi + pet_level * mod_agi;
	double pet_sta = base_sta + pet_level * mod_str + pet_sta_bonus;
	double pet_int = base_int + pet_level * mod_int + pet_int_bonus;
	double pet_spr = base_spr + pet_level * mod_spr;
	double pet_pwr = base_pwr + pet_level * mod_pwr;
	double pet_arm = base_arm + pet_level * mod_arm;
	
	// Calculate actual values.
	BaseStats[STAT_STRENGTH] = FL2UINT(pet_str);
	BaseStats[STAT_AGILITY] = FL2UINT(pet_agi);
	BaseStats[STAT_STAMINA] = FL2UINT(pet_sta);
	BaseStats[STAT_INTELLECT] = FL2UINT(pet_int);
	BaseStats[STAT_SPIRIT] = FL2UINT(pet_spr);
	
	for(uint32 x = 0; x < 5; ++x)
		CalcStat(x);

	// Apply armor and attack power.
	SetUInt32Value(UNIT_FIELD_ATTACK_POWER, FL2UINT(pet_pwr));
	BaseResistance[0] = FL2UINT(pet_arm);
	CalcResistance(0);
	CalcDamage();

	// Calculate health / mana
	double health = pet_sta * 10;
	double mana = has_mana ? (base_int * 15) : 0.0;
	SetUInt32Value(UNIT_FIELD_BASE_HEALTH, FL2UINT(health));
	SetUInt32Value(UNIT_FIELD_BASE_MANA, FL2UINT(mana));*/

	static double R_base_str[6] = {18.1884058, -15, -15, -15, -15, -15};
	static double R_mod_str[6] = {1.811594203, 2.4, 2.4, 2.4, 2.4, 2.4};
	static double R_base_agi[6] = {19.72463768, -1.25, -1.25, -1.25, -1.25, -1.25};
	static double R_mod_agi[6] = {0.275362319, 1.575, 1.575, 1.575, 1.575, 1.575};
	static double R_base_sta[6] = {17.23188406, -17.75, -17.75, -17.75, -17.75, 0};
	static double R_mod_sta[6] = {2.768115942, 4.525, 4.525, 4.525, 4.525, 4.044};
	static double R_base_int[6] = {19.44927536, 12.75, 12.75, 12.75, 12.75, 20};
	static double R_mod_int[6] = {4.550724638, 1.875, 1.875, 1.875, 1.875, 2.8276};
	static double R_base_spr[6] = {19.52173913, -2.25, -2.25, -2.25, -2.25, 20.5};
	static double R_mod_spr[6] = {3.47826087, 1.775, 1.775, 1.775, 1.775, 3.5};
	static double R_base_pwr[6] = {7.202898551, -101, -101, -101, -101, -101};
	static double R_mod_pwr[6] = {2.797101449, 6.5, 6.5, 6.5, 6.5, 6.5};
	static double R_base_armor[6] = {-11.69565217, -702, -929.4, -1841.25, -1157.55, -1000};
	static double R_mod_armor[6] = {31.69565217, 139.6, 74.62, 89.175, 101.1316667, 100};
	static double R_pet_sta_to_hp[6] = {6.405982906, 15.91304348, 7.956521739, 10.79813665, 11.55590062, 10.0};
	static double R_base_min_dmg[6] = {0.550724638, 4.566666667, 26.82, 29.15, 20.17888889, 20};
	static double R_mod_min_dmg[6] = {1.449275362, 1.433333333, 2.18, 1.85, 1.821111111, 1};
	static double R_base_max_dmg[6] = {1.028985507, 7.133333333, 36.16, 39.6, 27.63111111, 20};
	static double R_mod_max_dmg[6] = {1.971014493, 1.866666667, 2.84, 2.4, 2.368888889, 1.1};

	double base_str = R_base_str[stat_index];
	double mod_str = R_mod_str[stat_index];
	double base_agi = R_base_agi[stat_index];
	double mod_agi = R_mod_agi[stat_index];
	double base_sta = R_base_sta[stat_index];
	double mod_sta = R_mod_sta[stat_index];
	double base_int = R_base_int[stat_index];
	double mod_int = R_mod_int[stat_index];
	double base_spr = R_base_spr[stat_index];
	double mod_spr = R_mod_spr[stat_index];
	double base_pwr = R_base_pwr[stat_index];
	double mod_pwr = R_mod_pwr[stat_index];
	double base_armor = R_base_armor[stat_index];
	double mod_armor = R_mod_armor[stat_index];
	double base_min_dmg = R_base_min_dmg[stat_index];
	double mod_min_dmg = R_mod_min_dmg[stat_index];
	double base_max_dmg = R_base_max_dmg[stat_index];
	double mod_max_dmg = R_mod_max_dmg[stat_index];
	double pet_sta_to_hp = R_pet_sta_to_hp[stat_index];

	// Calculate bonuses
	double pet_sta_bonus = 0.3 * (double)m_Owner->BaseStats[STAT_STAMINA];	  // + sta_buffs
	double pet_int_bonus = 0.3 * (double)m_Owner->BaseStats[STAT_INTELLECT];	// + int_buffs
	double pet_arm_bonus = 0.35 * (double)m_Owner->BaseResistance[0];		   // + arm_buffs

	double pet_str = base_str + pet_level * mod_str;
	double pet_agi = base_agi + pet_level * mod_agi;
	double pet_sta = base_sta + pet_level * mod_sta + pet_sta_bonus;
	double pet_int = base_int + pet_level * mod_int + pet_int_bonus;
	double pet_spr = base_spr + pet_level * mod_spr;
	double pet_pwr = base_pwr + pet_level * mod_pwr;
	double pet_arm = base_armor + pet_level * mod_armor + pet_arm_bonus;

	// Calculate values
	BaseStats[STAT_STRENGTH] = FL2UINT(pet_str);
	BaseStats[STAT_AGILITY] = FL2UINT(pet_agi);
	BaseStats[STAT_STAMINA] = FL2UINT(pet_sta);
	BaseStats[STAT_INTELLECT] = FL2UINT(pet_int);
	BaseStats[STAT_SPIRIT] = FL2UINT(pet_spr);

	double pet_min_dmg = base_min_dmg + pet_level * mod_min_dmg;
	double pet_max_dmg = base_max_dmg + pet_level * mod_max_dmg;
	BaseDamage[0] = float(pet_min_dmg);
	BaseDamage[1] = float(pet_max_dmg);

	for(uint32 x = 0; x < 5; ++x)
		CalcStat(x);

	// Apply armor and attack power.
	SetUInt32Value(UNIT_FIELD_ATTACK_POWER, FL2UINT(pet_pwr));
	BaseResistance[0] = FL2UINT(pet_arm);
	CalcResistance(0);
	CalcDamage();

	// Calculate health / mana
	double health = pet_sta * pet_sta_to_hp;
	double mana = has_mana ? (pet_int * 15) : 0.0;
	SetUInt32Value(UNIT_FIELD_BASE_HEALTH, FL2UINT(health));
	SetUInt32Value(UNIT_FIELD_BASE_MANA, FL2UINT(mana));
}

void Pet::ApplyPetLevelAbilities()
{
	uint32 level = m_uint32Values[UNIT_FIELD_LEVEL];
	double dlevel = (double)level;

	/*
	----------[Pets]----------
		Family		 pet_mod_sta			 pet_mod_arm		   pet_mod_dps
	(1)	Wolf			 1.00				  1.05				  1.00
	(2)	 Cat			 0.98				1.00				1.10
	(3)	 Spider			 1.00				  1.00				 1.07
	(4)	 Bear			 1.08				 1.05				 0.91
	(5)	 Boar			 1.04				1.09				0.90
	(6)	 Crocolisk	   0.95				1.10				1.00
	(7)	 Carrion Bird	1.00				 1.05				 1.00
	(8)	 Crab			0.96				1.13				0.95
	(9)	 Gorilla		 1.04				1.00				1.02
	(10)
	(11)	Raptor			  0.95				  1.03				  1.10	 
	(12)	Tallstrider	 1.05				  1.00				  1.00
	(13)
	(14)
	(15)	Felhunter
	(16)	Voidwalker
	(17)	Succubus
	(18)
	(19)	Doomguard
	(20)	Scorpid		 1.00				 1.10				  0.94
	(21)	Turtle			 1.00				  1.13				  0.90
	(22)
	(23)	Imp
	(24)	Bat			 1.00					 1.00					 1.07
	(25)	Hyena		   1.00				  1.05				  1.00	
	(26)	Owl			 1.00					 1.00				  1.07
	(27)	Wind Serpent	1.00				  1.00				  1.07
	(28)	Remote Control
	(29)	Felguard
	(30)	Dragonhawk	  1.00					1.00					1.00
	(31)	Ravager	 0.93				  1.05				  1.10
	(32)	Warp Stalker	1.00				  1.05				  0.94
	(33)	Spore Bat	   1.00				  1.00				  1.00
	(34)	Nether Ray	 1.10				 0.90				  1.03	 
	(35)	Serpent	 1.00				 1.00				  1.00
	*/

	static double R_pet_mod_sta[36] = { 0, 1, 0.98, 1, 1.08, 1.04, 0.95, 1, 0.96, 1.04, 0, 0.95, 1.05, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0.93, 1, 1, 1.1, 1 };
	static double R_pet_mod_arm[36] = { 0, 1.05, 1, 1, 1.05, 1.09, 1.1, 1.05, 1.13, 1, 0, 1.03, 1, 0, 0, 0, 0, 0, 0, 0, 1.1, 1.13, 0, 0, 1, 1.05, 1, 1, 0, 0, 1, 1.05, 1.05, 1, 0.9, 1 };
	static double R_pet_mod_dps[36] = { 0, 1, 1.10, 1.07, 0.91, 0.9, 1, 1, 0.95, 1.02, 0, 1.1, 1, 0, 0, 0, 0, 0, 0, 0, 0.94, 0.9, 0, 0, 1.07, 1, 1.07, 1.07, 0, 0, 1, 1.1, 0.94, 1, 1.03, 1 };
	//"Base" part of pet damage (before applying of any modifiers) is equal for any pet family
	//Presumed to be linear increasing per level (surely not fully correct)
	double base_min_dmg = 1;
	double base_max_dmg = 2;
	double mod_min_dmg = 2;
	double mod_max_dmg = 2.48;
	
	double pet_mod_sta = 1, pet_mod_arm = 1, pet_mod_dps = 1;
	if(creature_info->Family > 35 || R_pet_mod_sta[creature_info->Family] == 0)
	{
		if( myFamily == NULL && myFamily->name != NULL )
            sLog.outError("PETSTAT: Creature family %u has missing data. Assuming to be 1.", creature_info->Family);
		else
			sLog.outError("PETSTAT: Creature family %u [%s] has missing data. Assuming to be 1.", creature_info->Family, myFamily->name);
	}
	else
	{
		pet_mod_sta = R_pet_mod_sta[creature_info->Family];
		pet_mod_arm = R_pet_mod_arm[creature_info->Family];
		pet_mod_dps = R_pet_mod_dps[creature_info->Family];
	}

	// Calculate Bonuses
//	double pet_sta_bonus = 0.3 * (double)m_Owner->BaseStats[STAT_STAMINA];
	//patch from darken
	double pet_sta_bonus = 0.3 * (double)m_Owner->GetUInt32Value(UNIT_FIELD_STAT2);
	double pet_arm_bonus = 0.35 * (double)m_Owner->BaseResistance[0];	   // Armor
	double pet_ap_bonus = 0.22 * (double)m_Owner->GetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER);

	//Base attributes from http://petopia.brashendeavors.net/html/art...ttributes.shtml
	static double R_pet_base_armor[70] = { 20, 21, 46, 82, 126, 180, 245, 322, 412, 518, 545, 580, 615,650, 685, 721, 756, 791, 826, 861, 897, 932, 967, 1002, 1037, 1072, 1108, 1142, 1177, 1212, 1247, 1283, 1317, 1353, 1387, 1494, 1607, 1724, 1849, 1980, 2117, 2262, 2414, 2574, 2742, 2798, 2853,2907, 2963, 3018, 3072, 3128, 3183, 3237, 3292, 3348, 3402, 3457, 3512, 3814, 4113, 4410, 4708, 5006, 5303, 5601, 5900, 6197, 6495, 6790 };
	static double R_pet_base_hp[70] = { 42, 55, 71, 86, 102, 120, 137, 156, 176, 198, 222, 247, 273, 300, 328, 356, 386, 417, 449, 484, 521, 562, 605, 651, 699, 750, 800, 853, 905, 955, 1006, 1057, 1110, 1163, 1220, 1277, 1336, 1395, 1459, 1524, 1585, 1651, 1716, 1782, 1848, 1919, 1990, 2062, 2138, 2215, 2292, 2371, 2453, 2533, 2614, 2699, 2784, 2871, 2961, 3052, 3144, 3237, 3331, 3425, 3524, 3624, 3728, 3834, 3941, 4049 };

	// Calculate HP
	//patch from darken
	double pet_hp;
	double pet_armor;
	if(level-1<70)
	{
		pet_hp= ( ( ( R_pet_base_hp[level-1]) + ( pet_sta_bonus * 10 ) ) * pet_mod_sta);
		pet_armor= ( (R_pet_base_armor[level-1] ) * pet_mod_arm + pet_arm_bonus );
	}
	else
	{
		pet_hp	= ( ( ( 0.6 * dlevel * dlevel + 10.6 * dlevel + 33 ) + ( pet_sta_bonus * 10 ) ) * pet_mod_sta);
		pet_armor = ( ( -75 + 50 * dlevel ) * pet_mod_arm + pet_arm_bonus );
	}
//	double pet_attack_power = ( ( ( 20 * dlevel) - 60 ) + pet_ap_bonus ) * pet_mod_dps;
	double pet_attack_power = ( ( 7.9 * ( ( dlevel * dlevel ) / ( dlevel * 3 ) ) ) + ( pet_ap_bonus ) ) * pet_mod_dps;

	if(pet_attack_power <= 0.0f) pet_attack_power = 1;
	if(pet_armor <= 0.0f) pet_armor = 1;

	// Set base values.
	SetUInt32Value(UNIT_FIELD_BASE_HEALTH, FL2UINT(pet_hp));
	BaseResistance[0] = FL2UINT(pet_armor);
	CalcResistance(0);

	// Calculate damage. Lvl 70 damage values are based on hunter thread from official forums
	// [url="http://forums.wow-europe.com/thread.html?topicId=184248445&postId=1842118585&sid=1#0"]http://forums.wow-europe.com/thread.html?t...585&sid=1#0[/url]
	SetUInt32Value(UNIT_FIELD_ATTACK_POWER, FL2UINT(pet_attack_power));
	double pet_min_dmg = base_min_dmg + getLevel() * mod_min_dmg;
	double pet_max_dmg = base_max_dmg + getLevel() * mod_max_dmg;
	BaseDamage[0] = float( pet_min_dmg );
	BaseDamage[1] = float( pet_max_dmg );
	CalcDamage();

	// These are just for visuals, no other actual purpose.
	BaseStats[0] = uint32(20+getLevel()*1.55);
	BaseStats[1] = uint32(20+getLevel()*0.64);
	BaseStats[3] = uint32(20+getLevel()*0.18);
	BaseStats[4] = uint32(20+getLevel()*0.36);

	// Reverse the health value to calculate stamina
	BaseStats[STAT_STAMINA] = FL2UINT(pet_hp / 10);

	for(uint32 x = 0; x < 5; ++x)
		CalcStat(x);
	UpdateTP();
}

void Pet::ApplyStatsForLevel()
{
	if(m_uint32Values[UNIT_CREATED_BY_SPELL])	   // Summon
		ApplySummonLevelAbilities();
	else
		ApplyPetLevelAbilities();

	// Apply common stuff
	float pet_level = float(m_uint32Values[UNIT_FIELD_LEVEL]);

	// Apply scale for this family.
	float level_diff = float(myFamily->maxlevel - myFamily->minlevel);
	float scale_diff = float(myFamily->maxsize - myFamily->minsize);
	float factor = scale_diff / level_diff;
	float scale = factor * pet_level + myFamily->minsize;
	SetFloatValue(OBJECT_FIELD_SCALE_X, scale);

	// Apply health fields.
	SetUInt32Value(UNIT_FIELD_HEALTH, m_uint32Values[UNIT_FIELD_BASE_HEALTH]);
	SetUInt32Value(UNIT_FIELD_MAXHEALTH, m_uint32Values[UNIT_FIELD_BASE_HEALTH]);
	SetUInt32Value(UNIT_FIELD_POWER1, m_uint32Values[UNIT_FIELD_BASE_MANA]);
	SetUInt32Value(UNIT_FIELD_MAXPOWER1, m_uint32Values[UNIT_FIELD_BASE_MANA]);
}

uint16 Pet::SpellTP(uint32 spellId)
{
	//returns required training points for spell
	skilllinespell *sk = objmgr.GetSpellSkill(spellId);
	if(sk)
		return sk->reqTP;
	return 0;
}
uint16 Pet::GetUsedTP()
{
	//goes through all pet spells and sums training points
	int16 sumTP = 0;
	if(mSpells.size() > 0)
	{
		PetSpellMap::iterator itr = mSpells.begin();
		for(; itr != mSpells.end(); ++itr)
		{
			//higher spell ranks takes TP incrementally from lower rank, so lets sum the highest rank spells
			if(itr->first->Id == GetHighestRankSpell(itr->first->Id))
				sumTP += SpellTP(itr->first->Id);
		}
	}
	return sumTP;
}
bool Pet::CanLearnSpellTP(uint32 spellId)
{
	//checks if pet has enough TPs to learn new spell
	//higher spell ranks take TPs incrementallly, so we need this calculation
	return SpellTP(spellId) - SpellTP(GetHighestRankSpell(spellId)) >  TP ? false : true;
}
void Pet::UpdateTP()
{
	//update pets TP
	//formula: TP = level*(loyaltyLvl - 1) - usedTP
	//http://petopia.brashendeavors.net/html/articles/skills_main.shtml
	if(!m_Owner || Summon) return;
	int16 pts = getLevel()*(GetLoyaltyLevel()-1)-GetUsedTP();
	TP = pts;
	SetUInt32Value(UNIT_TRAINING_POINTS, pts < 0?(-pts & 0xffff):(pts<<16));//uff, works, but has anybody better idea?
}
HappinessState Pet::GetHappinessState() 
{
	//gets happiness state from happiness points
	if(GetUInt32Value(UNIT_FIELD_POWER5)<PET_HAPPINESS_UPDATE_VALUE)
		return UNHAPPY;
	else if(GetUInt32Value(UNIT_FIELD_POWER5)>=2*PET_HAPPINESS_UPDATE_VALUE)
		return HAPPY;
	else
		return CONTENT;
}
void Pet::AddPetSpellToOwner(uint32 spellId)
{
	//exit if owner hasn't Beast training ability (id 5149)
	if(!m_Owner || !m_Owner->HasSpell(5149))
		return;
	//find appropriate teaching spell...
	uint32 TeachingSpellID = 0;
	TeachingSpellID = sWorld.GetTeachingSpell(spellId);
    if(TeachingSpellID)
	{
		if(m_Owner->HasSpell(TeachingSpellID))
			return;
		else
		{
			//...and add it to pet owner to be able teach other pets
			m_Owner->addSpell(TeachingSpellID);
			return;
		}
	
	}
	else
		sLog.outDebug("WORLD: Could not find teaching spell for spell %u", spellId);
}
uint32 Pet::GetHighestRankSpell(uint32 spellId)
{	
	//get the highest rank of spell from known spells
	SpellEntry *sp = dbcSpell.LookupEntry(spellId);
	SpellEntry *tmp = 0;
	if(sp && mSpells.size() > 0)
	{
		PetSpellMap::iterator itr = mSpells.begin();
		for(; itr != mSpells.end(); ++itr)
			if(sp->NameHash == itr->first->NameHash)
				if((!tmp || tmp->RankNumber < itr->first->RankNumber))
					 tmp = itr->first;
	}
	return tmp ? tmp->Id : 0;
}
void Pet::UpdateLoyalty(char pts)
{	
	//updates loyalty_pts and loyalty lvl if needed
	if((LoyaltyPts + pts) < 0)
		LoyaltyPts = 0;
	else if ((LoyaltyPts + pts) > BEST_FRIEND * PET_LOYALTY_LVL_RANGE)
		LoyaltyPts = BEST_FRIEND * PET_LOYALTY_LVL_RANGE;
	else 
		LoyaltyPts += pts;
	char curLvl = GetLoyaltyLevel();
	char newLvl = LoyaltyPts / PET_LOYALTY_LVL_RANGE; //result can be 0~7 ...
	newLvl < BEST_FRIEND ? newLvl += 1 : newLvl = BEST_FRIEND;//...so make it 1~6
	if (newLvl != curLvl)
	{
		SetUInt32Value(UNIT_FIELD_BYTES_1, 0 | (newLvl << 8));
		UpdateTP();
	}
}

AI_Spell * Pet::HandleAutoCastEvent()
{
	if(m_autoCastSpells[AUTOCAST_EVENT_ATTACK].size() > 1)
	{
		// more than one autocast spell. pick a random one.
		uint32 c = RandomUInt((uint32)m_autoCastSpells[AUTOCAST_EVENT_ATTACK].size());
		uint32 j = 0;
		list<AI_Spell*>::iterator itr = m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin();

		for(; itr != m_autoCastSpells[AUTOCAST_EVENT_ATTACK].end(), j < c; ++j, ++itr);
		if(itr == m_autoCastSpells[AUTOCAST_EVENT_ATTACK].end())
		{
			if( (*m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin())->autocast_type == AUTOCAST_EVENT_ATTACK )
				return *m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin();
			else
			{
				// bad pointers somehow end up here :S
				m_autoCastSpells[AUTOCAST_EVENT_ATTACK].erase(m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin());
				return HandleAutoCastEvent();
			}
		}
		else
		{
			if( (*itr)->autocast_type == AUTOCAST_EVENT_ATTACK )
				return *itr;
			else
			{
				m_autoCastSpells[AUTOCAST_EVENT_ATTACK].erase(itr);
				return HandleAutoCastEvent();
			}
		}
	}
	else if(m_autoCastSpells[AUTOCAST_EVENT_ATTACK].size())
	{
		AI_Spell * sp = *m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin();
		if( sp->autocast_type == AUTOCAST_EVENT_ATTACK )
		{
			if( sp->cooldown && getMSTime() >= sp->cooldowntime )
				return *m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin();
			else
				return NULL;
		}
		else
		{
			// bad pointers somehow end up here :S
			m_autoCastSpells[AUTOCAST_EVENT_ATTACK].erase(m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin());
			return NULL;
		}
	}
	
	return NULL;
}

void Pet::HandleAutoCastEvent(uint32 Type)
{
	if(!m_Owner)
		return;

	if(Type == AUTOCAST_EVENT_ATTACK)
	{
		if(m_autoCastSpells[AUTOCAST_EVENT_ATTACK].size() > 1)
		{
			// more than one autocast spell. pick a random one.
			uint32 i;
			uint32 ms = getMSTime();
			for(i=0;i<m_autoCastSpells[AUTOCAST_EVENT_ATTACK].size();++i)
			{
				uint32 c = RandomUInt((uint32)m_autoCastSpells[AUTOCAST_EVENT_ATTACK].size());
				uint32 j = 0;
				list<AI_Spell*>::iterator itr = m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin();

				for(; itr != m_autoCastSpells[AUTOCAST_EVENT_ATTACK].end(), j < c; ++j, ++itr);
				if(itr == m_autoCastSpells[AUTOCAST_EVENT_ATTACK].end())
				{
					m_aiInterface->SetNextSpell(*m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin());
					break;
				}
				else
				{
					if((*itr)->cooldowntime && (*itr)->cooldowntime > ms)
						continue;

					m_aiInterface->SetNextSpell(*itr);
				}
			}
		}
		else if(m_autoCastSpells[AUTOCAST_EVENT_ATTACK].size())
		{
			AI_Spell * sp =*m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin();
			if(sp->cooldown && getMSTime() < sp->cooldowntime)
				return;

			m_aiInterface->SetNextSpell(*m_autoCastSpells[AUTOCAST_EVENT_ATTACK].begin());
		}
		return;
	}

	for( list<AI_Spell*>::iterator itr = m_autoCastSpells[Type].begin(); itr != m_autoCastSpells[Type].end(); ++itr )
	{
		if( (*itr)->spelltargetType == TTYPE_OWNER )
			CastSpell( m_Owner, (*itr)->spell, false );
		else
		{
			//modified by Zack: Spell targetting will be generated in the castspell function now.You cannot force to target self all the time
			CastSpell( static_cast< Unit* >( NULL ), (*itr)->spell, false);
		}
	}
}

void Pet::SetAutoCast(AI_Spell * sp, bool on)
{
	if(sp->autocast_type > 0)
	{
		if(!on)
		{
			for(list<AI_Spell*>::iterator itr = m_autoCastSpells[sp->autocast_type].begin();
				itr != m_autoCastSpells[sp->autocast_type].end(); ++itr)
			{
				if( (*itr) == sp )
				{
					m_autoCastSpells[sp->autocast_type].erase(itr);
					break;
				}
			}
		}
		else
		{
			for(list<AI_Spell*>::iterator itr = m_autoCastSpells[sp->autocast_type].begin();
				itr != m_autoCastSpells[sp->autocast_type].end(); ++itr)
			{
				if((*itr) == sp)
					return;
			}

			m_autoCastSpells[sp->autocast_type].push_back(sp);
		}
	}
}
