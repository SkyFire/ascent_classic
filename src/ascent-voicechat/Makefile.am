INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include -I$(srcdir)/../shared
INCLUDES += -I$(srcdir)/../game

bin_PROGRAMS = ascent-voicechatd

ascent-voicechatd_SOURCES = \
	ConfigReader.c \
	Main.c \
	VoiceChat.c \
	VoiceChat.h \
	VoiceChatOpcodes.h

#ascent-voicechatd_LDADD = -ldatabase -lconfig -lauth -lshared -lzlib -lnetwork
#ascent-voicechatd_LDFLAGS = -L$(srcdir)/../shared \
               -L$(srcdir)/../../dep/src/zlib \
               -L$(srcdir)/../game

