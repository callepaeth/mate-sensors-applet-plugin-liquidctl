#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

export CPPFLAGS=-I$srcdir

PKG_NAME="mate-sensors-applet-plugin-liquidctl"

(test -f $srcdir/configure.ac) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level $PKG_NAME directory"
    exit 1
}

which mate-autogen || {
    echo "You need to install mate-common"
    exit 1
}

which yelp-build || {
    echo "You need to install yelp-tools"
    exit 1
}

REQUIRED_AUTOMAKE_VERSION=1.9
USE_COMMON_DOC_BUILD=yes

. mate-autogen
