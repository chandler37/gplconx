# GPLconx -- visualize 2-D hyperbolic geometry.
# Copyright (C) 1996-2001  David L. Chandler

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

## Process this file with automake to produce Makefile.in

## See the README in this directory

AUTOMAKE_OPTIONS = 1.4 foreign

EXTRA_DIST = AUTHORS README

AM_CFLAGS = $(FIRM_CFLAGS)
AM_CXXFLAGS = $(FIRM_CFLAGS)
AM_CPPFLAGS = -I$(srcdir)

noinst_LIBRARIES = libcxlcl.a
libcxlcl_a_SOURCES = lbaseobject.cc lforeach.cc lobject.cc lsimplelist.cc

noinst_HEADERS = lbaseobject.h lcl.h ldebug.h ldict.h lforeach.h lobject.h \
                 lsimplelist.h
