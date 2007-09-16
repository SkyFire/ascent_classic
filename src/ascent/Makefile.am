
INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include -I$(srcdir)/../shared -I$(srcdir)/../../dep/src/gm
INCLUDES += -I$(srcdir)/../game

bin_PROGRAMS = ascent

ascent_SOURCES = \
    Main.cpp \
    Master.cpp \
    Master.h \
    CConsole.cpp \
    CConsole.h

ascent_LDADD = -lgame -ldatabase -lconfig -lauth -lshared -lzlib -lnetwork -lgmscript

ascent_LDFLAGS = -L$(srcdir)/../shared \
               -L$(srcdir)/../game \
               -L$(srcdir)/../../dep/src/zlib \
               -L$(srcdir)/../../dep/src/gm \
               -Wl,-export-dynamic
#               @DLL_EXPORT@
