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
   Functions to draw three models of hyperbolic geometry,
   plus functions that do various mathematical tricks, that do NOT require
   the static, file-level global variables in `conxv.c'.  This file is
   ``clean''.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "viewer.h"
#include "util.h"
#include "point.h"

struct conxenum {
  ConxMenuChoice mc;
  const char *id;
};

static const struct conxenum enms[] = {
  { CONXCMD_ALTLINE, "CONXCMD_ALTLINE" },
  { CONXCMD_LINEF, "CONXCMD_LINEF" },
  { CONXCMD_FOCI, "CONXCMD_FOCI" },
  { CONXCMD_FOCUS, "CONXCMD_FOCUS" },
  { CONXCMD_SEGMENT, "CONXCMD_SEGMENT" },
  { CONXCMD_ELLIPSE, "CONXCMD_ELLIPSE" },
  { CONXCMD_CIRCLE, "CONXCMD_CIRCLE" },
  { CONXCMD_PARABOLA, "CONXCMD_PARABOLA" },
  { CONXCMD_BELLIPSE, "CONXCMD_BELLIPSE" },
  { CONXCMD_BCIRCLE, "CONXCMD_BCIRCLE" },
  { CONXCMD_BPARABOLA, "CONXCMD_BPARABOLA" },
  { CONXCMD_EQDIST, "CONXCMD_EQDIST" },
  { CONXCMD_BEQDIST, "CONXCMD_BEQDIST" },
  { CONXCMD_SHOWTOL, "CONXCMD_SHOWTOL" },
  { CONXCMD_SHOWCOMPTOL, "CONXCMD_SHOWCOMPTOL" },
  { CONXCMD_SHOWA, "CONXCMD_SHOWA" },
  { CONXCMD_SHOWR, "CONXCMD_SHOWR" },
  { CONXCMD_SHOWCD, "CONXCMD_SHOWCD" },
  { CONXCMD_SHOWLOGLEVEL, "CONXCMD_SHOWLOGLEVEL" },
  { CONXCMD_SHOWF1, "CONXCMD_SHOWF1" },
  { CONXCMD_SHOWF2, "CONXCMD_SHOWF2" },
  { CONXCMD_SHOWTSTEP, "CONXCMD_SHOWTSTEP" },
  { CONXCMD_SHOWXXYY, "CONXCMD_SHOWXXYY" },
  { CONXCMD_GETTOL, "CONXCMD_GETTOL" },
  { CONXCMD_GETCOMPTOL, "CONXCMD_GETCOMPTOL" },
  { CONXCMD_GETA, "CONXCMD_GETA" },
  { CONXCMD_GETR, "CONXCMD_GETR" },
  { CONXCMD_GETCD, "CONXCMD_GETCD" },
  { CONXCMD_GETLOGLEVEL, "CONXCMD_GETLOGLEVEL" },
  { CONXCMD_GETF1, "CONXCMD_GETF1" },
  { CONXCMD_GETF2, "CONXCMD_GETF2" },
  { CONXCMD_GETTSTEP, "CONXCMD_GETTSTEP" },
  { CONXCMD_GETXXYY, "CONXCMD_GETXXYY" },
  { CONXCMD_CLEAR, "CONXCMD_CLEAR" },
  { CONXCMD_BORDERSON, "CONXCMD_BORDERSON" },
  { CONXCMD_GARNISHON, "CONXCMD_GARNISHON" },
  { CONXCMD_AUTOFOCION, "CONXCMD_AUTOFOCION" },
  { CONXCMD_EXIT, "CONXCMD_EXIT" },
  { CONXCMD_OUTPUT, "CONXCMD_OUTPUT" },
  { CONXCMD_RESETTHISONE, "CONXCMD_RESETTHISONE" },
  { CONXCMD_MGETFS, "CONXCMD_MGETFS" },
  { CONXCMD_MGETF1, "CONXCMD_MGETF1" },
  { CONXCMD_MGETF2, "CONXCMD_MGETF2" },
  { CONXCMD_MZOOM, "CONXCMD_MZOOM" },
  { CONXCMD_MGETCD, "CONXCMD_MGETCD" },
  { CONXCMD_MGETALT, "CONXCMD_MGETALT" },
  { CONXCMD_CONX_NOT_A_CHOICE_3, NULL }
};

ConxMenuChoice conx_string2menu_choice(const char *s)
/* Returns the enumeration value that is named case-sensitively in s.
   Returns CONXCMD_CONX_NOT_A_CHOICE if s is not a known enumeration.

   Note that "CONXCMD_CONX_NOT_A_CHOICE", "CONXCMD_CONX_NOT_A_CHOICE_2", and
   "CONXCMD_CONX_NOT_A_CHOICE_3" intentionally do not map to anything.
*/
{
  int case_sensitive = 1;
  int i = 0;
  const struct conxenum *tmp;

  do {
    tmp = &enms[i++];
    if (tmp->id == NULL) {
      /* This string is not in the above array. */
      return CONXCMD_CONX_NOT_A_CHOICE;
    }
    if (streq(tmp->id, s, case_sensitive)) return tmp->mc;
  } while (1);
}

const char *conx_menu_choice2string(ConxMenuChoice mc)
/* Returns the string that is the stringified mc.  Returns UNKNOWN_STR if mc
   is not a known enumeration.

   Note that CONXCMD_CONX_NOT_A_CHOICE, CONXCMD_CONX_NOT_A_CHOICE_2, and
   CONXCMD_CONX_NOT_A_CHOICE_3 intentionally map to UNKNOWN_STR
*/
{
  int i = 0;
  const struct conxenum *tmp;

  do {
    tmp = &enms[i++];
    if (tmp->id == NULL) {
      /* This enumeration is not in the above array. */
      return UNKNOWN_STR;
    }
    if (tmp->mc == mc) return tmp->id;
  } while (1);
}

int conx_num_inputs(ConxMenuChoice a)
/*
@conxdox
   Returns the number of inputs (all real numbers) that the menu item
   @var{a} requires.
@end conxdox
*/
{
  switch (a) {
  case CONXCMD_GETCD:
  case CONXCMD_GETTOL:
  case CONXCMD_GETTSTEP:
  case CONXCMD_GETCOMPTOL:
  case CONXCMD_GETA:
  case CONXCMD_GETR:
  case CONXCMD_GETLOGLEVEL:
    return 1;
  case CONXCMD_GETF1:
  case CONXCMD_GETF2:
    return 2;
  case CONXCMD_GETXXYY:
    return 4;
  default: return 0;
  }
}

const char *conx_modelenum2string(ConxModlType m)
{
  switch (m) {
  case CONX_POINCARE_DISK: return "CONX_POINCARE_DISK";
  case CONX_POINCARE_UHP: return "CONX_POINCARE_UHP";
  case CONX_KLEIN_DISK: return "CONX_KLEIN_DISK";
  default: return UNKNOWN_STR;
  }
}

const char *conx_modelenum2short_string(ConxModlType m)
{
  switch (m) {
  case CONX_POINCARE_DISK: return "pd";
  case CONX_POINCARE_UHP: return "puhp";
  case CONX_KLEIN_DISK: return "kd";
  default: return UNKNOWN_STR;
  }
}

