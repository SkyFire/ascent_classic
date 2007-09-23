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
	grep -v '.libs' | \
	grep -v '^./win' | \
	grep -v '^./extras' | \
	grep -v 'autom4te.cache' | \
	grep -v '^./sql' | \
	xargs svn propset svn:ignore -F .svn.ignore.global

cat .svn.ignore .svn.ignore.global > .svn.root.ignore
svn propset svn:ignore -F .svn.root.ignore .
rm .svn.root.ignore

# Also add the 2 generated binaries.
cat .svn.ignore.global > .svn.ascent.ignore
echo "ascent" >> .svn.ascent.ignore
svn propset svn:ignore -F .svn.ascent.ignore src/ascent/.
rm .svn.ascent.ignore

cat .svn.ignore.global > .svn.logonserver.ignore
echo "logonserver" >> .svn.logonserver.ignore
svn propset svn:ignore -F .svn.logonserver.ignore src/logonserver/.
rm .svn.logonserver.ignore

# Now fixing the eol-style's 
echo "Updating eol-styles"

NEWLINE='
'
IFS=$NEWLINE

for i in `find . \
  -name "*.h" -o \
  -name "*.cpp" -o \
  -name "Makefile.am" -o \
  -name "configure.ac" -o \
  -name "*.txt" -o \
  -name "*.sql" -o \
  -name "*.conf" -o \
  -name "*.gm" -o \
  -name "*.patch" -o \
  -name "*.html"`; do
        EOLSTYLE=$(svn pg svn:eol-style $i)
        if [ $? -eq 0 ] && [ -z "$EOLSTYLE" ] ; then
		# first remove the returns just incase. (fucking windows)
		tr -d '\r' < $i > ${i}.tmp 
		mv ${i}.tmp $i
		svn ps svn:eol-style native $i 
        fi
done

