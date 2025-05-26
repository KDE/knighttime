#! /usr/bin/env bash
#
# SPDX-FileCopyrightText: None
# SPDX-License-Identifier: CC0-1.0

$XGETTEXT `find . -not -path "./daemon/*" -name \*.cpp` -o $podir/knighttime.pot
