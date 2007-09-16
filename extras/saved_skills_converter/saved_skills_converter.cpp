// saved_skills_converter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Database/DatabaseEnv.h"
#include "Common.h"
CLog Log;
#include <sstream>
Database sDatabase;

int _tmain(int argc, _TCHAR* argv[])
{
	Log.loglevel = 999;
	const char * username = "";
	const char * password = "";
	const char * dbname = "";
	if(!sDatabase.Initialize("localhost", 3306, username, password, dbname, 1, 65536))
	{
		Log.Error("SQL", "could not connect");
		return 1;
	}
	QueryResult * result = sDatabase.Query("SELECT guid, skills FROM characters");
	do 
	{
		char * skills = strdup(result->Fetch()[1].GetString());
		if(strchr(skills, ',') != 0)
		{
			// need to convert
			uint32 i = 0;
			stringstream ss;
			char * start = skills;
			char * end;
			for(;;)
			{
				// skill, skill+1, <add>0
				end = strchr(start, ',');
				if(!end)
					break;
				*end = '\0';
				uint32 val = atoi(start);
				start = end + 1;
				ss << val << " ";
				++i;
				if(!(i % 2))
					ss << "0 ";
			}

			sDatabase.Execute("UPDATE characters SET skills = '%s' WHERE guid = %u", ss.str().c_str(), result->Fetch()[0].GetUInt32());
			Log.Debug("SQL", "UPDATE characters SET skills = '%s' WHERE guid = %u", ss.str().c_str(), result->Fetch()[0].GetUInt32());
		}
	} while(result->NextRow());
	delete result;
	sDatabase.Shutdown();
	Log.Success("Program", "Exit normally");
	return 0;
}

