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

#ifndef __ACCOUNTCACHE_H
#define __ACCOUNTCACHE_H

enum RealmColours
{
	REALMCOLOUR_GREEN			= 0,
	REALMCOLOUR_RED				= 1,
	REALMCOLOUR_OFFLINE			= 2,
	REALMCOLOUR_BLUE			= 3,
};

enum RealmIcons
{
	REALMICON_UNK_0				= 0,
	REALMICON_UNK_1				= 1,
	REALMICON_UNK_2				= 2,
};

#include "Common.h"
#include "../ascent-shared/Database/DatabaseEnv.h"

struct Account
{
	uint32 AccountId;
	char * Username;
	char * GMFlags;
	uint8 AccountFlags;
	uint32 Banned;
	uint8 SrpHash[20];
	uint8 * SessionKey;
	uint32 Muted;

	Account()
	{
		GMFlags = NULL;
		SessionKey = NULL;
	}

	~Account()
	{
		if(GMFlags)
			delete [] GMFlags;
		if(SessionKey)
			delete [] SessionKey;
	}

	void SetGMFlags(const char * flags)
	{
		if(GMFlags)
			delete [] GMFlags;

		size_t len = strlen(flags);
		if(len == 0 || (len == 1 && flags[0] == '0'))
		{
			// no flags
			GMFlags = NULL;
			return;
		}
		
		GMFlags = new char[len+1];
		memcpy(GMFlags, flags, len);
		GMFlags[len] = 0;
	}

	void SetSessionKey(const uint8 *key)
	{
		if(SessionKey == NULL)
			SessionKey = new uint8[40];
		memcpy(SessionKey, key, 40);
	}

	char Locale[4];
	bool forcedLocale;

};

typedef struct 
{
	union
	{
		struct ipfull { uint8 b1, b2, b3, b4; } full;
		uint32 asbytes;
	} ip;
	uint32 ban_expire_time;
} IPBan;

enum BAN_STATUS
{
	BAN_STATUS_NOT_BANNED = 0,
	BAN_STATUS_TIME_LEFT_ON_BAN = 1,
	BAN_STATUS_PERMANENT_BAN = 2,
};

class IPBanner : public Singleton< IPBanner >
{
public:
	void Load();
	void Reload();
	void Remove(set<IPBan*>::iterator ban);
	bool Add(const char * ip, uint32 dur);
	bool Remove(const char * ip);

	BAN_STATUS CalculateBanStatus(in_addr ip_address);

protected:
	Mutex setBusy;
	set<IPBan*> banList;
};

class AccountMgr : public Singleton < AccountMgr >
{
public:
	~AccountMgr()
	{

#ifdef WIN32
		for(HM_NAMESPACE::hash_map<string,Account*>::iterator itr = AccountDatabase.begin(); itr != AccountDatabase.end(); ++itr)
#else
		for(map<string,Account*>::iterator itr = AccountDatabase.begin(); itr != AccountDatabase.end(); ++itr)
#endif
		{
			delete itr->second;
		}
	}

	void AddAccount(Field* field);

	Account* GetAccount(string Name)
	{
		setBusy.Acquire();
		Account * pAccount = NULL;
		// this should already be uppercase!
#ifdef WIN32
		HM_NAMESPACE::hash_map<string, Account*>::iterator itr = AccountDatabase.find(Name);
#else
		map<string, Account*>::iterator itr = AccountDatabase.find(Name);
#endif

		if(itr == AccountDatabase.end())	pAccount = NULL;
		else								pAccount = itr->second;

		setBusy.Release();
		return pAccount;
	}

	void UpdateAccount(Account * acct, Field * field);
	void ReloadAccounts(bool silent);
	void ReloadAccountsCallback();

	ASCENT_INLINE size_t GetCount() { return AccountDatabase.size(); }

private:
	Account* __GetAccount(string Name)
	{
		// this should already be uppercase!
#ifdef WIN32
		HM_NAMESPACE::hash_map<string, Account*>::iterator itr = AccountDatabase.find(Name);
#else
		map<string, Account*>::iterator itr = AccountDatabase.find(Name);
#endif

		if(itr == AccountDatabase.end())	return NULL;
		else								return itr->second;
	}

#ifdef WIN32
	HM_NAMESPACE::hash_map<string, Account*> AccountDatabase;
#else
	std::map<string, Account*> AccountDatabase;
#endif

protected:
	Mutex setBusy;
};

class LogonCommServerSocket;
typedef struct
{
	uint32 Id;
	string Name;
	string Address;
	uint32 Colour;
	uint32 Icon;
	uint32 TimeZone;
	float Population;

	Mutex m_charMapLock;
	HM_NAMESPACE::hash_map<uint32, uint8> CharacterMap;
	LogonCommServerSocket *ServerSocket;

	uint8 GetCharacterCount(uint32 acc)
	{
		uint8 r;
		m_charMapLock.Acquire();
		HM_NAMESPACE::hash_map<uint32, uint8>::iterator itr = CharacterMap.find( acc );
		r = (itr == CharacterMap.end()) ? 0 : itr->second;
		m_charMapLock.Release();
		return r;
	}

}Realm;

class AuthSocket;
class LogonCommServerSocket;

class InformationCore : public Singleton<InformationCore>
{
	map<uint32, Realm*>		  m_realms;
	set<LogonCommServerSocket*> m_serverSockets;
	Mutex serverSocketLock;
	Mutex realmLock;

	uint32 realmhigh;
	bool usepings;

public:
	ASCENT_INLINE Mutex & getServerSocketLock() { return serverSocketLock; }
	ASCENT_INLINE Mutex & getRealmLock() { return realmLock; }

	InformationCore()
	{ 
		realmhigh = 0;
		usepings  = !Config.MainConfig.GetBoolDefault("LogonServer", "DisablePings", false);
	}

	// Packets
	void		  SendRealms(AuthSocket * Socket);
	
	// Realm management
	uint32 GenerateRealmID()
	{
		uint32 r;
		realmLock.Acquire();
		r = ++realmhigh;
		realmLock.Release();
		return r;
	}

	void		  AddRealm(Realm * rlm);
	Realm*        GetRealmByName(const char * realmName);
	Realm*        GetRealmById(uint32 id);
	void		  SetRealmOffline(uint32 realm_id, LogonCommServerSocket *ss);

	ASCENT_INLINE void   AddServerSocket(LogonCommServerSocket * sock) { serverSocketLock.Acquire(); m_serverSockets.insert( sock ); serverSocketLock.Release(); }
	ASCENT_INLINE void   RemoveServerSocket(LogonCommServerSocket * sock) { serverSocketLock.Acquire(); m_serverSockets.erase( sock ); serverSocketLock.Release(); }

	void		  TimeoutSockets();
	void CheckServers();
};

#define sIPBanner IPBanner::getSingleton()
#define sAccountMgr AccountMgr::getSingleton()
#define sInfoCore InformationCore::getSingleton()

#endif
