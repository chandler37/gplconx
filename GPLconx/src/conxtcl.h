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

/*
  These macros make our Tcl procedures a bit cleaner and more uniform.
*/

#ifndef CONX_TCL_MACROS_H
#define CONX_TCL_MACROS_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <tcl.h>

#include "util.h"

/* do-while loops are necessary for pretty conditionals. */
#define TCONX_TCL_ERROR(compileTimeStringConstant) \
      do { \
        char mutable_string[] = compileTimeStringConstant; \
        Tcl_SetResult(interp, mutable_string, TCL_VOLATILE); \
        return TCL_ERROR; \
      } while (0)

#define TCONX_TCL_OK_ARRAY(mutable_string) \
      do { \
        Tcl_SetResult(interp, mutable_string, TCL_VOLATILE); \
        return TCL_OK; \
      } while (0)

#define TCONX_TCL_OK(compileTimeStringConstant) \
      do { \
        char mutable_string[] = compileTimeStringConstant; \
        TCONX_TCL_OK_ARRAY(mutable_string); \
      } while (0)

/* This macro assumes there is a
     `char usage[] = "Here is how to use this function";' as well
   as assuming that the Tcl interpreter is interp.
*/
#define TCONX_TCL_BAD_ARGS(interp) \
      do { \
        Tcl_SetResult(interp, help_message, TCL_VOLATILE); \
        return TCL_ERROR; \
      } while (0)

#ifdef __cplusplus
}
#endif

#endif /* CONX_TCL_MACROS_H */
