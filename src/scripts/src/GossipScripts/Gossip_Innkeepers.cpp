#include "StdAfx.h"
#include "Setup.h"

class SCRIPT_DECL InnkeeperGossip : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(InnkeeperGossip);
    void GossipHello(Creature* pCreature, Player* Plr, bool AutoSend);
    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId);
    void GossipEnd(Creature* pCreature, Player* Plr);
};

void InnkeeperGossip::GossipHello(Creature* pCreature, Player* Plr, bool AutoSend)
{
    sLog.outDetail("Gossip-Innkeepers.cpp : Hello by %s", Plr->GetName());
    GossipMenu *Menu;
    uint32 TextID = 820;
    // Hello (First) Menu.
    objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), TextID, Plr);

    if( pCreature->GetUInt32Value(UNIT_NPC_FLAGS) & UNIT_NPC_FLAG_VENDOR )
        Menu->AddItem( 1, "I would like to browse your goods.", 1 );
    
    Menu->AddItem( 5, "Make this inn your home.", 2 );
    Menu->AddItem( 0, "What can I do at an inn?", 3 );
    if(AutoSend)
        Menu->SendTo(Plr);
}

#define SendQuickMenu(textid) objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), textid, Plr); \
    Menu->SendTo(Plr);

void InnkeeperGossip::GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
{
    sLog.outDebug("Gossip-Innkeepers.cpp : Select option %u by %s", IntId, Plr->GetName());
    switch(IntId)
    {
    case 1:     // VENDOR
        Plr->GetSession()->SendInventoryList(pCreature);
        break;
    case 2:     // BINDER
        Plr->GetSession()->SendInnkeeperBind(pCreature);
        break;
    case 3:     // WHAT CAN I DO ?
        // Prepare second menu
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 1853, Plr);
        Menu->AddItem(5, "Make this inn your home.", 2);
        Menu->SendTo(Plr);
        break;
    }
}

void InnkeeperGossip::GossipEnd(Creature* pCreature, Player* Plr)
{
    GossipScript::GossipEnd(pCreature, Plr);
}

void SetupInnkeepers(ScriptMgr * mgr)
{
    /* Innkeeper List */
    mgr->register_gossip_script(15174, &InnkeeperGossip::Create);      //Calandrath
    mgr->register_gossip_script(18251, &InnkeeperGossip::Create);      //Caregiver Abidaar
    mgr->register_gossip_script(16739, &InnkeeperGossip::Create);      //Caregiver Breel
    mgr->register_gossip_script(16553, &InnkeeperGossip::Create);      //Caregiver Chellan
    mgr->register_gossip_script(18914, &InnkeeperGossip::Create);      //Caregiver Isel
    mgr->register_gossip_script(18906, &InnkeeperGossip::Create);      //Caregiver Ophera Windfury
    mgr->register_gossip_script(17553, &InnkeeperGossip::Create);      //Caregiver Topher Loaal
    mgr->register_gossip_script(21746, &InnkeeperGossip::Create);      //Caretaker Aluuro
    mgr->register_gossip_script(19352, &InnkeeperGossip::Create);      //Dreg Cloudsweeper
    mgr->register_gossip_script(19531, &InnkeeperGossip::Create);      //Eyonix
    mgr->register_gossip_script(21110, &InnkeeperGossip::Create);      //Fizit "Doc" Clocktock
    mgr->register_gossip_script(16602, &InnkeeperGossip::Create);      //Floyd Pinkus
    mgr->register_gossip_script(19470, &InnkeeperGossip::Create);      //Gholah
    mgr->register_gossip_script(23143, &InnkeeperGossip::Create);      //Horus
    mgr->register_gossip_script(11116, &InnkeeperGossip::Create);      //Innkeeper Abeqwa
    mgr->register_gossip_script(9501, &InnkeeperGossip::Create);      //Innkeeper Adegwa
    mgr->register_gossip_script(22922, &InnkeeperGossip::Create);      //Innkeeper Aelerya
    mgr->register_gossip_script(6740, &InnkeeperGossip::Create);      //Innkeeper Allison
    mgr->register_gossip_script(2352, &InnkeeperGossip::Create);      //Innkeeper Anderson
    mgr->register_gossip_script(6739, &InnkeeperGossip::Create);      //Innkeeper Bates
    mgr->register_gossip_script(18905, &InnkeeperGossip::Create);      //Innkeeper Bazil Olof'tazun
    mgr->register_gossip_script(1247, &InnkeeperGossip::Create);      //Innkeeper Belm
    mgr->register_gossip_script(19296, &InnkeeperGossip::Create);      //Innkeeper Biribi
    mgr->register_gossip_script(3934, &InnkeeperGossip::Create);      //Innkeeper Boorand Plainswind
    mgr->register_gossip_script(6727, &InnkeeperGossip::Create);      //Innkeeper Brianna
    mgr->register_gossip_script(7714, &InnkeeperGossip::Create);      //Innkeeper Byula
    mgr->register_gossip_script(18907, &InnkeeperGossip::Create);      //Innkeeper Coryth Stoktron
    mgr->register_gossip_script(19319, &InnkeeperGossip::Create);      //Innkeeper Darg Bloodclaw
    mgr->register_gossip_script(15433, &InnkeeperGossip::Create);      //Innkeeper Delaniel
    mgr->register_gossip_script(16458, &InnkeeperGossip::Create);      //Innkeeper Faralia
    mgr->register_gossip_script(295, &InnkeeperGossip::Create);      //Innkeeper Farley
    mgr->register_gossip_script(5111, &InnkeeperGossip::Create);      //Innkeeper Firebrew
    mgr->register_gossip_script(7733, &InnkeeperGossip::Create);      //Innkeeper Fizzgrimble
    mgr->register_gossip_script(7737, &InnkeeperGossip::Create);      //Innkeeper Greul
    mgr->register_gossip_script(18957, &InnkeeperGossip::Create);      //Innkeeper Grilka
    mgr->register_gossip_script(6928, &InnkeeperGossip::Create);      //Innkeeper Grosk
    mgr->register_gossip_script(6929, &InnkeeperGossip::Create);      //Innkeeper Gryshka
    mgr->register_gossip_script(19232, &InnkeeperGossip::Create);      //Innkeeper Haelthol
    mgr->register_gossip_script(6734, &InnkeeperGossip::Create);      //Innkeeper Hearthstove
    mgr->register_gossip_script(8931, &InnkeeperGossip::Create);      //Innkeeper Heather
    mgr->register_gossip_script(1464, &InnkeeperGossip::Create);      //Innkeeper Helbrek
    mgr->register_gossip_script(6272, &InnkeeperGossip::Create);      //Innkeeper Janene
    mgr->register_gossip_script(7731, &InnkeeperGossip::Create);      //Innkeeper Jayka
    mgr->register_gossip_script(17630, &InnkeeperGossip::Create);      //Innkeeper Jovia
    mgr->register_gossip_script(16542, &InnkeeperGossip::Create);      //Innkeeper Kalarin
    mgr->register_gossip_script(6930, &InnkeeperGossip::Create);      //Innkeeper Karakul
    mgr->register_gossip_script(6747, &InnkeeperGossip::Create);      //Innkeeper Kauth
    mgr->register_gossip_script(12196, &InnkeeperGossip::Create);      //Innkeeper Kaylisk
    mgr->register_gossip_script(6736, &InnkeeperGossip::Create);      //Innkeeper Keldamyr
    mgr->register_gossip_script(18908, &InnkeeperGossip::Create);      //Innkeeper Kerp
    mgr->register_gossip_script(6738, &InnkeeperGossip::Create);      //Innkeeper Kimlya
    mgr->register_gossip_script(11103, &InnkeeperGossip::Create);      //Innkeeper Lyshaerya
    mgr->register_gossip_script(6741, &InnkeeperGossip::Create);      //Innkeeper Norman
    mgr->register_gossip_script(6746, &InnkeeperGossip::Create);      //Innkeeper Pala
    mgr->register_gossip_script(19571, &InnkeeperGossip::Create);      //Innkeeper Remi Dodoso
    mgr->register_gossip_script(5688, &InnkeeperGossip::Create);      //Innkeeper Renee
    mgr->register_gossip_script(6735, &InnkeeperGossip::Create);      //Innkeeper Saelienne
    mgr->register_gossip_script(19495, &InnkeeperGossip::Create);      //Innkeeper Shaunessy
    mgr->register_gossip_script(6737, &InnkeeperGossip::Create);      //Innkeeper Shaussiy
    mgr->register_gossip_script(2388, &InnkeeperGossip::Create);      //Innkeeper Shay
    mgr->register_gossip_script(9356, &InnkeeperGossip::Create);      //Innkeeper Shul'kar
    mgr->register_gossip_script(7736, &InnkeeperGossip::Create);      //Innkeeper Shyria
    mgr->register_gossip_script(11106, &InnkeeperGossip::Create);      //Innkeeper Sikewa
    mgr->register_gossip_script(6807, &InnkeeperGossip::Create);      //Innkeeper Skindle
    mgr->register_gossip_script(5814, &InnkeeperGossip::Create);      //Innkeeper Thulbek
    mgr->register_gossip_script(7744, &InnkeeperGossip::Create);      //Innkeeper Thulfram
    mgr->register_gossip_script(6790, &InnkeeperGossip::Create);      //Innkeeper Trelayne
    mgr->register_gossip_script(16618, &InnkeeperGossip::Create);      //Innkeeper Velandra
    mgr->register_gossip_script(11118, &InnkeeperGossip::Create);      //Innkeeper Vizzie
    mgr->register_gossip_script(6791, &InnkeeperGossip::Create);      //Innkeeper Wiley
    mgr->register_gossip_script(16256, &InnkeeperGossip::Create);      //Jessica Chambers
    mgr->register_gossip_script(14731, &InnkeeperGossip::Create);      //Lard
    mgr->register_gossip_script(15397, &InnkeeperGossip::Create);      //Marniel Amberlight
    mgr->register_gossip_script(18913, &InnkeeperGossip::Create);      //Matron Tikkit
    mgr->register_gossip_script(21088, &InnkeeperGossip::Create);      //Matron Varah
    mgr->register_gossip_script(6778, &InnkeeperGossip::Create);      //Melika Isenstrider
    mgr->register_gossip_script(18245, &InnkeeperGossip::Create);      //Merajit
    mgr->register_gossip_script(19046, &InnkeeperGossip::Create);      //Minalei
    mgr->register_gossip_script(21744, &InnkeeperGossip::Create);      //Roldemar
    mgr->register_gossip_script(16826, &InnkeeperGossip::Create);      //Sid Limbardi
    mgr->register_gossip_script(6806, &InnkeeperGossip::Create);      //Tannok Frosthammer

    //cleanup:
    //added 36 new Innkeeper's ,81 working innkeeper's now :P
    //removed Innkeeper Monica(she dos not have gossip option she is from Old Hillsbrad Foothills)
}
