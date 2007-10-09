//sadly we cannot simply import files from ascent so we copy paste stuff we need here

#ifndef uint32
	typedef unsigned int uint32;
#endif
#ifndef int
	typedef int int32;
#endif

#define ASSERT assert

#define SERVER_DECL __declspec(dllexport)

const char * spellentrYFormat = "uuuuuuuuuuuuuuuuuuuuuuuuuuuuuiuuuuuuuuuufuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuffffffiiiiiiuuuuuuuuuuuuuuufffuuuuuuuuuuuufffuuuuuxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxuuuuuuuuuuifffuuuuuu";
/*
uuuuuuuuuu
uuuuuuuuuu
uuuuuuuuui
uuuuuuuuuu
fuuuuuuuuu
uuuuuuuuuu
uuuuuuuuuu
uuuffffffi
iiiiiuuuuu
uuuuuuuuuu
fffuuuuuuu
uuuuufffuu
uuuxxxxxxx
xxxxxxxxxx - 140
xxxxxxxxxx
xxxxxxxxxx
xxxxxxxxxx
xxxxxxxxxx
xxxxxxxxxx
xuuuuuuuuu
uifffuuuuu
u */

#define SPELL_DBC_CLIENT_NUMBER "2.2.0"
//this might change from 1 version to another of the DBC
#define SPELL_COLUMN_COUNT 211

#define SQL_INSERTS_PER_QUERY 1000

// Struct for the entry in Spell.dbc
struct SpellEntry
{
    uint32 Id;                              //1
    uint32 School;                          //2
    uint32 Category;                        //3
    uint32 field4;                          //4 something like spelltype 0: general spells 1: Pet spells 2: Disguise / transormation spells 3: enchantment spells
    uint32 DispelType;                      //5
    uint32 MechanicsType;                   //6
    uint32 Attributes;                      //7
    uint32 AttributesEx;                    //8
    uint32 Flags3;                          //9
    uint32 Flags4;                          //10 // Flags to
    uint32 field11;                         //11 // Flags....
    uint32 unk201_1;                        //12 // Flags 2.0.1 unknown one
    uint32 RequiredShapeShift;              //13 // Flags BitMask for shapeshift spells
    uint32 UNK14;                           //14-> this is wrong // Flags BitMask for which shapeshift forms this spell can NOT be used in.
    uint32 Targets;                         //15 - N / M
    uint32 TargetCreatureType;              //16
    uint32 RequiresSpellFocus;              //17
    uint32 CasterAuraState;                 //18
    uint32 TargetAuraState;                 //19
    uint32 unk201_2;                        //20 2.0.1 unknown two
    uint32 unk201_3;                        //21 2.0.1 unknown three
    uint32 CastingTimeIndex;                //22
    uint32 RecoveryTime;                    //23
    uint32 CategoryRecoveryTime;            //24 recoverytime
    uint32 InterruptFlags;                  //25
    uint32 AuraInterruptFlags;              //26
    uint32 ChannelInterruptFlags;           //27
    uint32 procFlags;                       //28
    uint32 procChance;                      //29
    int32 procCharges;                      //30
    uint32 maxLevel;                        //31
    uint32 baseLevel;                       //32
    uint32 spellLevel;                      //33
    uint32 DurationIndex;                   //34
    uint32 powerType;                       //35
    uint32 manaCost;                        //36
    uint32 manaCostPerlevel;                //37
    uint32 manaPerSecond;                   //38
    uint32 manaPerSecondPerLevel;           //39
    uint32 rangeIndex;                      //40
    float  speed;                           //41
    uint32 modalNextSpell;                  //42
    uint32 maxstack;                        //43
    uint32 Totem[2];                        //44 - 45
    uint32 Reagent[8];                      //46 - 53
    uint32 ReagentCount[8];                 //54 - 61
    uint32 EquippedItemClass;               //62
    uint32 EquippedItemSubClass;            //63
    uint32 RequiredItemFlags;               //64
    uint32 Effect[3];                       //65 - 67
    uint32 EffectDieSides[3];               //68 - 70
    uint32 EffectBaseDice[3];               //71 - 73
    float  EffectDicePerLevel[3];           //74 - 76
    float  EffectRealPointsPerLevel[3];     //77 - 79
    int32  EffectBasePoints[3];             //80 - 82
    int32  EffectMechanic[3];               //83 - 85       Related to SpellMechanic.dbc
    uint32 EffectImplicitTargetA[3];        //86 - 88
    uint32 EffectImplicitTargetB[3];        //89 - 91
    uint32 EffectRadiusIndex[3];            //92 - 94
    uint32 EffectApplyAuraName[3];          //95 - 97
    uint32 EffectAmplitude[3];              //98 - 100
    float  Effectunknown[3];                //101 - 103     This value is the $ value from description
    uint32 EffectChainTarget[3];            //104 - 106
    uint32 EffectSpellGroupRelation[3];     //107 - 109     Not sure maybe we should rename it. its the relation to field: SpellGroupType
    uint32 EffectMiscValue[3];              //110 - 112
    uint32 EffectTriggerSpell[3];           //113 - 115
    float  EffectPointsPerComboPoint[3];    //116 - 118
    uint32 SpellVisual;                     //119
    uint32 field114;                        //120
    uint32 dummy;                           //121
    uint32 CoSpell;                         //122   activeIconID;
    uint32 spellPriority;                   //123
    uint32 Name;                            //124
    uint32 NameAlt1;                        //125
    uint32 NameAlt2;                        //126
    uint32 NameAlt3;                        //127
    uint32 NameAlt4;                        //128
    uint32 NameAlt5;                        //129
    uint32 NameAlt6;                        //130
    uint32 NameAlt7;                        //131
    uint32 NameAlt8;                        //132
    uint32 NameAlt9;                        //133
    uint32 NameAlt10;                       //134
    uint32 NameAlt11;                       //135
    uint32 NameAlt12;                       //136
    uint32 NameAlt13;                       //137
    uint32 NameAlt14;                       //138
    uint32 NameAlt15;                       //139
    uint32 NameFlags;                       //140
    uint32 Rank;                            //141
    uint32 RankAlt1;                        //142
    uint32 RankAlt2;                        //143
    uint32 RankAlt3;                        //144
    uint32 RankAlt4;                        //145
    uint32 RankAlt5;                        //146
    uint32 RankAlt6;                        //147
    uint32 RankAlt7;                        //148
    uint32 RankAlt8;                        //149
    uint32 RankAlt9;                        //150
    uint32 RankAlt10;                       //151
    uint32 RankAlt11;                       //152
    uint32 RankAlt12;                       //153
    uint32 RankAlt13;                       //154
    uint32 RankAlt14;                       //155
    uint32 RankAlt15;                       //156
    uint32 RankFlags;                       //157
    uint32 Description;                     //158
    uint32 DescriptionAlt1;                 //159
    uint32 DescriptionAlt2;                 //160
    uint32 DescriptionAlt3;                 //161
    uint32 DescriptionAlt4;                 //162
    uint32 DescriptionAlt5;                 //163
    uint32 DescriptionAlt6;                 //164
    uint32 DescriptionAlt7;                 //165
    uint32 DescriptionAlt8;                 //166
    uint32 DescriptionAlt9;                 //167
    uint32 DescriptionAlt10;                //168
    uint32 DescriptionAlt11;                //169
    uint32 DescriptionAlt12;                //170
    uint32 DescriptionAlt13;                //171
    uint32 DescriptionAlt14;                //172
    uint32 DescriptionAlt15;                //173
    uint32 DescriptionFlags;                //174
    uint32 BuffDescription;                 //175
    uint32 BuffDescriptionAlt1;             //176
    uint32 BuffDescriptionAlt2;             //177
    uint32 BuffDescriptionAlt3;             //178
    uint32 BuffDescriptionAlt4;             //179
    uint32 BuffDescriptionAlt5;             //180
    uint32 BuffDescriptionAlt6;             //181
    uint32 BuffDescriptionAlt7;             //182
    uint32 BuffDescriptionAlt8;             //183
    uint32 BuffDescriptionAlt9;             //184
    uint32 BuffDescriptionAlt10;            //185
    uint32 BuffDescriptionAlt11;            //186
    uint32 BuffDescriptionAlt12;            //187
    uint32 BuffDescriptionAlt13;            //188
    uint32 BuffDescriptionAlt14;            //189
//    uint32 BuffDescriptionAlt15;            //190
	uint32 proc_interval;                   //190 - !!! Using it instead of BuffDescriptionAlt15 !!!
    uint32 buffdescflags;                   //191
    uint32 ManaCostPercentage;              //192
    uint32 unkflags;                        //193 
    uint32 StartRecoveryTime;               //194
    uint32 StartRecoveryCategory;           //195
    uint32 SpellFamilyName;                 //196
    uint32 SpellGroupType;                  //197   flags 
    uint32 unkne;                           //198   flags hackwow=shit 
    uint32 MaxTargets;                      //199 
    uint32 Spell_Dmg_Type;                  //200   dmg_class Integer      0=None, 1=Magic, 2=Melee, 3=Ranged
    uint32 FG;                              //201   0,1,2 related to Spell_Dmg_Type I think
    int32 FH;                               //202   related to paladin aura's 
    float dmg_multiplier[3];                //203 - 205   if the name is correct I dono
    uint32 FL;                              //206   only one spellid:6994 has this value = 369
    uint32 FM;                              //207   only one spellid:6994 has this value = 4
    uint32 FN;                              //208   only one spellid:26869  has this flag = 1

    // soz guys, gotta use these 3 vars :p
    uint32 unk201_4; // these are related to creating a item through a spell
    uint32 unk201_5;
    uint32 unk201_6; // related to custom spells, summon spell quest related spells
    
    // custom shit
    uint32 buffType;                        //209

    // this protects players from having >1 rank of a spell
    uint32 RankNumber;                      //210
    uint32 NameHash;                        //211

    uint32 DiminishStatus;                  //212
    uint32 buffIndexType;
};

const char sql_translation_table[220][3][300] = 
{
    {"uint32" ,"Id","0"},                              //1
    {"uint32" ,"School","0"},                          //2
    {"uint32" ,"Category","0"},                        //3
    {"uint32" ,"field4","0"},                          //4 something like spelltype 0: general spells 1: Pet spells 2: Disguise / transormation spells 3: enchantment spells
    {"uint32" ,"DispelType","0"},                      //5
    {"uint32" ,"MechanicsType","0"},                   //6
    {"uint32" ,"Attributes","0"},                      //7
    {"uint32" ,"AttributesEx","0"},                    //8
    {"uint32" ,"Flags3","0"},                          //9
    {"uint32" ,"Flags4","0"},                          //10 // Flags to
    {"uint32" ,"field11","0"},                         //11 // Flags....
    {"uint32" ,"unk201_1","0"},                        //12 // Flags 2.0.1 unknown one
    {"uint32" ,"RequiredShapeShift","0"},              //13 // Flags BitMask for shapeshift spells
    {"uint32" ,"UNK14","0"},                           //14-> this is wrong // Flags BitMask for which shapeshift forms this spell can NOT be used in.
    {"uint32" ,"Targets","0"},                         //15 - N / M
    {"uint32" ,"TargetCreatureType","0"},              //16
    {"uint32" ,"RequiresSpellFocus","0"},              //17
    {"uint32" ,"CasterAuraState","0"},                 //18
    {"uint32" ,"TargetAuraState","0"},                 //19
    {"uint32" ,"unk201_2","0"},                        //20 2.0.1 unknown two
    {"uint32" ,"unk201_3","0"},                        //21 2.0.1 unknown three
    {"uint32" ,"CastingTimeIndex","0"},                //22
    {"uint32" ,"RecoveryTime","0"},                    //23
    {"uint32" ,"CategoryRecoveryTime","0"},            //24 recoverytime
    {"uint32" ,"InterruptFlags","0"},                  //25
    {"uint32" ,"AuraInterruptFlags","0"},              //26
    {"uint32" ,"ChannelInterruptFlags","0"},           //27
    {"uint32" ,"procFlags","0"},                       //28
    {"uint32" ,"procChance","0"},                      //29
    {"int32"  ,"procCharges","0"},                      //30
    {"uint32" ,"maxLevel","0"},                        //31
    {"uint32" ,"baseLevel","0"},                       //32
    {"uint32" ,"spellLevel","0"},                      //33
    {"uint32" ,"DurationIndex","0"},                   //34
    {"uint32" ,"powerType","0"},                       //35
    {"uint32" ,"manaCost","0"},                        //36
    {"uint32" ,"manaCostPerlevel","0"},                //37
    {"uint32" ,"manaPerSecond","0"},                   //38
    {"uint32" ,"manaPerSecondPerLevel","0"},           //39
    {"uint32" ,"rangeIndex","0"},                      //40
    {"float"  ,"speed","0"},                           //41
    {"uint32" ,"modalNextSpell","0"},                  //42
    {"uint32" ,"maxstack","0"},                        //43
    {"uint32" ,"Totem_1","0"},                        //44 - 45
    {"uint32" ,"Totem_2","0"},                        //44 - 45
    {"uint32" ,"Reagent_1","0"},                      //46 - 53
    {"uint32" ,"Reagent_2","0"},                      //46 - 53
    {"uint32" ,"Reagent_3","0"},                      //46 - 53
    {"uint32" ,"Reagent_4","0"},                      //46 - 53
    {"uint32" ,"Reagent_5","0"},                      //46 - 53
    {"uint32" ,"Reagent_6","0"},                      //46 - 53
    {"uint32" ,"Reagent_7","0"},                      //46 - 53
    {"uint32" ,"Reagent_8","0"},                      //46 - 53
    {"uint32" ,"ReagentCount_1","0"},                 //54 - 61
    {"uint32" ,"ReagentCount_2","0"},                 //54 - 61
    {"uint32" ,"ReagentCount_3","0"},                 //54 - 61
    {"uint32" ,"ReagentCount_4","0"},                 //54 - 61
    {"uint32" ,"ReagentCount_5","0"},                 //54 - 61
    {"uint32" ,"ReagentCount_6","0"},                 //54 - 61
    {"uint32" ,"ReagentCount_7","0"},                 //54 - 61
    {"uint32" ,"ReagentCount_8","0"},                 //54 - 61
    {"uint32" ,"EquippedItemClass","0"},               //62
    {"uint32" ,"EquippedItemSubClass","0"},            //63
    {"uint32" ,"RequiredItemFlags","0"},               //64
    {"uint32" ,"Effect_1","0"},                       //65 - 67
    {"uint32" ,"Effect_2","0"},                       //65 - 67
    {"uint32" ,"Effect_3","0"},                       //65 - 67
    {"uint32" ,"EffectDieSides_1","0"},               //68 - 70
    {"uint32" ,"EffectDieSides_2","0"},               //68 - 70
    {"uint32" ,"EffectDieSides_3","0"},               //68 - 70
    {"uint32" ,"EffectBaseDice_1","0"},               //71 - 73
    {"uint32" ,"EffectBaseDice_2","0"},               //71 - 73
    {"uint32" ,"EffectBaseDice_3","0"},               //71 - 73
    {"float"  ,"EffectDicePerLevel_1","0"},           //74 - 76
    {"float"  ,"EffectDicePerLevel_2","0"},           //74 - 76
    {"float"  ,"EffectDicePerLevel_3","0"},           //74 - 76
    {"float"  ,"EffectRealPointsPerLevel_1","0"},     //77 - 79
    {"float"  ,"EffectRealPointsPerLevel_2","0"},     //77 - 79
    {"float"  ,"EffectRealPointsPerLevel_3","0"},     //77 - 79
    {"int32"  ,"EffectBasePoints_1","0"},             //80 - 82
    {"int32"  ,"EffectBasePoints_2","0"},             //80 - 82
    {"int32"  ,"EffectBasePoints_3","0"},             //80 - 82
    {"int32"  ,"EffectMechanic_1","0"},               //83 - 85       Related to SpellMechanic.dbc
    {"int32"  ,"EffectMechanic_2","0"},               //83 - 85       Related to SpellMechanic.dbc
    {"int32"  ,"EffectMechanic_3","0"},               //83 - 85       Related to SpellMechanic.dbc
    {"uint32" ,"EffectImplicitTargetA_1","0"},        //86 - 88
    {"uint32" ,"EffectImplicitTargetA_2","0"},        //86 - 88
    {"uint32" ,"EffectImplicitTargetA_3","0"},        //86 - 88
    {"uint32" ,"EffectImplicitTargetB_1","0"},        //89 - 91
    {"uint32" ,"EffectImplicitTargetB_2","0"},        //89 - 91
    {"uint32" ,"EffectImplicitTargetB_3","0"},        //89 - 91
    {"uint32" ,"EffectRadiusIndex_1","0"},            //92 - 94
    {"uint32" ,"EffectRadiusIndex_2","0"},            //92 - 94
    {"uint32" ,"EffectRadiusIndex_3","0"},            //92 - 94
    {"uint32" ,"EffectApplyAuraName_1","0"},          //95 - 97
    {"uint32" ,"EffectApplyAuraName_2","0"},          //95 - 97
    {"uint32" ,"EffectApplyAuraName_3","0"},          //95 - 97
    {"uint32" ,"EffectAmplitude_1","0"},              //98 - 100
    {"uint32" ,"EffectAmplitude_2","0"},              //98 - 100
    {"uint32" ,"EffectAmplitude_3","0"},              //98 - 100
    {"float"  ,"Effectunknown_1","0"},                //101 - 103     This value is the $ value from description
    {"float"  ,"Effectunknown_2","0"},                //101 - 103     This value is the $ value from description
    {"float"  ,"Effectunknown_3","0"},                //101 - 103     This value is the $ value from description
    {"uint32" ,"EffectChainTarget_1","0"},            //104 - 106
    {"uint32" ,"EffectChainTarget_2","0"},            //104 - 106
    {"uint32" ,"EffectChainTarget_3","0"},            //104 - 106
    {"uint32" ,"EffectSpellGroupRelation_1","0"},     //107 - 109     Not sure maybe we should rename it. its the relation to field: SpellGroupType
    {"uint32" ,"EffectSpellGroupRelation_2","0"},     //107 - 109     Not sure maybe we should rename it. its the relation to field: SpellGroupType
    {"uint32" ,"EffectSpellGroupRelation_3","0"},     //107 - 109     Not sure maybe we should rename it. its the relation to field: SpellGroupType
    {"uint32" ,"EffectMiscValue_1","0"},              //110 - 112
    {"uint32" ,"EffectMiscValue_2","0"},              //110 - 112
    {"uint32" ,"EffectMiscValue_3","0"},              //110 - 112
    {"uint32" ,"EffectTriggerSpell_1","0"},           //113 - 115
    {"uint32" ,"EffectTriggerSpell_2","0"},           //113 - 115
    {"uint32" ,"EffectTriggerSpell_3","0"},           //113 - 115
    {"float"  ,"EffectPointsPerComboPoint_1","0"},    //116 - 118
    {"float"  ,"EffectPointsPerComboPoint_2","0"},    //116 - 118
    {"float"  ,"EffectPointsPerComboPoint_3","0"},    //116 - 118
    {"uint32" ,"SpellVisual","0"},                     //119
    {"uint32" ,"field114","0"},                        //120
    {"uint32" ,"dummy","0"},                           //121
    {"uint32" ,"CoSpell","0"},                         //122   activeIconID","0"},
    {"uint32" ,"spellPriority","0"},                   //123
    {"str"	  ,"Name","0"},                            //124
    {"uint32" ,"NameAlt1","1"},                        //125
    {"uint32" ,"NameAlt2","1"},                        //126
    {"uint32" ,"NameAlt3","1"},                        //127
    {"uint32" ,"NameAlt4","1"},                        //128
    {"uint32" ,"NameAlt5","1"},                        //129
    {"uint32" ,"NameAlt6","1"},                        //130
    {"uint32" ,"NameAlt7","1"},                        //131
    {"uint32" ,"NameAlt8","1"},                        //132
    {"uint32" ,"NameAlt9","1"},                        //133
    {"uint32" ,"NameAlt10","1"},                       //134
    {"uint32" ,"NameAlt11","1"},                       //135
    {"uint32" ,"NameAlt12","1"},                       //136
    {"uint32" ,"NameAlt13","1"},                       //137
    {"uint32" ,"NameAlt14","1"},                       //138
    {"uint32" ,"NameAlt15","1"},                       //139
    {"uint32" ,"NameFlags","0"},                       //140
    {"uint32" ,"Rank","0"},                            //141
    {"uint32" ,"RankAlt1","1"},                        //142
    {"uint32" ,"RankAlt2","1"},                        //143
    {"uint32" ,"RankAlt3","1"},                        //144
    {"uint32" ,"RankAlt4","1"},                        //145
    {"uint32" ,"RankAlt5","1"},                        //146
    {"uint32" ,"RankAlt6","1"},                        //147
    {"uint32" ,"RankAlt7","1"},                        //148
    {"uint32" ,"RankAlt8","1"},                        //149
    {"uint32" ,"RankAlt9","1"},                        //150
    {"uint32" ,"RankAlt10","1"},                       //151
    {"uint32" ,"RankAlt11","1"},                       //152
    {"uint32" ,"RankAlt12","1"},                       //153
    {"uint32" ,"RankAlt13","1"},                       //154
    {"uint32" ,"RankAlt14","1"},                       //155
    {"uint32" ,"RankAlt15","1"},                       //156
    {"uint32" ,"RankFlags","0"},                       //157
    {"str"	  ,"Description","0"},                     //158
    {"uint32" ,"DescriptionAlt1","1"},                 //159
    {"uint32" ,"DescriptionAlt2","1"},                 //160
    {"uint32" ,"DescriptionAlt3","1"},                 //161
    {"uint32" ,"DescriptionAlt4","1"},                 //162
    {"uint32" ,"DescriptionAlt5","1"},                 //163
    {"uint32" ,"DescriptionAlt6","1"},                 //164
    {"uint32" ,"DescriptionAlt7","1"},                 //165
    {"uint32" ,"DescriptionAlt8","1"},                 //166
    {"uint32" ,"DescriptionAlt9","1"},                 //167
    {"uint32" ,"DescriptionAlt10","1"},                //168
    {"uint32" ,"DescriptionAlt11","1"},                //169
    {"uint32" ,"DescriptionAlt12","1"},                //170
    {"uint32" ,"DescriptionAlt13","1"},                //171
    {"uint32" ,"DescriptionAlt14","1"},                //172
    {"uint32" ,"DescriptionAlt15","1"},                //173
    {"uint32" ,"DescriptionFlags","0"},                //174
    {"uint32" ,"BuffDescription","0"},                 //175
    {"uint32" ,"BuffDescriptionAlt1","1"},             //176
    {"uint32" ,"BuffDescriptionAlt2","1"},             //177
    {"uint32" ,"BuffDescriptionAlt3","1"},             //178
    {"uint32" ,"BuffDescriptionAlt4","1"},             //179
    {"uint32" ,"BuffDescriptionAlt5","1"},             //180
    {"uint32" ,"BuffDescriptionAlt6","1"},             //181
    {"uint32" ,"BuffDescriptionAlt7","1"},             //182
    {"uint32" ,"BuffDescriptionAlt8","1"},             //183
    {"uint32" ,"BuffDescriptionAlt9","1"},             //184
    {"uint32" ,"BuffDescriptionAlt10","1"},            //185
    {"uint32" ,"BuffDescriptionAlt11","1"},            //186
    {"uint32" ,"BuffDescriptionAlt12","1"},            //187
    {"uint32" ,"BuffDescriptionAlt13","1"},            //188
    {"uint32" ,"BuffDescriptionAlt14","1"},            //189
    {"uint32" ,"BuffDescriptionAlt15","1"},			   //190
    {"uint32" ,"buffdescflags","0"},                   //191
    {"uint32" ,"ManaCostPercentage","0"},              //192
    {"uint32" ,"unkflags","0"},                        //193 
    {"uint32" ,"StartRecoveryTime","0"},               //194
    {"uint32" ,"StartRecoveryCategory","0"},           //195
    {"uint32" ,"SpellFamilyName","0"},                 //196
    {"uint32" ,"SpellGroupType","0"},                  //197   flags 
    {"uint32" ,"unkne","0"},                           //198   flags hackwow=shit 
    {"uint32" ,"MaxTargets","0"},                      //199 
    {"uint32" ,"Spell_Dmg_Type","0"},                  //200   dmg_class Integer      0=None, 1=Magic, 2=Melee, 3=Ranged
    {"uint32" ,"FG","0"},                              //201   0,1,2 related to Spell_Dmg_Type I think
    {"int32"  ,"FH","0"},                              //202   related to paladin aura's 
    {"float"  ,"dmg_multiplier_1","0"},                //203 - 205   if the name is correct I dono
    {"float"  ,"dmg_multiplier_2","0"},                //203 - 205   if the name is correct I dono
    {"float"  ,"dmg_multiplier_3","0"},                //203 - 205   if the name is correct I dono
    {"uint32" ,"FL","0"},                              //206   only one spellid:6994 has this value = 369
    {"uint32" ,"FM","0"},                              //207   only one spellid:6994 has this value = 4
    {"uint32" ,"FN","0"},                              //208   only one spellid:26869  has this flag = 1

    // soz guys, gotta use these 3 vars :p
//    {"uint32" ,"unk201_4","0"}, // these are related to creating a item through a spell
//    {"uint32" ,"unk201_5","0"},
//    {"uint32" ,"unk201_6","0"}, // related to custom spells, summon spell quest related spells
    
    // custom shit
    {"uint32" ,"buffType","0"},                        //209

    // this protects players from having >1 rank of a spell
    {"uint32" ,"RankNumber","0"},                      //210
    {"uint32" ,"NameHash","0"},                        //211

    {"uint32" ,"DiminishStatus","0"},                  //212
//    {"uint32" ,"buffIndexType","0"},
//	{"uint32" ,"proc_interval","0"},                   //190 - !!! Using it instead of BuffDescriptionAlt15 !!!
};