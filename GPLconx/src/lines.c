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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "point.h"
#include "viewer.h"
#include "util.h"
#include "gl.h"

static void conxp_graphIsegment(Pt A, Pt B);
static void conxp_graphI(double Ax, double y_min, double y_max);
static void conxk_graphline(double a, double b, double c, double d);
static void conxk_graphsegment(double a, double b, double c, double d);

/*
   It is hereby ordered that a vertical line shall have b==1024==KINFINITY
   and -1<m<1 where m is the x coordinate of the vertical line.
*/
void conxk_graphmb(double m, double b)
/* Draw a Klein disk line y=m*x+b.  In a Euclidean sense, this draws the
   segment of that line inside the unit circle.
*/
{
  Pt A, B;
  conxhm_getendptsmb(m, b, &A, &B);
  LOGGG6(LOGG_FULL, "\nconxk_graphmb %f %f m b %f %f A %f %f B\n", m, b,
         A.x, A.y, B.x, B.y);
  blue();
  conx_gl_draw_lineAB(A, B);
  green();
}

void conxk_graphline(double a, double b, double c, double d)
/* Draw a Klein disk line between Klein points (a, b) and (c, d). */
{
  double m, e;
  if (myabs(c-a)<=VERTICALNESS) {
    e=b=KINFINITY; /* DLC */
    m=(a+c)/2;
  } else {
    m=(d-b)/(c-a);
    e=b-m*a;
  }
  conxk_graphmb(m, e);
}

void conxk_graphlineAB(Pt A, Pt B)
/* Draw a Klein disk line between Klein points A and B. */
{
  conxk_graphline(A.x,A.y,B.x,B.y);
}

void conxk_graphsegment(double a, double b, double c, double d)
/* Draw a Klein disk line segment between Klein points (a, b) and (c, d). */
{
  blue();
  conx_gl_draw_line(a, b, c, d);
  green();
}

void conxp_graphIsegment(Pt A, Pt B)
/* Graph a Type I line segment between Poincare UHP points A and B. */
{
  blue();
  conx_gl_draw_lineAB(A, B);
  green();
}

/* DLC -- new renderer should do all GL_LINES at once as an optimization. */

void conxp_graphI(double Ax, double y_min, double y_max)
{
  blue();
  conx_gl_draw_line(Ax, y_min, Ax, y_max);
  green();
}

void conxp_graphsegment(Pt A, Pt B, double tstp)
/* Graph a line segment between Poincare UHP points A and B. */
{
  double a, r, min, max, ax;
  conxp_geteq(A, B, &a, &r);
  if (r==TYPEI) {
    conxp_graphIsegment(A, B);
  } else {
    min=asin(B.y/r); /* B.y=r*sin(min) */
    max=asin(A.y/r);
    ax=a+r*cos(min);
    if ((myabs(ax-A.x)>VERTICALNESS)&&(myabs(ax-B.x)>VERTICALNESS))
      min=M_PI-min;
    ax=a+r*cos(max);
    if ((myabs(ax-A.x)>VERTICALNESS)&&(myabs(ax-B.x)>VERTICALNESS))
      max=M_PI-max;
    if (myabs(min-max)<VERTICALNESS)
      min=M_PI-min;
    swap(&min, &max);
    blue();
    conx_gl_draw_arc(a, 0.0, r, min, max, tstp, NULL, -1);
    green();
  }
}

void conxp_graphar(double a, double r, double tstp, double y_min, double y_max)
/* Graph a line in the Poincare UHP.  If it is a Type I line, just draw
   from (a, y_min) to (a, y_max).  If it is a Type II line, use tstp.
*/
{
  if (r==TYPEI) {
    conxp_graphI(a, y_min, y_max);
  } else {
    blue();
    conx_gl_draw_arc(a, 0.0, r, 0.0, M_PI, tstp, NULL, -1);
    green();
  }
}

void conxp_graphlineAB(Pt A, Pt B, double tstp, double y_min, double y_max)
{
  double a, r;
  conxp_geteq(A, B, &a, &r);
  conxp_graphar(a, r, tstp, y_min, y_max);
}

void conxpd_graphlineAB(Pt f1, Pt f2, double tstp)
{
  double rad;
  Pt c;
  conxpd_getCenterAndRadiusABC(f1, f2, &c, &rad);
  if (rad!=DIAMETER)
    conxpd_graphc(c.x, c.y, tstp);
  else      /* m=cx */
    conxk_graphmb(c.y/c.x, 0);
}

void conxpd_graphc(double cx, double cy, double tstp)
{
  double t1, t2, rad;
  Pt A, B;
  if (sqr(cx)+sqr(cy)<=1) {
    LOGGG2(LOGG_FULL, "I don't know... %f %f\n", cy, cx);
    if (myabs(cx)<=VERTICALNESS) {
      conxk_graphmb(cx, KINFINITY);
    } else {
      conxk_graphmb(cy/cx, 0);
    }
  } else {
    rad=conxpd_getr(cx, cy);
    conxhm_getendptsc(cx, cy, &A, &B); /* A.x=C.x+rad*cos(t1) */
    if (A.y<cy) {
      t1=2.0*M_PI-acos((A.x-cx)/rad);
    } else {
      t1=acos((A.x-cx)/rad);
    }
    LOGGG1(LOGG_FULL, "\nt1= %f ", t1);
    if (B.y<cy) {
      t2=2.0*M_PI-acos((B.x-cx)/rad);
    } else {
      t2=acos((B.x-cx)/rad);
    }
    LOGGG1(LOGG_FULL, "\nt2= %f ", t2);
    if (t1>t2) {
      swap(&t1, &t2);
    }
    if (t2-t1>M_PI) {
      t1+=2.0*M_PI;
      swap(&t1, &t2);
    }
    LOGGG6(LOGG_FULL, "\n t1 t2 %f %f a  %f %f b %f %f ",
           t1, t2, A.x, A.y, B.x, B.y);
    LOGGG5(LOGG_FULL, "acos %f %f c %f %f rad %f",
           acos((B.x-cx)/rad), acos((A.x-cx)/rad), cx, cy, rad);
    blue();
    conx_gl_draw_arc(cx, cy, rad, t1, t2, tstp, NULL, -1);
    green();
  }
}

void conxpd_graphsegment(Pt A, Pt B, double tstp)
{
  double t1, t2, r;
  Pt C;
  blue();
  conxpd_getCenterAndRadiusABC(A, B, &C, &r);
  /*cx+r*cos(t1)=A.x;
    cx+r*cos(t2)=B.x; */
  if (r!=DIAMETER) {
    if (A.y<C.y) {
      t1=2.0*M_PI-acos((A.x-C.x)/r);
    } else {
      t1=acos((A.x-C.x)/r);
    }
    if (B.y<C.y) {
      t2=2.0*M_PI-acos((B.x-C.x)/r);
    } else {
      t2=acos((B.x-C.x)/r);
    }
    if (t1>t2) {
      swap(&t1, &t2);
    }
    LOGGG2(LOGG_FULL, "\n t1= %f t2= %f", t1, t2);
    conx_gl_draw_arc(C.x, C.y, r, t1, t2, tstp, NULL, -1);
  } else {
    conxk_graphsegmentAB(A, B);
  }
  green();
  FLUSH();
}

void conxk_graphsegmentAB(Pt A, Pt B)
/* Draw a Klein disk line segment between Klein points A and B. */
{
  conxk_graphsegment(A.x, A.y, B.x, B.y);
}

void conx_quickdraw_circle(Pt A, double r, ConxModlType typ, double tstp)
/* Use non-obvious algebra to draw a Circle the short way in the Poincare UHP
   and Poincare Disk models, where a circle is the same shape as a Euclidean
   circle as far as viz goes.  Klein Disk circles do not have this property,
   so they are drawn in a more computationally-intensive way.

   DLC we could draw the Poincare disk circle from straight math, right?
   Why bother with converting from pd2puhp ???
*/
/*******************************
  This is the short, efficient, hard-to-verify method to graph a circle.   For
  this method to work, every point that satisfies
  (x-xec)^2+(y-yec)^2=re^2 [1]
  must also satisfy
  distAB((x,y),(A.x,A.y))=r [2]
  where xec,yec,and re are defined as below.  (coming from the relation that
  distance along a type I line is log(A.y/B.y))
  
  First, [1] becomes
  (x-xec)^2+(y-yec)^2=(A.y*exp(r)-A.y*exp(-r))^2 / 4
 
x^2-2*x*(A.x)+(A.x)^2+y^2-2*y*(yec)+yec^2=(A.y*exp(r)-A.y*exp(-r)
)^2 / 4
  (x-A.x)^2=((A.y*exp(r)-A.y*exp(-r))^2-(y-yec)^2) /4
  I think Maple is called for.  I'll just do it later.
  ************************************/
{
  double cx, cy, rr;
  ConxPoint2DConverterFunc *cvrt;
  
  if (typ==CONX_POINCARE_DISK) {
    conxhm_pdtopAB(A, &A);
  }
  if (typ==CONX_KLEIN_DISK) {
    conxhm_ktopAB(A, &A);
  }
  cx=A.x;
  cy=myabs(A.y*cosh(r));
  rr=myabs(A.y*sinh(r));
  ccol();
  if (typ==CONX_POINCARE_UHP) {
    cvrt = NULL;
  } else if (typ==CONX_POINCARE_DISK) {
    cvrt = conxhm_ptopd;
  } else {
    cvrt = conxhm_ptok;
  }
  conx_gl_draw_arc(cx, cy, rr, 0.0, 2.0*M_PI, tstp, cvrt, -1);
  green();
}

void conx_draw_arc(double x, double y, double r, double theta_1,
                   double theta_2, double tstp,
                   ConxPoint2DConverterFunc *converter,
                   ConxPointFunc *pfunc, void *pArg)
/*
@conxdox
@deftypefn void conx_draw_arc (double @var{x}, double @var{y}, double @var{r}, double @var{theta_1}, double @var{theta_2}, double @var{tstp}, ConxPoint2DConverterFunc *@var{converter})

``Draws'' an arc of a Euclidean circle centered at
@Conx_2D_Coord{@var{x}, @var{y}}
with (Euclidean) radius @var{r}.  The arc begins at @Conx_Theta{} @equiv{}
@var{theta_1}
and ends at @Conx_Theta{} @equiv{} @var{theta_2}, and a point is ``drawn''
by calling @var{pfunc} every
@var{tstp} radians.  Before this point is drawn, and after it is calculated
from the sine and cosine, it is converted via @var{converter} if
@var{converter} is not @Conx_NULL{}.  This means that you can draw a
hyperbolic geometry circle in the Klein disk, which will not be a Euclidean
circle of any center or radius, from a Poincare Disk circle (which has a
Euclidean center and radius).

The points should be connected by lines, so a low tstp will create something
resembling a stop sign instead of a circle.
@end deftypefn
@end conxdox
*/
{
  double t;

  /* DLC use gluCircle wherever possible as an option. */

  /* Two for loops are necessary to avoid numerous tests of the `if'
     statement.
  */
  if (converter != NULL) {
    for (t=theta_1; t<=theta_2; t+=tstp) {
      double px, py;
      (*converter)(x+r*cos(t), y+r*sin(t), &px, &py);
      (*pfunc)(px, py, pArg);
    }
  } else {
    for (t=theta_1; t<=theta_2; t+=tstp) {
      (*pfunc)(x+r*cos(t), y+r*sin(t), pArg);
    }
  }
}

