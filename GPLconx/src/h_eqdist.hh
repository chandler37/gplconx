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
  C++ HG equidistant curve
*/

#ifndef GPLCONX_H_EQDIST_CXX_H
#define GPLCONX_H_EQDIST_CXX_H 1

#include "h_geomob.hh"

//////////////////////////////////////////////////////////////////////////////
class CConxEqDistCurve
  : VIRT protected CConxGeomObj, public CConxSimpleArtist {
  CCONX_CLASSNAME("CConxEqDistCurve")
  SA_IMP(SA_EQDISTCURVE, CConxEqDistCurve)
public:
#ifndef NDEBUG
  CConxGeomObj::setDebuggingTag;
  CConxGeomObj::getDebuggingTag;
#endif
  CConxEqDistCurve() : isValid(FALSE) { }
  CConxEqDistCurve(const CConxLine &l, double K) : isValid(FALSE)
  {
    setLine(l);
    setDistance(K);
  }
  CConxEqDistCurve(const CConxEqDistCurve &A)
    : CConxGeomObj(A) { uninitializedCopy(A); }
  void setLine(const CConxLine &L);
  CONX_USING CConxGeomObj::getLine;
  void setDistance(double d) { isValid = FALSE; setScalar(d); }
  double getDistance() const { return getScalar(); }
  void getPointsOn(CConxPoint *lb, CConxPoint *rb,
                   double ymin, double ymax) const;
  CConxEqDistCurve &operator=(const CConxEqDistCurve &o);
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;

  double definingFunction(const CConxPoint &X) const
  {
    return (getLine().distanceFrom(X) - getDistance());
  }
  void drawGarnishOn(CConxCanvas &cv) const;
  void drawBresenhamOn(CConxCanvas &cv) const;
  Boole requiresHeavyComputation() const { return TRUE; }
  int operator==(const CConxEqDistCurve &o) const {
    return CConxGeomObj::operator==(o);
  }
  int operator!=(const CConxEqDistCurve &o) const { return !operator==(o); }

private: // operations
  void uninitializedCopy(const CConxEqDistCurve &o);

private: // attributes
  mutable Boole isValid;
  mutable CConxPoint LB, RB;
}; // class CConxEqDistCurve


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_H_EQDIST_CXX_H
