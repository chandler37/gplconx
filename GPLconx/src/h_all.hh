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
  C++ classes to model elements of computational hyperbolic geometry such
  as points, lines, and conic sections.

  Many classes here extend CConxGeomObj, which contains a superset of
  the data required to represent any single subclass.  While multiple
  inheritance might seem more elegant here, multiple inheritance is born
  ugly.  This code may be ported to Java one day, so multiple inheritance
  of C++ classes that would be Java interfaces is permissible, but any other
  use is not allowed.  To avoid showing the user the superflous parts of
  CConxGeomObj, it is inherited with the `protected' access model and the
  useful operations are then granted public access.

  Most computed values are cached until a dependency changes.  For example,
  a point on a parabola found via CConxParabola::getPointOn is remembered
  until the parabola's focus or line is altered.

  Default values for floating-point attributes must be exactly representable
  in IEEE single-precision, floating-point numbers; `1.0', e.g., is okay.
  This is so that operator==() will work correctly with EQUALITY_TOL==`0.0'.

  See `README.DEV' for more on the above topics.
*/

#ifndef GPLCONX_H_ALL_CXX_H
#define GPLCONX_H_ALL_CXX_H 1

#include <iostream.h>

#include "point.hh"
#include "viewer.h"

#include "h_simple.hh"
#include "h_point.hh"
#include "h_twopts.hh"
#include "h_line.hh"
#include "h_geomob.hh"
#include "h_parabo.hh"
#include "h_hypell.hh"
#include "h_circle.hh"
#include "h_eqdist.hh"

// DLC Triangles, tangents, tractrix, perpendiculars, N-gons, midpoints,
// a distinction between points and ideal points, a grid for the canvas, ...



// DLC TODO Include polygons as geometric objects.

//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////

#endif // GPLCONX_H_ALL_CXX_H
