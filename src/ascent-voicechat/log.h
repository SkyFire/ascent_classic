#ifndef __LOG_H
#define __LOG_H

// logging functions
// level 0 = notice/error
// level 1 = information
// level 2 = debug

enum LOGLEVEL
{
#ifdef WIN32
#undef ERROR
#endif
	ERROR		= 0,
	NOTICE		= 0,
	WARNING		= 0,
	INFO		= 1,
	DEBUG		= 2,
};

void log_setloglevel(int new_level);
void log_setlogfile(const char * filename);
void log_write(int level, const char * format, ...);
void log_open();
void log_close();

#endif
