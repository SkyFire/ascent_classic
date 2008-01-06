/*
 * Ascent MMORPG Server
 * Voice Chat Engine
 * Copyright (C) 2005-2007 Burlex <burlex@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "VoiceChat.h"

int m_boundFdUDP;
int m_listenFdTCP;

int udp_listen_port;
int tcp_listen_port;
char udp_listen_address[100];
char tcp_listen_address[100];
struct sockaddr_in m_udplisten;
struct sockaddr_in m_tcplisten;

int LoadConfigs(int * udp_port, char * udp_address, int * tcp_port, char * tcp_address);

int Bind()
{
	m_boundFdUDP = (int)socket(AF_INET, SOCK_DGRAM, 0);
	if(m_boundFdUDP < 0)
		return -1;

	if(bind(m_boundFdUDP, (const struct sockaddr*)&m_udplisten, sizeof(struct sockaddr_in)) < 0)
	{
		closesocket(m_boundFdUDP);
		return -2;
	}

	return 0;
}

int Listen()
{
	m_listenFdTCP = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_listenFdTCP < 0)
		return -1;

	if(bind(m_listenFdTCP, (const struct sockaddr*)&m_tcplisten, sizeof(struct sockaddr_in)) < 0)
	{
		closesocket(m_boundFdUDP);
		return -2;
	}

	if(listen(m_listenFdTCP, 5) < 0)
	{
		closesocket(m_listenFdTCP);
		return -3;
	}

	return 0;
}

void dumphex(char * buf, int len)
{
	int i;
	for(i = 0; i < len; ++i)
		printf("%.2X ", buf[i]);
	printf("\n");
}

void OnUdpRead()
{
	char buffer[4096];
	int recv_len;
	int result;
	uint32 i;
	struct sockaddr_in from;
	uint32 header;
	uint16 channel_id;
	uint8 user_id;
	struct VoiceChatChannel * chn;
	struct VoiceChatChannelMember * chn_member;
	struct VoiceChatChannelMember * chn_destmember;

	result = recvfrom(m_boundFdUDP, buffer, 4096, 0, (struct sockaddr*)&from, &recv_len);
	if(result < 0)
	{
		printf("recvfrom() returned <0!\n");
		return;
	}

	printf("from: %s:%u (%u bytes)\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port), recv_len);
	dumphex(buffer, recv_len);

    // TODO: Decrypt the header.

	header = *(uint32*)&buffer[0];
	channel_id = *(uint16*)&buffer[4];
	user_id = buffer[5];
	chn = GetChannel(channel_id);
	if(chn == NULL)
	{
		// invalid channel
		return;
	}

	chn_member = GetChannelMember(user_id, chn);
	if(chn_member == NULL)
	{
		// coming from an invalid user
		return;
	}

	// depending on the opcode

	// distribute the packet to the rest of the members
    for(i = 0; i < chn->member_count; ++i)
	{
		chn_destmember = &chn->members[i];
		
		// TODO: Encrypt the header
		if(chn_destmember != chn_member)
		{
			result = sendto(m_boundFdUDP, buffer, recv_len, 0, (const struct sockaddr*)&chn_destmember->address, recv_len);
		}
	}
}

void OnListenTcpRead()
{
	int new_fd;
	struct WoWServer * ws;
	struct sockaddr_in addr;
	int outlen = sizeof(struct sockaddr_in);

	new_fd = (int)accept(m_listenFdTCP, (struct sockaddr*)&addr, &outlen);
	if(new_fd < 0)
	{
		printf("accept returned <0\n");
		return;
	}

	printf("new tcp connection from: %s:%u\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	ws = CreateServer(new_fd, &addr);
}

int SetFds(FD_SET * fds)
{
	struct WoWServer * current = m_serverBegin;
	int fd_count = 2;

	FD_ZERO(fds);
	FD_SET(m_listenFdTCP, fds);
	FD_SET(m_boundFdUDP, fds);

    while(current != NULL)
	{
		++fd_count;
		FD_SET(current->fd, fds);
		current = current->next;
	}

	return fd_count;
}

void HandleTcpRead(int fd)
{
	char buffer[1000];
	int len;
	int cmd;
	struct WoWServer * ws = GetServer(fd);

	len = recv(fd, buffer, 1000, 0);
	if(ws == NULL)
	{
		printf("tcpfrom: %u bytes from fd %u with unknown sockaddr\n", len, fd);
		return;
	}

	printf("tcpfrom: %s:%u (%u bytes)\n", inet_ntoa(ws->address.sin_addr), ntohs(ws->address.sin_port), len);

	if(len <= 0)
	{
		CloseServerConnection(ws);
		return;
	}

	dumphex(buffer, len);
	cmd = *(int*)&buffer[0];
	printf("tcpfrom: got cmd %u\n", cmd);
	switch(cmd)
	{
	case VOICECHAT_CMSG_CREATE_CHANNEL:
		{
			int t = *(int*)&buffer[4];
			int request_id = *(int*)&buffer[8];
			struct VoiceChatChannel * chn;
			uint8 out_buffer[12];
            
			chn = CreateChannel(GenerateChannelId(), t);
			printf("tcp: creating channel request id %u type %u new id %u\n",  request_id, t, chn->channel_id);
			*(int*)&out_buffer[0] = VOICECHAT_SMSG_CHANNEL_CREATED;
			*(int*)&out_buffer[4] = request_id;
			*(int*)&out_buffer[8] = chn->channel_id;
			if(send(fd, out_buffer, 12, 0) != 12)
			{
				printf("send() error (create channel)\n");
				return;
			}
		}break;
	}
}

int MessageLoop()
{
	FD_SET read_fds;
	u_int i;
	int result;
	int fd_count;
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	for(;;)
	{
		fd_count = SetFds(&read_fds);
		result = select(fd_count, &read_fds, NULL, NULL, &tv);
		if(result < 0)
		{
			printf("select() returned < 0\n");
			return -1;
		}
		else
		{
			if(FD_ISSET(m_boundFdUDP, &read_fds))
				OnUdpRead();

			if(FD_ISSET(m_listenFdTCP, &read_fds))
				OnListenTcpRead();

			if(fd_count > 2)
			{
				// we have servers connected
				for(i = 0; i < read_fds.fd_count; ++i)
				{
					if(read_fds.fd_array[i] == m_boundFdUDP || read_fds.fd_array[i] == m_listenFdTCP)
						continue;

					HandleTcpRead((int)read_fds.fd_array[i]);
				}
			}
		}
	}
}

void GenerateSockAddr(struct sockaddr_in * ptr, char * addr, int port)
{
	struct hostent * h;

	memset(ptr, 0, sizeof(struct sockaddr_in));
	ptr->sin_family = AF_INET;
	ptr->sin_port = htons((short)port);
	if(!strcmp(addr, "0.0.0.0"))
		ptr->sin_addr.s_addr = htonl(INADDR_ANY);
	else
	{
		h = gethostbyname(addr);
		if(h == 0)
			ptr->sin_addr.s_addr = htonl(INADDR_ANY);
		else
			memcpy(&ptr->sin_addr.s_addr, h->h_addr_list[0], sizeof(struct in_addr));
	}
}
int main(int argc, char * argv[])
{
#ifdef WIN32
	WSADATA d;
	WSAStartup(MAKEWORD(2,0), &d);
#endif

	printf("Ascent VoiceChat Server\n");
	printf("Copyright (C) 2007 Burlex\n");
	printf("This is free software, covered by the GNU General Public License, and you are\n"
		   "welcome to change it and/or distribute copies of it under certain conditions.\n\n");

	if(LoadConfigs(&udp_listen_port, udp_listen_address, &tcp_listen_port, tcp_listen_address) < 0)
	{
		printf("Could not parse config. Exiting.\n");
		return -1;
	}

	GenerateSockAddr(&m_udplisten, udp_listen_address, udp_listen_port);
	GenerateSockAddr(&m_tcplisten, tcp_listen_address, tcp_listen_port);

	printf("Binding...");
	if(Bind() < 0)
	{
		printf("Could not bind UDP listener!\n");
		return -2;
	}
	printf(" ok!\nListening...");

	if(Listen() < 0)
	{
		printf("Could not listen on TCP port!\n");
		return -3;
	}
	printf(" ok!\n");

	printf("\nServer is now running.\n");
	if(MessageLoop() < 0)
	{
		printf("Message loop exited with errors!\n");
		return -1;
	}
	else
	{
		printf("Message loop exited without errors.");
		return 0;
	}
}
