/****************************************************************************
 *
 * SpellHandler Plugin
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 */

#include "StdAfx.h"
#include "Setup.h"

/*
	How to add a new item spell to the dummy spell handler:

		1) Add a new function to handle the spell at the end of this file but before the
		   SetupItemSpells_1() function. SetupItemSpells_1() must always be the last function.

		2) Register the dummy spell by adding a new line to the end of the list in the
		   SetupItemSpells_1() function.

	Please look at how the other spells are handled and try to use the
	same variable names and formatting style in your new spell handler.
*/

// *****************************************************************************

bool GnomishTransporter(uint32 i, Spell * pSpell)
{
	if(!pSpell->p_caster) return true;

	pSpell->p_caster->EventAttackStop();
	pSpell->p_caster->SafeTeleport(1, 0, LocationVector(-7169.41f, -3838.63f, 8.72f));
	return true;
}

// -----------------------------------------------------------------------------

bool NoggenFoggerElixr(uint32 i, Spell * pSpell)
{
	if(!pSpell->p_caster) return true;

	uint32 chance = RandomUInt(2);

	switch(chance)
	{
	case 0:
		pSpell->p_caster->CastSpell(pSpell->p_caster, dbcSpell.LookupEntry(16591), true);
		break;
	case 1:
		pSpell->p_caster->CastSpell(pSpell->p_caster, dbcSpell.LookupEntry(16593), true);
		break;
	case 2:
		pSpell->p_caster->CastSpell(pSpell->p_caster, dbcSpell.LookupEntry(16595), true);
		break;
	}
	return true;
}

// -----------------------------------------------------------------------------

bool HallowsEndCandy(uint32 i, Spell * pSpell)
{
	if(!pSpell->p_caster) return true;

	int newspell = 24924 + RandomUInt(3);

	SpellEntry *spInfo = dbcSpell.LookupEntry(newspell);
	if(!spInfo) return true;

	pSpell->p_caster->CastSpell(pSpell->p_caster, spInfo, true);
	return true;
}

// -----------------------------------------------------------------------------

bool DeviateFish(uint32 i, Spell * pSpell)
{
	if(!pSpell->p_caster) return true;

	int newspell = 8064 + RandomUInt(4);

	SpellEntry *spInfo = dbcSpell.LookupEntry(newspell);
	if(!spInfo) return true;

	pSpell->p_caster->CastSpell(pSpell->p_caster, spInfo, true);
	return true;
}

// -----------------------------------------------------------------------------

bool CookedDeviateFish(uint32 i, Spell * pSpell)
{
	if(!pSpell->p_caster) return true;

	int chance = 0;
	int newspell = 0;

	chance = RandomUInt(1);

	switch(chance)
	{
	case 0:
		newspell = 8219; // Flip Out (60 min) (turns you into a ninja)
		break;
	case 1:
		newspell = 8221; // Yaaarrrr (60 min) (turns you into a pirate)
		break;
	}

	if(newspell)
	{
		SpellEntry *spInfo = dbcSpell.LookupEntry(newspell);
		if(!spInfo) return true;

		pSpell->p_caster->CastSpell(pSpell->p_caster, spInfo, true);
	}
	return true;
}

// -----------------------------------------------------------------------------

bool HolidayCheer(uint32 i, Spell * pSpell)
{
	if(!pSpell->m_caster) return true;

	Unit *target;
	float dist = pSpell->GetRadius(i);

	for(ObjectSet::iterator itr = pSpell->m_caster->GetInRangeSetBegin(); itr != pSpell->m_caster->GetInRangeSetEnd(); ++itr)
	{
		if((*itr)->IsUnit())
			target = (Unit*)*itr;
		else
			continue;

		if(pSpell->m_caster->CalcDistance(target) > dist || isAttackable(pSpell->m_caster, target))
			continue;

		target->Emote(EMOTE_ONESHOT_LAUGH);
	}
	return true;
}

// -----------------------------------------------------------------------------

bool NetOMatic(uint32 i, Spell * pSpell)
{
	Unit *target = pSpell->GetUnitTarget();
	if(!pSpell->p_caster || !target) return true;

	SpellEntry *spInfo = dbcSpell.LookupEntry(13099);
	if(!spInfo) return true;

	int chance = RandomUInt(99)+1;

	if(chance < 51) // nets target: 50%
		pSpell->p_caster->CastSpell(target, spInfo, true);

	else if(chance < 76) // nets you instead: 25%
		pSpell->p_caster->CastSpell(pSpell->p_caster, spInfo, true);

	else // nets you and target: 25%
	{
		pSpell->p_caster->CastSpell(pSpell->p_caster, spInfo, true);
		pSpell->p_caster->CastSpell(target, spInfo, true);
	}
	return true;
}

// -----------------------------------------------------------------------------

bool BanishExile(uint32 i, Spell * pSpell)
{
	Unit* target = pSpell->GetUnitTarget();
	if(!pSpell->p_caster || !target) return true;

	pSpell->p_caster->SpellNonMeleeDamageLog(target, pSpell->m_spellInfo->Id, target->GetUInt32Value(UNIT_FIELD_HEALTH), true);
	return true;
}

// -----------------------------------------------------------------------------

bool HolyShock(uint32 i, Spell *pSpell)
{
	Unit *target = pSpell->GetUnitTarget();
	if(!pSpell->p_caster || !target) return true;

	uint32 newspell = 0;

	if(isAttackable(pSpell->p_caster,target)) // if its an enemy
	{
		switch(pSpell->m_spellInfo->Id)
		{
		case 20473:
			newspell = 25912;
			break;
		case 20929:
			newspell = 25911;
			break;
		case 20930:
			newspell = 25902;
			break;
		case 27174:
			newspell = 27176;
			break;
		case 33072:
			newspell = 33073;
			break;
		}
	}
	else // if its friendly
	{
		switch(pSpell->m_spellInfo->Id)
		{
		case 20473:
			newspell = 25914;
			break;
		case 20929:
			newspell = 25913;
			break;
		case 20930:
			newspell = 25903;
			break;
		case 27174:
			newspell = 27175;
			break;
		case 33072:
			newspell = 33074;
			break;
		}
	}

	SpellEntry *spInfo = dbcSpell.LookupEntry(newspell);
	if(!spInfo) return true;

	pSpell->p_caster->CastSpell(target, spInfo, true);
	return true;
}

// -----------------------------------------------------------------------------

bool ForemansBlackjack(uint32 i, Spell *pSpell)
{
	Unit* target = pSpell->GetUnitTarget();
	if(!pSpell->p_caster || !target || target->GetTypeId() != TYPEID_UNIT) return true;

	// play sound and animation
	WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 12);
	data << uint32(6197) << target->GetGUID();
	pSpell->p_caster->SendMessageToSet(&data, true);

	// send chat message
	char msg[100];
	sprintf(msg, "Ow! Ok, I'll get back to work, %s", pSpell->p_caster->GetName());
	target->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, msg);

	Creature* c_target = (Creature*)target;
	if(!c_target) return true;

	uint32 creatureID = c_target->GetEntry();

  // check to see that we have the correct creature and increment the quest log
	if(creatureID == 10556)
		sQuestMgr.OnPlayerKill(pSpell->p_caster, c_target);

	return true;
}

// -----------------------------------------------------------------------------

bool NetherWraithBeacon(uint32 i, Spell *pSpell)
{
	if(!pSpell->p_caster) return true;

	float SSX = pSpell->p_caster->GetPositionX();
	float SSY = pSpell->p_caster->GetPositionY();
	float SSZ = pSpell->p_caster->GetPositionZ();
	float SSO = pSpell->p_caster->GetOrientation();

	pSpell->p_caster->GetMapMgr()->GetInterface()->SpawnCreature(22408, SSX, SSY, SSZ, SSO, true, false, 0, 0);
	return true;
}

// -----------------------------------------------------------------------------

bool SymbolOfLife(uint32 i, Spell *pSpell)
{
	/*  // commented out until the SpellTargetDummy (38) targeting is fixed
	Unit* target = pSpell->GetUnitTarget();
	if(!pSpell->p_caster || !target || target->GetTypeId() != TYPEID_UNIT) return true;

	Creature* c_target = (Creature*)target;
	if(!c_target) return true;

	uint32 creatureID = c_target->GetEntry();

  // check to see we that have the correct creature and increment the quest log
	if(creatureID == 17542 || creatureID == 6172)
		sQuestMgr.OnPlayerKill(pSpell->p_caster, c_target);
	*/
	return true;
}

// -----------------------------------------------------------------------------

bool NighInvulnBelt(uint32 i, Spell *pSpell)
{
    if(!pSpell->p_caster) return true;

    int chance = RandomUInt(99)+1;

    if(chance > 10) // Buff - Nigh-Invulnerability - 30456
        pSpell->p_caster->CastSpell(pSpell->p_caster, dbcSpell.LookupEntry(30456), true);
    else // Malfunction - Complete Vulnerability - 30457
        pSpell->p_caster->CastSpell(pSpell->p_caster, dbcSpell.LookupEntry(30457), true);

    return true;
}



// ADD NEW FUNCTIONS ABOVE THIS LINE
// *****************************************************************************

void SetupItemSpells_1(ScriptMgr * mgr)
{
	mgr->register_dummy_spell(23453, &GnomishTransporter); // Gnomish Transporter
	mgr->register_dummy_spell(16589, &NoggenFoggerElixr);  // Noggenfogger
	mgr->register_dummy_spell(24430, &HallowsEndCandy);    // Hallows End Candy
	mgr->register_dummy_spell( 8063, &DeviateFish);        // Deviate Fish
	mgr->register_dummy_spell( 8213, &CookedDeviateFish);  // Savory Deviate Delight
	mgr->register_dummy_spell(26074, &HolidayCheer);       // Holiday Cheer
	mgr->register_dummy_spell(13120, &NetOMatic);          // Net-o-Matic
	mgr->register_dummy_spell( 4130, &BanishExile);        // Essence of the Exile Quest
	mgr->register_dummy_spell( 4131, &BanishExile);        // Essence of the Exile Quest
	mgr->register_dummy_spell( 4132, &BanishExile);        // Essence of the Exile Quest
	mgr->register_dummy_spell(20473, &HolyShock);          // Holy Shock rank 1
	mgr->register_dummy_spell(20929, &HolyShock);          // Holy Shock rank 2
	mgr->register_dummy_spell(20930, &HolyShock);          // Holy Shock rank 3
	mgr->register_dummy_spell(27174, &HolyShock);          // Holy Shock rank 4
	mgr->register_dummy_spell(33072, &HolyShock);          // Holy Shock rank 5
	mgr->register_dummy_spell(19938, &ForemansBlackjack);  // Lazy Peons Quest
	mgr->register_dummy_spell(39105, &NetherWraithBeacon); // Spellfire Tailor Quest
	mgr->register_dummy_spell( 8593, &SymbolOfLife);       // Paladin's Redemption QuestLine
	mgr->register_dummy_spell(30458, &NighInvulnBelt);     // Nigh Invulnerability Belt



// REGISTER NEW DUMMY SPELLS ABOVE THIS LINE
// *****************************************************************************
}
