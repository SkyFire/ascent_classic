## Process this file with autoconf to produce a configure script.

AC_INIT([Ascent],
        [2.1.0],
        [Ascent],
        [ascent])

AC_PREREQ(2.5)

if test -d obj; then rm -rf obj; fi

AM_CONFIG_HEADER( config.h )
INCLUDES="-I/usr/include/mysql -I/usr/local/include/mysql -I/usr/include/openssl -I/usr/local/include/openssl"
AC_SUBST(INCLUDES)
LDFLAGS="-L/usr/lib/mysql -L/usr/local/lib/mysql"
AC_SUBST(LDFLAGS)
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LIBTOOL

# autoheader templates
AH_TEMPLATE([USE_EPOLL], [Define if you want to use the epoll socket engine module.])
AH_TEMPLATE([USE_KQUEUE], [Define if you want to use the kqueue socket engine module.])
AH_TEMPLATE([HAVE_DARWIN], [Define if you have MacOSX Darwin, to include the correct headers for kevent])

# Default prefix
AC_PREFIX_DEFAULT(`pwd`)
test "$prefix" = "NONE" && prefix=`pwd`
test "$sysconfdir" = "\${prefix}/etc" && sysconfdir='../etc'
test "$scriptdir" = "" && scriptdir='../lib'

# Determine architecture
#AC_CHECK_FUNCS([ kqueue kevent ])
#AC_CHECK_FUNCS([ epoll_ctl ])

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

# Required for SVN revision magic
AC_PROG_AWK

AC_CHECK_PROG([SVN], [svn], yes)
if test -z "$SVN"; then AC_MSG_ERROR(Subversion not found); fi

AC_CHECK_PROG([SED], [sed], yes)
if test -z "$SED"; then AC_MSG_ERROR(Sed not found); fi

AC_CHECK_PROG([TADA], [tada.wav\ extensions], yes)
if test -z "$TADA"; then AC_MSG_WARN(this build has no tada.wav extensions); fi

# Checks for libraries.
AC_CHECK_LIB( pthread, pthread_create, [],
    [LDFLAGS="-pthread $LDFLAGS"
     AC_TRY_LINK([char pthread_create();],
        pthread_create();,
        [], [AC_MSG_ERROR([Missing pthread])])
    ])
AC_CHECK_LIB( z, compress, [],[AC_MSG_ERROR([Missing zlib])] )
AC_CHECK_LIB( compat, ftime )
AC_CHECK_LIB( mysqlclient, mysql_init, [],[AC_CHECK_LIB(mysql, mysql_init,[],[AC_MSG_ERROR([Missing mysql])])])

if test "x$OSNAME" = "xDarwin" ; then
	AC_CHECK_LIB( crypto, SHA1_Init, [], [AC_CHECK_LIB(ssl, SHA1_Init,[],[AC_MSG_ERROR([Missing openssl])])])
	AC_DEFINE(HAVE_DARWIN)
	LIB_EXPORT="-u __dummy -framework System -framework Foundation"
	AC_SUBST(LIB_EXPORT)
else
	AC_CHECK_LIB( ssl, SHA1_Init, [], [AC_CHECK_LIB(ssl, SHA1_Init,[],[AC_MSG_ERROR([Missing openssl])])])
	LIB_EXPORT="-Wl,--export-dynamic"
	AC_SUBST(LIB_EXPORT)
fi

# THIS IS UGLY!
CPPFLAGS=""
CFLAGS=""
CXXFLAGS=""

AC_ARG_ENABLE(gcov,
     AC_HELP_STRING([--enable-gcov],[enable codecovering])],
     [ if test "x$enable_gcov" = "xyes" ; then
           CXXFLAGS="$CXXFLAGS -fprofile-arcs -ftest-coverage"
       fi
     ],
)

AC_ARG_ENABLE(debug,
     AC_HELP_STRING([--enable-debug],[enable debug])],
     [ if test "x$enable_debug" = "xyes" ; then
           CXXFLAGS="$CXXFLAGS -g"
       fi
     ],
)

AC_ARG_ENABLE(allwarnings,
     AC_HELP_STRING([--enable-allwarnings],[enable Wall compiling])],
     [ if test "x$enable_allwarnings" = "xyes" ; then
	   CXXFLAGS="$CXXFLAGS -Wall"
       fi
     ],
)

AC_ARG_ENABLE(profiling,
     AC_HELP_STRING([--enable-profiling],[build for profiling using gprof.])],
     [ if test "x$enable_profiling" = "xyes" ; then
           CXXFLAGS="$CXXFLAGS -pg" ;
           #CFLAGS="$CFLAGS -pg" ;
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

	INCLUDES="-I/sw/include/mysql -I/usr/include/openssl $INCLUDES"
	LDFLAGS="-L/sw/lib/mysql $LDFLAGS -L/usr/lib"
fi

CPPFLAGS_SAVE=$CPPFLAGS
CPPFLAGS=$INCLUDES $CPPFLAGS

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_CHECK_HEADERS( \
   limits.h sys/ioctl.h unistd.h fcntl.h float.h \
   mysql.h netdb.h netinet/in.h stddef.h sys/socket.h \
   sys/time.h sys/timeb.h opensslv.h \
 ,[],[AC_MSG_ERROR([Missing required header])])

#CPPFLAGS=$CPPFLAGS_SAVE


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE
AC_TYPE_SIZE_T
AC_TYPE_OFF_T
AC_TYPE_SIGNAL
AC_STRUCT_TM
AC_CHECK_TYPES([ ptrdiff_t ])

# Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_STRCOLL
AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STRFTIME
#AC_FUNC_STRNLEN
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(                                               \
   ftime strcspn strtoul atexit bzero floor gethostbyname     \
   gettimeofday localtime_r memset rint select socket         \
   sqrt stpcpy strchr strpbrk strrchr strstr strtol strtoul   \
)

AC_DEFINE_UNQUOTED([PREFIX], "${prefix}", "Prefix")
AC_DEFINE_UNQUOTED([SCRIPTDIR], "${scriptdir}", "librarydir")
AC_DEFINE_UNQUOTED([CONFDIR], "${sysconfdir}", "configdir")

AC_CONFIG_FILES([
   ./Makefile
   src/Makefile
   src/ascent/Makefile
   src/shared/Makefile
   src/logonserver/Makefile
   src/game/Makefile
   src/scripts/Makefile
   src/scripts/src/Makefile
   src/scripts/src/GossipScripts/Makefile
   src/scripts/src/InstanceScripts/Makefile
   src/scripts/src/ServerStatusPlugin/Makefile
   src/scripts/src/SpellHandlers/Makefile
   dep/Makefile
   dep/src/Makefile
   dep/src/zlib/Makefile
   dep/src/gm/Makefile
])

AC_OUTPUT
