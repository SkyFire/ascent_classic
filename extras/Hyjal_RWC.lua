//=====================================================================
//I                  Rage Winterchill challenge                       I
//=====================================================================

//=========================== Config ==================================
global RWC_RWC_ICEBOLT_TIMER = 10000
global RWC_RWC_FROSTNOVA_TIMER = 15000
global RWC_RWC_ICEARMOR_TIMER = 25000
global RWC_RWC_DEATHANDDECAY_TIMER = 13000
global RWC_ABOMINATION_POISONCLOUD_TIMER = 30000
global RWC_NECROMANCER_UNHOLYFRENZY_TIMER = 20000
global RWC_NECROMANCER_CRIPPLE_TIMER = 30000
global RWC_NECROMANCER_SUMMONSKELETONS_TIMER = 45000
global RWC_NECROMANCER_SHADOWBOLT_TIMER = 12500
global RWC_CRYPTFIEND_WEB_TIMER = 10000




//=========================== Implementation ==========================
//------------------THE BOSS---------------
global Jaina = null
global DeathAndDecayAllowed = 1

function RWC_RWC(unit)
    print "MH:RWC_RWC init"
    unit:MoveTo(X1,Y1,Z1,O1)
end

function RWC_RWC_Combat(unit)
    print "MH:RWC combat"
    unit:RegisterEvent("RWC_RWC_Icebolt",RWC_RWC_ICEBOLT_TIMER,0)
    unit:RegisterEvent("RWC_RWC_FrostNova",RWC_RWC_FROSTNOVA_TIMER,0)
    unit:RegisterEvent("RWC_RWC_IceArmor",RWC_RWC_ICEARMOR_TIMER,0)
    unit:RegisterEvent("RWC_RWC_DeathAndDecay",RWC_RWC_ICEARMOR_TIMER,0)
end

function RWC_RWC_Icebolt(unit)
    print "MH:RWC icebolt"
    local plr = unit:GetRandomPlayer()
    if (plr!=null)
        unit:CastSpellOnTarget(31249,plr)
end


function RWC_RWC_FrostNova(unit)
    print "MH:RWC forstnova"
    unit:CastSpell(32365)
    DeathAndDecayAllowed = 0
    unit:RegisterEvent("RWC_Allow_DeathaAndDecay",6000,1)
end

function RWC_Allow_DeathaAndDecay(unit)
    DeathAndDecayAllowed = 0
end

function RWC_RWC_IceArmor(unit)
    print "MH:RWC icearmor"
    unit:CastSpell(31256)
end

function RWC_RWC_DeathAndDecay(unit)
    print "MH:RWC death&decay"
    if (RWCNovaCooldown==0)
         unit:CastSpell(34642)
end

//------------------Adds-------------------
function RWC_Ghoul(unit)
    print "MH:RWC_A init"
    unit:MoveTo(X1,Y1,Z1,O1)
end

function RWC_Abomination(unit)
    print "MH:RWC_A init"
    unit:MoveTo(X1,Y1,Z1,O1)
end

function RWC_Abomination_Combat(unit)
    print "MH:RWC_A combat"
    unit:RegisterEvent("RWC_Abomination_PoisonCloud",RWC_ABOMINATION_POISONCOUD_TIMER,0)
end

function RWC_Abomination_PoisonCloud(unit)
    print "MH:RWC_A poison cloud"
    unit:FullCastSpell(30914)
end

function RWC_Necromancer(unit)
    print "MH:RWC_N init"
    unit:MoveTo(X1,Y1,Z1,O1)
end

function RWC_Necromancer_Combat(unit)
    print "MH:RWC_N combat"
    unit:RegisterEvent("RWC_Necromancer_UnholyFrenzy",RWC_NECROMANCER_UNHOLYFRENZY_TIMER,0)
    unit:RegisterEvent("RWC_Necromancer_SummonSkeletons",RWC_NECROMANCER_SUMMONSKELETONS_TIMER,0)
    unit:RegisterEvent("RWC_Necromancer_Cripple",RWC_NECROMANCER_CRIPPLE_TIMER,0)
    unit:RegisterEvent("RWC_Necromancer_ShadowBolt",RWC_NECROMANCER_SHADOWBOLT_TIMER,0)
end

function RWC_Necromancer_UnholyFrenzy(unit)
    print "MH:RWC_N unholy frenzy"
    local plr = unit:GetRandomAlly()
	if (plr != null)
	    unit:FullCastSpellOnTarget(31626,plr)
end    

function RWC_Necromancer_SummonSkeletons(unit)
    print "MH:RWC_N summonskeletons"
    unit:FullCastSpell(31617)
end 

function RWC_Necromancer_Cripple(unit)
    print "MH:RWC_N unholy frenzy"
    local plr = unit:GetClosestPlayer();
    if (plr != null)
       unit:FullCastSpellOnTarget(33787,plr)
end   

function RWC_Necromancer_ShadowBolt(unit)
    print "MH:RWC_N shadowbolt"
    unit:FullCastSpell(29487)
end    

function RWC_CryptFiend(unit)
    print "MH:RWC_CF init"
    unit:MoveTo(X1,Y1,Z1,O1)
end

function RWC_CryptFiend_Combat(unit)
    print "MH:RWC_CF combat"
    unit:RegisterEvent("RWC_CryptFiend_Web",RWC_CRYPTFIEND_WEB_TIMER,0)
end 

function RWC_CryptFiend_Web(unit)
    print "MH:RWC_CF web"
    local plr = unit:GetRandomPlayer()
    if (plr!=null)
        unit:FullCastSpellOnTarget(745,plr)
end 

//-------Weaves

function RWC_Wave1()
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:RegisterEvent("RWC_Wave2",125000,1)
end

function RWC_Wave2()
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:RegisterEvent("RWC_Wave3",125000,1)
end

function RWC_Wave3()
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:RegisterEvent("RWC_Wave4",125000,1)
end

function RWC_Wave4()
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:RegisterEvent("RWC_Wave5",125000,1)
end

function RWC_Wave5()
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:RegisterEvent("RWC_Wave6",125000,1)
end

function RWC_Wave6()
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:RegisterEvent("RWC_Wave7",125000,1)
end

function RWC_Wave7()
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:RegisterEvent("RWC_Wave8",125000,1)
end

function RWC_Wave8()
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17895,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17897,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17899,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:SpawnCreature(17898,X,Y,Z,O,FACTION,DURATION)
 Jaina:RegisterEvent("RWC_Boss",200000,1)
end

function RWC_Boss()
 Jaina:SpawnCreature(17767,X,Y,Z,O,FACTION,DURATION)
end
  
function RWC_Start(unit)
 Jaina = unit
 RegisterUnitEvent(17895,6,"RWC_Ghoul")
 RegisterUnitEvent(17898,6,"RWC_Abomination")
 RegisterUnitEvent(17898,1,"RWC_Abomination_Combat")
 RegisterUnitEvent(17897,6,"RWC_CryptFiend")
 RegisterUnitEvent(17897,1,"RWC_CryptFiend_Combat")
 RegisterUnitEvent(17899,6,"RWC_Necromancer")
 RegisterUnitEvent(17899,1,"RWC_Necromancer_Combat")
 RegisterUnitEvent(17767,6,"RWC_RWC")
 RegisterUnitEvent(17767,1,"RWC_RWC_Combat")
 Jaina:RegisterEvent("RWC_Wave1",100,1)
end

RegisterUnitEvent(17772,10,"RWC_Start")