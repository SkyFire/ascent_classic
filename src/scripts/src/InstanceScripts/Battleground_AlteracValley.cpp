#include "StdAfx.h"
#include "Setup.h"

#define CN_VANNDAR_STORMPIKE 11948
#define CN_DREKTHAR 11946
#define CN_BALINDASTONEHEARTH 11949
#define CN_GALVANGAR 11947
#define CN_STORMPIKEBOWMAN 13358
#define CN_FROSTWOLFBOWMAN 13359

#define BOWMAN_SHOOT 22121

#define DREKTHAR_STORMSTRIKE 17364
#define DREKTHAR_CLEAVE 29561
#define DREKTHAR_AOEKNOCKDOWN 20276

#define VANNDAR_AVATAR 19135
#define VANNDAR_THUNDERCLAP 15588
#define VANNDAR_STORMBOLT 20685

#define BALINDA_ARCANEEXPLOSION 46608
#define BALINDA_CONEOFCOLD 38384
#define BALINDA_FIREBALL 46988
#define BALINDA_FROSTBOLT 46987
#define BALINDA_SUMMONWATERELEMENTAL 45067

#define GALVANGAR_CLEAVE 15284
#define GALVANGAR_FRIGHTENINGSHOUT 19134
#define GALVANGAR_MORTALSTRIKE 16856
#define GALVANGAR_WHIRLWIND 15589

class SelfBuffNPCAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(SelfBuffNPCAI);

	SelfBuffNPCAI(Creature* pCreature) : CreatureAIScript(pCreature)
	{
		uint32 castspell = 0;
		switch(pCreature->GetEntry())
		{
		case 14762: // DB North
			castspell = 45828;
			break;
		case 14763: // DB South
			castspell = 45829;
			break;
		case 14764: // Icewing
			castspell = 45831;
			break;
		case 14765: // Stonehearth
			castspell = 45830;
			break;

		case 14773: // Iceblood
			castspell = 45722;
			break;
		case 14772: // East Frostwolf
			castspell = 45826;
			break;
		case 14776: // Tower Point
			castspell = 45823;
			break;
		case 14777: // West Frostwolf
			castspell = 45824;
			break;
		}

		if(castspell)
		{
			pCreature->CastSpell(pCreature, castspell, true);
		}
	}
};

class BowmanAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(BowmanAI);

	BowmanAI(Creature* pCreature) : CreatureAIScript(pCreature)
	{
		pCreature->GetAIInterface()->m_canMove = false;
	}

	void OnLoad()
	{
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
	}

	void OnCombatStart(Unit* mTarget)
	{
		
	}

	void OnCombatStop(Unit* mTarget)
	{

	}

	void OnDied(Unit * mKiller)
	{
		RemoveAIUpdateEvent();
	}

	void AIUpdate()
	{
		std::set<Object*>::iterator itr = _unit->GetInRangeOppFactsSetBegin();
		for(; itr != _unit->GetInRangeOppFactsSetEnd(); itr++)
		{
			Object * pObj = (*itr);
			if( pObj->IsUnit() && _unit->CanSee( static_cast<Unit*>(pObj) ))
			{
				if( pObj->IsPlayer() && TO_PLAYER(pObj)->m_isGmInvisible )
					continue;

				if( pObj->GetDistance2dSq(_unit) > (60*60) )
					continue;

				_unit->CastSpell(TO_UNIT(pObj), dbcSpell.LookupEntryForced(BOWMAN_SHOOT), true);
				return;
			}
		}
	}
};

class CaptainGalvangarAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(CaptainGalvangarAI);

	uint32 nrspells;
	SP_AI_Spell spells[3];
	bool m_spellcheck[3];

	CaptainGalvangarAI(Creature* pCreature) : CreatureAIScript(pCreature)
	{
		nrspells = 3;

		_unit->MechanicsDispels[ DISPEL_MECHANIC_CHARM ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_FEAR ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_ROOT ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_SLEEP ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_SNARE ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_STUN ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_KNOCKOUT ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_POLYMORPH ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_BANISH ] = 1;

		spells[0].info = dbcSpell.LookupEntry(GALVANGAR_CLEAVE);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000; // 1sec

		spells[1].info = dbcSpell.LookupEntry(GALVANGAR_FRIGHTENINGSHOUT);
		spells[1].targettype = TARGET_VARIOUS;
		spells[1].instant = true;
		spells[1].perctrigger = 10.0f;
		spells[1].attackstoptimer = 1000; // 1sec

		spells[2].info = dbcSpell.LookupEntry(GALVANGAR_MORTALSTRIKE);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].perctrigger = 10.0f;
		spells[2].attackstoptimer = 1000; // 1sec

		spells[3].info = dbcSpell.LookupEntry(GALVANGAR_WHIRLWIND);
		spells[3].targettype = TARGET_ATTACKING;
		spells[3].instant = true;
		spells[3].perctrigger = 10.0f;
		spells[3].attackstoptimer = 1000; // 1sec
	}

	void OnCombatStart(Unit* mTarget)
	{
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
		
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Die! Your kind has no place in Alterac Valley!");
		_unit->PlaySoundToSet(SOUND_ALLIANCE_CAPTURE);
	}

	void OnCombatStop(Unit* mTarget)
	{
		_unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
		_unit->GetAIInterface()->SetAIState(STATE_IDLE);
		RemoveAIUpdateEvent();
	}

	void OnDied(Unit * mKiller)
	{
		RemoveAIUpdateEvent();
	}

	void AIUpdate()
	{
		float val = (float)RandomFloat(100.0f);
		SpellCast(val);

#ifdef COLLISION
		// Cool method of determining if this guy is inside.
		if(!CollideInterface.IsIndoor(30, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ()))
		{
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I'll never fall for that, fool! If you want a battle, it will be on my terms and in my lair.");
			_unit->GetAIInterface()->WipeTargetList();
			_unit->GetAIInterface()->SetAIState(STATE_IDLE);
			_unit->GetAIInterface()->MoveTo(_unit->GetSpawnX(), _unit->GetSpawnY(), _unit->GetSpawnZ(), _unit->GetSpawnO());
			_unit->CombatStatus.Vanished();
		}
#endif
	}

	void SpellCast(float val)
	{
		if(_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
		{
			float comulativeperc = 0;
			Unit *target = NULL;
			for(uint32 i=0;i<nrspells;i++)
			{
				if(!spells[i].perctrigger) continue;

				if(m_spellcheck[i])
				{
					target = _unit->GetAIInterface()->GetNextTarget();
					switch(spells[i].targettype)
					{
					case TARGET_SELF:
					case TARGET_VARIOUS:
						_unit->CastSpell(_unit, spells[i].info, spells[i].instant); break;
					case TARGET_ATTACKING:
						_unit->CastSpell(target, spells[i].info, spells[i].instant); break;
					case TARGET_DESTINATION:
						_unit->CastSpellAoF(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(), spells[i].info, spells[i].instant); break;
					}
					m_spellcheck[i] = false;
					return;
				}

				if(val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger))
				{
					_unit->setAttackTimer(spells[i].attackstoptimer, false);
					m_spellcheck[i] = true;
				}
				comulativeperc += spells[i].perctrigger;
			}
		}
	}
};

class BalindaStonehearthAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(BalindaStonehearthAI);

	uint32 nrspells;
	SP_AI_Spell spells[3];
	bool m_spellcheck[3];

	BalindaStonehearthAI(Creature* pCreature) : CreatureAIScript(pCreature)
	{
		nrspells = 3;

		_unit->MechanicsDispels[ DISPEL_MECHANIC_CHARM ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_FEAR ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_ROOT ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_SLEEP ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_SNARE ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_STUN ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_KNOCKOUT ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_POLYMORPH ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_BANISH ] = 1;

		spells[0].info = dbcSpell.LookupEntry(BALINDA_FIREBALL);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = false;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000; // 1sec
		
		spells[1].info = dbcSpell.LookupEntry(BALINDA_FROSTBOLT);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = false;
		spells[1].perctrigger = 10.0f;
		spells[1].attackstoptimer = 1000; // 1sec

		spells[2].info = dbcSpell.LookupEntry(BALINDA_SUMMONWATERELEMENTAL);
		spells[2].targettype = TARGET_SELF;
		spells[2].instant = false;
		spells[2].perctrigger = 5.0f;
		spells[2].attackstoptimer = 1000; // 1sec

		spells[3].info = dbcSpell.LookupEntry(BALINDA_CONEOFCOLD);
		spells[3].targettype = TARGET_VARIOUS;
		spells[3].instant = false;
		spells[3].perctrigger = 8.0f;
		spells[3].attackstoptimer = 1000; // 1sec

		_unit->GetAIInterface()->m_canMove = false;
	}

	void OnCombatStart(Unit* mTarget)
	{
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));

		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Begone, uncouth scum! The Alliance will prevail in Alterac Valley!");
		_unit->PlaySoundToSet(SOUND_ALLIANCE_CAPTURE);
	}

	void OnCombatStop(Unit* mTarget)
	{
		_unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
		_unit->GetAIInterface()->SetAIState(STATE_IDLE);
		RemoveAIUpdateEvent();
	}

	void OnDied(Unit * mKiller)
	{
		RemoveAIUpdateEvent();
	}

	void AIUpdate()
	{
		float val = (float)RandomFloat(100.0f);
		SpellCast(val);

#ifdef COLLISION
		// Cool method of determining if this guy is inside.
		if(!CollideInterface.IsIndoor(30, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ()))
		{
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Filthy Frostwolf cowards! If you want a fight, you'll have to come to me!");
			_unit->GetAIInterface()->WipeTargetList();
			_unit->GetAIInterface()->SetAIState(STATE_IDLE);
			_unit->GetAIInterface()->MoveTo(_unit->GetSpawnX(), _unit->GetSpawnY(), _unit->GetSpawnZ(), _unit->GetSpawnO());
			_unit->CombatStatus.Vanished();
		}
#endif
	}

	void SpellCast(float val)
	{
		if(_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
		{
			float comulativeperc = 0;
			Unit *target = NULL;
			for(uint32 i=0;i<nrspells;i++)
			{
				if(!spells[i].perctrigger) continue;

				if(m_spellcheck[i])
				{
					target = _unit->GetAIInterface()->GetNextTarget();
					switch(spells[i].targettype)
					{
					case TARGET_SELF:
					case TARGET_VARIOUS:
						_unit->CastSpell(_unit, spells[i].info, spells[i].instant); break;
					case TARGET_ATTACKING:
						_unit->CastSpell(target, spells[i].info, spells[i].instant); break;
					case TARGET_DESTINATION:
						_unit->CastSpellAoF(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(), spells[i].info, spells[i].instant); break;
					}
					m_spellcheck[i] = false;
					return;
				}

				if(val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger))
				{
					_unit->setAttackTimer(spells[i].attackstoptimer, false);
					m_spellcheck[i] = true;
				}
				comulativeperc += spells[i].perctrigger;
			}
		}
	}
};

class DrektharAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(DrektharAI);

	uint32 nrspells;
	SP_AI_Spell spells[3];
	bool m_spellcheck[3];

	DrektharAI(Creature* pCreature) : CreatureAIScript(pCreature)
	{
		nrspells = 3;

		_unit->MechanicsDispels[ DISPEL_MECHANIC_CHARM ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_FEAR ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_ROOT ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_SLEEP ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_SNARE ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_STUN ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_KNOCKOUT ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_POLYMORPH ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_BANISH ] = 1;

		spells[0].info = dbcSpell.LookupEntry(DREKTHAR_AOEKNOCKDOWN);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = true;
		spells[0].perctrigger = 4.0f;
		spells[0].attackstoptimer = 1000; // 1sec
		

		spells[1].info = dbcSpell.LookupEntry(DREKTHAR_CLEAVE);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].perctrigger = 10.0f;
		spells[1].attackstoptimer = 1000; // 1sec
		

		spells[2].info = dbcSpell.LookupEntry(DREKTHAR_STORMSTRIKE);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].perctrigger = 10.0f;
		spells[2].attackstoptimer = 2000; // 1sec
	}

	void OnCombatStart(Unit* mTarget)
	{
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));

		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Stormpike filth! In my keep?! Slay them all!");
		_unit->PlaySoundToSet(SOUND_ALLIANCE_CAPTURE);

		Creature * pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14773);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mTarget, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14772);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mTarget, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14776);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mTarget, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14777);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mTarget, 1 );
	}

	void OnCombatStop(Unit* mTarget)
	{
		_unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
		_unit->GetAIInterface()->SetAIState(STATE_IDLE);
		RemoveAIUpdateEvent();
	}

	void OnDied(Unit * mKiller)
	{
		RemoveAIUpdateEvent();
	}

	void OnDamageTaken(Unit* mAttacker, float fAmount)
	{
		Creature * pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14773);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mAttacker, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14772);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mAttacker, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14776);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mAttacker, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14777);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mAttacker, 1 );
	}

	void AIUpdate()
	{
		float val = (float)RandomFloat(100.0f);
		SpellCast(val);

		if(Rand((uint32)3))
		{
			const char* quotes[] = {
				"Your attacks are slowed by the cold, I think!",
				"Today, you will meet your ancestors!",
				"If you will not leave Alterac Valley on your own, then the Frostwolves will force you out!",
				"You cannot defeat the Frostwolf clan!",
				"You are no match for the strength of the Horde!" //4
			};

			uint32 idx = RandomUInt(4);
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, quotes[idx]);
		}

#ifdef COLLISION
		// Cool method of determining if this guy is inside.
		if(!CollideInterface.IsIndoor(30, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ()))
		{
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You seek to draw the General of the Frostwolf legion out from his fortress? PREPOSTEROUS!");
			_unit->GetAIInterface()->WipeTargetList();
			_unit->GetAIInterface()->SetAIState(STATE_IDLE);
			_unit->GetAIInterface()->MoveTo(_unit->GetSpawnX(), _unit->GetSpawnY(), _unit->GetSpawnZ(), _unit->GetSpawnO());
			_unit->CombatStatus.Vanished();
		}
#endif
	}

	void SpellCast(float val)
	{
		if(_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
		{
			float comulativeperc = 0;
			Unit *target = NULL;
			for(uint32 i=0;i<nrspells;i++)
			{
				if(!spells[i].perctrigger) continue;

				if(m_spellcheck[i])
				{
					target = _unit->GetAIInterface()->GetNextTarget();
					switch(spells[i].targettype)
					{
					case TARGET_SELF:
					case TARGET_VARIOUS:
						_unit->CastSpell(_unit, spells[i].info, spells[i].instant); break;
					case TARGET_ATTACKING:
						_unit->CastSpell(target, spells[i].info, spells[i].instant); break;
					case TARGET_DESTINATION:
						_unit->CastSpellAoF(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(), spells[i].info, spells[i].instant); break;
					}
					m_spellcheck[i] = false;
					return;
				}

				if(val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger))
				{
					_unit->setAttackTimer(spells[i].attackstoptimer, false);
					m_spellcheck[i] = true;
				}
				comulativeperc += spells[i].perctrigger;
			}
		}
	}
};

class VanndarStormpikeAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(VanndarStormpikeAI);

	uint32 nrspells;
	SP_AI_Spell spells[3];
	bool m_spellcheck[3];

	VanndarStormpikeAI(Creature* pCreature) : CreatureAIScript(pCreature)
	{
		// -- Number of spells to add --
		nrspells = 3;

		// --- Initialization ---
		for(uint32 i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------

		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
		spells[0].info = dbcSpell.LookupEntry(VANNDAR_AVATAR);
		spells[0].targettype = TARGET_SELF;
		spells[0].instant = true;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 0; // 1sec

		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
		spells[1].info = dbcSpell.LookupEntry(VANNDAR_THUNDERCLAP);
		spells[1].targettype = TARGET_VARIOUS;
		spells[1].instant = true;
		spells[1].perctrigger = 10.0f;
		spells[1].attackstoptimer = 1000; // 1sec

		spells[2].info = dbcSpell.LookupEntry(VANNDAR_STORMBOLT);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = false;
		spells[2].perctrigger = 10.0f;
		spells[2].attackstoptimer = 1000; // 1sec

		_unit->MechanicsDispels[ DISPEL_MECHANIC_CHARM ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_FEAR ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_ROOT ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_SLEEP ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_SNARE ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_STUN ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_KNOCKOUT ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_POLYMORPH ] = 1;
		_unit->MechanicsDispels[ DISPEL_MECHANIC_BANISH ] = 1;
	}

	void OnCombatStart(Unit* mTarget)
	{
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
		// Avatar!
		_unit->CastSpell(_unit, spells[0].info, spells[0].instant);

		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Soldiers of Stormpike, your General is under attack! I require aid! Come! Come! Slay these mangy Frostwolf dogs.");
		_unit->PlaySoundToSet(SOUND_HORDE_CAPTURE);

		Creature * pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14762);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mTarget, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14763);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mTarget, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14764);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mTarget, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14765);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mTarget, 1 );
	}

	void OnDamageTaken(Unit* mAttacker, float fAmount)
	{
		Creature * pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14762);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mAttacker, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14763);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mAttacker, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14764);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mAttacker, 1 );

		pCreature = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( _unit->GetPositionX(), _unit->GetPositionY(), 0.0f, 14765);
		if(pCreature)
			pCreature->GetAIInterface()->AttackReaction( mAttacker, 1 );
	}

	void OnCombatStop(Unit* mTarget)
	{
		_unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
		_unit->GetAIInterface()->SetAIState(STATE_IDLE);
		RemoveAIUpdateEvent();
		_unit->RemoveAura(VANNDAR_AVATAR);
	}

	void OnDied(Unit * mKiller)
	{
		RemoveAIUpdateEvent();
	}

	void AIUpdate()
	{
		float val = (float)RandomFloat(100.0f);
		SpellCast(val);

		if(Rand((uint32)3))
		{
			const char* quotes[] = {
				"I will tell you this much...Alterac Valley will be ours.",
				"Why don't ya try again without yer cheap tactics, pansies! Or are you too chicken?",
				"Your attacks are weak! Go practice on some rabbits and come back when you're stronger.",
				"We will not be swayed from our mission!",
				"It'll take more than your rabble to bring me down!",
				"We, the Alliance, will prevail!",
				"The Stormpike clan bows to no one, especially the Horde!",
				"Is that the best you can do?",
				"Take no prisoners! Drive these heathens from our lands!" // 8
			};

			uint32 idx = RandomUInt(8);
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, quotes[idx]);
		}

#ifdef COLLISION
		// Cool method of determining if this guy is inside.
		if(!CollideInterface.IsIndoor(30, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ()))
		{
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You'll never get me out of me bunker, heathens!");
			_unit->GetAIInterface()->WipeTargetList();
			_unit->GetAIInterface()->SetAIState(STATE_IDLE);
			_unit->GetAIInterface()->MoveTo(_unit->GetSpawnX(), _unit->GetSpawnY(), _unit->GetSpawnZ(), _unit->GetSpawnO());
			_unit->CombatStatus.Vanished();
		}
#endif
	}

	void SpellCast(float val)
	{
		if(_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
		{
			float comulativeperc = 0;
			Unit *target = NULL;
			for(uint32 i=0;i<nrspells;i++)
			{
				if(!spells[i].perctrigger) continue;

				if(m_spellcheck[i])
				{
					target = _unit->GetAIInterface()->GetNextTarget();
					switch(spells[i].targettype)
					{
					case TARGET_SELF:
					case TARGET_VARIOUS:
						_unit->CastSpell(_unit, spells[i].info, spells[i].instant); break;
					case TARGET_ATTACKING:
						_unit->CastSpell(target, spells[i].info, spells[i].instant); break;
					case TARGET_DESTINATION:
						_unit->CastSpellAoF(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(), spells[i].info, spells[i].instant); break;
					}
					m_spellcheck[i] = false;
					return;
				}

				if(val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger))
				{
					_unit->setAttackTimer(spells[i].attackstoptimer, false);
					m_spellcheck[i] = true;
				}
				comulativeperc += spells[i].perctrigger;
			}
		}
	}
};

void SetupAlteracValleyBattleground(ScriptMgr * mgr)
{
	mgr->register_creature_script( CN_VANNDAR_STORMPIKE, &VanndarStormpikeAI::Create);
	mgr->register_creature_script( CN_DREKTHAR, &DrektharAI::Create);
	mgr->register_creature_script( CN_BALINDASTONEHEARTH, &BalindaStonehearthAI::Create);
	mgr->register_creature_script( CN_GALVANGAR, &CaptainGalvangarAI::Create);

	mgr->register_creature_script( CN_STORMPIKEBOWMAN, &BowmanAI::Create);
	mgr->register_creature_script( CN_FROSTWOLFBOWMAN, &BowmanAI::Create);

	mgr->register_creature_script( 14762, &SelfBuffNPCAI::Create);
	mgr->register_creature_script( 14763, &SelfBuffNPCAI::Create);
	mgr->register_creature_script( 14764, &SelfBuffNPCAI::Create);
	mgr->register_creature_script( 14765, &SelfBuffNPCAI::Create);
	mgr->register_creature_script( 14773, &SelfBuffNPCAI::Create);
	mgr->register_creature_script( 14772, &SelfBuffNPCAI::Create);
	mgr->register_creature_script( 14776, &SelfBuffNPCAI::Create);
	mgr->register_creature_script( 14777, &SelfBuffNPCAI::Create);

}