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

#include "../DatabaseEnv.h"
#include "../../CrashHandler.h"
#include "../../NGLog.h"

#ifdef DATABASE_SUPPORT_MYSQL

MySQLDatabase::MySQLDatabase() : Database(DATABASE_TYPE_MYSQL)
{
	ThreadType = THREADTYPE_DATABASE;
	_counter=0;
	Connections = NULL;
	mConnectionCount = -1;   // Not connected.
//	mNextPing = getMSTime();
	delete_after_use = false;
	ThreadRunning = true;
}

MySQLDatabase::~MySQLDatabase()
{
	//Shutdown();
	delete [] Connections;
}

bool MySQLDatabase::Initialize(const char* Hostname, unsigned int Port, const char* Username, const char* Password, const char* DatabaseName, uint32 ConnectionCount, uint32 BufferSize)
{
	mConnectionCount = ConnectionCount;
	mHostname = Hostname;
	mUsername = Username;
	mPassword = Password;
	mDatabaseName = DatabaseName;
	mPort = Port;

	Connections = new MysqlCon[mConnectionCount];
	Log.Notice("Database", "Connecting to MySQL on %s with (%s : *********)", mHostname.c_str(), mUsername.c_str());
	
	for(int i = 0; i < mConnectionCount; ++i)
	{
		Connections[i].busy = false;
		if(!Connect(&Connections[i]))
			return false;
	}
   
	// Spawn MySQLDatabase thread
	launch_thread(this);
	
	return true;
}


bool MySQLDatabase::Connect(MysqlCon * con)
{
	MYSQL* Descriptor = mysql_init(NULL);
	if(mysql_options(Descriptor, MYSQL_SET_CHARSET_NAME, "utf8"))
		sLog.outString("sql: Could not set utf8 character set [!!]");

   // MYSQL* Descriptor2 = Descriptor;
	// Set reconnect
	my_bool my_true = true;
	if (mysql_options(Descriptor, MYSQL_OPT_RECONNECT, &my_true))
		sLog.outString("sql: MYSQL_OPT_RECONNECT could not be set, connection drops may occur but will be counteracted.");

	con->con = mysql_real_connect(Descriptor, mHostname.c_str(),
		mUsername.c_str(), mPassword.c_str(), "", mPort, NULL, 0);
	if(con->con == NULL)
	{
		printf("sql: Connection failed. Reason was `%s`", mysql_error(Descriptor));
		return false;
	}
	
	if(mysql_select_db(con->con, mDatabaseName.c_str()))
	{
		sLog.outError("sql: Select of MySQLDatabase %s failed due to `%s`", mDatabaseName.c_str(),
			mysql_error(con->con));
		return false;
	}

	return true;
}


bool MySQLDatabase::Disconnect(MysqlCon * con)
{
	if(con->con)
	{
		mysql_close(con->con);
		con->con = NULL;
		return true;
	}
	return false;
}

void MySQLDatabase::CheckConnections()
{
/*
	// Check every 30 seconds (TODO: MAKE CUSTOMIZABLE)
	if(getMSTime() < mNextPing)
		return;

	mNextPing = getMSTime() + 60000;
	mSearchMutex.Acquire();
	for(uint32 i = 0; i < mNormalCount; ++i)
	{
		if(InUseMarkers[i].AttemptAcquire())
		{
			if(Connections[i] && mysql_ping(Connections[i]))
			{
				sLog.outString("sql: mysql_ping failed, attempting to reconnect to MySQLDatabase...");
				Disconnect(i);
				if(!Connect(i)) {
					sLog.outError("sql: Connection %u was unable to reconnect. This could mean a failure with your MySQLDatabase.");
					Disconnect(i);
				} else {
					SelectDatabase(i);
				}
				InUseMarkers[i].Release();
			} else if(Connections[i] == 0)
			{
				// Attempt to reconnect.
				if(Connect(i) && SelectDatabase(i))
				{
					sLog.outString("sql: Connection %u re-established.", i);
				}
			}
			InUseMarkers[i].Release();
		}
	}
	mSearchMutex.Release();
	*/
}

MysqlCon * MySQLDatabase::GetFreeConnection()
{
	lock.Acquire();
	while(true)
	{
		MysqlCon *con=&Connections[(++_counter)%mConnectionCount];
		if(!con->busy)
		{
			con->busy=true;
			lock.Release();
			return con;
		}
	}	
}

string MySQLDatabase::EscapeString(string Escape)
{
	char a2[16384] = {0};

	MysqlCon * con = GetFreeConnection();
	const char * ret;
	if(mysql_real_escape_string(con->con, a2, Escape.c_str(), Escape.length()) == 0)
		ret = Escape.c_str();
	else
		ret = a2;
	con->busy = false;
	return string(ret);
}

void MySQLDatabase::Shutdown()
{
	sLog.outString("sql: Closing all MySQLDatabase connections...");
	for(int32 i = 0; i < mConnectionCount; ++i)
	{
		Disconnect(&Connections[i]);
	}
	sLog.outString("sql: %u connections closed.", mConnectionCount);
}

bool MySQLDatabase::SendQuery(MysqlCon *con, const char* Sql, bool Self)
{
	//dunno what it does ...leaving untouched 
	int result = mysql_query(con->con, Sql);
	if(result > 0)
	{
		sLog.outDetail("Sql query failed due to [%s], Query: [%s]", mysql_error(con->con ), Sql);
		if( Self == false && HandleError(con, mysql_errno(con->con) ) )
		{
			// Re-send the query, the connection was successful.
			// The true on the end will prevent an endless loop here, as it will
			// stop after sending the query twice.
			result=SendQuery(con, Sql, true);
		}
	}

	return (result == 0 ? true : false);
}

bool MySQLDatabase::HandleError(MysqlCon * con, uint32 ErrorNumber)
{
	// Handle errors that should cause a reconnect to the MySQLDatabase.
	switch(ErrorNumber)
	{
	case 2006:  // Mysql server has gone away
	case 2008:  // Client ran out of memory
	case 2013:  // Lost connection to sql server during query
	case 2055:  // Lost connection to sql server - system error
		{
			// Let's instruct a reconnect to the db when we encounter these errors.
			Disconnect(con);
			return Connect(con);
		}break;
	}

	return false;
}

QueryResult * MySQLDatabase::Query(const char* QueryString, ...)
{	
	char sql[16384];
	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(sql, 16384, QueryString, vlist);
	va_end(vlist);

	// Send the query
	QueryResult * qResult = NULL;
	MysqlCon * con=GetFreeConnection();

	if(SendQuery(con, sql, false))
	{
		// We got a valid query. :)
		MYSQL_RES * Result = mysql_store_result(con->con);

		// Don't think we're gonna have more than 4 billion rows......
		uint32 RowCount = mysql_affected_rows(con->con);
		uint32 FieldCount = mysql_field_count(con->con);

		// Check if we have no rows.
		if(!RowCount || !FieldCount) 
		{
			mysql_free_result(Result);
		} else 
		{
			qResult = new MySQLQueryResult( Result, FieldCount, RowCount );
			qResult->NextRow();
		}
	}
	con->busy=false;
	return qResult;
}

bool MySQLDatabase::Execute(const char* QueryString, ...)
{
	char query[16384];

	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(query, 16384, QueryString, vlist);
	va_end(vlist);

	if(!ThreadRunning)
		return WaitExecute(query);

	int len = strlen(query);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, query, len + 1);

	queries_queue.push(pBuffer);
	return true;
}

//this will wait for completion
bool MySQLDatabase::WaitExecute(const char* QueryString, ...)
{
	char sql[16384];
	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(sql, 16384, QueryString, vlist);
	va_end(vlist);

	MysqlCon*con=GetFreeConnection();
	bool Result = SendQuery(con, sql, false);
	con->busy=false;
	return Result;
}

void MySQLDatabase::run()
{
	SetThreadName("MySQL Database Execute Thread");
	SetThreadState(THREADSTATE_BUSY);
	char * query = queries_queue.pop();
	while(query)
	{
		MysqlCon * con=GetFreeConnection();
		SendQuery(con,query);
		con->busy=false;
		delete [] query;
		if(ThreadState == THREADSTATE_TERMINATE && queries_queue.size == 0)
			break;

		query = queries_queue.pop();
	}
	ThreadRunning = false;
}


MySQLQueryResult::MySQLQueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount) : QueryResult(FieldCount, RowCount, DATABASE_TYPE_MYSQL), mResult(res)
{

}

MySQLQueryResult::~MySQLQueryResult()
{
	
}

bool MySQLQueryResult::NextRow()
{
	MYSQL_ROW row = mysql_fetch_row(mResult);
	if(row == NULL)
		return false;

	for(uint32 i = 0; i < mFieldCount; ++i)
		mCurrentRow[i].SetValue(row[i]);

	return true;
}

void MySQLQueryResult::Destroy()
{
	mysql_free_result(mResult);
	mResult = 0;
}

#endif


