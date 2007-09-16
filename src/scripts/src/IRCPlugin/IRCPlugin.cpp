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

#include "IRCPlugin.h"
void OnMessageChat(Player * pPlayer, uint32 Type, uint32 Lang, const char * Message, const char * Misc);

extern "C" SCRIPT_DECL uint32 _exp_get_version()
{
	return MAKE_SCRIPT_VERSION(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);
}

extern "C" SCRIPT_DECL void _exp_script_register(ScriptMgr* mgr)
{
	bool Enabled = Config.MainConfig.GetBoolDefault("IRCPlugin", "Enabled", false);
	if(!Enabled)
		return;

	char UserName[500];
	char AltUserName[500];
    char Channel[500];
	char Server[500];

	Config.MainConfig.GetString("IRCPlugin", UserName, "NickName", "WoWBot", 500 );
	Config.MainConfig.GetString("IRCPlugin", AltUserName, "AltNickName", "WoWBot_", 500 );
	Config.MainConfig.GetString("IRCPlugin", Channel, "Channel", "#ascent", 500 );
	Config.MainConfig.GetString("IRCPlugin", Server, "Server", "192.168.1.1", 500 );

	IRCThread * t = new IRCThread;
	t->Server = string(Server);
	t->AltNickName = string(AltUserName);
	t->NickName = string(UserName);
	t->Channel = string(Channel);
	t->Port = Config.MainConfig.GetIntDefault("IRCPlugin", "Port", 6667);

	launch_thread(t);
	mgr->register_hook(SERVER_HOOK_EVENT_ON_CHAT, &OnMessageChat);
}


/***** IRC THREAD ******/
void IRCThread::run()
{
	// Sleep for a bit to allow the server to start up fully.
	//Sleep(5000);
	State = STATE_CONNECTING;

	for(;;)
	{
		switch(State)
		{
		case STATE_CONNECTING:
			_HandleConnecting();
			break;

		default:
			_HandleConnected();
			break;
		}

		Sleep(20);
	}
}


void IRCThread::_HandleConnecting()
{
	socket = new IRCSocket(0);
	if(!socket->Connect( Server.c_str(), Port ) )
	{
		delete socket;
		return;
	}

	/* connection established */
	socket->Thread = this;
	State = STATE_CONNECTED;
}

void IRCThread::_HandleConnected()
{
	switch(State)
	{
	case STATE_CONNECTED:
		{
			// We just connected. Send our user and nick.
			socket->SendLine("USER %s \"localhost\" \"127.0.0.1\" :%s", NickName.c_str(), NickName.c_str());
			socket->SendLine("NICK %s", NickName.c_str());
			State = STATE_REGISTER;
		}break;

	case STATE_REGISTER:
		{
			// Wait for registration.
		}break;

	case STATE_JOINCHANNELS:
		{
			// Join channels
			socket->SendLine("JOIN %s", Channel.c_str());
			State = STATE_FULLCONNECTED;
		}break;
	}

	socket->PollRecvQ();
}

/* This is the hook for chat. */
void OnMessageChat(Player * pPlayer, uint32 Type, uint32 Lang, const char * Message, const char * Misc)
{

}
