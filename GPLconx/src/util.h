/*
    GPLconx -- visualize 2-D hyperbolic geometry.
    Copyright (C) 1996-2001  David L. Chandler

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Utility functions that have nothing to do with either visualization
   or hyperbolic geometry.

   Uses global variables.
*/

#ifndef CONXV_UTIL_H
#define CONXV_UTIL_H 1

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"

#define PROMPT(p) (void) fprintf(stdout, "%s", p); fflush(stdout)

#define LOGSTREAM stderr

/* #define DOF "%g" */
#define DOF "%#.18g"

enum {
  LOGG_TEXINFO = 25,
  LOGG_QUICK = 50,
  LOGG_FULL = 100,
  LOGG_CXX_LOC = 100,
  LOGG_CXX_LOC1 = 1000,
  LOGG_BRES2 = 800
};

#define F_LOGGG0(ind, string) \
    if ((ind)) { \
         (void) fprintf(LOGSTREAM, string); \
         (void) fflush(LOGSTREAM); \
    }

#define F_LOGGG1(ind, string, arg1) \
    if ((ind)) { \
         (void) fprintf(LOGSTREAM, string, arg1); \
         (void) fflush(LOGSTREAM); \
    }

#define F_LOGGG2(ind, string, arg1, arg2) \
    if ((ind)) { \
         (void) fprintf(LOGSTREAM, string, arg1, arg2); \
         (void) fflush(LOGSTREAM); \
    }

#define F_LOGGG3(ind, string, arg1, arg2, arg3) \
    if ((ind)) { \
         (void) fprintf(LOGSTREAM, string, arg1, arg2, arg3); \
         (void) fflush(LOGSTREAM); \
    }

#define F_LOGGG4(ind, string, arg1, arg2, arg3, arg4) \
    if ((ind)) { \
         (void) fprintf(LOGSTREAM, string, arg1, arg2, arg3, arg4); \
         (void) fflush(LOGSTREAM); \
    }

#define F_LOGGG5(ind, string, arg1, arg2, arg3, arg4, arg5) \
    if ((ind)) { \
         (void) fprintf(LOGSTREAM, string, arg1, arg2, arg3, arg4, arg5); \
         (void) fflush(LOGSTREAM); \
    }

#define F_LOGGG6(ind, string, arg1, arg2, arg3, arg4, arg5, arg6) \
    if ((ind)) { \
         (void) fprintf(LOGSTREAM, string, arg1, arg2, arg3, \
                        arg4, arg5, arg6); \
         (void) fflush(LOGSTREAM); \
    }

#define F_LOGGG7(ind, string, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    if ((ind)) { \
         (void) fprintf(LOGSTREAM, string, arg1, arg2, arg3, \
                        arg4, arg5, arg6, arg7); \
         (void) fflush(LOGSTREAM); \
    }

#define LOGGG0(Lvl, string) \
    F_LOGGG0(loglevel > Lvl, string)
#define LOGGG1(Lvl, string, arg1) \
    F_LOGGG1(loglevel > Lvl, string, arg1)
#define LOGGG2(Lvl, string, arg1, arg2) \
    F_LOGGG2(loglevel > Lvl, string, arg1, arg2)
#define LOGGG3(Lvl, string, arg1, arg2, arg3) \
    F_LOGGG3(loglevel > Lvl, string, arg1, arg2, arg3)
#define LOGGG4(Lvl, string, arg1, arg2, arg3, arg4) \
    F_LOGGG4(loglevel > Lvl, string, arg1, arg2, arg3, arg4)
#define LOGGG5(Lvl, string, arg1, arg2, arg3, arg4, arg5) \
    F_LOGGG5(loglevel > Lvl, string, arg1, arg2, arg3, arg4, arg5)
#define LOGGG6(Lvl, string, arg1, arg2, arg3, arg4, arg5, arg6) \
    F_LOGGG6(loglevel > Lvl, string, arg1, arg2, arg3, arg4, arg5, arg6)
#define LOGGG7(Lvl, string, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    F_LOGGG7(loglevel > Lvl, string, arg1, arg2, arg3, arg4, arg5, arg6, arg7)

/* We don't have const_cast<>, but this will do: */
#define CONST_CAST(mutable_type, mutable_ptr, const_ptr) \
    { \
      void *ptrptr = &(const_ptr); \
      mutable_ptr = (mutable_type) (*((void **)ptrptr)); \
    }

#define CHECK_OOM(a, b) \
   if (a == NULL) { F_LOGGG1(1, "Conx: Out of memory at `%s'", b); exit(79); }

int conx_str2ul(const char *, unsigned long *);
int conx_str2l(const char *, long *);
int conx_str2d(const char *, double *);
int conx_strcmp(const char *a, const char *b, int case_sensitive);

#define streq(a, b, cs) (conx_strcmp(a, b, cs) == 0)

int conx_prefix_streq(const char *whole, const char *maybe, size_t n,
                      int case_sensitive);

#define FATAL(st) \
    { (void) fprintf(stderr, "\n\n\nConx: Fatal Error `%s'\n", st); abort(); }

int conx_file_exists(const char *fn);

#endif /* CONXV_UTIL_H */
