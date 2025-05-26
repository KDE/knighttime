#! /usr/bin/env bash
#
# SPDX-FileCopyrightText: None
# SPDX-License-Identifier: CC0-1.0

$EXTRACTRC `find . -name \*.kcfg` >> rc.cpp || exit 11
$XGETTEXT `find . -name \*.cpp` -o $podir/knighttimed.pot
rm -f rc.cpp
