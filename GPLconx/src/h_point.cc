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
  Implementation of C++ classes in `h_point.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hypmath.hh"
#include "h_point.hh"
#include "cassert.h"
#include "canvas.hh"

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
void CConxPoint::modelToModel(double xFrom, double yFrom, ConxModlType mFrom,
                              double *xTo, double *yTo, ConxModlType mTo)
// Convert from (xFrom, yFrom) in the mFrom model to
// (*xTo, *yTo) in the mTo model.
{
  if (mFrom == CONX_POINCARE_UHP) {
    if (mTo == CONX_POINCARE_UHP) {
      *xTo = xFrom;
      *yTo = yFrom;
    } else if (mTo == CONX_POINCARE_DISK) {
      conxhm_ptopd(xFrom, yFrom, xTo, yTo);
    } else {
      assert(mTo == CONX_KLEIN_DISK);
      conxhm_ptok(xFrom, yFrom, xTo, yTo);
    }
  } else if (mFrom == CONX_POINCARE_DISK) {
    if (mTo == CONX_POINCARE_UHP) {
      conxhm_pdtop(xFrom, yFrom, xTo, yTo);
    } else if (mTo == CONX_POINCARE_DISK) {
      *xTo = xFrom;
      *yTo = yFrom;
    } else {
      assert(mTo == CONX_KLEIN_DISK);
      conxhm_pdtok(xFrom, yFrom, xTo, yTo);
    }
  } else {
    assert(mFrom == CONX_KLEIN_DISK);
    if (mTo == CONX_POINCARE_UHP) {
      conxhm_ktop(xFrom, yFrom, xTo, yTo);
    } else if (mTo == CONX_POINCARE_DISK) {
      conxhm_ktopd(xFrom, yFrom, xTo, yTo);
    } else {
      assert(mTo == CONX_KLEIN_DISK);
      *xTo = xFrom;
      *yTo = yFrom;
    }
  }
}

NF_INLINE
void CConxPoint::convertTo(ConxModlType modl) const
{
  CONX_INVARIANT(isValid != 0);
  for (int i = 0; i < CONX_NUM_MODELS; i++) {
    if (isValid & CONX_MODEL2BIT(i)) {
      // Convert from model i to model modl.
      modelToModel(x[i], y[i], (ConxModlType) i,
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

NF_INLINE
void CConxPoint::drawGarnishOn(CConxCanvas &cv) const
{
  // do nothing
}

NF_INLINE
void CConxPoint::drawBresenhamOn(CConxCanvas &cv) const
{
  cv.beginDraw(cv.POINTS);
  cv.drawVertex(getX(cv.getModel()), getY(cv.getModel()));
  cv.endDraw();
}

NF_INLINE
void CConxPoint::uninitializedCopy(const CConxPoint &o)
{
  isValid = o.isValid;
  for (int i = 0; i < CONX_NUM_MODELS; i++) {
    x[i] = o.x[i];
    y[i] = o.y[i];
  }
}

