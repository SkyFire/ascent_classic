/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Master.h"
#include "CConsole.h"
#include "../shared/CrashHandler.h"
#include "../game/StdAfx.h"
#include "../shared/ascent_getopt.h"

#ifdef HOARD
#define BANNER "Ascent r%u/%s-%s-%s-Hoard :: World Server"
#else
#define BANNER "Ascent r%u/%s-%s-%s :: World Server"
#endif

#ifndef WIN32
#include <sched.h>
#endif

#include "../shared/svn_revision.h"

#include "../game/WorldSession.h"
#include "LogonCommClient.h"

#include <signal.h>

createFileSingleton(Master);
std::string LogFileName;
bool bLogChat;
bool crashed = false;

volatile bool Master::m_stopEvent = false;

// Database defines.
SERVER_DECL Database* Database_Character;
SERVER_DECL Database* Database_World;

// mainserv defines
SessionLogWriter * GMCommand_Log;
SessionLogWriter * Anticheat_Log;
SessionLogWriter * Player_Log;
extern DayWatcherThread * dw;

void Master::_OnSignal(int s)
{
	switch (s)
	{
#ifndef WIN32
	case SIGHUP:
		sWorld.Rehash(true);
		break;
#endif
	case SIGINT:
	case SIGTERM:
	case SIGABRT:
#ifdef _WIN32
	case SIGBREAK:
#endif
		Master::m_stopEvent = true;
		break;
	}

	signal(s, _OnSignal);
}

Master::Master()
{
	m_ShutdownTimer = 0;
	m_ShutdownEvent = false;
	m_restartEvent = false;
}

Master::~Master()
{
}

struct Addr
{
	unsigned short sa_family;
	/* sa_data */
	unsigned short Port;
	unsigned long IP; // inet_addr
	unsigned long unusedA;
	unsigned long unusedB;
};

#define DEF_VALUE_NOT_SET 0xDEADBEEF

#ifdef WIN32
        static const char * default_config_file = "ascent.conf";
        static const char * default_realm_config_file = "realms.conf";
#else
        static const char * default_config_file = CONFDIR "/ascent.conf";
        static const char * default_realm_config_file = CONFDIR "/realms.conf";
#endif


bool Master::Run(int argc, char ** argv)
{
	char * config_file = (char*)default_config_file;
	char * realm_config_file = (char*)default_realm_config_file;

	int file_log_level = DEF_VALUE_NOT_SET;
	int screen_log_level = DEF_VALUE_NOT_SET;
	int do_check_conf = 0;
	int do_version = 0;

	struct ascent_option longopts[] =
	{
		{ "checkconf",			ascent_no_argument,				&do_check_conf,			1		},
		{ "screenloglevel",		ascent_required_argument,		&screen_log_level,		1		},
		{ "fileloglevel",		ascent_required_argument,		&file_log_level,		1		},
		{ "version",			ascent_no_argument,				&do_version,			1		},
		{ "conf",				ascent_required_argument,		NULL,					'c'		},
		{ "realmconf",			ascent_required_argument,		NULL,					'r'		},
		{ 0, 0, 0, 0 }
	};

	char c;
	while ((c = ascent_getopt_long_only(argc, argv, ":f:", longopts, NULL)) != -1)
	{
		switch (c)
		{
		case 'c':
			config_file = new char[strlen(ascent_optarg)];
			strcpy(config_file, ascent_optarg);
			break;

		case 'r':
			realm_config_file = new char[strlen(ascent_optarg)];
			strcpy(realm_config_file, ascent_optarg);
			break;

		case 0:
			break;
		default:
			sLog.m_fileLogLevel = -1;
			sLog.m_screenLogLevel = 3;
			printf("Usage: %s [--checkconf] [--screenloglevel <level>] [--fileloglevel <level>] [--conf <filename>] [--realmconf <filename>] [--version]\n", argv[0]);
			return true;
		}
	}

	// Startup banner
	UNIXTIME = time(NULL);
	if(!do_version && !do_check_conf)
	{
		sLog.Init(-1, 3);
	}
	else
	{
		sLog.m_fileLogLevel = -1;
		sLog.m_screenLogLevel = 3;
	}

	printf(BANNER, g_getRevision(), CONFIG, PLATFORM_TEXT, ARCH);
	printf("\nCopyright (C) 2005-2007 Ascent Team. http://www.ascentemu.com/\n");
	printf("This program comes with ABSOLUTELY NO WARRANTY, and is FREE SOFTWARE.\n");
	printf("You are welcome to redistribute it under the terms of the GNU General\n");
	printf("Public License, either version 3 or any later version. For a copy of\n");
	printf("this license, see the COPYING file provided with this distribution.\n");
	Log.Line();

	if(do_version)
		return true;

	if(do_check_conf)
	{
		Log.Notice("Config", "Checking config file: %s", config_file);
		if(Config.MainConfig.SetSource(config_file, true))
			Log.Success("Config", "Passed without errors.");
		else
			Log.Warning("Config", "Encountered one or more errors.");

		Log.Notice("Config", "Checking config file: %s\n", realm_config_file);
		if(Config.RealmConfig.SetSource(realm_config_file, true))
			Log.Success("Config", "Passed without errors.\n");
		else
			Log.Warning("Config", "Encountered one or more errors.\n");

		/* test for die variables */
		string die;
		if(Config.MainConfig.GetString("die", "msg", &die) || Config.MainConfig.GetString("die2", "msg", &die))
			Log.Warning("Config", "Die directive received: %s", die.c_str());

		return true;
	}

	printf("The key combination <Ctrl-C> will safely shut down the server at any time.\n");
	Log.Line();
    
	uint32 seed = time(NULL);
	new MTRand(seed);
	srand(seed);
	Log.Success("MTRand", "Initialized Random Number Generators.");

	ThreadPool.Startup();
	uint32 LoadingTime = getMSTime();

	Log.Notice("Config", "Loading Config Files...\n");
	if(Config.MainConfig.SetSource(config_file))
		Log.Success("Config", ">> ascent.conf");
	else
	{
		Log.Error("Config", ">> ascent.conf");
		return false;
	}

	string die;
	if(Config.MainConfig.GetString("die", "msg", &die) || Config.MainConfig.GetString("die2", "msg", &die))
	{
		Log.Warning("Config", "Die directive received: %s", die.c_str());
		return false;
	}	

	if(Config.RealmConfig.SetSource(realm_config_file))
		Log.Success("Config", ">> realms.conf");
	else
	{
		Log.Error("Config", ">> realms.conf");
		return false;
	}

	if(!_StartDB())
	{
		return false;
	}

	/*Log.Color(TWHITE);
	int left = 3;
	bool dodb = false;
	printf("\nHit F1 within the next 3 seconds to enter database maintenance mode.");
	fflush(stdout);
	while(left)
	{
        dodb = sConsole.PollForD();
		if(dodb) break;
		left--;
		printf(".");
		fflush(stdout);
	}

	if(dodb)
	{
		Log.Color(TNORMAL);
		printf("\nEntering database maintenance mode.\n\n");
		new DatabaseCleaner;
		DatabaseCleaner::getSingleton().Run();
		delete DatabaseCleaner::getSingletonPtr();
		Log.Color(TYELLOW);
		printf("\nMaintenence finished. Take a moment to review the output, and hit space to continue startup.");
		Log.Color(TNORMAL);
		fflush(stdout);
		sConsole.WaitForSpace();
	}
	else
		Log.Color(TNORMAL);*/

	Log.Line();
	sLog.outString("");

	ScriptSystem = new ScriptEngine;
	ScriptSystem->Reload();

	new EventMgr;
	new World;

	// open cheat log file
	Anticheat_Log = new SessionLogWriter(FormatOutputString("logs", "cheaters", false).c_str(), false);
	GMCommand_Log = new SessionLogWriter(FormatOutputString("logs", "gmcommand", false).c_str(), false);
	Player_Log = new SessionLogWriter(FormatOutputString("logs", "players", false).c_str(), false);

	/* load the config file */
	sWorld.Rehash(false);

	/* set new log levels */
	if(screen_log_level != (int)DEF_VALUE_NOT_SET)
		sLog.SetScreenLoggingLevel(screen_log_level);
	
	if(file_log_level != (int)DEF_VALUE_NOT_SET)
		sLog.SetFileLoggingLevel(file_log_level);

	// Initialize Opcode Table
	WorldSession::InitPacketHandlerTable();

	string host = Config.MainConfig.GetStringDefault("Listen", "Host", DEFAULT_HOST);
	int wsport = Config.MainConfig.GetIntDefault("Listen", "WorldServerPort", DEFAULT_WORLDSERVER_PORT);

	new ScriptMgr;

	sWorld.SetInitialWorldSettings();
	sWorld.SetStartTime((uint32)time(NULL));
	
	_HookSignals();

	ThreadPool.ExecuteTask(new CConsoleThread);

	uint32 realCurrTime, realPrevTime;
	realCurrTime = realPrevTime = getMSTime();

	// Socket loop!
	uint32 start;
	uint32 diff;
	uint32 last_time = now();
	uint32 etime;
	uint32 next_printout = getMSTime(), next_send = getMSTime();

	// Start Network Subsystem
	sLog.outString("Starting network subsystem...");
	new SocketMgr;
	new SocketGarbageCollector;
	sSocketMgr.SpawnWorkerThreads();

	sScriptMgr.LoadScripts();


	LoadingTime = getMSTime() - LoadingTime;
	sLog.outString ("\nServer is ready for connections. Startup time: %ums\n", LoadingTime );
 
	/* write pid file */
	FILE * fPid = fopen("ascent.pid", "w");
	if(fPid)
	{
		uint32 pid;
#ifdef WIN32
		pid = GetCurrentProcessId();
#else
		pid = getpid();
#endif
		fprintf(fPid, "%u", (unsigned int)pid);
		fclose(fPid);
	}
#ifdef WIN32
	HANDLE hThread = GetCurrentThread();
#endif

	uint32 loopcounter=0;
	ThreadPool.Gobble();

#ifndef CLUSTERING
	/* Connect to realmlist servers / logon servers */
	new LogonCommHandler();
	sLogonCommHandler.Startup();

	/* voicechat */
	new VoiceChatHandler();
	sVoiceChatHandler.Startup();

	// Create listener
	ListenSocket<WorldSocket> * ls = new ListenSocket<WorldSocket>(host.c_str(), wsport);
    bool listnersockcreate = ls->IsOpen();
	while(!m_stopEvent && listnersockcreate)
#else
	new ClusterInterface;
	sClusterInterface.ConnectToRealmServer();
	while(!m_stopEvent)
#endif
	{
		start = now();
		diff = start - last_time;
		if(! ((++loopcounter) % 3600) )		// 3mins
		{
			ThreadPool.ShowStats();
			ThreadPool.IntegrityCheck();
		}

#ifndef CLUSTERING
		sLogonCommHandler.UpdateSockets();
		sVoiceChatHandler.Update();
#ifdef WIN32
		ls->Update();
#endif
#else
		sClusterInterface.Update();
#endif
		sSocketGarbageCollector.Update();

		/* UPDATE */
		last_time = now();
		etime = last_time - start;
		if(m_ShutdownEvent)
		{
			if(getMSTime() >= next_printout)
			{
				if(m_ShutdownTimer > 60000.0f)
				{
					if(!((int)(m_ShutdownTimer)%60000))
						sLog.outString("Server shutdown in %i minutes.", (int)(m_ShutdownTimer / 60000.0f));
				}
				else
					sLog.outString("Server shutdown in %i seconds.", (int)(m_ShutdownTimer / 1000.0f));
					
				next_printout = getMSTime() + 500;
			}
			if(getMSTime() >= next_send)
			{
				// broadcast packet.
				WorldPacket data(20);
				data.SetOpcode(SMSG_SERVER_MESSAGE);
				data << uint32(SERVER_MSG_SHUTDOWN_TIME);
				int time = m_ShutdownTimer / 1000;
				if(time > 0)
				{
					int mins = 0, secs = 0;
					if(time > 60)
						mins = time / 60;
					if(mins)
						time -= (mins*60);
					secs = time;
					char str[20];
					snprintf(str, 20, "%02u:%02u", mins, secs);
					data << str;
					sWorld.SendGlobalMessage(&data, NULL);
				}
				next_send = getMSTime() + 1000;
			}
			if(diff >= m_ShutdownTimer)
				break;
			else
				m_ShutdownTimer -= diff;
		}
		
		Database_Character->CheckConnections();
		Database_World->CheckConnections();
		sWorld.UpdateQueuedSessions(diff);

		if(50 > etime)
		{
#ifdef WIN32
			WaitForSingleObject(hThread, 50 - etime);
#else
			Sleep(50-etime);
#endif
		}
	}
	_UnhookSignals();

	ThreadPool.ShowStats();
	/* Shut down console system */
	sCConsole.Kill();

	sLog.outString("Killing all sockets and network subsystem.");
#ifndef CLUSTERING
	ls->Close();
	delete ls;
#endif
#ifdef WIN32
	sSocketMgr.ShutdownThreads();
#endif
	sSocketMgr.CloseAll();

	// begin server shutdown
	time_t st = time(NULL);
	sLog.outString("Server shutdown initiated at %s", ctime(&st));

	// send a query to wake it up if its inactive
	sLog.outString("Executing pending database queries and closing database thread...");
	// kill the database thread first so we don't lose any queries/data
	CharacterDatabase.EndThreads();
	WorldDatabase.EndThreads();

	sLog.outString("All pending database operations cleared.\n");
	Log.Notice("DayWatcherThread", "Exiting...");
	dw->terminate();
	dw = NULL;

	ThreadPool.Shutdown();

	sWorld.SaveAllPlayers();
	sLog.outString("");

	delete LogonCommHandler::getSingletonPtr();

	sWorld.ShutdownClasses();
	sLog.outString("\nDeleting World...");
	delete World::getSingletonPtr();
	sScriptMgr.UnloadScripts();
	delete ScriptMgr::getSingletonPtr();

	sLog.outString("Deleting Event Manager...");
	delete EventMgr::getSingletonPtr();

	sLog.outString("Terminating MySQL connections...\n");
	_StopDB();

	sLog.outString("Deleting Network Subsystem...");
	delete SocketMgr::getSingletonPtr();
	delete SocketGarbageCollector::getSingletonPtr();

	sLog.outString("Deleting Script Engine...");
	delete ScriptSystem;

	delete GMCommand_Log;
	delete Anticheat_Log;
	delete Player_Log;

	// remove pid
	remove("ascent.pid");

	sLog.outString("\nServer shutdown completed successfully.\n");

#ifdef WIN32
	WSACleanup();

	// Terminate Entire Application
	//HANDLE pH = OpenProcess(PROCESS_TERMINATE, TRUE, GetCurrentProcessId());
	//TerminateProcess(pH, 0);
	//CloseHandle(pH);

#endif

	return true;
}

bool Master::_StartDB()
{
	string hostname, username, password, database;
	int port = 0;
	int type = 1;
	//string lhostname, lusername, lpassword, ldatabase;
	//int lport = 0;
	//int ltype = 1;
	// Configure Main Database
	
	bool result = Config.MainConfig.GetString("WorldDatabase", "Username", &username);
	Config.MainConfig.GetString("WorldDatabase", "Password", &password);
	result = !result ? result : Config.MainConfig.GetString("WorldDatabase", "Hostname", &hostname);
	result = !result ? result : Config.MainConfig.GetString("WorldDatabase", "Name", &database);
	result = !result ? result : Config.MainConfig.GetInt("WorldDatabase", "Port", &port);
	result = !result ? result : Config.MainConfig.GetInt("WorldDatabase", "Type", &type);
	Database_World = new Database();

	if(result == false)
	{
		sLog.outError("sql: One or more parameters were missing from WorldDatabase directive.");
		return false;
	}

	// Initialize it
	if(!WorldDatabase.Initialize(hostname.c_str(), (unsigned int)port, username.c_str(),
		password.c_str(), database.c_str(), Config.MainConfig.GetIntDefault("WorldDatabase", "ConnectionCount", 3),
		16384))
	{
		sLog.outError("sql: Main database initialization failed. Exiting.");
		return false;
	}


	result = Config.MainConfig.GetString("CharacterDatabase", "Username", &username);
	Config.MainConfig.GetString("CharacterDatabase", "Password", &password);
	result = !result ? result : Config.MainConfig.GetString("CharacterDatabase", "Hostname", &hostname);
	result = !result ? result : Config.MainConfig.GetString("CharacterDatabase", "Name", &database);
	result = !result ? result : Config.MainConfig.GetInt("CharacterDatabase", "Port", &port);
	result = !result ? result : Config.MainConfig.GetInt("CharacterDatabase", "Type", &type);
	Database_Character = new Database();

	if(result == false)
	{
		sLog.outError("sql: One or more parameters were missing from Database directive.");
		return false;
	}

	// Initialize it
	if(!CharacterDatabase.Initialize(hostname.c_str(), (unsigned int)port, username.c_str(),
		password.c_str(), database.c_str(), Config.MainConfig.GetIntDefault("CharacterDatabase", "ConnectionCount", 3),
		16384))
	{
		sLog.outError("sql: Main database initialization failed. Exiting.");
		return false;
	}

	return true;
}

void Master::_StopDB()
{
	CharacterDatabase.Shutdown();
	WorldDatabase.Shutdown();
	delete Database_World;
	delete Database_Character;
}

void Master::_HookSignals()
{
	signal(SIGINT, _OnSignal);
	signal(SIGTERM, _OnSignal);
	signal(SIGABRT, _OnSignal);
#ifdef _WIN32
	signal(SIGBREAK, _OnSignal);
#else
	signal(SIGHUP, _OnSignal);
#endif
}

void Master::_UnhookSignals()
{
	signal(SIGINT, 0);
	signal(SIGTERM, 0);
	signal(SIGABRT, 0);
#ifdef _WIN32
	signal(SIGBREAK, 0);
#else
	signal(SIGHUP, 0);
#endif

}

#ifdef WIN32

Mutex m_crashedMutex;

// Crash Handler
void OnCrash(bool Terminate)
{
	sLog.outString("Advanced crash handler initialized.");
	if(!m_crashedMutex.AttemptAcquire())
		TerminateThread(GetCurrentThread(), 0);

	try
	{
		if(World::getSingletonPtr() != 0 )
		{
			sLog.outString("Waiting for all database queries to finish...");
			WorldDatabase.EndThreads();
			CharacterDatabase.EndThreads();
			sLog.outString("All pending database operations cleared.\n");
			sWorld.SaveAllPlayers();
			sLog.outString("Data saved.");
		}
	}
	catch(...)
	{
		sLog.outString("Threw an exception while attempting to save all data.");
	}

	sLog.outString("Closing.");
	
	// beep
	//printf("\x7");
	
	// Terminate Entire Application
	if(Terminate)
	{
		HANDLE pH = OpenProcess(PROCESS_TERMINATE, TRUE, GetCurrentProcessId());
		TerminateProcess(pH, 1);
		CloseHandle(pH);
	}
}

#endif
