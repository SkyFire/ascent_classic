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

#include "CConsole.h"
#include "Log.h"
#include "Master.h"
#include "../game/StdAfx.h"
#include "../shared/svn_revision.h"

#ifndef WIN32
    #include <termios.h>
#endif

createFileSingleton(Console);
createFileSingleton(CConsole);
void CConsole::Kill()
{
#ifdef WIN32
	/* write the return keydown/keyup event */
	DWORD dwTmp;
	INPUT_RECORD ir[2];
	ir[0].EventType = KEY_EVENT;
	ir[0].Event.KeyEvent.bKeyDown = TRUE;
	ir[0].Event.KeyEvent.dwControlKeyState = 288;
	ir[0].Event.KeyEvent.uChar.AsciiChar = 13;
	ir[0].Event.KeyEvent.wRepeatCount = 1;
	ir[0].Event.KeyEvent.wVirtualKeyCode = 13;
	ir[0].Event.KeyEvent.wVirtualScanCode = 28;
	ir[1].EventType = KEY_EVENT;
	ir[1].Event.KeyEvent.bKeyDown = FALSE;
	ir[1].Event.KeyEvent.dwControlKeyState = 288;
	ir[1].Event.KeyEvent.uChar.AsciiChar = 13;
	ir[1].Event.KeyEvent.wRepeatCount = 1;
	ir[1].Event.KeyEvent.wVirtualKeyCode = 13;
	ir[1].Event.KeyEvent.wVirtualScanCode = 28;
	_thread->kill=true;
	WriteConsoleInput (GetStdHandle(STD_INPUT_HANDLE), ir, 2, & dwTmp);
	printf("Waiting for console thread to terminate....\n");
	while(_thread != NULL)
	{
		Sleep(100);
	}
	printf("Console shut down.\n");
#endif
}
void CConsoleThread::run()
{
	SetThreadName("Console Interpreter");
	sCConsole._thread = this;
	size_t i = 0;
	char cmd[96];

	while (kill != true)
	{
		// Make sure our buffer is clean to avoid Array bounds overflow
		memset(cmd,0,sizeof(cmd)); 
		// Read in single line from "stdin"
		fgets(cmd, 80, stdin);

		if(kill)
			break;

		for( i = 0 ; i < 80 || cmd[i] != '\0' ; i++ )
		{
			if( cmd[i] =='\n' )
			{
				cmd[i]='\0';
				sCConsole.ProcessCmd(cmd);
				fflush(stdin);
				break;
			}
		}
	}
	sCConsole._thread=NULL;
}

//------------------------------------------------------------------------------
// Protected methods:
//------------------------------------------------------------------------------
// Process one command
void CConsole::ProcessCmd(char *cmd)
{
	typedef void (CConsole::*PTranslater)(char *str);
	struct SCmd
	{
		const char *name;
		PTranslater tr;
	};

	SCmd cmds[] =
	{
		{ "?", &CConsole::TranslateHelp}, { "help", &CConsole::TranslateHelp},
		{ "ver", &CConsole::TranslateVersion}, { "version", &CConsole::TranslateVersion},
		{ "uptime", &CConsole::GetUptime},
		{ "threads", &CConsole::TranslateThreads},
		{ "cancelshutdown", &CConsole::CancelShutdown },
		{ "status", &CConsole::ObjectStats },
		{ "announce", &CConsole::Announce },
		{ "wannounce", &CConsole::WideAnnounce },
		{ "saveall", &CConsole::SaveallPlayers },
		{ "quit", &CConsole::TranslateQuit}, { "exit", &CConsole::TranslateQuit}, 
		{ "kick", &CConsole::Kick},
		{ "banaccount", &CConsole::BanAccount},
		{ "banip", &CConsole::IPBan},
		{ "unbanip", &CConsole::IPUnBan},
		{ "playerinfo", &CConsole::PlayerInfo},
		{ "reloadscripts", &CConsole::ReloadGMScripts},
	};

	char cmd2[80];
	strcpy(cmd2, cmd);
	for(size_t i = 0; i < strlen(cmd); ++i)
		cmd2[i] = tolower(cmd[i]);

	for (size_t i = 0; i < sizeof(cmds)/sizeof(SCmd); i++)
		if (strncmp(cmd2, cmds[i].name, strlen(cmds[i].name)) == 0)
		{
			(this->*(cmds[i].tr)) (cmd + strlen(cmds[i].name) +1);
			return;
		}

		printf("Console: Unknown console command (use \"help\" for help).\n");
}

void CConsole::CancelShutdown(char *str)
{
	printf("Shutdown aborted.\n");
	WorldPacket data(20);
	data.SetOpcode(SMSG_SERVER_MESSAGE);
	data << uint32(SERVER_MSG_SHUTDOWN_CANCELLED);
	sWorld.SendGlobalMessage(&data);
	sMaster.m_ShutdownEvent = false;
	sMaster.m_ShutdownTimer = 0;
}

void CConsole::GetUptime(char *str)
{
	uint32 count = objmgr._players.size();

	sLog.outString("Console: Server has been running for %s There are currently %d online players.",
		sWorld.GetUptimeString().c_str(), count);
}

//------------------------------------------------------------------------------
// ver[sion]
void CConsole::TranslateVersion(char *str)
{
	ProcessVersion();
}
void CConsole::ProcessVersion()
{
    sLog.outString("Console: Server %s, Rev: %d", _FULLVERSION, g_getRevision());
}
//------------------------------------------------------------------------------
// quit | exit
void CConsole::TranslateQuit(char *str)
{
	int delay = str != NULL ? atoi(str) : 5000;
	if(!delay)
		delay = 5000;
	else
		delay *= 1000;

	ProcessQuit(delay);
}
void CConsole::ProcessQuit(int delay)
{
	sMaster.m_ShutdownTimer = delay;
	sMaster.m_ShutdownEvent = true;
}
//------------------------------------------------------------------------------
// help | ?
void CConsole::TranslateHelp(char *str)
{
	ProcessHelp(NULL);
}
void CConsole::ProcessHelp(char *command)
{
	if (command == NULL)
	{
		sLog.outString("Console:--------help--------");
		sLog.outString("   help, ?: print this text");
		sLog.outString("   uptime: print uptime of the server");
		sLog.outString("   version, ver: print version");
		sLog.outString("   cancelshutdown: cancels the shutdown of the server");
		sLog.outString("   announce: announces a msg to the server.");
		sLog.outString("   wannounce: announces a widescreen msg to the server");
		sLog.outString("   saveall: saves all players");
		sLog.outString("   kick: kicks a player with a reason");
		sLog.outString("   banaccount: bans an account");
		sLog.outString("   banip <address> [duration]: bans an ip");
		sLog.outString("   unbanip <address>: unbans an ip");
		sLog.outString("   playerinfo: gets info on an online player");
		sLog.outString("   reloadscripts: reloads gamemonkey scripts");
		sLog.outString("   quit, exit: close program");
	}
}
//------------------------------------------------------------------------------

CConsoleThread::CConsoleThread()
{
	kill=false;
}

void CConsole::TranslateThreads(char* str)
{
	std::string threads = sThreadMgr.ShowStatus();
	printf(threads.c_str());
}

CConsoleThread::~CConsoleThread()
{

}

void CConsole::ObjectStats(char *str)
{
	printf("\n");
	printf("Loaded object information:\n");
	printf("\n");
}

void CConsole::Announce(char* str)
{
	if(!str)
		return;

	char msg[500];
	snprintf(msg, 500, "%sConsole:%s%s", "|cff00ccff", "|r", str);
	sWorld.SendWorldText(msg, 0);
}

void CConsole::BanAccount(char* str)
{
	sLogonCommHandler.LogonDatabaseSQLExecute("UPDATE accounts SET banned = 1 WHERE login = '%s'", str);
	sLog.outString("User %s banned!", str);
	sLogonCommHandler.LogonDatabaseReloadAccounts();
}

void CConsole::IPBan(char* str)
{
	char ip[16] = {0};		// IPv4 address
	uint32 dLength = 0;		// duration of ban, 0 = permanent
	char dType = {0};		// duration type, defaults to minutes ( see convTimePeriod() )

	// we require at least one argument, the network address to ban
	if ( sscanf(str, "%15s %u%c", ip, (unsigned int*)&dLength, &dType) < 1 )
	{
		sLog.outString("usage: banip <address> [duration]");
		return;
	}

	uint32 o1, o2, o3, o4;
	if ( sscanf(ip, "%3u.%3u.%3u.%3u", (unsigned int*)&o1, (unsigned int*)&o2, (unsigned int*)&o3, (unsigned int*)&o4) != 4
			|| o1 > 255 || o2 > 255 || o3 > 255 || o4 > 255)
	{
		sLog.outString("Invalid IPv4 address [%s]", ip);
		return;
	}

	time_t expire_time;
	if ( dLength == 0)		// permanent ban
		expire_time = 0;
	else
	{
		time_t dPeriod = convTimePeriod(dLength, dType);
		if ( dPeriod == 0)
		{
			sLog.outString("Invalid ban duration");
			return;
		}
		time( &expire_time );
		expire_time += dPeriod;
	}

	sLog.outString("Adding [%s] to IP ban table, expires: %s", ip, (expire_time == 0)? "Never" : ctime( &expire_time ));
	sLogonCommHandler.LogonDatabaseSQLExecute("REPLACE INTO ipbans VALUES ('%s', %u);", WorldDatabase.EscapeString(ip).c_str(), (uint32)expire_time);
	sLogonCommHandler.LogonDatabaseReloadAccounts();
}

void CConsole::IPUnBan(char* str)
{
	char ip[16] = {0};		// IPv4 address

	// we require at least one argument, the network address to unban
	if ( sscanf(str, "%15s", ip) < 1)
	{
		sLog.outString("usage: unbanip <address>");
		return;
	}

	/**
	 * We can afford to be less fussy with the validty of the IP address given since
	 * we are only attempting to remove it.
	 * Sadly, we can only blindly execute SQL statements on the logonserver so we have
	 * no idea if the address existed and so the account/IPBanner cache requires reloading.
	 */

	sLog.outString("Removing [%s] from IP ban table if it exists", ip);
	sLogonCommHandler.LogonDatabaseSQLExecute("DELETE FROM ipbans WHERE ip = '%s';", WorldDatabase.EscapeString(ip).c_str());
	sLogonCommHandler.LogonDatabaseReloadAccounts();
	return;
}

void CConsole::PlayerInfo(char* str)
{
	char player[100];
	if(sscanf(str, "%s", player) != 1)
		return;

	Player * _plr = objmgr.GetPlayer(player, false);
	if(!_plr)
	{
		sLog.outString("Cannot find online player %s", str);
		return;
	}
	
	if(!_plr) return;
	if(!_plr->GetSession())
	{
		sLog.outString("ERROR: this player hasn't got any session !");
		return;
	}

	WorldSession* sess = _plr->GetSession();

		static const char* _classes[12] =
	{"None","Warrior", "Paladin", "Hunter", "Rogue", "Priest", "None", "Shaman", "Mage", "Warlock", "None", "Druid"};
	static const char* _races[12] =
	{"None","Human","Orc","Dwarf","Night Elf","Undead","Tauren","Gnome","Troll","None","Blood Elf","Draenei"};


	sLog.outColor(TGREEN, "Name: ");
	sLog.outColor(TNORMAL, "%s\n", _plr->GetName());
	sLog.outColor(TGREEN, "Account: ");
	sLog.outColor(TNORMAL, "%s\n", sess->GetAccountName().c_str());
	sLog.outColor(TGREEN, "Level: ");
	sLog.outColor(TNORMAL, "%d\n",  _plr->getLevel());
	sLog.outColor(TGREEN, "Race: ");
	sLog.outColor(TNORMAL, "%s\n", _races[_plr->getRace()]);
	sLog.outColor(TGREEN, "Class: ");
	sLog.outColor(TNORMAL, "%s\n", _classes[_plr->getClass()]);
	sLog.outColor(TGREEN, "Map: ");
	sLog.outColor(TNORMAL, "%d\n",  _plr->GetMapId());
	sLog.outColor(TGREEN, "Banned: ");
	sLog.outColor(TNORMAL, "%s\n",  (_plr->IsBanned() ? "Yes" : "No"));
}

void CConsole::Kick(char* str)
{
	char player[100];
	char reason[256];
	if(sscanf(str, "%s %s", player, reason) != 2)
		return;

	Player * _plr = objmgr.GetPlayer(player, false);
	if(!_plr)
	{
		sLog.outColor(TRED, "Unable to find player %s\n", player);
		return;
	}

	_plr->BroadcastMessage("|cff00ccffYou have been kicked for |cffff0000%s", reason);
	_plr->Kick(6000);


	
}

void CConsole::WideAnnounce(char *str)
{
	if(!str)
		return;

	char msg[500];
	snprintf(msg, 500, "%sConsole:%s%s", "|cff00ccff", "|r", str);
	sWorld.SendWorldText(msg, 0);
	sWorld.SendWorldWideScreenText(msg, 0);
}

void CConsole::SaveallPlayers(char *str)
{
	PlayerStorageMap::const_iterator itr;
	uint32 stime = now();
	uint32 count = 0;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		if(itr->second->GetSession())
		{
			itr->second->SaveToDB(false);
			count++;
		}
	}
	objmgr._playerslock.ReleaseReadLock();
	char msg[100];
	snprintf(msg, 100, "Saved all %d online players in %d msec.", (unsigned int)count, (unsigned int)((uint32)now() - stime));
	sWorld.SendWorldText(msg);
	sWorld.SendWorldWideScreenText(msg);
}

void CConsole::ReloadGMScripts(char * str)
{
	ScriptSystem->Reload();
}

static char ConsoleBuffer[65536];

const char * Console::GetLine(uint32 Delay)
{
	if(PollConsole(Delay))
	{
#ifdef WIN32
		DWORD Bytes_Read;
		DWORD Result = ReadFile(GetStdHandle(STD_INPUT_HANDLE), ConsoleBuffer, 65536, &Bytes_Read, NULL);
		if(Bytes_Read != 0 && Result)
			return ConsoleBuffer;
		else
			return NULL;
#else
		struct termios initial_settings, new_settings;
        	tcgetattr(0,&initial_settings);
        	tcgetattr(0,&new_settings);
       		new_settings.c_lflag &= ~ICANON;
        	new_settings.c_lflag &= ~ECHO;
       		new_settings.c_lflag &= ~ISIG;
	        tcsetattr(0,TCSANOW,&new_settings);

		int br = read(fileno(stdin), ConsoleBuffer, 65536);
        	tcsetattr(0,TCSANOW,&initial_settings);

		if(br != 0)
			return ConsoleBuffer;
#endif
	}
	return NULL;
}

bool Console::PollConsole(uint32 Time)
{
	/* This is platform-dependant, unfortunately due to window's gayness of treating file descriptors differently. */
#ifndef WIN32
	fd_set fds;
	timeval tv;

	struct termios initial_settings, new_settings;
	tcgetattr(0,&initial_settings);
	tcgetattr(0,&new_settings);
	new_settings.c_lflag &= ~ICANON;
	new_settings.c_lflag &= ~ECHO;
	new_settings.c_lflag &= ~ISIG;
	tcsetattr(0,TCSANOW,&new_settings);
	
	FD_ZERO(&fds);
	FD_SET(fileno(stdin), &fds);
	
	tv.tv_sec  = Time / 1000;
	tv.tv_usec = (Time % 1000) * 1000;

	int result = select(1, &fds, NULL, NULL, &tv);
	tcsetattr(0,TCSANOW,&initial_settings);
	if(result > 0)
		return true;
	else
		return false;
#else
	uint32 e = getMSTime() + Time;
	uint32 n = getMSTime();
	while(n < e)
	{
        if(GetAsyncKeyState(VK_RETURN) != 0)
			return true;

		Sleep(100);
	}
	return false;
#endif
}

bool Console::PollForD()
{
#ifndef WIN32
	const char * buf = GetLine(1000);
	if(!buf || buf[0] != 27)
		return false;
	return true;
#else
	uint32 e = getMSTime() + 1000;
	uint32 n = getMSTime();
	while(n < e)
	{
		if(GetAsyncKeyState(VK_F1) != 0)
			return true;

		Sleep(100);
		n = getMSTime();
	}
	return false;
#endif
}

bool Console::WaitForSpace()
{
#ifndef WIN32
	const char * buf = GetLine(100000);
	if(buf && buf[0] != ' ')
		WaitForSpace();

	return true;

#else
	while(true)
	{
		if(GetAsyncKeyState(VK_SPACE) != 0)
			return true;

		Sleep(100);
	}
	return false;
#endif
}
