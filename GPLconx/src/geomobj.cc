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
  Implementation of C++ classes in `geomobj.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

extern "C" {
#include "viewer.h"
}

#include "hypmath.hh"
#include "cassert.h"
#include "geomobj.hh"

CF_INLINE
CConxPoint::CConxPoint()
{
  MMM("CConxPoint()");
  // (0.0, 0.0),  a valid Klein disk coordinate, is our mostly arbitrary
  // initial value (``Mostly'' because 0.0 loses no precision when stored
  // as a float, which is all we need.)
  for (int i = 0; i < CONX_NUM_MODELS; i++) {
    x[i] = y[i] = 0.0;
  }
  isValid = CONX_MODEL2BIT(CONX_KLEIN_DISK);
}

CF_INLINE
CConxPoint::CConxPoint(const CConxPoint &o)
  : CConxObject(o)
{
  MMM("CConxPoint(const CConxPoint &o)");
  uninitializedCopy(o);
}

NF_INLINE
CConxPoint &CConxPoint::operator=(const CConxPoint &o)
{
  MMM("CConxPoint &operator=(const CConxPoint &o)");
  (void) CConxObject::operator=(o);
  uninitializedCopy(o);
  return *this;
}

CF_INLINE
CConxPoint::CConxPoint(double x, double y, ConxModlType modl)
{
  MMM("CConxPoint(double x, double y, ConxModlType modl)");
  // So that the invariant is invariant:
  isValid = CONX_MODEL2BIT(modl);

  setPoint(x, y, modl);
}

CF_INLINE
CConxPoint::CConxPoint(Pt A, ConxModlType modl)
{
  // So that the invariant is invariant:
  isValid = CONX_MODEL2BIT(modl);

  setPoint(A, modl);
}

NF_INLINE
int CConxPoint::operator==(const CConxPoint &o) const
{
  return (distanceFrom(o) <= EQUALITY_TOL);
}

NF_INLINE
void CConxPoint::setPoint(double x, double y, ConxModlType modl)
{
  CONX_INVARIANT(isValid != 0); // One model is valid at all times.
  if (modl == CONX_POINCARE_UHP) {
    if (y < 0.0) y = 0.0;
  }
  this->x[modl] = x; this->y[modl] = y;
  isValid = CONX_MODEL2BIT(modl); // Only one bit, the correct one, is set.
}

NF_INLINE
Boole CConxPoint::isAtInfinity(double tol) const
// Returns TRUE if this point is at infinity, i.e. it is infinitely far
// away from any other point.  If the puhp, this is (x, 0.0); in the disks,
// this is a point that is greater than or equal to 1 Euclidean unit away
// from the origin (the center of the unit circle).
//
// If tol is positive, then some points that are not at infinity are treated
// as if they are.  The larger tol is, the more such points there are.
//
// This value is not cached.
{
  CONX_INVARIANT(isValid != 0);
  if (tol < 0.0) tol = 0.0;
  if (isValid & CONX_MODEL2BIT(CONX_POINCARE_UHP)) {
    return y[CONX_POINCARE_UHP] <= 0.0 + tol;
  } else {
    ConxModlType modl = CONX_KLEIN_DISK;
    if (isValid & CONX_MODEL2BIT(CONX_POINCARE_DISK))
      modl = CONX_POINCARE_DISK;
    // Test to see if the distance from the origin, sqrt(x^2+y^2), is within
    // tol of being greater than or equal 1.0.

    // sqrt(x^2+y^2) >= 1.0 - tol
    //  ===
    // x^2+y^2 >= (1 - tol)^2
    return (sqr(x[modl]) + sqr(y[modl]) >= sqr(1.0 - tol));
  }
}

NF_INLINE
double CConxPoint::getX(ConxModlType modl) const
{
  CONX_INVARIANT(isValid != 0); // One model is valid at all times.
  if (isValid & CONX_MODEL2BIT(modl))
    return x[modl];
  // Else, convert from a known model to this model.
  convertTo(modl);
  return x[modl];
}

NF_INLINE
double CConxPoint::getY(ConxModlType modl) const
{
  CONX_INVARIANT(isValid != 0);
  if (isValid & CONX_MODEL2BIT(modl))
    return y[modl];
  // Else, convert from a known model to this model.
  convertTo(modl);
  return y[modl];
}

NF_INLINE
Pt CConxPoint::getPt(ConxModlType modl) const
{
  Pt n;
  n.x = getX(modl);
  n.y = getY(modl);
  return n;
}

PF_INLINE
ostream &CConxPoint::printOn(ostream &o) const
{
  CONX_INVARIANT(isValid != 0);
  o << "[puhp(" << getX(CONX_POINCARE_UHP) << ", " << getY(CONX_POINCARE_UHP)
    << "), ";
  o << "kd(" << getX(CONX_KLEIN_DISK) << ", " << getY(CONX_KLEIN_DISK)
    << "), ";
  o << "pd(" << getX(CONX_POINCARE_DISK) << ", " << getY(CONX_POINCARE_DISK)
    << ")]";
  return o;
}

NF_INLINE
void CConxPoint::convertTo(ConxModlType modl) const
{
  CONX_INVARIANT(isValid != 0);
  for (int i = 0; i < CONX_NUM_MODELS; i++) {
    if (isValid & CONX_MODEL2BIT(i)) {
      // Convert from model i to model modl.
      CConxMultiModel::mmconvert(x[i], y[i], (ConxModlType) i,
                                 x+modl, y+modl, modl);
      isValid |= CONX_MODEL2BIT((ConxModlType) i);
      return;
    }
  }
}

NF_INLINE
double CConxPoint::distanceFrom(const CConxPoint &A, double tol) const
{
  if (isAtInfinity(tol) || A.isAtInfinity(tol)) return CCONX_INFINITY;
  return conxk_distAB(getPt(CONX_KLEIN_DISK), A.getPt(CONX_KLEIN_DISK));
}

NF_INLINE
double CConxPoint::distanceFrom(const CConxLine &L, double computol) const
// Returns the distance from the line L along the unique perpendicular.
{
  if (isAtInfinity(computol)) return CCONX_INFINITY;
  return conxk_distFrommbX(L.getK_M(), L.getK_B(), getPt(CONX_KLEIN_DISK),
                           computol);
}

NF_INLINE
Boole CConxPoint::isBetween(const CConxPoint &P, const CConxPoint &Q) const
// Returns non-zero if this point lies between P and Q on the line on which
// they are assumed to be collinear.
{
  return !conx_pt_not_between(P.getPt(CONX_POINCARE_UHP),
                              getPt(CONX_POINCARE_UHP),
                              Q.getPt(CONX_POINCARE_UHP));
}

PF_INLINE
ostream &CConxPoint::printOn(ostream &o, ConxModlType modl) const
{
  CONX_INVARIANT(isValid != 0);
  o << conx_modelenum2short_string(modl) << "(" << getX(modl) << ", "
    << getY(modl) << ")";
  return o;
}

CF_INLINE
CConxTwoPts::CConxTwoPts()
  : A(0.31, 0.32, CONX_KLEIN_DISK), B(0.33, 0.34, CONX_KLEIN_DISK)
       // These values may aid in debugging.
{
  MMM("CConxTwoPts()");
}

CF_INLINE
CConxTwoPts::CConxTwoPts(const CConxPoint &A, const CConxPoint &B)
  : A(), B()
{
  MMM("CConxTwoPts(const CConxPoint &_A, const CConxPoint &_B)");
  this->A = A; this->B = B;
}

CF_INLINE
CConxTwoPts::CConxTwoPts(const CConxTwoPts &m)
  : CConxObject(m), A(), B()
{
  MMM("CConxTwoPts(const CConxTwoPts &m)");
  A = m.getA(); B = m.getB();
}

NF_INLINE
CConxTwoPts &CConxTwoPts::operator=(const CConxTwoPts &o)
{
  MMM("CConxTwoPts &operator=(const CConxTwoPts &o)");
  (void) CConxObject::operator=(o);
  setA(o.getA()); setB(o.getB());
  return *this;
}

NF_INLINE
int CConxTwoPts::operator==(const CConxTwoPts &o) const
{
  return (getA() == o.getA() && getB() == o.getB());
}

NF_INLINE
int CConxTwoPts::operator!=(const CConxTwoPts &o) const
{
  return !operator==(o);
}

CF_INLINE
CConxLine::CConxLine()
  : isValid(0)
{
  MMM("CConxLine()");
  isSegment = FALSE;
}

CF_INLINE
CConxLine::CConxLine(const CConxLine &o)
  : CConxTwoPts(o)
{
  MMM("CConxLine(const CConxLine &o)");
  uninitializedCopy(o);
}

NF_INLINE
CConxLine &CConxLine::operator=(const CConxLine &o)
{
  MMM("CConxLine &CConxLine::operator=(const CConxLine &o)");
  (void) CConxTwoPts::operator=(o);
  uninitializedCopy(o);
  return *this;
}

CF_INLINE
CConxLine::CConxLine(const CConxPoint &A, const CConxPoint &B, Boole isSeg)
  : isValid(0)
{
  MMM("CConxLine(const CConxPoint &A, const CConxPoint &B, Boole isSeg = FALSE)");
  isSegment = isSeg;
  setA(A);
  setB(B);
}

NF_INLINE
Pt CConxLine::getKleinEndPoint1() const
{
  convertTo(CONX_KLEIN_DISK);
  return klein_endpts[0];
}

NF_INLINE
Pt CConxLine::getKleinEndPoint2() const
{
  convertTo(CONX_KLEIN_DISK);
  return klein_endpts[1];
}

NF_INLINE
Pt CConxLine::getPoincareDiskEndPoint1() const
{
  convertTo(CONX_POINCARE_DISK);
  return pd_endpts[0];
}

NF_INLINE
Pt CConxLine::getPoincareDiskEndPoint2() const
{
  convertTo(CONX_POINCARE_DISK);
  return pd_endpts[1];
}

NF_INLINE
int CConxLine::isTypeI() const
// Returns non-zero if this is a Type I Poincare UHP line; otherwise
// it is a Type II Poincare UHP line.
{
  return (getPUHP_R() < 0.0);
}

NF_INLINE
double CConxLine::distanceFrom(const CConxPoint &P, double computol) const
{
  return P.distanceFrom(*this, computol);
}

NF_INLINE
void CConxLine::getPerpendicular(CConxLine &P, const CConxPoint &A,
                                 double computol) const
// Sets P to be the unique line perpendicular to this line that goes
// through A.
{
  Pt Near;

  // We find the point `Near' on this line that is nearest to A.
  conxk_getPtNearXonmb(A.getPt(CONX_KLEIN_DISK), getK_M(), getK_B(), &Near,
                       computol);
  P.setA(A);
  P.setB(CConxPoint(Near, CONX_KLEIN_DISK));
}

PF_INLINE
ostream &CConxLine::printOn(ostream &o) const
{
  o << "CConxLine ";
  if (isLineSegment()) { //test DLC
    o << "segment ";
  }
  o << "(A=";
  getA().printOn(o);
  o << ", B=";
  getB().printOn(o);
  o << ")";
  return o;
}

PF_INLINE
ostream &CConxLine::printOn(ostream &o, ConxModlType modl) const
{
  o << "CConxLine ";
  if (isLineSegment()) { //test DLC
    o << "segment ";
  }
  o << "[(A=";
  getA().printOn(o, modl);
  o << ", B=";
  getB().printOn(o, modl);
  o << "), ";
  if (modl == CONX_POINCARE_DISK) {
    o << "Orthogonal circle's center=(" << getPD_Cx() << ", "
      << getPD_Cy() << ") which implies the radius is " << getPD_R() << "]";
  } else if (modl == CONX_KLEIN_DISK) {
    o << "m=" << getK_M() << ", b=" << getK_B() << "]";
  } else {
    o << "a=" << getPUHP_A() << ", r=" << getPUHP_R() << "]";
  }
  return o;
}

NF_INLINE
void CConxLine::convertTo(ConxModlType modl) const
// Convert so that the Euclidean description for model modl is valid.
{
  Pt A, B;

  if (!(isValid & CONX_MODEL2BIT(modl))) {
    // We will get the Poincare Disk description first since we can convert
    // from it to any of the others easily.
    A = getA().getPt(CONX_POINCARE_DISK);
    B = getB().getPt(CONX_POINCARE_DISK);
    conxpd_getCenterAndRadius(A.x, A.y, B.x, B.y, x+CONX_POINCARE_DISK,
                              y+CONX_POINCARE_DISK, &pd_radius);
    isValid |= CONX_MODEL2BIT(CONX_POINCARE_DISK);
    if (modl == CONX_POINCARE_UHP) {
      conxhm_getarfromc(x[CONX_POINCARE_DISK], y[CONX_POINCARE_DISK],
                        x+CONX_POINCARE_UHP, y+CONX_POINCARE_UHP);
      
      isValid |= CONX_MODEL2BIT(CONX_POINCARE_UHP);
    } else if (modl == CONX_KLEIN_DISK) {
      conxhm_getmbfromc(x[CONX_POINCARE_DISK], y[CONX_POINCARE_DISK],
                        x+CONX_KLEIN_DISK, y+CONX_KLEIN_DISK);
      conxhm_getendptsmb(x[CONX_KLEIN_DISK], y[CONX_KLEIN_DISK],
                         klein_endpts+0, klein_endpts+1);
      isValid |= CONX_MODEL2BIT(CONX_KLEIN_DISK);
    } else {
      assert(modl == CONX_POINCARE_DISK);

      // DLC test this in tgeomobj
      conxhm_getendptsc(x[CONX_POINCARE_DISK], y[CONX_POINCARE_DISK],
                        pd_endpts+0, pd_endpts+1);

    }
  }
  assert(isValid & CONX_MODEL2BIT(modl));
}

NF_INLINE
double CConxLine::getK_M() const
// Get the slope of the Euclidean line that this appears to be when in
// the Klein model.
{
  convertTo(CONX_KLEIN_DISK);
  return x[CONX_KLEIN_DISK];
}

NF_INLINE
double CConxLine::getK_B() const
// Get the y-intercept of the Euclidean line that this appears to be when in
// the Klein model.
{
  convertTo(CONX_KLEIN_DISK);
  return y[CONX_KLEIN_DISK];
}

NF_INLINE
double CConxLine::getPD_R() const
// Gets the radius of the center of the circle that represents the
// line, which is orthogonal to
// the unit circle that is the horizon of the Poincare Disk.  The radius
// is implied by the center.
{
  convertTo(CONX_POINCARE_DISK);
  return pd_radius;
}

NF_INLINE
double CConxLine::getPD_Cx() const
// Gets the x coordinate of the center of the circle that represents the line,
// which is orthogonal to
// the unit circle that is the horizon of the Poincare Disk.  The radius
// is implied by the center.
{
  convertTo(CONX_POINCARE_DISK);
  return x[CONX_POINCARE_DISK];
}

NF_INLINE
double CConxLine::getPD_Cy() const
// Gets the y coordinate of the center of the circle that represents the line,
// which is orthogonal to
// the unit circle that is the horizon of the Poincare Disk.  The radius
// is implied by the center.
{
  convertTo(CONX_POINCARE_DISK);
  return y[CONX_POINCARE_DISK];
}

NF_INLINE
double CConxLine::getPUHP_A() const
// In the Poincare UHP, a line is either a Type I line that is described
// (in Euclidean terms) by the equation x=A, or it is a Type II line that
// is (in Euclidean terms) the half of the circle centered at (A, 0) with
// radius R that is in the upper half plane.  Returns A.
{
  convertTo(CONX_POINCARE_UHP);
  return x[CONX_POINCARE_UHP];
}

NF_INLINE
double CConxLine::getPUHP_R() const
// Returns the radius of the Type II Poincare UHP circle, or a negative number
// to indicate that this is a Type I Poincare UHP line.
{
  convertTo(CONX_POINCARE_UHP);
  return y[CONX_POINCARE_UHP];
}

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

CF_INLINE
CConxParabola::CConxParabola()
  : isValid(FALSE)
{
}

CF_INLINE
CConxParabola::CConxParabola(const CConxParabola &a)
  : CConxGeomObj(a)
{
  uninitializedCopy(a);
}

CF_INLINE
CConxParabola::CConxParabola(const CConxPoint &P, const CConxLine &L)
  : isValid(FALSE)
{
  setFocus(P);
  setLine(L);
}

CF_INLINE
CConxParabola &CConxParabola::operator=(const CConxParabola &a)
{
  (void) CConxGeomObj::operator=(a);
  uninitializedCopy(a);
  return *this;
}

NF_INLINE
int CConxParabola::operator==(const CConxParabola &o) const
{
  return CConxGeomObj::operator==(o);
}

PF_INLINE
ostream &CConxParabola::printOn(ostream &o) const
{
  o << "CConxParabola (line=";
  getLine().printOn(o);
  o << ", focus=";
  getFocus().printOn(o);
  o << ")";
  return o;
}

PF_INLINE
ostream &CConxParabola::printOn(ostream &o, ConxModlType modl) const
{
  o << "CConxParabola (line=";
  getLine().printOn(o, modl);
  o << ", focus=";
  getFocus().printOn(o, modl);
  o << ")";
  return o;
}

NF_INLINE
void CConxParabola::setLine(const CConxLine &o)
{
  isValid = FALSE; CConxGeomObj::setLine(o);
}

NF_INLINE
void CConxParabola::getPointOn(CConxPoint *LB, double computol) const
{
  if (!isValid) {
    Pt tLB, tRB;
    conxhm_p_getPstarted(&tLB, &tRB, getFocus().getPt(CONX_POINCARE_UHP),
                         getLine().getPUHP_A(), getLine().getPUHP_R(),
                         computol);
    assert(tLB.x == tRB.x && tLB.y == tRB.y);
    pLB.setPoint(tLB, CONX_POINCARE_UHP);
    isValid = TRUE;
  }

  if (LB == NULL) {
    LB = new CConxPoint(pLB); // DLC Remember to free this.
  } else {
    *LB = pLB;
  }
}

NF_INLINE
CConxHypEllipse &CConxHypEllipse::operator=(const CConxHypEllipse &o)
{
  MMM("CConxHypEllipse &operator=(const CConxHypEllipse &o)");
  (void) CConxGeomObj::operator=(o);
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
void CConxHypEllipse::getPointsOn(CConxPoint *lb, CConxPoint *rb) const
{
  if (!isValid) {
    Pt tLB, tRB;
    if (isEllipse()) {
      conxhm_p_getEstarted(&tLB, &tRB, getFocus1().getPt(CONX_POINCARE_UHP),
                           getFocus2().getPt(CONX_POINCARE_UHP), getScalar());
    } else {
      conxhm_p_getHstarted(&tLB, &tRB, getFocus1().getPt(CONX_POINCARE_UHP),
                           getFocus2().getPt(CONX_POINCARE_UHP), getScalar());
    }
    LB.setPoint(tLB, CONX_POINCARE_UHP);
    RB.setPoint(tRB, CONX_POINCARE_UHP);
    isValid = TRUE;
  }
  if (lb == NULL) {
    lb = new CConxPoint(LB); // DLC Remember to free this.
  } else {
    *lb = LB;
  }
  if (rb == NULL) {
    rb = new CConxPoint(RB); // DLC Remember to free this.
  } else {
    *rb = RB;
  }
}

NF_INLINE
int CConxHypEllipse::isEllipse() const
// Returns non-zero if this is an ellipse, zero if this is a hyperbola.
{
  return (getScalar() > conxp_distAB(getFocus1().getPt(CONX_POINCARE_UHP),
                                     getFocus2().getPt(CONX_POINCARE_UHP)));
}

PF_INLINE
ostream &CConxHypEllipse::printOn(ostream &o) const
{
  o << "CConxHypEllipse (scalar=" << getScalar() << ", focus 1=";
  getA().printOn(o);
  o << ", focus 2=";
  getB().printOn(o);
  o << ")";
  return o;
}

PF_INLINE
ostream &CConxHypEllipse::printOn(ostream &o, ConxModlType modl) const
{
  o << "CConxHypEllipse (scalar=" << getScalar() << ", focus 1=";
  getA().printOn(o, modl);
  o << ", focus 2=";
  getB().printOn(o, modl);
  o << ")";
  return o;
}

NF_INLINE
void CConxEqDistCurve::setLine(const CConxLine &L)
{
  isValid = FALSE;
  CConxGeomObj::setLine(L);
}

NF_INLINE
void CConxEqDistCurve::getPointsOn(CConxPoint *lb, CConxPoint *rb,
                                   double ymin, double ymax) const
{
  if (!isValid) {
    Pt tLB, tRB;
    conxhm_p_getEQstarted(&tLB, &tRB, getLine().getPUHP_A(),
                          getLine().getPUHP_R(), getScalar(), ymin, ymax);
    LB.setPoint(tLB, CONX_POINCARE_UHP);
    RB.setPoint(tRB, CONX_POINCARE_UHP);
    isValid = TRUE;
  }
  if (lb == NULL) {
    lb = new CConxPoint(LB); // DLC Remember to free this.
  } else {
    *lb = LB;
  }
  if (rb == NULL) {
    rb = new CConxPoint(RB); // DLC Remember to free this.
  } else {
    *rb = RB;
  }
}

NF_INLINE
CConxEqDistCurve &CConxEqDistCurve::operator=(const CConxEqDistCurve &o)
{
  MMM("CConxEqDistCurve &operator=(const CConxEqDistCurve &o)");
  (void) CConxGeomObj::operator=(o);
  uninitializedCopy(o);
  return *this;
}

PF_INLINE
ostream &CConxEqDistCurve::printOn(ostream &o) const
{
  o << "CConxEqDistCurve (line=";
  getLine().printOn(o);
  o << ", distance=" << getScalar() << ")";
  return o;
}

PF_INLINE
ostream &CConxEqDistCurve::printOn(ostream &o, ConxModlType modl) const
{
  o << "CConxEqDistCurve (line=";
  getLine().printOn(o, modl);
  o << ", distance=" << getScalar() << ")";
  return o;
}

CF_INLINE
CConxCircle::CConxCircle()
{
  MMM("CConxCircle()");
  setRadius(2.0);
}

CF_INLINE
CConxCircle::CConxCircle(const CConxPoint &center, double radius)
{
  MMM("CConxCircle()");
  setRadius(radius);
  setCenter(center);
}

CF_INLINE
CConxCircle::CConxCircle(const CConxCircle &o)
  : CConxGeomObj(o)
{
  MMM("CConxCircle(const CConxCircle &o)");
  // we have no non-CConxGeomObj attributes
}

PF_INLINE
CConxCircle &CConxCircle::operator=(const CConxCircle &o)
{
  MMM("CConxCircle &operator=(const CConxCircle &o)");
  (void) CConxGeomObj::operator=(o);
  // we have no non-CConxGeomObj attributes
  return *this;
}

NF_INLINE
void CConxCircle::setRadius(double r)
{
  if (r < MIN_RADIUS) r = MIN_RADIUS;
  setScalar(r);
}

PF_INLINE
int CConxCircle::operator==(const CConxCircle &o) const
{
  return (myequals(getRadius(), o.getRadius(), EQUALITY_TOL)
          && getCenter() == o.getCenter());
}

PF_INLINE
ostream &CConxCircle::printOn(ostream &o) const
{
  o << "CConxCircle (center=";
  getCenter().printOn(o);
  o << ", radius=" << getRadius() << ")";
  return o;
}

PF_INLINE
ostream &CConxCircle::printOn(ostream &o, ConxModlType modl) const
{
  o << "CConxCircle (center=";
  getCenter().printOn(o, modl);
  o << ", radius=" << getRadius() << ")";
  return o;
}

OOLTLTI_INLINE PBM_STREAM_OUTPUT_SHORTCUT(CConxPoint)
OOLTLTI_INLINE PBM_STREAM_OUTPUT_SHORTCUT(CConxTwoPts)
OOLTLTI_INLINE PBM_STREAM_OUTPUT_SHORTCUT(CConxGeomObj)
OOLTLTI_INLINE PBM_STREAM_OUTPUT_SHORTCUT(CConxCircle)
OOLTLTI_INLINE PBM_STREAM_OUTPUT_SHORTCUT(CConxLine)
OOLTLTI_INLINE PBM_STREAM_OUTPUT_SHORTCUT(CConxHypEllipse)
OOLTLTI_INLINE PBM_STREAM_OUTPUT_SHORTCUT(CConxEqDistCurve)
OOLTLTI_INLINE PBM_STREAM_OUTPUT_SHORTCUT(CConxParabola)
