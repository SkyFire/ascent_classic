#!/bin/sh

echo Building Precompiled Header...
cd src/ascent-world
#g++ -DHAVE_CONFIG_H -I. -I. -I../.. -I/usr/include/mysql -I/usr/include/openssl -I/usr/local/include/openssl -I/usr/local/include -I. -I./../../dep/include  -I./../ascent-shared -I./../../dep/src -g -O2 -o StdAfx.h.gch StdAfx.h
g++ -DHAVE_CONFIG_H -I. -I../.. -I/usr/include/mysql -I/usr/include/openssl -I/usr/local/include/openssl -I/usr/local/include -I. -I./../../dep/include -I./../ascent-shared -I./../../dep/src -g -O2 -o StdAfx.h.gch StdAfx.h

echo Symlinking script gch files.
cd ../scripts/src/GossipScripts
rm -f StdAfx.h.gch
ln -s ../../../ascent-world/StdAfx.h.gch ./StdAfx.h.gch

cd ../InstanceScripts
rm -f StdAfx.h.gch
ln -s ../../../ascent-world/StdAfx.h.gch ./StdAfx.h.gch

cd ../SpellHandlers
rm -f StdAfx.h.gch
ln -s ../../../ascent-world/StdAfx.h.gch ./StdAfx.h.gch

cd ../ServerStatusPlugin
rm -f StdAfx.h.gch
ln -s ../../../ascent-world/StdAfx.h.gch ./StdAfx.h.gch

echo Done. Now you can run make, and experience much faster build times. Remember to re-run this script every time you svn update.


