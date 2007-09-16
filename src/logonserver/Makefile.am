INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include -I$(srcdir)/../shared
INCLUDES += -I$(srcdir)/../game

bin_PROGRAMS = logonserver

logonserver_SOURCES = \
	AccountCache.cpp \
	AccountCache.h \
	AuthSocket.cpp \
	AuthSocket.h \
	AuthStructs.h \
	LogonCommServer.cpp \
	LogonCommServer.h \
	LogonConsole.cpp \
	LogonConsole.h \
	LogonOpcodes.cpp \
	LogonOpcodes.h \
	LogonStdAfx.cpp \
	LogonStdAfx.h \
	Main.cpp \
	Main.h \
	PeriodicFunctionCall_Thread.h \
	RealmlistThreadMgr.cpp \
        ../game/CThreads.cpp

logonserver_LDADD = -ldatabase -lconfig -lauth -lshared -lzlib -lnetwork


logonserver_LDFLAGS = -L$(srcdir)/../shared \
               -L$(srcdir)/../../dep/src/zlib \
               -L$(srcdir)/../game

