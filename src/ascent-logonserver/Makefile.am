INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include -I$(srcdir)/../ascent-shared

bin_PROGRAMS = ascent-logonserver

ascent-logonserver_SOURCES = \
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
	AutoPatcher.h \
	AutoPatcher.cpp \
	Main.cpp \
	Main.h \
	PeriodicFunctionCall_Thread.h

logonserver_LDADD = -lshared -lzlib


logonserver_LDFLAGS = -L$(srcdir)/../ascent-shared \
               -L$(srcdir)/../../dep/src/zlib \
