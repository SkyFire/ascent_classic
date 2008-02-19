#include "DatabaseEnv.h"
#include "../CrashHandler.h"
#include "../NGLog.h"
#include "MySQLDatabase.h"
#include "PostgresDatabase.h"
#include "SQLiteDatabase.h"

Database * Database::CreateDatabaseInterface(uint32 uType)
{
	switch(uType)
	{
	case 1:		// MYSQL
		return new MySQLDatabase();
		break;
		
	case 2:		// POSTGRES
		return new PostgresDatabase();
		break;

	/*9case 3:		// SQLITE
		return new SQLiteDatabase();
		break;*/

	default:
		// invalid type
		abort();
		return NULL;
	}

	// shouldnt reach
	return NULL;
}
