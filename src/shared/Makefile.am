INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include

noinst_LIBRARIES = libshared.a libauth.a libconfig.a libdatabase.a libnetwork.a

libshared_a_SOURCES = \
    Log.cpp \
    MemoryLeaks.cpp \
    MersenneTwister.cpp \
    MersenneTwister.h \
    PreallocatedQueue.h \
    ByteBuffer.h \
    Common.h \
    Errors.h \
    Log.h \
    MemoryLeaks.h \
    Singleton.h \
    Threading.h \
    Timer.h \
    UnixThreading.h \
    Util.h \
    Util.cpp \
    WinThreading.h \
    WorldPacket.h \
    crc32.cpp \
    crc32.h \
    FastQueue.h \
    Threading/Mutex.cpp \
    Storage.h \
    ascent_getopt.h \
    ascent_getopt.cpp \
    CrashHandler.h \
    CrashHandler.cpp

libauth_a_SOURCES = \
    Auth/BigNumber.cpp \
    Auth/BigNumber.h \
    Auth/md5.c \
    Auth/md5.h \
    Auth/Sha1.cpp \
    Auth/Sha1.h \
    Auth/WowCrypt.cpp \
    Auth/WowCrypt.h


libconfig_a_SOURCES = \
    Config/Config.cpp \
    Config/Config.h \
    Config/ConfigEnv.h

libdatabase_a_SOURCES = \
    Database/DBC.cpp \
    Database/Database.cpp \
    Database/DBC.h \
    Database/Database.h \
    Database/DBCStores.cpp \
    Database/DatabaseEnv.h \
    Database/DBCStores.h \
    Database/Field.h \
    Database/DataStore.h \
    Database/dbcfile.cpp \
    Database/dbcfile.h \
    Database/impl/MySQLDatabase.h \
    Database/impl/MySQLDatabase.cpp \
    Database/impl/PostgreDatabase.h \
    Database/impl/PostgreDatabase.cpp

libnetwork_a_SOURCES = \
    Network/ListenSocketPoll.h \
    Network/ListenSocketSelect.h \
    Network/ListenSocketLinux.h \
    Network/ListenSocketFreeBSD.h \
    Network/Network.h \
    Network/Socket.cpp \
    Network/Socket.h \
    Network/SocketDefines.h \
    Network/SocketLinux.cpp \
    Network/SocketFreeBSD.cpp \
    Network/SocketMgrLinux.cpp \
    Network/SocketMgrLinux.h \
    Network/SocketMgrFreeBSD.cpp \
    Network/SocketMgrFreeBSD.h \
    Network/SocketOps.h \
    Network/SocketOpsLinux.cpp \
    Network/SocketOpsFreeBSD.cpp \
    Network/SocketMgrSelect.h \
    Network/SocketMgrSelect.cpp \
    Network/SocketOpsSelect.cpp \
    Network/SocketSelect.cpp \
    Network/SocketMgrPoll.h \
    Network/SocketMgrPoll.cpp \
    Network/SocketOpsPoll.cpp \
    Network/SocketPoll.cpp 
    
