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

/* Actual Aura Code */
bool HandleDummyAura(uint32 i, Aura* pAura, bool apply)
{
    if(i != 0) return false;

	// omfg, so god damn fucking hacky.. but i guess w/ dummy spells
    // you cant expect everything to be in dbc :D

    // well, i could parse the strings :P but bah i'll just hardcode it here,
    // that's the point of scripts, right? ;)

    uint32 applyId = 0;
    Unit * u_caster = pAura->GetUnitCaster();
    if(u_caster == 0 || !u_caster->IsPlayer()) return false;

    switch(pAura->m_spellProto->Id)
    {
        
    case 20154:         // Rank 1: "Melee attacks cause an additional $/87;20187s3 to $/25;20187s3 Holy damage."
        applyId = 20187;
        break; 
    case 20287:         // Rank 2: $/25;20280s3 
       applyId = 20280;
       break;
    case 20288:         // Rank 3:
        applyId = 20281;
        break;
    case 20289:         // Rank 4
        applyId = 20282;
        break;
    case 20290:         // Rank 5
        applyId = 20283;
        break;
    case 20291:         // Rank 6
        applyId = 20284;
        break;
    case 20292:         // Rank 7
        applyId = 20285;
        break;
    case 20293:         // Rank 8
        applyId = 20286;
        break;
    case 27155:         // Rank 9
        applyId = 27157;
        break;
    }

    SpellEntry * m_spellInfo = sSpellStore.LookupEntry(applyId);
    
    if(apply == true)
    {
        int32 value = 0;
        float randomPointsPerLevel = m_spellInfo->EffectDicePerLevel[2];
        int32 basePoints = m_spellInfo->EffectBasePoints[2] + 1;
        int32 randomPoints = m_spellInfo->EffectDieSides[2];
        if(u_caster)
            randomPoints += u_caster->getLevel() * (int32)randomPointsPerLevel;

        if(randomPoints <= 1)
            value = basePoints;
        else
            value = basePoints + rand()  %randomPoints;

        //this may be dangerous but let it be
        /*if(m_spellInfo->SpellGroupType)
        {
            SM_FIValue(u_caster->SM_FDummy,&value,m_spellInfo->SpellGroupType);
            SM_PIValue(u_caster->SM_PDummy,&value,m_spellInfo->SpellGroupType);
        }*/

        // add spell damage!
        uint32 max_dmg = value / 21;
        uint32 min_dmg = value / 27;
        ((Player*)u_caster)->AddOnStrikeSpellDamage(pAura->m_spellProto->Id, min_dmg, max_dmg);

        // set the seal business
        if(u_caster->GetTypeId() == TYPEID_PLAYER)
        {
            ((Player*)u_caster)->judgespell = applyId;
            ((Player*)u_caster)->Seal = pAura->m_spellProto->Id;
        }
        u_caster->SetFlag(UNIT_FIELD_AURASTATE, 16);
    }
    else
    {
        ((Player*)u_caster)->RemoveOnStrikeSpellDamage(pAura->m_spellProto->Id);
        // set the seal business
        if(u_caster->GetTypeId() == TYPEID_PLAYER)
        {
            ((Player*)u_caster)->judgespell = 0;
            ((Player*)u_caster)->Seal = 0;
        }
        u_caster->RemoveFlag(UNIT_FIELD_AURASTATE, 16);
    }

    return true;
}

void SetupPaladinSpells(ScriptMgr * mgr)
{
    mgr->register_dummy_aura(20154, &HandleDummyAura);
    mgr->register_dummy_aura(20287, &HandleDummyAura);
    mgr->register_dummy_aura(20288, &HandleDummyAura);
    mgr->register_dummy_aura(20289, &HandleDummyAura);
    mgr->register_dummy_aura(20290, &HandleDummyAura);
    mgr->register_dummy_aura(20291, &HandleDummyAura);
    mgr->register_dummy_aura(20292, &HandleDummyAura);
    mgr->register_dummy_aura(20293, &HandleDummyAura);
    mgr->register_dummy_aura(27155, &HandleDummyAura);
}
