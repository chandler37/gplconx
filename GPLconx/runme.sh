#! /bin/sh

# helps bootstrapping from Makefile.am and configure.in
# when this directory is checked out from CVS by the maintainer.
# You'll need [I used this version] autoconf [2.13] automake [1.4]
# libtoolize [1.3.4]

test -f ltmain.sh || libtoolize -c -f
# I put libtool.m4 in acinclude.m4 as libtool's textinfo documentation
# suggested
aclocal -I .
autoheader
automake -a -c
autoconf
echo "Now run \"/bin/sh ./configure\""

exit 0
