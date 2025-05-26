#! /usr/bin/env bash
$XGETTEXT `find . -not -path "./daemon/*" -name \*.cpp` -o $podir/knighttime.pot
