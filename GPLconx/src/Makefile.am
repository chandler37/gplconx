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

AUTOMAKE_OPTIONS = 1.4 foreign

## We either let configure find the libraries or we have to include this
## to make installation bearable:
include Makefile.inc

# Tell bison/yacc to make a header file:
AM_YFLAGS = -d
## For a C++ lexer, name scanner.l scanner.ll and use AM_LFLAGS = -+ -olex.yy.c

SUBDIRS = Tcl lcl

AM_CFLAGS = $(FIRM_CFLAGS)
AM_CXXFLAGS = $(FIRM_CFLAGS)

bin_PROGRAMS = @GCONX@ @TCONX@ cxxconx
EXTRA_PROGRAMS = gconx tconx
noinst_PROGRAMS = tgeomobj tdgeomob tCString tderive tparser
noinst_LTLIBRARIES = @LIBCONXLA@ libconxu.la libcxxconx.la libcls.la
EXTRA_LTLIBRARIES = libconx.la

ttalk-sh: tparser

if WE_HAVE_SYS_INTERP
TESTS = tgeomobj tdgeomob tCString tderive tparser ttalk-sh
else
TESTS = tgeomobj tdgeomob tCString tderive tparser
check-local:
	srcdir=$(srcdir); export srcdir; \
	top_builddir=$(top_builddir); export top_builddir; \
	$(SHELL) $(srcdir)/ttalk-sh || exit 1
endif
EXTRA_TESTS = ttalk-sh

EXTRA_DIST = tconxopt.ggo getopt.h parser.h ttalk-in ttalk-o ttalk-sh \
             Makefile.inc README

BUILT_SOURCES = tconxopt.c tconxopt.h

lcl_CPPFLAGS = -I$(srcdir)/lcl
lcl_LDFLAGS =
lcl_LIBS = $(top_builddir)/src/lcl/libcxlcl.a

## We have to include togl.h, and it is located relative to top_srcdir.
AM_CPPFLAGS = -I$(top_srcdir) $(lcl_CPPFLAGS) $(glut_CPPFLAGS) $(glu_CPPFLAGS) $(gl_CPPFLAGS) $(tk_CPPFLAGS) $(tcl_CPPFLAGS)

## Conceptually, EXTRA_libconxu_la_SOURCES = acosh.c, but it is
## discovered by automake.
## libconxu must be linked with -lm
libconxu_la_SOURCES = conxcln.c bres2.c \
                     longwaysv.c hypmath.c util.c
libconxu_la_LIBADD = @LTLIBOBJS@

## libconx must be linked with gl.c -lGLU -lGL
libconx_la_SOURCES = conxv.c lines.c

libcls_la_SOURCES = parser.y scanner.l cparse.cc clsmgr.cc mparse.cc \
                    sth_var.cc sth_answ.cc sth_msg.cc \
                    sth_kmsg.cc sth_karg.cc sth_assn.cc stmparse.cc \
		    sterror.cc stcharar.cc ststring.cc stsymbol.cc \
		    stobject.cc stsmalli.cc stnumber.cc gcobject.cc \
                    stboole.cc stundefo.cc stgarray.cc stfloat.cc \
                    stsystem.cc stmodlid.cc stcolor.cc stdrawbl.cc \
		    stpoint.cc stline.cc stcircle.cc stparabo.cc stcanvas.cc \
		    sthypell.cc

tparser_SOURCES = tparser.cc tester.cc
tparser_LDADD = libcls.la libcxxconx.la libconxu.la \
                $(lcl_LDFLAGS) $(lcl_LIBS)

EXTRA_tconx_SOURCES = getopt1.c getopt.c
tconx_SOURCES = toglconx.c tclprocs.c tclgproc.c tconxopt.c gl.c
tconx_LDADD = @GGOOBJS@ libconx.la libconxu.la \
              $(top_builddir)/Togl/libtogl.la \
              $(glu_LDFLAGS) $(glu_LIBS) \
              $(gl_LDFLAGS) $(gl_LIBS) \
              $(tk_LDFLAGS) $(tk_LIBS) \
              $(tcl_LDFLAGS) $(tcl_LIBS)
tconx_DEPENDENCIES = @GGOOBJS@ libconx.la libconxu.la \
                     $(top_builddir)/Togl/libtogl.la

libcxxconx_la_SOURCES = geomobj.cc hypmathx.cc CString.cc canvas.cc \
                        color.cc printon.cc dgeomobj.cc \
                        CObject.cc CArray.cc
## libcxxconx.la needs to be linked with libconxu.la

EXTRA_cxxconx_SOURCES = getopt1.c getopt.c
cxxconx_SOURCES = toglobj.cc cxxconx.cc tclgproc.c tconxopt.c glcanvas.cc
cxxconx_LDADD = @GGOOBJS@ libcls.la libcxxconx.la libconxu.la \
                $(top_builddir)/Togl/libtogl.la $(glu_LDFLAGS) $(glu_LIBS) \
                $(gl_LDFLAGS) $(gl_LIBS) $(tk_LDFLAGS) $(tk_LIBS) \
                $(tcl_LDFLAGS) $(tcl_LIBS) $(lcl_LDFLAGS) $(lcl_LIBS)

cxxconx_DEPENDENCIES = @GGOOBJS@ libconx.la libconxu.la \
                       $(top_builddir)/Togl/libtogl.la

tderive_SOURCES = tderive.cc tester.cc
tderive_LDADD =

tgeomobj_SOURCES = tgeomobj.cc tester.cc
tgeomobj_LDADD = libcxxconx.la libconxu.la

tdgeomob_SOURCES = tdgeomob.cc tester.cc
tdgeomob_LDADD = libcxxconx.la libconxu.la

tCString_SOURCES = tCString.cc tester.cc
tCString_LDADD = libcxxconx.la libconxu.la

glut_LDFLAGS = @GLUTLIBDIR@
glut_CPPFLAGS = @GLUTINCDIR@

gconx_SOURCES = glutconx.c gl.c
gconx_LDADD = libconx.la libconxu.la $(glut_LDFLAGS) $(glut_LIBS) \
                $(glu_LDFLAGS) $(glu_LIBS) $(gl_LDFLAGS) $(gl_LIBS)

# Dependency so that we can do a `./configure; cd src; make tconx cxxconx':
$(top_srcdir)/Togl/libtogl.la:
	cd ../Togl && $(MAKE) libtogl.la

tconxopt.c tconxopt.h: $(srcdir)/tconxopt.ggo
	$(GENGETOPT) --input=$(srcdir)/tconxopt.ggo --file-name=tconxopt \
             --unamed-opts
	if test "." != "$(srcdir)"; then \
		rm -f $(srcdir)/tconxopt.c $(srcdir)/tconxopt.h; \
		mv tconxopt.c tconxopt.h $(srcdir)/; \
	fi


noinst_HEADERS = viewer.h point.h globals.h util.h conxtcl.h bresint.h \
                 tclprocs.h tconxopt.h gl.h CString.hh toglobj.hh \
                 geomobj.hh hypmath.hh printon.hh cassert.h decls.hh \
                 canvas.hh color.hh glcanvas.hh dgeomobj.hh cparse.hh \
                 parsearg.h clsmgr.hh CObject.hh tester.hh Starray.hh \
                 CArray.hh mparse.hh stcommon.hh stconx.hh sthelper.hh \
	         sth_var.hh sth_answ.hh sth_msg.hh sth_kmsg.hh \
	         sth_assn.hh sth_karg.hh \
                 cmparse.hh stmparse.hh stmacros.hh sterror.hh stobject.hh \
                 stcharar.hh ststring.hh stsymbol.hh gcobject.hh stsmalli.hh \
                 stnumber.hh stboole.hh stundefo.hh stgarray.hh stfloat.hh \
                 stsystem.hh stmodlid.hh stcolor.hh stdrawbl.hh stpoint.hh \
                 stline.hh stcircle.hh stparabo.hh stcanvas.hh sthypell.hh \
		 point.hh


# How many lines of source code do we have?
# (Let's count the lines from the *.ui files.  The amount of labor
#  involved in each is roughly proportional to the lines of code in them.)
# `make numln' will tell us.

NL_SOURCE_FILES = $(srcdir)/toglconx.c $(srcdir)/glutconx.c \
	$(srcdir)/tconxopt.ggo $(srcdir)/tclgproc.c \
	$(srcdir)/tclprocs.c $(srcdir)/tclprocs.h $(srcdir)/gl.c \
	$(srcdir)/acosh.c $(srcdir)/conxv.c $(srcdir)/conxcln.c \
	$(srcdir)/bres2.c $(srcdir)/lines.c $(srcdir)/tdgeomob.cc \
	$(srcdir)/cxxconx.cc $(srcdir)/toglobj.cc $(srcdir)/tgeomobj.cc \
	$(srcdir)/geomobj.cc $(srcdir)/hypmathx.cc $(srcdir)/toglobj.hh \
	$(srcdir)/geomobj.hh $(srcdir)/hypmath.hh $(srcdir)/printon.hh \
	$(srcdir)/CString.hh $(srcdir)/tCString.cc $(srcdir)/CString.cc \
        $(srcdir)/CArray.cc $(srcdir)/Starray.hh $(srcdir)/CArray.hh \
	$(srcdir)/longwaysv.c $(srcdir)/hypmath.c $(srcdir)/util.c \
	$(srcdir)/viewer.h $(srcdir)/point.h $(srcdir)/globals.h \
	$(srcdir)/util.h $(srcdir)/conxtcl.h $(srcdir)/bresint.h \
        $(srcdir)/cassert.h $(srcdir)/decls.hh $(srcdir)/canvas.cc \
	$(srcdir)/canvas.hh $(srcdir)/color.hh $(srcdir)/color.cc \
	$(srcdir)/glcanvas.hh $(srcdir)/glcanvas.cc $(srcdir)/printon.cc \
	$(srcdir)/dgeomobj.hh $(srcdir)/dgeomobj.cc $(srcdir)/tderive.cc \
	$(srcdir)/scanner.l $(srcdir)/parser.y $(srcdir)/tparser.cc \
	$(srcdir)/cparse.hh $(srcdir)/cparse.cc $(srcdir)/clsmgr.cc \
	$(srcdir)/clsmgr.hh $(srcdir)/parsearg.h $(srcdir)/CObject.hh \
	$(srcdir)/tester.hh $(srcdir)/tester.cc $(srcdir)/CObject.cc \
	$(srcdir)/mparse.hh $(srcdir)/mparse.cc \
	$(srcdir)/stmparse.hh $(srcdir)/stmparse.cc \
	$(srcdir)/stcommon.hh \
	$(srcdir)/sthelper.hh \
	$(srcdir)/sth_var.hh $(srcdir)/sth_var.cc \
	$(srcdir)/sth_answ.hh $(srcdir)/sth_answ.cc \
	$(srcdir)/sth_msg.hh $(srcdir)/sth_msg.cc \
	$(srcdir)/sth_kmsg.hh $(srcdir)/sth_kmsg.cc \
	$(srcdir)/sth_assn.hh $(srcdir)/sth_assn.cc \
	$(srcdir)/sth_karg.hh $(srcdir)/sth_karg.cc \
	$(srcdir)/stconx.hh $(srcdir)/stmacros.hh \
	$(srcdir)/cmparse.hh $(srcdir)/stobject.hh $(srcdir)/stobject.cc \
	$(srcdir)/sterror.hh $(srcdir)/sterror.cc \
	$(srcdir)/stcharar.hh $(srcdir)/stcharar.cc \
	$(srcdir)/ststring.hh $(srcdir)/ststring.cc \
	$(srcdir)/stsymbol.hh $(srcdir)/stsymbol.cc \
	$(srcdir)/stsmalli.hh $(srcdir)/stsmalli.cc \
	$(srcdir)/stnumber.hh $(srcdir)/stnumber.cc \
	$(srcdir)/stboole.hh $(srcdir)/stboole.cc \
	$(srcdir)/stundefo.hh $(srcdir)/stundefo.cc \
	$(srcdir)/stfloat.hh $(srcdir)/stfloat.cc \
	$(srcdir)/stsystem.hh $(srcdir)/stsystem.cc \
	$(srcdir)/stmodlid.hh $(srcdir)/stmodlid.cc \
	$(srcdir)/stcolor.hh $(srcdir)/stcolor.cc \
	$(srcdir)/stdrawbl.hh $(srcdir)/stdrawbl.cc \
	$(srcdir)/stpoint.hh $(srcdir)/stpoint.cc \
	$(srcdir)/stline.hh $(srcdir)/stline.cc \
	$(srcdir)/stcircle.hh $(srcdir)/stcircle.cc \
	$(srcdir)/stparabo.hh $(srcdir)/stparabo.cc \
	$(srcdir)/sthypell.hh $(srcdir)/sthypell.cc \
	$(srcdir)/stcanvas.hh $(srcdir)/stcanvas.cc \
	$(srcdir)/stgarray.hh $(srcdir)/stgarray.cc \
	$(srcdir)/gcobject.hh $(srcdir)/gcobject.cc \
	$(srcdir)/point.hh \
	$(srcdir)/Makefile.am $(srcdir)/Makefile.inc \
        $(top_srcdir)/Makefile.am $(top_srcdir)/configure.in \
	$(srcdir)/Tcl/tconx_a.tcl $(srcdir)/Tcl/tconx_b.tcl \
        $(srcdir)/Tcl/tconx.tcl $(srcdir)/Tcl/tconx_c.tcl \
	$(srcdir)/Tcl/tconx_d.tcl $(srcdir)/Tcl/tconx_l.tcl \
	$(srcdir)/Tcl/tconx_m.tcl $(srcdir)/Tcl/tconx_s.tcl \
	$(srcdir)/Tcl/tconx_w.tcl $(srcdir)/Tcl/tconx_h.tcl \
        $(srcdir)/Tcl/tconx_u.tcl \
	$(srcdir)/Tcl/tconx_p.tcl $(srcdir)/Tcl/text.ui \
        $(srcdir)/Tcl/tconx_t.tcl \
	$(srcdir)/Tcl/tconxt.ui $(srcdir)/Tcl/simple.ui \
        $(srcdir)/Tcl/slider.ui

numln:
	@MTMPA=`wc -l $(NL_SOURCE_FILES) | tail -1 | awk '{print $$1}'`; MTMPB=`echo $(NL_SOURCE_FILES) | wc -w`; echo There are `echo $$MTMPA $$MTMPB 19 \* - p | dc` lines of code not counting the 19-line GPL boilerplate in each source file and not counting documentation

# This does not count the GPL boilerplate in each file.


# For easy grepping, do `grep DLC `make s`'
s:
	@echo $(NL_SOURCE_FILES)

MAINTAINERCLEANFILES = y.output parser.c parser.h
CLEANFILES = gconx cxxconx tconx
