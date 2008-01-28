#include "VoiceChat.h"

int LoadConfigs(int * udp_port, char * udp_address, int * tcp_port, char * tcp_address)
{
	FILE * f;
	char setting[100];
	char value[100];
	char buffer[1000];
	char * p, *q;
	int done=0;
	int line=0;

	f = fopen("voicechat.conf", "r");
	if(!f)
		return -2;

	while(fgets(buffer,1000,f))
	{
		line++;

		if(buffer[0] == '#') continue;
		p = strstr(buffer, "=");
		if(p == 0)
			continue;

		*p = 0;
		strcpy(setting, buffer);
		q = strstr(p+1, "\n");
		if(q == 0)
		{
			printf("Parse error at line %u\n", line);
			return -3;
		}

		*q = 0;
		p++;
		strcpy(value, p);
		

		// now handle the variables
		if(!strcmp(setting, "UDPListenPort"))
		{
			*udp_port = atoi(value);
			done++;
		}
		else if(!strcmp(setting, "UDPListenAddress"))
		{
			strcpy(udp_address, value);
			done++;
		}
		else if(!strcmp(setting, "TCPListenPort"))
		{
			*tcp_port = atoi(value);
			done++;
		}
		else if(!strcmp(setting, "TCPListenAddress"))
		{
			strcpy(tcp_address, value);
			done++;
		}
		else
		{
			printf("Unhandled setting: '%s' at line %u\n", setting, line);
			continue;
		}
	}

	return (done==4) ? 0 : -4;
}
