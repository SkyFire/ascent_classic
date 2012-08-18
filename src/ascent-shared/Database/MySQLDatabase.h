#ifndef __MYSQLDATABASE_H
#define __MYSQLDATABASE_H

#include <mysql/mysql.h>

struct MySQLDatabaseConnection : public DatabaseConnection
{
	
};

class SERVER_DECL MySQLDatabase : public Database
{
	friend class QueryThread;
	friend class AsyncQuery;

	

public:
	MySQLDatabase();
	~MySQLDatabase();

	bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize);

	void Shutdown();

	string EscapeString(string Escape);
	void EscapeLongString(const char * str, uint32 len, stringstream& out);
	string EscapeString(const char * esc, DatabaseConnection * con);

	bool SupportsReplaceInto() { return true; }
	bool SupportsTableLocking() { return true; }
	
protected:




	void _BeginTransaction(DatabaseConnection * conn);
	void _EndTransaction(DatabaseConnection * conn);
	
};

class SERVER_DECL MySQLQueryResult : public QueryResult
{
public:
	MySQLQueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount);
	~MySQLQueryResult();

	bool NextRow();

protected:
	
};

#endif		// __MYSQLDATABASE_H
