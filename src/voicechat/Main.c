#include "VoiceChat.h"

int m_boundFdUDP;
int m_listenFdTCP;


int Bind(struct sockaddr_in * address)
{
	m_boundFdUDP = (int)socket(AF_INET, SOCK_DGRAM, 0);
	if(m_boundFdUDP < 0)
		return -1;

	if(bind(m_boundFdUDP, (const struct sockaddr*)address, sizeof(struct sockaddr_in)) < 0)
	{
		closesocket(m_boundFdUDP);
		return -2;
	}

	return 0;
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
	struct WoWServer * ws;

	len = recv(fd, buffer, 1000, 0);
	if(len <= 0)
	{
		ws = GetServer(fd);
		if(ws)
		{
			CloseServerConnection(ws);
			return;
		}
	}

	cmd = *(int*)&buffer[0];
}

void MessageLoop()
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
			printf("select() returned < 0\n");
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

int main(int argc, char * argv[])
{
	return 0;
}
