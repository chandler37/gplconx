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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include "viewer.h"
#include "point.h"

void conx_longway(ConxMetric *test, void *testArg, ConxModlType modl,
                  double tlrance, double delta_x, double delta_y,
                  double x_min, double x_max, double y_min, double y_max,
                  ConxPointFunc *pfunc, void *pArg)
/* This allows you to find those points in the viz area that come within
   tlrance of being zeroes of the function *test.  When one is found,
   (*pfunc)(x,y,pArg) is called.
*/
{
  Pt X;

  if (modl==CONX_POINCARE_UHP) {
    for (X.x = x_min; X.x <= x_max; X.x += delta_x) {
      for (X.y = y_min; X.y <= y_max; X.y += delta_y) {
        if (myabs((*test)(X, testArg)) < tlrance)
          (*pfunc)(X.x, X.y, pArg);
      }
    }
  } else {
    /* test against xmin and xmax, right, or at most the unit circle,
       for a speedup and reduce of redundancy? DLC */
    for (X.x = -1.0; X.x <= 1.0; X.x += delta_x) {
      for (X.y = -1.0*sqrt(1.0-sqr(X.x));
           /* X.y <= sqrt(1.0-sqr(X.x)) is the same as but slower than: */
           ((X.y <= 0) || (sqr(X.y) <= 1.0-sqr(X.x)));
           X.y += delta_y) {
	if (myabs((*test)(X, testArg)) < tlrance)
	  (*pfunc)(X.x, X.y, pArg);
      }
    }
  }
}

