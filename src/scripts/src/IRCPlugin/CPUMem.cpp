#include "CPUMem.h"

#ifdef WIN32
#pragma warning(disable:4244)
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


#ifdef WIN32
bool m_bFirstTime = true;
LONGLONG		m_lnOldValue = 0;
LARGE_INTEGER	m_OldPerfTime100nSec;
#endif


void GenerateUptimeString(char * Dest, unsigned int seconds)
{
	unsigned int mins=0;
	unsigned int hours=0;
	unsigned int days=0;
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
	sprintf(Dest, "%d days, %d hours, %d minutes, %d seconds", days, hours, mins, seconds);
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
	a /= 2.0;
	return float(a * 100);
}

float GetRAMUsageWin32()
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	float ram = float(pmc.PagefileUsage);
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