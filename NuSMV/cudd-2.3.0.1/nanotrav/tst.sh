#! /bin/sh
#
# $Id: tst.sh,v 1.1 2001/10/29 13:40:07 none Exp $
#
./nanotrav -p 1 C17.blif > C17.tst
./nanotrav -p 1 -ordering dfs -autodyn -automethod sifting -reordering sifting -drop C880.blif > C880.tst
./nanotrav -p 1 -trav s27.blif > s27.tst
./nanotrav -p 1 -autodyn -reordering sifting -trav mult32a.blif > mult32a.tst
./nanotrav -p 1 -envelope rcn25.blif > rcn25.tst
