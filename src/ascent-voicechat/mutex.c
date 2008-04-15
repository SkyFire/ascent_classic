#include "common.h"
#include "mutex.h"

#ifndef WIN32

/* this is done slightly differently on bsd-variants */
#if defined(__FreeBSD__) ||  defined(__APPLE_CC__) || defined(__OpenBSD__)
#define recursive_mutex_flag PTHREAD_MUTEX_RECURSIVE
#else
#define recursive_mutex_flag PTHREAD_MUTEX_RECURSIVE_NP
#endif

static int attr_init = 0;
static pthread_mutexattr_t attr;

void mutex_initialize(mutex* mut)
{
	if(!attr_init)
	{
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, recursive_mutex_flag);
		attr_init= 1;
	}
}

#endif

