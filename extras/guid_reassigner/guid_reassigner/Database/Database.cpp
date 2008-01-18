/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

#include "DatabaseEnv.h"
#include <stdarg.h>

Database::Database()
{
    Connections = NULL;
    InUseMarkers = NULL;
	QueryBuffer = NULL;
    mConnectionCount = -1;   // Not connected.
    mNextPing = getMSTime();
}

Database::~Database()
{
    // Close connections..

    for(int32 i = 0; i < mConnectionCount; ++i)
    {
		if(Connections)
        if(Connections[i])
            Disconnect(i);

		delete [] QueryBuffer[i];
    }

    delete [] Connections;
    delete [] InUseMarkers;
	delete [] QueryBuffer;
}

bool Database::Initialize(const char* Hostname, unsigned int Port, const char* Username, const char* Password, const char* DatabaseName, uint32 ConnectionCount, uint32 BufferSize)
{
    // Create arrays
    mConnectionCount = ConnectionCount;
    mHostname = Hostname;
    mUsername = Username;
    mPassword = Password;
    mDatabaseName = DatabaseName;
    mPort = Port;

    Connections = new MYSQL*[mConnectionCount];
    InUseMarkers = new bool[mConnectionCount];
	QueryBuffer = new char*[mConnectionCount];

    for(int i = 0; i < mConnectionCount; ++i)
    {
        Connections[i] = NULL;
        InUseMarkers[i] = false;
		QueryBuffer[i] = new char[BufferSize];
    }

    bool result = Connect();
    if(!result) return false;

    result = SelectDatabase();
    if(result && mConnectionCount > 1)
    {
        // Spawn database thread
/*        ZThread::Thread t(new DatabaseThread(this));*/
        //Log.Notice("Database", "sql: Spawned delayed database query thread...");
    }
    return result;
}

bool Database::Connect()
{
    Log.Notice("Database", "Connecting to MySQL Database on %s with (%s : *********)", mHostname.c_str(), mUsername.c_str());
    for(uint32 i = 0; i < mConnectionCount; ++i)
    {
        if(!Connect(i))
            return false;
    }
    Log.Notice("Database", "sql: %u MySQL connections established.", mConnectionCount);
    return true;
}

bool Database::Connect(uint32 ConnectionIndex)
{
    MYSQL* Descriptor = mysql_init(NULL);
    MYSQL* Descriptor2 = Descriptor;
    // Set reconnect
    my_bool my_true = true;
    if (mysql_options(Descriptor, MYSQL_OPT_RECONNECT, &my_true))
        Log.Notice("Database", "sql: MYSQL_OPT_RECONNECT could not be set, connection drops may occur but will be counteracted.");

    Descriptor = mysql_real_connect(Descriptor2, mHostname.c_str(),
        mUsername.c_str(), mPassword.c_str(), "", mPort, NULL, 0);
    if(Descriptor == NULL)
    {
        Log.Error("Database", "sql: Connection failed. Reason was `%s`", mysql_error(Descriptor2));
        return false;
    }

    Connections[ConnectionIndex] = Descriptor;
    InUseMarkers[ConnectionIndex] = false;
    return true;
}

bool Database::SelectDatabase()
{
    for(uint32 i = 0; i < mConnectionCount; ++i)
    {
        if(!SelectDatabase(i))
            return false;
    }
    //Log.Notice("Database", "sql: %u database connections ready for use.", mConnectionCount);
    return true;
}

bool Database::SelectDatabase(uint32 ConnectionIndex)
{
    if(mysql_select_db(Connections[ConnectionIndex], mDatabaseName.c_str()))
    {
        Log.Error("Database", "sql: Select of database %s failed due to `%s`", mDatabaseName.c_str(),
            mysql_error(Connections[ConnectionIndex]));
        return false;
    }
    return true;
}

bool Database::Disconnect(uint32 ConnectionIndex)
{
    if(Connections[ConnectionIndex] != 0)
    {
        printf("sql: Closing connection %u\n", ConnectionIndex);
        mysql_close(Connections[ConnectionIndex]);
        Connections[ConnectionIndex] = NULL;
        InUseMarkers[ConnectionIndex] = true;
        return true;
    }
    return false;
}

void Database::CheckConnections()
{
    // Check every 30 seconds (TODO: MAKE CUSTOMIZABLE)
    if(getMSTime() < mNextPing)
        return;

    mNextPing = getMSTime() + 60000;
    for(uint32 i = 0; i < mConnectionCount; ++i)
    {
        // Temp solution: disable mysql_ping, causing heap corruption.
        if(Connections[i] && !InUseMarkers[i] && mysql_ping(Connections[i]))
        {
            Log.Notice("Database", "sql: mysql_ping failed, attempting to reconnect to database...");
            Disconnect(i);
            if(!Connect(i)) {
                Log.Error("Database", "sql: Connection %u was unable to reconnect. This could mean a failure with your database.");
                Disconnect(i);
			} else {
				SelectDatabase(i);
			}
		} else if(Connections[i] == 0)
        {
            // Attempt to reconnect.
            if(Connect(i) && SelectDatabase(i))
            {
                Log.Notice("Database", "sql: Connection %u re-established.", i);
            }
        }
    }
}

uint32 Database::GetConnection()
{
    int32 index = -1;
    while(index == -1)
    {
        for(uint32 i = 0; i < mConnectionCount; ++i)
        {
            if(Connections[i] && InUseMarkers[i] == false)
            {
                index = i;
                break;
            }
        }
    }
    return index;
}

void Database::Shutdown()
{
    Log.Notice("Database", "sql: Closing all database connections...");
    for(uint32 i = 0; i < mConnectionCount; ++i)
    {
        if(Connections[i])
        {
            mysql_close(Connections[i]);
            Connections[i] = 0;
        }
    }
    Log.Notice("Database", "sql: %u connections closed.", mConnectionCount);
}

bool Database::SendQuery(uint32 ConnectionIndex, const char* Sql, bool Self)
{
    int result = mysql_query(Connections[ConnectionIndex], Sql);
    if(result > 0)
    {
        Log.Debug("Database", "Sql query failed due to [%s]", mysql_error(Connections[ConnectionIndex]));
        if( Self == false && HandleError( ConnectionIndex, mysql_errno(Connections[ConnectionIndex]) ) )
        {
            // Re-send the query, the connection was successful.
            // The true on the end will prevent an endless loop here, as it will
            // stop after sending the query twice.
            SendQuery(ConnectionIndex, Sql, true);
        }
    }

    return (result == 0 ? true : false);
}

bool Database::HandleError(uint32 ConnectionIndex, uint32 ErrorNumber)
{
    // Handle errors that should cause a reconnect to the database.
    switch(ErrorNumber)
    {
    case 2006:  // Mysql server has gone away
    case 2008:  // Client ran out of memory
    case 2013:  // Lost connection to sql server during query
    case 2055:  // Lost connection to sql server - system error
        {
            // Let's instruct a reconnect to the db when we encounter these errors.
            Disconnect(ConnectionIndex);

            // Re-send the query if our connection actually succeeds.
            bool val = Connect(ConnectionIndex);
            if(val)
                val = SelectDatabase(ConnectionIndex);

            return val;
        }break;
    }

    return false;
}

QueryResult * Database::Query(const char* QueryString, ...)
{
    if(QueryString == NULL) return NULL;

    va_list vlist;
    va_start(vlist, QueryString);

    // Find a free connection
    uint32 i = GetConnection();

    // Mark the connection as busy
    InUseMarkers[i] = true;

	// Apply parameters
	vsprintf(QueryBuffer[i], QueryString, vlist);
	va_end(vlist);

	// Send the query
    bool Success = SendQuery(i, QueryBuffer[i], false);
    QueryResult * qResult = NULL;

    if(Success)
    {
        // We got a valid query. :)
        MYSQL_RES * Result = mysql_store_result(Connections[i]);

        // Don't think we're gonna have more than 4 billion rows......
        uint32 RowCount = mysql_affected_rows(Connections[i]);
        uint32 FieldCount = mysql_field_count(Connections[i]);

        // Check if we have no rows.
        if(!RowCount || !FieldCount) {
            mysql_free_result(Result);
        } else {
            qResult = new QueryResult( Result, FieldCount, RowCount );
            qResult->NextRow();
        }
    }

    InUseMarkers[i] = false;
    return qResult;
}

bool Database::Execute(const char* QueryString, ...)
{
    if(QueryString == NULL) return false;

    va_list vlist;
    va_start(vlist, QueryString);

	if(mQueryThread == 0)
	{
		// No query thread.
		// Assume we're dealing with a normal query.
		uint32 Connection = GetConnection();
		InUseMarkers[Connection] = true;
        
		vsprintf(QueryBuffer[Connection], QueryString, vlist);

		bool Result = SendQuery(Connection, QueryBuffer[Connection], false);
		InUseMarkers[Connection] = false;

		return Result;
	}

	/*DelayedQueryBufferMutex.acquire();
	
	vsprintf(DelayedQueryBuffer, QueryString, vlist);
	mQueryThread->AddQuery(DelayedQueryBuffer);

	DelayedQueryBufferMutex.release();
	return true;*/
}

bool Database::WaitExecute(const char* QueryString, ...)
{
    if(QueryString == NULL) return false;

    va_list vlist;
    va_start(vlist, QueryString);

	uint32 Connection = GetConnection();
	InUseMarkers[Connection] = true;

	vsprintf(QueryBuffer[Connection], QueryString, vlist);

	bool Result = SendQuery(Connection, QueryBuffer[Connection], false);
	InUseMarkers[Connection] = false;

	return Result;
}

/************************************************************************/
/* Thread class                                                         */
/************************************************************************/

/*
DatabaseThread::DatabaseThread(Database *db) : WowdThread()
{
    ThreadType = WOWD_THREADTYPE_DATABASE;
    _db = db;
    _db->mQueryThread = this;
}

void DatabaseThread::AddQuery(std::string query)
{
    _queue.add(query);
}

void DatabaseThread::run()
{
    WowdThread::run();
    bool err;
    std::string query;
    uint32 i;
    
    while(ThreadState != WOWD_THREADSTATE_TERMINATE)
    {
        // Provision for pausing this thread.
        if(ThreadState == WOWD_THREADSTATE_PAUSED)
        {
            while(ThreadState == WOWD_THREADSTATE_PAUSED)
            {
                ZThread::Thread::sleep(200);
            }
        }

        ThreadState = WOWD_THREADSTATE_BUSY;

        if(_queue.size())
        {
            _db->mSearchMutex.acquire();
            i = _db->GetConnection();
            _db->InUseMarkers[i] = true;
            _db->mSearchMutex.release();
            
            while(_queue.size())
            {
                query = _queue.next();
                err = _db->SendQuery(i, query.c_str(), false);
            }

            _db->InUseMarkers[i] = false;
        }
        // START PROCESSING


        ThreadState = WOWD_THREADSTATE_SLEEPING;
        ZThread::Thread::sleep(200);
    }
}

DatabaseThread::~DatabaseThread()
{
    _db->mQueryThread = NULL;
}
*/