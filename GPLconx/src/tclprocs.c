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
  Source for Tcl functions that allow input and output with a Togl widget.
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

int tconx_tcl_data_entry(struct Togl *togl, int argc, char *argv[])
/* `.modelpd.togl_wig data_entry GETF1 .3 .4', e.g., to enter focus 1 as
   (.3, .4).
   
   Data entry also does what `.modelpd.togl_wig viz' does for those
   commands that require no data entry.
*/
{
  Tcl_Interp *interp = Togl_Interp(togl);
  double rinput;
  ConxMenuChoice mc;
  ConxModlType modl = tconx_togl_id2model(togl);
  int mf, i, num_inputs;
  int height = Togl_Height(togl);
  char help_message[] = "Illegal usage.  Try `.modelpd.togl_wig "
    "data_entry GETF1 "
    ".3 .4' to enter focus 1 as (.3, .4)";
  
  argc -= 2; argv += 2;

  if (argc < 1) TCONX_TCL_BAD_ARGS(interp);
  mc = conx_string2menu_choice(argv[0]);

  argc -= 1; argv += 1;

  if (mc == CONXCMD_CONX_NOT_A_CHOICE)
    TCONX_TCL_ERROR(UNKNOWN_MENU_CHOICE);
  num_inputs = conx_num_inputs(mc);
  if (argc != num_inputs) {
    if (argc < num_inputs)
      TCONX_TCL_ERROR("Needed more inputs.");
    else
      TCONX_TCL_ERROR("Needed fewer inputs.");
  }
  for (i = 0; i < num_inputs; i++) {
    if (conx_str2d(argv[i], &rinput))
      TCONX_TCL_ERROR("Inputs are real numbers.");
    conx_add_datum(rinput, modl);
  }
  mf = conx_menufunc(mc, modl, height);
  if (mf & CONXMF_DISPLAY_ALL) {
    TCONX_TCL_OK("all");
  }
  TCONX_TCL_OK("this one");
}

int tconx_tcl_mouse(struct Togl *togl, int argc, char *argv[])
/* `.modelpd.togl_wig mouse up 40 55 MGETFS', e.g., to signify a mouse button
   release at (40,55), with the semantics for MGETFS in place,
   in the poincare disk model.
*/
{
  Tcl_Interp *interp = Togl_Interp(togl);
  unsigned long x, y;
  ConxMenuChoice mc;
  ConxModlType mdl = tconx_togl_id2model(togl);
  ConxMouseChoice cmc = CONXMOUSE_DOWN;
  int height = Togl_Height(togl);
  int case_sens = 1;
  char help_message[] = "Illegal usage.  Try `.modelpd.togl_wig "
    "mouse up 40 55 "
    "MGETFS', e.g., to signify a mouse button release at (40,55), with the "
    "semantics for MGETFS in place, in the poincare disk model.";


  argc -= 2; argv += 2;

  if (argc != 4) TCONX_TCL_BAD_ARGS(interp);
  if (conx_prefix_streq("up", argv[0], 1, case_sens)) {
    cmc = CONXMOUSE_UP;
  }
#ifndef NDEBUG
  else {
    if (!conx_prefix_streq("down", argv[0], 1, case_sens))
      TCONX_TCL_BAD_ARGS(interp);
  }
#endif

  if (conx_str2ul(argv[1], &x) || conx_str2ul(argv[2], &y))
    TCONX_TCL_BAD_ARGS(interp);
  if ((mc = conx_string2menu_choice(argv[3])) == CONXCMD_CONX_NOT_A_CHOICE)
    TCONX_TCL_BAD_ARGS(interp);

  if (conx_mouse(mdl, cmc, x, y, height, mc)) {
    TCONX_TCL_OK("do a redisplay");
  } else {
    TCONX_TCL_OK("do not redisplay");
  }
}

int tconx_tcl_query(struct Togl *togl, int argc, char *argv[])
/* `.modelpd.togl_wig query CONXCMD_SHOWCD', e.g., to get the current
   conic parameter (the radius of a circle, e.g.).
   Returns the desired value(s) as a list.
*/
{
  double params[4]; /* SHOWXXYY */
  Tcl_Interp *interp = Togl_Interp(togl);
  ConxMouseChoice mc;
  size_t nr, j;
  char help_message[] = "Illegal usage.  Try `.modelpd.togl_wig query "
    "CONXCMD_SHOWCD' to get the current radius or ellipse/hyperbola "
    "parameter, e.g.";

  argc -= 2; argv += 2;

  if (argc != 1) TCONX_TCL_BAD_ARGS(interp);

  mc = conx_string2menu_choice(argv[0]);

  if (mc == CONXCMD_CONX_NOT_A_CHOICE)
    TCONX_TCL_ERROR(UNKNOWN_MENU_CHOICE);
  nr = conx_query(tconx_togl_id2model(togl), mc, params, 4);
  if (nr == 0) {
    TCONX_TCL_ERROR(UNKNOWN_MENU_CHOICE);
  }
  for (j = 0; j < nr; j++) {
    char s[100];
    (void) sprintf(s, "%.18e", params[j]);
    Tcl_AppendElement(interp, s);
  }
  return TCL_OK;
}
