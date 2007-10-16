#ifndef SKILL_H
#define SKILL_H


#define SKILL_FROST                     6
#define SKILL_FIRE                      8
#define SKILL_ARMS                      26
#define SKILL_COMBAT                    38
#define SKILL_SUBTLETY                  39
#define SKILL_POISONS                   40
#define SKILL_SWORDS                    43
#define SKILL_AXES                      44
#define SKILL_BOWS                      45
#define SKILL_GUNS                      46
#define SKILL_BEAST_MASTERY             50
#define SKILL_SURVIVAL                  51
#define SKILL_MACES                     54
#define SKILL_2H_SWORDS                 55
#define SKILL_HOLY                      56
#define SKILL_SHADOW                    78
#define SKILL_DEFENSE                   95
#define SKILL_LANG_COMMON               98
#define SKILL_RACIAL_DWARVEN            101
#define SKILL_LANG_ORCISH               109
#define SKILL_LANG_DWARVEN              111
#define SKILL_LANG_DARNASSIAN           113
#define SKILL_LANG_TAURAHE              115
#define SKILL_DUAL_WIELD                118
#define SKILL_RACIAL_TAUREN             124
#define SKILL_ORC_RACIAL                125
#define SKILL_RACIAL_NIGHT_ELF          126
#define SKILL_FIRST_AID                 129
#define SKILL_FERAL_COMBAT              134
#define SKILL_LANG_THALASSIAN           137
#define SKILL_STAVES                    136
#define SKILL_LANG_DRACONIC             138
#define SKILL_LANG_DEMON_TONGUE         139
#define SKILL_LANG_TITAN                140
#define SKILL_LANG_OLD_TONGUE           141
#define SKILL_SURVIVAL2                 142
#define SKILL_RIDING_HORSE              148
#define SKILL_RIDING_WOLF               149
#define SKILL_RIDING_RAM                152
#define SKILL_RIDING_TIGER              150
#define SKILL_SWIMING                   155
#define SKILL_2H_MACES                  160
#define SKILL_UNARMED                   162
#define SKILL_MARKSMANSHIP              163
#define SKILL_BLACKSMITHING             164
#define SKILL_LEATHERWORKING            165
#define SKILL_ALCHEMY                   171
#define SKILL_2H_AXES                   172
#define SKILL_DAGGERS                   173
#define SKILL_THROWN                    176
#define SKILL_HERBALISM                 182
#define SKILL_GENERIC_DND               183
#define SKILL_RETRIBUTION               184
#define SKILL_COOKING                   185
#define SKILL_MINING                    186
#define SKILL_PET_IMP                   188
#define SKILL_PET_FELHUNTER             189
#define SKILL_TAILORING                 197
#define SKILL_ENGINERING                202
#define SKILL_PET_SPIDER                203
#define SKILL_PET_VOIDWALKER            204
#define SKILL_PET_SUCCUBUS              205
#define SKILL_PET_INFERNAL              206
#define SKILL_PET_DOOMGUARD             207
#define SKILL_PET_WOLF                  208
#define SKILL_PET_CAT                   209
#define SKILL_PET_BEAR                  210
#define SKILL_PET_BOAR                  211
#define SKILL_PET_CROCILISK             212
#define SKILL_PET_CARRION_BIRD          213
#define SKILL_PET_GORILLA               215
#define SKILL_PET_CRAB                  214
#define SKILL_PET_RAPTOR                217
#define SKILL_PET_TALLSTRIDER           218
#define SKILL_RACIAL_UNDED              220
#define SKILL_CROSSBOWS                 226
#define SKILL_SPEARS                    227
#define SKILL_WANDS                     228
#define SKILL_POLEARMS                  229
#define SKILL_ATTRIBUTE_ENCHANCEMENTS   230
#define SKILL_SLAYER_TALENTS            231
#define SKILL_MAGIC_TALENTS             233
#define SKILL_DEFENSIVE_TALENTS         234
#define SKILL_PET_SCORPID               236
#define SKILL_ARCANE                    237
#define SKILL_PET_TURTLE                251
#define SKILL_FURY                      256
#define SKILL_PROTECTION                257
#define SKILL_BEAST_TRAINING            261
#define SKILL_PROTECTION2               267
#define SKILL_PET_TALENTS               270
#define SKILL_PLATE_MAIL                293
#define SKILL_ASSASSINATION             253
#define SKILL_LANG_GNOMISH              313
#define SKILL_LANG_TROLL                315
#define SKILL_ENCHANTING                333
#define SKILL_DEMONOLOGY                354
#define SKILL_AFFLICTION                355
#define SKILL_FISHING                   356
#define SKILL_ENHANCEMENT               373
#define SKILL_RESTORATION               374
#define SKILL_ELEMENTAL_COMBAT          375
#define SKILL_SKINNING                  393
#define SKILL_LEATHER                   414
#define SKILL_CLOTH                     415
#define SKILL_MAIL                      413
#define SKILL_SHIELD                    433
#define SKILL_FIST_WEAPONS              473
#define SKILL_TRACKING_BEAST            513
#define SKILL_TRACKING_HUMANOID         514
#define SKILL_TRACKING_DEMON            516
#define SKILL_TRACKING_UNDEAD           517
#define SKILL_TRACKING_DRAGON           518
#define SKILL_TRACKING_ELEMENTAL        519
#define SKILL_RIDING_RAPTOR             533
#define SKILL_RIDING_MECHANOSTRIDER     553
#define SKILL_RIDING_UNDEAD_HORSE       554
#define SKILL_RESTORATION2              573
#define SKILL_BALANCE                   574
#define SKILL_DESTRUCTION               593
#define SKILL_HOLY2                     594
#define SKILL_DISCIPLINE                613
#define SKILL_LOCKPICKING               633
#define SKILL_PET_BAT                   653
#define SKILL_PET_HYENA                 654
#define SKILL_PET_OWL                   655
#define SKILL_PET_WIND_SERPENT          656
#define SKILL_LANG_GUTTERSPEAK          673
#define SKILL_RIDING_KODO               713
#define SKILL_RACIAL_TROLL              733
#define SKILL_RACIAL_GNOME              753
#define SKILL_RACIAL_HUMAN              754
#define SKILL_JEWELCRAFTING             755
#define SKILL_RACIAL_BLOODELF           756
#define SKILL_PET_EVENT_REMOTECONTROL   758
#define SKILL_LANG_DRAENEI              759
#define SKILL_DRAENEI_RACIAL            760
#define SKILL_PET_FELGUARD              761
#define SKILL_RIDING                    762
#define SKILL_PET_DRAGONHAWK            763
#define SKILL_PET_NETHER_RAY            764
#define SKILL_PET_SPOREBAT              765
#define SKILL_PET_WARP_STALKER          766
#define SKILL_PET_RAVAGER               767
#define SKILL_PET_SERPENT               768
#define SKILL_INTERNAL                  769


#define SKILL_TYPE_ATTRIBUTES           5
#define SKILL_TYPE_WEAPON               6
#define SKILL_TYPE_CLASS                7
#define SKILL_TYPE_ARMOR                8
#define SKILL_TYPE_SECONDARY            9
#define SKILL_TYPE_LANGUAGE             10
#define SKILL_TYPE_PROFESSION           11
#define SKILL_TYPE_NA                   12

static char **SkillNames=0;
static uint32 maxskill=0;

//Sorry capt, i prefer it to load from the dbc's rather then hardcoded -DGM
/*
static char * SkillNames[] = 
{
    0,//0
    0,//1
    0,//2
    0,//3
    0,//4
    0,//5
    "SKILL_FROST",//6
    0, //7
    "SKILL_FIRE",//8
    0,//9
    0,//10
    0,//11
    0,//12
    0,//13
    0,//14
    0,//15
    0,//16
    0,//17
    0,//18
    0,//19
    0,//20
    0,//21
    0,//22
    0,//23
    0,//24
    0,//25
    "SKILL_ARMS",//26
    0,//27
    0,//28
    0,//29
    0,//30
    0,//31
    0,//32
    0,//33
    0,//34
    0,//35
    0,//36
    0,//37
    "SKILL_COMBAT",//38
    "SKILL_SUBTLETY",//39
    "SKILL_POISONS",//40
    0,//41
    0,//42
    "SKILL_SWORDS",//43
    "SKILL_AXES",//44
    "SKILL_BOWS",//45
    "SKILL_GUNS",//46
    0,//47
    0,//48
    0,//49
    "SKILL_BEAST_MASTERY",//50
    "SKILL_SURVIVAL",//51
    0,//52
    0,//53
    "SKILL_MACES",//54
    "SKILL_2H_SWORDS",//55
    "SKILL_HOLY",//56
    0,//57
    0,//58
    0,//59
    0,//60
    0,//61
    0,//62
    0,//63
    0,//64
    0,//65
    0,//66
    0,//67
    0,//68
    0,//69
    0,//70
    0,//71
    0,//72
    0,//73
    0,//74
    0,//75
    0,//76
    0,//77
    "SKILL_SHADOW",//78
    0,//79
    0,//80
    0,//81
    0,//82
    0,//83
    0,//84
    0,//85
    0,//86
    0,//87
    0,//88
    0,//89
    0,//90
    0,//91
    0,//92
    0,//93
    0,//94
    "SKILL_DEFENSE",//95
    0,//96
    0,//97
    "SKILL_LANG_COMMON",//98
    0,//99
    0,//100
    "SKILL_RACIAL_DWARVEN",//101
    0,//102
    0,//103
    0,//104
    0,//105
    0,//106
    0,//107
    0,//108
    "SKILL_LANG_ORCISH",//109
    0,//110
    "SKILL_LANG_DWARVEN",//111
    0,//112
    "SKILL_LANG_DARNASSIAN",//113
    0,//114
    "SKILL_LANG_TAURAHE",//115
    0,//116
    0,//117
    "SKILL_DUAL_WIELD",//118
    0,//119
    0,//120
    0,//121
    0,//122
    0,//123
    "SKILL_RACIAL_TAUREN",//124
    "SKILL_ORC_RACIAL",//125
    "SKILL_RACIAL_NIGHT_ELF",//126
    0,
    0,
    "SKILL_FIRST_AID",//129
    0,
    0,
    0,
    0,
    "SKILL_FERAL_COMBAT",//134
    0,
    "SKILL_STAVES",//136
    "SKILL_LANG_THALASSIAN",//137
    "SKILL_LANG_DRACONIC",//138
    "SKILL_LANG_DEMON_TONGUE",//139
    "SKILL_LANG_TITAN",//140
    "SKILL_LANG_OLD_TONGUE",//141
    "SKILL_SURVIVAL2",//142
    0,
    0,
    0,
    0,
    0,
    "SKILL_RIDING_HORSE",//148
    "SKILL_RIDING_WOLF",//149
    "SKILL_RIDING_TIGER",//150
    0,
    "SKILL_RIDING_RAM",//152
    0,
    0,    
    "SKILL_SWIMING",//155
    0,
    0,
    0,
    0,
    "SKILL_2H_MACES",//160
    0,
    "SKILL_UNARMED",//162
    "SKILL_MARKSMANSHIP",//163
    "SKILL_BLACKSMITHING",//164
    "SKILL_LEATHERWORKING",//165
    0,
    0,
    0,
    0,
    0,
    "SKILL_ALCHEMY",//171
    "SKILL_2H_AXES",//172
    "SKILL_DAGGERS",//173
    0,
    0,
    "SKILL_THROWN",//176
    0,
    0,
    0,
    0,
    0,
    "SKILL_HERBALISM",//182
    "SKILL_GENERIC_DND",//183
    "SKILL_RETRIBUTION",//184
    "SKILL_COOKING",//185
    "SKILL_MINING",//186
    0,
    "SKILL_PET_IMP",//188
    "SKILL_PET_FELHUNTER",//189
    0,//190
    0,//191
    0,//192
    0,//193
    0,//194
    0,//195
    0,//196
    "SKILL_TAILORING",//197
    0,//198
    0,//199
    0,//200
    0,//201
    "SKILL_ENGINERING",//202
    "SKILL_PET_SPIDER",//203
    "SKILL_PET_VOIDWALKER",//204
    "SKILL_PET_SUCCUBUS",//205
    "SKILL_PET_INFERNAL",//206
    "SKILL_PET_DOOMGUARD",//207
    "SKILL_PET_WOLF",//208
    "SKILL_PET_CAT",//209
    "SKILL_PET_BEAR",//210
    "SKILL_PET_BOAR",//211
    "SKILL_PET_CROCILISK",//212
    "SKILL_PET_CARRION_BIRD",//213
    "SKILL_PET_CRAB",//214
    "SKILL_PET_GORILLA",//215
    0,
    "SKILL_PET_RAPTOR",//217
    "SKILL_PET_TALLSTRIDER",//218
    0,
    "SKILL_RACIAL_UNDED",//220
    0,//221
    0,//222
    0,//223
    0,//224
    0,//225
    "SKILL_CROSSBOWS",//226
    "SKILL_SPEARS",//227
    "SKILL_WANDS",//228
    "SKILL_POLEARMS",//229
    "SKILL_ATTRIBUTE_ENCHANCEMENTS",//230
    "SKILL_SLAYER_TALENTS",//231
    0,
    "SKILL_MAGIC_TALENTS",//233
    "SKILL_DEFENSIVE_TALENTS",//234
    0,
    "SKILL_PET_SCORPID",//236
    "SKILL_ARCANE",//237
    0,//238
    0,//239
    0,//240
    0,//241
    0,//242
    0,//243
    0,//244
    0,//245
    0,//246
    0,//247
    0,//248
    0,//249
    0,//250
    "SKILL_PET_TURTLE",//251
    0,
    "SKILL_ASSASSINATION",//253
    0,
    0,
    "SKILL_FURY ",//256
    "SKILL_PROTECTION",//257
    0,
    0,
    0,
    "SKILL_BEAST_TRAINING",//261
    0,
    0,
    0,
    0,
    0,
    "SKILL_PROTECTION2",//267
    0,
    0,
    "SKILL_PET_TALENTS",//270
    0,//271
    0,//272
    0,//273
    0,//274
    0,//275
    0,//276
    0,//277
    0,//278
    0,//279
    0,//280
    0,//281
    0,//282
    0,//283
    0,//284
    0,//285
    0,//286
    0,//287
    0,//288
    0,//289
    0,//290
    0,//291
    0,//292
    "SKILL_PLATE_MAIL",//293
    0,//294
    0,//295
    0,//296
    0,//297
    0,//298
    0,//299
    0,//300
    0,//301
    0,//302
    0,//303
    0,//304
    0,//305
    0,//306
    0,//307
    0,//308
    0,//309
    0,//310
    0,//311
    0,//312
    "SKILL_LANG_GNOMISH",//313
    0,
    "SKILL_LANG_TROLL",//315
    0,//316
    0,//317
    0,//318
    0,//319
    0,//320
    0,//321
    0,//322
    0,//323
    0,//324
    0,//325
    0,//326
    0,//327
    0,//328
    0,//329
    0,//330
    0,//331
    0,//332
    "SKILL_ENCHANTING",//333
    0,//334
    0,//335
    0,//336
    0,//337
    0,//338
    0,//339
    0,//340
    0,//341
    0,//342
    0,//343
    0,//344
    0,//345
    0,//346
    0,//347
    0,//348
    0,//349
    0,//350
    0,//351
    0,//352
    0,//353
    "SKILL_DEMONOLOGY",//354
    "SKILL_AFFLICTION",//355
    "SKILL_FISHING",//356
    0,//357
    0,//358
    0,//359
    0,//360
    0,//361
    0,//362
    0,//363
    0,//364
    0,//365
    0,//366
    0,//367
    0,//368
    0,//369
    0,//370
    0,//371
    0,//372
    "SKILL_ENHANCEMENT",//373
    "SKILL_RESTORATION",//374
    "SKILL_ELEMENTAL_COMBAT",//375
    0,//376
    0,//377
    0,//378
    0,//379
    0,//380
    0,//381
    0,//382
    0,//383
    0,//384
    0,//385
    0,//386
    0,//387
    0,//388
    0,//389
    0,//390
    0,//391
    0,//392
    "SKILL_SKINNING",//393
    0,//394
    0,//395
    0,//396
    0,//397
    0,//398
    0,//399
    0,//400
    0,//401
    0,//402
    0,//403
    0,//404
    0,//405
    0,//406
    0,//407
    0,//408
    0,//409
    0,//410
    0,//411
    0,//412
    "SKILL_MAIL ",//413
    "SKILL_LEATHER",//414
    "SKILL_CLOTH",//415
    0,//416
    0,//417
    0,//418
    0,//419
    0,//420
    0,//421
    0,//422
    0,//423
    0,//424
    0,//425
    0,//426
    0,//427
    0,//428
    0,//429
    0,//430
    0,//431
    0,//432
    "SKILL_SHIELD",//433
    0,//434
    0,//435
    0,//436
    0,//437
    0,//438
    0,//439
    0,//440
    0,//441
    0,//442
    0,//443
    0,//444
    0,//445
    0,//446
    0,//447
    0,//448
    0,//449
    0,//450
    0,//451
    0,//452
    0,//453
    0,//454
    0,//455
    0,//456
    0,//457
    0,//458
    0,//459
    0,//460
    0,//461
    0,//462
    0,//463
    0,//464
    0,//465
    0,//466
    0,//467
    0,//468
    0,//469
    0,//470
    0,//471
    0,//472
    "SKILL_FIST_WEAPONS",//473
    0,//474
    0,//475
    0,//476
    0,//477
    0,//478
    0,//479
    0,//480
    0,//481
    0,//482
    0,//483
    0,//484
    0,//485
    0,//486
    0,//487
    0,//488
    0,//489
    0,//490
    0,//491
    0,//492
    0,//493
    0,//494
    0,//495
    0,//496
    0,//497
    0,//498
    0,//499
    0,//500
    0,//501
    0,//502
    0,//503
    0,//504
    0,//505
    0,//506
    0,//507
    0,//508
    0,//509
    0,//510
    0,//511
    0,//512
    "SKILL_TRACKING_BEAST",//513
    "SKILL_TRACKING_HUMANOID",//514
    0,//515
    "SKILL_TRACKING_DEMON",//516
    "SKILL_TRACKING_UNDEAD",//517
    "SKILL_TRACKING_DRAGON",//518
    "SKILL_TRACKING_ELEMENTAL",//519
    0,//520
    0,//521
    0,//522
    0,//523
    0,//524
    0,//525
    0,//526
    0,//527
    0,//528
    0,//529
    0,//530
    0,//531
    0,//532
    "SKILL_RIDING_RAPTOR",//533
    0,//534
    0,//535
    0,//536
    0,//537
    0,//538
    0,//539
    0,//540
    0,//541
    0,//542
    0,//543
    0,//544
    0,//545
    0,//546
    0,//547
    0,//548
    0,//549
    0,//550
    0,//551
    0,//552
    "SKILL_RIDING_MECHANOSTRIDER",//553
    "SKILL_RIDING_UNDEAD_HORSE",//554
    0,//555
    0,//556
    0,//557
    0,//558
    0,//559
    0,//560
    0,//561
    0,//562
    0,//563
    0,//564
    0,//565
    0,//566
    0,//567
    0,//568
    0,//569
    0,//570
    0,//571
    0,//572
    "SKILL_RESTORATION2",//573
    "SKILL_BALANCE",//574
    0,//575
    0,//576
    0,//577
    0,//578
    0,//579
    0,//580
    0,//581
    0,//582
    0,//583
    0,//584
    0,//585
    0,//586
    0,//587
    0,//588
    0,//589
    0,//590
    0,//591
    0,//592
    "SKILL_DESTRUCTION",//593
    "SKILL_HOLY2",//594
    0,//595
    0,//596
    0,//597
    0,//598
    0,//599
    0,//600
    0,//601
    0,//602
    0,//603
    0,//604
    0,//605
    0,//606
    0,//607
    0,//608
    0,//609
    0,//610
    0,//611
    0,//612
    "SKILL_DISCIPLINE",//613
    0,//614
    0,//615
    0,//616
    0,//617
    0,//618
    0,//619
    0,//620
    0,//621
    0,//622
    0,//623
    0,//624
    0,//625
    0,//626
    0,//627
    0,//628
    0,//629
    0,//630
    0,//631
    0,//632
    "SKILL_LOCKPICKING",//633
    0,//634
    0,//635
    0,//636
    0,//637
    0,//638
    0,//639
    0,//640
    0,//641
    0,//642
    0,//643
    0,//644
    0,//645
    0,//646
    0,//647
    0,//648
    0,//649
    0,//650
    0,//651
    0,//652
    "SKILL_PET_BAT",//653
    "SKILL_PET_HYENA",//654
    "SKILL_PET_OWL",//655
    "SKILL_PET_WIND_SERPENT",//656
    0,//657
    0,//658
    0,//659
    0,//660
    0,//661
    0,//662
    0,//663
    0,//664
    0,//665
    0,//666
    0,//667
    0,//668
    0,//669
    0,//670
    0,//671
    0,//672
    "SKILL_LANG_GUTTERSPEAK",//673
    0,//674
    0,//675
    0,//676
    0,//677
    0,//678
    0,//679
    0,//680
    0,//681
    0,//682
    0,//683
    0,//684
    0,//685
    0,//686
    0,//687
    0,//688
    0,//689
    0,//690
    0,//691
    0,//692
    0,//693
    0,//694
    0,//695
    0,//696
    0,//697
    0,//698
    0,//699
    0,//700
    0,//701
    0,//702
    0,//703
    0,//704
    0,//705
    0,//706
    0,//707
    0,//708
    0,//709
    0,//710
    0,//711
    0,//712
    "SKILL_RIDING_KODO",//713
    0,//714
    0,//715
    0,//716
    0,//717
    0,//718
    0,//719
    0,//720
    0,//721
    0,//722
    0,//723
    0,//724
    0,//725
    0,//726
    0,//727
    0,//728
    0,//729
    0,//730
    0,//731
    0,//732
    "SKILL_RACIAL_TROLL",//733
    0,//734
    0,//735
    0,//736
    0,//737
    0,//738
    0,//739
    0,//740
    0,//741
    0,//742
    0,//743
    0,//744
    0,//745
    0,//746
    0,//747
    0,//748
    0,//749
    0,//750
    0,//751
    0,//752
    "SKILL_RACIAL_GNOME",//753
    "SKILL_RACIAL_HUMAN",//754
    "SKILL_JEWELCRAFTING",//755
    "SKILL_RACIAL_BLOODELF",//756
    0,//757
    "SKILL_PET_EVENT_REMOTECONTROL",//758
    "SKILL_LANG_DRAENEI",//759
    "SKILL_DRAENEI_RACIAL",//760
    "SKILL_PET_FELGUARD",//761
    "SKILL_RIDING",//762
    "SKILL_PET_DRAGONHAWK",//763
    "SKILL_PET_NETHER_RAY",//764
    "SKILL_PET_SPOREBAT",           //765
    "SKILL_PET_WARP_STALKER",       //766
    "SKILL_PET_RAVAGER",            //767
    "SKILL_PET_SERPENT",            //768
    "SKILL_INTERNAL",               //769
};
*/

typedef struct{
    uint8   itemclass;
    uint32  subclass;
}ItemProf;
const ItemProf * GetProficiencyBySkill(uint32 skill);


#endif
