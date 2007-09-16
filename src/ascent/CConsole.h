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

#ifndef __CConsole_H
#define __CConsole_H

#include "Common.h"
#include "../game/CThreads.h"

class CConsoleThread : public ThreadBase
{
public:
	bool kill;
	CConsoleThread();
	~CConsoleThread();
	void run();
};

class CConsole :  public Singleton < CConsole >
{
	friend class CConsoleThread;

protected:					// Protected methods:
	CConsoleThread *_thread;

	// Process one command
	void ProcessCmd(char *cmd);

	// ver[sion]
	void TranslateVersion(char *str);
	void ProcessVersion();

	// quit | exit
	void TranslateQuit(char *str);
	void ProcessQuit(int delay);
	void CancelShutdown(char *str);

	// help | ?
	void TranslateHelp(char *str);
	void ProcessHelp(char *command);


	void TranslateThreads(char* str);
	void ObjectStats(char *str);

	// getuptime
	void GetUptime(char *str);

	void WideAnnounce(char *str);
	void Announce(char* str);
	void SaveallPlayers(char *str);
	void Kick(char *str);
	void IPBan(char *str);
	void IPUnBan(char *str);
	void BanAccount(char *str);
	void PlayerInfo(char *str);
	void ReloadGMScripts(char * str);
public:
	void Kill();
};

#define sCConsole CConsole::getSingleton()

class Console : public Singleton<Console>
{
public:
	const char * GetLine(uint32 Delay);
	bool PollConsole(uint32 Time);
	bool PollForD();		// Used at startup :P
	bool WaitForSpace();
};

#define sConsole Console::getSingleton()

#endif
