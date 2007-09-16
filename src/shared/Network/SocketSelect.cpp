/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * SocketSelect.cpp - Select-specific functions of Socket class are located here.
 *
 */

#include "Network.h"
#ifdef CONFIG_USE_SELECT

void Socket::BurstPush()
{
	if(AcquireSendLock())
		sSocketMgr.WantWrite(GetFd());
}

void Socket::ReadCallback(uint32 len)
{
	int bytes = recv(m_fd, ((char*)m_readBuffer + m_readByteCount), m_readBufferSize - m_readByteCount, 0);
	if(bytes <= 0)
	{
		m_readMutex.Release();
		Disconnect();
		return;
	}    
	else if(bytes > 0)
	{
		m_readByteCount += bytes;
		// call virtual onread()
		OnRead();
	}

	m_readMutex.Release();
}

void Socket::WriteCallback()
{
	// We should already be locked at this point, so try to push everything out.
	int bytes_written = send(m_fd, (const char*)m_writeBuffer, m_writeByteCount, 0);
	if(bytes_written < 0)
	{
		// error.
		Disconnect();
		return;
	}

	if(bytes_written)
		RemoveWriteBufferBytes(bytes_written, false);
}

#endif
