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
  Implementation of C++ classes in `h_eqdist.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hypmath.hh"
#include "h_eqdist.hh"
#include "cassert.h"
#include "canvas.hh"

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

NF_INLINE
void CConxEqDistCurve::drawGarnishOn(CConxCanvas &cv) const
{
  // DLC draw the line and possibly some perpendicular segments?
}

NF_INLINE
void CConxEqDistCurve::drawBresenhamOn(CConxCanvas &cv) const
{
  CConxPoint lb, rb; // DLC static for a slight speed increase.
  getPointsOn(&lb, &rb, cv.getYmin(), cv.getYmax());
  cv.drawByBresenham(lb, rb, definingFunctionWrapper, this);
}

NF_INLINE
void CConxEqDistCurve::uninitializedCopy(const CConxEqDistCurve &o)
{
  isValid = o.isValid;
  if (isValid) {
    LB = o.LB;
    RB = o.RB;
  }
}

