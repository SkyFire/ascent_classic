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

using namespace std;
class QueryResult;
class DatabaseThread;

enum DatabaseType
{
	DATABASE_TYPE_NONE	  = 0,
	DATABASE_TYPE_MYSQL	 = 1,
	DATABASE_TYPE_PGSQL	 = 2,
	DATABASE_TYPE_ORACLE10  = 3,
	DATABASE_TYPE_TOTAL	 = 4,
};

class Database
{
	friend class DatabaseThread;
public:
	Database(DatabaseType type) : mType(type) {}
	virtual ~Database() {}

	virtual bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize) = 0;

	virtual void Shutdown() = 0;

	virtual QueryResult* Query(const char* QueryString, ...) = 0;
	virtual bool WaitExecute(const char* QueryString, ...) = 0;
	virtual bool Execute(const char* QueryString, ...) = 0;
	virtual string EscapeString(string Escape) = 0;

	virtual void CheckConnections() = 0;

	inline DatabaseType GetType() { return mType; }

protected:
	DatabaseType mType;
};

class QueryResult
{
public:
	QueryResult(uint32 FieldCount, uint32 RowCount, uint32 Type);
	virtual ~QueryResult();

	virtual bool NextRow() = 0;

	inline Field* Fetch() { return mCurrentRow; }
	inline uint32 GetFieldCount() const { return mFieldCount; }
	inline uint32 GetRowCount() const { return mRowCount; }

protected:

	Field *mCurrentRow;
	uint32 mFieldCount;
	uint32 mRowCount;
	uint32 mType;

};

Database* CreateDatabaseInterface(DatabaseType type);
void DestroyDatabaseInterface(Database * ptr);

#endif
