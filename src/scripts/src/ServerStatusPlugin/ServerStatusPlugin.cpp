#include "StdAfx.h"
#include "../../../shared/svn_revision.h"

#ifdef WIN32
#pragma pack(push,8)
#include "PerfCounters.h"
#pragma pack(pop)
#include <Psapi.h>

#define SYSTEM_OBJECT_INDEX					2		// 'System' object
#define PROCESS_OBJECT_INDEX				230		// 'Process' object
#define PROCESSOR_OBJECT_INDEX				238		// 'Processor' object
#define TOTAL_PROCESSOR_TIME_COUNTER_INDEX	240		// '% Total processor time' counter (valid in WinNT under 'System' object)
#define PROCESSOR_TIME_COUNTER_INDEX		6		// '% processor time' counter (for Win2K/XP)

#pragma comment(lib, "advapi32")
#pragma comment(lib, "Psapi")
#endif

extern "C" SCRIPT_DECL uint32 _exp_get_version()
{
    return MAKE_SCRIPT_VERSION(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);
}

#ifdef WIN32
bool m_bFirstTime = true;
LONGLONG		m_lnOldValue = 0;
LARGE_INTEGER	m_OldPerfTime100nSec;
uint32 number_of_cpus;
#endif

#ifdef WIN32
/* This is needed because windows is a piece of shit. ;) */
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}
#endif

char Filename[MAX_PATH];
class StatDumper
{
public:
    void DumpStats();
};

StatDumper dumper;

extern "C" SCRIPT_DECL void _exp_script_register(ScriptMgr* mgr)
{
    //strcpy(Filename, Config.MainConfig.GetStringDefault("StatDumper.Filename", "stats.xml").c_str());
    //strcpy(Filename, "stats.xml");
	if(!Config.MainConfig.GetString("StatDumper", Filename, "Filename", "stats.xml", MAX_PATH))
		strcpy(Filename, "stats.xml");

#ifdef WIN32
memset(&m_OldPerfTime100nSec, 0, sizeof(m_OldPerfTime100nSec));
SYSTEM_INFO si;
GetSystemInfo(&si);
number_of_cpus = si.dwNumberOfProcessors;
#endif
    dumper.DumpStats();

	int t = Config.MainConfig.GetIntDefault("StatDumper", "Interval", 120000);
	TimedEvent * te = TimedEvent::Allocate(&dumper, new CallbackP0<StatDumper>(&dumper, &StatDumper::DumpStats), 1, t, 0);
    sWorld.event_AddEvent(te);
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

void GenerateUptimeString(char * Dest)
{
    uint32 seconds = sWorld.GetUptime();
    uint32 mins=0;
    uint32 hours=0;
    uint32 days=0;
    if(seconds >= 60)
    {
        mins = seconds / 60;
        if(mins)
        {
            seconds -= mins*60;
            if(mins >= 60)
            {
                hours = mins / 60;
                if(hours)
                {
                    mins -= hours*60;
                    if(hours >= 24)
                    {
                        days = hours/24;
                        if(days)
                            hours -= days*24;
                    }
                }
            }
        }
    }
    sprintf(Dest, "%d days, %d hours, %d minutes, %d seconds", (int)days, (int)hours, (int)mins, (int)seconds);
}

#ifdef WIN32

float GetCPUUsageWin32()
{
    CPerfCounters<LONGLONG> PerfCounters;
    DWORD dwObjectIndex = PROCESS_OBJECT_INDEX;
    DWORD dwCpuUsageIndex = PROCESSOR_TIME_COUNTER_INDEX;

    int				CpuUsage = 0;
    LONGLONG		lnNewValue = 0;
    PPERF_DATA_BLOCK pPerfData = NULL;
    LARGE_INTEGER	NewPerfTime100nSec = {0};

    lnNewValue = PerfCounters.GetCounterValueForProcessID(&pPerfData, dwObjectIndex, dwCpuUsageIndex, GetCurrentProcessId());
    NewPerfTime100nSec = pPerfData->PerfTime100nSec;

    if (m_bFirstTime)
    {
        m_bFirstTime = false;
        m_lnOldValue = lnNewValue;
        m_OldPerfTime100nSec = NewPerfTime100nSec;
        return 0;
    }

    LONGLONG lnValueDelta = lnNewValue - m_lnOldValue;
    double DeltaPerfTime100nSec = (double)NewPerfTime100nSec.QuadPart - (double)m_OldPerfTime100nSec.QuadPart;

    m_lnOldValue = lnNewValue;
    m_OldPerfTime100nSec = NewPerfTime100nSec;

    double a = (double)lnValueDelta / DeltaPerfTime100nSec;
    a /= double(number_of_cpus);
    return (a * 100);
}

float GetRAMUsageWin32()
{
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    float ram = pmc.PagefileUsage;
    ram /= 1024.0f;
    ram /= 1024.0f;
    return ram;
}

#endif

float GetCPUUsage()
{
#ifdef WIN32
    return GetCPUUsageWin32();
#else
    return 0.0f;
#endif
}

float GetRAMUsage()
{
#ifdef WIN32
    return GetRAMUsageWin32();
#else
     return 0.0f;
#endif
}

void FillOnlineTime(uint32 Time, char * Dest)
{
    uint32 seconds = Time;
    uint32 mins=0;
    uint32 hours=0;
    uint32 days=0;
    if(seconds >= 60)
    {
        mins = seconds / 60;
        if(mins)
        {
            seconds -= mins*60;
            if(mins >= 60)
            {
                hours = mins / 60;
                if(hours)
                {
                    mins -= hours*60;
                    if(hours >= 24)
                    {
                        days = hours/24;
                        if(days)
                            hours -= days*24;
                    }
                }
            }
        }
    }
    sprintf(Dest, "%d hours, %d minutes, %d seconds", (int)hours, (int)mins, (int)seconds);
}

void StatDumper::DumpStats()
{
    if (!Filename)
        return;
    FILE * f = fopen(Filename, "w");
    if (!f)
        return;

    // Dump Header
    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(f, "<?xml-stylesheet type=\"text/xsl\" href=\"server_stats.xsl\"?>\n");
    fprintf(f, "<serverpage>\n");
    fprintf(f, "  <status>\n");
    std::deque<Player*> gms;
    {
        // Dump server information.
		fprintf(f, "    <platform>Ascent r%u/%s-%s-%s</platform>\n", g_getRevision(), CONFIG, PLATFORM_TEXT, ARCH);

        char uptime[80];
        GenerateUptimeString(uptime);
        float AvgLat;
        uint32 GMCount;
        int gm = 0;
        int count = 0;
        int avg = 0;
        // lock players reader
        objmgr._playerslock.AcquireReadLock();

        HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
        for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
        {
            if(itr->second->GetSession() && itr->second->IsInWorld())
            {
                count++;
                avg += itr->second->GetSession()->GetLatency();
                if(itr->second->GetSession()->GetPermissionCount())
                {
                    gm++;
                    gms.push_back(itr->second);
                }
            }            
        }
        objmgr._playerslock.ReleaseReadLock();

        AvgLat = count ? (float)((float)avg / (float)count) : 0;
        GMCount = gm;

        fprintf(f, "    <uptime>%s</uptime>\n", uptime);
        fprintf(f, "    <oplayers>%u</oplayers>\n", (unsigned int)(sWorld.AlliancePlayers + sWorld.HordePlayers));
        fprintf(f, "    <cpu>%2.2f</cpu>\n", GetCPUUsage());
        fprintf(f, "    <qplayers>%u</qplayers>\n", (unsigned int)sWorld.GetQueueCount());
        fprintf(f, "    <ram>%.3f</ram>\n", GetRAMUsage());
        fprintf(f, "    <avglat>%.3f</avglat>\n", AvgLat);
        fprintf(f, "    <threads>%u</threads>\n", (unsigned int)sThreadMgr.GetThreadCount());
        time_t t = time(NULL);
        fprintf(f, "    <gmcount>%u</gmcount>\n", (unsigned int)GMCount);
        fprintf(f, "    <lastupdate>%s</lastupdate>\n", asctime(localtime(&t)));
        fprintf(f, "    <alliance>%u</alliance>\n", (unsigned int)sWorld.AlliancePlayers);
        fprintf(f, "    <horde>%u</horde>\n", (unsigned int)sWorld.HordePlayers);
        fprintf(f, "    <acceptedconns>%u</acceptedconns>\n", (unsigned int)sWorld.mAcceptedConnections);
        fprintf(f, "    <peakcount>%u</peakcount>\n", (unsigned int)sWorld.PeakSessionCount);
		fprintf(f, "    <wdbquerysize>%u</wdbquerysize>\n", (unsigned int)(((MySQLDatabase*)Database_World)->GetQueueSize()));
		fprintf(f, "    <cdbquerysize>%u</cdbquerysize>\n", (unsigned int)(((MySQLDatabase*)Database_Character)->GetQueueSize()));
    }
    fprintf(f, "  </status>\n");
    Player * plr;
    uint32 t = time(NULL);
    char otime[100];
    {
        fprintf(f, "  <instances>\n");
        
        // need a big buffer..
        static char buf[500000];
        memset(buf, 0, 500000);

        // Dump Instance Information
        sWorldCreator.BuildXMLStats(buf);
        fprintf(f, buf);
        fprintf(f, "  </instances>\n");
    }
    {
        // GM Information
        fprintf(f, "  <gms>\n");
        while(!gms.empty())
        {
            plr = gms.front();
            gms.pop_front();

            FillOnlineTime(t - plr->OnlineTime, otime);
            fprintf(f, "    <gmplr>\n");
            fprintf(f, "      <name>%s</name>\n", plr->GetName());
            fprintf(f, "      <race>%u</race>\n", plr->getRace());
            fprintf(f, "      <class>%u</class>\n", (unsigned int)plr->getClass());
            fprintf(f, "      <gender>%u</gender>\n", (unsigned int)plr->getGender());
            fprintf(f, "      <pvprank>%u</pvprank>\n", (unsigned int)plr->GetPVPRank());
            fprintf(f, "      <level>%u</level>\n", (unsigned int)plr->GetUInt32Value(UNIT_FIELD_LEVEL));
            fprintf(f, "      <map>%u</map>\n", (unsigned int)plr->GetMapId());
            fprintf(f, "      <areaid>%u</areaid>\n", (unsigned int)plr->GetAreaID());
            fprintf(f, "      <ontime>%s</ontime>\n", otime);
            fprintf(f, "      <latency>%u</latency>\n", (unsigned int)plr->GetSession()->GetLatency());
            fprintf(f, "      <permissions>%s</permissions>\n", plr->GetSession()->GetPermissions());
            fprintf(f, "    </gmplr>\n");
        }

        fprintf(f, "  </gms>\n");
    }

    {
    fprintf(f, "  <sessions>\n");
        // Dump Player Information
        objmgr._playerslock.AcquireReadLock();
        HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;

        for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
        {
            plr = itr->second;
            if(itr->second->GetSession() && itr->second->IsInWorld())
            {
                FillOnlineTime(t - plr->OnlineTime, otime);

                fprintf(f, "    <plr>\n");
                fprintf(f, "      <name>%s</name>\n", plr->GetName());
                fprintf(f, "      <race>%u</race>\n", (unsigned int)plr->getRace());
                fprintf(f, "      <class>%u</class>\n", (unsigned int)plr->getClass());
				fprintf(f, "      <gender>%u</gender>\n", (unsigned int)plr->getGender());
				fprintf(f, "      <pvprank>%u</pvprank>\n", (unsigned int)plr->GetPVPRank());
                fprintf(f, "      <level>%u</level>\n", (unsigned int)plr->GetUInt32Value(UNIT_FIELD_LEVEL));
                fprintf(f, "      <map>%u</map>\n", (unsigned int)plr->GetMapId());
                fprintf(f, "      <areaid>%u</areaid>\n", (unsigned int)plr->GetAreaID());
				//requested by Zdarkside for he's online map. I hope it does not scre up any parser. If so, then make a better one :P
                fprintf(f, "      <xpos>%f</xpos>\n", plr->GetPositionX ());
                fprintf(f, "      <ypos>%f</ypos>\n", plr->GetPositionY());
                fprintf(f, "      <ontime>%s</ontime>\n", otime);
                fprintf(f, "      <latency>%u</latency>\n", (unsigned int)plr->GetSession()->GetLatency());
                fprintf(f, "    </plr>\n");
                if(plr->GetSession()->GetPermissionCount() > 0)
                    gms.push_back(plr);
            }
        }
        objmgr._playerslock.ReleaseReadLock();
        fprintf(f, "  </sessions>\n");

        
    }

    fprintf(f, "</serverpage>\n");
    fclose(f);
}
