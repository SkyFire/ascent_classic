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

#ifndef _DATABASE_H
#define _DATABASE_H

#include <string>
#include "../Threading/Queue.h"
#include "../CallBack.h"

using namespace std;
class QueryResult;
class QueryThread;
class Database;

typedef struct{
	MYSQL*	con;
	FastMutex busy;
}MysqlCon;

struct SERVER_DECL AsyncQueryResult
{
	QueryResult * result;
	char * query;
};

class SERVER_DECL AsyncQuery
{
	friend class Database;
	SQLCallbackBase * func;
	vector<AsyncQueryResult> queries;
	Database * db;
public:
	AsyncQuery(SQLCallbackBase * f) : func(f) {}
	~AsyncQuery();
	void AddQuery(const char * format, ...);
	void Perform();
	ASCENT_INLINE void SetDB(Database * dbb) { db = dbb; }
};

class SERVER_DECL QueryBuffer
{
	vector<char*> queries;
public:
	friend class Database;
	void AddQuery( const char * format, ... );
	void AddQueryNA( const char * str );
	void AddQueryStr(const string& str);
};

class SERVER_DECL Database : public CThread
{
	friend class QueryThread;
	friend class AsyncQuery;
public:
	Database();
	~Database();

	bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize);
	bool run();
	void Shutdown();

	QueryResult* Query(const char* QueryString, ...);
	QueryResult* QueryNA(const char* QueryString);
	QueryResult * FQuery(const char * QueryString, MysqlCon * con);
	void FWaitExecute(const char * QueryString, MysqlCon * con);
	bool WaitExecute(const char* QueryString, ...);//Wait For Request Completion
	bool WaitExecuteNA(const char* QueryString);//Wait For Request Completion
	bool Execute(const char* QueryString, ...);
	bool ExecuteNA(const char* QueryString);

	void CheckConnections();
	bool ThreadRunning;

	ASCENT_INLINE string GetHostName() { return mHostname; }
	ASCENT_INLINE string GetDatabaseName() { return mDatabaseName; }
	ASCENT_INLINE uint32 GetQueueSize() { return queries_queue.get_size(); }

	string EscapeString(string Escape);
	void EscapeLongString(const char * str, uint32 len, stringstream& out);
	string EscapeString(const char * esc, MysqlCon * con);
	void QueueAsyncQuery(AsyncQuery * query);
	void EndThreads();
	void thread_proc_query();
	void FreeQueryResult(QueryResult * p);

	MysqlCon * GetFreeConnection();

	void PerformQueryBuffer(QueryBuffer * b, MysqlCon * ccon);
	void AddQueryBuffer(QueryBuffer * b);

protected:

	bool Connect(MysqlCon * con);
	bool Disconnect(MysqlCon * con);

	bool HandleError(MysqlCon*, uint32 ErrorNumber);

	bool SendQuery(MysqlCon *con, const char* Sql, bool Self = false);

	////////////////////////////////
	//FQueue<AsyncQuery*> qqueries_queue;
	FQueue<QueryBuffer*> query_buffer;

	////////////////////////////////
	FQueue<char*> queries_queue;
	MysqlCon * Connections;
	
	uint32 _counter;
	///////////////////////////////

	int32 mConnectionCount;

	// For reconnecting a broken connection
	string mHostname;
	string mUsername;
	string mPassword;
	string mDatabaseName;
	uint32 mPort;

	//uint32 mNextPing;
	QueryThread * qt;
};

class SERVER_DECL QueryResult
{
public:
	QueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount);
	~QueryResult();

	bool NextRow();
	void Delete();

	ASCENT_INLINE Field* Fetch() { return mCurrentRow; }
	ASCENT_INLINE uint32 GetFieldCount() const { return mFieldCount; }
	ASCENT_INLINE uint32 GetRowCount() const { return mRowCount; }

protected:
	MYSQL_RES* mResult;
	Field *mCurrentRow;
	uint32 mFieldCount;
	uint32 mRowCount;
	uint32 mType;
};

class SERVER_DECL QueryThread : public CThread
{
	friend class Database;
	Database * db;
public:
	QueryThread(Database * d) : CThread(), db(d) {}
	~QueryThread();
	bool run();
};

#endif
