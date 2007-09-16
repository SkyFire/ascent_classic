/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * SocketMgr - iocp-based SocketMgr for Select socket engine.
 *
 */

#ifndef SOCKETMGR_H_POLL
#define SOCKETMGR_H_POLL

#ifdef CONFIG_USE_POLL

#include <poll.h>

#define SOCKET_HOLDER_SIZE 65536    // You don't want this number to be too big, otherwise you're gonna be eating
                                    // memory. 65536 = 256KB, so thats no big issue for now, and I really can't
                                    // see anyone wanting to have more than 65536 concurrent connections.

class Socket;
class SERVER_DECL SocketMgr : public Singleton<SocketMgr>
{
	/* Unix sockets start from 0 for every process.
	 */
	Socket * fds[SOCKET_HOLDER_SIZE];

	/** How many sockets are we holding? 
	 */
	int socket_count;

	/** Highest fd - used in poll() call.
	 */
	int highest_fd;

	/** Our poll set.
	 */
	struct pollfd poll_events[SOCKET_HOLDER_SIZE];

public:

	SocketMgr();
	~SocketMgr();

	/// returns number of sockets in array
	inline int Count() { return socket_count; }

	/** socket removal/adding
	 */
	void AddSocket(Socket * s);
	void RemoveSocket(Socket * s);
	void WantWrite(int fd);

	void SpawnWorkerThreads();
	void CloseAll();
	void ShutdownThreads();

	void thread_function();
};

#define sSocketMgr SocketMgr::getSingleton()

class SocketWorkerThread : public ThreadBase
{
public:
	void run();
};

#endif
#endif
