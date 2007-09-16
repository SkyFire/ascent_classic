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

#include "LogonStdAfx.h"
initialiseSingleton(AccountMgr);
initialiseSingleton(IPBanner);
initialiseSingleton(InformationCore);

void AccountMgr::ReloadAccounts(bool silent)
{
	setBusy.Acquire();
	if(!silent) sLog.outString("[AccountMgr] Reloading Accounts...");

	// Load *all* accounts.
	QueryResult * result = sLogonSQL->Query("SELECT acct, login, password, gm, flags, banned FROM accounts");
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
			transform(AccountName.begin(), AccountName.end(), AccountName.begin(), towupper);

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
	HM_NAMESPACE::hash_map<string, Account>::iterator itr = AccountDatabase.begin();
	HM_NAMESPACE::hash_map<string, Account>::iterator it2;
#else
	std::map<string, Account>::iterator itr = AccountDatabase.begin();
	std::map<string, Account>::iterator it2;
#endif

	for(; itr != AccountDatabase.end();)
	{
		it2 = itr;
		++itr;

		if(account_list.find(it2->first) == account_list.end())
			AccountDatabase.erase(it2);
	}

	if(!silent) sLog.outString("[AccountMgr] Found %u accounts.", AccountDatabase.size());
	setBusy.Release();

	IPBanner::getSingleton().Reload();
}

void AccountMgr::AddAccount(Field* field)
{
	Account acct;
	Sha1Hash hash;
	acct.AccountId	  = field[0].GetUInt32();
	acct.Username	   = field[1].GetString();
	acct.Password	   = field[2].GetString();
	acct.GMFlags		= field[3].GetString();
	acct.AccountFlags   = field[4].GetUInt32();
	acct.Banned			= field[5].GetUInt32();

	// Convert username/password to uppercase. this is needed ;)
	transform(acct.Username.begin(), acct.Username.end(), acct.Username.begin(), towupper);
	transform(acct.Password.begin(), acct.Password.end(), acct.Password.begin(), towupper);

	// Prehash the I value.
	hash.UpdateData((acct.Username + ":" + acct.Password));
	hash.Finalize();
	memcpy(acct.SrpHash, hash.GetDigest(), 20);

	AccountDatabase[acct.Username] = acct;
}

void AccountMgr::UpdateAccount(Account * acct, Field * field)
{
	uint32 id = field[0].GetUInt32();
	if(id != acct->AccountId)
	{
		//printf("Account %u `%s` is a duplicate.\n", id, acct->Username.c_str());
		sLog.outColor(TYELLOW, " >> deleting duplicate account %u [%s]...", id, acct->Username.c_str());
		sLog.outColor(TNORMAL, "\n");
		sLogonSQL->Execute("DELETE FROM accounts WHERE acct=%u", id);
		return;
	}
	acct->AccountId	  = field[0].GetUInt32();
	acct->Username	   = field[1].GetString();
	acct->Password	   = field[2].GetString();
	acct->GMFlags		= field[3].GetString();
	acct->AccountFlags   = field[4].GetUInt32();
	acct->Banned		 = field[5].GetUInt32();

	// Convert username/password to uppercase. this is needed ;)
	transform(acct->Username.begin(), acct->Username.end(), acct->Username.begin(), towupper);
	transform(acct->Password.begin(), acct->Password.end(), acct->Password.begin(), towupper);

	Sha1Hash hash;
	hash.UpdateData((acct->Username + ":" + acct->Password));
	hash.Finalize();
	memcpy(acct->SrpHash, hash.GetDigest(), 20);
}

bool AccountMgr::LoadAccount(string Name)
{
	QueryResult * result = sLogonSQL->Query("SELECT acct, login, password, gm, flags, banned FROM account_database WHERE login='%s'", Name.c_str());
	if(result == 0)
		return false;

	AddAccount(result->Fetch());
	delete result;

	return true;
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
		sLog.outDebug("[IPBanner] IP has no ban entry");
		return BAN_STATUS_NOT_BANNED;
	}
	
	if (expiretime == 0)
	{
		sLog.outDebug("[IPBanner] IP permanently banned");
		return BAN_STATUS_PERMANENT_BAN;
	}
	
	time_t rawtime;
	time( &rawtime );
	if(expiretime > (uint32)rawtime)
	{
		// temporary ban.
		time_t expire_time = expiretime;
		sLog.outDebug("[IPBanner] IP temporary banned, Expires: %s", ctime( &expire_time ));
		return BAN_STATUS_TIME_LEFT_ON_BAN;
	}
	if(expiretime <= (uint32)rawtime)
	{
		// ban has expired. erase it from the banlist and database
		sLog.outDebug("[IPBanner] Expired IP temporary ban has been removed");
		Remove(itr);
		return BAN_STATUS_NOT_BANNED;
	}

	// shouldnt get this far, but just in case...
	sLog.outDebug("[IPBanner] Unknown IP ban state/duration, enforcing anyway");
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

	sLog.outDebug("[IPBanner] Removed expired IPBan for ip '%s'", strIp);
}

Realm * InformationCore::AddRealm(uint32 realm_id, Realm * rlm)
{
	realmLock.Acquire();
	m_realms.insert( make_pair( realm_id, rlm ) );
	map<uint32, Realm*>::iterator itr = m_realms.find(realm_id);
	realmLock.Release();
	return rlm;
}

Realm * InformationCore::GetRealm(uint32 realm_id)
{
	Realm * ret = 0;

	realmLock.Acquire();
	map<uint32, Realm*>::iterator itr = m_realms.find(realm_id);
	if(itr != m_realms.end())
	{
		ret = itr->second;
	}
	realmLock.Release();
	return ret;
}

void InformationCore::RemoveRealm(uint32 realm_id)
{
	realmLock.Acquire();
	map<uint32, Realm*>::iterator itr = m_realms.find(realm_id);
	if(itr != m_realms.end())
	{
		sLog.outString("Removing realm `%s` (%u) due to socket close.", itr->second->Name.c_str(), realm_id);
		delete itr->second;
		m_realms.erase(itr);
	}
	realmLock.Release();
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
	HM_NAMESPACE::hash_map<uint32, uint8>::iterator it;
	for(; itr != m_realms.end(); ++itr)
	{
		data << uint8(itr->second->Colour);
		data << uint8(0);				   // Locked Flag
		data << uint8(itr->second->Icon);

		// This part is the same for all.
		data << itr->second->Name;
		data << itr->second->Address;
		data << itr->second->Population;

		/* Get our character count */
		it = itr->second->CharacterMap.find(Socket->GetAccountID());
		data << uint8( (it == itr->second->CharacterMap.end()) ? 0 : it->second );
		data << uint8(itr->second->TimeZone);   // time zone
		data << uint8(0);
	}
	realmLock.Release();

	data << uint8(0x15);
	data << uint8(0);

	// Re-calculate size.
#ifdef USING_BIG_ENDIAN
	*(uint16*)&data.contents()[1] = swap16(uint16(data.size() - 3));
#else
	*(uint16*)&data.contents()[1] = data.size() - 3;
#endif

	// Send to the socket.
	Socket->Send((const uint8*)data.contents(), data.size());
}

BigNumber * InformationCore::GetSessionKey(uint32 account_id)
{
	m_sessionKeyLock.Acquire();
	BigNumber * bn = 0;
	map<uint32, BigNumber*>::iterator itr = m_sessionkeys.find(account_id);
	if(itr != m_sessionkeys.end())
	{
		bn = itr->second;
	}

	m_sessionKeyLock.Release();
	return bn;
}

void InformationCore::DeleteSessionKey(uint32 account_id)
{
	m_sessionKeyLock.Acquire();
	map<uint32, BigNumber*>::iterator itr = m_sessionkeys.find(account_id);
	if(itr != m_sessionkeys.end())
	{
		delete itr->second;
		m_sessionkeys.erase(itr);
	}
	m_sessionKeyLock.Release();
}

void InformationCore::SetSessionKey(uint32 account_id, BigNumber * key)
{
	m_sessionKeyLock.Acquire();
	m_sessionkeys[account_id] = key;
	m_sessionKeyLock.Release();
}

void InformationCore::TimeoutSockets()
{
	if(!usepings)
		return;

	/* burlex: this is vulnerable to race conditions, adding a mutex to it. */
	serverSocketLock.Acquire();

	uint32 t = time(NULL);
	// check the ping time
	set<LogonCommServerSocket*>::iterator itr, it2;
	LogonCommServerSocket * s;
	for(itr = m_serverSockets.begin(); itr != m_serverSockets.end();)
	{
		s = *itr;
		it2 = itr;
		++itr;

		if(s->last_ping < t && ((t - s->last_ping) > 60))
		{
			// ping timeout
			printf("Closing socket due to ping timeout.\n");
			s->removed = true;
			set<uint32>::iterator itr = s->server_ids.begin();
			for(; itr != s->server_ids.end(); ++itr)
				RemoveRealm(*itr);
			m_serverSockets.erase(it2);

			s->Disconnect();
		}
	}

	serverSocketLock.Release();
}
