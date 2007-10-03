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
	volatile	bool	busy;
}MysqlCon;

struct AsyncQueryResult
{
	QueryResult * result;
	char * query;
};

class AsyncQuery
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
};

class Database : public CThread
{
	friend class QueryThread;
	friend class AsyncQuery;
public:
	Database();
	~Database();

	bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize);
	void run();
	void Shutdown();

	QueryResult* Query(const char* QueryString, ...);
	QueryResult* QueryNA(const char* QueryString);
	QueryResult * FQuery(const char * QueryString, MysqlCon * con);
	bool WaitExecute(const char* QueryString, ...);//Wait For Request Completion
	bool WaitExecuteNA(const char* QueryString);//Wait For Request Completion
	bool Execute(const char* QueryString, ...);
	bool ExecuteNA(const char* QueryString);

	void CheckConnections();
	bool ThreadRunning;

	inline string GetHostName() { return mHostname; }
	inline string GetDatabaseName() { return mDatabaseName; }
	inline uint32 GetQueueSize() { return queries_queue.get_size(); }

	string EscapeString(string Escape);
	void QueueAsyncQuery(AsyncQuery * query);
	void EndThreads();
	void thread_proc_query();

protected:

	bool Connect(MysqlCon * con);
	bool Disconnect(MysqlCon * con);

	bool HandleError(MysqlCon*, uint32 ErrorNumber);

	bool SendQuery(MysqlCon *con, const char* Sql, bool Self = false);

	////////////////////////////////
	FQueue<AsyncQuery*> qqueries_queue;

	////////////////////////////////
	FQueue<char*> queries_queue;
	Mutex	lock;
	MysqlCon * Connections;
	MysqlCon * GetFreeConnection();
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

class QueryResult
{
public:
	QueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount);
	~QueryResult();

	bool NextRow();

	inline Field* Fetch() { return mCurrentRow; }
	inline uint32 GetFieldCount() const { return mFieldCount; }
	inline uint32 GetRowCount() const { return mRowCount; }

protected:
	MYSQL_RES* mResult;
	Field *mCurrentRow;
	uint32 mFieldCount;
	uint32 mRowCount;
	uint32 mType;
};

class QueryThread : public CThread
{
	friend class Database;
	Database * db;
public:
	QueryThread(Database * d) : db(d), CThread() {}
	~QueryThread();
	void run();
};

#endif
