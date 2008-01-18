// guid_reassigner.cpp : Defines the entry point for the console application.
//

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <winsock2.h>
#include "Database/DatabaseEnv.h"
using namespace std;
Database sDatabase;
CLog Log;
map<uint64, uint32> guidMap;

int main(int argc, char* argv[])
{
	const char * username = "root";
	const char * password = "";
	const char * hostname = "localhost";
	const char * database = "";
	uint32 guid_high = 50001;

	printf("This is the ascent guid re-assigning tool v1.0\n");
	printf("==============================================\n\n");
	printf("This tool will attempt to re-assign all player and item guids in your database \nfrom zero to highest.\n");
	printf("WARNING: We are not responsible for any damages caused by improper use of this tool.\n");
	printf("NOTE: All groups and instances will be cleared.\n\n");

	printf("Do you want to continue? (y/n) y\n\n");

	if(!sDatabase.Initialize(hostname, 3306, username, password, database, 2, 128000))
	{
		printf("Connect failed.\n");
		return 1;
	}

	printf("Loading characters...");
	QueryResult * result = sDatabase.Query("SELECT guid FROM characters");
	if(!result)
	{
		printf("Could not load characters.");
		return 2;
	}

	do 
	{
		uint64 guid = result->Fetch()[0].GetUInt64();
		guidMap.insert(make_pair(guid, guid_high));
		++guid_high;
	} while(result->NextRow());
	delete result;
	printf(" %u.\n", guidMap.size());

	printf("Reassigning guids, this will take some time (sometimes even an hour depending on DB size)....\n");

	for(map<uint64,uint32>::iterator itr = guidMap.begin(); itr != guidMap.end(); ++itr)
	{
		printf(I64FMTD" -> %u...\n", itr->first,itr->second);
		sDatabase.Execute("UPDATE auctions SET owner = %u WHERE owner = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE auctions SET bidder = %u WHERE bidder = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET leaderGuid = %u WHERE leaderGuid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer1 = %u WHERE signer1 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer2 = %u WHERE signer2 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer3 = %u WHERE signer3 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer4 = %u WHERE signer4 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer5 = %u WHERE signer5 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer6 = %u WHERE signer6 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer7 = %u WHERE signer7 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer8 = %u WHERE signer8 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer9 = %u WHERE signer9 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE gm_tickets SET guid = %u WHERE guid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE charters SET signer1 = %u WHERE signer1 = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE guilds SET leaderGuid = %u WHERE leaderGuid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE mailbox SET player_guid = %u WHERE player_guid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE mailbox SET sender_guid = %u WHERE sender_guid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE playercooldownitems SET OwnerGuid = %u WHERE OwnerGuid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE playercooldownsecurity SET OwnerGuid = %u WHERE OwnerGuid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE playeritems SET ownerguid = %u WHERE ownerguid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE playerpets SET ownerguid = %u WHERE ownerguid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE playerpetspells SET ownerguid = %u WHERE ownerguid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE playersummonspells SET ownerguid = %u WHERE ownerguid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE questlog SET player_guid = %u WHERE player_guid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE social SET guid = %u WHERE guid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE social SET socialguid = %u WHERE socialguid = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE tutorials SET playerId = %u WHERE playerId = "I64FMTD, itr->second,itr->first);
		sDatabase.Execute("UPDATE arenateams SET leader = %u WHERE leader = "I64FMTD,itr->second,itr->first);

		// last but not least
		sDatabase.Execute("UPDATE characters SET guid = %u WHERE guid = "I64FMTD, itr->second,itr->first);
	}

	printf("\nProcessing arena team data...\n");
	result = sDatabase.Query("SELECT * FROM arenateams");
	do
	{
		Field * f = result->Fetch();
		uint32 f1,f2,f3,f4,guid;
		for(uint32 i = 0; i < 10; ++i)
		{
			const char * stuff = f[12+i].GetString();
			if(sscanf(stuff,"%u %u %u %u %u", &guid,&f1,&f2,&f3,&f4)==5)
			{
				map<uint64,uint32>::iterator itr = guidMap.find(guid);
				if(itr==guidMap.end())
					guid=0;
				else
					guid=itr->second;

				sDatabase.Execute("UPDATE arenateams SET player_data%u = '%u %u %u %u %u'", i+1, guid,f1,f2,f3,f4);
			}
		}
	}while(result->NextRow());
	delete result;
	return 0;
}

