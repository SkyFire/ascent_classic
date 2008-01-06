#ifndef _BASECONSOLE_H
#define _BASECONSOLE_H

class ConsoleSocket;

class BaseConsole
{
public:
	virtual void Write(const char * Format, ...) = 0;
	virtual void WriteNA(const char * Format) = 0;
};

class RemoteConsole : public BaseConsole
{
	ConsoleSocket * m_pSocket;
public:
	RemoteConsole(ConsoleSocket* pSocket);
	void Write(const char * Format, ...);
	void WriteNA(const char * Format);
};

class LocalConsole : public BaseConsole
{
public:
	void Write(const char * Format, ...);
	void WriteNA(const char * Format);
};

void HandleConsoleInput(BaseConsole * pConsole, const char * szInput);

#endif
