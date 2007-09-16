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

Database* CreateDatabaseInterface(DatabaseType type)
{
	switch((int)type)
	{

#ifdef DATABASE_SUPPORT_MYSQL

	case DATABASE_TYPE_MYSQL:
		return new MySQLDatabase;
		break;

#endif

#ifdef DATABASE_SUPPORT_PGSQL

	case DATABASE_TYPE_PGSQL:
		return new PostgreDatabase;
		break;

#endif

	}
	sLog.outError("Invalid database type specified. It has to be in the range of 1 to 3.");
	return 0;
}

void DestroyDatabaseInterface(Database * ptr)
{
	switch((int)ptr->GetType())
	{

#ifdef DATABASE_SUPPORT_MYSQL

	case DATABASE_TYPE_MYSQL:
		delete ((MySQLDatabase*)ptr);
		return;
		break;

#endif

#ifdef DATABASE_SUPPORT_PGSQL

	case DATABASE_TYPE_PGSQL:
		delete ((PostgreDatabase*)ptr);
		return;
		break;

#endif

	}
	sLog.outError("Invalid database type specified. It has to be in the range of 1 to 3.");
}


QueryResult::QueryResult(uint32 FieldCount, uint32 RowCount, uint32 Type)
{
	mCurrentRow = new Field[FieldCount];
	mRowCount = RowCount;
	mFieldCount = FieldCount;
	mType = Type;
}

QueryResult::~QueryResult()
{
	delete [] mCurrentRow;
	switch(mType)
	{

#ifdef DATABASE_SUPPORT_MYSQL

	case DATABASE_TYPE_MYSQL:
		((MySQLQueryResult*)this)->Destroy();
		break;

#endif

#ifdef DATABASE_SUPPORT_PGSQL

	case DATABASE_TYPE_PGSQL:
		((PostgreQueryResult*)this)->Destroy();
		break;

#endif

	default:
		assert(false);
		break;
	}
}
