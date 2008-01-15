
INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include -I$(srcdir)/../shared -I$(srcdir)/../../dep/src
INCLUDES += -I$(srcdir)/../game

bin_PROGRAMS = ascent

ascent_SOURCES = \
	Main.cpp \
	Master.cpp \
	Master.h \
	CConsole.cpp \
	CConsole.h \
	BaseConsole.h \
	ConsoleCommands.cpp \
	ConsoleCommands.h \
	ConsoleListener.cpp

ascent_LDADD = -lgame -ldatabase -lconfig -lauth -lshared -lzlib -lnetwork -llua -lpcre

ascent_LDFLAGS = -L$(srcdir)/../shared \
               -L$(srcdir)/../game \
               -L$(srcdir)/../../dep/src/zlib \
               -L$(srcdir)/../../dep/src/lua \
               -L$(srcdir)/../../dep/src/pcre \
               -L$(srcdir)/../../dep/src \
               @LIB_EXPORT@
