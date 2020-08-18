#!/bin/sh

export WANT_AUTOCONF_2_5="1"
export WANT_AUTOMAKE_1_7="1"

libtoolize --copy
intltoolize --copy --force
aclocal -I m4
autoconf
automake --add-missing --copy
