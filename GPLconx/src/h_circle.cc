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
  Implementation of C++ classes in `h_circle.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include "hypmath.hh"
#include "h_circle.hh"
#include "cassert.h"
#include "canvas.hh"

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

NF_INLINE
void CConxCircle::drawGarnishOn(CConxCanvas &cv) const
{
  // DLC draw the center and possibly some spokes.
}

NF_INLINE
void CConxCircle::drawBresenhamOn(CConxCanvas &cv) const
{
  ConxModlType modl = cv.getModel();

  // The Poincare UHP and Poincare Disk
  // circles are Euclidean circular, but their Euclidean center is not
  // the same as the true, hyperbolic center.  We convert and draw the
  // Euclidean circles.

  // DLC implement like we do in conx_quickdraw_circle()
  Pt uhpCenter = getCenter().getPt(CONX_POINCARE_UHP);
  double r = getRadius();
  Pt uhpEuclideanCenter = { uhpCenter.x, myabs(uhpCenter.y * cosh(r)) };
  // DLC explain this math in terms of the uhp distance metric.
  double uhpEuclideanRadius = myabs(uhpCenter.y * sinh(r));
  if (modl == CONX_POINCARE_UHP) {
    cv.drawCircle(uhpEuclideanCenter, uhpEuclideanRadius);
  } else {
    // The Klein disk circle is not Euclidean-circular, but the Poincare Disk
    // circle is. Luckily, it is roughly the same number of
    // pixels as the Euclidean-circular circle in the Poincare Disk,
    // so we can just find the
    // points that correspond to those pixels, convert them to Klein Disk
    // coordinates, and then draw them.  We might want to zoom in
    // (xmin, xmax, ymin, ymax) on the Poincare disk so that we draw a few
    // more points than we need, else we'll need to do
    // a line strip and the accuracy may not be as high as you'd like.

    // For now, we do the simpler trick of calculating the HG points on the
    // Poincare UHP circle via the Euclidean equations
    // ( x=uhpEuclideanCenter.x+uhpEuclideanRadius*cos(theta),
    //   y=uhpEuclideanCenter.y+uhpEuclideanRadius*sin(theta)  )
    // and then converting these uhp points to Klein disk or Poincare disk
    // points.  To avoid the zooming in and out and still get a good-looking
    // circle, we just use a small theta increment, smaller than is needed.

    // DLC for greater efficiency, calculate the Euclidean center and
    // radius of the Poincare Disk circle.  For now, we do what we do in
    // the Klein disk.

#define NUM_POINTS_PER_CIRCLE 1500.0
    // A 1500-gon is going to be drawn, which will look pretty much like
    // a smooth curve.

    double thetaIncr = M_PI * 2.0 / NUM_POINTS_PER_CIRCLE;
    cv.beginDraw(cv.LINE_STRIP);
    CConxPoint pointOnTheCircle;
    for (double theta = 0.0; theta < M_PI * 2.0; theta += thetaIncr) {
      pointOnTheCircle.setPoint( \
                         uhpEuclideanCenter.x+uhpEuclideanRadius*cos(theta), \
                         uhpEuclideanCenter.y+uhpEuclideanRadius*sin(theta), \
                         CONX_POINCARE_UHP);
      cv.drawVertex(pointOnTheCircle.getPt(modl));
    }
    cv.endDraw();
  }
}

