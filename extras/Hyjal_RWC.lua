RWC_RWC_ICEBOLT_TIMER = 9000
RWC_RWC_FROSTNOVA_TIMER = 15000
RWC_RWC_ICEARMOR_TIMER = 23000
RWC_RWC_DEATHANDDECAY_TIMER = 11000
RWC_ABOMINATION_POISONCLOUD_TIMER = 20000
RWC_NECROMANCER_UNHOLYFRENZY_TIMER = 8000
RWC_NECROMANCER_CRIPPLE_TIMER = 7000
RWC_NECROMANCER_SUMMONSKELETONS_TIMER =20000
RWC_NECROMANCER_SHADOWBOLT_TIMER = 5000
RWC_CRYPTFIEND_WEB_TIMER = 13500

Jaina = null
RWC_IsInProgress = 0
RWC_DeathAndDecayAllowed = 1

function RWC_RWC(unit)
    unit:RegisterEvent("RWC_Waves_Move",10000,1)
end

function RWC_RWC_Combat(unit)
    unit:RegisterEvent("RWC_RWC_Icebolt",RWC_RWC_ICEBOLT_TIMER,0)
    unit:RegisterEvent("RWC_RWC_FrostNova",RWC_RWC_FROSTNOVA_TIMER,0)
    unit:RegisterEvent("RWC_RWC_IceArmor",RWC_RWC_ICEARMOR_TIMER,0)
    unit:RegisterEvent("RWC_RWC_DeathAndDecay",RWC_RWC_DEATHANDDECAY_TIMER ,0)
end

function RWC_RWC_Icebolt(unit)
    local plr = unit:GetRandomPlayer(0)
    if (plr ~= nil) then
        unit:FullCastSpellOnTarget(31249,plr)
    end
end


function RWC_RWC_FrostNova(unit)
    unit:FullCastSpell(32365)
    RWC_DeathAndDecayAllowed = 0
    unit:RegisterEvent("RWC_Allow_DeathAndDecay",6000,1)
end

function RWC_Allow_DeathAndDecay(unit)
    RWC_DeathAndDecayAllowed = 1
end

function RWC_RWC_IceArmor(unit)
    unit:FullCastSpell(31256)
end

function RWC_RWC_DeathAndDecay(unit)
    if (RWC_DeathAndDecayAllowed>0) then
         local plr = unit:GetRandomPlayer(0)
         if (plr ~= nil) then
             unit:FullCastSpellOnTarget(34642,plr)
         end
    end
end

function RWC_Ghoul(unit)
    unit:RegisterEvent("RWC_Waves_Move",10000,1)
end

function RWC_Abomination(unit)
    unit:RegisterEvent("RWC_Waves_Move",10000,1)
end

function RWC_Waves_Move(unit)
    unit:CreateWaypoint(5042.476074,-1786.176147,1321.710205,0,0,0,0)
    unit:MoveToWaypoint(0)
end
function RWC_Abomination_Combat(unit)
    unit:RegisterEvent("RWC_Abomination_PoisonCloud",RWC_ABOMINATION_POISONCLOUD_TIMER,0)
end

function RWC_Abomination_PoisonCloud(unit)
    unit:FullCastSpell(30914)
end

function RWC_Necromancer(unit)
    unit:RegisterEvent("RWC_Waves_Move",10000,1)
end

function RWC_Necromancer_Combat(unit)
    unit:RegisterEvent("RWC_Necromancer_UnholyFrenzy",RWC_NECROMANCER_UNHOLYFRENZY_TIMER,0)
    unit:RegisterEvent("RWC_Necromancer_SummonSkeletons",RWC_NECROMANCER_SUMMONSKELETONS_TIMER,0)
    unit:RegisterEvent("RWC_Necromancer_Cripple",RWC_NECROMANCER_CRIPPLE_TIMER,0)
    unit:RegisterEvent("RWC_Necromancer_ShadowBolt",RWC_NECROMANCER_SHADOWBOLT_TIMER,0)
end

function RWC_Necromancer_UnholyFrenzy(unit)
    local plr = unit:GetRandomFriend()
    if (plr ~= nil) then
        unit:FullCastSpellOnTarget(31626,plr)
    end

end    

function RWC_Necromancer_SummonSkeletons(unit)
    unit:FullCastSpell(31617)
end 

function RWC_Necromancer_Cripple(unit)
    local plr = unit:GetClosestPlayer();
    if (plr ~= nil) then
        unit:FullCastSpellOnTarget(33787,plr)
    end
end   

function RWC_Necromancer_ShadowBolt(unit)
    local plr = unit:GetClosestPlayer();
    if (plr ~= nil) then
        unit:FullCastSpellOnTarget(29487,plr)
    end

end    

function RWC_CryptFiend(unit)
    unit:RegisterEvent("RWC_Waves_Move",10000,1)
end

function RWC_CryptFiend_Combat(unit)
    unit:RegisterEvent("RWC_CryptFiend_Web",RWC_CRYPTFIEND_WEB_TIMER,0)
end 

function RWC_CryptFiend_Web(unit)
    local plr = unit:GetRandomPlayer(0)
    if (plr ~= nil) then
        unit:FullCastSpellOnTarget(745,plr)
    end
end 

function RWC_Wave1()
 print "RWC_Wave1"
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:RegisterEvent("RWC_Wave2",125000,1)
end

function RWC_Wave2()
 print "RWC_Wave2"
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:RegisterEvent("RWC_Wave3",125000,1)
end

function RWC_Wave3()
 print "RWC_Wave3"
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:RegisterEvent("RWC_Wave4",125000,1)
end

function RWC_Wave4()
 print "RWC_Wave4"
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:RegisterEvent("RWC_Wave5",125000,1)
end

function RWC_Wave5()
 print "RWC_Wave5"
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:RegisterEvent("RWC_Wave6",125000,1)
end

function RWC_Wave6()
 print "RWC_Wave6"
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:RegisterEvent("RWC_Wave7",125000,1)
end

function RWC_Wave7()
 print "RWC_Wave7"
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:RegisterEvent("RWC_Wave8",125000,1)
end

function RWC_Wave8()
 print "RWC_Wave8"
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17895,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17897,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17899,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:SpawnCreature(17898,4955.778320,-1681.963013,1343.135986,0,1720,0)
 Jaina:RegisterEvent("RWC_Boss",200000,1)
end

function RWC_Boss()
 print "RWC_Boss"
 Jaina:SpawnCreature(17767,4955.778320,-1681.963013,1343.135986,0,1720,0)
end
  
function RWC_Start(unit)
 Jaina = unit
 if (RWC_IsInProgress == 0) then
     print "MH:RWC Start"
     local plr = Jaina:GetClosestPlayer()
     if (plr~=nil) then
	RWC_IsInProgress = plr:GetInstanceID()
     end
     Jaina:RegisterEvent("RWC_Wave1",100,1)
 end
end

function RWC_Failed(unit)
 print "MH:RWC Failed"
 Jaina:RemoveEvents()
 Jaina:SendChatMessage(12,0,"Event Failed. Try once again and dont let me die")
 Jaina:Despawn(0,5000)
 RWC_IsInProgress = 0
end

RegisterUnitEvent(17895,6,"RWC_Ghoul")
RegisterUnitEvent(17898,6,"RWC_Abomination")
RegisterUnitEvent(17898,1,"RWC_Abomination_Combat")
RegisterUnitEvent(17897,6,"RWC_CryptFiend")
RegisterUnitEvent(17897,1,"RWC_CryptFiend_Combat")
RegisterUnitEvent(17899,6,"RWC_Necromancer")
RegisterUnitEvent(17899,1,"RWC_Necromancer_Combat")
RegisterUnitEvent(17767,6,"RWC_RWC")
RegisterUnitEvent(17767,1,"RWC_RWC_Combat")
RegisterUnitEvent(17772,10,"RWC_Start")
RegisterUnitEvent(17772,4,"RWC_Failed")