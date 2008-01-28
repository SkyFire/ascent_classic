## Process this file with autoconf to produce a configure script.

AC_INIT([Ascent],
        [3.6],
        [www.ascentemu.com],
        [ascent])

AC_PREREQ(2.5)

if test -d obj; then rm -rf obj; fi

AM_CONFIG_HEADER( config.h )
INCLUDES="-I/usr/include/mysql -I/usr/include/openssl -I/usr/local/include/openssl -I/usr/local/include"
AC_SUBST(INCLUDES)
LDFLAGS="-L/usr/lib/mysql -L/usr/local/lib -L/usr/local/lib/mysql"
AC_SUBST(LDFLAGS)
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# autoheader templates
AH_TEMPLATE([USE_EPOLL], Define if you want to use the epoll socket engine module.)
AH_TEMPLATE([USE_KQUEUE], Define if you want to use the kqueue socket engine module.)
AH_TEMPLATE([HAVE_DARWIN], Define if you have MacOSX Darwin, to include the correct headers for kevent)
AH_TEMPLATE([ENABLE_CLUSTERING], Define if you want to enable clustering, this is experimental.)
AH_TEMPLATE([ENABLE_COLLISION], Define if you want to enable collision calculation, warning. this can cause. a heavy load)
AH_TEMPLATE([ENABLE_VOICECHAT], Define this if you want to enable voice chat system)
AH_TEMPLATE([USING_BIG_ENDIAN], Define this if you are on a big-endian system.)
AH_TEMPLATE([USING_LITTLE_ENDIAN], Define this if you are on a non-big-endian system.)
AH_TEMPLATE([X64], Define this if you are under an X64-based operating system.)
AH_TEMPLATE([X86], Define this if you are under an X86-based 32bit operating system.)

# Default prefix
AC_PREFIX_DEFAULT(`pwd`)
test "$prefix" = "NONE" && prefix=`pwd`
test "$sysconfdir" = "\${prefix}/etc" && sysconfdir='../etc'
test "$scriptdir" = "" && scriptdir='../lib'

# Determine the operating system
AC_MSG_CHECKING([uname -s for detecting host operating system])
OSNAME=`uname -s`
case "$OSNAME" in
	FreeBSD*)
		dnl FreeBSD Owns !
		AC_MSG_RESULT(FreeBSD Owns your hd)
		FREEBSD=yes
	;;
	Linux*)
		dnl Linux...
		AC_MSG_RESULT(Linux)
		LINUX=yes
	;;
	Darwin*)
		dnl Darwin owns to (includes BSD) :P
		AC_MSG_RESULT(Darwin)
		FREEBSD=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
esac

AC_MSG_CHECKING([uname -m for detecting host operating system architecture])
ARCH=`uname -m`
case "$ARCH" in
	amd64*)
		dnl yay 21st century hardware
		AC_MSG_RESULT(x64 feel teh power)
		AC_DEFINE(X64)
	;;
	x86_64*)
		dnl yay 21st century hardware
                AC_MSG_RESULT(x64 feel teh power)
                AC_DEFINE(X64)
	;;
	*)
		AC_MSG_RESULT(x86 oh noes i ran out of address space)
		AC_DEFINE(X86)
	;;
esac

AC_C_BIGENDIAN([AC_DEFINE(USING_BIG_ENDIAN)],[AC_DEFINE(USING_LITTLE_ENDIAN)],[])

# Required for SVN revision magic
AC_PROG_AWK

AC_CHECK_PROG([SVN], [svn], yes)
if test -z "$SVN"; then AC_MSG_ERROR(Subversion not found); fi

AC_CHECK_PROG([SED], [sed], yes)
if test -z "$SED"; then AC_MSG_ERROR(Sed not found); fi

# Checks for libraries.
AC_CHECK_LIB( pthread, pthread_create, [],
    [LDFLAGS="-pthread $LDFLAGS"
     AC_TRY_LINK([char pthread_create();],
        pthread_create();,
        [], [AC_MSG_ERROR([Missing pthread])])
    ])
    
AC_CHECK_LIB( z, compress, [],[AC_MSG_ERROR([Missing zlib])] )
AC_CHECK_LIB( compat, ftime )

if test "x$OSNAME" = "xDarwin" ; then
	AC_CHECK_LIB( crypto, SHA1_Init, [], [AC_CHECK_LIB(ssl, SHA1_Init,[],[AC_MSG_ERROR([Missing openssl])])])
	AC_DEFINE(HAVE_DARWIN)
	LIB_EXPORT="-framework System -framework Foundation"
	AC_SUBST(LIB_EXPORT)
else
	AC_CHECK_LIB( ssl, SHA1_Init, [], [AC_CHECK_LIB(ssl, SHA1_Init,[],[AC_MSG_ERROR([Missing openssl])])])
	LIB_EXPORT="-Wl,--export-dynamic"
	AC_SUBST(LIB_EXPORT)
fi

AC_ARG_ENABLE(clustering,
     AC_HELP_STRING([--enable-clustering],enable clustering. warning this is experimental.),
     [ if test "x$enable_clustering" = "xyes" ; then
   	AC_DEFINE(ENABLE_CLUSTERING)
       fi
     ],
)

AC_ARG_ENABLE(collision,
     AC_HELP_STRING([--enable-collision],enable collision calculation. warning this can cause a high load),
     [ if test "x$enable_collision" = "xyes" ; then
   	AC_DEFINE(ENABLE_COLLISION)
       fi
     ],
)

AC_ARG_ENABLE(gcov,
     AC_HELP_STRING([--enable-gcov],[enable codecovering]),
     [ if test "x$enable_gcov" = "xyes" ; then
           CXXFLAGS="$CXXFLAGS -fprofile-arcs -ftest-coverage"
       fi
     ],
)

AC_ARG_ENABLE(debug,
     AC_HELP_STRING([--enable-debug],[enable debug]),
     [ if test "x$enable_debug" = "xyes" ; then
           CXXFLAGS="$CXXFLAGS -g"
       fi
     ],
)

AC_ARG_ENABLE(allwarnings,
     AC_HELP_STRING([--enable-allwarnings],[enable Wall compiling]),
     [ if test "x$enable_allwarnings" = "xyes" ; then
	   CXXFLAGS="$CXXFLAGS -Wall"
       fi
     ],
)

AC_ARG_ENABLE(profiling,
     AC_HELP_STRING([--enable-profiling],[build for profiling using gprof.]),
     [ if test "x$enable_profiling" = "xyes" ; then
           CXXFLAGS="$CXXFLAGS -pg" ;
           #CFLAGS="$CFLAGS -pg" ;
       fi
     ],
)

AC_ARG_ENABLE(voicechat,
     AC_HELP_STRING([--enable-voicechat],enable voice chat system),
     [ if test "x$enable_voicechat" = "xyes" ; then
   	AC_DEFINE(ENABLE_VOICECHAT)
       fi
     ],
)


# Check for networking stuff
AC_MSG_CHECKING([detecting network socket engine])
if test "x$OSNAME" = "xLinux" ; then
	AC_MSG_RESULT(epoll)
	AC_CHECK_HEADERS(sys/epoll.h)
	AC_CHECK_FUNC(epoll_ctl, ,
		[AC_CHECK_LIB(epoll, epoll_ctl, , AC_MSG_ERROR([Cannot find epoll_ctl() function for socket engine]))])
	AC_DEFINE(USE_EPOLL)
fi
if test "x$OSNAME" = "xFreeBSD" ; then
	AC_MSG_RESULT(kqueue)
	AC_CHECK_FUNC(kevent, , AC_MSG_ERROR([Cannot find kevent() function for socket engine]))
	AC_CHECK_HEADER(sys/event.h, , AC_MSG_ERROR([Cannot find sys/event.h for socket engine]))
	AC_DEFINE(USE_KQUEUE)
fi

if test "x$OSNAME" = "xDarwin" ; then
	AC_MSG_RESULT(kqueue)
	AC_CHECK_FUNC(kevent, , AC_MSG_ERROR([Cannot find kevent() function for socket engine]))
	AC_CHECK_HEADER(sys/event.h, , AC_MSG_ERROR([Cannot find sys/event.h for socket engine]))
	AC_DEFINE(USE_KQUEUE)

	INCLUDES+="-I/sw/include -I/sw/include/mysql -I/usr/include/openssl"
	LDFLAGS+="-L/sw/lib -L/sw/lib/mysql -L/usr/lib"
fi

AC_CHECK_LIB( mysqlclient, mysql_init, [],[AC_CHECK_LIB(mysql, mysql_init,[],[AC_MSG_ERROR([Missing mysql])])])


AC_DEFINE_UNQUOTED([PREFIX], "${prefix}", "Prefix")
AC_DEFINE_UNQUOTED([SCRIPTDIR], "${scriptdir}", "librarydir")
AC_DEFINE_UNQUOTED([CONFDIR], "${sysconfdir}", "configdir")

AC_CONFIG_FILES([
   ./Makefile
   src/Makefile
   src/ascent-shared/Makefile
   src/ascent-world/Makefile
   src/ascent-logonserver/Makefile
   src/ascent-voicechat/Makefile
   src/ascent-realmserver/Makefile
   src/scripts/Makefile
   src/scripts/src/Makefile
   src/scripts/src/GossipScripts/Makefile
   src/scripts/src/InstanceScripts/Makefile
   src/scripts/src/ServerStatusPlugin/Makefile
   src/scripts/src/SpellHandlers/Makefile
   src/scripts/src/LUAScripting/Makefile
   dep/Makefile
   dep/src/Makefile
   dep/src/pcre/Makefile
   dep/src/zlib/Makefile
   extras/Makefile
   extras/collision/Makefile
   extras/collision/collision_dll/Makefile
])

AC_OUTPUT
