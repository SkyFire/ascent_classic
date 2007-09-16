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

enum _errors
{
	CE_SUCCESS = 0x00,
	CE_IPBAN=0x01,									  //2bd -- unable to connect (some internal problem)
	CE_ACCOUNT_CLOSED=0x03,							 // "This account has been closed and is no longer in service -- Please check the registered email address of this account for further information.";
	CE_NO_ACCOUNT=0x04,								 //(5)The information you have entered is not valid.  Please check the spelling of the account name and password.  If you need help in retrieving a lost or stolen password and account
	CE_ACCOUNT_IN_USE=0x06,							 //This account is already logged in.  Please check the spelling and try again.
	CE_PREORDER_TIME_LIMIT=0x07,
	CE_SERVER_FULL=0x08,								//Could not log in at this time.  Please try again later.
	CE_WRONG_BUILD_NUMBER=0x09,						 //Unable to validate game version.  This may be caused by file corruption or the interference of another program.
	CE_UPDATE_CLIENT=0x0a,
	CE_ACCOUNT_FREEZED=0x0c
} ; 

AuthSocket::AuthSocket(SOCKET fd) : Socket(fd, 32768, 4096)
{
	N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
	g.SetDword(7);
	s.SetRand(256);
	m_authenticated = false;
	m_account = 0;
	last_recv = time(NULL);
	removedFromSet = false;
	_authSocketLock.Acquire();
	_authSockets.insert(this);
	_authSocketLock.Release();
}

AuthSocket::~AuthSocket()
{

}

void AuthSocket::OnDisconnect()
{
	if(m_authenticated && m_account)
		sInfoCore.DeleteSessionKey(m_account->AccountId);

	if(!removedFromSet)
	{
		_authSocketLock.Acquire();
		_authSockets.erase(this);
		_authSocketLock.Release();
	}
}

void AuthSocket::HandleChallenge()
{
	// No header
	if(GetReadBufferSize() < 4)
		return;	

	// Check the rest of the packet is complete.
	uint8 * ReceiveBuffer = this->GetReadBuffer(0);
#ifdef USING_BIG_ENDIAN
	uint16 full_size = swap16(*(uint16*)&ReceiveBuffer[2]);
#else
	uint16 full_size = *(uint16*)&ReceiveBuffer[2];
#endif

	sLog.outDetail("[AuthChallenge] got header, body is 0x%02X bytes", full_size);

	if(GetReadBufferSize() < uint32(full_size+4))
		return;

	// Copy the data into our cached challenge structure
	if(full_size > sizeof(sAuthLogonChallenge_C))
	{
		Disconnect();
		return;
	}

	sLog.outDebug("[AuthChallenge] got full packet.");

	memcpy(&m_challenge, ReceiveBuffer, full_size + 4);
	RemoveReadBufferBytes(full_size + 4, true);
//#ifdef USING_BIG_ENDIAN
//	uint16 build = swap16(m_challenge.build);
//	printf("Build: %u\n", build);
//#endif
 
	// Check client build.
#ifdef USING_BIG_ENDIAN
	if(swap16(m_challenge.build) > LogonServer::getSingleton().max_build ||
		swap16(m_challenge.build) < LogonServer::getSingleton().min_build)
#else
	if(m_challenge.build > LogonServer::getSingleton().max_build ||
		m_challenge.build < LogonServer::getSingleton().min_build)
#endif
	{
		SendChallengeError(CE_WRONG_BUILD_NUMBER);
		return;
	}

	// Check for a possible IP ban on this client.
	BAN_STATUS ipb = IPBanner::getSingleton().CalculateBanStatus(GetRemoteAddress());

	switch(ipb)
	{
		case BAN_STATUS_PERMANENT_BAN:
			SendChallengeError(CE_ACCOUNT_CLOSED);
			return;

		case BAN_STATUS_TIME_LEFT_ON_BAN:
			SendChallengeError(CE_ACCOUNT_FREEZED);
			return;
	}

	// Null-terminate the account string
	m_challenge.I[m_challenge.I_len] = 0;

	// Look up the account information
	string AccountName = (char*)&m_challenge.I;
	sLog.outDebug("[AuthChallenge] Account Name: \"%s\"", AccountName.c_str());

	m_account = AccountMgr::getSingleton().GetAccount(AccountName);
	if(m_account == 0)
	{
		sLog.outDebug("[AuthChallenge] Invalid account.");

		// Non-existant account
		SendChallengeError(CE_NO_ACCOUNT);
		return;
	}

	sLog.outDebug("[AuthChallenge] Account banned state = %u", m_account->Banned);

	// Check that the account isn't banned.
	if(m_account->Banned == 1)
	{
		SendChallengeError(CE_ACCOUNT_CLOSED);
		return;
	}
	else if(m_account->Banned > 0)
	{
		SendChallengeError(CE_ACCOUNT_FREEZED);
		return;
	}

	Sha1Hash sha;
	//uint32 tc = s.GetNumBytes();
	sha.UpdateData( s.AsByteArray(), 32 );
	sha.UpdateData( m_account->SrpHash, 20 );
	sha.Finalize();

	BigNumber x;
	x.SetBinary( sha.GetDigest(), sha.GetLength() );
	v = g.ModExp(x, N);
	b.SetRand(152);

	BigNumber gmod = g.ModExp(b, N);
	B = ((v * 3) + gmod) % N;
	ASSERT(gmod.GetNumBytes() <= 32);

	BigNumber unk;
	unk.SetRand(128);

	uint8 response[200];
	uint32 c = 0;
	response[c] = 0;										c += 1;
	response[c] = 0;										c += 1;
	response[c] = CE_SUCCESS;								c += 1;
	memcpy(&response[c], B.AsByteArray(), 32);				c += 32;
	response[c] = 1;										c += 1;
	response[c] = g.AsByteArray()[0];						c += 1;
	response[c] = 32;										c += 1;
	memcpy(&response[c], N.AsByteArray(), 32);				c += 32;
	memcpy(&response[c], s.AsByteArray(), s.GetNumBytes()); c += s.GetNumBytes();
	memcpy(&response[c], unk.AsByteArray(), 16);			c += 16;
	response[c] = 0;										c += 1;

	Send(response, c);
}

void AuthSocket::HandleProof()
{
	if(!m_account || GetReadBufferSize() < sizeof(sAuthLogonProof_C))
		return ;

	sLog.outDebug("[AuthLogonProof] Interleaving and checking proof...");

	sAuthLogonProof_C lp;
	Read(sizeof(sAuthLogonProof_C), (uint8*)&lp);

	BigNumber A;
	A.SetBinary(lp.A, 32);

	Sha1Hash sha;
	sha.UpdateBigNumbers(&A, &B, 0);
	sha.Finalize();

	BigNumber u;
	u.SetBinary(sha.GetDigest(), 20);
	
	BigNumber S = (A * (v.ModExp(u, N))).ModExp(b, N);
	uint8 t[32];
	uint8 t1[16];
	uint8 vK[40];
	memcpy(t, S.AsByteArray(), 32);
	for (int i = 0; i < 16; i++)
	{
		t1[i] = t[i*2];
	}
	sha.Initialize();
	sha.UpdateData(t1, 16);
	sha.Finalize();
	for (int i = 0; i < 20; i++)
	{
		vK[i*2] = sha.GetDigest()[i];
	}
	for (int i = 0; i < 16; i++)
	{
		t1[i] = t[i*2+1];
	}
	sha.Initialize();
	sha.UpdateData(t1, 16);
	sha.Finalize();
	for (int i = 0; i < 20; i++)
	{
		vK[i*2+1] = sha.GetDigest()[i];
	}
	m_sessionkey.SetBinary(vK, 40);

	uint8 hash[20];

	sha.Initialize();
	sha.UpdateBigNumbers(&N, NULL);
	sha.Finalize();
	memcpy(hash, sha.GetDigest(), 20);
	sha.Initialize();
	sha.UpdateBigNumbers(&g, NULL);
	sha.Finalize();
	for (int i = 0; i < 20; i++)
	{
		hash[i] ^= sha.GetDigest()[i];
	}
	BigNumber t3;
	t3.SetBinary(hash, 20);

	sha.Initialize();
	sha.UpdateData(m_account->Username);
	sha.Finalize();

	BigNumber t4;
	t4.SetBinary(sha.GetDigest(), 20);

	sha.Initialize();
	sha.UpdateBigNumbers(&t3, &t4, &s, &A, &B, &m_sessionkey, NULL);
	sha.Finalize();

	BigNumber M;
	M.SetBinary(sha.GetDigest(), 20);

	// Compare M1 values.
	if(memcmp(lp.M1, M.AsByteArray(), 20) != 0)
	{
		// Authentication failed.
		//SendProofError(4, 0);
		SendChallengeError(CE_NO_ACCOUNT);
		sLog.outDebug("[AuthLogonProof] M1 values don't match.");
		return;
	}

	// Store sessionkey
	BigNumber * bs = new BigNumber(m_sessionkey);
	sInfoCore.SetSessionKey(m_account->AccountId, bs);

	// let the client know
	sha.Initialize();
	sha.UpdateBigNumbers(&A, &M, &m_sessionkey, 0);
	sha.Finalize();

	SendProofError(0, sha.GetDigest());
	sLog.outDebug("[AuthLogonProof] Authentication Success.");

	// we're authenticated now :)
	m_authenticated = true;

	// Don't update when IP banned, but update anyway if it's an account ban
	sLogonSQL->Execute("UPDATE accounts SET lastlogin=NOW(), lastip='%s' WHERE acct=%u;", GetRemoteIP().c_str(), m_account->AccountId);
}

void AuthSocket::SendChallengeError(uint8 Error)
{
	uint8 buffer[3];
	buffer[0] = buffer[1] = 0;
	buffer[2] = Error;

	Send(buffer, 3);
}

void AuthSocket::SendProofError(uint8 Error, uint8 * M2)
{
	uint8 buffer[28];
	memset(buffer, 0, 28);

	buffer[0] = 1;
	buffer[1] = Error;
	if(M2 == 0)
	{
#ifdef USING_BIG_ENDIAN
		*(uint32*)&buffer[2] = swap32(3);
#else
		*(uint32*)&buffer[2] = 3;
#endif
		Send(buffer, 6);
		return;
	}
	
	memcpy(&buffer[2], M2, 20);
	Send(buffer, 28);
}

void AuthSocket::OnRead()
{
	if(GetReadBufferSize() < 1)
		return;

	uint8 Command = GetReadBuffer(0)[0];

	// Handle depending on command
	switch(Command)
	{
	case 0:	 // AUTH_CHALLENGE
		last_recv = time(NULL);
		HandleChallenge();
		break;

	case 1:	 // AUTH_PROOF
		last_recv = time(NULL);
		HandleProof();
		break;

	case 0x10:  // REALM_LIST
		last_recv = time(NULL);
		HandleRealmlist();
		break;
	}
}

void AuthSocket::HandleRealmlist()
{
	sInfoCore.SendRealms(this);
}

