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
  Implementation of C++ classes in `h_hypell.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hypmath.hh"
#include "h_hypell.hh"
#include "cassert.h"
#include "canvas.hh"

NF_INLINE
double CConxHypEllipse::definingFunction(const CConxPoint &X) const
{
  return (isEllipse()
          ? (getFocus1().distanceFrom(X)
             + getFocus2().distanceFrom(X) - getScalar())
          : (myabs(getFocus1().distanceFrom(X)
                   - getFocus2().distanceFrom(X)) - getScalar()));
}

NF_INLINE
void CConxHypEllipse::init()
{
  isValid = FALSE;
  isEllipseIsValid = FALSE;

  isEllips = FALSE;
  // This is not really necessary, but we don't want to copy an
  // uninitialized field.
}

NF_INLINE
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

NF_INLINE
void CConxHypEllipse::drawBresenhamOn(CConxCanvas &cv) const
{
  // DLC this is the same for parabolas, hyp/ells, and eqdistcurves, so
  // implement only once!
  CConxPoint lb, rb; // DLC static for a slight speed increase.
  getPointsOn(&lb, &rb);
  cv.drawByBresenham(lb, rb, definingFunctionWrapper, this);
}

NF_INLINE
void CConxHypEllipse::drawGarnishOn(CConxCanvas &cv) const
{
  // DLC draw the foci (and the line between them?)
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
Boole CConxHypEllipse::isEllipse() const
// Returns TRUE if this is an ellipse, FALSE if this is a hyperbola.
{
  if (!isEllipseIsValid) {
    isEllips = (getScalar()
                  > conxp_distAB(getFocus1().getPt(CONX_POINCARE_UHP),
                                 getFocus2().getPt(CONX_POINCARE_UHP)));
  }
  return isEllips;
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

