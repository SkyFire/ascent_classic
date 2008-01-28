INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include -I$(srcdir)/../ascent-shared
INCLUDES += -I$(srcdir)/../../dep/src

bin_PROGRAMS = ascent-realmserver

ascent_realmserver_SOURCES = \
ClientManager.h \
ClusterManager.h \
LogonCommClient.h \
LogonCommHandler.h \
Master.h \
ObjectStorage.h \
RStdAfx.h \
Session.h \
Structures.h \
WorkerOpcodes.h \
WorkerServer.h \
WorkerServerSocket.h \
WorldSocket.h \
CharacterHandler.cpp \
ClientManager.cpp \
ClusterManager.cpp \
LogonCommClient.cpp \
LogonCommHandler.cpp \
Master.cpp \
ObjectStorage.cpp \
QueryHandler.cpp \
RStdAfx.cpp \
Session.cpp \
WorkerServer.cpp \
WorkerServerSocket.cpp \
WorldSocket.cpp 
	
ascent_realmserver_LDADD = -lshared -lzlib

ascent_realmserver_LDFLAGS = -L$(srcdir)/../ascent-shared \
               -L$(srcdir)/../../dep/src/zlib \
               -L$(srcdir)/../../dep/src 
               
