#ifndef __CONFIGFILE_H
#define __CONFIGFILE_H

#include "common.h"

typedef struct
{
	int tcp_listen_port;
	char * tcp_listen_host;

	int udp_listen_port;
	char * udp_listen_host;

	int log_loglevel;
	char * log_logfile;

	int daemonize;
	// max channels maybe?
} server_config;

// global accessable server config struct
extern server_config g_serverConfig;

// load/parse/reload the config file
int configfile_load_config(const char * filename);
int configfile_init();
int configfile_parsecmdoverride(int argc, char* argv[]);

#endif
