/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * Socket implementable class.
 *
 */

#include "Network.h"

initialiseSingleton(SocketGarbageCollector);

Socket::Socket(SOCKET fd, uint32 sendbuffersize, uint32 recvbuffersize) : m_readBufferSize(recvbuffersize), 
	m_writeBufferSize(sendbuffersize), m_fd(fd), m_readByteCount(0), m_writeByteCount(0), m_connected(false),
	m_deleted(false)
{
	// Allocate Buffers
	m_readBuffer = (uint8*)malloc(recvbuffersize);
	m_writeBuffer = (uint8*)malloc(sendbuffersize);

	// IOCP Member Variables
#ifdef CONFIG_USE_IOCP
	m_writeLock = 0;
	m_completionPort = 0;
#else
	m_writeLock = 0;
#endif

	// Check for needed fd allocation.
	if(m_fd == 0)
		m_fd = SocketOps::CreateTCPFileDescriptor();
}

Socket::~Socket()
{
	// Deallocate buffers
	free(m_readBuffer);
	free(m_writeBuffer);
}

bool Socket::Connect(const char * Address, uint32 Port)
{
	struct hostent * ci = gethostbyname(Address);
	if(ci == 0)
		return false;

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)Port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);

	SocketOps::Blocking(m_fd);
	if(connect(m_fd, (const sockaddr*)&m_client, sizeof(m_client)) == -1)
		return false;

	// at this point the connection was established
#ifdef CONFIG_USE_IOCP
	m_completionPort = sSocketMgr.GetCompletionPort();
#endif
	_OnConnect();
	return true;
}

void Socket::Accept(sockaddr_in * address)
{
	memcpy(&m_client, address, sizeof(*address));
	_OnConnect();
}

void Socket::_OnConnect()
{
	// set common parameters on the file descriptor
	SocketOps::Nonblocking(m_fd);
	SocketOps::DisableBuffering(m_fd);
	SocketOps::SetRecvBufferSize(m_fd, m_writeBufferSize);
	SocketOps::SetSendBufferSize(m_fd, m_writeBufferSize);
	m_connected = true;

	// IOCP stuff
#ifdef CONFIG_USE_IOCP
	AssignToCompletionPort();
	SetupReadEvent();
#endif
	sSocketMgr.AddSocket(this);

	// Call virtual onconnect
	OnConnect();
}

bool Socket::Send(const uint8 * Bytes, uint32 Size)
{
	bool rv;

	// This is really just a wrapper for all the burst stuff.
	BurstBegin();
	rv = BurstSend(Bytes, Size);
	if(rv)
		BurstPush();
	BurstEnd();

	return rv;
}

bool Socket::BurstSend(const uint8 * Bytes, uint32 Size)
{
	// Check for buffer space.
	if((Size + m_writeByteCount) >= m_writeBufferSize)
		return false;

	// Copy into the buffer.
	memcpy(&m_writeBuffer[m_writeByteCount], Bytes, Size);
	m_writeByteCount += Size;

	return true;
}

void Socket::RemoveReadBufferBytes(uint32 size, bool lock)
{
	if(lock)
		m_readMutex.Acquire();

	if(m_readByteCount < size)
	{
	#ifdef WIN32
		printf("Requested erase size: %u\n", size);
		printf("Actual size: %u\n", m_readByteCount);
		ASSERT(false);
	#endif
	}

	if(size == m_readByteCount)	 // complete erasure
		m_readByteCount = 0;
	else
	{
		memcpy(m_readBuffer, &m_readBuffer[size], m_readByteCount - size);
		m_readByteCount -= size;
	}

	if(lock)
		m_readMutex.Release();
}

void Socket::RemoveWriteBufferBytes(uint32 size, bool lock)
{
	if(lock)
		m_writeMutex.Acquire();

	if(m_writeByteCount < size)
	{
	#ifdef WIN32
		printf("Requested erase size: %u\n", size);
		printf("Actual size: %u\n", m_writeByteCount);
		ASSERT(false);
	#endif
	}

	if(size == m_writeByteCount)	 // complete erasure
		m_writeByteCount = 0;
	else
	{
		//memcpy(m_writeBuffer, &m_writeBuffer[size], m_writeByteCount - size);
		memmove(m_writeBuffer, &m_writeBuffer[size], m_writeByteCount - size);
		m_writeByteCount -= size;
	}

	if(lock)
		m_writeMutex.Release();
}

string Socket::GetRemoteIP()
{
	// inet_ntoa may leak memory, so we'll do this our own way.
	char * ip = (char*)inet_ntoa(m_client.sin_addr);
	string ret = string(ip);
	//free(ip);
	return ret;
}

void Socket::Disconnect()
{
	m_connected = false;

	// remove from mgr
	sSocketMgr.RemoveSocket(this);

	SocketOps::CloseSocket(m_fd);

	// Call virtual ondisconnect
	OnDisconnect();

	if(!m_deleted) Delete();
}

void Socket::Delete()
{
	if(m_deleted) return;
	m_deleted = true;

	if(m_connected) Disconnect();
	sSocketGarbageCollector.QueueSocket(this);
}

void Socket::Read(uint32 size, uint8 * buffer)
{
	assert(size <= m_readByteCount);
	memcpy(buffer, m_readBuffer, size);
	RemoveReadBufferBytes(size, false);
}

