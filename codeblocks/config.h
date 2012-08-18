/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* "configdir" */
#define CONFDIR "."

/* Define if you want to enable clustering */
/* #undef ENABLE_CLUSTERING */

/* Define if you want to enable collision calculation */
/* #define ENABLE_COLLISION 1 */

/* Define this if you want postgres support. */
/* #undef ENABLE_DATABASE_POSTGRES */

/* Define this if you want sqlite support. */
/* #undef ENABLE_DATABASE_SQLITE */

/* Define this if you want to enable voice chat system */
/* #undef ENABLE_VOICECHAT */

/* Define if you have MacOSX Darwin */
/* #undef HAVE_DARWIN */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `compat' library (-lcompat). */
/* #undef HAVE_LIBCOMPAT */

/* Define to 1 if you have the `crypto' library (-lcrypto). */
/* #undef HAVE_LIBCRYPTO */

/* Define to 1 if you have the `epoll' library (-lepoll). */
/* #undef HAVE_LIBEPOLL */

/* Define to 1 if you have the `pcre' library (-lpcre). */
#define HAVE_LIBPCRE 1

/* Define to 1 if you have the `ssl' library (-lssl). */
/* #undef HAVE_LIBSSL */

/* Define to 1 if you have the `z' library (-lz). */
#define HAVE_LIBZ 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if MySQL libraries are available */
#define HAVE_MYSQL 1

/* Define to 1 if PostgreSQL libraries are available */
/* #undef HAVE_POSTGRESQL */

/* Define if you have POSIX threads libraries and header files. */
/* #undef HAVE_PTHREAD */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/epoll.h> header file. */
#define HAVE_SYS_EPOLL_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define this if you want to disable mysql support. */
/* #undef NO_DBLAYER_MYSQL */

/* Name of package */
#define PACKAGE "ascent"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "www.ascentemu.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Ascent"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Ascent 3.6"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ascent"

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.6"

/* "Prefix" */
#define PREFIX "."

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* "librarydir" */
#define SCRIPTDIR "./script_bin"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* SVN revision generated */
#define SVN_REVISION_DONE 1

/* Define if you want to use the epoll socket engine module. */
#define USE_EPOLL 1

/* Define if you want to use the kqueue socket engine module. */
/* #undef USE_KQUEUE */

/* Define if you want to use the kqueue socket engine module for DFLY. */
/* #undef USE_KQUEUE_DFLY */

/* Version number of package */
#define VERSION "3.6"

/* Define this if you are under an X64-based operating system. */
#define X64 1

/* Define this if you are under an X86-based 32bit operating system. */
/* #undef X86 */
