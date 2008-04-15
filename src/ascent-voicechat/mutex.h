#ifndef __MUTEX_H
#define __MUTEX_H

#include "common.h"

#ifdef WIN32

typedef CRITICAL_SECTION mutex;
static void mutex_lock(mutex* mut) { EnterCriticalSection(mut); }
static void mutex_unlock(mutex* mut) { LeaveCriticalSection(mut); }
static void mutex_initialize(mutex* mut) { InitializeCriticalSection(mut); }
static void mutex_free(mutex* mut) { DeleteCriticalSection(mut); }

#else

#include <pthread.h>
typedef pthread_mutex_t mutex;
static void mutex_lock(mutex* mut) { pthread_mutex_lock(mut); }
static void mutex_unlock(mutex* mut) { pthread_mutex_unlock(mut); }
void mutex_initialize(mutex* mut);
static void mutex_free(mutex* mut) { pthread_mutex_destroy(mut); }

#endif

#endif
