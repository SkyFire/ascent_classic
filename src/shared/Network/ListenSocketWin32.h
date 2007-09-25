/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * ListenSocket<T>: Creates a socket listener on specified address and port,
 *				  requires Update() to be called every loop.
 *
 */

#ifndef LISTEN_SOCKET_WIN32_H
#define LISTEN_SOCKET_WIN32_H

#ifdef CONFIG_USE_IOCP

#include <mswsock.h>

class SERVER_DECL ListenSocketBase
{
public:
	SOCKET m_new_fd;
	char m_accept_buffer[1024];
	virtual void OnAccept() = 0;
};

template<class T>
class SERVER_DECL ListenSocket : public ListenSocketBase
{
public:
	ListenSocket(const char * ListenAddress, uint32 Port) : ListenSocketBase()
	{
		m_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		SocketOps::ReuseAddr(m_socket);
		SocketOps::Nonblocking(m_socket);

		m_address.sin_family = AF_INET;
		m_address.sin_port = ntohs((u_short)Port);
		m_address.sin_addr.s_addr = htonl(INADDR_ANY);
		m_opened = false;

		if(strcmp(ListenAddress, "0.0.0.0"))
		{
			struct hostent * hostname = gethostbyname(ListenAddress);
			if(hostname != 0)
				memcpy(&m_address.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
		}

		// bind.. well attempt to.
		int ret = bind(m_socket, (const sockaddr*)&m_address, sizeof(m_address));
		if(ret != 0)
		{
			printf("Bind unsuccessful on port %u.", Port);
			return;
		}

		ret = listen(m_socket, 5);
		if(ret != 0) 
		{
			printf("Unable to listen on port %u.", Port);
			return;
		}

		m_opened = true;
		len = sizeof(sockaddr_in);
		m_cp = sSocketMgr.GetCompletionPort();

		// assign socket to completion port
		if(CreateIoCompletionPort( ((HANDLE)m_socket), m_cp, (ULONG_PTR)this, 0 ) == INVALID_HANDLE_VALUE)
		{
			printf("CreateIoCompletionPort: %u\n", GetLastError());
		}
		
		CreateEvent();
	}

	~ListenSocket()
	{
		Close();	
	}

	/*void Update()
	{
		aSocket = WSAAccept(m_socket, (sockaddr*)&m_tempAddress, (socklen_t*)&len, NULL, NULL);
		if(aSocket == INVALID_SOCKET)
			return;

		socket = new T(aSocket);
		socket->SetCompletionPort(m_cp);
		socket->Accept(&m_tempAddress);
	}*/

	void OnAccept()
	{
		sockaddr_in * addr = (sockaddr_in*)&((char*)m_accept_buffer)[42];
		socket = new T(m_new_fd);
		socket->SetCompletionPort(m_cp);
		socket->Accept(addr);

		// post another event for the next socket
		CreateEvent();
	}

	void CreateEvent()
	{
		DWORD bytes;
		OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_EVENT_ACCEPT);
		int len = sizeof(sockaddr_in) + 16;
		m_new_fd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if(!AcceptEx(m_socket, m_new_fd, ((char*)m_accept_buffer) + sizeof(int), 0, len, len, &bytes, &ov->m_overlap))
		{
			DWORD err = WSAGetLastError();
			if(err != WSA_IO_PENDING)
				printf("AcceptEx: %u\n", err);
		}
	}

	void Close()
	{
		if(m_opened)
			SocketOps::CloseSocket(m_socket);
		m_opened = false;
	}

	inline bool IsOpen() { return m_opened; }

private:
	SOCKET m_socket;
	struct sockaddr_in m_address;
	struct sockaddr_in m_tempAddress;
	bool m_opened;
	uint32 len;
	SOCKET aSocket;
	T * socket;
	HANDLE m_cp;
};

#endif
#endif
