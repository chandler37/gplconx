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
  C++ HG hyperbola/ellipse
*/

#ifndef GPLCONX_H_HYPELL_CXX_H
#define GPLCONX_H_HYPELL_CXX_H 1

#include "h_geomob.hh"

//////////////////////////////////////////////////////////////////////////////
// The foci are called `A' and `B'.  Use `getA()', `setA()', etc. to query
// and change them.  The sum (for an ellipse) or the absolute value of the
// difference (for a hyperbola)
// of the distances between each focus and a point C is equal to Scalar.  Use
// getScalar() and setScalar() to change it.
class CConxHypEllipse
  : VIRT protected CConxGeomObj, public CConxSimpleArtist {
  CCONX_CLASSNAME("CConxHypEllipse")
  SA_IMP(SA_HYPELLIPSE, CConxHypEllipse)
public:
#ifndef NDEBUG
  CConxGeomObj::setDebuggingTag;
  CConxGeomObj::getDebuggingTag;
#endif
  CConxHypEllipse() { init(); }
  CConxHypEllipse(const CConxPoint &f1, const CConxPoint &f2, double s)
  {
    init(); setScalar(s); setFocus1(f1); setFocus2(f2);
  }
  CConxHypEllipse(const CConxHypEllipse &A)
    : CConxGeomObj(A) { uninitializedCopy(A); }
  void setScalar(double S) { isValid = FALSE; CConxGeomObj::setScalar(S); }
  CONX_USING CConxGeomObj::getScalar;
  void setFocus1(const CConxPoint &p) { isValid = FALSE; setA(p); }
  const CConxPoint &getFocus1() const { return getA(); }
  void setFocus2(const CConxPoint &p) { isValid = FALSE; setB(p); }
  const CConxPoint &getFocus2() const { return getB(); }
  int isEllipse() const;
  void getPointsOn(CConxPoint *lb, CConxPoint *rb) const;
  CConxHypEllipse &operator=(const CConxHypEllipse &o);

  // With a large enough tol, you may decide that an ellipse is equal to
  // a hyperbola.
  int operator==(const CConxHypEllipse &o) const {
    return CConxGeomObj::operator==(o);
  }
  int operator!=(const CConxHypEllipse &o) const { return !operator==(o); }
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;

  void drawGarnishOn(CConxCanvas &cv) const;
  void drawBresenhamOn(CConxCanvas &cv) const;
  double definingFunction(const CConxPoint &X) const;

private: // operations
  void init();
  void uninitializedCopy(const CConxHypEllipse &o);

private: // attributes
  // Save the point(s) analytically determined to be on the ellipse(hyperbola)
  mutable Boole isValid, isEllipseIsValid, isEllips;
  mutable CConxPoint LB, RB;
}; // class CConxHypEllipse


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_H_HYPELL_CXX_H
