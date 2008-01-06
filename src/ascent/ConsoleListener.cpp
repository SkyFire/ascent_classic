#include <Common.h>
#include <Network/Network.h>
#include <svn_revision.h>

#include "BaseConsole.h"
#include "ConsoleCommands.h"
#define LOCAL_BUFFER_SIZE 2048
//#define ENABLE_REMOTE_CONSOLE 1

enum STATES
{
	STATE_USER		= 1,
	STATE_PASSWORD	= 2,
	STATE_LOGGED	= 3,
};

class ConsoleSocket : public Socket
{
	RemoteConsole * m_pConsole;
	char * m_pBuffer;
	uint32 m_pBufferLen;
	uint32 m_pBufferPos;
	uint32 m_state;
	string m_username;
	string m_password;

public:
	ConsoleSocket(SOCKET iFd);
	~ConsoleSocket();

	void OnRead();
	void OnDisconnect();
	void OnConnect();
	void TryAuthenticate();
};

ListenSocket<ConsoleSocket> * g_pListenSocket;

bool StartConsoleListener( )
{
#ifndef ENABLE_REMOTE_CONSOLE
	return false;
#else
	g_pListenSocket = new ListenSocket<ConsoleSocket>( "0.0.0.0", 8000 );
	if( g_pListenSocket == NULL )
		return false;

	if( !g_pListenSocket->IsOpen( ) )
	{
		g_pListenSocket->Close( );
		delete g_pListenSocket;
		return false;
	}

	return true;
#endif
}

ThreadBase * GetConsoleListener()
{
	return (ThreadBase*)g_pListenSocket;
}

ConsoleSocket::ConsoleSocket( SOCKET iFd ) : Socket(iFd, 10000, 1000)
{
	m_pBufferLen = LOCAL_BUFFER_SIZE;
	m_pBufferPos = 0;
	m_pBuffer = new char[LOCAL_BUFFER_SIZE];
	m_pConsole = new RemoteConsole(this);
	m_state = STATE_USER;
}

ConsoleSocket::~ConsoleSocket( )
{
	if( m_pBuffer != NULL )
		delete [] m_pBuffer;

	if( m_pConsole != NULL )
		delete m_pConsole;
}

void ConsoleSocket::OnRead()
{
	uint32 readlen = GetReadBufferSize();
	uint32 len;
	char * p;
	if( ( readlen + m_pBufferPos ) >= m_pBufferLen )
	{
		Disconnect();
		return;
	}

	Read( readlen, (uint8*)&m_pBuffer[m_pBufferPos] );
	m_pBufferPos += readlen;

	// let's look for any newline bytes.
	p = strchr(m_pBuffer, '\n');
	while( p != NULL )
	{
		// windows is stupid. :P
		len = (uint32)((p+1) - m_pBuffer);
		if( *(p-1) == '\r' )
			*(p-1) = '\0';

		*p = '\0';

		// handle the command
		if( *m_pBuffer != '\0' )
		{
			switch(m_state)
			{
			case STATE_USER:
				m_username = string(m_pBuffer);
				m_pConsole->Write("password: ");
				m_state = STATE_PASSWORD;
				break;

			case STATE_PASSWORD:
				m_password = string(m_password);
				m_pConsole->Write("\r\nAttempting to authenticate. Please wait.\r\n");
				m_pConsole->Write("\r\nYou are now logged in under user `%s` with password `%s`.\r\n", m_username.c_str(), m_password.c_str());

				m_state = STATE_LOGGED;
				break;

			case STATE_LOGGED:
                HandleConsoleInput(m_pConsole, m_pBuffer);
				break;
			}
		}

		// move the bytes back
		if( len == m_pBufferPos )
		{
			m_pBuffer[0] = '\0';
			m_pBufferPos = 0;
		}
		else
		{
			memcpy(m_pBuffer, &m_pBuffer[len], m_pBufferPos - len);
			m_pBufferPos -= len;
		}

		p = strchr(m_pBuffer, '\n');
	}
}

void ConsoleSocket::OnConnect()
{
	m_pConsole->Write("Welcome to Ascent's Remote Administration Console.\r\n");
	m_pConsole->Write("Please authenticate to continue.\r\n\r\n");
	m_pConsole->Write("login: ");
}

void ConsoleSocket::OnDisconnect()
{
	printf("ConsoleSocket::OnDisconnect()\n");
}

RemoteConsole::RemoteConsole(ConsoleSocket* pSocket)
{
	m_pSocket = pSocket;
}

void RemoteConsole::Write(const char * Format, ...)
{
	char obuf[65536];
	va_list ap;

    va_start(ap, Format);
	vsnprintf(obuf, 65536, Format, ap);
	va_end(ap);

	if( *obuf == '\0' )
		return;

	m_pSocket->Send((const uint8*)obuf, (uint32)strlen(obuf));
}

void RemoteConsole::WriteNA(const char * Format)
{
	if( *Format == '\0' )
		return;

	m_pSocket->Send((const uint8*)Format, (uint32)strlen(Format));
}


struct ConsoleCommand
{
	bool(*CommandPointer)(BaseConsole*, int, const char*[]);
	const char * Name;					// 10 chars
	const char * ArgumentFormat;		// 30 chars
	const char * Description;			// 40 chars
										// = 70 chars
};

void HandleConsoleInput(BaseConsole * pConsole, const char * szInput)
{
	static ConsoleCommand Commands[] = {
		{ &HandleInfoCommand, "info", "none", "Gives server runtime information." },
		{ &HandleGMsCommand, "gms", "none", "Shows online GMs." },
		{ &HandleAnnounceCommand, "announce", "<announce string>", "Shows the message in all client chat boxes." },
		{ &HandleWAnnounceCommand, "wannounce", "<wannounce string>", "Shows the message in all client title areas." },
		{ &HandleKickCommand, "kick", "<plrname> <reason>", "Kicks player x for reason y." },
		{ NULL, NULL, NULL, NULL },
	};

	uint32 i;
	char * p, *q;

	// let's tokenize into arguments.
	vector<const char*> tokens;

	q = (char*)szInput;
	p = strchr(q, ' ');
	while( p != NULL )
	{
		*p = 0;
		tokens.push_back(q);

		q = p+1;
		p = strchr(q, ' ');
	}

	if( q != NULL && *q != '\0' )
		tokens.push_back( q	);

	if( !stricmp(tokens[0], "help") || tokens[0][0] == '?' )
	{
		pConsole->Write("=========================================================================================\r\n");
		pConsole->Write("| %10s | %20s | %49s |\r\n", "Name", "Arguments", "Description");
		for(i = 0; Commands[i].Name != NULL; ++i)
		{
			pConsole->Write("| %10s | %20s | %49s |\r\n", Commands[i].Name, Commands[i].ArgumentFormat, Commands[i].Description);
		}
		pConsole->Write("=========================================================================================\r\n");		
	}
	else
	{
		for(i = 0; Commands[i].Name != NULL; ++i)
		{
			if( !stricmp( Commands[i].Name, tokens[0] ) )
			{
				if( !Commands[i].CommandPointer( pConsole, (int)tokens.size(), &tokens[0] ) )
				{
					pConsole->Write("Command '%s' failed. Try with syntax: '%s'.\r\n\r\n", Commands[i].Name, Commands[i].ArgumentFormat );
					return;
				}
				else
					return;
			}
		}

		pConsole->Write("No command named '%s' could be found. Please check your syntax and try again.\r\n\r\n", tokens[0]);
	}
}

void LocalConsole::Write(const char * Format, ...)
{
	va_list ap;
	va_start(ap, Format);
	vprintf(Format, ap);
}

void LocalConsole::WriteNA(const char * Format)
{
	printf(Format);
}
