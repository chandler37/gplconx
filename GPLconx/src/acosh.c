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
   arccosh implementation for systems whose libm is not up to par.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include "viewer.h"

double acosh(double x)
/* arc-hyperbolic-cosine, i.e., cosh(acosh(.3)) == .3 after accounting
   for machine arithmetic.
*/
{
  return log(x+sqrt(myabs((x-1.0)*(x+1.0))));
  /* DLC why not sqr(x)-1.0; compare the numerical properties. */
}

