#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>

#include <string>
#include <vector>
#include <map>
#include <hash_map>
#include <hash_set>
#include <set>
#include <queue>
#include <list>

// windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Typedefs
typedef unsigned long long  u64;
typedef unsigned long       u32;
typedef unsigned short      u16;
typedef unsigned char       u8;

typedef long long           i64;
typedef long                i32;
typedef short               i16;
typedef char                i8;

typedef unsigned long long  uint64;
typedef unsigned long       uint32;
typedef unsigned short      uint16;
typedef unsigned char       uint8;

typedef long long           int64;
typedef long                int32;
typedef short               int16;
typedef char                int8;

#include <mmsystem.h>
inline u32 mstime()
{
    return timeGetTime();
}

// Important shit
#include "Log.h"

#include <winsock2.h>

#define I64FMT "%016I64X"
#define I64FMTD "%I64u"
#define SI64FMTD "%I64d"
#define snprintf _snprintf
#define atoll __atoi64

#define getMSTime timeGetTime
class Database;
extern Database sDatabase;

using namespace std;

#define ASSERT assert

#endif