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

#include "CConsole.h"
#include "Log.h"
#include "Master.h"
#include "../game/StdAfx.h"
#include "../shared/svn_revision.h"
#include "BaseConsole.h"
LocalConsole MyConsole;

#ifndef WIN32
    #include <termios.h>
#endif

createFileSingleton(Console);
createFileSingleton(CConsole);
void CConsole::Kill()
{
	_thread->kill = true;
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
	WriteConsoleInput( GetStdHandle( STD_INPUT_HANDLE ), ir, 2, & dwTmp );
#endif
	printf( "Waiting for console thread to terminate....\n" );
	while( _thread != NULL )
	{
		Sleep( 100 );
	}
	printf( "Console shut down.\n" );
}

bool CConsoleThread::run()
{
	SetThreadName("Console Interpreter");
	sCConsole._thread = this;
	size_t i = 0;
	size_t len;
	char cmd[300];
#ifndef WIN32
	fd_set fds;
	struct timeval tv;
#endif

	while( kill != true )
	{
		// Make sure our buffer is clean to avoid Array bounds overflow
		memset( cmd, 0, sizeof( cmd ) ); 

#ifdef WIN32
		// Read in single line from "stdin"
		fgets( cmd, 300, stdin );

		if( kill )
			break;

#else
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		if(select(1, &fds, NULL, NULL, &tv) <= 0)
		{
			if(!kill)	// timeout
				continue;
			else
				break;
		}

		fgets( cmd, 300, stdin );
#endif

		len = strlen(cmd);
		for( i = 0; i < len; ++i )
		{
			if(cmd[i] == '\n' || cmd[i] == '\r')
				cmd[i] = '\0';
		}

		HandleConsoleInput(&MyConsole, cmd);
	}
	sCConsole._thread = NULL;
	return true;
}

CConsoleThread::CConsoleThread()
{
	kill = false;
}

CConsoleThread::~CConsoleThread()
{

}

//------------------------------------------------------------------------------
// Protected methods:
//------------------------------------------------------------------------------
// Process one command

static char ConsoleBuffer[65536];

const char * Console::GetLine(uint32 Delay)
{
	if(PollConsole(Delay))
	{
#ifdef WIN32
		DWORD Bytes_Read;
		DWORD Result = ReadFile( GetStdHandle(STD_INPUT_HANDLE), ConsoleBuffer, 65536, &Bytes_Read, NULL );
		if( Bytes_Read != 0 && Result )
			return ConsoleBuffer;
		else
			return NULL;
#else
		struct termios initial_settings, new_settings;
        
		tcgetattr( 0,&initial_settings );
        tcgetattr( 0,&new_settings );
       	new_settings.c_lflag &= ~ICANON;
        new_settings.c_lflag &= ~ECHO;
       	new_settings.c_lflag &= ~ISIG;
	    tcsetattr( 0,TCSANOW,&new_settings );

		int br = read( fileno(stdin), ConsoleBuffer, 65536 );
        
		tcsetattr( 0, TCSANOW, &initial_settings );

		if( br != 0 )
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
	tcgetattr( 0, &initial_settings );
	tcgetattr( 0, &new_settings );
	new_settings.c_lflag &= ~ICANON;
	new_settings.c_lflag &= ~ECHO;
	new_settings.c_lflag &= ~ISIG;
	tcsetattr( 0, TCSANOW, &new_settings );
	
	FD_ZERO( &fds );
	FD_SET( fileno( stdin ), &fds );
	
	tv.tv_sec  = Time / 1000;
	tv.tv_usec = (Time % 1000) * 1000;

	int result = select( 1, &fds, NULL, NULL, &tv );
	tcsetattr( 0, TCSANOW, &initial_settings );
	if( result > 0 )
		return true;
	else
		return false;
#else
	uint32 e = getMSTime() + Time;
	uint32 n = getMSTime();
	while( n < e )
	{
        if( GetAsyncKeyState( VK_RETURN ) != 0 )
			return true;

		Sleep( 100 );
	}
	return false;
#endif
}

bool Console::PollForD()
{
#ifndef WIN32
	const char * buf = GetLine(1000);
	if( buf == NULL || buf[0] != 27 )
		return false;
	return true;
#else
	uint32 e = getMSTime() + 1000;
	uint32 n = getMSTime();
	while( n < e )
	{
		if( GetAsyncKeyState(VK_F1) != 0 )
			return true;

		Sleep( 100 );
		n = getMSTime();
	}
	return false;
#endif
}

bool Console::WaitForSpace()
{
#ifndef WIN32
	const char * buf = GetLine( 100000 );
	if( buf != NULL && buf[0] != ' ' )
		WaitForSpace();

	return true;
#else
	while(true)
	{
		if( GetAsyncKeyState( VK_SPACE ) != 0 )
			return true;

		Sleep( 100 );
	}
	return false;
#endif
}
