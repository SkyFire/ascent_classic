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

bool GnomishTransporter(uint32 i, Spell * pSpell)
{
    Player * plr = pSpell->p_caster;
    if(plr == 0) return true;

    plr->EventAttackStop();
    plr->SafeTeleport(1, 0, LocationVector(-7169.41f, -3838.63, 8.72f));
    return true;
}

bool NoggenFoggerElixr(uint32 i, Spell * pSpell)
{
    uint32 chance = sRand.randInt(3);
    switch(chance)
    {
    case 0:
        pSpell->u_caster->CastSpell(pSpell->u_caster, sSpellStore.LookupEntry(16591), true);
        break;

    case 1:
        pSpell->u_caster->CastSpell(pSpell->u_caster, sSpellStore.LookupEntry(16593), true);
        break;

    case 2:
        pSpell->u_caster->CastSpell(pSpell->u_caster, sSpellStore.LookupEntry(16595), true);
        break;
    }
    return true;
}

bool BanishExile(uint32 i, Spell * pSpell)
{
    Unit * target = pSpell->GetUnitTarget();
    if(target == 0) return true;

    pSpell->m_caster->SpellNonMeleeDamageLog(target, pSpell->m_spellInfo->Id, target->GetUInt32Value(UNIT_FIELD_HEALTH), true);
    return true;
}

bool QuestSpellHandler(uint32 i, Spell *pSpell)
{
    // actual spell handling code will be in here.
    Unit *unitTarget = pSpell->GetUnitTarget();
    Player *p_caster = pSpell->p_caster;
    if(!unitTarget || !p_caster || unitTarget->GetTypeId() != TYPEID_UNIT)
        return true;

    switch(pSpell->m_spellInfo->Id)
    {
    case 11536:
        {
            WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 12);
            data << uint32(6197) << unitTarget->GetGUID();
            p_caster->SendMessageToSet(&data, true);

            sQuestMgr.OnPlayerKill(p_caster, ((Creature*)unitTarget));  // this should increment in quest log
        }break;
    case 19938:
        {
            WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 12);
            data << uint32(6197) << unitTarget->GetGUID();
            p_caster->SendMessageToSet(&data, true);

            sQuestMgr.OnPlayerKill(p_caster, ((Creature*)unitTarget));  // this should increment in quest log

            // Send chat message
            char msg[100];
            sprintf(msg, "Ow! Ok, I'll get back to work, %s", p_caster->GetName());
            unitTarget->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, msg);
        }break;
    }

    return true;
}

bool HolyShock(uint32 i, Spell *pSpell)
{
    // actual spell handling code will be in here.
    Unit *unitTarget = pSpell->GetUnitTarget();
    if(!pSpell->p_caster || !unitTarget) return true;

    SpellEntry *spInfo = NULL;

    if (isAttackable(pSpell->p_caster,unitTarget))
    {
        uint32 spellid = 0;
        switch(pSpell->m_spellInfo->Id)
        {
        case 20473:
            spellid = 25912;
            break;
        case 20929:
            spellid = 25911;
            break;
        case 20930:
            spellid = 25902;
            break;
        case 27174:
            spellid = 27176;
            break;
        case 33072:
            spellid = 33073;
            break;
        }
        spInfo = sSpellStore.LookupEntry(spellid); // if its a enemy
    } else {
        uint32 spellid = 0;
        switch(pSpell->m_spellInfo->Id)
        {
        case 20473:
            spellid = 25914;
            break;
        case 20929:
            spellid = 25913;
            break;
        case 20930:
            spellid = 25903;
            break;
        case 27174:
            spellid = 27175;
            break;
        case 33072:
            spellid = 33074;
            break;
        }
        spInfo = sSpellStore.LookupEntry(spellid); // if its a ally
    }

    if(!spInfo)
        return true;

    pSpell->p_caster->CastSpell(unitTarget, spInfo, true);
    return true;
}


bool HallowsEndCandy(uint32 i, Spell * pSpell)
{
    // actual spell handling code will be in here.
    if(!pSpell->p_caster) return true;

    int chance = sRand.randInt(4);
    int spellId = 24924;
    
    SpellEntry *spInfo = sSpellStore.LookupEntry(spellId+chance);
    if(!spInfo) return true;

    pSpell->p_caster->CastSpell(pSpell->p_caster, spInfo, true);
    return true;
}

bool CookiedDeviateFish(uint32 i, Spell * pSpell)
{
    // actual spell handling code will be in here.
    Player *playerTarget = pSpell->GetPlayerTarget();
    //Unit *unitTarget = pSpell->GetUnitTarget();

    if(!playerTarget) return true;
    //(60 min) Yaaarrrr (turns you into a pirate)
    //(60 min) Flip Out (turns you into a ninja)
    //(10 sec) Oops (turns you into a pile of goo)
    //(2 min) Rapid Cast
    //(10 min) Cowardice (causes 5sec "Run Away" fear)
    //(insta) +50 Rage (Warriors Only)
    //(5 min) Fake Death
    //8221,8219,8223,8215,8224,8226

    int chance = 0;
    int spellId = 0;

    if(playerTarget->getClass() == WARRIOR)
        chance = sRand.randInt(7);
    else
        chance = sRand.randInt(6);

    switch(chance)
    {
    case 0:
        spellId = 8215;
        break;
    case 1:
        spellId = 8219;
        break;
    case 2:
        spellId = 8221;
        break;
    case 3:
        spellId = 8223;
        break;
    case 4:
        spellId = 8224;
        break;
    case 5:
        spellId = 8226;
        break;
    default:
        spellId = 0;
        break;
    };

    if(spellId)
    {
        SpellEntry *spInfo = sSpellStore.LookupEntry(spellId);
        if(!spInfo)
            return true;

        pSpell->p_caster->CastSpell(pSpell->p_caster, spInfo, true);
    }
    else
    {
        uint32 val=playerTarget->GetUInt32Value(UNIT_FIELD_POWER2)+500;
        playerTarget->SetUInt32Value(UNIT_FIELD_POWER2, val>=1000?1000:val);
    }
    return true;
}

bool NetOMatic(uint32 i, Spell * pSpell)
{
    Player *playerTarget = pSpell->GetPlayerTarget();
    //Unit *unitTarget = pSpell->GetUnitTarget();
    //FIXME: Captures the target in a net for 10 sec. The net has a lot of hooks however and sometimes gets caught in the user's clothing when fired......            
    //Successful Net target: ~50% could be a little less.

    //Net's you instead: ~25%-30%
    //basically roots you for 10 seconds

    //Net's you and target: ~25-30%
    //This is an interesting effect, but if you're chasing a runner it is just as good.  Basically what it does is shoots the net, runs you up to the target and roots both of you. 

    if(!playerTarget)
        return true;
    SpellCastTargets tgt,tgt2;
    int spell = (sRand.randInt(100))+1;
    uint64 t1 = 0;
    uint64 t2 = 0;

    if(spell < 50)
        t1 = playerTarget->GetSelection();
    else if(spell < 75)
        t2 = playerTarget->GetGUID();
    else
    {
        t1 = playerTarget->GetSelection();
        t2 = playerTarget->GetGUID();
    }

    SpellEntry *spInfo = sSpellStore.LookupEntry(13099);
    if(!spInfo)
        return true;

    if(t1)
    {
        Unit * un = pSpell->u_caster->GetMapMgr()->GetUnit(t1);
        if(un) pSpell->u_caster->CastSpell(un, spInfo, true);
    }
    
    if(t2)
    {
        Unit * un = pSpell->u_caster->GetMapMgr()->GetUnit(t2);
        if(un) pSpell->u_caster->CastSpell(un, spInfo, true);
    }
    return true;
}

bool HolidayCheer(uint32 i, Spell * pSpell)
{
    Unit * pUnit;
    float dist = pSpell->GetRadius(i);
    for(ObjectSet::iterator itr = pSpell->m_caster->GetInRangeSetBegin();
        itr != pSpell->m_caster->GetInRangeSetEnd(); ++itr)
    {
        if((*itr)->IsUnit())
            pUnit = (Unit*)*itr;
        else
            continue;

        if(pSpell->m_caster->CalcDistance(pUnit) > dist || isAttackable(pSpell->m_caster, pUnit))
            continue;

        pUnit->Emote(EMOTE_ONESHOT_LAUGH);
    }
    return true;
}

bool DeviateFish(uint32 i, Spell * pSpell)
{
    int spell = sRand.randInt(5) + 1;

    SpellEntry *spInfo = sSpellStore.LookupEntry((pSpell->m_spellInfo->Id+spell));
    if(!spInfo)
        return true;

    pSpell->u_caster->CastSpell(pSpell->u_caster, spInfo, true);
    return true;
}

void SetupItemSpells_1(ScriptMgr * mgr)
{
    mgr->register_dummy_spell(20473, &HolyShock);       // Holy Shock rank 1
    mgr->register_dummy_spell(20939, &HolyShock);       // Holy Shock rank 2
    mgr->register_dummy_spell(20930, &HolyShock);       // Holy Shock rank 3
    mgr->register_dummy_spell(27174, &HolyShock);       // Holy Shock rank 4
    mgr->register_dummy_spell(33072, &HolyShock);       // Holy Shock rank 5
    mgr->register_dummy_spell(23453, &GnomishTransporter);       // Gnomish Transporter
    mgr->register_dummy_spell(16589, &NoggenFoggerElixr);       // Noggenfogger
    mgr->register_dummy_spell(4130, &BanishExile);        // Banish Exile
    mgr->register_dummy_spell(4131, &BanishExile);        // Banish Exile
    mgr->register_dummy_spell(4132, &BanishExile);        // Banish Exile
    mgr->register_dummy_spell(11536, &QuestSpellHandler);       // Quest spell
    mgr->register_dummy_spell(19938, &QuestSpellHandler);       // Quest spell
    mgr->register_dummy_spell(24430, &HallowsEndCandy);       // Hallows End Candy
    mgr->register_dummy_spell(8213, &CookiedDeviateFish);        // Cooked Deviate Fish
    mgr->register_dummy_spell(26074, &HolidayCheer);       // Holiday Cheer
    mgr->register_dummy_spell(8063, &DeviateFish);        // Deviate Fish
    mgr->register_dummy_spell(13120, &NetOMatic);       // Net-o-Matic
}
