/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * SocketMgr - Select manager.
 *
 */

#include "Network.h"
#ifdef CONFIG_USE_POLL

initialiseSingleton(SocketMgr);

SocketMgr::SocketMgr()
{
	memset(fds, 0, sizeof(void*) * SOCKET_HOLDER_SIZE);
	memset(poll_events, 0, sizeof(pollfd) * SOCKET_HOLDER_SIZE);
	highest_fd = 0;
	socket_count = 0;
}

SocketMgr::~SocketMgr()
{

}

void SocketMgr::AddSocket(Socket * s)
{
	printf("AddSocket %u\n", s->GetFd());
	if(socket_count >= SOCKET_HOLDER_SIZE || fds[s->GetFd()] != 0)
	{
		printf("Double add\n");
		s->Delete();
		return;
	}

	/* add it to the big set */
	fds[s->GetFd()] = s;
	/*poll_events[s->GetFd()].fd = s->GetFd();
	
	if(s->GetWriteBufferSize() > 0)
		poll_events[s->GetFd()].events = POLLOUT;
	else
		poll_events[s->GetFd()].events = POLLOUT;*/

	if((s->GetFd()+1) > highest_fd)
	{
		printf("New highest fd %u\n", s->GetFd());
		highest_fd = s->GetFd()+1;
	}

	socket_count++;
}

void SocketMgr::RemoveSocket(Socket * s)
{
	printf("RemoveSocket %u\n", s->GetFd());
	//assert(false);
	if(socket_count >= SOCKET_HOLDER_SIZE || fds[s->GetFd()] == 0)
	{
		return;
	}

	fds[s->GetFd()] = 0;
	socket_count--;
}

void SocketMgr::WantWrite(int fd)
{
	//m_setLock.Acquire();
	//m_setLock.Release();
}

void SocketMgr::thread_function()
{
	Socket * s;
	uint32 i, j, k;
	bool d = false;
	printf("thread_function()\n");
	for(;;)
	{
		/* dump all the fd's into the poll() struct */
		//printf("highest_fd = %u\n", highest_fd);
		for(i = 0, j = 0; i < highest_fd; ++i)
		{
			
			if(fds[i] != 0)
			{
				//printf("Adding fd %u to poll set.\n", fds[i]->GetFd());
				poll_events[j].events = 0;
				poll_events[j].revents = 0;
				if(fds[i]->GetWriteBufferSize() > 0)
				{
					//printf("Has writable data\n");
					poll_events[j].events |= POLLOUT;
				}
				else
				{
					poll_events[j].events |= POLLIN;
				}
				poll_events[j].fd = fds[i]->GetFd();
				if(!d)printf("soll_events[%u] = %u %u %u\n", j, poll_events[j].fd, poll_events[j].events, poll_events[j].revents);
				++j;
			}
		}

		if(!j)
		{
			Sleep(20);
			continue;
		}

		k = poll(poll_events, j, 1000);
		//printf("poll() returned %u\n", k);
		if(!k || k < 0)
			continue;

		for(i = 0; i < k; ++i)
		{
			if(!poll_events[i].revents)
			{
				if(!d)
				{
					d = true;
//					assert(false);
					printf("looped\n");
				}
				continue;
			}

 		        s = fds[poll_events[i].fd];
	    		printf("poll_events[%u] = %u %u %u\n", i, poll_events[i].fd, poll_events[i].events, poll_events[i].revents);
			if(!s) continue;

			if(poll_events[i].revents & POLLERR || poll_events[i].revents & POLLHUP)
			{
				s->Delete();
				continue;
			}

			if(poll_events[i].revents & POLLIN)
				s->ReadCallback(0);

			if(poll_events[i].revents & POLLOUT)
			{
				s->BurstBegin();
				s->WriteCallback();
				s->BurstEnd();
			}
		}
	}
}

void SocketWorkerThread::run()
{
	sSocketMgr.thread_function();
}

void SocketMgr::CloseAll()
{

}

void SocketMgr::SpawnWorkerThreads()
{
	int tc = 1;
	for(int i = 0; i < tc; ++i)
		launch_thread(new SocketWorkerThread());
}

void SocketMgr::ShutdownThreads()
{

}

#endif
