
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

#include "DatabaseEnv.h"
#include "../CrashHandler.h"
#include "../NGLog.h"

#if defined(ENABLE_DATABASE_MYSQL)

#include "MySQLDatabase.h"

MySQLDatabase::~MySQLDatabase()
{
	for(int32 i = 0; i < mConnectionCount; ++i)
		delete Connections[i];

	delete [] Connections;
}

MySQLDatabase::MySQLDatabase() : Database()
{

}

void MySQLDatabase::_BeginTransaction(DatabaseConnection * conn)
{
	_SendQuery( conn, "START TRANSACTION", false );
}

void MySQLDatabase::_EndTransaction(DatabaseConnection * conn)
{
	_SendQuery( conn, "COMMIT", false );
}

bool MySQLDatabase::Initialize(const char* Hostname, unsigned int port, const char* Username, const char* Password, const char* DatabaseName, uint32 ConnectionCount, uint32 BufferSize)
{


	Database::_Initialize();
	return true;
}

string MySQLDatabase::EscapeString(string Escape)
{

}





void MySQLDatabase::Shutdown()
{
	// TODO
}



MySQLQueryResult::MySQLQueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount) : QueryResult(FieldCount, RowCount), mResult( res )
{
	mCurrentRow = new Field[FieldCount];
}

MySQLQueryResult::~MySQLQueryResult()
{

}

bool MySQLQueryResult::NextRow()
{

}



#endif
