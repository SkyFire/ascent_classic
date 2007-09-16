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

#ifndef _POSTGREDATABASE_H
#define _POSTGREDATABASE_H

#include <string>
#include "../Database.h"
#include <postgre/libpq-fe.h>

using namespace std;
class PostgreQueryResult;
class PostgreDatabaseThread;

class PostgreDatabase : public Database
{
public:
	PostgreDatabase();
	~PostgreDatabase();

	bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize);

	void Shutdown();

	QueryResult* Query(const char* QueryString, ...);
	bool WaitExecute(const char* QueryString, ...);
	bool Execute(const char* QueryString, ...);

	void CheckConnections();

	/** Burlex TODO: Implement these :P
	 */
	string EscapeString(const char * QueryString, ...) {}

protected:

	bool Connect();

	bool Connect(uint32 ConnectionIndex);
	bool Disconnect(uint32 ConnectionIndex);
	PGresult * SendQuery(uint32 ConnectionIndex, const char* Sql, bool Self = false);

	uint32 GetConnection();

	PGconn ** Connections;
	char ** QueryBuffer;

	Mutex DelayedQueryBufferMutex;
	char * DelayedQueryBuffer;

	bool * InUseMarkers;

	int32 mConnectionCount;

	// For reconnecting a broken connection
	string mConnectionString;

	uint32 mPort;

	uint32 mNextPing;

	Mutex mSearchMutex;
	PostgreDatabaseThread* mQueryThread;
};

class PostgreQueryResult : public QueryResult
{
public:
	PostgreQueryResult(PGresult * res, uint32 FieldCount, uint32 RowCount);
	~PostgreQueryResult();

	bool NextRow();
	void Destroy();

protected:
	Field *mCurrentRow;
	uint32 mRow;

	PGresult * mResult;
};

#endif