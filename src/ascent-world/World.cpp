/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

initialiseSingleton( World );

DayWatcherThread* dw = NULL;

float World::m_movementCompressThreshold;
float World::m_movementCompressThresholdCreatures;
uint32 World::m_movementCompressRate;
uint32 World::m_movementCompressInterval;

World::World()
{
	m_playerLimit = 0;
	m_allowMovement = true;
	m_gmTicketSystem = true;

	reqGmClient = false;
	GmClientChannel = "";

	m_StartTime = 0;
	eventholder = new EventableObjectHolder(-1);
	m_holder = eventholder;
	m_event_Instanceid = eventholder->GetInstanceID();

	mQueueUpdateInterval = 10000;
	PeakSessionCount = 0;
	mInWorldPlayerCount = 0;
	mAcceptedConnections = 0;
	HordePlayers = 0;
	AlliancePlayers = 0;
	gm_skip_attunement = false;
	show_gm_in_who_list = true;
	map_unload_time=0;
#ifndef CLUSTERING
	SocketSendBufSize = WORLDSOCKET_SENDBUF_SIZE;
	SocketRecvBufSize = WORLDSOCKET_RECVBUF_SIZE;
#endif
	m_levelCap=70;
	m_genLevelCap=70;
	m_limitedNames=false;
	m_banTable = NULL;
}

void CleanupRandomNumberGenerators();
void World::LogoutPlayers()
{
	Log.Notice("World", "Logging out players...");
	for(SessionMap::iterator i=m_sessions.begin();i!=m_sessions.end();i++)
	{
		(i->second)->LogoutPlayer(true);
	}

	Log.Notice("World", "Deleting sessions...");
	WorldSession * p;
	for(SessionMap::iterator i=m_sessions.begin();i!=m_sessions.end();)
	{
		p = i->second;
		++i;

		DeleteSession(p);
		//delete p;
	}
}

World::~World()
{
	Log.Notice("LocalizationMgr", "~LocalizationMgr()");
	sLocalizationMgr.Shutdown();

	Log.Notice("WorldLog", "~WorldLog()");
	delete WorldLog::getSingletonPtr();

	Log.Notice("SocialMgr", "~SocialMgr()");
	delete SocialMgr::getSingletonPtr();

	Log.Notice("ObjectMgr", "~ObjectMgr()");
	delete ObjectMgr::getSingletonPtr();
	
	Log.Notice("LootMgr", "~LootMgr()");
	delete LootMgr::getSingletonPtr();
	
	Log.Notice("LfgMgr", "~LfgMgr()");
	delete LfgMgr::getSingletonPtr();

	Log.Notice("ChannelMgr", "~ChannelMgr()");
	delete ChannelMgr::getSingletonPtr();

	Log.Notice("QuestMgr", "~QuestMgr()");
	delete QuestMgr::getSingletonPtr();
  
	Log.Notice("WeatherMgr", "~WeatherMgr()");
	delete WeatherMgr::getSingletonPtr();

	Log.Notice("TaxiMgr", "~TaxiMgr()");
	delete TaxiMgr::getSingletonPtr();
	
	Log.Notice("BattlegroundMgr", "~BattlegroundMgr()");
	delete CBattlegroundManager::getSingletonPtr();

	Log.Notice("InstanceMgr", "~InstanceMgr()");
	sInstanceMgr.Shutdown();

	//sLog.outString("Deleting Thread Manager..");
	//delete ThreadMgr::getSingletonPtr();
	Log.Notice("WordFilter", "~WordFilter()");
	delete g_chatFilter;
	delete g_characterNameFilter;

	Log.Notice("Rnd", "~Rnd()");
	CleanupRandomNumberGenerators();

	for( AreaTriggerMap::iterator i = m_AreaTrigger.begin( ); i != m_AreaTrigger.end( ); ++ i ) 
	{
		delete i->second;
	}

	//eventholder = 0;
	delete eventholder;

	Storage_Cleanup();
	for(list<SpellEntry*>::iterator itr = dummyspells.begin(); itr != dummyspells.end(); ++itr)
		delete *itr;
}


WorldSession* World::FindSession(uint32 id)
{
	m_sessionlock.AcquireReadLock();
	WorldSession * ret = 0;
	SessionMap::const_iterator itr = m_sessions.find(id);

	if(itr != m_sessions.end())
		ret = itr->second;
	
	m_sessionlock.ReleaseReadLock();

	return ret;
}

void World::RemoveSession(uint32 id)
{
	m_sessionlock.AcquireWriteLock();

	SessionMap::iterator itr = m_sessions.find(id);

	if(itr != m_sessions.end())
	{
		delete itr->second;
		m_sessions.erase(itr);
	}

	m_sessionlock.ReleaseWriteLock();
}

void World::AddSession(WorldSession* s)
{
	if(!s)
		return;

	m_sessionlock.AcquireWriteLock();

	ASSERT(s);
	m_sessions[s->GetAccountId()] = s;

	if(m_sessions.size() >  PeakSessionCount)
		PeakSessionCount = (uint32)m_sessions.size();

	m_sessionlock.ReleaseWriteLock();
}

void World::AddGlobalSession(WorldSession *session)
{
	if(!session)
		return;

	SessionsMutex.Acquire();
	Sessions.insert(session);
	SessionsMutex.Release();
}

void World::RemoveGlobalSession(WorldSession *session)
{
	SessionsMutex.Acquire();
	Sessions.erase(session);
	SessionsMutex.Release();
}

bool BasicTaskExecutor::run()
{
	/* Set thread priority, this is a bitch for multiplatform :P */
#ifdef WIN32
	switch(priority)
	{
		case BTE_PRIORITY_LOW:
			::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_LOWEST );
			break;

		case BTW_PRIORITY_HIGH:
			::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL );
			break;

		default:		// BTW_PRIORITY_MED
			::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_NORMAL );
			break;
	}
#else
	struct sched_param param;
	switch(priority)
	{
	case BTE_PRIORITY_LOW:
		param.sched_priority = 0;
		break;

	case BTW_PRIORITY_HIGH:
		param.sched_priority = 10;
		break;

	default:		// BTW_PRIORITY_MED
		param.sched_priority = 5;
		break;
	}
	pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
#endif

	// Execute the task in our new context.
	cb->execute();
#ifdef WIN32
	::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
#else
	param.sched_priority = 5;
	pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
#endif

	return true;
}

void CreateDummySpell(uint32 id)
{
	const char * name = "Dummy Trigger";
	SpellEntry * sp = new SpellEntry;
	memset(sp, 0, sizeof(SpellEntry));
	sp->Id = id;
	sp->Attributes = 384;
	sp->AttributesEx = 268435456;
	sp->Flags3 = 4;
	sp->CastingTimeIndex=1;
	sp->procChance=75;
	sp->rangeIndex=13;
	sp->EquippedItemClass=uint32(-1);
	sp->Effect[0]=3;
	sp->EffectImplicitTargetA[0]=25;
	sp->NameHash=crc32((const unsigned char*)name, (unsigned int)strlen(name));
	sp->dmg_multiplier[0]=1.0f;
	sp->FH=-1;
	dbcSpell.SetRow(id,sp);
	sWorld.dummyspells.push_back(sp);
}

bool World::SetInitialWorldSettings()
{
	Log.Line();
	Player::InitVisibleUpdateBits();

	CharacterDatabase.WaitExecute("UPDATE characters SET online = 0 WHERE online = 1");
	//CharacterDatabase.WaitExecute("UPDATE characters SET level = 70 WHERE level > 70");
	CharacterDatabase.WaitExecute("UPDATE characters SET banned=0,banReason='' WHERE banned > 100 AND banned < %u", UNIXTIME);
   
	m_lastTick = UNIXTIME;

	// TODO: clean this
	time_t tiempo;
	char hour[3];
	char minute[3];
	char second[3];
	struct tm *tmPtr;
	tiempo = UNIXTIME;
	tmPtr = localtime(&tiempo);
	strftime( hour, 3, "%H", tmPtr );
	strftime( minute, 3, "%M", tmPtr );
	strftime( second, 3, "%S", tmPtr );
	m_gameTime = (3600*atoi(hour))+(atoi(minute)*60)+(atoi(second)); // server starts at noon

	// TODO: clean this
	// fill in emotes table
	// it appears not every emote has an animation
	mPrices[1] = 10;
	mPrices[4] = 80;
	mPrices[6] = 150;
	mPrices[8] = 200;
	mPrices[10] = 300;
	mPrices[12] = 800;
	mPrices[14] = 900;
	mPrices[16] = 1800;
	mPrices[18] = 2200;
	mPrices[20] = 2300;
	mPrices[22] = 3600;
	mPrices[24] = 4200;
	mPrices[26] = 6700;
	mPrices[28] = 7200;
	mPrices[30] = 8000;
	mPrices[32] = 11000;
	mPrices[34] = 14000;
	mPrices[36] = 16000;
	mPrices[38] = 18000;
	mPrices[40] = 20000;
	mPrices[42] = 27000;
	mPrices[44] = 32000;
	mPrices[46] = 37000;
	mPrices[48] = 42000;
	mPrices[50] = 47000;
	mPrices[52] = 52000;
	mPrices[54] = 57000;
	mPrices[56] = 62000;
	mPrices[58] = 67000;
	mPrices[60] = 72000;

	// Relation Groups

	uint32 group_relation_paladin_healing_light = 0;
	uint32 group_relation_paladin_sanctified_light = 0;
	uint32 group_relation_shaman_enhancing_totems = 0;
	uint32 group_relation_shaman_restorative_totems = 0;
	uint32 group_relation_shaman_totems = 0;
	uint32 group_relation_shaman_lightning = 0;
	uint32 group_relation_shaman_shock = 0;
	uint32 group_relation_shaman_mental_quickness = 0;
	uint32 group_relation_rogue_elusiveness = 0;
	uint32 group_relation_rogue_poisons = 0;
	uint32 group_relation_rogue_find_weakness = 0;
	uint32 group_relation_rogue_shadow_step = 0;
	uint32 group_relation_rogue_lethality = 0;

	// Start

	uint32 start_time = getMSTime();

	Log.Notice( "World", "Loading DBC files..." );
	if( !LoadDBCs() )
	{
		Log.LargeErrorMessage(LARGERRORMESSAGE_ERROR, "One or more of the DBC files are missing.", "These are absolutely necessary for the server to function.", "The server will not start without them.", NULL);
		return false;
	}

	/*{
		DBCFile moo;
		moo.open("DBC/FactionTemplate.dbc");
		set<pair<uint32, uint32> > fs;
		set<uint32> known;
		for(uint32 i = 0; i < moo.getRecordCount(); ++i)
		{
			fs.insert(make_pair(moo.getRecord(i).getUInt(0), moo.getRecord(i).getUInt(1)));
			known.insert(moo.getRecord(i).getUInt(0));
		}

		QueryResult * result = sDatabase.Query("SELECT entry, faction FROM creature_proto");
		uint32 e, f;
		do 
		{
			e = result->Fetch()[0].GetUInt32();
			f = result->Fetch()[1].GetUInt32();
			FactionDBC * d;
			if(known.find(f) == known.end())
			{
				printf("Bad faction %u!", f);
				//d = sFactionStore.LookupEntryForced(f);
				d = ((FastIndexedDataStore<FactionDBC>*)FactionStore::getSingletonPtr())->LookupEntryForced(f);
				if(!d)
					printf(".. REALLY bad faction\n");
				else
				{
					// find a faction template that matches.. meh ;p
					set<pair<uint32, uint32> >::iterator itr = fs.begin();
					for(; itr != fs.end(); ++itr)
					{
						if(itr->second == f)
						{
							printf(" replaced with %u (%s)\n", itr->first, sFactionStore.LookupString(d->Name));
							sDatabase.WaitExecute("UPDATE creature_proto SET faction = %u WHERE entry = %u", e, itr->second);
							sDatabase.WaitExecute("UPDATE creature_spawns SET factionid = %u WHERE entry = %u", e, itr->second);
							break;
						}
					}
				}
			}
		} while(result->NextRow());
	}*/

	/* Convert area table ids/flags */
	DBCFile area;

	if( !area.open( "DBC/AreaTable.dbc" ) )
	{
		Log.Error( "World", "Cannot find file ./DBC/AreaTable.dbc" );
		return false;
	}

	uint32 flag_, area_, zone_;
	for(uint32 i = 0; i < area.getRecordCount(); ++i)
	{
		area_ = area.getRecord(i).getUInt(0);
		flag_ = area.getRecord(i).getUInt(3);
		zone_ = area.getRecord(i).getUInt(2);

		mAreaIDToTable[flag_] = dbcArea.LookupEntryForced(area_);
		if(mZoneIDToTable.find(zone_) != mZoneIDToTable.end())
		{
			if(mZoneIDToTable[zone_]->AreaFlags != 312 &&
				mAreaIDToTable[flag_]->AreaFlags == 312)
			{
				// over ride.
				mZoneIDToTable[zone_] = mAreaIDToTable[flag_];
			}
		}
		else
		{
			mZoneIDToTable[zone_] = mAreaIDToTable[flag_];
		}
	}

	new ObjectMgr;
	new QuestMgr;
	new LootMgr;
	new LfgMgr;
	new WeatherMgr;
	new TaxiMgr;
	new AddonMgr;
	new SocialMgr;
	new WorldLog;
	new ChatHandler;

	// grep: this only has to be done once between version updates
	// to re-fill the table.

	/*sLog.outString("Filling spell replacements table...");
	FillSpellReplacementsTable();
	sLog.outString("");*/

#define MAKE_TASK(sp, ptr) tl.AddTask(new Task(new CallbackP0<sp>(sp::getSingletonPtr(), &sp::ptr)))
	// Fill the task list with jobs to do.
	TaskList tl;
	Storage_FillTaskList(tl);

	// spawn worker threads (2 * number of cpus)
	tl.spawn();

	/* storage stuff has to be loaded first */
	tl.wait();

	Storage_LoadAdditionalTables();

	MAKE_TASK(ObjectMgr, LoadPlayerCreateInfo);
	MAKE_TASK(ObjectMgr, LoadPlayersInfo);
	tl.wait();

	MAKE_TASK(ObjectMgr, LoadCreatureWaypoints);
	MAKE_TASK(ObjectMgr, LoadTrainers);
	MAKE_TASK(ObjectMgr, LoadTotemSpells);
	MAKE_TASK(ObjectMgr, LoadSpellSkills);
	MAKE_TASK(ObjectMgr, LoadSpellFixes);
	MAKE_TASK(ObjectMgr, LoadSpellOverride);
	MAKE_TASK(ObjectMgr, LoadVendors);
	MAKE_TASK(ObjectMgr, LoadAIThreatToSpellId);
	MAKE_TASK(ObjectMgr, LoadDefaultPetSpells);
	MAKE_TASK(ObjectMgr, LoadPetSpellCooldowns);
	MAKE_TASK(ObjectMgr, LoadGuildCharters);
	MAKE_TASK(ObjectMgr, LoadGMTickets);
	MAKE_TASK(SocialMgr, LoadFromDB);
	MAKE_TASK(AddonMgr, LoadFromDB);
	MAKE_TASK(ObjectMgr, SetHighestGuids);
	MAKE_TASK(ObjectMgr, LoadReputationModifiers);
	MAKE_TASK(ObjectMgr, LoadMonsterSay);
	MAKE_TASK(WeatherMgr, LoadFromDB);
	MAKE_TASK(ObjectMgr,LoadGroups);

	MAKE_TASK(ObjectMgr, LoadExtraCreatureProtoStuff);
	MAKE_TASK(ObjectMgr, LoadExtraItemStuff);
	MAKE_TASK(QuestMgr, LoadExtraQuestStuff);
	MAKE_TASK(ObjectMgr, LoadArenaTeams);

#undef MAKE_TASK

	// wait for all loading to complete.
	tl.wait();
	sLocalizationMgr.Reload(false);

	CommandTableStorage::getSingleton().Load();
	Log.Notice("WordFilter", "Loading...");
	
	g_characterNameFilter = new WordFilter();
	g_chatFilter = new WordFilter();
	g_characterNameFilter->Load("wordfilter_character_names");
	g_chatFilter->Load("wordfilter_chat");

	Log.Notice("WordFilter", "Done.");

	sLog.outString("");
	Log.Notice("World", "Database loaded in %ums.", getMSTime() - start_time);
	sLog.outString("");

#ifdef COLLISION
	CollideInterface.Init();
#endif

	// calling this puts all maps into our task list.
	sInstanceMgr.Load(&tl);

	// wait for the events to complete.
	tl.wait();

	// wait for them to exit, now.
	tl.kill();
	tl.waitForThreadsToExit();
	sLog.outString("");
	LoadNameGenData();

	Log.Notice("World", "Object size: %u bytes", sizeof(Object));
	Log.Notice("World", "Unit size: %u bytes", sizeof(Unit) + sizeof(AIInterface));
	Log.Notice("World", "Creature size: %u bytes", sizeof(Creature) + sizeof(AIInterface));
	Log.Notice("World", "Player size: %u bytes", sizeof(Player) + sizeof(ItemInterface) + 50000 + 30000 + 1000 + sizeof(AIInterface));
	Log.Notice("World", "GameObject size: %u bytes", sizeof(GameObject));

	//Updating spell.dbc--this is slow like hell due to we cant read string fields
	//dbc method will be changed in future
	DBCFile dbc;

	if( !dbc.open( "DBC/Spell.dbc" ) )
	{
		Log.Error("World", "Cannot find file ./DBC/Spell.dbc" );
		return false;
	}

	Log.Notice("World", "Processing %u spells...", dbc.getRecordCount());

	QueryResult * result = WorldDatabase.Query("SELECT * FROM spell_proc_data");
	map<uint32, pair<uint32, int32> > procMap;
	if(result)
	{
		do 
		{
			pair<uint32,int32> p;
			p.first = result->Fetch()[1].GetUInt32();
			p.second = result->Fetch()[2].GetInt32();
			procMap.insert(make_pair(result->Fetch()[0].GetUInt32(), p));
		} while(result->NextRow());
		delete result;
	}
	uint32 cnt = (uint32)dbc.getRecordCount();
	uint32 effect;
	uint32 All_Seal_Groups_Combined=0;

	map<uint32, uint32> talentSpells;
	map<uint32,uint32>::iterator talentSpellIterator;
	unsigned int i,j;
	for(i = 0; i < dbcTalent.GetNumRows(); ++i)
	{
		TalentEntry * tal = dbcTalent.LookupRow(i);
		for(j = 0; j < 5; ++j)
			if(tal->RankID[j] != 0)
				talentSpells.insert(make_pair(tal->RankID[j], tal->TalentTree));
	}


	for(uint32 x=0; x < cnt; x++)
	{
		uint32 result = 0;
		// SpellID
		uint32 spellid = dbc.getRecord(x).getUInt(0);
		// Description field
		char* desc = (char*)dbc.getRecord(x).getString(157); 
		const char* ranktext = dbc.getRecord(x).getString(140);
		const char* nametext = dbc.getRecord(x).getString(123);

		uint32 rank = 0;
		uint32 type = 0;
		uint32 namehash = 0;

		// get spellentry
		SpellEntry * sp = dbcSpell.LookupEntry(spellid);

		// hash the name
		//!!!!!!! representing all strings on 32 bits is dangerous. There is a chance to get same hash for a lot of strings ;)
        namehash = crc32((const unsigned char*)nametext, (unsigned int)strlen(nametext));
		sp->NameHash   = namehash; //need these set before we start processing spells

		float radius=std::max(::GetRadius(dbcSpellRadius.LookupEntry(sp->EffectRadiusIndex[0])),::GetRadius(dbcSpellRadius.LookupEntry(sp->EffectRadiusIndex[1])));
		radius=std::max(::GetRadius(dbcSpellRadius.LookupEntry(sp->EffectRadiusIndex[2])),radius);
		radius=std::max(GetMaxRange(dbcSpellRange.LookupEntry(sp->rangeIndex)),radius);
		sp->base_range_or_radius_sqr = radius*radius;

		//there are some spells that change the "damage" value of 1 effect to another : devastate = bonus first then damage
		//this is a total bullshit so remove it when spell system supports effect overwriting
		for( uint32 col1_swap = 0; col1_swap < 2 ; col1_swap++ )
			for( uint32 col2_swap = col1_swap ; col2_swap < 3 ; col2_swap++ )
				if( sp->Effect[col1_swap] == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE && sp->Effect[col2_swap] == SPELL_EFFECT_DUMMYMELEE )
				{
					uint32 temp;
					float ftemp;
					temp = sp->Effect[col1_swap];			sp->Effect[col1_swap] = sp->Effect[col2_swap] ;						sp->Effect[col2_swap] = temp;
					temp = sp->EffectDieSides[col1_swap];	sp->EffectDieSides[col1_swap] = sp->EffectDieSides[col2_swap] ;		sp->EffectDieSides[col2_swap] = temp;
					temp = sp->EffectBaseDice[col1_swap];	sp->EffectBaseDice[col1_swap] = sp->EffectBaseDice[col2_swap] ;		sp->EffectBaseDice[col2_swap] = temp;
					ftemp = sp->EffectDicePerLevel[col1_swap];			sp->EffectDicePerLevel[col1_swap] = sp->EffectDicePerLevel[col2_swap] ;				sp->EffectDicePerLevel[col2_swap] = ftemp;
					ftemp = sp->EffectRealPointsPerLevel[col1_swap];	sp->EffectRealPointsPerLevel[col1_swap] = sp->EffectRealPointsPerLevel[col2_swap] ;	sp->EffectRealPointsPerLevel[col2_swap] = ftemp;
					temp = sp->EffectBasePoints[col1_swap];	sp->EffectBasePoints[col1_swap] = sp->EffectBasePoints[col2_swap] ;	sp->EffectBasePoints[col2_swap] = temp;
					temp = sp->EffectMechanic[col1_swap];	sp->EffectMechanic[col1_swap] = sp->EffectMechanic[col2_swap] ;	sp->EffectMechanic[col2_swap] = temp;
					temp = sp->EffectImplicitTargetA[col1_swap];	sp->EffectImplicitTargetA[col1_swap] = sp->EffectImplicitTargetA[col2_swap] ;	sp->EffectImplicitTargetA[col2_swap] = temp;
					temp = sp->EffectImplicitTargetB[col1_swap];	sp->EffectImplicitTargetB[col1_swap] = sp->EffectImplicitTargetB[col2_swap] ;	sp->EffectImplicitTargetB[col2_swap] = temp;
					temp = sp->EffectRadiusIndex[col1_swap];	sp->EffectRadiusIndex[col1_swap] = sp->EffectRadiusIndex[col2_swap] ;	sp->EffectRadiusIndex[col2_swap] = temp;
					temp = sp->EffectApplyAuraName[col1_swap];	sp->EffectApplyAuraName[col1_swap] = sp->EffectApplyAuraName[col2_swap] ;	sp->EffectApplyAuraName[col2_swap] = temp;
					temp = sp->EffectAmplitude[col1_swap];		sp->EffectAmplitude[col1_swap] = sp->EffectAmplitude[col2_swap] ;	sp->EffectAmplitude[col2_swap] = temp;
					ftemp = sp->Effectunknown[col1_swap];		sp->Effectunknown[col1_swap] = sp->Effectunknown[col2_swap] ;	sp->Effectunknown[col2_swap] = ftemp;
					temp = sp->EffectChainTarget[col1_swap];	sp->EffectChainTarget[col1_swap] = sp->EffectChainTarget[col2_swap] ;	sp->EffectChainTarget[col2_swap] = temp;
					temp = sp->EffectSpellGroupRelation[col1_swap];	sp->EffectSpellGroupRelation[col1_swap] = sp->EffectSpellGroupRelation[col2_swap] ;	sp->EffectSpellGroupRelation[col2_swap] = temp;
					temp = sp->EffectMiscValue[col1_swap];		sp->EffectMiscValue[col1_swap] = sp->EffectMiscValue[col2_swap] ;	sp->EffectMiscValue[col2_swap] = temp;
					temp = sp->EffectTriggerSpell[col1_swap];	sp->EffectTriggerSpell[col1_swap] = sp->EffectTriggerSpell[col2_swap] ;	sp->EffectTriggerSpell[col2_swap] = temp;
					ftemp = sp->EffectPointsPerComboPoint[col1_swap];	sp->EffectPointsPerComboPoint[col1_swap] = sp->EffectPointsPerComboPoint[col2_swap] ;	sp->EffectPointsPerComboPoint[col2_swap] = ftemp;
				}

		for(uint32 b=0;b<3;++b)
		{
			if(sp->EffectTriggerSpell[b] != 0 && dbcSpell.LookupEntryForced(sp->EffectTriggerSpell[b]) == NULL)
			{
				/* proc spell referencing non-existant spell. create a dummy spell for use w/ it. */
				CreateDummySpell(sp->EffectTriggerSpell[b]);
			}
			/** Load teaching spells (used for hunters when learning pets wild abilities) */
			if(sp->Effect[b]==SPELL_EFFECT_LEARN_SPELL && sp->EffectImplicitTargetA[b]==EFF_TARGET_PET)
			{
				map<uint32,uint32>::iterator itr = TeachingSpellMap.find(sp->EffectTriggerSpell[b]);
				if(itr == TeachingSpellMap.end())
					TeachingSpellMap.insert(make_pair(sp->EffectTriggerSpell[b],sp->Id));
			}

			if( sp->Attributes & ATTRIBUTES_ONLY_OUTDOORS && sp->EffectApplyAuraName[b] == SPELL_AURA_MOUNTED )
			{
				sp->Attributes &= ~ATTRIBUTES_ONLY_OUTDOORS;
			}
		}

		sp->proc_interval = 0;//trigger at each event
		sp->c_is_flags = 0;
		sp->spell_coef_flags = 0;
		sp->Dspell_coef_override = -1;
		sp->OTspell_coef_override = -1;
		sp->casttime_coef = 0;
		sp->fixed_dddhcoef = -1;
		sp->fixed_hotdotcoef = -1;

		talentSpellIterator = talentSpells.find(sp->Id);
		if(talentSpellIterator == talentSpells.end())
			sp->talent_tree = 0;
		else
			sp->talent_tree = talentSpellIterator->second;

		// parse rank text
		if( !sscanf( ranktext, "Rank %d", (unsigned int*)&rank) )
			rank = 0;

		//seal of light 
		if( namehash == SPELL_HASH_SEAL_OF_LIGHT )			
			sp->procChance = 45;	/* this will do */

		//seal of command
		else if( namehash == SPELL_HASH_SEAL_OF_COMMAND )		
			sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC;

		//judgement of command
		else if( namehash == SPELL_HASH_JUDGEMENT_OF_COMMAND )		
			sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC;

		else if( namehash == SPELL_HASH_ARCANE_SHOT )		
			sp->c_is_flags |= SPELL_FLAG_IS_NOT_USING_DMG_BONUS;

		else if( namehash == SPELL_HASH_SERPENT_STING )		
			sp->c_is_flags |= SPELL_FLAG_IS_NOT_USING_DMG_BONUS;

		//Rogue: Posion time fix for 2.3
		if( strstr( nametext, "Crippling Poison") && sp->Effect[0] == 54 ) //I, II
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Mind-numbing Poison") && sp->Effect[0] == 54 ) //I,II,III
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Instant Poison") && sp->Effect[0] == 54 ) //I,II,III,IV,V,VI,VII    
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Deadly Poison") && sp->Effect[0] == 54 ) //I,II,III,IV,V,VI,VII
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Wound Poison") && sp->Effect[0] == 54 ) //I,II,III,IV,V
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Anesthetic Poison") && sp->Effect[0] == 54 ) //I
			sp->EffectBasePoints[0] = 3599;

        if( strstr( nametext, "Sharpen Blade") && sp->Effect[0] == 54 ) //All BS stones
            sp->EffectBasePoints[0] = 3599;

		//these mostly do not mix so we can use else 
        // look for seal, etc in name
        if( strstr( nametext, "Seal"))
		{
            type |= SPELL_TYPE_SEAL;
			All_Seal_Groups_Combined |= sp->SpellGroupType;
		}
        else if( strstr( nametext, "Blessing"))
            type |= SPELL_TYPE_BLESSING;
        else if( strstr( nametext, "Curse"))
            type |= SPELL_TYPE_CURSE;
        else if( strstr( nametext, "Aspect"))
            type |= SPELL_TYPE_ASPECT;
        else if( strstr( nametext, "Sting") || strstr( nametext, "sting"))
            type |= SPELL_TYPE_STING;
        // don't break armor items!
        else if(strcmp(nametext, "Armor") && strstr( nametext, "Armor") || strstr( nametext, "Demon Skin"))
            type |= SPELL_TYPE_ARMOR;
        else if( strstr( nametext, "Aura"))
            type |= SPELL_TYPE_AURA;
		else if( strstr( nametext, "Track")==nametext)
            type |= SPELL_TYPE_TRACK;
		else if( namehash == SPELL_HASH_GIFT_OF_THE_WILD || namehash == SPELL_HASH_MARK_OF_THE_WILD )
            type |= SPELL_TYPE_MARK_GIFT;
		else if( namehash == SPELL_HASH_IMMOLATION_TRAP || namehash == SPELL_HASH_FREEZING_TRAP || namehash == SPELL_HASH_FROST_TRAP || namehash == SPELL_HASH_EXPLOSIVE_TRAP || namehash == SPELL_HASH_SNAKE_TRAP )
            type |= SPELL_TYPE_HUNTER_TRAP;
		else if( namehash == SPELL_HASH_ARCANE_INTELLECT || namehash == SPELL_HASH_ARCANE_BRILLIANCE )
            type |= SPELL_TYPE_MAGE_INTEL;
		else if( namehash == SPELL_HASH_AMPLIFY_MAGIC || namehash == SPELL_HASH_DAMPEN_MAGIC )
            type |= SPELL_TYPE_MAGE_MAGI;
		else if( namehash == SPELL_HASH_FIRE_WARD || namehash == SPELL_HASH_FROST_WARD )
            type |= SPELL_TYPE_MAGE_WARDS;
		else if( namehash == SPELL_HASH_SHADOW_PROTECTION || namehash == SPELL_HASH_PRAYER_OF_SHADOW_PROTECTION )
            type |= SPELL_TYPE_PRIEST_SH_PPROT;
		else if( namehash == SPELL_HASH_WATER_SHIELD || namehash == SPELL_HASH_EARTH_SHIELD || namehash == SPELL_HASH_LIGHTNING_SHIELD )
            type |= SPELL_TYPE_SHIELD;
		else if( namehash == SPELL_HASH_POWER_WORD__FORTITUDE || namehash == SPELL_HASH_PRAYER_OF_FORTITUDE )
            type |= SPELL_TYPE_FORTITUDE;
		else if( namehash == SPELL_HASH_DIVINE_SPIRIT || namehash == SPELL_HASH_PRAYER_OF_SPIRIT )
            type |= SPELL_TYPE_SPIRIT;
//		else if( strstr( nametext, "Curse of Weakness") || strstr( nametext, "Curse of Agony") || strstr( nametext, "Curse of Recklessness") || strstr( nametext, "Curse of Tongues") || strstr( nametext, "Curse of the Elements") || strstr( nametext, "Curse of Idiocy") || strstr( nametext, "Curse of Shadow") || strstr( nametext, "Curse of Doom"))
//		else if(namehash==4129426293 || namehash==885131426 || namehash==626036062 || namehash==3551228837 || namehash==2784647472 || namehash==776142553 || namehash==3407058720 || namehash==202747424)
//		else if( strstr( nametext, "Curse of "))
//            type |= SPELL_TYPE_WARLOCK_CURSES;
		else if( strstr( nametext, "Immolate") || strstr( nametext, "Conflagrate"))
			type |= SPELL_TYPE_WARLOCK_IMMOLATE;
		else if( strstr( nametext, "Amplify Magic") || strstr( nametext, "Dampen Magic"))
			type |= SPELL_TYPE_MAGE_AMPL_DUMP;
        else if( strstr( desc, "Battle Elixir"))
            type |= SPELL_TYPE_ELIXIR_BATTLE;
        else if( strstr( desc, "Guardian Elixir"))
            type |= SPELL_TYPE_ELIXIR_GUARDIAN;
        else if( strstr( desc, "Battle and Guardian elixir"))
            type |= SPELL_TYPE_ELIXIR_FLASK;
		else if( namehash == SPELL_HASH_HUNTER_S_MARK )		// hunter's mark
			type |= SPELL_TYPE_HUNTER_MARK;
		else if( strstr( desc, "Finishing move")==desc)
			sp->c_is_flags |= SPELL_FLAG_IS_FINISHING_MOVE;
		if( IsDamagingSpell( sp ) )
			sp->c_is_flags |= SPELL_FLAG_IS_DAMAGING;
		if( IsHealingSpell( sp ) )
			sp->c_is_flags |= SPELL_FLAG_IS_HEALING;
		if( IsTargetingStealthed( sp ) )
			sp->c_is_flags |= SPELL_FLAG_IS_TARGETINGSTEALTHED;
		

		//stupid spell ranking problem
		if(sp->spellLevel==0)
		{
			uint32 new_level=0;
			if( strstr( nametext, "Apprentice "))
				new_level = 1;
			else if( strstr( nametext, "Journeyman "))
				new_level = 2;
			else if( strstr( nametext, "Expert "))
				new_level = 3;
			else if( strstr( nametext, "Artisan "))
				new_level = 4;
			else if( strstr( nametext, "Master "))
				new_level = 5;
			if(new_level!=0)
			{
				uint32 teachspell=0;
				if(sp->Effect[0]==SPELL_EFFECT_LEARN_SPELL)
					teachspell = sp->EffectTriggerSpell[0];
				else if(sp->Effect[1]==SPELL_EFFECT_LEARN_SPELL)
					teachspell = sp->EffectTriggerSpell[1];
				else if(sp->Effect[2]==SPELL_EFFECT_LEARN_SPELL)
					teachspell = sp->EffectTriggerSpell[2];
				if(teachspell)
				{
					SpellEntry *spellInfo;
					spellInfo = dbcSpell.LookupEntryForced(teachspell);
					spellInfo->spellLevel = new_level;
					sp->spellLevel = new_level;
				}
			}
		}

		/*FILE * f = fopen("C:\\spells.txt", "a");
		fprintf(f, "case 0x%08X:		// %s\n", namehash, nametext);
		fclose(f);*/

		// find diminishing status
		sp->DiminishStatus = GetDiminishingGroup(namehash);
		sp->buffIndexType=0;
		switch(namehash)
		{
		case SPELL_HASH_HUNTER_S_MARK:		// Hunter's mark
			sp->buffIndexType = SPELL_TYPE_INDEX_MARK;
			break;

		case SPELL_HASH_POLYMORPH:			// Polymorph
		case SPELL_HASH_POLYMORPH__CHICKEN:	// Polymorph: Chicken
		case SPELL_HASH_POLYMORPH__PIG:		// Polymorph: Pig
		case SPELL_HASH_POLYMORPH__SHEEP:	// Polymorph: Sheep
		case SPELL_HASH_POLYMORPH__TURTLE:	// Polymorph: Turtle
			sp->buffIndexType = SPELL_TYPE_INDEX_POLYMORPH;
			break;

		case SPELL_HASH_FEAR:				// Fear
			sp->buffIndexType = SPELL_TYPE_INDEX_FEAR;
			break;

		case SPELL_HASH_SAP:				// Sap
			sp->buffIndexType = SPELL_TYPE_INDEX_SAP;
			break;

		case SPELL_HASH_SCARE_BEAST:		// Scare Beast
			sp->buffIndexType = SPELL_TYPE_INDEX_SCARE_BEAST;
			break;

		case SPELL_HASH_HIBERNATE:			// Hibernate
			sp->buffIndexType = SPELL_TYPE_INDEX_HIBERNATE;
			break;

//		removed by Zack Earth shield stacks 10 times. Current code does not support it
//		case SPELL_HASH_EARTH_SHIELD:		// Earth Shield
//			sp->buffIndexType = SPELL_TYPE_INDEX_EARTH_SHIELD;
//			break;

		case SPELL_HASH_CYCLONE:			// Cyclone
			sp->buffIndexType = SPELL_TYPE_INDEX_CYCLONE;
			break;

		case SPELL_HASH_BANISH:				// Banish
			sp->buffIndexType = SPELL_TYPE_INDEX_BANISH;
			break;

		//case SPELL_HASH_JUDGEMENT_OF_VENGEANCE:
		case SPELL_HASH_JUDGEMENT_OF_THE_CRUSADER:
		case SPELL_HASH_JUDGEMENT_OF_LIGHT:
		case SPELL_HASH_JUDGEMENT_OF_WISDOM:
		case SPELL_HASH_JUDGEMENT_OF_JUSTICE:
			sp->buffIndexType = SPELL_TYPE_INDEX_JUDGEMENT;
			break;
		}

		// HACK FIX: Break roots/fear on damage.. this needs to be fixed properly!
		if(!(sp->AuraInterruptFlags & AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN))
		{
			for(uint32 z = 0; z < 3; ++z) {
				if(sp->EffectApplyAuraName[z] == SPELL_AURA_MOD_FEAR ||
					sp->EffectApplyAuraName[z] == SPELL_AURA_MOD_ROOT)
				{
					sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_UNUSED2;
					break;
				}

				if( ( sp->Effect[z] == SPELL_EFFECT_SCHOOL_DAMAGE && sp->Spell_Dmg_Type == SPELL_DMG_TYPE_MELEE ) || sp->Effect[z] == SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL || sp->Effect[z] == SPELL_EFFECT_WEAPON_DAMAGE || sp->Effect[z] == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE || sp->Effect[z] == SPELL_EFFECT_DUMMYMELEE )
					sp->is_melee_spell = true;
				if( ( sp->Effect[z] == SPELL_EFFECT_SCHOOL_DAMAGE && sp->Spell_Dmg_Type == SPELL_DMG_TYPE_RANGED ) )
				{
					//sLog.outString( "Ranged Spell: %u [%s]" , sp->Id , sp->Name );
					sp->is_ranged_spell = true;
				}
			}
		}

		// set extra properties
		sp->buffType   = type;
		sp->RankNumber = rank;

		uint32 pr=sp->procFlags;
		for(uint32 y=0;y < 3; y++)
		{
			// get the effect number from the spell
			effect = dbc.getRecord(x).getUInt(64 + y); // spelleffect[0] = 64 // 2.0.1

			//spell group
			/*if(effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT1||effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT2||
				effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT3||effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT4)
			{
			
					const char *p=desc;
					while(p=strstr(p,"$"))
					{
						p++;
						//got $  -> check if spell
						if(*p>='0' && *p <='9')
						{//woot this is spell id
							uint32 tmp=atoi(p);
							SpellEntry*s=sSpellStore.LookupEntryForced(tmp);
							bool ch=false;
							for(uint32 i=0;i<3;i++)
								if(s->EffectTriggerSpell[i])
								{
									ch=true;
									result=tmp;
									break;
								}
							if(ch)break;
							result=tmp;
							
						}
					
					}
				
			}else*/
			/*if(effect==SPELL_EFFECT_ENCHANT_ITEM)//add inventory type check
			{
				result=0;
				//136--desc field
				//dirty code
				if( strstr( desc,"head"))
					result|=(1<<INVTYPE_HEAD);
				if( strstr( desc,"leg"))
					result|=(1<<INVTYPE_LEGS);
				if( strstr( desc,"neck"))
					result|=(1<<INVTYPE_NECK);
				if( strstr( desc,"shoulder"))
					result|=(1<<INVTYPE_SHOULDERS);
				if( strstr( desc,"body"))
					result|=(1<<INVTYPE_BODY);
				if( strstr( desc,"chest"))
					result|=((1<<INVTYPE_CHEST)|(1<<INVTYPE_ROBE));
				if( strstr( desc,"waist"))
					result|=(1<<INVTYPE_WAIST);
				if( strstr( desc,"foot")||strstr(desc,"feet")||strstr(desc,"boot"))
					result|=(1<<INVTYPE_FEET);
				if( strstr( desc,"wrist")||strstr(desc,"bracer"))
					result|=(1<<INVTYPE_WRISTS);
				if( strstr( desc,"hand")||strstr(desc,"glove"))
					result|=(1<<INVTYPE_HANDS);
				if( strstr( desc,"finger")||strstr(desc,"ring"))
					result|=(1<<INVTYPE_FINGER);
				if( strstr( desc,"trinket"))
					result|=(1<<INVTYPE_TRINKET);
				if( strstr( desc,"shield"))
					result|=(1<<INVTYPE_SHIELD);
				if( strstr( desc,"cloak"))
					result|=(1<<INVTYPE_CLOAK);
				if( strstr( desc,"robe"))
					result|=(1<<INVTYPE_ROBE);
				//if( strstr( desc,"two")||strstr(desc,"Two"))
				//	result|=(1<<INVTYPE_2HWEAPON);<-handled in subclass
			}
			else*/
			if(effect==SPELL_EFFECT_APPLY_AURA)
			{
				uint32 aura = dbc.getRecord(x).getUInt(94+y); // 58+30+3 = 91
				if( aura == SPELL_AURA_PROC_TRIGGER_SPELL ||
					aura == SPELL_AURA_PROC_TRIGGER_DAMAGE
					)//search for spellid in description
				{
					const char *p=desc;
					while((p=strstr(p,"$")))
					{
						p++;
						//got $  -> check if spell
						if(*p>='0' && *p <='9')
						{//woot this is spell id
						
							result=atoi(p);
						}					
					}
					pr=0;

					uint32 len = (uint32)strlen(desc);
					for(i = 0; i < len; ++i)
						desc[i] = tolower(desc[i]);
					//dirty code for procs, if any1 got any better idea-> u are welcome
					//139944 --- some magic number, it will trigger on all hits etc
						//for seems to be smth like custom check
					if( strstr( desc,"your ranged criticals"))
						pr|=PROC_ON_RANGED_CRIT_ATTACK;
					if( strstr( desc,"chance on hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"takes damage"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"attackers when hit"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"character strikes an enemy"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"strike you with a melee attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"target casts a spell"))
						pr|=PROC_ON_CAST_SPELL;
                    if( strstr( desc,"your harmful spells land"))
                        pr|=PROC_ON_CAST_SPELL;
                    if( strstr( desc,"on spell critical hit"))
                        pr|=PROC_ON_SPELL_CRIT_HIT;
                    if( strstr( desc,"spell critical strikes"))
                        pr|=PROC_ON_SPELL_CRIT_HIT;
                    if( strstr( desc,"being able to resurrect"))
                        pr|=PROC_ON_DIE;
					if( strstr( desc,"any damage caused"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"the next melee attack against the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"when successfully hit"))
						pr|=PROC_ON_MELEE_ATTACK ;
					if( strstr( desc,"an enemy on hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"when it hits"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"when successfully hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"on a successful hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"damage to attacker on hit"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"on a hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"strikes you with a melee attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"when caster takes damage"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"when the caster is using melee attacks"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"when struck in combat") || strstr(desc,"When struck in combat"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"successful melee attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"chance per attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"chance per hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"that strikes a party member"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"when hit by a melee attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"landing a melee critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"your critical strikes"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"whenever you deal ranged damage"))
						pr|=PROC_ON_RANGED_ATTACK;
//					if( strstr( desc,"whenever you deal melee damage"))
					if( strstr( desc,"you deal melee damage"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"your melee attacks"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"damage with your Sword"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"when struck in melee combat"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"any successful spell cast against the priest"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if( strstr( desc,"the next melee attack on the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"striking melee or ranged attackers"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM|PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"when damaging an enemy in melee"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"victim of a critical strike"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
					if( strstr( desc,"on successful melee or ranged attack"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"enemy that strikes you in melee"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"after getting a critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"whenever damage is dealt to you"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"when ranged or melee damage is dealt"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"damaging melee attacks"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"on melee or ranged attack"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"on a melee swing"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"Chance on melee"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"spell criticals against you"))
						pr|=PROC_ON_SPELL_CRIT_HIT_VICTIM;
					if( strstr( desc,"after being struck by a melee or ranged critical hit"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
//					if( strstr( desc,"on a critical hit"))
					if( strstr( desc,"critical hit"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"strikes the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"a spell, melee or ranged attack hits the caster"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"after dealing a critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"each melee or ranged damage hit against the priest"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM|PROC_ON_RANGED_ATTACK_VICTIM;				
					if( strstr( desc, "a chance to deal additional"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc, "chance to get an extra attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc, "melee attacks has"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc, "any damage spell hits a target"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc, "giving each melee attack a chance"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc, "damage when hit"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM; //myabe melee damage ?
					if( strstr( desc, "gives your"))
					{
						if( strstr( desc, "melee"))
							pr|=PROC_ON_MELEE_ATTACK;
						else if( strstr( desc,"sinister strike, backstab, gouge and shiv"))
							pr|=PROC_ON_CAST_SPELL;
						else if( strstr( desc,"chance to daze the target"))
							pr|=PROC_ON_CAST_SPELL;
						else if( strstr( desc,"finishing moves"))
							pr|=PROC_ON_CAST_SPELL;
//						else if( strstr( desc,"shadow bolt, shadowburn, soul fire, incinerate, searing pain and conflagrate"))
//							pr|=PROC_ON_CAST_SPELL|PROC_TARGET_SELF;
						//we should find that specific spell (or group) on what we will trigger
						else pr|=PROC_ON_CAST_SPECIFIC_SPELL;
					}
					if( strstr( desc, "chance to add an additional combo") && strstr(desc, "critical") )
						pr|=PROC_ON_CRIT_ATTACK;
					else if( strstr( desc, "chance to add an additional combo"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc, "victim of a melee or ranged critical strike"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
					if( strstr( desc, "getting a critical effect from"))
						pr|=PROC_ON_SPELL_CRIT_HIT_VICTIM;
					if( strstr( desc, "damaging attack is taken"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc, "struck by a Stun or Immobilize"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if( strstr( desc, "melee critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( nametext, "Bloodthirst"))
						pr|=PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
					if( strstr( desc, "experience or honor"))
						pr|=PROC_ON_GAIN_EXPIERIENCE;
					if( strstr( desc,"your next offensive ability"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"hit by a melee or ranged attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"enemy strikes the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"melee and ranged attacks against you"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"when a block occurs"))
						pr|=PROC_ON_BLOCK_VICTIM;
					if( strstr( desc,"dealing a critical strike from a weapon swing, spell, or ability"))
						pr|=PROC_ON_CRIT_ATTACK|PROC_ON_SPELL_CRIT_HIT;
					if( strstr( desc,"dealing a critical strike from a weapon swing, spell, or ability"))
						pr|=PROC_ON_CRIT_ATTACK|PROC_ON_SPELL_CRIT_HIT;
					if( strstr( desc,"shadow bolt critical strikes increase shadow damage"))
						pr|=PROC_ON_SPELL_CRIT_HIT;
					if( strstr( desc,"next offensive ability"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"after being hit with a shadow or fire spell"))
						pr|=PROC_ON_SPELL_LAND_VICTIM;
					if( strstr( desc,"giving each melee attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"each strike has"))
						pr|=PROC_ON_MELEE_ATTACK;		
					if( strstr( desc,"your Fire damage spell hits"))
						pr|=PROC_ON_CAST_SPELL;		//this happens only on hit ;)
					if( strstr( desc,"corruption, curse of agony, siphon life and seed of corruption spells also cause"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"pain, mind flay and vampiric touch spells also cause"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"shadow damage spells have"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"on successful spellcast"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"your spell criticals have"))
						pr|=PROC_ON_SPELL_CRIT_HIT | PROC_ON_SPELL_CRIT_HIT_VICTIM;
					if( strstr( desc,"after dodging their attack"))
					{
						pr|=PROC_ON_DODGE_VICTIM;
						if( strstr( desc,"add a combo point"))
							pr|=PROC_TARGET_SELF;
					}
					if( strstr( desc,"fully resisting"))
						pr|=PROC_ON_RESIST_VICTIM;
					if( strstr( desc,"Your Shadow Word: Pain, Mind Flay and Vampiric Touch spells also cause the target"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"your melee and ranged attacks"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
//					if( strstr( desc,"chill effect to your Blizzard"))
//						pr|=PROC_ON_CAST_SPELL;	
					//////////////////////////////////////////////////
					//proc dmg flags
					//////////////////////////////////////////////////
					if( strstr( desc,"each attack blocked"))
						pr|=PROC_ON_BLOCK_VICTIM;
					if( strstr( desc,"into flame, causing an additional"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"victim of a critical melee strike"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
					if( strstr( desc,"damage to melee attackers"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"target blocks a melee attack"))
						pr|=PROC_ON_BLOCK_VICTIM;
					if( strstr( desc,"ranged and melee attacks to deal"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"damage on hit"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"chance on hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"after being hit by any damaging attack"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"striking melee or ranged attackers"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"damage to attackers when hit"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"striking melee attackers"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"whenever the caster takes damage"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"damage on every attack"))
						pr|=PROC_ON_MELEE_ATTACK | PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"chance to reflect Fire spells"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if( strstr( desc,"hunter takes on the aspects of a hawk"))
						pr|=PROC_TARGET_SELF | PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"successful auto shot attacks"))
						pr|=PROC_ON_AUTO_SHOT_HIT;
					if( strstr( desc,"after getting a critical effect from your"))
						pr=PROC_ON_SPELL_CRIT_HIT;
//					if( strstr( desc,"Your critical strikes from Fire damage"))
//						pr|=PROC_ON_SPELL_CRIT_HIT;
				}//end "if procspellaura"
				//dirty fix to remove auras that should expire on event and they are not
//				else if(sp->procCharges>0)
//				{
					//there are at least 185 spells that should loose charge uppon some event.Be prepared to add more here !
					// ! watch it cause this might conflict with our custom modified spells like : lighning shield !

					//spells like : Presence of Mind,Nature's Swiftness, Inner Focus,Amplify Curse,Coup de Grace
					//SELECT * FROM dbc_spell where proc_charges!=0 and (effect_aura_1=108 or effect_aura_2=108 and effect_aura_3=108) and description!=""
//					if(aura == SPELL_AURA_ADD_PCT_MODIFIER)
//						sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_CAST_SPELL;
					//most of them probably already have these flags...not sure if we should add to all of them without checking
/*					if( strstr( desc, "melee"))
						sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_START_ATTACK;
					if( strstr( desc, "ranged"))
						sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_START_ATTACK;*/
//				}
			}//end "if aura"
		}//end "for each effect"
		sp->procFlags = pr;

		if( strstr( desc, "Must remain seated"))
		{
			sp->RecoveryTime = 1000;
			sp->CategoryRecoveryTime = 1000;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// procintervals
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//omg lighning shield trigger spell id's are all wrong ?
		//if you are bored you could make thiese by hand but i guess we might find other spells with this problem..and this way it's safe
		if( strstr( nametext, "Lightning Shield" ) && sp->EffectTriggerSpell[0] )
		{
			//check if we can find in the desription
			char *startofid = strstr(desc, "for $");
			if( startofid )
			{
				startofid += strlen("for $");
				sp->EffectTriggerSpell[0] = atoi( startofid ); //get new lightning shield trigger id
			}
			sp->proc_interval = 3000; //few seconds
		}
		//mage ignite talent should proc only on some chances
		else if( strstr( nametext, "Ignite") && sp->Id>=11119 && sp->Id<=12848 && sp->EffectApplyAuraName[0] == 4 )
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "an additional ");
			if(startofid)
			{
				startofid += strlen("an additional ");
				sp->EffectBasePoints[0]=atoi(startofid); //get new value. This is actually level*8 ;)
			}
			sp->Effect[0] = 6; //aura
			sp->EffectApplyAuraName[0] = 42; //force him to use procspell effect
			sp->EffectTriggerSpell[0] = 12654; //evil , but this is good for us :D
			sp->procFlags = PROC_ON_SPELL_CRIT_HIT; //add procflag here since this was not processed with the others !
		}
		// Winter's Chill handled by frost school
		else if( strstr( nametext, "Winter's Chill"))
		{
			sp->School = 4;
		}
		// Blackout handled by Shadow school
		else if( strstr( nametext, "Blackout"))
		{
			sp->School = 5;
		}
#ifndef NEW_PROCFLAGS
		// Shadow Weaving
		else if( strstr( nametext, "Shadow Weaving"))
		{
			sp->School = 5;
			sp->EffectApplyAuraName[0] = 42;
			sp->procChance = sp->EffectBasePoints[0] + 1;
			sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
		}
#endif
		//Improved Aspect of the Hawk
		else if( strstr( nametext, "Improved Aspect of the Hawk"))
			sp->EffectSpellGroupRelation[1] = 0x00100000;
		//more triggered spell ids are wrong. I think blizz is trying to outsmart us :S
		else if( strstr( nametext, "Nature's Guardian"))
		{
			sp->EffectTriggerSpell[0] = 31616;
			sp->proc_interval = 5000;
		}
		//Chain Heal all ranks %50 heal value (49 + 1)
		else if( strstr( nametext, "Chain Heal"))
		{
			sp->EffectDieSides[0] = 49;
		}
		//this starts to be an issue for trigger spell id : Deep Wounds
		else if( strstr( nametext, "Deep Wounds") && sp->EffectTriggerSpell[0])
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "over $");
			if(startofid)
			{
				startofid += strlen("over $");
				sp->EffectTriggerSpell[0] = atoi(startofid);
			}
		}
		else if( strstr( nametext, "Holy Shock"))
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "causing $");
			if(startofid)
			{
				startofid += strlen("causing $");
				sp->EffectTriggerSpell[0] = atoi(startofid);
			}
			//check if we can find in the desription
			startofid=strstr(desc, " or $");
			if(startofid)
			{
				startofid += strlen(" or $");
				sp->EffectTriggerSpell[1]=atoi(startofid);
			}
		}
		else if( strstr( nametext, "Touch of Weakness"))
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "cause $");
			if(startofid)
			{
				startofid += strlen("cause $");
				sp->EffectTriggerSpell[0] = atoi(startofid);
				sp->EffectTriggerSpell[1]=sp->EffectTriggerSpell[0]; //later versions of this spell changed to eff[1] the aura
				sp->procFlags = uint32(PROC_ON_MELEE_ATTACK_VICTIM);
			}
		}
		else if( strstr( nametext, "Firestone Passive"))
		{
			//Enchants the main hand weapon with fire, granting each attack a chance to deal $17809s1 additional fire damage.
			//check if we can find in the desription
			char * startofid=strstr(desc, "to deal $");
			if(startofid)
			{
				startofid += strlen("to deal $");
				sp->EffectTriggerSpell[0] = atoi(startofid);
				sp->EffectApplyAuraName[0] = 42;
				sp->procFlags = PROC_ON_MELEE_ATTACK;
				sp->procChance = 50;
			}
		}
		//some procs trigger at intervals
		else if( strstr( nametext, "Water Shield"))
		{
			sp->proc_interval = 3000; //few seconds
			sp->procFlags |= PROC_TARGET_SELF;
		}
		else if( strstr( nametext, "Earth Shield"))
			sp->proc_interval = 3000; //few seconds
		else if( strstr( nametext, "Shadowguard"))
			sp->proc_interval = 3000; //few seconds
		else if( strstr( nametext, "Poison Shield"))
			sp->proc_interval = 3000; //few seconds
		else if( strstr( nametext, "Infused Mushroom"))
			sp->proc_interval = 10000; //10 seconds
		else if( strstr( nametext, "Aviana's Purpose"))
			sp->proc_interval = 10000; //10 seconds
		//don't change to namehash since we are searching only a protion of the name
 		else if( strstr( nametext, "Crippling Poison"))
		{
			sp->SpellGroupType |= 16384; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}
		else if( strstr( nametext, "Mind-Numbing Poison"))
		{
			sp->SpellGroupType |= 32768; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}
		else if( strstr( nametext, "Instant Poison"))
		{
			sp->SpellGroupType |= 8192; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}
		else if( strstr( nametext, "Deadly Poison"))
		{
			sp->SpellGroupType |= 65536; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}
		else if( strstr( nametext, "Wound Poison"))
		{
			sp->SpellGroupType |= 268435456; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}

		//warlock - shadow bolt
		if( sp->NameHash == SPELL_HASH_SHADOW_BOLT )
			sp->SpellGroupType |= 1; //some of them do have the flags but i's hard to write down those some from 130 spells

		//mage Ice Floes affects these spells : Cone of Cold,Cold Snap,Ice Barrier,Ice Block
		if( sp->NameHash == SPELL_HASH_CONE_OF_COLD || sp->NameHash == SPELL_HASH_COLD_SNAP || sp->NameHash == SPELL_HASH_ICE_BARRIER || sp->NameHash == SPELL_HASH_ICE_BLOCK )
			sp->EffectSpellGroupRelation[0] = 0x00200000;

/*		else if( strstr( nametext, "Anesthetic Poison"))
			sp->SpellGroupType |= 0; //not yet known ? 
		else if( strstr( nametext, "Blinding Powder"))
			sp->SpellGroupType |= 0; //not yet known ?*/
//		else if( strstr( nametext, "Illumination"))
//			sp->EffectTriggerSpell[0] = 20272;
		//sp->dummy=result;
/*		//if there is a proc spell and has 0 as charges then it's probably going to triger infinite times. Better not save these
		if(sp->procCharges==0)
			sp->procCharges=-1;*/

		//Set Silencing spells mech.
		if( sp->EffectApplyAuraName[0] == 27 || 
			sp->EffectApplyAuraName[1] == 27 ||
			sp->EffectApplyAuraName[2] == 27 )
			sp->MechanicsType = MECHANIC_SILENCED;

		//Set Stunning spells mech.
		if( sp->EffectApplyAuraName[0] == 12 || 
			sp->EffectApplyAuraName[1] == 12 ||
			sp->EffectApplyAuraName[2] == 12 )
			sp->MechanicsType = MECHANIC_STUNNED;

		//Set Fearing spells mech
		if( sp->EffectApplyAuraName[0] == 7 || 
			sp->EffectApplyAuraName[1] == 7 ||
			sp->EffectApplyAuraName[2] == 7 )
			sp->MechanicsType = MECHANIC_FLEEING;

		if( sp->proc_interval != 0 )
			sp->procFlags |= PROC_REMOVEONUSE;

		// Seal of Command - Proc Chance
		if( sp->NameHash == SPELL_HASH_SEAL_OF_COMMAND )
		{
			sp->procChance = 25;
			sp->School = SCHOOL_HOLY; //the procspells of the original seal of command have fizical school instead of holy
			sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC; //heh, crazy spell uses melee/ranged/magic dmg type for 1 spell. Now which one is correct ?
		}

		//Seal of Jusice - Proc Chance
		if( sp->NameHash == SPELL_HASH_SEAL_OF_JUSTICE )
			sp->procChance = 25;

		/* Decapitate */
		if( sp->NameHash == SPELL_HASH_DECAPITATE )
			sp->procChance = 30;

		//shaman - shock, has no spellgroup.very dangerous move !
		if( sp->NameHash == SPELL_HASH_SHOCK )
			sp->SpellGroupType = 4;

		//mage - fireball. Only some of the spell has the flags 
		if( sp->NameHash == SPELL_HASH_FIREBALL )
			sp->SpellGroupType |= 1;

		if( sp->NameHash == SPELL_HASH_DIVINE_SHIELD || sp->NameHash == SPELL_HASH_DIVINE_PROTECTION || sp->NameHash == SPELL_HASH_BLESSING_OF_PROTECTION )
			sp->MechanicsType = 25;

		/* hackfix for this - FIX ME LATER - Burlex */
		if( namehash == SPELL_HASH_SEAL_FATE )
			sp->procFlags = 0;

		map< uint32, pair< uint32, int32 > >::iterator itr = procMap.find( namehash );
		if( itr != procMap.end())
		{
			if( itr->second.second > 0 )
				sp->procFlags = (uint32)itr->second.second;
			if( itr->first != 0 )
				sp->procChance = itr->first;
		}

		if(
			((sp->Attributes & ATTRIBUTES_TRIGGER_COOLDOWN) && (sp->AttributesEx & ATTRIBUTESEX_NOT_BREAK_STEALTH)) //rogue cold blood
			|| ((sp->Attributes & ATTRIBUTES_TRIGGER_COOLDOWN) && (!sp->AttributesEx || sp->AttributesEx & ATTRIBUTESEX_REMAIN_OOC))
			)
		{
			sp->c_is_flags |= SPELL_FLAG_IS_REQUIRECOOLDOWNUPDATE;
		}

		if( namehash == SPELL_HASH_SHRED || namehash == SPELL_HASH_BACKSTAB || namehash == SPELL_HASH_AMBUSH )
		{
			// Shred, Backstab, Ambush
			sp->in_front_status = 2;
		}

//junk code to get me has :P 
//if(sp->Id==11267 || sp->Id==11289 || sp->Id==6409)
//	printf("!!!!!!! name %s , id %u , hash %u \n",nametext,sp->Id, namehash);
	}


	/////////////////////////////////////////////////////////////////
	//SPELL COEFFICIENT SETTINGS START
	//////////////////////////////////////////////////////////////////

	for(uint32 x=0; x < cnt; x++)
	{
		// SpellID
		uint32 spellid = dbc.getRecord(x).getUInt(0);
		// get spellentry
		SpellEntry * sp = dbcSpell.LookupEntry(spellid);

		//Setting Cast Time Coefficient
		SpellCastTime *sd = dbcSpellCastTime.LookupEntry(sp->CastingTimeIndex);
		float castaff = float(GetCastTime(sd));
		if(castaff < 1500) castaff = 1500;
		else
			if(castaff > 7000) castaff = 7000;

		sp->casttime_coef = castaff / 3500;		 

		SpellEntry * spz;
		bool spcheck = false;

		//Flag for DoT and HoT
		for( uint8 i = 0 ; i < 3 ; i++ )
		{
			if (sp->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_DAMAGE ||
				sp->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_HEAL ||
				sp->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_LEECH )
			{
				sp->spell_coef_flags |= SPELL_FLAG_IS_DOT_OR_HOT_SPELL;
				break;
			}
		}

		//Flag for DD or DH
		for( uint8 i = 0 ; i < 3 ; i++ )
		{
			if ( sp->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_TRIGGER_SPELL && sp->EffectTriggerSpell[i] )
			{
				spz = dbcSpell.LookupEntry( sp->EffectTriggerSpell[i] );
				if( spz &&
					spz->Effect[i] == SPELL_EFFECT_SCHOOL_DAMAGE ||
					spz->Effect[i] == SPELL_EFFECT_HEAL
					) 
					spcheck = true;
			}
			if (sp->Effect[i] == SPELL_EFFECT_SCHOOL_DAMAGE ||
				sp->Effect[i] == SPELL_EFFECT_HEAL ||
				spcheck
				)
			{
				sp->spell_coef_flags |= SPELL_FLAG_IS_DD_OR_DH_SPELL;
				break;
			}
		}

		for(uint8 i = 0 ; i < 3; i++)
		{
			switch (sp->EffectImplicitTargetA[i])
			{
				//AoE
			case EFF_TARGET_ALL_TARGETABLE_AROUND_LOCATION_IN_RADIUS:
			case EFF_TARGET_ALL_ENEMY_IN_AREA:
			case EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT:
			case EFF_TARGET_ALL_PARTY_AROUND_CASTER:
			case EFF_TARGET_ALL_ENEMIES_AROUND_CASTER:
			case EFF_TARGET_IN_FRONT_OF_CASTER:
			case EFF_TARGET_ALL_ENEMY_IN_AREA_CHANNELED:
			case EFF_TARGET_ALL_PARTY_IN_AREA_CHANNELED:
			case EFF_TARGET_ALL_FRIENDLY_IN_AREA:
			case EFF_TARGET_ALL_TARGETABLE_AROUND_LOCATION_IN_RADIUS_OVER_TIME:
			case EFF_TARGET_ALL_PARTY:
			case EFF_TARGET_LOCATION_INFRONT_CASTER:
			case EFF_TARGET_BEHIND_TARGET_LOCATION:
			case EFF_TARGET_LOCATION_INFRONT_CASTER_AT_RANGE:
				{
					sp->spell_coef_flags |= SPELL_FLAG_AOE_SPELL;
					break;
				}
			}	
		}

		for(uint8 i = 0 ; i < 3 ; i++)
		{
			switch (sp->EffectImplicitTargetB[i])
			{
				//AoE
			case EFF_TARGET_ALL_TARGETABLE_AROUND_LOCATION_IN_RADIUS:
			case EFF_TARGET_ALL_ENEMY_IN_AREA:
			case EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT:
			case EFF_TARGET_ALL_PARTY_AROUND_CASTER:
			case EFF_TARGET_ALL_ENEMIES_AROUND_CASTER:
			case EFF_TARGET_IN_FRONT_OF_CASTER:
			case EFF_TARGET_ALL_ENEMY_IN_AREA_CHANNELED:
			case EFF_TARGET_ALL_PARTY_IN_AREA_CHANNELED:
			case EFF_TARGET_ALL_FRIENDLY_IN_AREA:
			case EFF_TARGET_ALL_TARGETABLE_AROUND_LOCATION_IN_RADIUS_OVER_TIME:
			case EFF_TARGET_ALL_PARTY:
			case EFF_TARGET_LOCATION_INFRONT_CASTER:
			case EFF_TARGET_BEHIND_TARGET_LOCATION:
			case EFF_TARGET_LOCATION_INFRONT_CASTER_AT_RANGE:
				{
					sp->spell_coef_flags |= SPELL_FLAG_AOE_SPELL;
					break;
				}
			}	
		}

		//Special Cases
		//Holy Light & Flash of Light
		if(sp->NameHash == SPELL_HASH_HOLY_LIGHT ||
			sp->NameHash == SPELL_HASH_FLASH_OF_LIGHT)
			sp->spell_coef_flags |= SPELL_FLAG_IS_DD_OR_DH_SPELL;

		//Additional Effect (not healing or damaging)
		for( uint8 i = 0 ; i < 3 ; i++ )
		{
			if(sp->Effect[i] == 0)
				continue;

			switch (sp->Effect[i])
			{
			case SPELL_EFFECT_SCHOOL_DAMAGE:
			case SPELL_EFFECT_ENVIRONMENTAL_DAMAGE:
			case SPELL_EFFECT_HEALTH_LEECH:
			case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
			case SPELL_EFFECT_ADD_EXTRA_ATTACKS:
			case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
			case SPELL_EFFECT_POWER_BURN:
			case SPELL_EFFECT_ATTACK:
			case SPELL_EFFECT_HEAL:
			case SPELL_EFFECT_HEALTH_FUNNEL:
			case SPELL_EFFECT_HEAL_MAX_HEALTH:
			case SPELL_EFFECT_DUMMY:
				continue;
			}

			switch (sp->EffectApplyAuraName[i])
			{
			case SPELL_AURA_PERIODIC_DAMAGE:
			case SPELL_AURA_PROC_TRIGGER_DAMAGE:
			case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
			case SPELL_AURA_POWER_BURN:
			case SPELL_AURA_PERIODIC_HEAL:
			case SPELL_AURA_MOD_INCREASE_HEALTH:
			case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
			case SPELL_AURA_DUMMY:
				continue;
			}

			sp->spell_coef_flags |= SPELL_FLAG_ADITIONAL_EFFECT;
			break;

		}

		//Calculating fixed coeficients
		//Channeled spells
		if( sp->ChannelInterruptFlags != 0 )
		{
			float Duration = float( GetDuration( dbcSpellDuration.LookupEntry( sp->DurationIndex ) ));
			if(Duration < 1500) Duration = 1500;
			else if(Duration > 7000) Duration = 7000;
			sp->fixed_hotdotcoef = (Duration / 3500.0f);

			if( sp->spell_coef_flags & SPELL_FLAG_ADITIONAL_EFFECT )
				sp->fixed_hotdotcoef *= 0.95f;
			if( sp->spell_coef_flags & SPELL_FLAG_AOE_SPELL )
				sp->fixed_hotdotcoef *= 0.5f;
		}

		//Standard spells
		else if( (sp->spell_coef_flags & SPELL_FLAG_IS_DD_OR_DH_SPELL) && !(sp->spell_coef_flags & SPELL_FLAG_IS_DOT_OR_HOT_SPELL) )
		{
			sp->fixed_dddhcoef = sp->casttime_coef;
			if( sp->spell_coef_flags & SPELL_FLAG_ADITIONAL_EFFECT )
				sp->fixed_dddhcoef *= 0.95f;
			if( sp->spell_coef_flags & SPELL_FLAG_AOE_SPELL )
				sp->fixed_dddhcoef *= 0.5f;
		}

		//Over-time spells
		else if( !(sp->spell_coef_flags & SPELL_FLAG_IS_DD_OR_DH_SPELL) && (sp->spell_coef_flags & SPELL_FLAG_IS_DOT_OR_HOT_SPELL) )
		{
			float Duration = float( GetDuration( dbcSpellDuration.LookupEntry( sp->DurationIndex ) ));
			sp->fixed_hotdotcoef = (Duration / 15000.0f);

			if( sp->spell_coef_flags & SPELL_FLAG_ADITIONAL_EFFECT )
				sp->fixed_hotdotcoef *= 0.95f;
			if( sp->spell_coef_flags & SPELL_FLAG_AOE_SPELL )
				sp->fixed_hotdotcoef *= 0.5f;

		}

		//Combined standard and over-time spells
		else if( sp->spell_coef_flags & SPELL_FLAG_IS_DD_DH_DOT_SPELL )
		{
			float Duration = float( GetDuration( dbcSpellDuration.LookupEntry( sp->DurationIndex ) ));
			float Portion_to_Over_Time = (Duration / 15000.0f) / ((Duration / 15000.0f) + sp->casttime_coef );
			float Portion_to_Standard = 1.0f - Portion_to_Over_Time;

			sp->fixed_dddhcoef = sp->casttime_coef * Portion_to_Standard;
			sp->fixed_hotdotcoef = (Duration / 15000.0f) * Portion_to_Over_Time;

			if( sp->spell_coef_flags & SPELL_FLAG_ADITIONAL_EFFECT )
			{
				sp->fixed_dddhcoef *= 0.95f;
				sp->fixed_hotdotcoef *= 0.95f;
			}
			if( sp->spell_coef_flags & SPELL_FLAG_AOE_SPELL )
			{
				sp->fixed_dddhcoef *= 0.5f;
				sp->fixed_hotdotcoef *= 0.5f;
			}		
		}
	}

	//Settings for special cases
	QueryResult * resultx = WorldDatabase.Query("SELECT * FROM spell_coef_override");
	if( resultx != NULL )
	{
		do 
		{
			Field * f;
			f = resultx->Fetch();
			SpellEntry * sp = dbcSpell.LookupEntryForced( f[0].GetUInt32() );
			if( sp != NULL )
			{
				sp->Dspell_coef_override = f[2].GetFloat();
				sp->OTspell_coef_override = f[3].GetFloat();
			}
		} while( resultx->NextRow() );
		delete resultx;
	}

	//Fully loaded coefficients, we must share channeled coefficient to its triggered spells
	for(uint32 x=0; x < cnt; x++)
	{
		// SpellID
		uint32 spellid = dbc.getRecord(x).getUInt(0);
		// get spellentry
		SpellEntry * sp = dbcSpell.LookupEntry(spellid);
		SpellEntry * spz;

		//Case SPELL_AURA_PERIODIC_TRIGGER_SPELL
		for( uint8 i = 0 ; i < 3 ; i++ )
		{
			if ( sp->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_TRIGGER_SPELL )
			{
				spz = dbcSpell.LookupEntry( sp->EffectTriggerSpell[i] );
				if( spz != NULL ) 
				{
					if( sp->Dspell_coef_override >= 0 )
						spz->Dspell_coef_override = sp->Dspell_coef_override;
					else
					{
						//we must set bonus per tick on triggered spells now (i.e. Arcane Missiles)
						if( sp->ChannelInterruptFlags != 0 )
						{
							float Duration = float( GetDuration( dbcSpellDuration.LookupEntry( sp->DurationIndex ) ));
							float amp = float(sp->EffectAmplitude[i]);
							sp->fixed_dddhcoef = sp->fixed_hotdotcoef * amp / Duration;
						}			
						spz->fixed_dddhcoef = sp->fixed_dddhcoef;
					}

					if( sp->OTspell_coef_override >= 0 )
						spz->OTspell_coef_override = sp->OTspell_coef_override;
					else
					{
						//we must set bonus per tick on triggered spells now (i.e. Arcane Missiles)
						if( sp->ChannelInterruptFlags != 0 )
						{
							float Duration = float( GetDuration( dbcSpellDuration.LookupEntry( sp->DurationIndex ) ));
							float amp = float(sp->EffectAmplitude[i]);
							sp->fixed_hotdotcoef *= amp / Duration;
						}
						spz->fixed_hotdotcoef = sp->fixed_hotdotcoef;
					}
					break;
				}
			}
		}
	}	

	/////////////////////////////////////////////////////////////////
	//SPELL COEFFICIENT SETTINGS END
	/////////////////////////////////////////////////////////////////

	//this is so lame : shamanistic rage triggers a new spell which borrows it's stats from parent spell :S
	SpellEntry*  parentsp = dbcSpell.LookupEntryForced( 30823 );
	SpellEntry* triggersp = dbcSpell.LookupEntryForced( 30824 );

	if( parentsp != NULL && triggersp != NULL )
		triggersp->EffectBasePoints[0] = parentsp->EffectBasePoints[0];

	/// Elemental Focus
	SpellEntry* sp = dbcSpell.LookupEntryForced( 16164 );
	if( sp != NULL && sp->Id == 16164 )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;

	//remove stormstrike effect 0
	sp = dbcSpell.LookupEntryForced( 17364 );
	if( sp != NULL && sp->Id == 17364 )
		sp->Effect[0] = 0;

	//Warlock: Nether Protection
	sp = dbcSpell.LookupEntryForced( 30299 );
	if (sp != NULL)
		sp->procChance = 10;
	sp = dbcSpell.LookupEntryForced( 30301 );
	if (sp != NULL)
		sp->procChance = 20;
	sp = dbcSpell.LookupEntryForced( 30302 );
	if (sp != NULL)
		sp->procChance = 30;

	//Warlock: Backlash
	sp = dbcSpell.LookupEntryForced( 34935 );
	if (sp != NULL)
		sp->procFlags |= PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 34938 );
	if (sp != NULL)
		sp->procFlags |= PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 34939 );
	if (sp != NULL)
		sp->procFlags |= PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 34936 );
	if (sp != NULL)
	{
		sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_CAST_SPELL;
		sp->EffectSpellGroupRelation[0] = 1 ;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}

	//Warlock: Demonic Knowledge
    sp = dbcSpell.LookupEntryForced( 35691 );
    if( sp != NULL )
	{
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
        sp->Effect[1] = 6;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_DONE;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectImplicitTargetA[1]= EFF_TARGET_PET;
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 35696;
		sp->EffectImplicitTargetA[2]=EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
    }
    sp = dbcSpell.LookupEntryForced( 35692 );
    if( sp != NULL )
	{
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
        sp->Effect[1] = 6;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_DONE;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 35696;
		sp->EffectImplicitTargetA[2] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
    }
    sp = dbcSpell.LookupEntryForced( 35693 );
    if( sp != NULL )
	{
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
        sp->Effect[1] = 6;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_DONE;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 35696;
		sp->EffectImplicitTargetA[2] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
    }
    sp = dbcSpell.LookupEntryForced( 35696 );
    if( sp != NULL )
	{
        sp->Effect[0] = 6; //making this only for the visible effect
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY; //no effect here
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

    //Priest - Holy Nova
    sp = dbcSpell.LookupEntryForced( 15237 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 23455;
    }
    sp = dbcSpell.LookupEntryForced( 15430 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 23458;
    }
    sp = dbcSpell.LookupEntryForced( 15431 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 23459;
    }
    sp = dbcSpell.LookupEntryForced( 27799 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 27803;
    }
    sp = dbcSpell.LookupEntryForced( 27800 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 27804;
    }
    sp = dbcSpell.LookupEntryForced( 27801 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 27805;
    }
    sp = dbcSpell.LookupEntryForced( 25331 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 25329;
    }

	// Moroes' garrote targets a single enemy instead of us
	sp = dbcSpell.LookupEntryForced( 37066 );
    if( sp != NULL )
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
    }

	//Bloodlust targets sorounding party members instead of us
	sp = dbcSpell.LookupEntryForced( 2825 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2] = 0;
		sp->EffectImplicitTargetB[0] = 0;
		sp->EffectImplicitTargetB[1] = 0;
		sp->EffectImplicitTargetB[2] = 0;
	}

	//Heroism targets sorounding party members instead of us
	sp = dbcSpell.LookupEntryForced( 32182 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2] = 0;
		sp->EffectImplicitTargetB[0] = 0;
		sp->EffectImplicitTargetB[1] = 0;
		sp->EffectImplicitTargetB[2] = 0;
	}
	// Drums of war targets sorounding party members instead of us
	sp = dbcSpell.LookupEntryForced( 35475 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2] = 0;
		sp->EffectImplicitTargetB[0] = 0;
		sp->EffectImplicitTargetB[1] = 0;
		sp->EffectImplicitTargetB[2] = 0;
	}

	// Symbol of Hope targets sorounding party members instead of us
	sp = dbcSpell.LookupEntryForced( 32548 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2] = 0;
		sp->EffectImplicitTargetB[0] = 0;
		sp->EffectImplicitTargetB[1] = 0;
		sp->EffectImplicitTargetB[2] = 0;
	}

	// Drums of Battle targets sorounding party members instead of us
	sp = dbcSpell.LookupEntryForced( 35476 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2] = 0;
		sp->EffectImplicitTargetB[0] = 0;
		sp->EffectImplicitTargetB[1] = 0;
		sp->EffectImplicitTargetB[2] = 0;
	}

	// Drums of Panic targets sorounding creatures instead of us
	sp = dbcSpell.LookupEntryForced( 35474 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
		sp->EffectImplicitTargetA[2] = 0;
		sp->EffectImplicitTargetB[0] = 0;
		sp->EffectImplicitTargetB[1] = 0;
		sp->EffectImplicitTargetB[2] = 0;
	}

	// Drums of Restoration targets sorounding party members instead of us
	sp = dbcSpell.LookupEntryForced( 35478 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2] = 0;
		sp->EffectImplicitTargetB[0] = 0;
		sp->EffectImplicitTargetB[1] = 0;
		sp->EffectImplicitTargetB[2] = 0;
	}
	// Drums of Speed targets sorounding party members instead of us
	sp = dbcSpell.LookupEntryForced( 35477 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2] = 0;
		sp->EffectImplicitTargetB[0] = 0;
		sp->EffectImplicitTargetB[1] = 0;
		sp->EffectImplicitTargetB[2] = 0;
	}

	//paladin - Blessing of Light. Changed to scripted because it needs to mod target and should not influence holy nova
	sp = dbcSpell.LookupEntryForced( 19977 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
		sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
	}
	sp = dbcSpell.LookupEntryForced( 19978 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
		sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
	}
	sp = dbcSpell.LookupEntryForced( 19979 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
		sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
	}
	sp = dbcSpell.LookupEntryForced( 27144 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
		sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
	}
	sp = dbcSpell.LookupEntryForced( 32770 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
		sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
	}

	//paladin - Improved Righteous Fury
	sp = dbcSpell.LookupEntryForced( 20468 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 20469 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 20470 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation[1] = 1;
	}

	//paladin ( grouping ) Healing Light
	group_relation_paladin_healing_light = 0x40000000 | 0x80000000;

	//paladin - Healing Light
	sp = dbcSpell.LookupEntryForced( 20237 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_healing_light;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
	sp = dbcSpell.LookupEntryForced( 20238 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_healing_light;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
	sp = dbcSpell.LookupEntryForced( 20239 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_healing_light;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}

	//paladin - Improved Devotion Aura
	sp = dbcSpell.LookupEntryForced( 20142 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;
	sp = dbcSpell.LookupEntryForced( 20141 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;
	sp = dbcSpell.LookupEntryForced( 20140 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;
	sp = dbcSpell.LookupEntryForced( 20139 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;
	sp = dbcSpell.LookupEntryForced( 20138 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;

	//paladin - Guardian's Favor
	sp = dbcSpell.LookupEntryForced( 20175 );
	if (sp != NULL)
	{
		sp->EffectSpellGroupRelation[0]=128;
		sp->EffectSpellGroupRelation[1]=268435472;
	}
	sp = dbcSpell.LookupEntryForced( 20174 );
	if (sp != NULL)
	{
		sp->EffectSpellGroupRelation[0]=128;
		sp->EffectSpellGroupRelation[1]=268435472;
	}
	//paladin - Improved Concentration Aura
	sp = dbcSpell.LookupEntryForced( 20256 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=131072;
	sp = dbcSpell.LookupEntryForced( 20255 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=131072;
	sp = dbcSpell.LookupEntryForced( 20254 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=131072;
	//paladin - Sacred Duty
	sp = dbcSpell.LookupEntryForced( 31849 );
	if (sp != NULL)
	{
		sp->EffectSpellGroupRelation[0]=4194304;
		sp->EffectSpellGroupRelation[1]=4194304;
	}
	sp = dbcSpell.LookupEntryForced( 31848 );
	if (sp != NULL)
	{
		sp->EffectSpellGroupRelation[0]=4194304;
		sp->EffectSpellGroupRelation[1]=4194304;
	}

	//paladin - Aura Mastery
	sp = dbcSpell.LookupEntryForced( 31821 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 131072 | 67108864 | 8 | 64;

	//paladin ( grouping ) Sanctified Light
	group_relation_paladin_sanctified_light = 0x80000000;

	//paladin - Sanctified Light
	sp = dbcSpell.LookupEntryForced( 20359 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_sanctified_light;
	sp = dbcSpell.LookupEntryForced( 20360 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_sanctified_light;
	sp = dbcSpell.LookupEntryForced( 20361 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_sanctified_light;

	//paladin - Improved Seal of the Crusader
	sp = dbcSpell.LookupEntryForced( 20335 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;
	sp = dbcSpell.LookupEntryForced( 20336 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;
	sp = dbcSpell.LookupEntryForced( 20337 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;
	sp = dbcSpell.LookupEntryForced( 28852 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;
	sp = dbcSpell.LookupEntryForced( 33557 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;

	//paladin - Light's Grace
	sp = dbcSpell.LookupEntryForced( 31834 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0x80000000;

	//paladin - Stoicism
	sp = dbcSpell.LookupEntryForced( 31844 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[1] = 0xFFFFFFFF;
	sp = dbcSpell.LookupEntryForced( 31845 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[1] = 0xFFFFFFFF;

	//paladin - Fanaticism
	sp = dbcSpell.LookupEntryForced( 31879 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;
	sp = dbcSpell.LookupEntryForced( 31880 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;
	sp = dbcSpell.LookupEntryForced( 31881 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;
	sp = dbcSpell.LookupEntryForced( 31882 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;
	sp = dbcSpell.LookupEntryForced( 31883 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;

	//paladin - Seal of Vengeance
	sp = dbcSpell.LookupEntryForced( 31801 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
        sp->EffectTriggerSpell[0] = 31803;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
	}

	//paladin - Reckoning
	sp = dbcSpell.LookupEntryForced( 20177 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 20179 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 20180 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 20181 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 20182 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;

	//paladin - Reckoning Effect = proc on proc
	sp = dbcSpell.LookupEntryForced( 20178 );
	if( sp != NULL )
	{
		sp->procChance = 100;
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
	}

	//paladin - Judgement of Wisdom
	sp = dbcSpell.LookupEntryForced( 20186 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 20268;
	}
	sp = dbcSpell.LookupEntryForced( 20354 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 20352;
	}
	sp = dbcSpell.LookupEntryForced( 20355 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 20353;
	}
	sp = dbcSpell.LookupEntryForced( 27164 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 27165;
	}
	sp = dbcSpell.LookupEntryForced( 20268 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	sp = dbcSpell.LookupEntryForced( 20352 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	sp = dbcSpell.LookupEntryForced( 20353 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	sp = dbcSpell.LookupEntryForced( 27165 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;

	//paladin - Judgement of Light
	sp = dbcSpell.LookupEntryForced( 20185 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 20267;
	}
	sp = dbcSpell.LookupEntryForced( 20344 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 20341;
	}
	sp = dbcSpell.LookupEntryForced( 20345 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 20342;
	}
	sp = dbcSpell.LookupEntryForced( 20346 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 20343;
	}
	sp = dbcSpell.LookupEntryForced( 27162 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 27163;
	}
	sp = dbcSpell.LookupEntryForced( 20267 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	sp = dbcSpell.LookupEntryForced( 20341 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	sp = dbcSpell.LookupEntryForced( 20342 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	sp = dbcSpell.LookupEntryForced( 20343 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	sp = dbcSpell.LookupEntryForced( 27163 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;

	//paladin - Eye for an Eye
	sp = dbcSpell.LookupEntryForced( 9799 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT_VICTIM;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 25997;
	}
	sp = dbcSpell.LookupEntryForced( 25988 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT_VICTIM;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 25997;
	}

	//paladin - sanctified judgement
	sp = dbcSpell.LookupEntryForced( 31876 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 31930;
	}
	sp = dbcSpell.LookupEntryForced( 31877 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 31930;
	}
	sp = dbcSpell.LookupEntryForced( 31878 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 31930;
		//sp->procChance=100;
	}

	//paladin - Blessed Life
	sp = dbcSpell.LookupEntryForced( 31828 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 31828;
	}
	sp = dbcSpell.LookupEntryForced( 31829 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 31828;
	}
	sp = dbcSpell.LookupEntryForced( 31830 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 31828;
	}

	//paladin - Light's Grace
	sp = dbcSpell.LookupEntryForced( 31833 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	sp = dbcSpell.LookupEntryForced( 31835 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	sp = dbcSpell.LookupEntryForced( 31836 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	//shaman - Lightning Mastery
	sp = dbcSpell.LookupEntryForced( 16578 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0]=1|2;
	sp = dbcSpell.LookupEntryForced( 16579 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0]=1|2;
	sp = dbcSpell.LookupEntryForced( 16580 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0]=1|2;
	sp = dbcSpell.LookupEntryForced( 16581 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0]=1|2;
	sp = dbcSpell.LookupEntryForced( 16582 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0]=1|2;

	//shaman - Lightning Overload 
	sp = dbcSpell.LookupEntryForced( 30675 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 30678 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 30679 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 30680 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 30681 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//shaman - Purge 
	sp = dbcSpell.LookupEntryForced( 370 ); 
	if( sp != NULL )
		sp->DispelType = DISPEL_MAGIC;
	sp = dbcSpell.LookupEntryForced( 8012 ); 
	if( sp != NULL )
		sp->DispelType = DISPEL_MAGIC;
	sp = dbcSpell.LookupEntryForced( 27626 ); 
	if( sp != NULL )
		sp->DispelType = DISPEL_MAGIC;
	sp = dbcSpell.LookupEntryForced( 33625 ); 
	if( sp != NULL )
		sp->DispelType = DISPEL_MAGIC;

	//shaman - Elemental mastery 
	sp = dbcSpell.LookupEntryForced( 16166 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF;//nature+fire+frost is all that shaman can do
		sp->EffectSpellGroupRelation_high[0] = 0xFFFFFFFF;//nature+fire+frost is all that shaman can do
		sp->EffectSpellGroupRelation[1] = 0xFFFFFFFF;//nature+fire+frost is all that shaman can do
		sp->EffectSpellGroupRelation_high[1] = 0xFFFFFFFF;//nature+fire+frost is all that shaman can do
	}

	//Shaman - Eye of the Storm
	sp = dbcSpell.LookupEntryForced( 29062 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	sp = dbcSpell.LookupEntryForced( 29064 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	sp = dbcSpell.LookupEntryForced( 29065 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;

	//Shaman - Focused Casting
	sp = dbcSpell.LookupEntryForced( 29063 );
	if( sp != NULL )
	{
	//        sp->EffectSpellGroupRelation[0] =  1 | 2 | 64 | 128 | 256;
		sp->EffectSpellGroupRelation[0] =  0xFFFFFFFF; // shaman spells. Guess that wraps them all 
		sp->EffectSpellGroupRelation_high[0] =  0xFFFFFFFF; // shaman spells. Guess that wraps them all 
	}

	//Shaman - Healing Focus
	sp = dbcSpell.LookupEntryForced( 16181 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;
	sp = dbcSpell.LookupEntryForced( 16230 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;
	sp = dbcSpell.LookupEntryForced( 16232 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;
	sp = dbcSpell.LookupEntryForced( 16233 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;
	sp = dbcSpell.LookupEntryForced( 16234 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;

	//shaman - Improved Lightning shield 
	sp = dbcSpell.LookupEntryForced( 16261 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;
	sp = dbcSpell.LookupEntryForced( 16290 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;
	sp = dbcSpell.LookupEntryForced( 16291 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;

	//shaman - Tidal focus . Add more heal spells here if i forgot any :P
	sp = dbcSpell.LookupEntryForced( 16179 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;
	sp = dbcSpell.LookupEntryForced( 16214 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;
	sp = dbcSpell.LookupEntryForced( 16215 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;
	sp = dbcSpell.LookupEntryForced( 16216 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;
	sp = dbcSpell.LookupEntryForced( 16217 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;

	//shaman ( grouping ) Enhancing Totems = Strength of Earth + Grace of Air
	group_relation_shaman_enhancing_totems = 0x00010000 | 0x00020000;

	//shaman - Enhancing Totems
	sp = dbcSpell.LookupEntryForced( 16259 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_enhancing_totems;
	sp = dbcSpell.LookupEntryForced( 16295 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_enhancing_totems;

	//shaman - Elemental Fury - ! Not finished !
	sp = dbcSpell.LookupEntryForced( 16089 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF ; //damn, what other spells do there remain after that list ? Maybe later :P
		//sp->EffectSpellGroupRelation[0] = 1073741824 | 32 | 1048576 | 1 | ... ; //Searing/Magma/Fire Nova Totem effects and Fire,Frost,Nature spells
		sp->EffectSpellGroupRelation_high[0] = 0xFFFFFFFF ; //damn, what other spells do there remain after that list ? Maybe later :P
	}

	//shaman ( grouping ) Restorative Totems = Mana Spring + Healing Stream
	group_relation_shaman_restorative_totems = 0x00004000 | 0x00002000;

	//shaman - Restorative Totems
	sp = dbcSpell.LookupEntryForced( 16259 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	sp = dbcSpell.LookupEntryForced( 16205 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	sp = dbcSpell.LookupEntryForced( 16206 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	sp = dbcSpell.LookupEntryForced( 16207 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	sp = dbcSpell.LookupEntryForced( 16208 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	
#ifndef NEW_PROCFLAGS
	//shaman - Healing Way
	sp = dbcSpell.LookupEntryForced( 29202 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 29205 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 29206 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
	}
#else
	//shaman - Healing Way
	sp = dbcSpell.LookupEntryForced( 29202 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;
	sp = dbcSpell.LookupEntryForced( 29205 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;
	sp = dbcSpell.LookupEntryForced( 29206 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;
#endif

	//shaman - Elemental Devastation
	sp = dbcSpell.LookupEntryForced( 29179 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	sp = dbcSpell.LookupEntryForced( 29180 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	sp = dbcSpell.LookupEntryForced( 30160 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;

	//shaman - Ancestral healing
	sp = dbcSpell.LookupEntryForced( 16176 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	sp = dbcSpell.LookupEntryForced( 16235 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	sp = dbcSpell.LookupEntryForced( 16240 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;

    //shaman - Ancestral healing proc spell
    sp = dbcSpell.LookupEntryForced( 16177 );
    if( sp != NULL )
        sp->rangeIndex = 4;
    sp = dbcSpell.LookupEntryForced( 16236 );
    if( sp != NULL )
        sp->rangeIndex = 4;
    sp = dbcSpell.LookupEntryForced( 16237 );
    if( sp != NULL )
        sp->rangeIndex = 4;

	//shaman ( grouping ) Mental Quickness (missing 18 spells which have no grouping)
	group_relation_shaman_mental_quickness = 0x00000008 | 0x00000010 | 0x00000200 | 0x00000400 | 0x00080000 | 0x00100000 | 0x00400000 | 0x20000000 | 0x10000000 | 0x80000000;

	//shaman - Mental Quickness
	sp = dbcSpell.LookupEntryForced( 30812 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_mental_quickness;
	sp = dbcSpell.LookupEntryForced( 30813 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_mental_quickness;
	sp = dbcSpell.LookupEntryForced( 30814 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_mental_quickness;

	//shaman ( grouping ) Totems
	group_relation_shaman_totems |= 0x00000008 | 0x00000010 | 0x00001000 | 0x00080000 | 0x20000000; //these are only selected flag since some totems contain more then 1 flag

	//shaman - Totemic focus
	sp = dbcSpell.LookupEntryForced( 16173 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;
	sp = dbcSpell.LookupEntryForced( 16222 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;
	sp = dbcSpell.LookupEntryForced( 16223 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;
	sp = dbcSpell.LookupEntryForced( 16224 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;
	sp = dbcSpell.LookupEntryForced( 16225 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;

	//shaman ( grouping ) Lightning = Lightning Bolt + Chain Lightning
	group_relation_shaman_lightning = 0x00000001 | 0x00000002;

	//shaman - Call of Thunder
	sp = dbcSpell.LookupEntryForced( 16041 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;
	sp = dbcSpell.LookupEntryForced( 16117 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;
	sp = dbcSpell.LookupEntryForced( 16118 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;
	sp = dbcSpell.LookupEntryForced( 16119 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;
	sp = dbcSpell.LookupEntryForced( 16120 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;

    //shaman ( grouping ) Shock = Earth + Flame + Frost
	group_relation_shaman_shock = 0x00100000 | 0x10000000 | 0x80000000;

	//shaman - Convection
	sp = dbcSpell.LookupEntryForced( 16039 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
	sp = dbcSpell.LookupEntryForced( 16109 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
	sp = dbcSpell.LookupEntryForced( 16110 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
	sp = dbcSpell.LookupEntryForced( 16111 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
	sp = dbcSpell.LookupEntryForced( 16112 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;

    //shaman - Concussion
    sp = dbcSpell.LookupEntryForced( 16035 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
    sp = dbcSpell.LookupEntryForced( 16105 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
    sp = dbcSpell.LookupEntryForced( 16106 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
    sp = dbcSpell.LookupEntryForced( 16107 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
    sp = dbcSpell.LookupEntryForced( 16108 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;

	//rogue ( grouping ) Elusiveness = blind + vanish
	group_relation_rogue_elusiveness = 0x00000800 | 0x01000000;

	//rogue - Elusiveness
	sp = dbcSpell.LookupEntryForced( 13981 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_elusiveness;
	sp = dbcSpell.LookupEntryForced( 14066 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_elusiveness;

	//rogue ( grouping ) Poisons
	group_relation_rogue_poisons = 0x00002000 | 0x00004000 | 0x00008000 | 0x00010000 | 0x10000000;

	//rogue - Vile Poisons
	sp = dbcSpell.LookupEntryForced( 14168 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}
	sp = dbcSpell.LookupEntryForced( 16514 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}
	sp = dbcSpell.LookupEntryForced( 16515 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}
	sp = dbcSpell.LookupEntryForced( 16719 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}
	sp = dbcSpell.LookupEntryForced( 16720 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}

	//rogue - Improved Poisons
	sp = dbcSpell.LookupEntryForced( 14113 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	sp = dbcSpell.LookupEntryForced( 14114 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	sp = dbcSpell.LookupEntryForced( 14115 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	sp = dbcSpell.LookupEntryForced( 14116 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	sp = dbcSpell.LookupEntryForced( 14117 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	sp = dbcSpell.LookupEntryForced( 21881 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;

	//rogue - Improved Expose Armor
	sp = dbcSpell.LookupEntryForced( 14168 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0x00080000;
	sp = dbcSpell.LookupEntryForced( 14169 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0x00080000;

	//rogue - Master Poisoner.
	sp = dbcSpell.LookupEntryForced( 31226 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	sp = dbcSpell.LookupEntryForced( 31227 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;

	//rogue - Find Weakness.
	sp = dbcSpell.LookupEntryForced( 31233 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	sp = dbcSpell.LookupEntryForced( 31239 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	sp = dbcSpell.LookupEntryForced( 31240 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	sp = dbcSpell.LookupEntryForced( 31241 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	sp = dbcSpell.LookupEntryForced( 31242 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	//rogue ( grouping ) Find Weakness
	group_relation_rogue_find_weakness = 0x00000008 | 0x00000010 | 0x00000100 | 0x00100000 | 0x00800000 | 0x04000000 | 0x20000000;

	//rogue - Find Weakness. The effect
	sp = dbcSpell.LookupEntryForced( 31234 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	sp = dbcSpell.LookupEntryForced( 31235 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	sp = dbcSpell.LookupEntryForced( 31236 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	sp = dbcSpell.LookupEntryForced( 31237 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	sp = dbcSpell.LookupEntryForced( 31238 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	
	//rogue - Camouflage.
	sp = dbcSpell.LookupEntryForced( 13975 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}
	sp = dbcSpell.LookupEntryForced( 14062 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}
	sp = dbcSpell.LookupEntryForced( 14063 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}
	sp = dbcSpell.LookupEntryForced( 14064 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}
	sp = dbcSpell.LookupEntryForced( 14065 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}

	//rogue - Mace Specialization.
	sp = dbcSpell.LookupEntryForced( 13709 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	sp = dbcSpell.LookupEntryForced( 13800 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	sp = dbcSpell.LookupEntryForced( 13801 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	sp = dbcSpell.LookupEntryForced( 13802 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	sp = dbcSpell.LookupEntryForced( 13803 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;

	//rogue - Dirty Tricks 
	sp = dbcSpell.LookupEntryForced( 14076 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16777216 | 128; // blind + sap
		sp->EffectSpellGroupRelation[1] = 16777216 | 128; // blind + sap
	}
	sp = dbcSpell.LookupEntryForced( 14094 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16777216 | 128; // blind + sap
		sp->EffectSpellGroupRelation[1] = 16777216 | 128; // blind + sap
	}

	//rogue - Dirty Deeds
	sp = dbcSpell.LookupEntryForced( 14082 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024 | 256; // Cheap Shot + Garrote
	sp = dbcSpell.LookupEntryForced( 14083 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024 | 256; // Cheap Shot + Garrote


	//rogue ( grouping ) Shadowstep
	group_relation_rogue_shadow_step |= 512;//rogue - ambush (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_shadow_step |= 4;//rogue - Backstab (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_shadow_step |= 256;//Garrote
	group_relation_rogue_shadow_step |= 536870912 | 16 | 8 | 8389120 | 41943040 | 33554432 | 32 | 67108864 | 64 | 128 ;

	//rogue - Shadowstep
	sp = dbcSpell.LookupEntryForced( 36563 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[1] = group_relation_rogue_shadow_step;
		sp->EffectSpellGroupRelation_high[1] = 256 | 128 ;
		sp->EffectMiscValue[1] = SMT_SPELL_VALUE;
	}

	//rogue ( grouping ) Lethality
	group_relation_rogue_lethality |= 2;//rogue - Sinister Strike (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 4;//rogue - backstab (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 8;//rogue - Gouge (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 33554432UL;//rogue - Hemorrhage (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 536870912UL;//rogue - Shiv (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 1073741824UL;//rogue - Ghostly Strike (only a part of the whole group since it would affect other spells too)

	//rogue Lethality
	sp = dbcSpell.LookupEntryForced( 14128 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;
	sp = dbcSpell.LookupEntryForced( 14132 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;
	sp = dbcSpell.LookupEntryForced( 14135 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;
	sp = dbcSpell.LookupEntryForced( 14136 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;
	sp = dbcSpell.LookupEntryForced( 14137 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;

	//rogue - Endurance 
	sp = dbcSpell.LookupEntryForced( 13742 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 32;	//Sprint + Evasion
	sp = dbcSpell.LookupEntryForced( 13872 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 32;	//Sprint + Evasion

     //priest - Holy Concentration
     sp = dbcSpell.LookupEntryForced( 34753 );
     if (sp != NULL)
          sp->procFlags = PROC_ON_CAST_SPELL;
     sp = dbcSpell.LookupEntryForced( 34859 );
     if (sp != NULL)
	     sp->procFlags = PROC_ON_CAST_SPELL;
     sp = dbcSpell.LookupEntryForced( 34860 );
     if (sp != NULL)
          sp->procFlags = PROC_ON_CAST_SPELL;
     sp = dbcSpell.LookupEntryForced( 34754 );
     if (sp != NULL)
     {
          //sp->EffectSpellGroupRelation[0] = 2048 | 4096;
          sp->EffectSpellGroupRelation_high[0] = 4;
     }

	//priest - Focused Mind 
	sp = dbcSpell.LookupEntryForced( 33213 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192 | 131072 | 8388608; // Mind Blast + Mind Control + Mind Flay
	sp = dbcSpell.LookupEntryForced( 33214 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192 | 131072 | 8388608; // Mind Blast + Mind Control + Mind Flay
	sp = dbcSpell.LookupEntryForced( 33215 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192 | 131072 | 8388608; // Mind Blast + Mind Control + Mind Flay

	//Priest: Blessed Recovery
	sp = dbcSpell.LookupEntryForced(27811);
	if(sp != NULL)
	{
		sp->EffectTriggerSpell[0] = 27813;
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	}
	sp = dbcSpell.LookupEntryForced(27815);
	if(sp != NULL)
	{
		sp->EffectTriggerSpell[0] = 27817;
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	}
	sp = dbcSpell.LookupEntryForced(27816);
	if(sp != NULL)
	{
		sp->EffectTriggerSpell[0] = 27818;
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	}
	//priest- Blessed Resilience
	sp = dbcSpell.LookupEntryForced( 33142 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	sp = dbcSpell.LookupEntryForced( 33145 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	sp = dbcSpell.LookupEntryForced( 33146 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;

	//priest- Focused Will
	sp = dbcSpell.LookupEntryForced( 45234 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	sp = dbcSpell.LookupEntryForced( 45243 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	sp = dbcSpell.LookupEntryForced( 45244 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;

	//priest - Improved Divine Spirit 
	sp = dbcSpell.LookupEntryForced( 33174 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32 ;
		sp->EffectSpellGroupRelation[1] = 32 ;
	}
	sp = dbcSpell.LookupEntryForced( 33182 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32 ;
		sp->EffectSpellGroupRelation[1] = 32 ;
	}

	//priest - Empowered Healing 
	sp = dbcSpell.LookupEntryForced( 33158 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
		sp->EffectSpellGroupRelation_high[1] = 4 ;
	}
	sp = dbcSpell.LookupEntryForced( 33159 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
		sp->EffectSpellGroupRelation_high[1] = 4 ;
	}
	sp = dbcSpell.LookupEntryForced( 33160 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
		sp->EffectSpellGroupRelation_high[1] = 4 ;
	}
	sp = dbcSpell.LookupEntryForced( 33161 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
		sp->EffectSpellGroupRelation_high[1] = 4 ;
	}
	sp = dbcSpell.LookupEntryForced( 33162 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
		sp->EffectSpellGroupRelation_high[1] = 4 ;
	}

	//priest - Force of Will 
	sp = dbcSpell.LookupEntryForced( 18544 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[2] = 0 ;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024 ;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024 ;
	}
	sp = dbcSpell.LookupEntryForced( 18547 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[2] = 0 ;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024 ;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024 ;
	}
	sp = dbcSpell.LookupEntryForced( 18548 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864 ;
		sp->EffectSpellGroupRelation[2] = 0 ;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024 ;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024 ;
	}
	sp = dbcSpell.LookupEntryForced( 18549 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[2] = 0;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024 ;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024 ;
	}
	sp = dbcSpell.LookupEntryForced( 18550 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[2] = 0;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024;
	}

	//Priest: Shadowguard
	sp = dbcSpell.LookupEntryForced( 18137 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28377;
	}
	sp = dbcSpell.LookupEntryForced( 19308 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28378;
	}
	sp = dbcSpell.LookupEntryForced( 19309 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28379;
	}
	sp = dbcSpell.LookupEntryForced( 19310 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28380;
	}
	sp = dbcSpell.LookupEntryForced( 19311 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28381;
	}
	sp = dbcSpell.LookupEntryForced( 19312 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28382;
	}
	sp = dbcSpell.LookupEntryForced( 25477 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28385;
	}

	//priest - Absolution 
	sp = dbcSpell.LookupEntryForced( 33167 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 128;
	}
	sp = dbcSpell.LookupEntryForced( 33171 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 128;
	}
	sp = dbcSpell.LookupEntryForced( 33172 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 128;
	}

	//priest - Mental Agility - all instant spells. I wonder if it conflicts with any other spells 
	sp = dbcSpell.LookupEntryForced( 14520 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}
	sp = dbcSpell.LookupEntryForced( 14780 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}
	sp = dbcSpell.LookupEntryForced( 14781 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}
	sp = dbcSpell.LookupEntryForced( 14782 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}
	sp = dbcSpell.LookupEntryForced( 14783 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}

	//priest - Focused Power
	sp = dbcSpell.LookupEntryForced( 33186 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation[1] = 128 | 8192 | 2147483648UL;
	}
	sp = dbcSpell.LookupEntryForced( 33190 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation[1] = 128 | 8192 | 2147483648UL;
	}

	//priest - Shadow Reach 
	sp = dbcSpell.LookupEntryForced( 17322 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768 | 65536 | 4 | 8192 | 16 | 8388608 | 2147483648UL;
	sp = dbcSpell.LookupEntryForced( 17323 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768 | 65536 | 4 | 8192 | 16 | 8388608 | 2147483648UL;

	//priest - Shadow Focus 
	sp = dbcSpell.LookupEntryForced( 15260 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}
	sp = dbcSpell.LookupEntryForced( 15327 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}
	sp = dbcSpell.LookupEntryForced( 15328 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}
	sp = dbcSpell.LookupEntryForced( 15329 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}
	sp = dbcSpell.LookupEntryForced( 15330 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}

	//Paladin: Seal of Wisdom
	uint32 procchance = 0;
	sp = dbcSpell.LookupEntryForced( 27116 );
	if( sp != NULL )
		procchance = sp->procChance;
	sp = dbcSpell.LookupEntryForced( 20166 );
	if( sp != NULL )
		sp->procChance = procchance;
	sp = dbcSpell.LookupEntryForced( 20356 );
	if( sp != NULL )
		sp->procChance = procchance;
	sp = dbcSpell.LookupEntryForced( 20357 );
	if( sp != NULL )
		sp->procChance = procchance;
	sp = dbcSpell.LookupEntryForced( 27166 );
	if( sp != NULL )
		sp->procChance = procchance;

	//Druid: Natural Perfection
	sp = dbcSpell.LookupEntryForced( 33881 );
	if ( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	sp = dbcSpell.LookupEntryForced( 33882 );
	if ( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	sp = dbcSpell.LookupEntryForced( 33883 );
	if ( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;

	//Druid: Frenzied Regeneration
	sp = dbcSpell.LookupEntryForced( 22842 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 22845;
	}
	sp = dbcSpell.LookupEntryForced( 22895 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 22845;
	}
	sp = dbcSpell.LookupEntryForced( 22896 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 22845;
	}
	sp = dbcSpell.LookupEntryForced( 26999 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 22845;
	}

	//Druid - Ferocity.
	sp = dbcSpell.LookupEntryForced( 16934 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}
	sp = dbcSpell.LookupEntryForced( 16935 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}
	sp = dbcSpell.LookupEntryForced( 16936 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}
	sp = dbcSpell.LookupEntryForced( 16937 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}
	sp = dbcSpell.LookupEntryForced( 16938 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}

	//Druid - Focused Starlight
	sp = dbcSpell.LookupEntryForced( 35363 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	sp = dbcSpell.LookupEntryForced( 35364 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;

	//Druid - Celestial Focus
	sp = dbcSpell.LookupEntryForced( 16850 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 16923 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 16924 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectSpellGroupRelation[1] = 1;
	}

	//Druid - Feral Aggression. Blizz made a mistake here ?
	sp = dbcSpell.LookupEntryForced( 16858 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}
	sp = dbcSpell.LookupEntryForced( 16859 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}
	sp = dbcSpell.LookupEntryForced( 16860 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}
	sp = dbcSpell.LookupEntryForced( 16861 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}
	sp = dbcSpell.LookupEntryForced( 16862 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}

	//paladin - seal of blood
	sp = dbcSpell.LookupEntryForced( 31892 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 31893;
	}
	sp = dbcSpell.LookupEntryForced( 38008 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 31893;
	}

	//paladin - Spiritual Attunement 
	sp = dbcSpell.LookupEntryForced( 31785 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_TARGET_SELF ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 31786;
	}
	sp = dbcSpell.LookupEntryForced( 33776 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_TARGET_SELF;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 31786;
	}

	//Druid: Leader of the Pack
	sp = dbcSpell.LookupEntryForced( 24932 );
	if( sp != NULL )
	{
		sp->Effect[1] = 0;
		sp->Effect[2] = 0; //removing strange effects.
	}

	//Druid: Improved Leader of the Pack
	sp = dbcSpell.LookupEntryForced( 34299 );
	if( sp != NULL )
		sp->proc_interval = 6000;//6 secs

	//fix for the right Enchant ID for Enchant Cloak - Major Resistance
	sp = dbcSpell.LookupEntryForced( 27962 );
	if( sp != NULL )
		sp->EffectMiscValue[0] = 2998;
	sp = dbcSpell.LookupEntryForced( 36285 );
	if( sp != NULL )
		sp->EffectMiscValue[0] = 2998;

	//muhaha, rewriting Retaliation spell as old one :D
	sp = dbcSpell.LookupEntryForced( 20230 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 22858; //evil , but this is good for us :D
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM; //add procflag here since this was not processed with the others !
	}

	//"bloodthirst" new version is ok but old version is wrong from now on :(
	sp = dbcSpell.LookupEntryForced( 23881 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //cast on us, it is good
		sp->EffectTriggerSpell[1] = 23885; //evil , but this is good for us :D
	}
	sp = dbcSpell.LookupEntryForced( 23892 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64;
		sp->EffectTriggerSpell[1] = 23886; //evil , but this is good for us :D
	}
	sp = dbcSpell.LookupEntryForced( 23893 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //
		sp->EffectTriggerSpell[1] = 23887; //evil , but this is good for us :D
	}
	sp = dbcSpell.LookupEntryForced( 23894 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //
		sp->EffectTriggerSpell[1] = 23888; //evil , but this is good for us :D
	}
	sp = dbcSpell.LookupEntryForced( 25251 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //aura
		sp->EffectTriggerSpell[1] = 25252; //evil , but this is good for us :D
	}
	sp = dbcSpell.LookupEntryForced( 30335 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //aura
		sp->EffectTriggerSpell[1] = 30339; //evil , but this is good for us :D
	}

	// Hunter - Master Tactician
	sp = dbcSpell.LookupEntryForced( 34506 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 34507 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 34508 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 34838 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 34839 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;

	// Hunter - Spirit Bond
	sp = dbcSpell.LookupEntryForced( 19578 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19579;
	}
	sp = dbcSpell.LookupEntryForced( 20895 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 24529;
	}
	sp = dbcSpell.LookupEntryForced( 19579 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA; //we should do the same for player too as we did for pet
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectAmplitude[1] = sp->EffectAmplitude[0];
		sp->EffectDieSides[1] = sp->EffectDieSides[0];
	}
	sp = dbcSpell.LookupEntryForced( 24529 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA; //we should do the same for player too as we did for pet
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectAmplitude[1] = sp->EffectAmplitude[0];
		sp->EffectDieSides[1] = sp->EffectDieSides[0];
	}

	// Hunter - Animal Handler
	sp = dbcSpell.LookupEntryForced( 34453 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HIT_CHANCE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 34454 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HIT_CHANCE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}

	// Hunter - Catlike Reflexes
	sp = dbcSpell.LookupEntryForced( 34462 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 34464 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 34465 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}

	// Hunter - Serpent's Swiftness
	sp = dbcSpell.LookupEntryForced( 34466 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 34467 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 34468 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 34469 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 34470 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}

	// Hunter - Ferocious Inspiration
	sp = dbcSpell.LookupEntryForced( 34455 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectTriggerSpell[0] = 34456;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF; //maybe target master ?
		sp->Effect[1] = 0; //remove this
	}
	sp = dbcSpell.LookupEntryForced( 34459 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectTriggerSpell[0] = 34456;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF; 
		sp->Effect[1] = 0; //remove this
	}
	sp = dbcSpell.LookupEntryForced( 34460 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectTriggerSpell[0] = 34456;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->Effect[1] = 0; //remove this
	}

	// Hunter - Focused Fire
	sp = dbcSpell.LookupEntryForced( 35029 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 35060;
		sp->EffectSpellGroupRelation_high[1] = 2048;
	}
	sp = dbcSpell.LookupEntryForced( 35030 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 35061;
		sp->EffectSpellGroupRelation_high[1] = 2048;
	}
	sp = dbcSpell.LookupEntryForced( 35060 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
	sp = dbcSpell.LookupEntryForced( 35061 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;

	// Hunter - Thick Hide
	sp = dbcSpell.LookupEntryForced( 19609 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_RESISTANCE; //we do not support armor rating for pets yet !
		sp->EffectBasePoints[0] *= 10; //just give it a little juice :P
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19610 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_RESISTANCE; //we do not support armor rating for pets yet !
		sp->EffectBasePoints[0] *= 10; //just give it a little juice :P
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19612 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_RESISTANCE; //we do not support armor rating for pets yet !
		sp->EffectBasePoints[0] *= 10; //just give it a little juice :P
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	// Hunter - Ferocity
	sp = dbcSpell.LookupEntryForced( 19612 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19599 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19600 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19601 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19602 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	// Hunter - Bestial Swiftness
	sp = dbcSpell.LookupEntryForced( 19596 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_SPEED; 
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	// Hunter - Endurance Training
	sp = dbcSpell.LookupEntryForced( 19583 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19584 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19585 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19586 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19587 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	// Hunter - Thrill of the Hunt
	sp = dbcSpell.LookupEntryForced( 34497 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->procChance = sp->EffectBasePoints[0]+1;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 34720;
	}
	sp = dbcSpell.LookupEntryForced( 34498 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->procChance = sp->EffectBasePoints[0]+1;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 34720;
	}
	sp = dbcSpell.LookupEntryForced( 34499 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->procChance = sp->EffectBasePoints[0]+1;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 34720;
	}

	// Hunter - Expose Weakness
	sp = dbcSpell.LookupEntryForced( 34500 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_CRIT_ATTACK;
	sp = dbcSpell.LookupEntryForced( 34502 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_CRIT_ATTACK;
	sp = dbcSpell.LookupEntryForced( 34503 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_CRIT_ATTACK;

	// Hunter - Hawk Eye
	sp = dbcSpell.LookupEntryForced( 19498 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	sp = dbcSpell.LookupEntryForced( 19499 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	sp = dbcSpell.LookupEntryForced( 19500 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;

	//Hunter - Frenzy
	sp = dbcSpell.LookupEntryForced( 19621 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}
	sp = dbcSpell.LookupEntryForced( 19622 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}
	sp = dbcSpell.LookupEntryForced( 19623 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}
	sp = dbcSpell.LookupEntryForced( 19624 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}
	sp = dbcSpell.LookupEntryForced( 19625 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}

	//Hunter - Unleashed Fury
	sp = dbcSpell.LookupEntryForced( 19616 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19617 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19618 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19619 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}
	sp = dbcSpell.LookupEntryForced( 19620 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}

	//Hunter : Pathfinding
	sp = dbcSpell.LookupEntryForced( 19559 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
	sp = dbcSpell.LookupEntryForced( 19560 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
	sp = dbcSpell.LookupEntryForced( 5131 );
	if( sp != NULL )
		sp->SpellGroupType = 2097152;
	sp = dbcSpell.LookupEntryForced( 13160 );
	if( sp != NULL )
		sp->SpellGroupType = 2097152;

	//Hunter : Rapid Killing - might need to add honor trigger too here. I'm guessing you receive Xp too so i'm avoiding double proc
	sp = dbcSpell.LookupEntryForced( 34948 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_GAIN_EXPIERIENCE | PROC_TARGET_SELF;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	sp = dbcSpell.LookupEntryForced( 34949 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_GAIN_EXPIERIENCE | PROC_TARGET_SELF;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	//Hunter : Rapid Killing - PROC
	sp = dbcSpell.LookupEntryForced( 35098 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 131072 | 2048 | 1;
	sp = dbcSpell.LookupEntryForced( 35099 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 131072 | 2048 | 1;

	//Hunter : Improved Stings
	sp = dbcSpell.LookupEntryForced( 19464 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}
	sp = dbcSpell.LookupEntryForced( 19465 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}
	sp = dbcSpell.LookupEntryForced( 19466 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}
	sp = dbcSpell.LookupEntryForced( 19467 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}
	sp = dbcSpell.LookupEntryForced( 19468 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}

	//we need to adress this somehow : shot
	sp = dbcSpell.LookupEntryForced( 3018 );
	if( sp != NULL )
		sp->SpellGroupType = 4;

	//Hunter : Mortal Shots
	sp = dbcSpell.LookupEntryForced( 19485 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot
	sp = dbcSpell.LookupEntryForced( 19487 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot
	sp = dbcSpell.LookupEntryForced( 19488 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot
	sp = dbcSpell.LookupEntryForced( 19489 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot
	sp = dbcSpell.LookupEntryForced( 19490 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot

	//Hunter : Improved Barrage
	sp = dbcSpell.LookupEntryForced( 35104 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
		sp->EffectSpellGroupRelation[1] = 8192;
	}
	sp = dbcSpell.LookupEntryForced( 35110 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
		sp->EffectSpellGroupRelation[1] = 8192;
	}
	sp = dbcSpell.LookupEntryForced( 35111 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
		sp->EffectSpellGroupRelation[1] = 8192;
	}

	//Hunter : Clever Traps
	sp = dbcSpell.LookupEntryForced( 19239 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16 | 8;
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[0] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 19245 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16 | 8;
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[0] = 128;
	}

	//Hunter : Resourcefulness
	sp = dbcSpell.LookupEntryForced( 34491 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128 | 2 | 64; 
		sp->EffectSpellGroupRelation[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 34492 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128 | 2 | 64; 
		sp->EffectSpellGroupRelation[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 34493 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128 | 2 | 64; 
		sp->EffectSpellGroupRelation[1] = 128;
	}

	/*
	//Hunter : Entrapment
	sp = dbcSpell.LookupEntryForced( 19184 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	sp = dbcSpell.LookupEntryForced( 19387 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	sp = dbcSpell.LookupEntryForced( 19388 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	*/
	
	//Mage - Icy Veins
	sp = dbcSpell.LookupEntryForced( 12472 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[1] = 512;
		sp->EffectMiscValue[1] = SMT_TRIGGER;
	}

	//Wand Shoot - Needs a group that is not used for mage and priest
	sp = dbcSpell.LookupEntryForced( 5019 );
	if( sp != NULL )
		sp->SpellGroupType = 134217728;
	
	//Mage - Wand Specialization. Not the forst thing we messed up. Blizz uses attack as magic and wandds as weapons :S
	sp = dbcSpell.LookupEntryForced( 6057 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	sp = dbcSpell.LookupEntryForced( 6085 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}

	//Priest - Wand Specialization
	sp = dbcSpell.LookupEntryForced( 14524 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	sp = dbcSpell.LookupEntryForced( 14525 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	sp = dbcSpell.LookupEntryForced( 14526 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	sp = dbcSpell.LookupEntryForced( 14527 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	sp = dbcSpell.LookupEntryForced( 14528 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}

	//Mage - Spell Power
	sp = dbcSpell.LookupEntryForced( 35578 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF;
		//sp->EffectSpellGroupRelation_high[0] = 0xFFFFFFFF;
	}
	sp = dbcSpell.LookupEntryForced( 35581 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF;
		//sp->EffectSpellGroupRelation_high[0] = 0xFFFFFFFF;
	}

	//Mage - Frost Channeling
	sp = dbcSpell.LookupEntryForced( 11160 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 524288 | 131072  ;
		sp->EffectSpellGroupRelation_high[0] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 12518 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 524288 | 131072  ;
		sp->EffectSpellGroupRelation_high[0] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 12519 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 524288 | 131072  ;
		sp->EffectSpellGroupRelation_high[0] = 1;
	}

	//Mage - Elemental Precision
	sp = dbcSpell.LookupEntryForced( 29438 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectSpellGroupRelation[0] = 8388608 | 2 | 16 | 4 | 1573376 | 524288 | 8 | 131072 | 262144 | 4194304 | 1 ;
		sp->EffectSpellGroupRelation_high[0] = 64 | 128 | 1;
		sp->EffectMiscValue[0] = SMT_COST;
	}
	sp = dbcSpell.LookupEntryForced( 29439 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectSpellGroupRelation[0] = 8388608 | 2 | 16 | 4 | 1573376 | 524288 | 8 | 131072 | 262144 | 4194304 | 1 ;
		sp->EffectSpellGroupRelation_high[0] = 64 | 128 | 1;
		sp->EffectMiscValue[0] = SMT_COST;
	}
	sp = dbcSpell.LookupEntryForced( 29440 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectSpellGroupRelation[0] = 8388608 | 2 | 16 | 4 | 1573376 | 524288 | 8 | 131072 | 262144 | 4194304 | 1 ;
		sp->EffectSpellGroupRelation_high[0] = 64 | 128 | 1;
		sp->EffectMiscValue[0] = SMT_COST;
	}

	//Mage - Arcane Concentration
	sp = dbcSpell.LookupEntryForced( 11213 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 12574 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 12575 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 12576 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 12577 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
		//sp->procChance = 100;
	}

	//Mage - ClearCasting Effect
	sp = dbcSpell.LookupEntryForced( 12536 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF;		//!!!this will set value multiple times but we do not care since it is 100%
		sp->EffectSpellGroupRelation_high[0] = 0xFFFFFFFF;	//!!!this will set value multiple times but we do not care since it is 100%
	}	

	//Mage - Arcane Blast
	sp = dbcSpell.LookupEntryForced( 30451 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[1] = 36032;
		sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
	}

	//Mage - Magic Attunement
	sp = dbcSpell.LookupEntryForced( 11247 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192;
	sp = dbcSpell.LookupEntryForced( 12606 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192; //strange lvl 2 was working 

	//Mage - Arcane Blast proc spell
	sp = dbcSpell.LookupEntryForced( 36032 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912;
		sp->EffectSpellGroupRelation[1] = 536870912;
	}

	// general - clearcasting
	sp = dbcSpell.LookupEntryForced( 12536 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all possible spells we can affect
	sp = dbcSpell.LookupEntryForced( 16246 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all possible spells we can affect
	sp = dbcSpell.LookupEntryForced( 16870 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all possible spells we can affect
	sp = dbcSpell.LookupEntryForced( 34754 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all possible spells we can affect

	// Shaman - Storm Reach
	sp = dbcSpell.LookupEntryForced( 28999 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 1;
	sp = dbcSpell.LookupEntryForced( 29000 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 1;

	//rogue - Seal Fate
	sp = dbcSpell.LookupEntryForced( 14186 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 20;
	}
	sp = dbcSpell.LookupEntryForced( 14190 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 40;
	}
	sp = dbcSpell.LookupEntryForced( 14193 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 60;
	}
	sp = dbcSpell.LookupEntryForced( 14194 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 80;
	}
	sp = dbcSpell.LookupEntryForced( 14195 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 100;
	}

	//druid Savage Fury
	sp = dbcSpell.LookupEntryForced( 16998 );
	if( sp != NULL ) 
	{
		sp->EffectSpellGroupRelation_high[0] |= 1024;
		//sp->EffectSpellGroupRelation_high[1] |= 1024;
		//sp->EffectSpellGroupRelation_high[2] |= 1024;
	}
	sp = dbcSpell.LookupEntryForced( 16999 );
	if( sp != NULL ) 
	{
		sp->EffectSpellGroupRelation_high[0] |= 1024;
		//sp->EffectSpellGroupRelation_high[1] |= 1024;
		//sp->EffectSpellGroupRelation_high[2] |= 1024;
	}

	//druid - Blood Frenzy
	sp = dbcSpell.LookupEntryForced( 16954 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 100;
	}
	sp = dbcSpell.LookupEntryForced( 16952 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 50;
	}

	//druid - Primal Fury
	sp = dbcSpell.LookupEntryForced( 16961 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 100;
	}
	sp = dbcSpell.LookupEntryForced( 16958 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 50;
	}

	//druid - Intensity
	sp = dbcSpell.LookupEntryForced( 17106 );
	if( sp != NULL )
	{
	   sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
	   sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 17107 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		 sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 17108 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

#ifndef NEW_PROCFLAGS
	//Improved Sprint
	sp = dbcSpell.LookupEntryForced( 13743 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 50;
	}
	sp = dbcSpell.LookupEntryForced( 13875 );
	if( sp != NULL )
	{
		sp->procChance = 100;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
#else
	//Improved Sprint
	sp = dbcSpell.LookupEntryForced( 13743 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0]=64;
	sp = dbcSpell.LookupEntryForced( 13875 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0]=64;
#endif

	//warlock -  Seed of Corruption
	sp = dbcSpell.LookupEntryForced( 27243 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[1] = 27285;
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_DIE;
		sp->procChance = 100;
	}

	//warlock -  soul link
	sp = dbcSpell.LookupEntryForced( 19028 );
	if( sp != NULL )
	{
		//this is for the trigger effect
		sp->Effect[0]=6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 25228;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM ;
		//this is for the extra 5% dmg for caster and pet
		sp->Effect[1] = 6;
		sp->EffectApplyAuraName[1] = 79;
		sp->EffectBasePoints[1] = 4; //4+1=5
		sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF ;
		sp->EffectImplicitTargetB[1] = EFF_TARGET_PET ;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
	}
	sp = dbcSpell.LookupEntryForced( 25228 );
	if( sp != NULL )
	{
		sp->Effect[0]=3;	
		sp->Effect[1]=0;//disable this part to not have multiple effects
	}

	//warlock: Demonic Aegis
	sp = dbcSpell.LookupEntryForced( 30143 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 32;
	sp = dbcSpell.LookupEntryForced( 30144 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 32;
	sp = dbcSpell.LookupEntryForced( 30145 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 32;

	//warlock: Nightfall
	sp = dbcSpell.LookupEntryForced( 18094 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 17941;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 2;
	}
	sp = dbcSpell.LookupEntryForced( 18095 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 17941;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 4;
	}

	//warlock: Contagion
	sp = dbcSpell.LookupEntryForced( 30060 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}
	sp = dbcSpell.LookupEntryForced( 30061 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}
	sp = dbcSpell.LookupEntryForced( 30062 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}
	sp = dbcSpell.LookupEntryForced( 30063 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}
	sp = dbcSpell.LookupEntryForced( 30064 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}

	//warlock: Malediction
	sp = dbcSpell.LookupEntryForced( 32477 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectSpellGroupRelation_high[0] = 512;
	}
	sp = dbcSpell.LookupEntryForced( 32483 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectSpellGroupRelation_high[0] = 512;
	}
	sp = dbcSpell.LookupEntryForced( 32484 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectSpellGroupRelation_high[0] = 512;
	}

	//warlock: Improved Searing Pain
	sp = dbcSpell.LookupEntryForced( 17927 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;
	sp = dbcSpell.LookupEntryForced( 17929 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;
	sp = dbcSpell.LookupEntryForced( 17930 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;

	//warlock: Empowered Corruption
	sp = dbcSpell.LookupEntryForced( 32381 );
	if( sp != NULL )
	{
		sp->EffectBasePoints[0] *= 6;
		sp->EffectSpellGroupRelation[0] = 2;
	}
	sp = dbcSpell.LookupEntryForced( 32382 );
	if( sp != NULL )
	{
		sp->EffectBasePoints[0] *= 6;
		sp->EffectSpellGroupRelation[0] = 2;
	}
	sp = dbcSpell.LookupEntryForced( 32383 );
	if( sp != NULL )
	{
		sp->EffectBasePoints[0] *= 6;
		sp->EffectSpellGroupRelation[0] = 2;
	}

	//warlock: Improved Enslave Demon
	sp = dbcSpell.LookupEntryForced( 18821 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectMiscValue[0]=SMT_SPELL_VALUE_PCT;
		sp->EffectBasePoints[0] = -(sp->EffectBasePoints[0]+2);
		//sp->EffectSpellGroupRelation[1] = 2048; //we do not handle this misc type yet anyway. Removed it just as a reminder
		sp->EffectSpellGroupRelation[2] = 2048;
	}
	sp = dbcSpell.LookupEntryForced( 18822 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation[1] = 2048;
		sp->EffectSpellGroupRelation[2] = 2048;
	}

	//warlock - Intensity
	sp = dbcSpell.LookupEntryForced( 4224 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512; //destruction spells
	sp = dbcSpell.LookupEntryForced( 18135 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512; //destruction spells

	//warlock: Devastation
	sp = dbcSpell.LookupEntryForced( 18130 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512; //destruction spells
	sp = dbcSpell.LookupEntryForced( 18131 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512;
	sp = dbcSpell.LookupEntryForced( 18132 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512;
	sp = dbcSpell.LookupEntryForced( 18133 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512;
	sp = dbcSpell.LookupEntryForced( 18134 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512;

	//warlock - Shadow Mastery
	sp = dbcSpell.LookupEntryForced( 18271 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}
	sp = dbcSpell.LookupEntryForced( 18272 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}
	sp = dbcSpell.LookupEntryForced( 18273 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}
	sp = dbcSpell.LookupEntryForced( 18274 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}
	sp = dbcSpell.LookupEntryForced( 18275 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}

	//mage: Arcane Power
	sp = dbcSpell.LookupEntryForced( 12042 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[1] = 1 | 8192 | 4194304 | 8388608 | 262144 | 131072 | 536870912 | 524352 | 4 | 4096 | 2 | 2048 | 16;
		sp->EffectSpellGroupRelation[2] = 1 | 8192 | 4194304 | 8388608 | 262144 | 131072 | 536870912 | 524352 | 4 | 4096 | 2 | 2048 | 16;
	}

	//mage : Improved Blink
	sp = dbcSpell.LookupEntryForced( 31569 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 65536;
	sp = dbcSpell.LookupEntryForced( 31570 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 65536;

	//mage : Empowered Arcane Missiles
	sp = dbcSpell.LookupEntryForced( 31579 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectBasePoints[0] *= 5; //heh B thinks he is smart by adding this to description ? If it doesn;t work std then it still needs to made by hand
		sp->EffectSpellGroupRelation[1] = 2048;
	}
	sp = dbcSpell.LookupEntryForced( 31582 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectBasePoints[0] *= 5; //heh B thinks he is smart by adding this to description ? If it doesn;t work std then it still needs to made by hand
		sp->EffectSpellGroupRelation[1] = 2048;
	}
	sp = dbcSpell.LookupEntryForced( 31583 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152; //damage
		sp->EffectBasePoints[0] *= 5; //heh B thinks he is smart by adding this to description ? If it doesn;t work std then it still needs to made by hand
		sp->EffectSpellGroupRelation[1] = 2048; //cost
	}

	//mage : Empowered Fireball
	sp = dbcSpell.LookupEntryForced( 31656 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	sp = dbcSpell.LookupEntryForced( 31657 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	sp = dbcSpell.LookupEntryForced( 31658 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	sp = dbcSpell.LookupEntryForced( 31659 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	sp = dbcSpell.LookupEntryForced( 31660 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//mage : Ice Floes
	sp = dbcSpell.LookupEntryForced( 31670 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 512;
		sp->EffectSpellGroupRelation_high[0] = 4 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 31672 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 512;
		sp->EffectSpellGroupRelation_high[0] = 4 | 1;
	}

	//mage : Empowered Frostbolt
	sp = dbcSpell.LookupEntryForced( 31682 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	sp = dbcSpell.LookupEntryForced( 31683 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	sp = dbcSpell.LookupEntryForced( 31684 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	sp = dbcSpell.LookupEntryForced( 31685 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	sp = dbcSpell.LookupEntryForced( 31686 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}

	//Mage - Ice Shards
	sp = dbcSpell.LookupEntryForced( 11207 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;
	sp = dbcSpell.LookupEntryForced( 12672 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;
	sp = dbcSpell.LookupEntryForced( 15047 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;
	sp = dbcSpell.LookupEntryForced( 15052 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;
	sp = dbcSpell.LookupEntryForced( 15053 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;

	//Mage - Improved Blizzard
	sp = dbcSpell.LookupEntryForced( 11185 );
	if( sp != NULL )
	{    
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 12484;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 12487 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 12485;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 12488 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 12486;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//mage: Fire Power
	sp = dbcSpell.LookupEntryForced( 11124 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 12398 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 12399 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 12400 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 12378 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	
	//mage - Burning Soul
	sp = dbcSpell.LookupEntryForced( 11083 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	sp = dbcSpell.LookupEntryForced( 12351 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;

	//mage - Combustion
	sp = dbcSpell.LookupEntryForced( 11129 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->procCharges = 0;
		sp->c_is_flags |= SPELL_FLAG_IS_REQUIRECOOLDOWNUPDATE;
	}
	sp = dbcSpell.LookupEntryForced( 28682 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 4194304 | 1;

	//mage - Empowered Fireball
	sp = dbcSpell.LookupEntryForced( 31656 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	sp = dbcSpell.LookupEntryForced( 31657 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	sp = dbcSpell.LookupEntryForced( 31658 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	sp = dbcSpell.LookupEntryForced( 31659 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	sp = dbcSpell.LookupEntryForced( 31660 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//mage - Empowered Frostbolt
	sp = dbcSpell.LookupEntryForced( 31682 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	sp = dbcSpell.LookupEntryForced( 31683 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	sp = dbcSpell.LookupEntryForced( 31684 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	sp = dbcSpell.LookupEntryForced( 31685 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	sp = dbcSpell.LookupEntryForced( 31686 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}

	//mage - Master of Elements
	sp = dbcSpell.LookupEntryForced( 29074 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 29077;
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT|PROC_TARGET_SELF);
		sp->procChance = 100;
	}
	sp = dbcSpell.LookupEntryForced( 29075 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 29077;
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT|PROC_TARGET_SELF);
		sp->procChance = 100;
	}
	sp = dbcSpell.LookupEntryForced( 29076 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 29077;
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT|PROC_TARGET_SELF);
		sp->procChance = 100;
	}

	//mage: Blazing Speed
	sp = dbcSpell.LookupEntryForced( 31641 ); 
	if( sp != NULL )
		sp->EffectTriggerSpell[0] = 31643;
	sp = dbcSpell.LookupEntryForced( 31642 );
	if( sp != NULL )
		sp->EffectTriggerSpell[0] = 31643;

	//mage talent "frostbyte". we make it to be dummy
	sp = dbcSpell.LookupEntryForced( 11071 );
	if( sp != NULL )
		sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
	sp = dbcSpell.LookupEntryForced( 12496 );
	if( sp != NULL )
		sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
	sp = dbcSpell.LookupEntryForced( 12497 );
	if( sp != NULL )
		sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
	//rogue-shiv -> add 1 combo point
	sp = dbcSpell.LookupEntryForced( 5938 );
	if( sp != NULL )
		sp->Effect[1]=80;

	//warlock - Improved Curse of Weakness
	sp = dbcSpell.LookupEntryForced( 18179 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768;
	sp = dbcSpell.LookupEntryForced( 18180 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768;

	//warlock - Amplify Curse
	sp = dbcSpell.LookupEntryForced( 18288 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024;
		sp->EffectSpellGroupRelation_high[0] = 2;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}

	//warlock - Improved Howl of Terror
	sp = dbcSpell.LookupEntryForced( 30054 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 8;
	sp = dbcSpell.LookupEntryForced( 30057 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 8;

	//warlock - Emberstorm
	sp = dbcSpell.LookupEntryForced( 17954 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
		sp->EffectSpellGroupRelation[1] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 17955 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
		sp->EffectSpellGroupRelation[1] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 17956 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
		sp->EffectSpellGroupRelation[1] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 17957 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
		sp->EffectSpellGroupRelation[1] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 17958 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
		sp->EffectSpellGroupRelation[1] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
	}

	//warlock - Shadow and Flame
	sp = dbcSpell.LookupEntryForced( 30288 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}
	sp = dbcSpell.LookupEntryForced( 30289 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}
	sp = dbcSpell.LookupEntryForced( 30290 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}
	sp = dbcSpell.LookupEntryForced( 30291 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}
	sp = dbcSpell.LookupEntryForced( 30292 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}

	//warlock - Ruin
	sp = dbcSpell.LookupEntryForced( 17959 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
	}

	//warlock - destructive reach 
	sp = dbcSpell.LookupEntryForced( 17917 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	sp = dbcSpell.LookupEntryForced( 17918 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}

	//warlock - Cataclysm 
	sp = dbcSpell.LookupEntryForced( 17778 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	sp = dbcSpell.LookupEntryForced( 17779 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	sp = dbcSpell.LookupEntryForced( 17780 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	sp = dbcSpell.LookupEntryForced( 17781 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	sp = dbcSpell.LookupEntryForced( 17782 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}

	//warlock - Intensity 
	sp = dbcSpell.LookupEntryForced( 18135 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	sp = dbcSpell.LookupEntryForced( 18136 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}

	//warlock - Suppression
	sp = dbcSpell.LookupEntryForced( 18174 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304; //affliction spell
	sp = dbcSpell.LookupEntryForced( 18175 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
	sp = dbcSpell.LookupEntryForced( 18176 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
	sp = dbcSpell.LookupEntryForced( 18177 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
	sp = dbcSpell.LookupEntryForced( 18178 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;

	//warlock - Improved Curse of Agony
	sp = dbcSpell.LookupEntryForced( 18827 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;
	sp = dbcSpell.LookupEntryForced( 18829 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;

	//warlock - Grim Reach
	sp = dbcSpell.LookupEntryForced( 18218 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
	sp = dbcSpell.LookupEntryForced( 18219 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;

	//warlock - Demonic Sacrifice
	sp = dbcSpell.LookupEntryForced( 18789 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;
	sp = dbcSpell.LookupEntryForced( 18790 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;
	sp = dbcSpell.LookupEntryForced( 18791 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;
	sp = dbcSpell.LookupEntryForced( 18792 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;
	sp = dbcSpell.LookupEntryForced( 35701 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;

	//warlock - Demonic Tactics
	sp = dbcSpell.LookupEntryForced( 30242 );
	if( sp != NULL )
	{
		sp->Effect[0] = 0; //disble this. This is just blizz crap. Pure proove that they suck :P
		sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
		sp->EffectApplyAuraName[2] = SPELL_AURA_MOD_SPELL_CRIT_CHANCE; //lvl 1 has it fucked up :O
		sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
	}
	sp = dbcSpell.LookupEntryForced( 30245 );
	if( sp != NULL )
	{
		sp->Effect[0] = 0; //disble this. This is just blizz crap. Pure proove that they suck :P
		sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
		sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
	}
	sp = dbcSpell.LookupEntryForced( 30246 );
	if( sp != NULL )
	{
		sp->Effect[0] = 0; //disble this. This is just blizz crap. Pure proove that they suck :P
		sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
		sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
	}
	sp = dbcSpell.LookupEntryForced( 30247 );
	if( sp != NULL )
	{
		sp->Effect[0] = 0; //disble this. This is just blizz crap. Pure proove that they suck :P
		sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
		sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
	}
	sp = dbcSpell.LookupEntryForced( 30248 );
	if( sp != NULL )
	{
		sp->Effect[0] = 0; //disble this. This is just blizz crap. Pure proove that they suck :P
		sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
		sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
	}

	//warlock - Demonic Resilience
	sp = dbcSpell.LookupEntryForced( 30319 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
	}
	sp = dbcSpell.LookupEntryForced( 30320 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
	}
	sp = dbcSpell.LookupEntryForced( 30321 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
	}

	//warlock - Improved Imp
	sp = dbcSpell.LookupEntryForced( 18694 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18695 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18696 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	//warlock - Improved Voidwalker
	sp = dbcSpell.LookupEntryForced( 18705 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18706 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18707 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	//warlock - Improved Succubus
	sp = dbcSpell.LookupEntryForced( 18754 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18755 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18756 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}

	//warlock - Fel Intellect
	sp = dbcSpell.LookupEntryForced( 18731 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 3;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18743 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 3;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18744 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 3;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	//warlock - Fel Stamina
	sp = dbcSpell.LookupEntryForced( 18748 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 2;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18749 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 2;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	sp = dbcSpell.LookupEntryForced( 18750 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 2;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	//warlock - Demonic Tactics
	sp = dbcSpell.LookupEntryForced( 30242 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}

	//warlock - Unholy Power
	sp = dbcSpell.LookupEntryForced( 18769 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}
	sp = dbcSpell.LookupEntryForced( 18770 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}
	sp = dbcSpell.LookupEntryForced( 18771 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}
	sp = dbcSpell.LookupEntryForced( 18772 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}
	sp = dbcSpell.LookupEntryForced( 18773 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}

	//warlock - Master Demonologist - 25 spells here
	sp = dbcSpell.LookupEntryForced( 23785 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 23784;
	}
	sp = dbcSpell.LookupEntryForced( 23822 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 23830;
	}
	sp = dbcSpell.LookupEntryForced( 23823 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 23831;
	}
	sp = dbcSpell.LookupEntryForced( 23824 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 23832;
	}
	sp = dbcSpell.LookupEntryForced( 23825 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 35708;
	}
	//and the rest
	sp = dbcSpell.LookupEntryForced( 23784 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	sp = dbcSpell.LookupEntryForced( 23830 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	sp = dbcSpell.LookupEntryForced( 23831 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	sp = dbcSpell.LookupEntryForced( 23832 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	sp = dbcSpell.LookupEntryForced( 35708 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	sp = dbcSpell.LookupEntryForced( 23759 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	sp = dbcSpell.LookupEntryForced( 23760 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	sp = dbcSpell.LookupEntryForced( 23761 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	sp = dbcSpell.LookupEntryForced( 23762 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	sp = dbcSpell.LookupEntryForced( 23826 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	sp = dbcSpell.LookupEntryForced( 23827 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	sp = dbcSpell.LookupEntryForced( 23828 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	sp = dbcSpell.LookupEntryForced( 23829 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	for(uint32 i=23833;i<=23844;i++)
	{
		sp = dbcSpell.LookupEntryForced( i );
		if( sp != NULL )
		{
			sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
			sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		}
	}
	sp = dbcSpell.LookupEntryForced( 35702 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	sp = dbcSpell.LookupEntryForced( 35703 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	sp = dbcSpell.LookupEntryForced( 35704 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	sp = dbcSpell.LookupEntryForced( 35705 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	sp = dbcSpell.LookupEntryForced( 35706 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	//warlock - Improved Healthstone
	sp = dbcSpell.LookupEntryForced( 18692 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->EffectSpellGroupRelation[0] = 65536;
	}
	sp = dbcSpell.LookupEntryForced( 18693 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->EffectSpellGroupRelation[0] = 65536;
	}

	//warlock - Unstable Affliction
	sp = dbcSpell.LookupEntryForced( 30108 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_PRE_DISPELL_AURA_VICTIM ;
		sp->procChance = 100;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[1] = 43523;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_SINGLE_FRIEND;
	}
	sp = dbcSpell.LookupEntryForced( 30404 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_PRE_DISPELL_AURA_VICTIM ;
		sp->procChance = 100;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[1] = 43523;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_SINGLE_FRIEND;
	}
	sp = dbcSpell.LookupEntryForced( 30405 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_PRE_DISPELL_AURA_VICTIM ;
		sp->procChance = 100;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[1] = 43523;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_SINGLE_FRIEND;
	}

	//warlock - Improved Drain Soul
	sp = dbcSpell.LookupEntryForced( 18213 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_TARGET_DIE | PROC_TARGET_SELF;
		sp->procChance = 100;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 18371;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->EffectSpellGroupRelation[1] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
		sp->Effect[2] = 0 ; //remove this effect
	}
	sp = dbcSpell.LookupEntryForced( 18372 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_TARGET_DIE | PROC_TARGET_SELF;
		sp->procChance = 100;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 18371;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->EffectSpellGroupRelation[1] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
		sp->Effect[2] = 0 ; //remove this effect
	}

	//warlock - Shadow Embrace
	sp = dbcSpell.LookupEntryForced( 32385 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}
	sp = dbcSpell.LookupEntryForced( 32387 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}
	sp = dbcSpell.LookupEntryForced( 32392 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}
	sp = dbcSpell.LookupEntryForced( 32393 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}
	sp = dbcSpell.LookupEntryForced( 32394 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}

	//warlock - Bane
	sp = dbcSpell.LookupEntryForced( 17788 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 17789 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 17790 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 17791 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 17792 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}

	//warlock - soul leech
	sp = dbcSpell.LookupEntryForced( 30293 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 30294;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}
	sp = dbcSpell.LookupEntryForced( 30295 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 30294;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}
	sp = dbcSpell.LookupEntryForced( 30296 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 30294;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}

	//warlock - Pyroclasm
	sp = dbcSpell.LookupEntryForced( 18073 );
	if( sp != NULL )
	{
		sp->Effect[0] = 0; //delete this owerride effect :P
		sp->EffectTriggerSpell[1] = 18093; //trigger spell was wrong :P
		sp->procFlags = PROC_ON_ANY_HOSTILE_ACTION;
		sp->procChance = 13; //god, save us from fixed values !
	}
	sp = dbcSpell.LookupEntryForced( 18096 );
	if( sp != NULL )
	{
		sp->Effect[0] = 0; //delete this owerride effect :P
		sp->EffectTriggerSpell[1] = 18093; //trigger spell was wrong :P
		sp->procFlags = PROC_ON_ANY_HOSTILE_ACTION;
		sp->procChance = 26; //god, save us from fixed values !
	}

	//Mage - Improved Scorch
	sp = dbcSpell.LookupEntryForced( 11095 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance =33;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 12872 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL; 
		sp->procChance =66;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 12873 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance =100;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//Nature's Grasp
	sp = dbcSpell.LookupEntryForced( 16689 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 339; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	sp = dbcSpell.LookupEntryForced( 16810 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 1062; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	sp = dbcSpell.LookupEntryForced( 16811 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 5195; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	sp = dbcSpell.LookupEntryForced( 16812 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 5196; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	sp = dbcSpell.LookupEntryForced( 16813 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 9852; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	sp = dbcSpell.LookupEntryForced( 17329 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 9853; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	sp = dbcSpell.LookupEntryForced( 27009 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26989; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}

	//wrath of air totem targets sorounding creatures instead of us
	sp = dbcSpell.LookupEntryForced( 2895 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
		sp->EffectImplicitTargetA[2] = 0;
		sp->EffectImplicitTargetB[0] = 0;
		sp->EffectImplicitTargetB[1] = 0;
		sp->EffectImplicitTargetB[2] = 0;
	}

	//-----BLEED effects----- (setting mech to BLEED)
	//garrot
	sp = dbcSpell.LookupEntryForced( 703 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 8631 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 8632 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 8633 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 11289 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 11290 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 26839 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 26884 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//pounce
	sp = dbcSpell.LookupEntryForced( 9007 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 9824 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 9826 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 27007 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//rend
	sp = dbcSpell.LookupEntryForced( 772 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 6546 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 6547 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 6548 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 11572 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 11573 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 11574 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 25208 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//rip
	sp = dbcSpell.LookupEntryForced( 1079 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 9492 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 9493 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 9752 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 9894 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 9896 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 27008 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//rupture
	sp = dbcSpell.LookupEntryForced( 1943 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 8639 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 8640 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 11273 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 11274 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 11275 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 26867 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//rake
	sp = dbcSpell.LookupEntryForced( 1822 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 1823 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 1824 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 9904 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	sp = dbcSpell.LookupEntryForced( 27003 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//lacerate
	sp = dbcSpell.LookupEntryForced( 33745 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//deuid - Pounce Bleed
	sp = dbcSpell.LookupEntryForced( 9007 );
	if( sp != NULL )
		sp->DurationIndex = 18000;
	sp = dbcSpell.LookupEntryForced( 9824 );
	if( sp != NULL )
		sp->DurationIndex = 18000;
	sp = dbcSpell.LookupEntryForced( 9826 );
	if( sp != NULL )
		sp->DurationIndex = 18000;
	sp = dbcSpell.LookupEntryForced( 27007 );
	if( sp != NULL )
		sp->DurationIndex = 18000;

	//Druid: Natural Shapeshifter
	sp = dbcSpell.LookupEntryForced( 16833 );
	if( sp != NULL )
		sp->DurationIndex = 0;
	sp = dbcSpell.LookupEntryForced( 16834 );
	if( sp != NULL )
		sp->DurationIndex = 0;
	sp = dbcSpell.LookupEntryForced( 16835 );
	if( sp != NULL )
		sp->DurationIndex = 0;

	//Priest: Shadow Power
	sp = dbcSpell.LookupEntryForced( 15310 );
	if( sp != NULL )
	{
		uint32 group = sp->EffectSpellGroupRelation[0];
		sp = dbcSpell.LookupEntryForced( 33221 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
		sp = dbcSpell.LookupEntryForced( 33222 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
		sp = dbcSpell.LookupEntryForced( 33223 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
		sp = dbcSpell.LookupEntryForced( 33224 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
		sp = dbcSpell.LookupEntryForced( 33225 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
	}

#ifdef NEW_PROCFLAGS
	//priest -  Shadow Weaving
	if (sp != NULL)
	{
		uint32 group = sp->EffectSpellGroupRelation[0];
		sp = dbcSpell.LookupEntryForced(15334);
		if (sp !=NULL)
			sp->EffectSpellGroupRelation[0] = group;
		sp = dbcSpell.LookupEntryForced(15333);
		if (sp !=NULL)
			sp->EffectSpellGroupRelation[0] = group;
		sp = dbcSpell.LookupEntryForced(15332);
		if (sp !=NULL)
			sp->EffectSpellGroupRelation[0] = group;
		sp = dbcSpell.LookupEntryForced(15331);
		if (sp !=NULL)
			sp->EffectSpellGroupRelation[0] = group;
		sp = dbcSpell.LookupEntryForced(15257);
		if (sp !=NULL)
			sp->EffectSpellGroupRelation[0] = group;
	}
#endif

    //Priest - Inspiration proc spell
    sp = dbcSpell.LookupEntryForced( 14893 );
    if( sp != NULL )
        sp->rangeIndex = 4;
    sp = dbcSpell.LookupEntryForced( 15357 );
    if( sp != NULL )
        sp->rangeIndex = 4;
    sp = dbcSpell.LookupEntryForced( 15359 );
    if( sp != NULL )
        sp->rangeIndex = 4;

#ifndef NEW_PROCFLAGS
	//Relentless Strikes
	sp = dbcSpell.LookupEntryForced( 14179 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;//proc spell
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectBasePoints[1] = 20; //client showes 20% chance but whe do not have it ? :O
	}
#else
	//Relentless Strikes
	sp = dbcSpell.LookupEntryForced( 14179 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0]= 262144 | 2097152 | 8388608 | 8519680 | 524288 | 1048576 | 8388608;
#endif

	//priest - surge of light
	sp = dbcSpell.LookupEntryForced( 33150 );
	if( sp != NULL )
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT_VICTIM | PROC_TARGET_SELF);
	sp = dbcSpell.LookupEntryForced( 33154 );
	if( sp != NULL )
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT_VICTIM | PROC_TARGET_SELF);
	sp = dbcSpell.LookupEntryForced( 33151 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
		sp->EffectSpellGroupRelation[1] = 128;
		sp->EffectSpellGroupRelation[2] = 128;
		sp->AuraInterruptFlags = AURA_INTERRUPT_ON_CAST_SPELL;
	}

	//Seal of Justice -lowered proc chance (experimental values !)
	sp = dbcSpell.LookupEntryForced( 20164 );
	if( sp != NULL )
		sp->procChance = 20;
	sp = dbcSpell.LookupEntryForced( 31895 );
	if( sp != NULL )
		sp->procChance = 20;

	//make Berserking a simple spell 
	sp = dbcSpell.LookupEntryForced( 20554 );
	if( sp != NULL )
	{
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26635;
	}
	sp = dbcSpell.LookupEntryForced( 26296 );
	if( sp != NULL )
	{
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26635;
	}
	sp = dbcSpell.LookupEntryForced( 26297 );
	if( sp != NULL )
	{
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26635;
	}

#ifndef NEW_PROCFLAGS
	//rogue - intiative
	sp = dbcSpell.LookupEntryForced( 13976 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}
	sp = dbcSpell.LookupEntryForced( 13979 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}
	sp = dbcSpell.LookupEntryForced( 13980 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}
#else
	//rogue - intiative
	sp = dbcSpell.LookupEntryForced( 13976 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8389120 | 256 | 1024;
	sp = dbcSpell.LookupEntryForced( 13979 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8389120 | 256 | 1024;
	sp = dbcSpell.LookupEntryForced( 13980 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8389120 | 256 | 1024;
#endif

	//this an on equip item spell(2824) :  ice arrow(29501)
	sp = dbcSpell.LookupEntryForced( 29501 );
	if( sp != NULL )
	{
		sp->procChance = 30;//some say it is triggered every now and then
		sp->procFlags = PROC_ON_RANGED_ATTACK;
	}

#ifdef NEW_PROCFLAGS
	//warrior - deep wounds
	sp = dbcSpell.LookupEntry( 12162);
	if ( sp!=NULL )
		sp->SpellGroupType = 32;
	sp = dbcSpell.LookupEntry( 12850);
	if ( sp!=NULL )
		sp->SpellGroupType = 32;
	sp = dbcSpell.LookupEntry( 12868);
	if ( sp!=NULL )
		sp->SpellGroupType = 32;
#endif

	//warrior - second wind should trigger on self
	sp = dbcSpell.LookupEntryForced( 29841 );
	if( sp != NULL )
		sp->procFlags |= PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 29842 );
	if( sp != NULL )
		sp->procFlags |= PROC_TARGET_SELF;

	//warrior - berserker rage is missing 1 effect = regenerate rage
	sp = dbcSpell.LookupEntryForced( 18499 );
	if( sp != NULL )
	{
		sp->Effect[2] = 6;
		sp->EffectApplyAuraName[2] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[2] = 9174; //not sure if this is the one. In my time this adds 30 rage
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TARGET_SELF;
	}

	//warrior - improved berserker rage
	sp = dbcSpell.LookupEntryForced( 20500 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 20501 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TARGET_SELF;

	//warrior - berserker rage is missing 1 effect = regenerate rage
	sp = dbcSpell.LookupEntryForced( 18499 );
	if( sp != NULL )
	{
		sp->Effect[2] = 6;
		sp->EffectApplyAuraName[2] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[2] = 9174; //not sure if this is the one. In my time this adds 30 rage
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TARGET_SELF;
	}

#ifndef NEW_PROCFLAGS
	//warrior - Blood Frenzy
	sp = dbcSpell.LookupEntryForced( 29836 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 29859 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
	}
#else
	//warrior - Blood Frenzy
	sp = dbcSpell.LookupEntryForced( 29836 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32;
	sp = dbcSpell.LookupEntryForced( 29859 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32;
#endif

	//warrior - Rampage
	sp = dbcSpell.LookupEntryForced( 30030 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
		sp->EffectTriggerSpell[0] = sp->EffectTriggerSpell[1];
	}
	sp = dbcSpell.LookupEntryForced( 30033 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
		sp->EffectTriggerSpell[0] = sp->EffectTriggerSpell[1];
	}

	//warrior - Unbridled Wrath
	sp = dbcSpell.LookupEntryForced( 12322 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 12999 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 13000 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 13001 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
	sp = dbcSpell.LookupEntryForced( 13002 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;

	//warrior - Commanding Presence
	sp = dbcSpell.LookupEntryForced( 12318 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 12857 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 12858 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 12860 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 12861 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}

	//warrior - Booming Voice
	sp = dbcSpell.LookupEntryForced( 12321 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 12835 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 12836 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 12837 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 12838 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}

	//warrior - Improved Intercept
	sp = dbcSpell.LookupEntryForced( 29888 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1073741824;
	sp = dbcSpell.LookupEntryForced( 29889 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1073741824;

	//warrior - Improved Mortal Strike
	sp = dbcSpell.LookupEntryForced( 35446 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	sp = dbcSpell.LookupEntryForced( 35448 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	sp = dbcSpell.LookupEntryForced( 35449 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	sp = dbcSpell.LookupEntryForced( 35450 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	sp = dbcSpell.LookupEntryForced( 35451 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;

	//warrior - Focused Rage
	sp = dbcSpell.LookupEntryForced( 29787 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824UL | 2048 | 64 | 1024 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 8 | 256 | 64;
	}
	sp = dbcSpell.LookupEntryForced( 29790 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824UL | 2048 | 64 | 1024 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 8 | 256 | 64;
	}
	sp = dbcSpell.LookupEntryForced( 29792 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824UL | 2048 | 64 | 1024 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 8 | 256 | 64;
	}

	//warrior - impale
	sp = dbcSpell.LookupEntryForced( 16493 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824 | 2048 | 64 | 1024;
	sp = dbcSpell.LookupEntryForced( 16494 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824 | 2048 | 64 | 1024;

	//warrior - Improved Whirlwind
	sp = dbcSpell.LookupEntryForced( 29721 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 4;
	sp = dbcSpell.LookupEntryForced( 29776 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 4;

	//warrior - Improved Commanding Shout
	sp = dbcSpell.LookupEntryForced( 38408 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 128;

	// priest - Reflective Shield
	sp = dbcSpell.LookupEntryForced( 33201 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	sp = dbcSpell.LookupEntryForced( 33202 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	sp = dbcSpell.LookupEntryForced( 33203 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	sp = dbcSpell.LookupEntryForced( 33204 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	sp = dbcSpell.LookupEntryForced( 33205 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}

	// mage - Frost Warding
	sp = dbcSpell.LookupEntryForced( 11189 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	sp = dbcSpell.LookupEntryForced( 28332 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;

	//Gnomish Poultryizer trinket - Poultryizer
	sp = dbcSpell.LookupEntryForced( 30507 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_TRANSFORM;
	}

	// paladin - benediction
	sp = dbcSpell.LookupEntryForced( 20101 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;
	sp = dbcSpell.LookupEntryForced( 20102 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;
	sp = dbcSpell.LookupEntryForced( 20103 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;
	sp = dbcSpell.LookupEntryForced( 20104 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;
	sp = dbcSpell.LookupEntryForced( 20105 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;

	/*// paladin - Improved Hammer of Justice
	uint32 Hammer_of_Justice_group=0;
	tsp = dbcSpell.LookupEntryForced( 853 ); //Hammer of Justice
	if(tsp)
		Hammer_of_Justice_group = tsp->SpellGroupType;
	sp = dbcSpell.LookupEntryForced( 20487 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	sp = dbcSpell.LookupEntryForced( 20488 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	sp = dbcSpell.LookupEntryForced( 20489 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	sp = dbcSpell.LookupEntryForced( 24188 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group; 
	*/

	/*// paladin - Improved Judgement
	sp = dbcSpell.LookupEntryForced( 25956 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = judgement_group;
	sp = dbcSpell.LookupEntryForced( 25957 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = judgement_group;
	*/

	// paladin - Improved Sanctity Aura
	sp = dbcSpell.LookupEntryForced( 31869 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 67108864;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
	sp = dbcSpell.LookupEntryForced( 31870 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 67108864;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
    sp = dbcSpell.LookupEntryForced( 20608 ); //Reincarnation
    if( sp != NULL )
    {
        for(uint32 i=0;i<8;i++)
        {
            if(sp->Reagent[i])
            {
                sp->Reagent[i] = 0;
                sp->ReagentCount[i] = 0;
            }
        }
    }

	// druid - Tree of Life
	sp = dbcSpell.LookupEntryForced( 5420 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[2] = 32 | 64 | 16; //for the mana cost tweak
		sp->EffectSpellGroupRelation_high[2] = 2 | 4096 | 128 | 524288 ;
	}

	// druid - Shredding Attacks
	sp = dbcSpell.LookupEntryForced( 16966 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32768;
		sp->EffectSpellGroupRelation_high[1] = 256;
	}
	sp = dbcSpell.LookupEntryForced( 16968 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32768;
		sp->EffectSpellGroupRelation_high[1] = 256;
	}

	// druid - Naturalist
	sp = dbcSpell.LookupEntryForced( 17069 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1;  
	}
	sp = dbcSpell.LookupEntryForced( 17070 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1; 
	}
	sp = dbcSpell.LookupEntryForced( 17071 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1; 
	}
	sp = dbcSpell.LookupEntryForced( 17072 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 17073 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1; 
	}

	// druid - Gift of Nature
	sp = dbcSpell.LookupEntryForced( 17104 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}
	sp = dbcSpell.LookupEntryForced( 24943 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}
	sp = dbcSpell.LookupEntryForced( 24944 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}
	sp = dbcSpell.LookupEntryForced( 24945 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}
	sp = dbcSpell.LookupEntryForced( 24946 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}

	// druid - Empowered Touch
	sp = dbcSpell.LookupEntryForced( 33879 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32;
	sp = dbcSpell.LookupEntryForced( 33880 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32;

	// druid - Empowered Rejuvenation
	sp = dbcSpell.LookupEntryForced( 33886 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;
	sp = dbcSpell.LookupEntryForced( 33887 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;
	sp = dbcSpell.LookupEntryForced( 33888 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;
	sp = dbcSpell.LookupEntryForced( 33889 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;
	sp = dbcSpell.LookupEntryForced( 33890 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;

	// druid - Wrath of cenarius
	sp = dbcSpell.LookupEntryForced( 33603 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 33604 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 33605 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 33606 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 33607 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}

	// druid - Nature's Grace
	sp = dbcSpell.LookupEntryForced( 16880 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;

	sp = dbcSpell.LookupEntryForced( 16886 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all spells, too bad not all spells have grouping flags :S
		sp->procCharges = 0;
	}

	// druid - Starlight Wrath
	sp = dbcSpell.LookupEntryForced( 16814 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	sp = dbcSpell.LookupEntryForced( 16815 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	sp = dbcSpell.LookupEntryForced( 16816 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	sp = dbcSpell.LookupEntryForced( 16817 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	sp = dbcSpell.LookupEntryForced( 16818 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;

	// Druid: Omen of Clarity
	sp = dbcSpell.LookupEntryForced( 16864 );
	if( sp != NULL )
	{
		sp->procChance=100; //procchance dynamic. 3ppm
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_ON_CRIT_ATTACK;
	}

	//---------ITEMS-----------------
#ifdef NEW_PROCFLAGS
	//Bonescythe Armor
	sp = dbcSpell.LookupEntryForced( 28814 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=8519680;

	//Tome of the Lightbringer
	sp = dbcSpell.LookupEntryForced( 41042 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=8388608;
	//Gladiator's Libram of Fortitude
	sp = dbcSpell.LookupEntryForced( 43850 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=8388608;
	//Vengeful Gladiator's Libram of Fortitude
	sp = dbcSpell.LookupEntryForced( 43852 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=8388608;
	//Merciless Gladiator's Libram of Fortitude
	sp = dbcSpell.LookupEntryForced( 43851 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=8388608;
	//Gladiator's Libram of Vengeance
	sp = dbcSpell.LookupEntryForced( 43854 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=64;
	//Merciless Gladiator's Libram of Vengeance
	sp = dbcSpell.LookupEntryForced( 43855 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=64;
	//Vengeful Gladiator's Libram of Vengeance
	sp = dbcSpell.LookupEntryForced( 43856 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=64;
	//The Earthshatterer
	sp = dbcSpell.LookupEntryForced( 28821 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=1024;
	//Idol of the White Stag
	sp = dbcSpell.LookupEntryForced( 41037 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=64 | 1024;
	//Merciless Gladiator's Idol of Resolve
	sp = dbcSpell.LookupEntryForced( 43842 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=64 | 1024;
	//Vengeful Gladiator's Idol of Resolve
	sp = dbcSpell.LookupEntryForced( 43843 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=64 | 1024;
	//Merciless Gladiator's Idol of Steadfastness
	sp = dbcSpell.LookupEntryForced( 43844 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=2;
	//Vengeful Gladiator's Idol of Steadfastness
	sp = dbcSpell.LookupEntryForced( 43845 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=2;
	//Merciless Gladiator's Totem of Indomitability
	sp = dbcSpell.LookupEntryForced( 43858 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=16;
	//Vengeful Gladiator's Totem of Indomitability
	sp = dbcSpell.LookupEntryForced( 43859 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=16;
	//Gladiator's Totem of Indomitability
	sp = dbcSpell.LookupEntryForced( 43857 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=16;
	//Merciless Gladiator's Totem of Survival
	sp = dbcSpell.LookupEntryForced( 43861 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]= 1048576 |268435456 | 2147483648;
	//Vengeful Gladiator's Totem of Survival
	sp = dbcSpell.LookupEntryForced( 43862 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]= 1048576 |268435456 | 2147483648;
	//Gladiator's Totem of Survival
	sp = dbcSpell.LookupEntryForced( 43861 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]= 1048576 |268435456 | 2147483648;
	//Wolfshead Helm
	sp = dbcSpell.LookupEntryForced( 17768 );
	if (sp != NULL)
	{
		sp->EffectSpellGroupRelation[0]= 1073741824;
		sp->EffectSpellGroupRelation[1]= 2147483648;
	}
	//Set: Plagueheart Raiment
	sp = dbcSpell.LookupEntryForced( 28831 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]= 1;
	//Set: Gladiator's Idol of Resolve
	sp = dbcSpell.LookupEntryForced( 37191 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=64 | 1024;
	//Set: Gladiator's Idol of Steadfastness
	sp = dbcSpell.LookupEntryForced( 43841 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=2;
	//Set: Incarnate Raiment
	sp = dbcSpell.LookupEntryForced( 37564 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=512;
	//Talon of Al'ar
	sp = dbcSpell.LookupEntryForced( 37507 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=2048;
	//Set: Crystalforge Armor
	sp = dbcSpell.LookupEntryForced( 37191 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation_high[0]=64;
	//Set: Redemption Armor
	sp = dbcSpell.LookupEntryForced( 28787 );
	if (sp != NULL)
		sp->EffectSpellGroupRelation[0]=4096;
	//Idol of the Claw
	sp = dbcSpell.LookupEntryForced( 34323 );
	if( sp != NULL )
	{
		sp->Flags5 = FLAGS5_PROCCHANCE_COMBOBASED;
		sp->EffectSpellGroupRelation[0]=8388608;
		sp->EffectSpellGroupRelation_high[0]=128;
	}
#endif

	//Thunderfury
	sp = dbcSpell.LookupEntryForced( 21992 );
	if( sp != NULL )
	{
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 27648;
		sp->EffectImplicitTargetA[2] = EFF_TARGET_SELF;
	}

	//Solarian's Sapphire
	sp = dbcSpell.LookupEntryForced( 37536 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 65536;

	//Totem of the Pulsing Earth
	sp = dbcSpell.LookupEntryForced( 37740 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//Totem of the Maelstrom
	sp = dbcSpell.LookupEntryForced( 37738 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;

	//Totem of Living Water
	sp = dbcSpell.LookupEntryForced( 43752 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;

	//Totem of Healing Rains
	sp = dbcSpell.LookupEntryForced( 38322 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;

	//Totem of Lightning
	sp = dbcSpell.LookupEntryForced( 33696 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//Everbloom Idol
	sp = dbcSpell.LookupEntryForced( 33693 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768;

	//Idol of the Avian Heart
	sp = dbcSpell.LookupEntryForced( 38321 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32;

	//Idol of the Crescent Goddess
	sp = dbcSpell.LookupEntryForced( 37737 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;

	//Idol of the Avenger
	sp = dbcSpell.LookupEntryForced( 37760 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//Energized 
	sp = dbcSpell.LookupEntryForced( 43750 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	//Spell Haste Trinket
	sp = dbcSpell.LookupEntryForced( 33297 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;

	// Band of the Eternal Sage
	sp = dbcSpell.LookupEntryForced( 35083 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	// Band of the Eternal Restorer 
	sp = dbcSpell.LookupEntryForced( 35086 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	// Ashtongue Talisman of Shadows 
	sp = dbcSpell.LookupEntryForced( 40478 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	// Ashtongue Talisman of Swiftness
	sp = dbcSpell.LookupEntryForced( 40485 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	// Ashtongue Talisman of Valor
	sp = dbcSpell.LookupEntryForced( 40458 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	// Memento of Tyrande
	sp = dbcSpell.LookupEntryForced( 37655 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	// Ashtongue Talisman of Insight
	sp = dbcSpell.LookupEntryForced( 40482 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;

	//Ashtongue Talisman of Equilibrium
	sp = dbcSpell.LookupEntryForced( 40442 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 40;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectTriggerSpell[0] = 40452;
		sp->maxstack = 1;
		sp->Effect[1] = 6;
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 25;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectTriggerSpell[1] = 40445;
		sp->maxstack = 1;
		sp->Effect[2] = 6;
		sp->EffectApplyAuraName[2] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 25;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectTriggerSpell[2] = 40446;
		sp->maxstack = 1;
	}

	//Ashtongue Talisman of Acumen
	sp = dbcSpell.LookupEntryForced( 40438 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 10;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectTriggerSpell[0] = 40441;
		sp->maxstack = 1;
		sp->Effect[1] = 6;
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 10;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectTriggerSpell[1] = 40440;
		sp->maxstack = 1;
	}

#ifndef NEW_PROCFLAGS
	//Ashtongue Talisman of Lethality
	sp = dbcSpell.LookupEntryForced( 40460 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 20;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectTriggerSpell[0] = 40461;
		sp->maxstack = 1;
	}
#else
	sp = dbcSpell.LookupEntryForced( 40460 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 262144 | 2097152 | 8388608 | 8519680 | 524288 | 1048576 | 8388608;
#endif

	//Serpent-Coil Braid
	sp = dbcSpell.LookupEntryForced( 37447 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 100;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectTriggerSpell[0] = 37445;
		sp->maxstack = 1;
	}

	//Item Set: Thunderheart Harness
	sp = dbcSpell.LookupEntryForced( 38447 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=1024;
		sp->EffectSpellGroupRelation_high[1] |=64;
	}
	sp = dbcSpell.LookupEntryForced( 38416 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=8388608;
		sp->EffectSpellGroupRelation_high[0] |=1048576;
		sp->EffectSpellGroupRelation[1] |=8388608;
	}

	//Item Set: Thunderheart Regalia
	sp = dbcSpell.LookupEntryForced( 38414 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=2;
	}
	sp = dbcSpell.LookupEntryForced( 38415 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=4;
	}

	//Item Set: Thunderheart Raiment
	sp = dbcSpell.LookupEntryForced( 38417 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=2;
	}
	sp = dbcSpell.LookupEntryForced( 38420 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=32;
	}

	//Item Set: Nordrassil Harness
	sp = dbcSpell.LookupEntryForced( 37333 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[1] |=256;
		sp->EffectSpellGroupRelation[0] |=32768;
	}

	//Item Set: Nordrassil Raiment
	sp = dbcSpell.LookupEntryForced( 37313 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=64;
	}
	sp = dbcSpell.LookupEntryForced( 37314 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=16;
	}

	//Item Set: Malorne Raiment
	sp = dbcSpell.LookupEntryForced( 37292 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=524288;
	}

	//Item Set: Malorne Regalia
	sp = dbcSpell.LookupEntryForced( 37297 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=4096;
	}

	//Item Set: Malorne Harness
	sp = dbcSpell.LookupEntryForced( 37306 );
	if( sp != NULL )
	{
		sp->procChance = 4;
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	}
	sp = dbcSpell.LookupEntryForced( 37311 );
	if( sp != NULL )
	{
		sp->procChance = 4;
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	}

	//Item Set: Slayer's Armor
	sp = dbcSpell.LookupEntryForced( 38388 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=262144;
	}
	sp = dbcSpell.LookupEntryForced( 38389 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |= 2 | 4;
		sp->EffectSpellGroupRelation[0] |= 8388612 |8388610 |41943040;
	}

	//Item Set: Deathmantle
	sp = dbcSpell.LookupEntryForced( 37170 );
	if( sp != NULL )
	{
		sp->procChance = 4;
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	}

	//Item Set: Netherblade
	sp = dbcSpell.LookupEntryForced( 37167 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |= 262144;
	}
	sp = dbcSpell.LookupEntryForced( 37168 );
	if( sp != NULL )
	{
		sp->procChance = 15;
		//sp->procFlags = PROC_ON_CAST_SPELL; Need new flag - PROC_ON_FINISH_MOVE;
	}

	//Item Set: Tempest Regalia
	sp = dbcSpell.LookupEntryForced( 38396 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 67108864;
	}
	sp = dbcSpell.LookupEntryForced( 38397 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1572896 | 1 | 2048;
	}

	//Item Set: Tirisfal Regalia
	sp = dbcSpell.LookupEntryForced( 37441 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912;
		sp->EffectSpellGroupRelation[1] = 536870912;
	}
	sp = dbcSpell.LookupEntryForced( 37443 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	}

	//Item Set: Aldor Regalia
	sp = dbcSpell.LookupEntryForced( 37438 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1572896 | 1;
	}
	sp = dbcSpell.LookupEntryForced( 37439 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 32;
		sp->EffectSpellGroupRelation_high[1] = 64;
		//sp->EffectSpellGroupRelation_high[2] = 64; NEED DBC 2.3.2 :P
	}

	//Item Set: Absolution Regalia
	sp = dbcSpell.LookupEntryForced( 38413 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32768;
	}
	sp = dbcSpell.LookupEntryForced( 38412 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8192;
	}

	//Item Set: Vestments of Absolution
	sp = dbcSpell.LookupEntryForced( 38410 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 512;
	}
	sp = dbcSpell.LookupEntryForced( 38411 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
	}

	//Item Set: Avatar Raiment
	sp = dbcSpell.LookupEntryForced( 26171 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 64;
	}

	//Item Set: Avatar Regalia
	sp = dbcSpell.LookupEntryForced( 37600 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 6;
	}

	//Item Set: Incarnate Raiment
	sp = dbcSpell.LookupEntryForced( 37568 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 37565 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
	}

	//Item Set: Incarnate Regalia
	sp = dbcSpell.LookupEntryForced( 37570 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[1] = 256;
	}
	sp = dbcSpell.LookupEntryForced( 37571 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}

	//Item Set: Malefic Raiment
	sp = dbcSpell.LookupEntryForced( 38393 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}

	//Item Set: Voidheart Raiment
	sp = dbcSpell.LookupEntryForced( 37377 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 5;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->proc_interval = 20;
		sp->EffectTriggerSpell[0] = 37379;
	}
	sp = dbcSpell.LookupEntryForced( 39437 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 5;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->proc_interval = 20;
		sp->EffectTriggerSpell[0] = 37378;
	}
	sp = dbcSpell.LookupEntryForced( 37380 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2|4;
	}

	//Item Set: Gronnstalker's Armor
	sp = dbcSpell.LookupEntryForced( 38392 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 1;
	}

	//Item Set: Rift Stalker Armor
	sp = dbcSpell.LookupEntryForced( 37505 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 1;
	}

	//Item Set: Demon Stalker Armor
	sp = dbcSpell.LookupEntryForced( 37484 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 256;
	}
	sp = dbcSpell.LookupEntryForced( 37485 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
	}

	//Item Set: Skyshatter Harness
	sp = dbcSpell.LookupEntryForced( 38429 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1048576 | 268435456 | 2147483648UL;
	}

	//Item Set: Skyshatter Raiment
	sp = dbcSpell.LookupEntryForced( 38434 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 256;
	}
	sp = dbcSpell.LookupEntryForced( 38435 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 256;
	}

	//Item Set: Skyshatter Regalia
	sp = dbcSpell.LookupEntryForced( 38436 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
	}

	//Item Set: Cataclysm Raiment
	sp = dbcSpell.LookupEntryForced( 37225 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 37227 );
	if( sp != NULL )
	{
		sp->proc_interval = 60000;
		sp->procChance = 100;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	}
	sp = dbcSpell.LookupEntryForced( 39950 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 64;
	}

	//Item Set: Cataclysm Regalia
	sp = dbcSpell.LookupEntryForced( 37228 );
	if( sp != NULL )
	{
		sp->procChance = 7;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	sp = dbcSpell.LookupEntryForced( 37234 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 37237 );
	if( sp != NULL )
	{
		sp->procChance = 25;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	}

	//Item Set: Cataclysm Harness
	sp = dbcSpell.LookupEntryForced( 37239 );
	if( sp != NULL )
	{
		sp->procChance = 2;
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	}
	sp = dbcSpell.LookupEntryForced( 37240 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 37241 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 512;
	}

	//Item Set: Cyclone Raiment
	sp = dbcSpell.LookupEntryForced( 37210 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 524288;
	}
	sp = dbcSpell.LookupEntryForced( 37211 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 524288;
	}

	//Item Set: Cyclone Harness
	sp = dbcSpell.LookupEntryForced( 37224 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 16;
	}
	sp = dbcSpell.LookupEntryForced( 37223 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912UL;
	}

	//Item Set: Cyclone Regalia
	sp = dbcSpell.LookupEntryForced( 37212 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 256;
	}
	sp = dbcSpell.LookupEntryForced( 37213 );
	if( sp != NULL )
	{
		sp->procChance = 11;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	}

	//Item Set: Lightbringer Armor
	sp = dbcSpell.LookupEntryForced( 38421 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 4096;
	}
	sp = dbcSpell.LookupEntryForced( 38422 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
	}

	//Item Set: Lightbringer Battlegear
	sp = dbcSpell.LookupEntryForced( 38427 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp->procChance = 20;
	}
	sp = dbcSpell.LookupEntryForced( 38424 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
	}

	//Item Set: Lightbringer Raiment
	sp = dbcSpell.LookupEntryForced( 38426 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
	}
	sp = dbcSpell.LookupEntryForced( 38425 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1073741824UL;
	}

	//Item Set: Crystalforge Armor
	sp = dbcSpell.LookupEntryForced( 37190 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
	}
	sp = dbcSpell.LookupEntryForced( 37191 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//Item Set: Crystalforge Battlegear
	sp = dbcSpell.LookupEntryForced( 37190 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 524288 | 1048576 | 536870912UL;
		sp->EffectSpellGroupRelation_high[0] = 1|520;
	}
	sp = dbcSpell.LookupEntryForced( 37195 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 6;
	}

	//Item Set: Crystalforge Raiment
	sp = dbcSpell.LookupEntryForced( 37189 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
		sp->proc_interval = 60000;
	}
	sp = dbcSpell.LookupEntryForced( 43837 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
	}
	sp = dbcSpell.LookupEntryForced( 37188 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//Item Set: Justicar Raiment
	sp = dbcSpell.LookupEntryForced( 37182 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 1;
	}
	sp = dbcSpell.LookupEntryForced( 37183 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 256;
	}

	//Item Set: Justicar Armor
	sp = dbcSpell.LookupEntryForced( 37184 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 1024;
		sp->EffectSpellGroupRelation[0] = 134217728;
		sp->EffectSpellGroupRelation_high[1] = 8;
	}
	sp = dbcSpell.LookupEntryForced( 37185 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 64;
	}

	//Item Set: Justicar Battlegear
	sp = dbcSpell.LookupEntryForced( 37186 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912UL;
	}
	sp = dbcSpell.LookupEntryForced( 37187 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 520;
	}

	//Item Set: Onslaught Battlegear
	sp = dbcSpell.LookupEntryForced( 38399 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1024;
	}
	sp = dbcSpell.LookupEntryForced( 38398 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912UL;
	}

	//Item Set: Onslaught Armor
	sp = dbcSpell.LookupEntryForced( 38408 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	sp = dbcSpell.LookupEntryForced( 38407 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 513;
	}

	//Item Set: Destroyer Armor
	sp = dbcSpell.LookupEntryForced( 37525 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
		sp->procChance = 7;
	}

	//Item Set: Destroyer Battlegear
	sp = dbcSpell.LookupEntryForced( 37528 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 100;
	}
	sp = dbcSpell.LookupEntryForced( 37535 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 33554432;
		sp->EffectSpellGroupRelation_high[0] = 1024;
	}

	//Item Set: Warbringer Armor
	sp = dbcSpell.LookupEntryForced( 37516 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 100;
	}

	//Item Set: Warbringer Battlegear
	sp = dbcSpell.LookupEntryForced( 37518 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 4;

	//all Drums 
	sp = dbcSpell.LookupEntryForced( 35474 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;
	sp = dbcSpell.LookupEntryForced( 35475 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;
	sp = dbcSpell.LookupEntryForced( 35476 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;
	sp = dbcSpell.LookupEntryForced( 35477 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;
	sp = dbcSpell.LookupEntryForced( 35478 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;

	//Paladin - Improved Lay on Hands
	sp = dbcSpell.LookupEntryForced( 20234 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	sp = dbcSpell.LookupEntryForced( 20235 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	//windfury weapon changes. Start to hate these day by day
	EnchantEntry* Enchantment;
		
	Enchantment = dbcEnchant.LookupEntryForced( 283 );
	if( Enchantment != NULL )
	{
		Enchantment->spell[0] = 33757; //this is actually good
		sp = dbcSpell.LookupEntryForced( 33757 );
		if( sp != NULL )
		{
			sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
			sp->procFlags = PROC_ON_MELEE_ATTACK; //we do not need proc on spell ;)
			sp->EffectTriggerSpell[0] = 8232; //for the logs and rest
			sp->procChance = 20;
			sp->proc_interval = 3000;//http://www.wowwiki.com/Windfury_Weapon
			sp->maxstack = 1;
		}
	}

	Enchantment = dbcEnchant.LookupEntryForced( 284 );
	if( Enchantment != NULL )
	{
		Enchantment->spell[0] = 33756; 
		sp = dbcSpell.LookupEntryForced( 33756 );
		if( sp != NULL )
		{
			sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
			sp->procFlags = PROC_ON_MELEE_ATTACK; //we do not need proc on spell ;)
			sp->EffectTriggerSpell[0] = 8235; //for the logs and rest
			sp->procChance = 20;
			sp->proc_interval = 3000; //http://www.wowwiki.com/Windfury_Weapon
			sp->maxstack = 1;
		}
	}

	Enchantment = dbcEnchant.LookupEntryForced( 525 );
	if( Enchantment != NULL )
	{
		Enchantment->spell[0] = 33755; 
		sp = dbcSpell.LookupEntryForced( 33755 );
		if( sp != NULL )
		{
			sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
			sp->procFlags = PROC_ON_MELEE_ATTACK; //we do not need proc on spell ;)
			sp->EffectTriggerSpell[0] = 10486; //for the logs and rest
			sp->procChance = 20;
			sp->proc_interval = 3000;//http://www.wowwiki.com/Windfury_Weapon
			sp->maxstack = 1;
		}
	}

	Enchantment = dbcEnchant.LookupEntryForced( 1669 );
	if( Enchantment != NULL )
	{
		Enchantment->spell[0] = 33754; 
		sp = dbcSpell.LookupEntryForced( 33754 );
		if( sp != NULL )
		{
			sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
			sp->procFlags = PROC_ON_MELEE_ATTACK; //we do not need proc on spell ;)
			sp->EffectTriggerSpell[0] = 16362; //for the logs and rest
			sp->procChance = 20;
			sp->proc_interval = 3000;//http://www.wowwiki.com/Windfury_Weapon
			sp->maxstack = 1;
		}
	}

	Enchantment = dbcEnchant.LookupEntryForced( 2636 );
	if( Enchantment != NULL )
	{
		Enchantment->spell[0] = 33727; 
		sp = dbcSpell.LookupEntryForced( 33727 );
		if( sp != NULL )
		{
			sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
			sp->procFlags = PROC_ON_MELEE_ATTACK; //we do not need proc on spell ;)
			sp->EffectTriggerSpell[0] = 25505; //for the logs and rest
			sp->procChance = 20;
			sp->proc_interval = 3000;//http://www.wowwiki.com/Windfury_Weapon
			sp->maxstack = 1;
		}
	}

	//for test only
	//sp = dbcSpell.LookupEntryForced( 32796 );
	//if( sp != NULL )
	//{
		//printf("!!!!!!hash %u \n",sp->NameHash);
		//sp->procChance=100;
		//SpellDuration *sd=sSpellDuration.LookupEntryForced(sp->DurationIndex);
		//printf("iterruptflag %u, duration %u",sp->AuraInterruptFlags,GetDuration(sd));
	//}

	//improoved berserker stance should be triggered on berserker stance use
	//sp = sSpellStore.LookupEntryForced(12704);
	//if( sp != NULL )	sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;

	//sp = sSpellStore.LookupEntryForced(16280);
	//if( sp != NULL )	printf("!!Interrupt flags %u interval %u charges %u\n",sp->AuraInterruptFlags,sp->proc_interval,sp->procCharges);
	//sp = sSpellStore.LookupEntryForced(16284);
	//if( sp != NULL )	printf("Interrupt flags %u\n",sp->AuraInterruptFlags);

	//fix for Predatory Strikes
	uint32 mm = (1<<(FORM_BEAR-1))|(1<<(FORM_DIREBEAR-1))|(1<<(FORM_MOONKIN-1))|(1<<(FORM_CAT-1));

	sp = dbcSpell.LookupEntryForced( 16972 );
	if( sp != NULL )
		sp->RequiredShapeShift = mm;
	sp = dbcSpell.LookupEntryForced( 16974 );
	if( sp != NULL )
		sp->RequiredShapeShift = mm;
	sp = dbcSpell.LookupEntryForced( 16975 );
	if( sp != NULL )
		sp->RequiredShapeShift = mm;

	sp = dbcSpell.LookupEntryForced( 20134 );
	if( sp != NULL )
		sp->procChance = 50;

	/* aspect of the pack - change to AA */
	sp = dbcSpell.LookupEntryForced( 13159 );
	if( sp != NULL )
		sp->Effect[0] = SPELL_EFFECT_APPLY_AREA_AURA;
	sp = dbcSpell.LookupEntryForced( 13159 );
	if( sp != NULL )
		sp->Effect[1] = SPELL_EFFECT_APPLY_AREA_AURA;

	/* aspect of the cheetah - add proc flags */
	sp = dbcSpell.LookupEntryForced( 5118 );
	if( sp != NULL )
		sp->procFlags = 139944;
	
	/* thrown - add a 1.6 second cooldown */
	const static uint32 thrown_spells[] = {SPELL_RANGED_GENERAL,SPELL_RANGED_THROW,SPELL_RANGED_WAND, 26679, 27084, 29436, 37074, 41182, 41346, 0};
	for(i = 0; thrown_spells[i] != 0; ++i)
	{
		sp = dbcSpell.LookupEntryForced( thrown_spells[i] );
		if( sp->RecoveryTime==0 && sp->StartRecoveryTime == 0 )
			sp->RecoveryTime = 1600;
	}

	/* wands - s5019 - change Spell_Dmg_Type from magic to ranged (3) */
	sp = dbcSpell.LookupEntryForced( SPELL_RANGED_WAND );
	if( sp != NULL )
		sp->Spell_Dmg_Type = SPELL_DMG_TYPE_RANGED;

// ------------------------------------------------------------------------------------------------

	Log.Notice("World","Starting Transport System...");
	objmgr.LoadTransporters();

	// start mail system
	MailSystem::getSingleton().StartMailSystem();

	Log.Notice("World", "Starting Auction System...");
	new AuctionMgr;
	sAuctionMgr.LoadAuctionHouses();

	m_queueUpdateTimer = mQueueUpdateInterval;
	if(Config.MainConfig.GetBoolDefault("Startup", "BackgroundLootLoading", true))
	{
		Log.Notice("World", "Backgrounding loot loading...");

		// loot background loading in a lower priority thread.
		ThreadPool.ExecuteTask(new BasicTaskExecutor(new CallbackP0<LootMgr>(LootMgr::getSingletonPtr(), &LootMgr::LoadLoot), 
			BTE_PRIORITY_LOW));
	}
	else
	{
		Log.Notice("World", "Loading loot in foreground...");
		lootmgr.LoadLoot();
	}

	Channel::LoadConfSettings();
	Log.Notice("BattlegroundManager", "Starting...");
	new CBattlegroundManager;

	dw = new DayWatcherThread();
	ThreadPool.ExecuteTask( dw );

	ThreadPool.ExecuteTask( new CharacterLoaderThread() );

	// Preload and compile talent and talent tab data to speed up talent inspect

	uint32 talent_max_rank;
	uint32 talent_pos;
	uint32 talent_class;

    for( uint32 i = 0; i < dbcTalent.GetNumRows(); ++i )
    {
        TalentEntry const* talent_info = dbcTalent.LookupRow( i );
		if( talent_info == NULL )
			continue;

		TalentTabEntry const* tab_info = dbcTalentTab.LookupEntry( talent_info->TalentTree );
		if( tab_info == NULL )
			continue;

        talent_max_rank = 0;
        for( uint32 j = 5; j > 0; --j )
        {
            if( talent_info->RankID[j - 1] )
            {
                talent_max_rank = j;
                break;
            }
		}

		InspectTalentTabBit[( talent_info->Row << 24 ) + ( talent_info->Col << 16 ) + talent_info->TalentID] = talent_max_rank;
		InspectTalentTabSize[talent_info->TalentTree] += talent_max_rank;
	}

	for( uint32 i = 1; i < dbcTalentTab.GetNumRows(); ++i )
	{
		TalentTabEntry const* tab_info = dbcTalentTab.LookupRow( i );
		if( tab_info == NULL )
			continue;

		talent_pos = 0;
        
		for( talent_class = 0; talent_class < 12; ++talent_class )
		{
			if( tab_info->ClassMask & ( 1 << talent_class ) )
				break;
		}

		InspectTalentTabPages[talent_class + 1][tab_info->TabPage] = tab_info->TalentTabID;

		for( std::map< uint32, uint32 >::iterator itr = InspectTalentTabBit.begin(); itr != InspectTalentTabBit.end(); ++itr )
		{
			uint32 talent_id = itr->first & 0xFFFF;
			TalentEntry const* talent_info = dbcTalent.LookupEntry( talent_id );
			if( talent_info == NULL )
				continue;

			if( talent_info->TalentTree != tab_info->TalentTabID )
				continue;

			InspectTalentTabPos[talent_id] = talent_pos;
			talent_pos += itr->second;
		}
	}

	sEventMgr.AddEvent(this, &World::CheckForExpiredInstances, EVENT_WORLD_UPDATEAUCTIONS, 120000, 0, 0);
	return true;
}


void World::Update(time_t diff)
{
	eventholder->Update((uint32)diff);
	sAuctionMgr.Update();
	_UpdateGameTime();
	UpdateQueuedSessions((uint32)diff);
}


void World::SendGlobalMessage(WorldPacket *packet, WorldSession *self)
{
	m_sessionlock.AcquireReadLock();

	SessionMap::iterator itr;
	for (itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
	{
		if (itr->second->GetPlayer() &&
			itr->second->GetPlayer()->IsInWorld()
			&& itr->second != self)  // dont send to self!
		{
			itr->second->SendPacket(packet);
		}
	}

	m_sessionlock.ReleaseReadLock();
}
void World::SendFactionMessage(WorldPacket *packet, uint8 teamId)
{
	m_sessionlock.AcquireReadLock();
	SessionMap::iterator itr;
	Player * plr;
	for(itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
	{
		plr = itr->second->GetPlayer();
		if(!plr || !plr->IsInWorld())
			continue;

		if(plr->GetTeam() == teamId)
			itr->second->SendPacket(packet);
	}
	m_sessionlock.ReleaseReadLock();
}

void World::SendZoneMessage(WorldPacket *packet, uint32 zoneid, WorldSession *self)
{
	m_sessionlock.AcquireReadLock();

	SessionMap::iterator itr;
	for (itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
	{
		if (itr->second->GetPlayer() &&
			itr->second->GetPlayer()->IsInWorld()
			&& itr->second != self)  // dont send to self!
		{
			if (itr->second->GetPlayer()->GetZoneId() == zoneid)
				itr->second->SendPacket(packet);
		}
	}

	m_sessionlock.ReleaseReadLock();
}

void World::SendInstanceMessage(WorldPacket *packet, uint32 instanceid, WorldSession *self)
{
	m_sessionlock.AcquireReadLock();

	SessionMap::iterator itr;
	for (itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
	{
		if (itr->second->GetPlayer() &&
			itr->second->GetPlayer()->IsInWorld()
			&& itr->second != self)  // dont send to self!
		{
			if (itr->second->GetPlayer()->GetInstanceID() == instanceid)
				itr->second->SendPacket(packet);
		}
	}

	m_sessionlock.ReleaseReadLock();
}

void World::SendWorldText(const char* text, WorldSession *self)
{
    uint32 textLen = (uint32)strlen((char*)text) + 1;

    WorldPacket data(textLen + 40);

	data.Initialize(SMSG_MESSAGECHAT);
	data << uint8(CHAT_MSG_SYSTEM);
	data << uint32(LANG_UNIVERSAL);
	
	data << (uint64)0; // Who cares about guid when there's no nickname displayed heh ?
	data << (uint32)0;
	data << (uint64)0;

	data << textLen;
	data << text;
	data << uint8(0);

	SendGlobalMessage(&data, self);

	sLog.outString("> %s", text);
}

void World::SendWorldWideScreenText(const char *text, WorldSession *self)
{
	WorldPacket data(256);
	data.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
	data << (uint32)0 << text << (uint8)0x00;
	SendGlobalMessage(&data, self);
}

void World::UpdateSessions(uint32 diff)
{
	SessionSet::iterator itr, it2;
	WorldSession *session;
	int result;
	for(itr = Sessions.begin(); itr != Sessions.end();)
	{
		session = (*itr);
		it2 = itr;
		++itr;
		if(!session || session->GetInstance() != 0)
		{
			Sessions.erase(it2);
			continue;
		}

		if((result = session->Update(0)))
		{
			if(result == 1)
			{
				// complete deletion
				DeleteSession(session);
			}
			Sessions.erase(it2);
		}
	}
}

std::string World::GenerateName(uint32 type)
{
	if(_namegendata[type].size() == 0)
		return "ERR";

	uint32 ent = RandomUInt((uint32)_namegendata[type].size()-1);
	return _namegendata[type].at(ent).name;
}

void World::DeleteSession(WorldSession *session)
{
	m_sessionlock.AcquireWriteLock();
	// remove from big map
	m_sessions.erase(session->GetAccountId());

	m_sessionlock.ReleaseWriteLock();

	// delete us
	session->Delete();
}

uint32 World::GetNonGmSessionCount()
{
	m_sessionlock.AcquireReadLock();

	uint32 total = (uint32)m_sessions.size();

	SessionMap::const_iterator itr = m_sessions.begin();
	for( ; itr != m_sessions.end(); itr++ )
	{
		if( (itr->second)->HasGMPermissions() )
			total--;
	}

	m_sessionlock.ReleaseReadLock();

	return total;
}

uint32 World::AddQueuedSocket(WorldSocket* Socket)
{
	// Since we have multiple socket threads, better guard for this one,
	// we don't want heap corruption ;)
	queueMutex.Acquire();

	// Add socket to list
	mQueuedSessions.push_back(Socket);
	queueMutex.Release();
	// Return queue position
	return (uint32)mQueuedSessions.size();
}

void World::RemoveQueuedSocket(WorldSocket* Socket)
{
	// Since we have multiple socket threads, better guard for this one,
	// we don't want heap corruption ;)
	queueMutex.Acquire();

	// Find socket in list
	QueueSet::iterator iter = mQueuedSessions.begin();
	for(; iter != mQueuedSessions.end(); ++iter)
	{
		if((*iter) == Socket)
		{
			// Remove from the queue and abort.
			// This will be slow (Removing from middle of a vector!) but it won't
			// get called very much, so it's not really a big deal.

			mQueuedSessions.erase(iter);
			queueMutex.Release();
			return;
		}
	}
	queueMutex.Release();
}

uint32 World::GetQueuePos(WorldSocket* Socket)
{
	// Since we have multiple socket threads, better guard for this one,
	// we don't want heap corruption ;)
	queueMutex.Acquire();

	// Find socket in list
	QueueSet::iterator iter = mQueuedSessions.begin();
	uint32 QueuePos = 1;
	for(; iter != mQueuedSessions.end(); ++iter, ++QueuePos)
	{
		if((*iter) == Socket)
		{
			queueMutex.Release();
			// Return our queue position.
			return QueuePos;
		}
	}
	queueMutex.Release();
	// We shouldn't get here..
	return 1;
}

void World::UpdateQueuedSessions(uint32 diff)
{
#ifndef CLUSTERING
	if(diff >= m_queueUpdateTimer) 
	{
		m_queueUpdateTimer = mQueueUpdateInterval;
		queueMutex.Acquire();

		if(mQueuedSessions.size() == 0)
		{
			queueMutex.Release();
			return;
		}
		
		while(m_sessions.size() < m_playerLimit && mQueuedSessions.size())
		{
			// Yay. We can let another player in now.
			// Grab the first fucker from the queue, but guard of course, since
			// this is in a different thread again.

			QueueSet::iterator iter = mQueuedSessions.begin();
			WorldSocket * QueuedSocket = *iter;
			mQueuedSessions.erase(iter);

			// Welcome, sucker.
			if(QueuedSocket->GetSession())
			{
				QueuedSocket->GetSession()->deleteMutex.Acquire();
				QueuedSocket->Authenticate();
				QueuedSocket->GetSession()->deleteMutex.Release();
			}
		}

		if(mQueuedSessions.size() == 0)
		{
			queueMutex.Release();
			return;
		}

		// Update the remaining queue members.
		QueueSet::iterator iter = mQueuedSessions.begin();
		uint32 Position = 1;
		while(iter != mQueuedSessions.end())
		{
			(*iter)->UpdateQueuePosition(Position++);
			if(iter==mQueuedSessions.end())
				break;
			else
				++iter;
		}
		queueMutex.Release();
	} 
	else 
	{
		m_queueUpdateTimer -= diff;
	}
#endif
}

void World::SaveAllPlayers()
{
	if(!(ObjectMgr::getSingletonPtr()))
		return;

	sLog.outString("Saving all players to database...");
	uint32 count = 0;
	PlayerStorageMap::const_iterator itr;
		// Servers started and obviously runing. lets save all players.
	uint32 mt;
	objmgr._playerslock.AcquireReadLock();   
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
		{
			if(itr->second->GetSession())
			{
				mt = getMSTime();
				itr->second->SaveToDB(false);
				sLog.outString("Saved player `%s` (level %u) in %ums.", itr->second->GetName(), itr->second->GetUInt32Value(UNIT_FIELD_LEVEL), getMSTime() - mt);
				++count;
			}
		}
	objmgr._playerslock.ReleaseReadLock();
	sLog.outString("Saved %u players.", count);
}

WorldSession* World::FindSessionByName(const char * Name)//case insensetive
{
	m_sessionlock.AcquireReadLock();

	// loop sessions, see if we can find him
	SessionMap::iterator itr = m_sessions.begin();
	for(; itr != m_sessions.end(); ++itr)
	{
	  if(!stricmp(itr->second->GetAccountName().c_str(),Name))
	  {
		  m_sessionlock.ReleaseReadLock();
			return itr->second;
	  }
	}
	m_sessionlock.ReleaseReadLock();
	return 0;
}

void World::ShutdownClasses()
{
	Log.Notice("AddonMgr", "~AddonMgr()");
	sAddonMgr.SaveToDB();
	delete AddonMgr::getSingletonPtr();

	Log.Notice("AuctionMgr", "~AuctionMgr()");
	delete AuctionMgr::getSingletonPtr();
	Log.Notice("LootMgr", "~LootMgr()");
	delete LootMgr::getSingletonPtr();

	Log.Notice("MailSystem", "~MailSystem()");
	delete MailSystem::getSingletonPtr();
}

void World::GetStats(uint32 * GMCount, float * AverageLatency)
{
	int gm = 0;
	int count = 0;
	int avg = 0;
	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		if(itr->second->GetSession())
		{
			count++;
			avg += itr->second->GetSession()->GetLatency();
			if(itr->second->GetSession()->GetPermissionCount())
				gm++;
		}			
	}
	objmgr._playerslock.ReleaseReadLock();

	*AverageLatency = count ? (float)((float)avg / (float)count) : 0;
	*GMCount = gm;
}

void TaskList::AddTask(Task * task)
{
	queueLock.Acquire();
	tasks.insert(task);
	queueLock.Release();
}

Task * TaskList::GetTask()
{
	queueLock.Acquire();

	Task* t = 0;
	for(set<Task*>::iterator itr = tasks.begin(); itr != tasks.end(); ++itr)
	{
		if(!(*itr)->in_progress)
		{
			t = (*itr);
			t->in_progress = true;
			break;
		}
	}
	queueLock.Release();
	return t;
}

void TaskList::spawn()
{
	running = true;
	thread_count = 0;

	uint32 threadcount;
	if(Config.MainConfig.GetBoolDefault("Startup", "EnableMultithreadedLoading", true))
	{
		// get processor count
#ifndef WIN32
#if UNIX_FLAVOUR == UNIX_FLAVOUR_LINUX
#ifdef X64
		threadcount = 2;
#else
		long affmask;
		sched_getaffinity(0, 4, (cpu_set_t*)&affmask);
		threadcount = (BitCount8(affmask)) * 2;
		if(threadcount > 8) threadcount = 8;
		else if(threadcount <= 0) threadcount = 1;
#endif
#else
		threadcount = 2;
#endif
#else
		SYSTEM_INFO s;
		GetSystemInfo(&s);
		threadcount = s.dwNumberOfProcessors * 2;
		if(threadcount>8)
			threadcount=8;
#endif
	}
	else
		threadcount = 1;

	Log.Line();
	Log.Notice("World", "Beginning %s server startup with %u threads.", (threadcount == 1) ? "progressive" : "parallel", threadcount);
	Log.Line();

	for(uint32 x = 0; x < threadcount; ++x)
		ThreadPool.ExecuteTask(new TaskExecutor(this));
}

void TaskList::wait()
{
	bool has_tasks = true;
	while(has_tasks)
	{
		queueLock.Acquire();
		has_tasks = false;
		for(set<Task*>::iterator itr = tasks.begin(); itr != tasks.end(); ++itr)
		{
			if(!(*itr)->completed)
			{
				has_tasks = true;
				break;
			}
		}
		queueLock.Release();
		Sleep(20);
	}
}

void TaskList::kill()
{
	running = false;
}

void Task::execute()
{
	_cb->execute();
}

bool TaskExecutor::run()
{
	Task * t;
	THREAD_TRY_EXECUTION
	{
		while(starter->running)
		{
			t = starter->GetTask();
			if(t)
			{
				t->execute();
				t->completed = true;
				starter->RemoveTask(t);
				delete t;
			}
			else
				Sleep(20);
		}
	}
	THREAD_HANDLE_CRASH
	return true;
}

void TaskList::waitForThreadsToExit()
{
	while(thread_count)
	{
		Sleep(20);
	}
}

void World::DeleteObject(Object * obj)
{
	delete obj;
}

void World::Rehash(bool load)
{
	if(load)
	{
		#ifdef WIN32
		Config.MainConfig.SetSource("ascent-world.conf", true);
		#else
		Config.MainConfig.SetSource((char*)CONFDIR "/ascent-world.conf", true);
		#endif
	}

	if(!ChannelMgr::getSingletonPtr())
		new ChannelMgr;

	if(!MailSystem::getSingletonPtr())
		new MailSystem;

	channelmgr.seperatechannels = Config.MainConfig.GetBoolDefault("Server", "SeperateChatChannels", false);
	MapPath = Config.MainConfig.GetStringDefault("Terrain", "MapPath", "maps");
	UnloadMapFiles = Config.MainConfig.GetBoolDefault("Terrain", "UnloadMapFiles", true);
	BreathingEnabled = Config.MainConfig.GetBoolDefault("Server", "EnableBreathing", true);
	SendStatsOnJoin = Config.MainConfig.GetBoolDefault("Server", "SendStatsOnJoin", true);
	compression_threshold = Config.MainConfig.GetIntDefault("Server", "CompressionThreshold", 1000);

	// load regeneration rates.
	setRate(RATE_HEALTH,Config.MainConfig.GetFloatDefault("Rates", "Health",1));
	setRate(RATE_POWER1,Config.MainConfig.GetFloatDefault("Rates", "Power1",1));
	setRate(RATE_POWER2,Config.MainConfig.GetFloatDefault("Rates", "Power2",1));
	setRate(RATE_POWER3,Config.MainConfig.GetFloatDefault("Rates", "Power4",1));
	setRate(RATE_DROP0,Config.MainConfig.GetFloatDefault("Rates", "DropGrey",1));
  setRate(RATE_DROP1,Config.MainConfig.GetFloatDefault("Rates", "DropWhite",1));
  setRate(RATE_DROP2,Config.MainConfig.GetFloatDefault("Rates", "DropGreen",1));
  setRate(RATE_DROP3,Config.MainConfig.GetFloatDefault("Rates", "DropBlue",1));
  setRate(RATE_DROP4,Config.MainConfig.GetFloatDefault("Rates", "DropPurple",1));
  setRate(RATE_DROP5,Config.MainConfig.GetFloatDefault("Rates", "DropOrange",1));
  setRate(RATE_DROP6,Config.MainConfig.GetFloatDefault("Rates", "DropArtifact",1));
	setRate(RATE_XP,Config.MainConfig.GetFloatDefault("Rates", "XP",1));
	setRate(RATE_RESTXP,Config.MainConfig.GetFloatDefault("Rates", "RestXP", 1));
	setRate(RATE_QUESTXP,Config.MainConfig.GetFloatDefault("Rates", "QuestXP", 1));
	setIntRate(INTRATE_SAVE, Config.MainConfig.GetIntDefault("Rates", "Save", 1));
	setRate(RATE_MONEY, Config.MainConfig.GetFloatDefault("Rates", "DropMoney", 1.0f));
	setRate(RATE_QUESTREPUTATION, Config.MainConfig.GetFloatDefault("Rates", "QuestReputation", 1.0f));
	setRate(RATE_KILLREPUTATION, Config.MainConfig.GetFloatDefault("Rates", "KillReputation", 1.0f));
	setRate(RATE_HONOR, Config.MainConfig.GetFloatDefault("Rates", "Honor", 1.0f));
	setRate(RATE_SKILLCHANCE, Config.MainConfig.GetFloatDefault("Rates", "SkillChance", 1.0f));
	setRate(RATE_SKILLRATE, Config.MainConfig.GetFloatDefault("Rates", "SkillRate", 1.0f));
	setIntRate(INTRATE_COMPRESSION, Config.MainConfig.GetIntDefault("Rates", "Compression", 1));
	setIntRate(INTRATE_PVPTIMER, Config.MainConfig.GetIntDefault("Rates", "PvPTimer", 300000));
	setRate(RATE_ARENAPOINTMULTIPLIER2X, Config.MainConfig.GetFloatDefault("Rates", "ArenaMultiplier2x", 1.0f));
	setRate(RATE_ARENAPOINTMULTIPLIER3X, Config.MainConfig.GetFloatDefault("Rates", "ArenaMultiplier3x", 1.0f));
	setRate(RATE_ARENAPOINTMULTIPLIER5X, Config.MainConfig.GetFloatDefault("Rates", "ArenaMultiplier5x", 1.0f));
	SetPlayerLimit(Config.MainConfig.GetIntDefault("Server", "PlayerLimit", 1000));
	SetMotd(Config.MainConfig.GetStringDefault("Server", "Motd", "Ascent Default MOTD").c_str());
	mQueueUpdateInterval = Config.MainConfig.GetIntDefault("Server", "QueueUpdateInterval", 5000);
	SetKickAFKPlayerTime(Config.MainConfig.GetIntDefault("Server", "KickAFKPlayers", 0));
	sLog.SetScreenLoggingLevel(Config.MainConfig.GetIntDefault("LogLevel", "Screen", 1));
	sLog.SetFileLoggingLevel(Config.MainConfig.GetIntDefault("LogLevel", "File", -1));
	Log.log_level = Config.MainConfig.GetIntDefault("LogLevel", "Screen", 1);
	gm_skip_attunement = Config.MainConfig.GetBoolDefault("Server", "SkipAttunementsForGM", true);
#ifndef CLUSTERING
	SocketRecvBufSize = Config.MainConfig.GetIntDefault("WorldSocket", "RecvBufSize", WORLDSOCKET_RECVBUF_SIZE);
	SocketSendBufSize = Config.MainConfig.GetIntDefault("WorldSocket", "SendBufSize", WORLDSOCKET_SENDBUF_SIZE);
#endif

	bool log_enabled = Config.MainConfig.GetBoolDefault("Log", "Cheaters", false);
	if(Anticheat_Log->IsOpen())
	{
		if(!log_enabled)
			Anticheat_Log->Close();
	}
	else
		if(log_enabled)
			Anticheat_Log->Open();

	log_enabled = Config.MainConfig.GetBoolDefault("Log", "GMCommands", false);
	if(GMCommand_Log->IsOpen())
	{
		if(!log_enabled)
			GMCommand_Log->Close();
	}
	else
		if(log_enabled)
			GMCommand_Log->Open();

	log_enabled = Config.MainConfig.GetBoolDefault("Log", "Player", false);
	if(Player_Log->IsOpen())
	{
		if(!log_enabled)
			Player_Log->Close();
	}
	else
	{
		if(log_enabled)
			Player_Log->Open();
	}

#ifdef WIN32
	DWORD current_priority_class = GetPriorityClass( GetCurrentProcess() );
	bool high = Config.MainConfig.GetBoolDefault( "Server", "AdjustPriority", false );

	if( high )
	{
		if( current_priority_class != HIGH_PRIORITY_CLASS )
			SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
	}
	else
	{
		if( current_priority_class != NORMAL_PRIORITY_CLASS )
			SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS );
	}
#endif

	reqGmClient = Config.MainConfig.GetBoolDefault("GMClient", "ReqGmClient", false);
	if(!Config.MainConfig.GetString("GMClient", "GmClientChannel", &GmClientChannel))
	{
		GmClientChannel = "";
	}

	realmtype = Config.MainConfig.GetBoolDefault("Server", "RealmType", false);
	TimeOut= uint32(1000* Config.MainConfig.GetIntDefault("Server", "ConnectionTimeout", 180) );

	uint32 config_flags = 0;
	if(Config.MainConfig.GetBoolDefault("Mail", "DisablePostageCostsForGM", true))
		config_flags |= MAIL_FLAG_NO_COST_FOR_GM;

	if(Config.MainConfig.GetBoolDefault("Mail", "DisablePostageCosts", false))
		config_flags |= MAIL_FLAG_DISABLE_POSTAGE_COSTS;

	if(Config.MainConfig.GetBoolDefault("Mail", "DisablePostageDelayItems", true))
		config_flags |= MAIL_FLAG_DISABLE_HOUR_DELAY_FOR_ITEMS;

	if(Config.MainConfig.GetBoolDefault("Mail", "DisableMessageExpiry", false))
		config_flags |= MAIL_FLAG_NO_EXPIRY;

	if(Config.MainConfig.GetBoolDefault("Mail", "EnableInterfactionMail", true))
		config_flags |= MAIL_FLAG_CAN_SEND_TO_OPPOSITE_FACTION;

	if(Config.MainConfig.GetBoolDefault("Mail", "EnableInterfactionForGM", true))
		config_flags |= MAIL_FLAG_CAN_SEND_TO_OPPOSITE_FACTION_GM;

	sMailSystem.config_flags = config_flags;
	flood_lines = Config.MainConfig.GetIntDefault("FloodProtection", "Lines", 0);
	flood_seconds = Config.MainConfig.GetIntDefault("FloodProtection", "Seconds", 0);
	flood_message = Config.MainConfig.GetBoolDefault("FloodProtection", "SendMessage", false);
	show_gm_in_who_list = Config.MainConfig.GetBoolDefault("Server", "ShowGMInWhoList", true);
	if(!flood_lines || !flood_seconds)
		flood_lines = flood_seconds = 0;

	map_unload_time=Config.MainConfig.GetIntDefault("Server", "MapUnloadTime", 0);

	antihack_teleport = Config.MainConfig.GetBoolDefault("AntiHack", "Teleport", true);
	antihack_speed = Config.MainConfig.GetBoolDefault("AntiHack", "Speed", true);
	antihack_falldmg = Config.MainConfig.GetBoolDefault("AntiHack", "FallDamage", true);
	antihack_flight = Config.MainConfig.GetBoolDefault("AntiHack", "Flight", true);
	no_antihack_on_gm = Config.MainConfig.GetBoolDefault("AntiHack", "DisableOnGM", false);
	SpeedhackProtection = antihack_speed;
	m_levelCap = Config.MainConfig.GetIntDefault("Server", "LevelCap", 70);
	m_genLevelCap = Config.MainConfig.GetIntDefault("Server", "GenLevelCap", 70);
	m_limitedNames = Config.MainConfig.GetBoolDefault("Server", "LimitedNames", true);
	m_useAccountData = Config.MainConfig.GetBoolDefault("Server", "UseAccountData", false);

	// ======================================
	m_movementCompressInterval = Config.MainConfig.GetIntDefault("Movement", "FlushInterval", 1000);
	m_movementCompressRate = Config.MainConfig.GetIntDefault("Movement", "CompressRate", 1);
	
	m_movementCompressThresholdCreatures = Config.MainConfig.GetFloatDefault("Movement", "CompressThresholdCreatures", 15.0f);
	m_movementCompressThresholdCreatures *= m_movementCompressThresholdCreatures;

	m_movementCompressThreshold = Config.MainConfig.GetFloatDefault("Movement", "CompressThreshold", 25.0f);
	m_movementCompressThreshold *= m_movementCompressThreshold;		// square it to avoid sqrt() on checks
	// ======================================

	if( m_banTable != NULL )
		free( m_banTable );

	m_banTable = NULL;
	string s = Config.MainConfig.GetStringDefault( "Server", "BanTable", "" );
	if( !s.empty() )
		m_banTable = strdup( s.c_str() );

	if( load )
		Channel::LoadConfSettings();
}

void World::LoadNameGenData()
{
	DBCFile dbc;

	if( !dbc.open( "DBC/NameGen.dbc" ) )
	{
		Log.Error( "World", "Cannot find file ./DBC/NameGen.dbc" );
		return;
	}

	for(uint32 i = 0; i < dbc.getRecordCount(); ++i)
	{
		NameGenData d;
		if(dbc.getRecord(i).getString(1)==NULL)
			continue;

		d.name = string(dbc.getRecord(i).getString(1));
		d.type = dbc.getRecord(i).getUInt(3);
		_namegendata[d.type].push_back(d);
	}
}

void World::CharacterEnumProc(QueryResultVector& results, uint32 AccountId)
{
	WorldSession * s = FindSession(AccountId);
	if(s == NULL)
		return;

	s->CharacterEnumProc(results[0].result);
}

void World::LoadAccountDataProc(QueryResultVector& results, uint32 AccountId)
{
	WorldSession * s = FindSession(AccountId);
	if(s == NULL)
		return;

	s->LoadAccountDataProc(results[0].result);
}

void World::CleanupCheaters()
{
	/*uint32 guid;
	string name;
	uint32 cl;
	uint32 level;
	uint32 talentpts;
	char * start, *end;
	Field * f;
	uint32 should_talents;
	uint32 used_talents;
	SpellEntry * sp;

	QueryResult * result = CharacterDatabase.Query("SELECT guid, name, class, level, available_talent_points, spells FROM characters");
	if(result == NULL)
		return;

	do 
	{
		f = result->Fetch();
		guid = f[0].GetUInt32();
		name = string(f[1].GetString());
		cl = f[2].GetUInt32();
		level = f[3].GetUInt32();
		talentpts = f[4].GetUInt32();
		start = f[5].GetString();
		should_talents = (level<10 ? 0 : level - 9);
		used_talents -= 
        		

		start = (char*)get_next_field.GetString();//buff;
		while(true) 
		{
			end = strchr(start,',');
			if(!end)break;
			*end=0;
			sp = dbcSpell.LookupEntry(atol(start));
			start = end +1;

			if(sp->talent_tree)

		}

	} while(result->NextRow());*/

}

void World::CheckForExpiredInstances()
{
	sInstanceMgr.CheckForExpiredInstances();
}

struct insert_playeritem
{
	uint32 ownerguid;
	uint32 entry;
	uint32 wrapped_item_id;
	uint32 wrapped_creator;
	uint32 creator;
	uint32 count;
	uint32 charges;
	uint32 flags;
	uint32 randomprop;
	uint32 randomsuffix;
	uint32 itemtext;
	uint32 durability;
	int32 containerslot;
	int32 slot;
	string enchantments;
};

#define LOAD_THREAD_SLEEP 180

void CharacterLoaderThread::OnShutdown()
{
	running=false;
#ifdef WIN32
	SetEvent(hEvent);
#else
	pthread_cond_signal(&cond);
#endif
}

CharacterLoaderThread::CharacterLoaderThread()
{

}

CharacterLoaderThread::~CharacterLoaderThread()
{
#ifdef WIN32
	CloseHandle(hEvent);
#else
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
#endif
}

bool CharacterLoaderThread::run()
{
#ifdef WIN32
	hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
#else
	struct timeval now;
	struct timespec tv;

	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);
#endif
	running=true;
	for(;;)
	{
		// Get a single connection to maintain for the whole process.
		DatabaseConnection * con = CharacterDatabase.GetFreeConnection();

		sWorld.PollCharacterInsertQueue(con);
		sWorld.PollMailboxInsertQueue(con);
		/* While this looks weird, it ensures the system doesn't waste time switching to these contexts.
		   WaitForSingleObject will suspend the thread,
		   and on unix, select will as well. - Burlex
			*/
		con->Busy.Release();
#ifdef WIN32
		WaitForSingleObject(hEvent,LOAD_THREAD_SLEEP*1000);
#else
		gettimeofday(&now, NULL);
		tv.tv_sec = now.tv_sec + LOAD_THREAD_SLEEP;
		tv.tv_nsec = now.tv_usec * 1000;
		pthread_mutex_lock(&mutex);
		pthread_cond_timedwait(&cond, &mutex, &tv);
		pthread_mutex_unlock(&mutex);
#endif
		if(!running)
			break;
	}

	return true;
}

void World::PollMailboxInsertQueue(DatabaseConnection * con)
{
	QueryResult * result;
	Field * f;
	Item * pItem;
	uint32 itemid;
	uint32 stackcount;

	result = CharacterDatabase.FQuery("SELECT * FROM mailbox_insert_queue", con);
	if( result != NULL )
	{
		Log.Notice("MailboxQueue", "Sending queued messages....");
		do 
		{
			f = result->Fetch();
			itemid = f[6].GetUInt32();
			stackcount = f[7].GetUInt32();
			
			if( itemid != 0 )
			{
				pItem = objmgr.CreateItem( itemid, NULL );
				if( pItem != NULL )
				{
					pItem->SetUInt32Value( ITEM_FIELD_STACK_COUNT, stackcount );
					pItem->SaveToDB( 0, 0, true, NULL );
				}
			}
			else
				pItem = NULL;

			Log.Notice("MailboxQueue", "Sending message to %u (item: %u)...", f[1].GetUInt32(), itemid);
			sMailSystem.SendAutomatedMessage( 0, f[0].GetUInt64(), f[1].GetUInt64(), f[2].GetString(), f[3].GetString(), f[5].GetUInt32(),
				0, pItem ? pItem->GetGUID() : 0, f[4].GetUInt32() );

			if( pItem != NULL )
				delete pItem;

		} while ( result->NextRow() );
		delete result;
		Log.Notice("MailboxQueue", "Done.");
		CharacterDatabase.FWaitExecute("DELETE FROM mailbox_insert_queue", con);
	}
}

void World::PollCharacterInsertQueue(DatabaseConnection * con)
{
	// Our local stuff..
	bool has_results = false;
	map<uint32, vector<insert_playeritem> > itemMap;
	map<uint32,vector<insert_playeritem> >::iterator itr;
	Field * f;
	insert_playeritem ipi;                          
	static const char * characterTableFormat = "uSuuuuuussuuuuuuuuuuuuuuffffuususuufffuuuuusuuuUssuuuuuuffffuuuuufffssssssuuuuuuuu";

	// Lock the table to prevent any more inserts
	CharacterDatabase.FWaitExecute("LOCK TABLES `playeritems_insert_queue` WRITE", con);

	// Cache all items in memory. This will save us doing additional queries and slowing down the db.
	QueryResult * result = CharacterDatabase.FQuery("SELECT * FROM playeritems_insert_queue", con);
	if(result)
	{
		do 
		{
			f = result->Fetch();
			
			ipi.ownerguid = f[0].GetUInt32();
			ipi.entry = f[1].GetUInt32();
			ipi.wrapped_item_id = f[2].GetUInt32();
			ipi.wrapped_creator = f[3].GetUInt32();
			ipi.creator = f[4].GetUInt32();
			ipi.count = f[5].GetUInt32();
			ipi.charges = f[6].GetUInt32();
			ipi.flags = f[7].GetUInt32();
			ipi.randomprop = f[8].GetUInt32();
			ipi.randomsuffix = f[9].GetUInt32();
			ipi.itemtext = f[10].GetUInt32();
			ipi.durability = f[11].GetUInt32();
			ipi.containerslot = f[12].GetInt32();
			ipi.slot = f[13].GetInt32();
			ipi.enchantments = string(f[14].GetString());

			itr = itemMap.find(ipi.ownerguid);
			if(itr == itemMap.end())
			{
				vector<insert_playeritem> to_insert;
				to_insert.push_back(ipi);
				itemMap.insert(make_pair(ipi.ownerguid,to_insert));
			}
			else
			{
				itr->second.push_back(ipi);
			}
		
		} while(result->NextRow());
		delete result;
	}

	// Unlock the item table
	CharacterDatabase.FWaitExecute("UNLOCK TABLES", con);

	// Lock the character table
	CharacterDatabase.FWaitExecute("LOCK TABLES `characters_insert_queue` WRITE", con);

	// Load the characters, and assign them their new guids, and insert them into the main db.
	result = CharacterDatabase.FQuery("SELECT * FROM characters_insert_queue", con);

	// Can be unlocked now.
	CharacterDatabase.FWaitExecute("UNLOCK TABLES", con);

	if(result)
	{
		uint32 guid;
		std::stringstream ss;
		do 
		{
			f = result->Fetch();
			char * p = (char*)characterTableFormat;
			uint32 i = 1;
			guid = f[0].GetUInt32();
			uint32 new_guid = objmgr.GenerateLowGuid(HIGHGUID_PLAYER);
			uint32 new_item_guid;
			ss << "INSERT INTO characters VALUES(" << new_guid;

			// create his playerinfo in the server
			PlayerInfo * inf = new PlayerInfo();
			inf->acct = f[1].GetUInt32();

			while(*p != 0)
			{
				switch(*p)
				{
				case 's':
					ss << ",'" << CharacterDatabase.EscapeString(f[i].GetString(), con) << "'";
					break;

				case 'f':
					ss << ",'" << f[i].GetFloat() << "'";
					break;

				case 'S':
					{
						// this is the character name, append a hex version of the guid to it to prevent name clashes.
						char newname[100];
						snprintf(newname,20,"%5s%X",f[i].GetString(),new_guid);
						ss << ",'" << CharacterDatabase.EscapeString(newname,con) << "'";
						inf->name = strdup(newname);
					}break;

				case 'U':
					{
						// this is our forced rename field. force it to one.
						ss << ",1";
					}break;

				default:
					ss << "," << f[i].GetUInt32();
					break;
				}

				++i;
				++p;
			}

			ss << ")";
			CharacterDatabase.FWaitExecute(ss.str().c_str(),con);

			inf->cl = f[4].GetUInt32();
			inf->gender = f[5].GetUInt32();
			inf->guid = new_guid;
			inf->lastLevel = f[7].GetUInt32();
			inf->lastOnline = UNIXTIME;
			inf->lastZone = 0;
			inf->m_Group=NULL;
			inf->m_loggedInPlayer=NULL;
			inf->guild=NULL;
			inf->guildRank=NULL;
			inf->guildMember=NULL;
			inf->race=f[3].GetUInt32();
			inf->subGroup=0;
			switch(inf->race)
			{
			case RACE_HUMAN:
			case RACE_GNOME:
			case RACE_DWARF:
			case RACE_NIGHTELF:
			case RACE_DRAENEI:
				inf->team=0;
				break;

			default:
				inf->team=1;
				break;
			}
			
			// add playerinfo to objectmgr
			objmgr.AddPlayerInfo(inf);

			// grab all his items, assign them their new guids and insert them
			itr = itemMap.find(guid);
			if(itr != itemMap.end())
			{
				for(vector<insert_playeritem>::iterator vtr = itr->second.begin(); vtr != itr->second.end(); ++vtr)
				{
					ss.rdbuf()->str("");
					ss << "INSERT INTO playeritems VALUES(";
					new_item_guid = objmgr.GenerateLowGuid(HIGHGUID_ITEM);
					ss << new_guid << ","
						<< new_item_guid << ","
						<< (*vtr).entry << ","
						<< (*vtr).wrapped_item_id << ","
						<< (*vtr).wrapped_creator << ","
						<< (*vtr).creator << ","
						<< (*vtr).count << ","
						<< (*vtr).charges << ","
						<< (*vtr).flags << ","
						<< (*vtr).randomprop << ","
						<< (*vtr).randomsuffix << ","
						<< (*vtr).itemtext << ","
						<< (*vtr).durability << ","
						<< (*vtr).containerslot << ","
						<< (*vtr).slot << ",'"
						<< (*vtr).enchantments << "')";
					CharacterDatabase.FWaitExecute(ss.str().c_str(),con);
				}
			}
			ss.rdbuf()->str("");
		} while(result->NextRow());
		has_results = true;
		delete result;
	}

	// Clear all the data in the tables.
	if(has_results)
	{
		CharacterDatabase.FWaitExecute("DELETE FROM characters_insert_queue", con);
		CharacterDatabase.FWaitExecute("DELETE FROM playeritems_insert_queue", con);
	}
}

void World::DisconnectUsersWithAccount(const char * account, WorldSession * m_session)
{
	SessionMap::iterator itr;
	WorldSession * session;
	m_sessionlock.AcquireReadLock();
	for(itr = m_sessions.begin(); itr != m_sessions.end();)
	{
		session = itr->second;
		++itr;

		if(!stricmp(account, session->GetAccountNameS()))
		{
			m_session->SystemMessage("Disconnecting user with account `%s` IP `%s` Player `%s`.", session->GetAccountNameS(), 
				session->GetSocket() ? session->GetSocket()->GetRemoteIP().c_str() : "noip", session->GetPlayer() ? session->GetPlayer()->GetName() : "noplayer");

			session->Disconnect();
		}
	}
	m_sessionlock.ReleaseReadLock();
}

void World::DisconnectUsersWithIP(const char * ip, WorldSession * m_session)
{
	SessionMap::iterator itr;
	WorldSession * session;
	m_sessionlock.AcquireReadLock();
	for(itr = m_sessions.begin(); itr != m_sessions.end();)
	{
		session = itr->second;
		++itr;

		if(!session->GetSocket())
			continue;

		string ip2 = session->GetSocket()->GetRemoteIP().c_str();
		if(!stricmp(ip, ip2.c_str()))
		{
			m_session->SystemMessage("Disconnecting user with account `%s` IP `%s` Player `%s`.", session->GetAccountNameS(), 
				ip2.c_str(), session->GetPlayer() ? session->GetPlayer()->GetName() : "noplayer");

			session->Disconnect();
		}
	}
	m_sessionlock.ReleaseReadLock();
}

void World::DisconnectUsersWithPlayerName(const char * plr, WorldSession * m_session)
{
	SessionMap::iterator itr;
	WorldSession * session;
	m_sessionlock.AcquireReadLock();
	for(itr = m_sessions.begin(); itr != m_sessions.end();)
	{
		session = itr->second;
		++itr;

		if(!session->GetPlayer())
			continue;

		if(!stricmp(plr, session->GetPlayer()->GetName()))
		{
			m_session->SystemMessage("Disconnecting user with account `%s` IP `%s` Player `%s`.", session->GetAccountNameS(), 
				session->GetSocket() ? session->GetSocket()->GetRemoteIP().c_str() : "noip", session->GetPlayer() ? session->GetPlayer()->GetName() : "noplayer");

			session->Disconnect();
		}
	}
	m_sessionlock.ReleaseReadLock();
}
