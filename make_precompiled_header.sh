#!/bin/sh

echo Building Precompiled Header...
cd src/game
g++ -DHAVE_CONFIG_H -I. -I../.. -I/usr/include/mysql -I/usr/local/include/mysql -I/usr/include/openssl -I/usr/local/include/openssl -I. -I./../../dep/include -I./../shared -I./../../dep/src/gm -I./../logonserver  -I/usr/include/mysql -I/usr/local/include/mysql -I/usr/include/openssl -I/usr/local/include/openssl -c -o StdAfx.h.gch StdAfx.h


echo Symlinking script gch files.
cd ../scripts/src/GossipScripts
rm -f StdAfx.h.gch
ln -s ../../../game/StdAfx.h.gch ./StdAfx.h.gch

cd ../InstanceScripts
rm -f StdAfx.h.gch
ln -s ../../../game/StdAfx.h.gch ./StdAfx.h.gch

cd ../SpellHandlers
rm -f StdAfx.h.gch
ln -s ../../../game/StdAfx.h.gch ./StdAfx.h.gch

cd ../ServerStatusPlugin
rm -f StdAfx.h.gch
ln -s ../../../game/StdAfx.h.gch ./StdAfx.h.gch

cd ../IRCPlugin
rm -f StdAfx.h.gch
ln -s ../../../game/StdAfx.h.gch ./StdAfx.h.gch

echo Done. Now you can run make, and experience much faster build times. Remember to re-run this script every time you svn update.


