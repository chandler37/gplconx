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
  C++ HG circle
*/

#ifndef GPLCONX_H_CIRCLE_CXX_H
#define GPLCONX_H_CIRCLE_CXX_H 1

#include "h_geomob.hh"

//////////////////////////////////////////////////////////////////////////////
class CConxCircle : VIRT protected CConxGeomObj, public CConxSimpleArtist {
  CCONX_CLASSNAME("CConxCircle")
  SA_IMP(SA_CIRCLE, CConxCircle)
public:
#define MIN_RADIUS 0.0

public:
#ifndef NDEBUG
  CConxGeomObj::setDebuggingTag;
  CConxGeomObj::getDebuggingTag;
#endif
  CConxCircle();
  CConxCircle(const CConxPoint &center, double radius);
  CConxCircle(const CConxCircle &o);
  CConxCircle &operator=(const CConxCircle &o);

  double definingFunction(const CConxPoint &X) const
  // If this function evaluates to zero at X, then X is on this circle.
  {
    return (getCenter().distanceFrom(X) - getRadius());
  }
  void setCenter(const CConxPoint &c) { setA(c); }
  const CConxPoint &getCenter() const { return getA(); }
  void setRadius(double r);
  double getRadius() const { return getScalar(); }
  // DLC CConxPoint intersectionWith(const CConxLine &L) const;
  int operator==(const CConxCircle &o) const;
  int operator!=(const CConxCircle &o) const { return !operator==(o); }
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;

  void drawGarnishOn(CConxCanvas &cv) const;
  void drawBresenhamOn(CConxCanvas &cv) const;
  Boole requiresHeavyComputation() const { return TRUE; }
}; // class CConxCircle
// DLC TODO What is the HG area of a circle?


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_H_CIRCLE_CXX_H
