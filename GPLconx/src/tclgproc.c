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
  Source for Tcl functions that both tconx and cxxconx need.
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <tcl.h>
#include <tk.h>
#include <Togl/togl.h>

#include "viewer.h"
#include "point.h"
#include "util.h"
#include "conxtcl.h"
#include "tclprocs.h"

#define UNKNOWN_MENU_CHOICE "Unknown conx menu choice."

ConxModlType tconx_togl_id2model(struct Togl *togl)
/* Each Togl widget has an id, specified by the Tcl `-ident' option, that
   we use to identify which model we are visualizing.
*/
{
  const char *ident = Togl_Ident(togl);
  if (!strcmp(ident, "puhp")) {
    return CONX_POINCARE_UHP;
  } else if (!strcmp(ident, "pd")) {
    return CONX_POINCARE_DISK;
  } else {
    assert(!strcmp(ident, "kd"));
    return CONX_KLEIN_DISK;
  }
}
