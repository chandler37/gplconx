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
  C++ Hyperbolic Geometry Line class
*/

#ifndef GPLCONX_H_LINE_CXX_H
#define GPLCONX_H_LINE_CXX_H 1

#include "h_twopts.hh"

//////////////////////////////////////////////////////////////////////////////
// DLC do line intersect line
// This models a line or line segment in hyperbolic geometry.  None of the
// mathematical functions notice that this is a line segment, though, so the
// distance from a point to the line may be less than than the distance from
// the point to any point on the line segment, e.g.
//
// You can ask an instance to describe itself in Euclidean terms in any of
// the models with getPUHP*, getK*, getPoincareDisk*, getPD*, etc.  This is
// useful for both visualization and some calculations.  In the Klein disk,
// it is easy to find the intersection of two lines, e.g.
class CConxLine : VIRT protected CConxTwoPts, public CConxSimpleArtist {
  CCONX_CLASSNAME("CConxLine")
  SA_IMP(SA_LINE, CConxLine)
public:
#ifndef NDEBUG
  CConxTwoPts::setDebuggingTag;
  CConxTwoPts::getDebuggingTag;
#endif
  CConxLine();
  CConxLine(const CConxPoint &A, const CConxPoint &B, Boole isSeg = FALSE);
  CConxLine(const CConxLine &o);
  CConxLine &operator=(const CConxLine &o);

  void setA(const CConxPoint &A) { isValid = 0; CConxTwoPts::setA(A); }
  void setB(const CConxPoint &A) { isValid = 0; CConxTwoPts::setB(A); }
  CONX_USING CConxTwoPts::getA;
  CONX_USING CConxTwoPts::getB;
  Pt getKleinEndPoint1() const;
  Pt getKleinEndPoint2() const;
  Pt getPoincareDiskEndPoint1() const;
  Pt getPoincareDiskEndPoint2() const;
  double getK_M() const;
  double getK_B() const;
  double getPD_R() const;
  double getPD_Cx() const;
  double getPD_Cy() const;
  double getPUHP_A() const;
  double getPUHP_R() const;
  int isTypeI() const;
  Boole isLineSegment() const { return isSegment; }
  void setSegment(Boole yess) { isSegment = yess; }
  double distanceFrom(const CConxPoint &P,
                      double computol = EQUALITY_TOL) const;
  void getPerpendicular(CConxLine &P, const CConxPoint &A,
                        double computol = EQUALITY_TOL) const;
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;

  int operator==(const CConxLine &o) const
  {
    return (isSegment == o.isSegment && CConxTwoPts::operator==(o));
  }
  int operator!=(const CConxLine &o) const { return !operator==(o); }

  void drawGarnishOn(CConxCanvas &cv) const;
  void drawBresenhamOn(CConxCanvas &cv) const;
  double definingFunction(const CConxPoint &X) const
  {
    return distanceFrom(X);
  }

private: // operations
  void convertTo(ConxModlType modl) const;

  void uninitializedCopy(const CConxLine &o);


private: // attributes
  Boole isSegment;
  mutable Bitflag isValid;
  mutable double x[CONX_NUM_MODELS], y[CONX_NUM_MODELS];
  // x[kd], y[kd] == m, b
  // x[pd], y[pd] == cx, cy
  // x[puhp], y[puhp] == a, r

  mutable double pd_radius; // the radius of this line in the Poincare disk.

  mutable Pt klein_endpts[2];
  mutable Pt pd_endpts[2];
  // The points on the Klein disk's boundary where the line goes at infinity;
  // useful for drawing.
}; // class CConxLine


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_H_LINE_CXX_H
