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

#include "StdAfx.h"
#include "../shared/AuthCodes.h"
#include "../shared/svn_revision.h"

bool VerifyName(const char * name, size_t nlen)
{
	static const char * bannedCharacters = "\t\v\b\f\a\n\r\\\"\'\? <>[](){}_=+-|/!@#$%^&*~`.,0123456789\0";
	const char * p;
	for(size_t i = 0; i < nlen; ++i)
	{
		p = bannedCharacters;
		while(*p != 0 && name[i] != *p && name[i] != 0)
			++p;

		if(*p != 0)
			return false;
	}
	return true;
}

void CapitalizeString(string& arg)
{
	if(arg.length() == 0) return;
	arg[0] = toupper(arg[0]);
	for(uint32 x = 1; x < arg.size(); ++x)
		arg[x] = tolower(arg[x]);
}

void WorldSession::CharacterEnumProc(QueryResult * result)
{
	struct player_item
	{
		uint32 displayid;
		uint8 invtype;
	};

	player_item items[20];
	uint32 slot;
	uint32 i;
	ItemPrototype * proto;
	QueryResult * res;
	CreatureInfo *info = NULL;
	uint8 num = 0;

	// should be more than enough.. 200 bytes per char..
	WorldPacket data(SMSG_CHAR_ENUM, (result ? result->GetRowCount() * 200 : 1));	

	// parse m_characters and build a mighty packet of
	// characters to send to the client.
	data << num;
	if( result )
	{
		uint64 guid;
		uint8 Class;
		uint32 bytes2;
		uint32 flags;
		uint32 banned;
		Field *fields;
		do
		{
			fields = result->Fetch();
			guid = fields[0].GetUInt32();
			bytes2 = fields[6].GetUInt32();
			Class = fields[3].GetUInt8();			
			flags = fields[18].GetUInt32();

			/* build character enum, w0000t :p */
			data << fields[0].GetUInt64();		// guid
			data << fields[8].GetString();		// name
			data << fields[2].GetUInt8();		// race
			data << fields[3].GetUInt8();		// class
			data << fields[4].GetUInt8();		// gender
			data << fields[5].GetUInt32();		// PLAYER_BYTES
			data << uint8(bytes2 & 0xFF);		// facial hair
			data << fields[1].GetUInt8();		// Level
			data << fields[13].GetUInt32();		// zoneid
			data << fields[12].GetUInt32();		// Mapid
			data << fields[11].GetFloat();		// X
			data << fields[10].GetFloat();		// Y
			data << fields[9].GetFloat();		// Z
			data << fields[7].GetUInt32();		// GuildID

			banned = fields[14].GetUInt32();
			if(banned && (banned<10 || banned > UNIXTIME))
				data << uint32(0x01A04040);
			else
			{
				if(fields[16].GetUInt32() != 0)
					data << (uint32)8704; // Dead (displaying as Ghost)
				else
					data << uint32(1);		// alive
			}
			
			data << fields[15].GetUInt8();		// Rest State

			if(Class==9 || Class==3)
			{
				res = CharacterDatabase.Query("SELECT entryid FROM playerpets WHERE ownerguid="I64FMTD" AND active=1", guid);

				if(res)
				{
					info = CreatureNameStorage.LookupEntry(res->Fetch()[0].GetUInt32());
					delete res;
				}
				else
					info=NULL;
			}
			else
				info=NULL;

			if(info)  //PET INFO uint32 displayid,	uint32 level,		 uint32 familyid
				data << uint32(info->Male_DisplayID) << uint32(10) << uint32(info->Family);
			else
				data << uint32(0) << uint32(0) << uint32(0);

			res = CharacterDatabase.Query("SELECT slot, entry FROM playeritems WHERE ownerguid=%u and containerslot=-1 and slot < 19 and slot >= 0", GUID_LOPART(guid));

			memset(items, 0, sizeof(player_item) * 20);
			if(res)
			{
				do 
				{
					proto = ItemPrototypeStorage.LookupEntry(res->Fetch()[1].GetUInt32());
					if(proto)
					{
						// slot0 = head, slot14 = cloak
						slot = res->Fetch()[0].GetUInt32();
						if(!(slot == 0 && (flags & (uint32)PLAYER_FLAG_NOHELM) != 0) && !(slot == 14 && (flags & (uint32)PLAYER_FLAG_NOCLOAK) != 0)) {
							items[slot].displayid = proto->DisplayInfoID;
							items[slot].invtype = proto->InventoryType;
						}
					}
				} while(res->NextRow());
				delete res;
			}

			for( i = 0; i < 20; ++i )
				data << items[i].displayid << items[i].invtype;

			num++;
		}
		while( result->NextRow() );
	}

	data.put<uint8>(0, num);

	//Log.Debug("Character Enum", "Built in %u ms.", getMSTime() - start_time);
	SendPacket( &data );
}

void WorldSession::HandleCharEnumOpcode( WorldPacket & recv_data )
{	
	AsyncQuery * q = new AsyncQuery( new SQLClassCallbackP1<World, uint32>(World::getSingletonPtr(), &World::CharacterEnumProc, GetAccountId()) );
	q->AddQuery("SELECT guid, level, race, class, gender, bytes, bytes2, guildid, name, positionX, positionY, positionZ, mapId, zoneId, banned, restState, deathstate, forced_rename_pending, player_flags FROM characters WHERE acct=%u ORDER BY guid", GetAccountId());
	CharacterDatabase.QueueAsyncQuery(q);
}

void WorldSession::LoadAccountDataProc(QueryResult * result)
{
	size_t len;
	const char * data;
	char * d;

	if(!result)
		return;

	for(uint32 i = 0; i < 7; ++i)
	{
		data = result->Fetch()[1+i].GetString();
		len = data ? strlen(data) : 0;
		if(len > 1)
		{
			d = new char[len+1];
			memcpy(d, data, len+1);
			SetAccountData(i, d, true, (uint32)len);
		}
	}
}

void WorldSession::HandleCharCreateOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 10);
	std::string name;
	uint8 race, class_;

	recv_data >> name >> race >> class_;
	recv_data.rpos(0);

	if(!VerifyName(name.c_str(), name.length()))
	{
		OutPacket(SMSG_CHAR_CREATE, 1, "\x31");
		return;
	}

	if(objmgr.GetPlayerInfoByName(name) != 0)
	{
		OutPacket(SMSG_CHAR_CREATE, 1, "\x31");
		return;
	}

	if(!sHookInterface.OnNewCharacter(race, class_, this, name.c_str()))
	{
		OutPacket(SMSG_CHAR_CREATE, 1, "\x31");
		return;
	}

	name = WorldDatabase.EscapeString(name);
	QueryResult * result = WorldDatabase.Query("SELECT COUNT(*) FROM banned_names WHERE name = '%s'", name.c_str());
	if(result)
	{
		if(result->Fetch()[0].GetUInt32() > 0)
		{
			// That name is banned!
			OutPacket(SMSG_CHAR_CREATE, 1, "\x50"); // You cannot use that name
			delete result;
			return;
		}
		delete result;
	}
	// loading characters
	
	//checking number of chars is useless since client will not allow to create more than 10 chars
	//as the 'create' button will not appear (unless we want to decrease maximum number of characters)

	Player * pNewChar = objmgr.CreatePlayer();
	pNewChar->SetSession(this);
	if(!pNewChar->Create( recv_data ))
	{
		// failed.
		pNewChar->ok_to_remove = true;
		delete pNewChar;
		return;
	}

	QueryResult * resulta = CharacterDatabase.Query("SELECT race FROM characters WHERE acct = %u AND race in (1,3,4,7,11)", _accountId);
	QueryResult * resulth = CharacterDatabase.Query("SELECT race FROM characters WHERE acct = %u AND race in (2,5,8,9,10)", _accountId);
	SetSide(0);
	if (resulta)
		SetSide(1);
	if (resulth)
		SetSide(2);



	//Same Faction limitation only applies to PVP and RPPVP realms :)
	uint32 realmType = sLogonCommHandler.GetRealmType();
	if(!HasGMPermissions() && (realmType==REALMTYPE_PVP||realmType==REALMTYPE_RPPVP))
	{
		if(
			((pNewChar->GetTeam()== 0) && (_side == 2))||
			((pNewChar->GetTeam()== 1) && (_side == 1))
			)
		{
			pNewChar->ok_to_remove = true;
			delete pNewChar;
			WorldPacket data(1);
			data.SetOpcode(SMSG_CHAR_CREATE);
			data << (uint8)ALL_CHARS_ON_PVP_REALM_MUST_AT_SAME_SIDE;
			SendPacket( &data );
			return;
		}
	}
	pNewChar->UnSetBanned();
	
	pNewChar->addSpell(22027);	  // Remove Insignia


	if(pNewChar->getClass() == WARLOCK)
	{
		pNewChar->AddSummonSpell(416, 3110);		// imp fireball
		pNewChar->AddSummonSpell(417, 19505);
		pNewChar->AddSummonSpell(1860, 3716);
		pNewChar->AddSummonSpell(1863, 7814);
	}

	pNewChar->SaveToDB(true);	

	PlayerInfo *pn=new PlayerInfo ;
	pn->guid = pNewChar->GetGUIDLow();
	pn->name = strdup(pNewChar->GetName());
	pn->cl = pNewChar->getClass();
	pn->race = pNewChar->getRace();
	pn->gender = pNewChar->getGender();
	pn->publicNote=NULL;
	pn->officerNote=NULL;
	pn->m_Group=0;
	pn->subGroup=0;
	pn->m_loggedInPlayer=NULL;
	pn->team = pNewChar->GetTeam ();
	objmgr.AddPlayerInfo(pn);

	pNewChar->ok_to_remove = true;
	delete  pNewChar;

	// CHAR_CREATE_SUCCESS
	OutPacket(SMSG_CHAR_CREATE, 1, "\x2E");

	sLogonCommHandler.UpdateAccountCount(GetAccountId(), 1);
}

/* FOR 1.10.1
SMSG_CHAR_CREATE Error Codes:
0x00 Success
0x01 Failure
0x02 Canceled
0x03 Disconnect from server
0x04 Failed to connect
0x05 Connected
0x06 Wrong client version
0x07 Connecting to server
0x08 Negotiating security
0x09 Negotiating security complete
0x0A Negotiating security failed
0x0B Authenticating
0x0C Authentication successful
0x0D Authentication failed
0x0E Login unavailable - Please contact Tech Support
0x0F Server is not valid
0x10 System unavailable 
0x11 System error
0x12 Billing system error
0x13 Account billing has expired
0x14 Wrong client version
0x15 Unknown account
0x16 Incorrect password
0x17 Session expired
0x18 Server Shutting Down
0x19 Already logged in
0x1A Invalid login server
0x1B Position in Queue: 0
0x1C This account has been banned
0x1D This character is still logged on
0x1E Your WoW subscription has expired
0x1F This session has timed out
0x20 This account has been temporarily suspended
0x21 Access to this account blocked by parental controls 
0x22 Retrieving realmlist
0x23 Realmlist retrieved
0x24 Unable to connect to realmlist server
0x25 Invalid realmlist
0x26 The game server is currently down
0x27 Creating account
0x28 Account created
0x29 Account creation failed
0x2A Retrieving character list
0x2B Character list retrieved
0x2C Error retrieving character list
0x2D Creating character
0x2E Character created
0x2F Error creating character
0x30 Character creation failed
0x31 That name is unavailable
0x32 Creation of that race/class is disabled
0x33 You cannot have both horde and alliance character at pvp realm
0x33 All characters on a PVP realm must be on the same team
0x34 You already have maximum number of characters
0x35 You already have maximum number of characters
0x36 The server is currently queued
0x37 Only players who have characters on this realm..
0x38 Creation of that race requires an account that has been upgraded to the approciate expansion
0x39 Deleting character
0x3A Character deleted
0x3B Char deletion failed
0x3c Entering world of warcraft
0x3D Login successful
0x3E World server is down
0x3F A character with that name already exists
0x40 No instance server available
0x41 Login failed
0x42 Login for that race/class is disabled
0x43 Character not found
0x44 Enter a name for your character
0x45 Names must be atleast 2 characters long
0x46 Names must be no more then 12 characters
0x47 Names can only contain letters
0x48 Names must contain only one language
0x49 That name contains profanity
0x4A That name is unavailable
0x4B You cannot use an apostrophe
0x4C You can only have one apostrophe
0x4D You cannot use the same letter three times consecutively
0x4E You cannit use space as the first or last character of your name
0x4F <Blank>
0x50 Invalid character name
0x51 <Blank>
All further codes give the number in dec.
*/

void WorldSession::HandleCharDeleteOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 8);
	uint8 fail = 0x3A;

	uint64 guid;
	recv_data >> guid;

	if(objmgr.GetPlayer((uint32)guid) != NULL)
	{
		// "Char deletion failed"
		fail = 0x3B;
	} else {

		QueryResult * result = CharacterDatabase.Query("SELECT guildid, name FROM characters WHERE guid = %u", (uint32)guid);
		PlayerInfo * inf = objmgr.GetPlayerInfo((uint32)guid);
		if(!result || !inf)
			return;

		uint32 guildid = result->Fetch()[0].GetUInt32();
		string name = result->Fetch()[1].GetString();
		delete result;

		for(int i = 0; i < NUM_CHARTER_TYPES; ++i)
		{
			Charter * c = objmgr.GetCharterByGuid(guid, (CharterTypes)i);
			if(c != NULL)
				c->RemoveSignature((uint32)guid);
		}


		for(int i = 0; i < NUM_ARENA_TEAM_TYPES; ++i)
		{
			ArenaTeam * t = objmgr.GetArenaTeamByGuid((uint32)guid, i);
			if(t != NULL)
				t->RemoveMember(inf);
		}
		
		if(guildid)
		{
			Guild *pGuild = objmgr.GetGuild(guildid);
			if(pGuild)
			{
				if(pGuild->GetGuildLeaderGuid() == guid)
				{
					pGuild->DeleteGuildMembers();
					pGuild->RemoveFromDb();
				}
				else
					pGuild->DeleteGuildMember(guid);
			}
		}

		sPlrLog.write("Account: %s | IP: %s >> Deleted player %s", GetAccountName().c_str(), GetSocket()->GetRemoteIP().c_str(), name.c_str());

		sSocialMgr.RemovePlayer((uint32)guid);

		CharacterDatabase.WaitExecute("DELETE FROM characters WHERE guid = %u", (uint32)guid);

		Corpse * c=objmgr.GetCorpseByOwner((uint32)guid);
		if(c)
			CharacterDatabase.Execute("DELETE FROM corpses WHERE guid = %u", c->GetGUIDLow());

		CharacterDatabase.Execute("DELETE FROM playeritems WHERE ownerguid=%u",(uint32)guid);
		CharacterDatabase.Execute("DELETE FROM gm_tickets WHERE guid = %u", (uint32)guid);
		CharacterDatabase.Execute("DELETE FROM playerpets WHERE ownerguid = %u", (uint32)guid);
		CharacterDatabase.Execute("DELETE FROM playerpetspells WHERE ownerguid = %u", (uint32)guid);
		CharacterDatabase.Execute("DELETE FROM playersummonspells WHERE ownerguid = %u", (uint32)guid);
		CharacterDatabase.Execute("DELETE FROM tutorials WHERE playerId = %u", (uint32)guid);
		CharacterDatabase.Execute("DELETE FROM questlog WHERE player_guid = %u", (uint32)guid);
		CharacterDatabase.Execute("DELETE FROM playercooldownitems WHERE OwnerGuid = %u", (uint32)guid);
		CharacterDatabase.Execute("DELETE FROM mailbox WHERE player_guid = %u", (uint32)guid);

		/* remove player info */
		objmgr.DeletePlayerInfo((uint32)guid);
	}

	OutPacket(SMSG_CHAR_DELETE, 1, &fail);
}

void WorldSession::HandleCharRenameOpcode(WorldPacket & recv_data)
{
	WorldPacket data(SMSG_CHAR_RENAME, recv_data.size() + 1);

	uint64 guid;
	string name;
	recv_data >> guid >> name;

	PlayerInfo * pi = objmgr.GetPlayerInfo((uint32)guid);
	if(pi == 0) return;

	QueryResult * result = CharacterDatabase.Query("SELECT forced_rename_pending FROM characters WHERE guid = %u AND acct = %u", 
		(uint32)guid, _accountId);
	if(result == 0)
	{
		delete result;
		return;
	}
	delete result;

	// Check name for rule violation.
	const char * szName=name.c_str();
	for(uint32 x=0;x<strlen(szName);x++)
	{
		if((int)szName[x]<65||((int)szName[x]>90&&(int)szName[x]<97)||(int)szName[x]>122)
		{
			data << uint8(0x31);
			data << guid << name;
			SendPacket(&data);
			return;
		}
	}

	name = WorldDatabase.EscapeString(name);
	QueryResult * result2 = WorldDatabase.Query("SELECT COUNT(*) FROM banned_names WHERE name = '%s'", name.c_str());
	if(result2)
	{
		if(result2->Fetch()[0].GetUInt32() > 0)
		{
			// That name is banned!
			data << uint8(0x31);
			data << guid << name;
			SendPacket(&data);
		}
		delete result2;
	}

	// Check if name is in use.
	if(objmgr.GetPlayerInfoByName(name) != 0)
	{
		data << uint8(0x31);
		data << guid << name;
		SendPacket(&data);
		return;
	}

	// correct capitalization
	CapitalizeString(name);

	// If we're here, the name is okay.
	free(pi->name);
	pi->name = strdup(name.c_str());
	CharacterDatabase.WaitExecute("UPDATE characters SET name = '%s' WHERE guid = %u", name.c_str(), (uint32)guid);
	CharacterDatabase.WaitExecute("UPDATE characters SET forced_rename_pending = 0 WHERE guid = %u", (uint32)guid);
	
	data << uint8(0) << guid << name;
	SendPacket(&data);
}


void WorldSession::HandlePlayerLoginOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 8);
	uint64 playerGuid = 0;

	sLog.outDebug( "WORLD: Recvd Player Logon Message" );

	recv_data >> playerGuid; // this is the GUID selected by the player
	if(objmgr.GetPlayer((uint32)playerGuid) != NULL || m_loggingInPlayer || _player)
	{
		// A character with that name already exists 0x3E
		uint8 respons = CHAR_LOGIN_DUPLICATE_CHARACTER;
		OutPacket(SMSG_CHARACTER_LOGIN_FAILED, 1, &respons);
		return;
	}

	Player* plr = new Player(HIGHGUID_PLAYER,(uint32)playerGuid);
	ASSERT(plr);
	plr->SetSession(this);
	m_bIsWLevelSet = false;
	
	Log.Debug("WorldSession", "Async loading player %u", (uint32)playerGuid);
	m_loggingInPlayer = plr;
	plr->LoadFromDB((uint32)playerGuid);
}

void WorldSession::FullLogin(Player * plr)
{
	Log.Debug("WorldSession", "Fully loading player %u", plr->GetGUIDLow());
	SetPlayer(plr); 
	m_MoverWoWGuid.Init(plr->GetGUID());

	// copy to movement array
	movement_packet[0] = m_MoverWoWGuid.GetNewGuidMask();
	memcpy(&movement_packet[1], m_MoverWoWGuid.GetNewGuid(), m_MoverWoWGuid.GetNewGuidLen());

#ifndef USING_BIG_ENDIAN
	StackWorldPacket<20> datab(CMSG_DUNGEON_DIFFICULTY);
#else
	WorldPacket datab(CMSG_DUNGEON_DIFFICULTY, 20);
#endif
	datab << plr->iInstanceType;
	datab << uint32(0x01);
	datab << uint32(0x00);
	SendPacket(&datab);

	/* world preload */
	plr->SendLoginVerifyWorld();

	// send voicechat state - active/inactive
	/*
	{SERVER} Packet: (0x03C7) UNKNOWN PacketSize = 2
	|------------------------------------------------|----------------|
	|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
	|------------------------------------------------|----------------|
	|02 01							               |..              |
	-------------------------------------------------------------------
	*/

	datab.Initialize(SMSG_VOICE_SYSTEM_STATUS);
	datab << uint8(2) << uint8(sVoiceChatHandler.CanUseVoiceChat() ? 1 : 0);
	SendPacket(&datab);

	plr->UpdateAttackSpeed();
	if(plr->getLevel()>70)
		plr->SetUInt32Value(UNIT_FIELD_LEVEL,70);

	// enable trigger cheat by default
	plr->triggerpass_cheat = HasGMPermissions();

	// Make sure our name exists (for premade system)
	PlayerInfo * info = objmgr.GetPlayerInfo(plr->GetGUIDLow());
	if(info == 0)
	{
		info = new PlayerInfo;
		info->cl = plr->getClass();
		info->gender = plr->getGender();
		info->guid = plr->GetGUIDLow();
		info->name = strdup(plr->GetName());
		info->lastLevel = plr->getLevel();
		info->lastOnline = UNIXTIME;
		info->lastZone = plr->GetZoneId();
		info->officerNote = NULL;
		info->publicNote = NULL;
		info->race = plr->getRace();
		info->Rank = plr->GetPVPRank();
		info->team = plr->GetTeam();
		info->m_Group=0;
		info->subGroup=0;
		objmgr.AddPlayerInfo(info);
	}
	plr->m_playerInfo = info;
	info->m_loggedInPlayer = plr;

	// account data == UI config
#ifndef USING_BIG_ENDIAN
	StackWorldPacket<128> data(SMSG_ACCOUNT_DATA_MD5);
#else
	WorldPacket data(SMSG_ACCOUNT_DATA_MD5, 128);
#endif

	MD5Hash md5hash;

	for (int i = 0; i < 8; i++)
	{
		AccountDataEntry* acct_data = GetAccountData(i);

		if (!acct_data->data)
		{
			data << uint64(0) << uint64(0);				// Nothing.
			continue;
		}
		md5hash.Initialize();
		md5hash.UpdateData((const uint8*)acct_data->data, acct_data->sz);
		md5hash.Finalize();

#ifndef USING_BIG_ENDIAN
		data.Write(md5hash.GetDigest(), MD5_DIGEST_LENGTH);
#else
		data.append(md5hash.GetDigest(), MD5_DIGEST_LENGTH);
#endif
	}
	SendPacket(&data);

	// Set TIME OF LOGIN
	CharacterDatabase.Execute (
		"UPDATE characters SET online = 1 WHERE guid = %u" , plr->GetGUIDLow());

	bool enter_world = true;
#ifndef CLUSTERING
	// Find our transporter and add us if we're on one.
	if(plr->m_TransporterGUID != 0)
	{
		Transporter * pTrans = objmgr.GetTransporter(plr->m_TransporterGUID);
		if(pTrans)
		{
			if(plr->isDead())
			{
				plr->ResurrectPlayer();
				plr->SetUInt32Value(UNIT_FIELD_HEALTH, plr->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
				plr->SetUInt32Value(UNIT_FIELD_POWER1, plr->GetUInt32Value(UNIT_FIELD_MAXPOWER1));
			}

			float c_tposx = pTrans->GetPositionX() + plr->m_TransporterX;
			float c_tposy = pTrans->GetPositionY() + plr->m_TransporterY;
			float c_tposz = pTrans->GetPositionZ() + plr->m_TransporterZ;
			if(plr->GetMapId() != pTrans->GetMapId())	   // loaded wrong map
			{
				plr->SetMapId(pTrans->GetMapId());
#ifndef USING_BIG_ENDIAN
				StackWorldPacket<20> dataw(SMSG_NEW_WORLD);
#else
				WorldPacket dataw(SMSG_NEW_WORLD, 20);
#endif
				dataw << pTrans->GetMapId() << c_tposx << c_tposy << c_tposz << plr->GetOrientation();
				SendPacket(&dataw);

				// shit is sent in worldport ack.
				enter_world = false;
			}

			plr->SetPosition(c_tposx, c_tposy, c_tposz, plr->GetOrientation(), false);
			plr->m_CurrentTransporter = pTrans;
			pTrans->AddPlayer(plr);
		}
	}
#endif

	sLog.outString("Player %s logged in.", plr->GetName());

#ifndef CLUSTERING
	// send extended message
	sWorld.BroadcastExtendedMessage(this, "[SM:PLRLOGIN:%u:%u]%s", plr->getRace(), plr->getClass(), plr->GetName());
	if(HasFlag(ACCOUNT_FLAG_XTEND_INFO))
	{
		// send gm list
		stringstream ssg;
		uint32 c = 0;
		for(set<WorldSession*>::iterator itr = sWorld.gmList.begin(); itr != sWorld.gmList.end(); ++itr)
		{
			if((*itr)->GetPlayer())
			{
				ssg << *(*itr)->GetPlayer()->GetNameString();
				ssg << ",";
				++c;
			}
		}
		_player->BroadcastMessage("[SM:GMLIST:%s]", ssg.str().c_str());
	}

	if(HasGMPermissions())
	{
		sWorld.BroadcastExtendedMessage(this, "[SM:GMLOGIN]%s", plr->GetName());
	}

#endif

	if(plr->GetTeam() == 1)
		sWorld.HordePlayers++;
	else
		sWorld.AlliancePlayers++;

#ifndef CLUSTERING
	// send info
	sWorld.BroadcastExtendedMessage(0, "[SM:INFO:%u:%u]", sWorld.HordePlayers, sWorld.AlliancePlayers);
#endif

	if(plr->m_FirstLogin && !HasGMPermissions())
	{
		uint32 racecinematic = plr->myRace->cinematic_id;
#ifdef USING_BIG_ENDIAN
		swap32(&racecinematic);
#endif
		OutPacket(SMSG_TRIGGER_CINEMATIC, 4, &racecinematic);
	}

	sLog.outDetail( "WORLD: Created new player for existing players (%s)", plr->GetName() );

	// Login time, will be used for played time calc
	plr->m_playedtime[2] = (uint32)UNIXTIME;

	//Issue a message telling all guild members that this player has signed on
	if(plr->IsInGuild())
	{
		Guild *pGuild = objmgr.GetGuild( plr->GetGuildId() );
		if(pGuild)
		{
			WorldPacket data(50);
			data.Initialize(SMSG_GUILD_EVENT);
			data << uint8(GUILD_EVENT_MOTD);
			data << uint8(0x01);
			data << pGuild->GetGuildMotd();
			SendPacket(&data);

			data.Initialize(SMSG_GUILD_EVENT);
			data << uint8(GUILD_EVENT_HASCOMEONLINE);
			data << uint8(0x01);
			data << plr->GetName();
			data << plr->GetGUID();
			pGuild->SendMessageToGuild(0,&data,G_MSGTYPE_ALL);
		}
	}

	// Send online status to people having this char in friendlist
	sSocialMgr.LoggedIn( GetPlayer() );

	// Send MOTD
	_player->BroadcastMessage(sWorld.GetMotd());

	// Send revision (if enabled)
	if(sWorld.sendRevisionOnJoin)
	{
		uint32 rev = g_getRevision();
#ifdef WIN32
		_player->BroadcastMessage("Server: %sAscent %s r%u/%s-Win-%s %s(www.ascentemu.com)", MSG_COLOR_WHITE, BUILD_TAG,
			rev, CONFIG, ARCH, MSG_COLOR_LIGHTBLUE);		
#else
		_player->BroadcastMessage("Server: %sAscent %s r%u/%s-%s %s(www.ascentemu.com)", MSG_COLOR_WHITE, BUILD_TAG,
			rev, PLATFORM_TEXT, ARCH, MSG_COLOR_LIGHTBLUE);
#endif
	}

	if(sWorld.SendStatsOnJoin)
	{
		_player->BroadcastMessage("Online Players: %s%u |rPeak: %s%u|r Accepted Connections: %s%u",
			MSG_COLOR_WHITE, sWorld.GetSessionCount(), MSG_COLOR_WHITE, sWorld.PeakSessionCount, MSG_COLOR_WHITE, sWorld.mAcceptedConnections);
	}

	// Calculate rested experience if there is time between lastlogoff and now
	uint32 currenttime = (uint32)UNIXTIME;
	uint32 timediff = currenttime - plr->m_timeLogoff;

	if(plr->m_timeLogoff > 0 && plr->GetUInt32Value(UNIT_FIELD_LEVEL) < plr->GetUInt32Value(PLAYER_FIELD_MAX_LEVEL))	// if timelogoff = 0 then it's the first login
	{
		if(plr->m_isResting) 
		{
			// We are resting at an inn, calculate XP and add it.
			uint32 RestXP = plr->CalculateRestXP((uint32)timediff);
			plr->AddRestXP(RestXP);
			sLog.outDebug("REST: Added %d of rest XP.", RestXP);
			plr->ApplyPlayerRestState(true);
		}
		else if(timediff > 0)
		{
			// We are resting in the wilderness at a slower rate.
			uint32 RestXP = plr->CalculateRestXP((uint32)timediff);
			RestXP >>= 2;		// divide by 4 because its at 1/4 of the rate
			plr->AddRestXP(RestXP);
		}
	}

#ifdef CLUSTERING
	plr->SetInstanceID(forced_instance_id);
	plr->SetMapId(forced_map_id);
#else
	sHookInterface.OnEnterWorld2(_player);
#endif

	if(enter_world)
	{
/*		MapMgr * mapMgr = sWorldCreator.GetInstance(plr->GetMapId(), plr);
		
		if(mapMgr && mapMgr->m_battleground)
		{
			CBattleground * bg = mapMgr->m_battleground;
			int32 team;
			if(bg->GetType() <= BATTLEGROUND_ARENA_5V5 && bg->GetType() >= BATTLEGROUND_ARENA_2V2)
			{
				team = ((Arena*)bg)->GetFreeTeam();
			}
			else
			{
				team = plr->GetTeam();
			}

			if(team >= 0 && bg->CanPlayerJoin(plr))
			{
				plr->m_bgTeam = team;
				bg->PortPlayer(plr, false);
			}
			else
			{
				LocationVector vec(plr->m_bgEntryPointX, plr->m_bgEntryPointY, plr->m_bgEntryPointZ, plr->m_bgEntryPointO);
				plr->SetMapId(plr->m_bgEntryPointMap);
				plr->SetInstanceID(plr->m_bgEntryPointInstance);
			}
		}*/

		//plr->SendInitialLogonPackets();
		plr->AddToWorld();
	}

	/* delay antiflight hack checks and speedhack checks for 10 seconds. */
	_player->_delayAntiFlyUntil = UNIXTIME + 10;
	_player->_heartBeatDisabledUntil = UNIXTIME + 10;

	objmgr.AddPlayer(_player);
}

bool ChatHandler::HandleRenameCommand(const char * args, WorldSession * m_session)
{
	// prevent buffer overflow
	if(strlen(args) > 100)
		return false;

	char name1[100];
	char name2[100];

	if(sscanf(args, "%s %s", name1, name2) != 2)
		return false;

	if(VerifyName(name2, strlen(name2)) == false)
	{
		RedSystemMessage(m_session, "That name is invalid or contains invalid characters.");
		return true;
	}

	string new_name = name2;
	string oldn = name1;//some crazy func, dunno who invented that shit...
	PlayerInfo * pi = objmgr.GetPlayerInfoByName(oldn);
	if(pi == 0)
	{
		RedSystemMessage(m_session, "Player not found with this name.");
		return true;
	}

	free(pi->name);
	pi->name = strdup(new_name.c_str());

	// look in world for him
	Player * plr = objmgr.GetPlayer(pi->guid);
	if(plr != 0)
	{
		plr->SetName(new_name);
		BlueSystemMessageToPlr(plr, "%s changed your name to '%s'.", m_session->GetPlayer()->GetName(), new_name.c_str());
		plr->SaveToDB(false);
	}
	else
	{
		CharacterDatabase.WaitExecute("UPDATE characters SET name = '%s' WHERE guid = %u", CharacterDatabase.EscapeString(new_name).c_str(), (uint32)pi->guid);
	}

	GreenSystemMessage(m_session, "Changed name of '%s' to '%s'.", name1, name2);
	return true;
}
