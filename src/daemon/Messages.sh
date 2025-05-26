#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.kcfg` >> rc.cpp || exit 11
$XGETTEXT `find . -name \*.cpp` -o $podir/knighttimed.pot
rm -f rc.cpp
