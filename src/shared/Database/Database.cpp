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


#include "DatabaseEnv.h"
#include "../CrashHandler.h"
#include "../NGLog.h"

SQLCallbackBase::~SQLCallbackBase()
{

}

Database::Database() : CThread()
{
	ThreadType = THREADTYPE_DATABASE;
	_counter=0;
	Connections = NULL;
	mConnectionCount = -1;   // Not connected.
//	mNextPing = getMSTime();
	delete_after_use = false;
	ThreadRunning = true;
}

Database::~Database()
{
	//Shutdown();
	delete [] Connections;
}

bool Database::Initialize(const char* Hostname, unsigned int Port, const char* Username, const char* Password, const char* DatabaseName, uint32 ConnectionCount, uint32 BufferSize)
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
		if(!Connect(&Connections[i]))
			return false;
	}
   
	// Spawn Database thread
	ThreadPool.ExecuteTask(this);

	// launch the query thread
	qt = new QueryThread(this);
	ThreadPool.ExecuteTask(qt);
	
	return true;
}


bool Database::Connect(MysqlCon * con)
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
		sLog.outError("sql: Select of Database %s failed due to `%s`", mDatabaseName.c_str(),
			mysql_error(con->con));
		return false;
	}

	return true;
}


bool Database::Disconnect(MysqlCon * con)
{
	if(con->con)
	{
		mysql_close(con->con);
		con->con = NULL;
		return true;
	}
	return false;
}

void Database::CheckConnections()
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
				sLog.outString("sql: mysql_ping failed, attempting to reconnect to Database...");
				Disconnect(i);
				if(!Connect(i)) {
					sLog.outError("sql: Connection %u was unable to reconnect. This could mean a failure with your Database.");
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

MysqlCon * Database::GetFreeConnection()
{
	uint32 i = 0;
	for(;;)
	{
		MysqlCon * con = &Connections[ ((i++) % mConnectionCount) ];
		if(con->busy.AttemptAcquire())
			return con;
	}

	// shouldn't be reached
	return NULL;
}

string Database::EscapeString(string Escape)
{
	char a2[16384] = {0};

	MysqlCon * con = GetFreeConnection();
	const char * ret;
	if(mysql_real_escape_string(con->con, a2, Escape.c_str(), (unsigned long)Escape.length()) == 0)
		ret = Escape.c_str();
	else
		ret = a2;
	con->busy.Release();
	return string(ret);
}

void Database::Shutdown()
{
	sLog.outString("sql: Closing all Database connections...");
	for(int32 i = 0; i < mConnectionCount; ++i)
	{
		Disconnect(&Connections[i]);
	}
	sLog.outString("sql: %u connections closed.", mConnectionCount);
}

bool Database::SendQuery(MysqlCon *con, const char* Sql, bool Self)
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

bool Database::HandleError(MysqlCon * con, uint32 ErrorNumber)
{
	// Handle errors that should cause a reconnect to the Database.
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

QueryResult * Database::Query(const char* QueryString, ...)
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
		uint32 RowCount = (uint32)mysql_affected_rows(con->con);
		uint32 FieldCount = mysql_field_count(con->con);

		// Check if we have no rows.
		if(!RowCount || !FieldCount) 
		{
			mysql_free_result(Result);
		} else 
		{
			qResult = new QueryResult( Result, FieldCount, RowCount );
			qResult->NextRow();
		}
	}
	con->busy.Release();
	return qResult;
}

QueryResult * Database::QueryNA(const char* QueryString)
{	
	// Send the query
	QueryResult * qResult = NULL;
	MysqlCon * con=GetFreeConnection();

	if(SendQuery(con, QueryString, false))
	{
		// We got a valid query. :)
		MYSQL_RES * Result = mysql_store_result(con->con);

		// Don't think we're gonna have more than 4 billion rows......
		uint32 RowCount = (uint32)mysql_affected_rows(con->con);
		uint32 FieldCount = mysql_field_count(con->con);

		// Check if we have no rows.
		if(!RowCount || !FieldCount) 
		{
			mysql_free_result(Result);
		} else 
		{
			qResult = new QueryResult( Result, FieldCount, RowCount );
			qResult->NextRow();
		}
	}
	con->busy.Release();
	return qResult;
}

QueryResult * Database::FQuery(const char * QueryString, MysqlCon * con)
{	
	// Send the query
	QueryResult * qResult = NULL;
	if(SendQuery(con, QueryString, false))
	{
		// We got a valid query. :)
		MYSQL_RES * Result = mysql_store_result(con->con);

		// Don't think we're gonna have more than 4 billion rows......
		uint32 RowCount = (uint32)mysql_affected_rows(con->con);
		uint32 FieldCount = mysql_field_count(con->con);

		// Check if we have no rows.
		if(!RowCount || !FieldCount) 
		{
			mysql_free_result(Result);
		} else 
		{
			qResult = new QueryResult( Result, FieldCount, RowCount );
			qResult->NextRow();
		}
	}
	return qResult;
}


bool Database::Execute(const char* QueryString, ...)
{
	char query[16384];

	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(query, 16384, QueryString, vlist);
	va_end(vlist);

	if(!ThreadRunning)
		return WaitExecuteNA(query);

	size_t len = strlen(query);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, query, len + 1);

	queries_queue.push(pBuffer);
	return true;
}

bool Database::ExecuteNA(const char* QueryString)
{
	if(!ThreadRunning)
		return WaitExecuteNA(QueryString);

	size_t len = strlen(QueryString);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, QueryString, len + 1);

	queries_queue.push(pBuffer);
	return true;
}

//this will wait for completion
bool Database::WaitExecute(const char* QueryString, ...)
{
	char sql[16384];
	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(sql, 16384, QueryString, vlist);
	va_end(vlist);

	MysqlCon*con=GetFreeConnection();
	bool Result = SendQuery(con, sql, false);
	con->busy.Release();
	return Result;
}

bool Database::WaitExecuteNA(const char* QueryString)
{
	MysqlCon*con=GetFreeConnection();
	bool Result = SendQuery(con, QueryString, false);
	con->busy.Release();
	return Result;
}

void Database::run()
{
	SetThreadName("MySQL Database Execute Thread");
	SetThreadState(THREADSTATE_BUSY);
	char * query = queries_queue.pop();
	while(query)
	{
		MysqlCon * con=GetFreeConnection();
		SendQuery(con,query);
		con->busy.Release();
		delete [] query;
		if(ThreadState == THREADSTATE_TERMINATE)
			break;

		query = queries_queue.pop();
	}

	if(queries_queue.get_size() > 0)
	{
		// execute all the remaining queries
		query = queries_queue.pop_nowait();
		while(query)
		{
			MysqlCon * con=GetFreeConnection();
			SendQuery(con,query);
			con->busy.Release();
			delete [] query;
		}
	}

	ThreadRunning = false;
}


QueryResult::QueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount) : mFieldCount(FieldCount), mRowCount(RowCount), mResult(res)
{
	mCurrentRow = new Field[FieldCount];
}

QueryResult::~QueryResult()
{
	mysql_free_result(mResult);
	delete [] mCurrentRow;
}

bool QueryResult::NextRow()
{
	MYSQL_ROW row = mysql_fetch_row(mResult);
	if(row == NULL)
		return false;

	for(uint32 i = 0; i < mFieldCount; ++i)
		mCurrentRow[i].SetValue(row[i]);

	return true;
}

void AsyncQuery::AddQuery(const char * format, ...)
{
	AsyncQueryResult res;
	va_list ap;
	char buffer[10000];
	size_t len;
	va_start(ap, format);
	vsnprintf(buffer, 10000, format, ap);
	va_end(ap);
	len = strlen(buffer);
	ASSERT(len);
	res.query = new char[len+1];
	res.query[len] = 0;
	memcpy(res.query, buffer, len);
	res.result = NULL;
	queries.push_back(res);
}

void AsyncQuery::Perform()
{
	MysqlCon * conn = db->GetFreeConnection();
	for(vector<AsyncQueryResult>::iterator itr = queries.begin(); itr != queries.end(); ++itr)
		itr->result = db->FQuery(itr->query, conn);

	conn->busy.Release();
	func->run(queries);

	delete this;
}

AsyncQuery::~AsyncQuery()
{
	delete func;
	for(vector<AsyncQueryResult>::iterator itr = queries.begin(); itr != queries.end(); ++itr)
	{
		if(itr->result)
			delete itr->result;

		delete itr->query;
	}
}

void Database::EndThreads()
{
	SetThreadState(THREADSTATE_TERMINATE);
	while(ThreadRunning || qt)
	{
		if(qqueries_queue.get_size() == 0)
			qqueries_queue.GetCond().Broadcast();
	
		if(queries_queue.get_size() == 0)
			queries_queue.GetCond().Broadcast();

		Sleep(100);
		if(!ThreadRunning)
			break;
			
		Sleep(1000);
	}
}

void QueryThread::run()
{
	db->thread_proc_query();
}

QueryThread::~QueryThread()
{
	db->qt = NULL;
}

void Database::thread_proc_query()
{
	AsyncQuery * q;

	q = qqueries_queue.pop();
	while(q)
	{
		q->Perform();

		if(ThreadState == THREADSTATE_TERMINATE)
			break;

		q = qqueries_queue.pop();
	}

	// kill any queries
	q = qqueries_queue.pop_nowait();
	while(q)
	{
		delete q;
		q = qqueries_queue.pop_nowait();
	}
}

void Database::QueueAsyncQuery(AsyncQuery * query)
{
	query->db = this;
	if(qt == NULL)
	{
		query->Perform();
		return;
	}

	qqueries_queue.push(query);
}
