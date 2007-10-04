#include "StdAfx.h"
#include "../../../shared/svn_revision.h"

extern "C" SCRIPT_DECL uint32 _exp_get_version()
{
    return MAKE_SCRIPT_VERSION(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);
}

#ifdef WIN32
/* This is needed because windows is a piece of shit. ;) */
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}
#endif

char URL[MAX_PATH];
char MyID[33];
char sKey[33];
sockaddr_in server;
bool OfficialStats = false;

class AStatDumper
{
public:
	AStatDumper()
	{
		trunning=false;
	}
    void DumpStats();
	bool trunning;
};

AStatDumper dumper;

class AStatThread : public ThreadBase
{
public:
	AStatThread()
	{
		delete_after_use=false;
	}

	~AStatThread()
	{
		dumper.trunning=false;
	}
	void run();
};


extern "C" SCRIPT_DECL void _exp_script_register(ScriptMgr* mgr)
{
	int enabled = Config.MainConfig.GetIntDefault("AscentStats", "Enable", 0);
	if(!enabled) return;

	if(!Config.MainConfig.GetString("AscentStats", MyID, "ServerGUID", "NONE", 33))
		strcpy(MyID, "NONE");

	if(!Config.MainConfig.GetString("AscentStats", sKey, "ServerKey", "NONE", 33))
		strcpy(sKey, "NONE");

	//hostent * h = gethostbyname("www.ascentemu.com");
	hostent * h = gethostbyname("192.168.1.2");
	if(h == NULL)
	{
		printf("AscentStats: Could not resolve www.ascentemu.com. Aborting.\n");
		return;
	}

	memset(&server,0,sizeof(sockaddr_in));
	memcpy(&server.sin_addr.s_addr, h->h_addr_list[0], sizeof(in_addr));
	server.sin_port = ntohs(80);
	server.sin_family = AF_INET;

    dumper.DumpStats();

/*	int t = Config.MainConfig.GetIntDefault("StatDumper", "Interval", 120000);
	TimedEvent * te = TimedEvent::Allocate(&dumper, new CallbackP0<StatDumper>(&dumper, &StatDumper::DumpStats), 1, t, 0);
    sWorld.event_AddEvent(te);*/
}

#ifdef WIN32
/*** GRR ***/
int __cdecl HandleCrash(PEXCEPTION_POINTERS pExceptPtrs)
{
    return 0;
}
#endif

void SetThreadName(const char* format, ...)
{

}

void strcatprintf(char *buffer, const char * format,...)
{
	char tempbuffer[1000];
	va_list ap;
	va_start(ap,format);
	vsnprintf(tempbuffer,1000,format,ap);
	va_end(ap);
	strcat(buffer,tempbuffer);
}

char post_data[50000];
char http_request[60000];

void AStatDumper::DumpStats()
{
	post_data[0] = 0;
	http_request[0] = 0;
	float AvgLat;
	uint32 GMCount;
	int gm = 0;
	int count = 0;
	int avg = 0;
	int players=0;

	strcatprintf(post_data, "stats=Ascent|");
	strcatprintf(post_data, PLATFORM_TEXT"|");
	strcatprintf(post_data, CONFIG"|");
	strcatprintf(post_data, ARCH"|");
	strcatprintf(post_data, "%u|", g_getRevision());

    // lock players reader
    objmgr._playerslock.AcquireReadLock();

    HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
    for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
    {
        if(itr->second->GetSession() && itr->second->IsInWorld())
        {
            count++;
            avg += itr->second->GetSession()->GetLatency();
			players++;
            if(itr->second->GetSession()->GetPermissionCount())
                gm++;
        }            
    }
    objmgr._playerslock.ReleaseReadLock();

    AvgLat = count ? (float)((float)avg / (float)count) : 0;
    GMCount = gm;

	strcatprintf(post_data,"%u|%u|%u|%u|%.3f|%u", players, sWorld.GetQueueCount(), sWorld.AlliancePlayers, sWorld.HordePlayers, AvgLat, sWorld.PeakSessionCount);

	/* Post to the server */
	if(!trunning)
	{
		ThreadPool.ExecuteTask(new AStatThread);
		trunning=true;
	}
}

void AStatThread::run()
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
#ifndef WIN32
	uint32 arg = 0;
	assert(ioctl(s, FIONBIO, &arg) == 0);
#else
	u_long arg = 0;
	assert(ioctlsocket(s, FIONBIO, &arg) == 0);
#endif
		
	int len;
	char reply[400];
	int p_len = strlen(post_data);

	if(connect(s,(const sockaddr*)&server, sizeof(sockaddr_in)) < 0)
	{
		printf("AscentStats: Could not connect to www.ascentemu.com!\n");
		closesocket(s);
		delete this;
		return;
	}

	/* Formulate our HTTP request */
	strcatprintf(http_request, "POST /ascentstats/stat_update.php?server_id=%s&key=%s&port=%u HTTP/1.1\r\n", MyID, sKey, Config.MainConfig.GetIntDefault("Listen", "WorldServerPort", 8129));
	strcatprintf(http_request, "Host: www.ascentemu.com\r\n");
	strcatprintf(http_request, "Content-Length: %u\r\n", p_len);
	strcatprintf(http_request, "User-Agent: Ascent Server\r\n");
	strcatprintf(http_request, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcatprintf(http_request, "\r\n");
	strcatprintf(http_request, post_data);
	len = strlen(http_request);
	
	if(send(s, http_request, len, 0) != len)
	{
		printf("AscentStats: Could not send HTTP request to server.\n");
		closesocket(s);
		delete this;
		return;
	}

	/*if(send(s, post_data, p_len, 0) != p_len)
	{
		printf("AscentStats: Could not send multipart data.\n");
		closesocket(s);
		delete this;
		return;
	}*/
	
	for(;;)
	{
		len = recv(s, reply, 400, 0);
		if(len < 0)
		{
			printf("AscentStats: Error in recv()\n");
			closesocket(s);
			delete this;
			return;
		}

		if(strchr(reply, '\n'))
		{
			char * p = strchr(reply, ' ');
			char * r = strchr(p+1,' ');
			int reply_code;
			*r = 0;
			++p;
			reply_code = atoi(p);
			if(reply_code != 200)
			{
				printf("AscentStats: Got HTTP response code: %u\n", reply_code);
				break;
			}
			
			printf("AscentStats: Updated!\n");
			break;
		}
	}

	closesocket(s);
	delete this;
}
