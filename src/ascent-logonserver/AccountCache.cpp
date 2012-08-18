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

#include "LogonStdAfx.h"
initialiseSingleton(AccountMgr);
initialiseSingleton(IPBanner);
initialiseSingleton(InformationCore);

void AccountMgr::ReloadAccounts(bool silent)
{
	setBusy.Acquire();
	if(!silent) sLog.outString("[AccountMgr] Reloading Accounts...");

	// Load *all* accounts.
	QueryResult * result = sLogonSQL->Query("SELECT acct, login, password, gm, flags, banned, forceLanguage, muted FROM accounts");
	Field * field;
	string AccountName;
	set<string> account_list;
	Account * acct;

	if(result)
	{
		do 
		{
			field = result->Fetch();
			AccountName = field[1].GetString();

			// transform to uppercase
			ASCENT_TOUPPER(AccountName);

			//Use private __GetAccount, for locks
			acct = __GetAccount(AccountName);
			if(acct == 0)
			{
				// New account.
				AddAccount(field);
			}
			else
			{
				// Update the account with possible changed details.
				UpdateAccount(acct, field);
			}

			// add to our "known" list
			account_list.insert(AccountName);

		} while(result->NextRow());

		delete result;
	}

	// check for any purged/deleted accounts
#ifdef WIN32
	HM_NAMESPACE::hash_map<string, Account*>::iterator itr = AccountDatabase.begin();
	HM_NAMESPACE::hash_map<string, Account*>::iterator it2;
#else
	std::map<string, Account*>::iterator itr = AccountDatabase.begin();
	std::map<string, Account*>::iterator it2;
#endif

	for(; itr != AccountDatabase.end();)
	{
		it2 = itr;
		++itr;

		if(account_list.find(it2->first) == account_list.end())
		{
			delete it2->second;
			AccountDatabase.erase(it2);
		}
	}

	if(!silent) sLog.outString("[AccountMgr] Found %u accounts.", AccountDatabase.size());
	setBusy.Release();

	IPBanner::getSingleton().Reload();
}

void AccountMgr::AddAccount(Field* field)
{
	Account * acct = new Account;
	Sha1Hash hash;
	string Username     = field[1].GetString();
	string Password	    = field[2].GetString();
	string GMFlags		= field[3].GetString();

	acct->AccountId				= field[0].GetUInt32();
	acct->AccountFlags			= field[4].GetUInt8();
	acct->Banned				= field[5].GetUInt32();
	acct->SetGMFlags(GMFlags.c_str());
	acct->Locale[0] = 'e';
	acct->Locale[1] = 'n';
	acct->Locale[2] = 'U';
	acct->Locale[3] = 'S';
	if(strcmp(field[6].GetString(), "enUS"))
	{
		// non-standard language forced
		memcpy(acct->Locale, field[6].GetString(), 4);
		acct->forcedLocale = true;
	}
	else
		acct->forcedLocale = false;

    acct->Muted = field[7].GetUInt32();

	// Convert username/password to uppercase. this is needed ;)
	ASCENT_TOUPPER(Username);
	ASCENT_TOUPPER(Password);
	acct->Username				= strdup(Username.c_str());
	
	// Prehash the I value.
	hash.UpdateData((Username + ":" + Password));
	hash.Finalize();
	memcpy(acct->SrpHash, hash.GetDigest(), 20);

	AccountDatabase[Username] = acct;
}

void AccountMgr::UpdateAccount(Account * acct, Field * field)
{
	uint32 id = field[0].GetUInt32();
	Sha1Hash hash;
	string Username     = field[1].GetString();
	string Password	    = field[2].GetString();
	string GMFlags		= field[3].GetString();

	if(id != acct->AccountId)
	{
		//printf("Account %u `%s` is a duplicate.\n", id, acct->Username.c_str());
		sLog.outColor(TYELLOW, " >> deleting duplicate account %u [%s]...", id, Username.c_str());
		sLog.outColor(TNORMAL, "\n");
		sLogonSQL->Execute("DELETE FROM accounts WHERE acct=%u", id);
		return;
	}

	acct->AccountId				= field[0].GetUInt32();
	acct->AccountFlags			= field[4].GetUInt8();
	acct->Banned				= field[5].GetUInt32();
	acct->SetGMFlags(GMFlags.c_str());
	if(strcmp(field[6].GetString(), "enUS"))
	{
		// non-standard language forced
		memcpy(acct->Locale, field[6].GetString(), 4);
		acct->forcedLocale = true;
	}
	else
		acct->forcedLocale = false;

	acct->Muted = field[7].GetUInt32();

	// Convert username/password to uppercase. this is needed ;)
	ASCENT_TOUPPER(Username);
	ASCENT_TOUPPER(Password);

	// Prehash the I value.
	hash.UpdateData((Username + ":" + Password));
	hash.Finalize();
	memcpy(acct->SrpHash, hash.GetDigest(), 20);
}

void AccountMgr::ReloadAccountsCallback()
{
	ReloadAccounts(true);
}
BAN_STATUS IPBanner::CalculateBanStatus(in_addr ip_address)
{
	setBusy.Acquire();

	uint8 b1 = ((uint8*)&ip_address)[0];
	uint8 b2 = ((uint8*)&ip_address)[1];
	uint8 b3 = ((uint8*)&ip_address)[2];
	uint8 b4 = ((uint8*)&ip_address)[3];

	// loop storage array
	set<IPBan*>::iterator itr = banList.begin();
	uint32 expiretime;
	bool banned = false;

	for(; itr != banList.end(); ++itr)
	{
		// compare first byte
		if((*itr)->ip.full.b1 == b1 || (*itr)->ip.full.b1 == 0xFF)
		{
			// compare second byte if there was a first match
			if((*itr)->ip.full.b2 == b2 || (*itr)->ip.full.b2 == 0xFF)
			{
				// compare third byte if there was a second match
				if((*itr)->ip.full.b3 == b3 || (*itr)->ip.full.b3 == 0xFF)
				{
					// compare last byte if there was a third match
					if((*itr)->ip.full.b4 == b4 || (*itr)->ip.full.b4 == 0xFF)
					{
						// full IP match
						banned = true;
						expiretime = (*itr)->ban_expire_time;
						break;
					}
				}
			}
		}
	}

	//Release here because we're not touching stored stuff anymore
	//-except in Remove, which acquires the lock itself (avoiding deadlocks :p)
	setBusy.Release();

	// calculate status
	if(!banned)
	{
		DEBUG_LOG("[IPBanner] IP has no ban entry");
		return BAN_STATUS_NOT_BANNED;
	}
	
	if (expiretime == 0)
	{
		DEBUG_LOG("[IPBanner] IP permanently banned");
		return BAN_STATUS_PERMANENT_BAN;
	}
	
	time_t rawtime;
	time( &rawtime );
	if(expiretime > (uint32)rawtime)
	{
		// temporary ban.
		time_t expire_time = expiretime;
		DEBUG_LOG("[IPBanner] IP temporary banned, Expires: %s", ctime( &expire_time ));
		return BAN_STATUS_TIME_LEFT_ON_BAN;
	}
	if(expiretime <= (uint32)rawtime)
	{
		// ban has expired. erase it from the banlist and database
		DEBUG_LOG("[IPBanner] Expired IP temporary ban has been removed");
		Remove(itr);
		return BAN_STATUS_NOT_BANNED;
	}

	// shouldnt get this far, but just in case...
	DEBUG_LOG("[IPBanner] Unknown IP ban state/duration, enforcing anyway");
	return BAN_STATUS_PERMANENT_BAN;
}

void IPBanner::Load()
{
	QueryResult * result = sLogonSQL->Query("SELECT ip, expire FROM ipbans");
	Field * fields;
	IPBan * ban;
	const char * ip_str;
	if(result)
	{
		do 
		{
			ban = new IPBan;
			fields = result->Fetch();

			ip_str = fields[0].GetString();
			unsigned int b1, b2, b3, b4;
			if(sscanf(ip_str, "%u.%u.%u.%u", &b1, &b2, &b3, &b4) != 4)
			{
				delete ban;
				continue;
			}

			ban->ip.full.b1 = b1;
			ban->ip.full.b2 = b2;
			ban->ip.full.b3 = b3;
			ban->ip.full.b4 = b4;
			ban->ban_expire_time = fields[1].GetUInt32();

			banList.insert( ban );
		}
		while(result->NextRow());

		delete result;
	}
}

bool IPBanner::Add(const char * ip, uint32 dur)
{
	unsigned int b1, b2, b3, b4;
	if(sscanf(ip, "%u.%u.%u.%u", &b1, &b2, &b3, &b4) != 4)
	{
		return false;
	}

	IPBan * ban = new IPBan;
	ban->ip.full.b1 = b1;
	ban->ip.full.b2 = b2;
	ban->ip.full.b3 = b3;
	ban->ip.full.b4 = b4;
	ban->ban_expire_time = dur;

	setBusy.Acquire();
	banList.insert(ban);
	setBusy.Release();
	return true;
}

bool IPBanner::Remove(const char * ip)
{
	unsigned int b1, b2, b3, b4;
	union {
		unsigned char b1, b2, b3, b4;
		uint32 asbytes;
	} b;
	if(sscanf(ip, "%u.%u.%u.%u", &b1, &b2, &b3, &b4) != 4)
	{
		return false;
	}

	b.b1 = b1;
	b.b2 = b2;
	b.b3 = b3;
	b.b4 = b4;

	setBusy.Acquire();
	set<IPBan*>::iterator itr, itr2;
	for(itr = banList.begin(); itr != banList.end(); )
	{
		itr2 = itr++;
		if( (*itr2)->ip.asbytes == b.asbytes )
			banList.erase(itr2);
	}
	setBusy.Release();
	return true;
}


void IPBanner::Reload()
{
	setBusy.Acquire();
	banList.clear();
	Load();
	setBusy.Release();
}

void IPBanner::Remove(set<IPBan*>::iterator ban)
{
	setBusy.Acquire();

	char strIp[16] = {0};
	snprintf(strIp, 16, "%u.%u.%u.%u", (*ban)->ip.full.b1, (*ban)->ip.full.b2, (*ban)->ip.full.b3,
		(*ban)->ip.full.b4 );

	sLogonSQL->Execute("DELETE FROM ipbans WHERE ip='%s'", strIp);
	banList.erase(ban);

	setBusy.Release();

	DEBUG_LOG("[IPBanner] Removed expired IPBan for ip '%s'", strIp);
}

void InformationCore::SetRealmOffline(uint32 realm_id, LogonCommServerSocket *ss)
{
	realmLock.Acquire();
	map<uint32,Realm*>::iterator itr = m_realms.find( realm_id );
	if( itr != m_realms.end() )
	{
		Realm * pr = itr->second;
		if( pr->ServerSocket == ss )
		{
			printf("Realm: %s is now offline.\n", pr->Name.c_str());
			pr->Colour = REALMCOLOUR_OFFLINE;
	
			// clear the mapping, when its re-registered it will send it again
			pr->m_charMapLock.Acquire();
			pr->CharacterMap.clear();
			pr->m_charMapLock.Release();
			pr->ServerSocket = NULL;
		}
	}
	realmLock.Release();
}

void InformationCore::AddRealm(Realm * rlm)
{
	realmLock.Acquire();
	m_realms.insert( make_pair( rlm->Id, rlm ) );
	realmLock.Release();
}

Realm * InformationCore::GetRealmByName(const char * realmName)
{
	realmLock.Acquire();
	map<uint32, Realm*>::iterator itr = m_realms.begin();
	for( ; itr != m_realms.end(); ++itr )
	{
		if( !stricmp(realmName, itr->second->Name.c_str()) )
		{
			realmLock.Release();
			return itr->second;
		}
	}

	realmLock.Release();
	return NULL;
}

Realm * InformationCore::GetRealmById(uint32 id)
{
	realmLock.Acquire();
	map<uint32, Realm*>::iterator itr = m_realms.find(id);
	if( itr != m_realms.end() )
	{
		realmLock.Release();
		return itr->second;
	}

	return NULL;
}

void InformationCore::SendRealms(AuthSocket * Socket)
{
	realmLock.Acquire();

	// packet header
	ByteBuffer data(m_realms.size() * 150 + 20);
	data << uint8(0x10);
	data << uint16(0);	  // Size Placeholder

	// dunno what this is..
	data << uint32(0);

	//sAuthLogonChallenge_C * client = Socket->GetChallenge();
	data << uint16(m_realms.size());
	
	// loop realms :/
	map<uint32, Realm*>::iterator itr = m_realms.begin();
	for(; itr != m_realms.end(); ++itr)
	{
		data << uint8(itr->second->Icon);
		data << uint8(0);				   // Locked Flag
		data << uint8(itr->second->Colour);		

		// This part is the same for all.
		data << itr->second->Name;
		data << itr->second->Address;
		data << itr->second->Population;

		/* Get our character count */
		if( itr->second->Colour != REALMCOLOUR_OFFLINE )
			data << itr->second->GetCharacterCount( Socket->GetAccountID() );
		else
			data << uint8(0);
	
		data << uint8(itr->second->TimeZone);
		data << uint8(6);
	}

	realmLock.Release();

	data << uint8(0x10);
	data << uint8(0);

	// Re-calculate size.
	*(uint16*)&data.contents()[1] = uint16(data.size() - 3);

	// Send to the socket.
	Socket->Send((const uint8*)data.contents(), uint32(data.size()));
}

void InformationCore::TimeoutSockets()
{
	if(!usepings)
		return;

	/* burlex: this is vulnerable to race conditions, adding a mutex to it. */
	serverSocketLock.Acquire();

	uint32 t = uint32(time(NULL));
	// check the ping time
	set<LogonCommServerSocket*>::iterator itr, it2;
	LogonCommServerSocket * s;
	for(itr = m_serverSockets.begin(); itr != m_serverSockets.end();)
	{
		s = *itr;
		it2 = itr;
		++itr;

		if(s->last_ping < t && ((t - s->last_ping) > 240))
		{
			// ping timeout
			printf("Closing socket due to ping timeout.\n");
			s->removed = true;
			set<uint32>::iterator itr = s->server_ids.begin();
			for(; itr != s->server_ids.end(); ++itr)
				SetRealmOffline(*itr, s);
			m_serverSockets.erase(it2);

			s->Disconnect();
			continue;
		}

		// send a ping back every 20 seconds.
		// this'll kill the socket if its inactive. meaning update times are minimal.
		if( t >= s->next_server_ping )
			s->SendPing();
	}

	serverSocketLock.Release();
}

void InformationCore::CheckServers()
{
	serverSocketLock.Acquire();

	set<LogonCommServerSocket*>::iterator itr, it2;
	LogonCommServerSocket * s;
	for(itr = m_serverSockets.begin(); itr != m_serverSockets.end();)
	{
		s = *itr;
		it2 = itr;
		++itr;

		if(!IsServerAllowed(s->GetRemoteAddress().s_addr))
		{
			printf("Disconnecting socket: %s due to it no longer being on an allowed IP.\n", s->GetRemoteIP().c_str());
			s->Disconnect();
		}
	}

	serverSocketLock.Release();
}
