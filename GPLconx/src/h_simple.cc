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
  Implementation of C++ classes in `h_simple.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hypmath.hh"
#include "h_simple.hh"
#include "canvas.hh"

CF_INLINE
CConxSimpleArtist::~CConxSimpleArtist()
{
  LLL("CConxSimpleArtist interface implementor destructor activated.  If this were not virtual, there would be a crash.");
}

NF_INLINE
const char *CConxSimpleArtist::humanSAType(SAType s)
{
  switch (s) {
  case SA_POINT: return "Point";
  case SA_LINE: return "Line";
  case SA_CIRCLE: return "Circle";
  case SA_PARABOLA: return "Parabola";
  case SA_HYPELLIPSE: return "HypEllipse";
  case SA_EQDISTCURVE: return "EqDistCurve";
  default: throw "how?";
  }
}

