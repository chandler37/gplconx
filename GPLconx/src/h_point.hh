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
  C++ Hyperbolic Geometry Point class
*/

#ifndef GPLCONX_H_POINT_CXX_H
#define GPLCONX_H_POINT_CXX_H 1

#include "h_simple.hh"

class CConxLine;
//////////////////////////////////////////////////////////////////////////////
// This is a point that is in the plane or disk or on the boundary of it.
class CConxPoint : VIRT public CConxObject, public CConxSimpleArtist {
  CCONX_CLASSNAME("CConxPoint")
  SA_IMP(SA_POINT, CConxPoint)
public:
  CConxPoint();
  ~CConxPoint() { MMM("destructor"); }
  CConxPoint(const CConxPoint &);
  CConxPoint(double x, double y, ConxModlType modl);
  CConxPoint(Pt A, ConxModlType modl);
  CConxPoint &operator=(const CConxPoint &o);

  Boole isAtInfinity(double tol = EQUALITY_TOL) const; // This is not cached.
  void setPoint(Pt A, ConxModlType modl) { setPoint(A.x, A.y, modl); }
  void setPoint(double x, double y, ConxModlType modl);
  double getX(ConxModlType modl) const;
  double getY(ConxModlType modl) const;
  Pt getPt(ConxModlType modl) const;
  double distanceFrom(const CConxPoint &A, double tol = EQUALITY_TOL) const;
  double distanceFrom(const CConxLine &L,
                      double computol = EQUALITY_TOL) const;
  Boole isBetween(const CConxPoint &P, const CConxPoint &Q) const;
  int operator==(const CConxPoint &o) const;
  int operator!=(const CConxPoint &o) const { return !operator==(o); }
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;

  double definingFunction(const CConxPoint &X) const
  {
    return distanceFrom(X);
  }
  void drawGarnishOn(CConxCanvas &cv) const;
  void drawBresenhamOn(CConxCanvas &cv) const;

private: // operations
  static void modelToModel(double xFrom, double yFrom, ConxModlType mFrom,
                           double *xTo, double *yTo, ConxModlType mTo);
  void convertTo(ConxModlType modl) const;

  void uninitializedCopy(const CConxPoint &o);
  // Copies CConxPoint-specific data from o, but does not rely on this object
  // being in a defined state.

private: // attributes
  mutable double x[CONX_NUM_MODELS], y[CONX_NUM_MODELS];
  // (x[CONX_POINCARE_UHP], y[CONX_POINCARE_UHP]) is this point in the
  // Poincare UHP.

  mutable Bitflag isValid;
  // This is a bitmask that indicates which of the three models we have
  // already converted to.
}; // class CConxPoint


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_H_POINT_CXX_H
