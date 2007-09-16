/****************************************************************************
 *
 * IRC Plugin
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 */

#ifndef _IRC_SOCKET_H
#define _IRC_SOCKET_H

class IRCThread;
class IRCSocket : public Socket
{
public:
	IRCSocket(SOCKET fd);
	~IRCSocket();

	void OnRead();
	void SendLine(const char * format, ...);
	void SendMessage(const char * destination_user, const char * format, ...);
	void SendBoldMessage(const char * destination_user, const char * format, ...);

	Mutex m_recvQLock;
	string recvq;

	bool BufferReady();
	string GetLine();
	IRCThread * Thread;

	void PollRecvQ();

	char * Parameters[10];

	/** Handlers **/
	void Handle001();			// Sent on connect.
	void Handle433();			// Nickname already in use
	void HandleJoin();
	void HandlePrivMsg();
};

void info_cmd(const char* source, IRCSocket * s, int argc, char ** argv);
void kick_cmd(const char* source, IRCSocket * s, int argc, char ** argv);
void uptime_cmd(const char* source, IRCSocket * s, int argc, char ** argv);
void gm_cmd(const char* source, IRCSocket * s, int argc, char ** argv);
void announce_cmd(const char* source, IRCSocket * s, int argc, char ** argv);
void wannounce_cmd(const char* source, IRCSocket * s, int argc, char ** argv);

#endif
