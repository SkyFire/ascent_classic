/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

#ifndef _DATABASE_H
#define _DATABASE_H

#include <string>

using namespace std;
class QueryResult;
class DatabaseThread;

class Database
{
    friend class DatabaseThread;
public:
    Database();
    ~Database();

    bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize);

    void Shutdown();

    QueryResult* Query(const char* QueryString, ...);
    bool WaitExecute(const char* QueryString, ...);
    bool Execute(const char* QueryString, ...);

    void CheckConnections();

protected:

    bool Connect();

    bool Connect(uint32 ConnectionIndex);
    bool Disconnect(uint32 ConnectionIndex);

    bool SelectDatabase();
    bool SelectDatabase(uint32 ConnectionIndex);
    bool HandleError(uint32 ConnectionIndex, uint32 ErrorNumber);

    bool SendQuery(uint32 ConnectionIndex, const char* Sql, bool Self = false);

    uint32 GetConnection();

    MYSQL ** Connections;
	char ** QueryBuffer;

   
	bool * InUseMarkers;

    int32 mConnectionCount;

    // For reconnecting a broken connection
    string mHostname;
    string mUsername;
    string mPassword;
    string mDatabaseName;
    uint32 mPort;

    uint32 mNextPing;

    DatabaseThread* mQueryThread;
};

#endif
