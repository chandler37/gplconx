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
  Implementation of C++ classes in `h_line.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hypmath.hh"
#include "h_line.hh"
#include "cassert.h"
#include "canvas.hh"

NF_INLINE
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

NF_INLINE
void CConxLine::drawGarnishOn(CConxCanvas &cv) const
{
  // DLC draw the points.
}

NF_INLINE
void CConxLine::drawBresenhamOn(CConxCanvas &cv) const
{
  // We use only one method, since the lines are Euclidean arcs or lines.
  ConxModlType modl = cv.getModel();

  if (modl == CONX_KLEIN_DISK) {
    if (isLineSegment()) {
      DRAW_SINGLE_LINE(cv, getA().getPt(modl), getB().getPt(modl));
    } else {
      // Draw a line across the unit circle.
      DRAW_SINGLE_LINE(cv, getKleinEndPoint1(), getKleinEndPoint2());
    }
  } else {
    // Draw a circle for most lines, a straight line for the rare few.
    Pt cc; // the circle's center (in most cases)
    double circleRadius;
    if (modl == CONX_POINCARE_UHP) {
      if (isTypeI()) {
        if (isLineSegment()) {
          DRAW_SINGLE_LINE(cv, getA().getPt(modl), getB().getPt(modl));
        } else {
          double midx = (getA().getX(modl) + getB().getX(modl)) / 2.0;
          // DLC could clip
          Pt p1, p2;
          p1.x = p2.x = midx;
          p1.y = greater(0.0, cv.getXmin());
          p2.y = cv.getXmax();
          DRAW_SINGLE_LINE(cv, p1, p2);
        }
      } else {
        cc.x = getPUHP_A();
        cc.y = 0.0;
        circleRadius = getPUHP_R();
        if (isLineSegment()) {
          // from conxp_graphsegment()
          Pt A = getA().getPt(modl);
          Pt B = getB().getPt(modl);
          double min, max, ax;
          double r = circleRadius;
          min=asin(B.y/r); /* B.y=r*sin(min) */
          max=asin(A.y/r);
          ax=cc.x+r*cos(min);
          if ((myabs(ax-A.x)>VERTICALNESS)&&(myabs(ax-B.x)>VERTICALNESS))
            min=M_PI-min;
          ax=cc.x+r*cos(max);
          if ((myabs(ax-A.x)>VERTICALNESS)&&(myabs(ax-B.x)>VERTICALNESS))
            max=M_PI-max;
          if (myabs(min-max)<VERTICALNESS)
            min=M_PI-min;
          swap(&min, &max);
          cv.drawArc(cc, circleRadius, min, max);
        } else {
          // Draw a semicircle in the upper half plane
          cv.drawTopSemiCircle(cc, circleRadius);
        }
      }
    } else {
      assert(modl == CONX_POINCARE_DISK);
      cc.x = getPD_Cx();
      cc.y = getPD_Cy();
      circleRadius = getPD_R(); // DLC test to see if we should just
                                  // draw a point because it's too small.
      if (circleRadius == DIAMETER) {
        // Draw a diameter, i.e. a line with
        // slope m = cc.y/cc.x and y-intercept 0
        if (isLineSegment()) {
          // Just draw a straight line between the two points.
          DRAW_SINGLE_LINE(cv, getA().getPt(modl), getB().getPt(modl));
        } else {
          Pt endPt1, endPt2;
          // Draw a straight line across the unit circle.  We could use
          // existing math functionality, but getKleinEndPoint?'s calculation
          // and conversion is a lot of overhead.

          /*
            y = cc.y/cc.x * x                // because this is a diameter
            y^2 = 1-x^2
            (cc.y^2/cc.x^2 * x^2) + x^2 = 1
            x^2 = 1 / (1+(cc.y/cc.x)^2)
          */

          endPt1.x = sqrt(1.0 / (1.0+sqr(cc.y/cc.x)));
          endPt2.x = -endPt1.x;
          endPt1.y = sqrt(1.0-sqr(endPt1.x));
          endPt2.y = sqrt(1.0-sqr(endPt2.x));
          DRAW_SINGLE_LINE(cv, endPt1, endPt2);
        }
      } else {
        // Draw an arc of the circle orthogonal to the unit circle.
        double t1, t2;
        Pt oc1, oc2;

        if (isLineSegment()) {
          oc1 = getA().getPt(modl);
          oc2 = getB().getPt(modl);
        } else {
          oc1 = getPoincareDiskEndPoint1();
          oc2 = getPoincareDiskEndPoint2();
        }

        // from conxpd_graphc():
        if (oc1.y<cc.y) {
          t1=2.0*M_PI-acos((oc1.x-cc.x)/circleRadius);
        } else {
          t1=acos((oc1.x-cc.x)/circleRadius);
        }
        if (oc2.y<cc.y) {
          t2=2.0*M_PI-acos((oc2.x-cc.x)/circleRadius);
        } else {
          t2=acos((oc2.x-cc.x)/circleRadius);
        }
        if (t1>t2) {
          swap(&t1, &t2);
        }
        if (t2-t1>M_PI) {
          t1+=2.0*M_PI;
          swap(&t1, &t2);
        }
        cv.drawArc(cc, circleRadius, t1, t2);
      }
    } // pd treatment
  } // uhp and pd treatment
}

