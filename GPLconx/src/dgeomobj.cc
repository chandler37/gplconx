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
  Implementation of C++ classes in `dgeomobj.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include "dgeomobj.hh"
#include "canvas.hh"

#define DRAW_SINGLE_LINE(cv, A, B) \
   do { \
           cv.beginDraw(CConxCanvas::LINES); \
           cv.drawVertex(A); \
           cv.drawVertex(B); \
           cv.endDraw(); \
   } while (0)


NF_INLINE
void CConxDwPoint::drawOn(CConxCanvas &cv) const throw(int)
{
  MMM("void drawOn(CConxCanvas &cv) const throw(int)");
  // How do you garnish a point??? `getGarnishing()' makes no difference.
  // The method makes no difference either.  If we did use a stored drawing,
  // we would inherit with protected access and then define our own
  // versions of setGarnishing and setDrawingMethod so that setValidity
  // would not be called.  (Other methods would have their access raised.)
  // This is necessary even with a virtual setGarnishing if you
  // assume the worst of the class's users, i.e. that they will say
  // ((CConxDwGeomObj *)&cconxdwpoint)->CConxDwGeomObj::setGarnishing(TRUE)
  // just to make us less efficient.

  // A stored drawing is too much overhead for as simple a thing as this.
  cv.setDrawingColor(getColor());
  cv.setPointSize(getThickness());
  cv.beginDraw(cv.POINTS);
  cv.drawVertex(P.getX(cv.getModel()), P.getY(cv.getModel()));
  cv.endDraw();

// For those that use SD's, setValidity(TRUE) if startSD did not throw by now.
}

NF_INLINE
void CConxDwLine::drawOn(CConxCanvas &cv) const throw(int)
{
  ConxModlType modl = cv.getModel();

  // A stored drawing is too much overhead for as simple a thing as this.
  // Also, we use only one method, since the lines are Euclidean arcs or lines.
  cv.setDrawingColor(getColor());
  // DLC  cv.setLineSize(getThickness());
  
  if (modl == CONX_KLEIN_DISK) {
    if (L.isLineSegment()) {
      DRAW_SINGLE_LINE(cv, L.getA().getPt(modl), L.getB().getPt(modl));
    } else {
      // Draw a line across the unit circle.
      DRAW_SINGLE_LINE(cv, L.getKleinEndPoint1(), L.getKleinEndPoint2());
    }
  } else {
    // Draw a circle for most lines, a straight line for the rare few.
    Pt cc; // the circle's center (in most cases)
    double circleRadius;
    if (modl == CONX_POINCARE_UHP) {
      if (L.isTypeI()) {
        if (L.isLineSegment()) {
          DRAW_SINGLE_LINE(cv, L.getA().getPt(modl), L.getB().getPt(modl));
        } else {
          double midx = (L.getA().getX(modl) + L.getB().getX(modl)) / 2.0;
          // DLC could clip
          Pt p1, p2;
          p1.x = p2.x = midx;
          p1.y = greater(0.0, cv.getXmin());
          p2.y = cv.getXmax();
          DRAW_SINGLE_LINE(cv, p1, p2);
        }
      } else {
        cc.x = L.getPUHP_A();
        cc.y = 0.0;
        circleRadius = L.getPUHP_R();
        if (L.isLineSegment()) {
          // from conxp_graphsegment()
          Pt A = L.getA().getPt(modl);
          Pt B = L.getB().getPt(modl);
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
      cc.x = L.getPD_Cx();
      cc.y = L.getPD_Cy();
      circleRadius = L.getPD_R(); // DLC test to see if we should just
                                  // draw a point because it's too small.
      if (circleRadius == DIAMETER) {
        // Draw a diameter, i.e. a line with
        // slope m = cc.y/cc.x and y-intercept 0
        if (L.isLineSegment()) {
          // Just draw a straight line between the two points.
          DRAW_SINGLE_LINE(cv, L.getA().getPt(modl), L.getB().getPt(modl));
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

        if (L.isLineSegment()) {
          oc1 = L.getA().getPt(modl);
          oc2 = L.getB().getPt(modl);
        } else {
          oc1 = L.getPoincareDiskEndPoint1();
          oc2 = L.getPoincareDiskEndPoint2();
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

// For those that use SD's, setValidity(TRUE) if startSD did not throw by now.
}

NF_INLINE
void CConxDwCircle::drawOn(CConxCanvas &cv) const throw(int)
// DLC you can get modl from cv!! At least assert.
{
  cv.setDrawingColor(getColor());
  cv.setPointSize(getThickness());
  switch (getDrawingMethod()) {
  case LONGWAY: longwayDrawOn(cv); break;
  default: smartDrawOn(cv); break;
  }
}

NF_INLINE
double CConxDwCircle::longwayMetric(Pt x, void *t)
// We need this to send to the longway method, but it must use stored
// information.
{
  static CConxPoint sPoint;
  sPoint.setPoint(x, ((CConxDwCircle *)t)->getLongwaySavedModel());
  return (((CConxDwCircle *)t)->getCircle()).definingFunction(sPoint);
}

NF_INLINE
void CConxDwCircle::longwayDrawOn(CConxCanvas &cv) const throw(int)
{
  saveLongwayModel(cv.getModel());
  drawLongway(cv, CConxDwCircle::longwayMetric);
}

NF_INLINE
void CConxDwCircle::smartDrawOn(CConxCanvas &cv) const throw(int)
{
  // DLC implement
  ConxModlType modl = cv.getModel();

  // The Poincare UHP and Poincare Disk
  // circles are Euclidean circular, but their Euclidean center is not
  // the same as the true, hyperbolic center.  We convert and draw the
  // Euclidean circles.

  // DLC implement like we do in conx_quickdraw_circle()
  Pt uhpCenter = L.getCenter().getPt(CONX_POINCARE_UHP);
  double r = L.getRadius();
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

NF_INLINE
void CConxDwGeomObj::longwayDrawVertex(double a, double b, void *t)
{
  (((CConxDwGeomObj *)t)->getStoredCanvas())->drawVertex(a, b);
}

NF_INLINE
void CConxDwGeomObj::drawLongway(CConxCanvas &cv,
                                 ConxMetric *test) const
  // DLC modl is not needed!
{
  // DLC  CONX_BEGIN_DISP_LIST(dl);
  cv.beginDraw(cv.POINTS);
  storeCanvas(&cv);
#ifdef HAVE_CONST_CAST
  CConxDwGeomObj *constlessThis = const_cast< CConxDwGeomObj * >( this );
#else
  CConxDwGeomObj *constlessThis = this;
#endif
  conx_longway(test, constlessThis, cv.getModel(), getLongwayTolerance(),
               cv.getPixelWidth(), cv.getPixelHeight(),
               cv.getXmin(), cv.getXmax(), cv.getYmin(), cv.getYmax(),
               CConxDwGeomObj::longwayDrawVertex, constlessThis);
  removeStoredCanvas();
  cv.endDraw();
  // DLC  CONX_END_DISP_LIST(dl);
}

OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxDwGeomObj)
OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxArtist)
