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

class CConxLine;
class CConxCanvas;
class CConxPoint;


//////////////////////////////////////////////////////////////////////////////
// A class that implements this interface draws itself without regard to
// color, line size, etc.
class /* interface */ CConxSimpleArtist : public CConxPrintable {
public:
  virtual ~CConxSimpleArtist() { LLL("CConxSimpleArtist interface implementor destructor activated.  If this were not virtual, there would be a crash."); }
  /* So that we don't need run-type type identification (rtti): */
  enum SAType { SA_POINT, SA_LINE, SA_CIRCLE, SA_HYPELLIPSE, SA_EQDISTCURVE,
                SA_PARABOLA
  };
  static const char *humanSAType(SAType s);
  virtual SAType getSAType() const = 0;

  // DLC use rtti cast:
#define SA_TYPE(m, Me) private: SAType getSAType() const { return m; } \
           public: int eql(const CConxSimpleArtist &o) const \
                   { \
                     return (getSAType() == o.getSAType() \
                             && operator==((const Me &)o)); \
                   }
                   
  


  // You must overload the assignment operator, copy constructor,
  // and equality operator.

  // There are many ways to draw yourself.  These may all do the same
  // thing for some objects.
  
  // If a display list (stored drawing) might be a good idea, override
  // this to return TRUE:
  virtual Boole requiresHeavyComputation() const { return FALSE; }

  // If you are a line, draw your two defining points, e.g.:
  virtual void drawGarnishOn(CConxCanvas &cv) const = 0;

  // Bresenham means ``Either by the Bresenham method, or the best, fastest,
  // way you know how''.
  virtual void drawBresenhamOn(CConxCanvas &cv) const = 0;
   
  // This function returns zero if and only if X is on the object.
  // Most of the time, nearly zero means nearly on the object.
  virtual double definingFunction(const CConxPoint &X) const = 0;
#define SA_DEFFN() \
 private: \
   static double definingFunctionWrapper(const CConxSimpleArtist *sa, \
                                         const CConxPoint &X) \
   { \
     assert(sa != NULL); \
     return sa->definingFunction(X); \
   }

  // A virtual copy constructor, if you will.  This is needed
  // because CConxDwGeomObj is a container, but copying it is a deep copy,
  // not a shallow one.
  virtual CConxSimpleArtist *clone() const = 0;

// Relies on the copy constructor:
#define SA_CLONE(Me) private: \
          CConxSimpleArtist *clone() const { return new Me(*this); }

  // Virtual operator==, if you will:
  virtual int eql(const CConxSimpleArtist &o) const = 0;


#define SA_IMP(m, Me) \
 SA_TYPE(m, Me) \
 SA_CLONE(Me) \
 SA_DEFFN()


}; // class CConxSimpleArtist


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
  virtual int operator==(const CConxPoint &o) const;
  virtual int operator!=(const CConxPoint &o) const { return !operator==(o); }
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType modl) const;

  double definingFunction(const CConxPoint &X) const
  {
    return distanceFrom(X);
  }
  void drawGarnishOn(CConxCanvas &cv) const;
  void drawBresenhamOn(CConxCanvas &cv) const;

private: // operations
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
}; // class CConxTwoPts


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
  CConxHypEllipse(const CConxHypEllipse &A)
    : CConxGeomObj(A)
  {
    uninitializedCopy(A);
  }
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

private: // operations
  void uninitializedCopy(const CConxEqDistCurve &o);

private: // attributes
  mutable Boole isValid;
  mutable CConxPoint LB, RB;
}; // class CConxEqDistCurve


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

// DLC TODO Include polygons as geometric objects.

OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxPoint);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxTwoPts);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxGeomObj);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxCircle);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxLine);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxHypEllipse);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxEqDistCurve);
OOLTLT_INLINE PBM_STREAM_OUTPUT_SHORTCUT_DECL(CConxParabola);


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////

inline const char *CConxSimpleArtist::humanSAType(SAType s)
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

inline
void CConxPoint::uninitializedCopy(const CConxPoint &o)
{
  isValid = o.isValid;
  for (int i = 0; i < CONX_NUM_MODELS; i++) {
    x[i] = o.x[i];
    y[i] = o.y[i];
  }
}

inline
void CConxLine::uninitializedCopy(const CConxLine &o)
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

inline
void CConxParabola::uninitializedCopy(const CConxParabola &o)
{
  isValid = o.isValid;
  if (isValid)
    pLB = o.pLB;
}

inline
double CConxHypEllipse::definingFunction(const CConxPoint &X) const
{
  return (isEllipse()
          ? (getFocus1().distanceFrom(X)
             + getFocus2().distanceFrom(X) - getScalar())
          : (myabs(getFocus1().distanceFrom(X)
                   - getFocus2().distanceFrom(X)) - getScalar()));
}

inline
void CConxHypEllipse::init()
{
  isValid = FALSE;
  isEllipseIsValid = FALSE;

  isEllips = FALSE;
  // This is not really necessary, but we don't want to copy an
  // uninitialized field.
}

inline
void CConxHypEllipse::uninitializedCopy(const CConxHypEllipse &o)
{
  isValid = o.isValid;
  isEllipseIsValid = o.isEllipseIsValid;
  isEllips = o.isEllips;
  if (isValid) {
    LB = o.LB;
    RB = o.RB;
  }
}

inline
void CConxEqDistCurve::uninitializedCopy(const CConxEqDistCurve &o)
{
  isValid = o.isValid;
  if (isValid) {
    LB = o.LB;
    RB = o.RB;
  }
}

#endif // GPLCONX_GEOMOBJ_CXX_H
