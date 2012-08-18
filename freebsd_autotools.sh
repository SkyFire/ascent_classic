#!/bin/sh
#dirty script, for FreeBSD autotool 
aclocal -I ./m4
libtoolize --force
autoconf
autoheader
automake -a -c
#reconfigure
#autoreconf -fi