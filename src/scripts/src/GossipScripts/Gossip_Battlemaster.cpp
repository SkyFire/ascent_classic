#include "StdAfx.h"
#include "Setup.h"

class SCRIPT_DECL WarsongGulchBattlemaster : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(WarsongGulchBattlemaster);
    void GossipHello(Creature * pCreature, Player * plr, bool AutoSend)
    {
        sLog.outDetail("Gossip-Battlemaster.cpp : WSG Hello by %s", plr->GetName());

        GossipMenu *Menu;
        uint32 Team = plr->GetTeam();
        if(Team > 1) Team = 1;
        
        // Check if the player can be entered into the bg or not.
        if(plr->getLevel() < 10)
        {
            uint32 FactMessages[2] = { 7599, 7688 };

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);

            if(Rand(30.0f))    // 30% chance to send message
            {
                char message[200];
                sprintf(message, "Sorry, %s, you are not skilled enough to enter the hazards of Warsong Gulch. Come back when you have trained some more.", plr->GetName());
                pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message);
            }
        }
        else
        {
            uint32 FactMessages[2] = { 7689, 7705 }; // need to find the second one

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);
            Menu->AddItem( 0, "I would like to enter the battleground.", 1);
        }
        
        if(AutoSend)
            Menu->SendTo(plr);
    }

    void GossipSelectOption(Creature * pCreature, Player * plr, uint32 Id, uint32 IntId)
    {
        // Check the player is capable.
        if(plr->getLevel() < 10)
        {
            // We shouldn't actually get here.. it means they've cheated with packets or
            // are using a bot :p
            pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You cannot enter the battleground." );
        }

        // Send battleground list.
        plr->GetSession()->SendBattlegroundList(pCreature, 2);  // WSG = 2
    }

    void Destroy()
    {
        delete this;
    }
};

class SCRIPT_DECL ArathiBasinBattlemaster : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(ArathiBasinBattlemaster);

    void GossipHello(Creature * pCreature, Player * plr, bool AutoSend)
    {
        sLog.outDetail("Gossip-Battlemaster.cpp : AB Hello by %s", plr->GetName());

        GossipMenu *Menu;
        uint32 Team = plr->GetTeam();
        if(Team > 1) Team = 1;

        // Check if the player can be entered into the bg or not.
        if(plr->getLevel() < 20)
        {
            uint32 FactMessages[2] = { 7700, 7667 };

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);

            if(Rand(30.0f))    // 30% chance to send message
            {
                char message[200];
                sprintf(message, "You cannot help our struggle yet, %s. Once you are stronger come back and join the fight!", plr->GetName());
                pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message);
            }
        }
        else
        {
            uint32 FactMessages[2] = { 7700, 7555 }; // need to find the second one

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);
            Menu->AddItem( 0, "I would like to enter the battleground.", 1);
        }

        if(AutoSend)
            Menu->SendTo(plr);
    }

    void GossipSelectOption(Creature * pCreature, Player * plr, uint32 Id, uint32 IntId)
    {
        // Check the player is capable.
        if(plr->getLevel() < 20)
        {
            // We shouldn't actually get here.. it means they've cheated with packets or
            // are using a bot :p
            pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You cannot enter the battleground." );
        }

        // Send battleground list.
        plr->GetSession()->SendBattlegroundList(pCreature, 3);  // WSG = 3
    }

    void Destroy()
    {
        delete this;
    }
};

class SCRIPT_DECL AlteracValleyBattlemaster : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(AlteracValleyBattlemaster);
    void GossipHello(Creature * pCreature, Player * plr, bool AutoSend)
    {
        sLog.outDetail("Gossip-Battlemaster.cpp : AV Hello by %s", plr->GetName());

        GossipMenu *Menu;
        uint32 Team = plr->GetTeam();
        if(Team > 1) Team = 1;

        // Check if the player can be entered into the bg or not.
        if(plr->getLevel() < 60)
        {
            uint32 FactMessages[2] = { 7658, 7658 };

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);

            if(Rand(30.0f))    // 30% chance to send message
            {
                char message[200];
                sprintf(message, "Only our best warriors can enter Alterac Valley, %s. Come back when you are more experienced.", plr->GetName());
                pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message);
            }
        }
        else
        {
            uint32 FactMessages[2] = { 7658, 7659 }; // need to find the second one

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);
            Menu->AddItem( 0, "I would like to enter the battleground.", 1);
        }

        if(AutoSend)
            Menu->SendTo(plr);
    }

    void GossipSelectOption(Creature * pCreature, Player * plr, uint32 Id, uint32 IntId)
    {
        // Check the player is capable.
        if(plr->getLevel() < 60)
        {
            // We shouldn't actually get here.. it means they've cheated with packets or
            // are using a bot :p
            pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You cannot enter the battleground." );
        }

        // Send battleground list.
        plr->GetSession()->SendBattlegroundList(pCreature, 1);  // AV = 2
    }

    void Destroy()
    {
        delete this;
    }
};

void SetupBattlemaster(ScriptMgr * mgr)
{
    /* Battlemaster List */
    mgr->register_gossip_script(19910, &WarsongGulchBattlemaster::Create); // Gargok
    mgr->register_gossip_script(15105, &WarsongGulchBattlemaster::Create); // Warsong Emissary
    mgr->register_gossip_script(20118, &WarsongGulchBattlemaster::Create); // Jihi
    mgr->register_gossip_script(16696, &WarsongGulchBattlemaster::Create); // Krukk
    mgr->register_gossip_script(2804, &WarsongGulchBattlemaster::Create);  // Kurden Bloodclaw
    mgr->register_gossip_script(20272, &WarsongGulchBattlemaster::Create); // Lylandor
    mgr->register_gossip_script(20269, &WarsongGulchBattlemaster::Create); // Montok Redhands
    mgr->register_gossip_script(19908, &WarsongGulchBattlemaster::Create); // Su'ura Swiftarrow
    mgr->register_gossip_script(15102, &WarsongGulchBattlemaster::Create); // Silverwing Emissary
    mgr->register_gossip_script(14981, &WarsongGulchBattlemaster::Create); // Elfarran
    mgr->register_gossip_script(14982, &WarsongGulchBattlemaster::Create); // Lylandris
    mgr->register_gossip_script(2302, &WarsongGulchBattlemaster::Create);  // Aethalas
    mgr->register_gossip_script(10360, &WarsongGulchBattlemaster::Create); // Kergul Bloodaxe
    mgr->register_gossip_script(3890, &WarsongGulchBattlemaster::Create);  // Brakgul Deathbringer
    mgr->register_gossip_script(20273, &ArathiBasinBattlemaster::Create); // Adam Eternum
    mgr->register_gossip_script(16694, &ArathiBasinBattlemaster::Create); // Karen Wentworth
    mgr->register_gossip_script(20274, &ArathiBasinBattlemaster::Create); // Keldor the Lost
    mgr->register_gossip_script(15007, &ArathiBasinBattlemaster::Create); // Sir Malory Wheeler
    mgr->register_gossip_script(19855, &ArathiBasinBattlemaster::Create); // Sir Maximus Adams
    mgr->register_gossip_script(19905, &ArathiBasinBattlemaster::Create); // The Black Bride
    mgr->register_gossip_script(20120, &ArathiBasinBattlemaster::Create); // Tolo
    mgr->register_gossip_script(15008, &ArathiBasinBattlemaster::Create); // Lady Hoteshem
    mgr->register_gossip_script(857, &ArathiBasinBattlemaster::Create);   // Donald Osgood
    mgr->register_gossip_script(907, &ArathiBasinBattlemaster::Create);   // Keras Wolfheart
    mgr->register_gossip_script(12198, &ArathiBasinBattlemaster::Create); // Martin Lindsev
    mgr->register_gossip_script(14990, &ArathiBasinBattlemaster::Create); // Defilers Emissary
    mgr->register_gossip_script(15006, &ArathiBasinBattlemaster::Create); // Deze Snowbane
    mgr->register_gossip_script(14991, &ArathiBasinBattlemaster::Create); // League of Arathor Emissary
    mgr->register_gossip_script(347, &AlteracValleyBattlemaster::Create);   // Grizzle Halfmane
    mgr->register_gossip_script(19907, &AlteracValleyBattlemaster::Create); // Grumbol Grimhammer
    mgr->register_gossip_script(16695, &AlteracValleyBattlemaster::Create); // Gurak
    mgr->register_gossip_script(20271, &AlteracValleyBattlemaster::Create); // Haelga Slatefist
    mgr->register_gossip_script(20119, &AlteracValleyBattlemaster::Create); // Mahul
    mgr->register_gossip_script(19906, &AlteracValleyBattlemaster::Create); // Usha Eyegouge
    mgr->register_gossip_script(20276, &AlteracValleyBattlemaster::Create); // Wolf-Sister Maka
    mgr->register_gossip_script(7410, &AlteracValleyBattlemaster::Create);  // Thelman Slatefist
    mgr->register_gossip_script(12197, &AlteracValleyBattlemaster::Create); // Glordrum Steelbeard
    mgr->register_gossip_script(5118, &AlteracValleyBattlemaster::Create);  // Brogun Stoneshield
    mgr->register_gossip_script(15106, &AlteracValleyBattlemaster::Create); // Frostwolf Emissary
    mgr->register_gossip_script(15103, &AlteracValleyBattlemaster::Create); // Stormpike Emissary
    mgr->register_gossip_script(14942, &AlteracValleyBattlemaster::Create); // Kartra Bloodsnarl

   //cleanup:
   //removed Sandfury Soul Eater(hes a npc in Zul'Farrak and has noting to do whit the battleground masters) 
   //added Warsong Emissary, Stormpike Emissary , League of Arathor Emissary
}
