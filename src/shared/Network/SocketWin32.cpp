/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * SocketWin32.cpp - Windows-specific functions of Socket class are located here.
 *
 */


#include "Network.h"
#ifdef CONFIG_USE_IOCP

void Socket::WriteCallback()
{
	if(m_deleted || !m_connected)
		return;

	//printf("\nSocket::Writecallback(): sendsize : %u\n", this->m_writeByteCount);
	// We don't want any writes going on while this is happening.
	m_writeMutex.Acquire();
	if(m_writeByteCount)
	{
		DWORD w_length = 0;
		DWORD flags = 0;

		// attempt to push all the data out in a non-blocking fashion.
		WSABUF buf;
		buf.len = m_writeByteCount;
		buf.buf = (char*)m_writeBuffer;

		OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_EVENT_WRITE_END);
		int r = WSASend(m_fd, &buf, 1, &w_length, flags, &ov->m_overlap, 0);
		if(r == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSA_IO_PENDING)
			{
				DecSendLock();
				Disconnect();
			}
		}
	}
	else
	{
		// Write operation is completed.
		PostSocketMessage(SOCKET_IO_EVENT_WRITE_END);
	}
	m_writeMutex.Release();
}

void Socket::SetupReadEvent()
{
	if(m_deleted || !m_connected)
		return;

	m_readMutex.Acquire();
	DWORD r_length = 0;
	DWORD flags = 0;
	WSABUF buf;
	buf.buf = (char*)m_readBuffer + m_readByteCount;
	buf.len = m_readBufferSize - m_readByteCount;

	// event that will trigger after data is receieved
	OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_EVENT_READ_COMPLETE);

	if(WSARecv(m_fd, &buf, 1, &r_length, &flags, &ov->m_overlap, 0) == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
			Disconnect();
	}
	m_readMutex.Release();
}

void Socket::ReadCallback(uint32 len)
{
	AddRecvData(len);
	OnRead();
	SetupReadEvent();
}

void Socket::AssignToCompletionPort()
{
	/*HANDLE h = */CreateIoCompletionPort((HANDLE)m_fd, m_completionPort, (ULONG_PTR)this, 0);
	//__asm int 3;
}

void Socket::PostSocketMessage(SocketIOEvent type)
{
	// don't post any more events
	if(m_deleted)
		return;

	// create struct
	OverlappedStruct * ov = new OverlappedStruct(type);

	// post it to the event queue.
	if(!PostQueuedCompletionStatus(m_completionPort, 0, (ULONG_PTR)this, &ov->m_overlap))
	{
		// This shouldn't really happen.. :/
		delete ov;
	}
}

void Socket::BurstPush()
{
	if(AcquireSendLock())
		WriteCallback();
}

#endif
