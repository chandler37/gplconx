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
  use is not allowed (though virtual inheritance is always used, though
  useless without multiple inheritance of classes with attributes, just
  because I can't think of a time when you would want the default
  model).  To avoid showing the user the superflous parts of CConxGeomObj,
  it is inherited with the `protected' access model and the useful operations
  are then granted public access.

  Most computed values are cached until a dependency changes.  For example,
  a point on a parabola found via CConxParabola::getPointOn is remembered
  until the parabola's focus or line is altered.

  Default values for floating-point attributes must be exactly representable
  in IEEE single-precision, floating-point numbers; `1.0', e.g., is okay.
  This is so that operator==() will work correctly with EQUALITY_TOL==`0.0'.

  See `README.DEV' for more on the above topics.
*/

#ifndef GPLCONX_GEOMOBJ_CXX_H
#define GPLCONX_GEOMOBJ_CXX_H 1

#include <iostream.h>

#include "point.h"
extern "C" {
#include "viewer.h"
}
#include "printon.hh"
#include "decls.hh"

// The rest of the code doesn't want +INF or NaN, it wants a large double.
// This is within an order of magnitude of the largest double, which could
// be found by enquire or paranoia.  <float.h> would have this as DBL_MAX.
// We don't bother with all that since it is not standard and since
// this is plenty big:
#define CCONX_INFINITY 1.8e+307
// On x86, I think 1.7976931348623157e+308 is the correct value.


// So that the bit-flags 0, a=CONX_MODEL2BIT(CONX_POINCARE_DISK),
// b=CONX_MODEL2BIT(CONX_KLEIN_DISK),
// c=CONX_MODEL2BIT(CONX_POINCARE_UHP),
// a|b, a|c, b|c, and a|b|c are all distinct, we define:
#define CONX_MODEL2BIT(modl) (1 << modl)


class CConxLine;


//////////////////////////////////////////////////////////////////////////////
// This is a point that is in the plane or disk or on the boundary of it.
class CConxPoint : VIRT public CConxObject, public CConxPrintableByModel {
  CCONX_CLASSNAME("CConxPoint")
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
  virtual int operator==(const CConxPoint &o) const;
  virtual int operator!=(const CConxPoint &o) const { return !operator==(o); }
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;


private: // operations
  void convertTo(ConxModlType modl) const;
  void uninitializedCopy(const CConxPoint &o)
  // copy CConxPoint-specific data from o, but do not rely on this object
  // being in a defined state.
  {
    isValid = o.isValid;
    for (int i = 0; i < CONX_NUM_MODELS; i++) {
      x[i] = o.x[i];
      y[i] = o.y[i];
    }
  }

private: // attributes
  mutable double x[CONX_NUM_MODELS], y[CONX_NUM_MODELS];
  // (x[CONX_POINCARE_UHP], y[CONX_POINCARE_UHP]) is this point in the
  // Poincare UHP.

  mutable Bitflag isValid;
  // This is a bitmask that indicates which of the three models we have
  // already converted to.
};


//////////////////////////////////////////////////////////////////////////////
// A class that contains two points, A and B.
class CConxTwoPts : VIRT public CConxObject, public CConxPrintableByModel {
  CCONX_CLASSNAME("CConxTwoPts")
  DEFAULT_PRINTON()
public:
  CConxTwoPts();
  CConxTwoPts(const CConxPoint &A, const CConxPoint &B);
  CConxTwoPts(const CConxTwoPts &m);
  CConxTwoPts &operator=(const CConxTwoPts &o);
  ~CConxTwoPts() { MMM("destructor"); }

  void setA(const CConxPoint &A) { this->A = A; }
  void setB(const CConxPoint &B) { this->B = B; }
  const CConxPoint &getA() const { return A; }
  const CConxPoint &getB() const { return B; }
  virtual int operator==(const CConxTwoPts &o) const;
  virtual int operator!=(const CConxTwoPts &o) const;
  ostream &printOn(ostream &o, ConxModlType m) const { return printOn(o); }

private:
  CConxPoint A, B;
};


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
class CConxLine : VIRT protected CConxTwoPts {
  CCONX_CLASSNAME("CConxLine")
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


private: // operations
  void convertTo(ConxModlType modl) const;

  void uninitializedCopy(const CConxLine &o)
  // copy CConxLine-specific data from o, but do not rely on this object
  // being in a defined state.
  {
    isSegment = o.isSegment;
    isValid = o.isValid;
    for (int i = 0; i < CONX_NUM_MODELS; i++) {
      x[i] = o.x[i];
      y[i] = o.y[i];
    }
    pd_radius = o.pd_radius;
    klein_endpts[0] = o.klein_endpts[0];
    klein_endpts[1] = o.klein_endpts[1];
    pd_endpts[0] = o.pd_endpts[0];
    pd_endpts[1] = o.pd_endpts[1];
  }


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
// An abstract class that has a superset of the required data for any
// other geometric class in this file.  Other classes may wish to change
// the names of some fields.
class CConxGeomObj : VIRT public CConxTwoPts {
  CCONX_CLASSNAME("CConxGeomObj")
  DEFAULT_PRINTON()
public:
  CConxGeomObj();
  CConxGeomObj(const CConxGeomObj &a);
  CConxGeomObj &operator=(const CConxGeomObj &o);
  ~CConxGeomObj() { MMM("destructor"); }

  void setLine(const CConxLine &L) { this->L = L; }
  const CConxLine &getLine() const { return L; }
  double getScalar() const { return scalar; }
  void setScalar(double x) { scalar = x; }
  int operator==(const CConxGeomObj &o) const;
  int operator!=(const CConxGeomObj &o) const { return !operator==(o); }
  ostream &printOn(ostream &o, ConxModlType m) const { return printOn(o); }


private:
  CConxLine L;
  double scalar;
}; // class CConxGeomObj
// DLC TODO int CConxLine:isParallelTo(const CConxLine& o);


//////////////////////////////////////////////////////////////////////////////
class CConxParabola : VIRT protected CConxGeomObj {
  CCONX_CLASSNAME("CConxParabola")
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
  void getPointOn(CConxPoint *LB, double computol = EQUALITY_TOL) const;
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;

  // For equality,
  // isValid and pLB don't matter -- they'll be the same if the line and
  // focus are.  Thus we can let CConxGeomObj::operator== and != remain.
  int operator==(const CConxParabola &o) const;
  int operator!=(const CConxParabola &o) const { return !operator==(o); }


private: // operations
  void uninitializedCopy(const CConxParabola &o)
  {
    isValid = o.isValid;
    if (isValid)
      pLB = o.pLB;
  }

private: // attributes
  mutable Boole isValid;
  mutable CConxPoint pLB; // A Point on the Parabola.
};


//////////////////////////////////////////////////////////////////////////////
// The foci are called `A' and `B'.  Use `getA()', `setA()', etc. to query
// and change them.  The sum (for an ellipse) or the absolute value of the
// difference (for a hyperbola)
// of the distances between each focus and a point C is equal to Scalar.  Use
// getScalar() and setScalar() to change it.
class CConxHypEllipse : VIRT protected CConxGeomObj {
  CCONX_CLASSNAME("CConxHypEllipse")
public:
#ifndef NDEBUG
  CConxGeomObj::setDebuggingTag;
  CConxGeomObj::getDebuggingTag;
#endif
  CConxHypEllipse() : isValid(FALSE) { }
  CConxHypEllipse(const CConxHypEllipse &A)
    : CConxGeomObj(A)
  {
    uninitializedCopy(A);
  }
  void setScalar(double S) { isValid = 0; CConxGeomObj::setScalar(S); }
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


private: // operations
  void uninitializedCopy(const CConxHypEllipse &o)
  {
    isValid = o.isValid;
    if (isValid) {
      LB = o.LB;
      RB = o.RB;
    }
  }

private: // attributes
  // Save the point(s) analytically determined to be on the ellipse(hyperbola)
  mutable Boole isValid;
  mutable CConxPoint LB, RB;
};


//////////////////////////////////////////////////////////////////////////////
class CConxEqDistCurve : VIRT protected CConxGeomObj {
  CCONX_CLASSNAME("CConxEqDistCurve")
public:
#ifndef NDEBUG
  CConxGeomObj::setDebuggingTag;
  CConxGeomObj::getDebuggingTag;
#endif
  CConxEqDistCurve() : isValid(FALSE) { }
  CConxEqDistCurve(const CConxEqDistCurve &A)
    : CConxGeomObj(A)
  {
    uninitializedCopy(A);
  }
  void setLine(const CConxLine &L);
  CONX_USING CConxGeomObj::getLine;
  void setDistance(double d) { isValid = FALSE; setScalar(d); }
  double getDistance() const { return getScalar(); }
  void getPointsOn(CConxPoint *lb, CConxPoint *rb,
                   double ymin, double ymax) const;
  CConxEqDistCurve &operator=(const CConxEqDistCurve &o);
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;


private: // operations
  void uninitializedCopy(const CConxEqDistCurve &o)
  {
    isValid = o.isValid;
    if (isValid) {
      LB = o.LB;
      RB = o.RB;
    }
  }

private: // attributes
  mutable Boole isValid;
  mutable CConxPoint LB, RB;
};


//////////////////////////////////////////////////////////////////////////////
class CConxCircle : VIRT protected CConxGeomObj {
  CCONX_CLASSNAME("CConxCircle")
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
};

// DLC TODO What is the HG area of a circle?

// DLC TODO Include polygons as geometric objects.

OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxPoint);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxTwoPts);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxGeomObj);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxCircle);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxLine);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxHypEllipse);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxEqDistCurve);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxParabola);

#endif // GPLCONX_GEOMOBJ_CXX_H

