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
  Implementation of C++ classes in `h_parabo.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hypmath.hh"
#include "h_parabo.hh"
#include "cassert.h"
#include "canvas.hh"

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
int CConxParabola::getPointOn(CConxPoint *LB, double computol) const
// Returns 0 on success, or 1 if the math doesn't quite work out, such as
// if the point or the line is not in the plane.
{
  if (!isValid) {
    Pt tLB, tRB;
    if (conxhm_p_getPstarted(&tLB, &tRB, getFocus().getPt(CONX_POINCARE_UHP),
                             getLine().getPUHP_A(), getLine().getPUHP_R(),
                             computol))
      return 1;
    assert(tLB.x == tRB.x && tLB.y == tRB.y);
    pLB.setPoint(tLB, CONX_POINCARE_UHP);
    isValid = TRUE;
  }

  if (LB == NULL) {
    LB = new CConxPoint(pLB); // DLC Remember to free this.
  } else {
    *LB = pLB;
  }
  return 0;
}

NF_INLINE
void CConxParabola::drawBresenhamOn(CConxCanvas &cv) const
{
  // DLC this is nearly the same for parabolas, hyp/ells, and eqdistcurves, so
  // implement only once!
  CConxPoint lb, rb; // DLC static for a slight speed increase.

  if (getPointOn(&lb)) return; // DLC should the user be able to find out
  // about this if she wants to?

  cv.drawByBresenham(lb, lb, definingFunctionWrapper, this);
}

NF_INLINE
void CConxParabola::drawGarnishOn(CConxCanvas &cv) const
{
  // DLC draw the line and the focus
}

NF_INLINE
void CConxParabola::uninitializedCopy(const CConxParabola &o)
{
  isValid = o.isValid;
  if (isValid)
    pLB = o.pLB;
}
