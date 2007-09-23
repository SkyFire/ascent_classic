#!/usr/bin/env sh
# Ascent 2007, Aim

# WARNING:
#  Running this script will wipe all previous svn:ignore propsets

if [ ! -f configure ] ; then
  echo "You must run this script in the root of the trunk."
  exit 1
fi

find . -type d | \
	grep -v '.svn' | \
	grep -v '.deps' | \
	grep -v '^./win' | \
	grep -v '^./extras' | \
	grep -v 'autom4te.cache' | \
	grep -v '^./sql' | \
	xargs svn propset svn:ignore -F .svn.ignore.global

cat .svn.ignore .svn.ignore.global > .svn.root.ignore
svn propset svn:ignore -F .svn.root.ignore .
rm .svn.root.ignore

