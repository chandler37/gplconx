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
  C++ HG parabola.
*/

#ifndef GPLCONX_H_PARABO_CXX_H
#define GPLCONX_H_PARABO_CXX_H 1

#include "h_geomob.hh"

//////////////////////////////////////////////////////////////////////////////
class CConxParabola : VIRT protected CConxGeomObj, public CConxSimpleArtist {
  CCONX_CLASSNAME("CConxParabola")
  SA_IMP(SA_PARABOLA, CConxParabola)
public:
#ifndef NDEBUG
  CConxGeomObj::setDebuggingTag;
  CConxGeomObj::getDebuggingTag;
#endif
  CConxParabola();
  CConxParabola(const CConxParabola &a); // DLC does this do a parent copy constructor??
  CConxParabola(const CConxPoint &P, const CConxLine &L);
  CConxParabola &operator=(const CConxParabola &a);
  CONX_USING CConxGeomObj::getLine;
  void setLine(const CConxLine &o);
  const CConxPoint &getFocus() const { return getA(); }
  void setFocus(const CConxPoint &A) { isValid = FALSE; setA(A); }
  int getPointOn(CConxPoint *LB, double computol = EQUALITY_TOL) const;
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;

  // For equality,
  // isValid and pLB don't matter -- they'll be the same if the line and
  // focus are.  Thus we can let CConxGeomObj::operator== and != remain.
  int operator==(const CConxParabola &o) const;
  int operator!=(const CConxParabola &o) const { return !operator==(o); }

  void drawGarnishOn(CConxCanvas &cv) const;
  void drawBresenhamOn(CConxCanvas &cv) const;
  double definingFunction(const CConxPoint &X) const
  {
    return getFocus().distanceFrom(X) - getLine().distanceFrom(X);
  }

private: // operations
  void uninitializedCopy(const CConxParabola &o);

private: // attributes
  mutable Boole isValid;
  mutable CConxPoint pLB; // A Point on the Parabola.
}; // class CConxParabola


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_H_PARABO_CXX_H
