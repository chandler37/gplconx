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
  Implementation of C++ classes in `h_geomob.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hypmath.hh"
#include "h_geomob.hh"
#include "cassert.h"
#include "canvas.hh"

CF_INLINE
CConxGeomObj::CConxGeomObj()
{
  MMM("CConxGeomObj()");
  scalar = 0.0;
}

CF_INLINE
CConxGeomObj::CConxGeomObj(const CConxGeomObj &a)
  : CConxTwoPts(a)
{
  MMM("CConxGeomObj(const CConxGeomObj &a)");
  L = a.getLine();
  scalar = a.getScalar();
}

NF_INLINE
CConxGeomObj &CConxGeomObj::operator=(const CConxGeomObj &o)
{
  MMM("CConxGeomObj &operator=(const CConxGeomObj &o)");
  (void) CConxTwoPts::operator=(o);
  setLine(o.getLine());
  setScalar(o.getScalar());
  return *this;
}

NF_INLINE
int CConxGeomObj::operator==(const CConxGeomObj &o) const
{
  return (CConxTwoPts::operator==(o)
          && getLine() == o.getLine()
          && myequals(getScalar(), o.getScalar(), EQUALITY_TOL));
}

