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
   Functions to aid in the mathematics of hyperbolic geometry.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "viewer.h"
#include "util.h"

#ifndef HAVE_ACOSH
extern double acosh(double);
#endif

inline static void conx_swapAB(Pt *X, Pt *Y);
inline static double yarx(double a, double r, double x);
static void conxhm_getCircleIntersectionnotvert(double a, double r, double c,
                                                double d, double s, Pt *A,
                                                Pt *B);
static void conxhm_getCircleIntersection(double a, double r, double c,
                                         double d, double s, Pt *A, Pt *B);



void conx_swapAB(Pt *X, Pt *Y)
{
  Pt T;
  assert(X != NULL); assert(Y != NULL);
  T=*X;
  *X=*Y;
  *Y=T;
}

int conx_pt_not_between(Pt A, Pt B, Pt C)
/* Assumes colinearity of All three.  Answers the question,
   ``Is B between A and C on the line through all three?''

   DLC Does this only work in the Poincare UHP???
 */
{
  if (myabs(A.x-C.x) < VERTICALNESS) {
    if (A.y > C.y)
      conx_swapAB(&A, &C);
    return ((B.y > A.y) && (B.y < C.y)) ? 0 : 1;
  } else {
    if (A.x > C.x)
      conx_swapAB(&A, &C);
    return ((B.x > A.x) && (B.x < C.x)) ? 0 : 1;
  }
}

void conxp_geteq(Pt A, Pt B, double *a, double *r)
/* Find the Poincare UHP line that contains points A and B.  In hyperbolic
   geometry, this is a unique line.  Visualized, it appears to be a semicircle
   in the upper half plane (quadrants I and II) that has its center at (a, 0)
   and has a radius r.  When A and B have the same X coordinate, then the circle
   has infinite radius.  A negative radius indicates this in our model; specifically,
   r will be set to TYPEI in such a case.  This distinction is further complicated
   by the notion of equality in machine arithmetic.

   (x-a)^2+(y-0)^2=r^2  =>  x^2+y^2-r^2 = 2(1-a)  =>  a=1+(x^2+y^2-r^2)/2;

   (A.x-a)^2+(A.y-0)^2=r^2=(B.x-a)^2+(B.y-0)^2
           =>
   (A.x)^2-2a(A.x)+a^2+(A.y)^2=(B.x)^2-2a(B.x)+a^2+(B.y)^2
           =>
   (A.x)^2-2a(A.x)+(A.y)^2=(B.x)^2-2a(B.x)+(B.y)^2
           =>
   (A.x)^2-(B.x)^2-2a(A.x-B.x)+(A.y)^2-(B.y)^2=0
           =>
   (A.x)^2-(B.x)^2+(A.y)^2-(B.y)^2=2a(A.x-B.x)
           =>
   a=((A.x)^2-(B.x)^2+(A.y)^2-(B.y)^2)/(2(A.x-B.x))     * Thus we must have A.x != B.x
           =>
   a=((B.x)^2-(A.y)^2+(B.y)^2-(A.x)^2)/(2(B.x-A.x))
           =>
   a=((B.y)^2-(A.y)^2)/(2(B.x-A.x))+((B.x)^2-(A.x)^2)/(2(B.x-A.x))
           =>
   a=((B.y)^2-(A.y)^2)/(2(B.x-A.x))+(B.x+A.x)/2

   Note that this final form has nicer numerical properties and is quicker to
   evaluate.
*/
{
  assert(a != NULL); assert(r != NULL);
  if (myabs(B.x-A.x)>VERTICALNESS) {
    *a=(sqr(B.y)-sqr(A.y)) / (2.0*(B.x-A.x)) + (A.x+B.x)/2.0;
    *r=sqrt(sqr(A.x-*a)+sqr(A.y));
  } else {
    *a=(A.x+B.x)/2.0;
    *r=TYPEI; /* signalling typeIeq */
  }
}

double conxpd_getr(double cx, double cy)
/* In the Poincare disk, a line is an arc of a circle that is
   orthogonal to the disk's unit circle, i.e. the tangents at the
   points of intersection are orthogonal.  For any two points in the
   Poincare disk that do not lie colinear on a diameter of the unit
   circle, you will find only one such orthogonal circle, whose center
   point (cx, cy) determines its radius.

   For the unit circle, we have `x^2+y^2=1'.  For the Poincare disk ``line'',
   we have `(x-cx)^2+(y-cy)^2=r^2' => `x^2+y^2+cx^2+cy^2-2x(cx)-2y(cy)=r^2'.
   Why continue along this avenue, though, when we have a right triangle at
   the ready?  It has one vertex at (0, 0), another at (cx, cy), and the third
   on the unit circle where the two circles intersect.

   In fact, there are two of these triangles, but they are
   symmetric and have a common side, the hypotenuse.  That side's length is the
   distance between the (0, 0) and (cx, cy), but from the Pythagorean theorem
   it is also sqrt(1^2+r^2).  Thus `1^2+r^2=cx^2+cy^2', and we have
   `r=sqrt(cx^2+cy^2-1)'.

   If the two points that uniquely define the line are colinear on a
   diameter of the unit circle, then they lie on one of two circles of
   infinite radius.  We use a negative radius to represent this.  This
   corresponds to a vertical line in the Poincare UHP.
*/
{
  double sum;
  if ((sum=sqr(cx)+sqr(cy)-1.0)>=0.0) {
    return sqrt(sum);
  } else {
    return DIAMETER;
  }
}

double conxp_distAB(Pt A, Pt B)
{
  Pt K1, K2;
  
  conxhm_ptokAB(A, &K1);
  conxhm_ptokAB(B, &K2);
  return conxk_distAB(K1, K2);
}

double conxk_dist(double x, double y, double xx, double yy)
/* See Gunn. */
{
  return myabs(acosh(myabs(x*xx+y*yy-1.0) \
                     / sqrt(myabs((sqr(x)+sqr(y)-1.0) \
                                  * (sqr(xx)+sqr(yy)-1.0)))));
}

double conxk_distAB(Pt A, Pt B)
{
  return conxk_dist(A.x, A.y, B.x, B.y);
}

double conxpd_distAB(Pt A, Pt B)
{
  Pt K, P;

  conxhm_pdtokAB(A, &K);
  conxhm_pdtokAB(B, &P);
  return conxk_distAB(K, P);
}

double yarx(double a, double r, double x)
/*
  If we have a Poincare UHP line, i.e. a semicircle `(x-a)^2+y^2=r^2',
  then for a given x we have only one y (the solution in the UHP).
  This returns the y coordinate that corresponds to a given x.
 */
{
  return sqrt(myabs((r-(x-a))*(r+(x-a))));
}

void conxk_getPtNearXonmb(Pt X, double m, double b, Pt *A, double computol)
{
  double t, d1, d2, d3, d4;
  Pt left, right, midl, midr;
  int vert;
  
  assert(A != NULL);

  /* begin aggressive search x=a+r*cos(theta) */
  conxhm_getendptsmb(m, b, &left, &right);
  LOGGG4(LOGG_FULL, "\nendpts %f %f %f %f", left.x, left.y, right.x, right.y);
  vert=((b==KINFINITY)&&(m>-1.0)&&(m<1.0));
  if (vert) {
    if (left.y>right.y)
      conx_swapAB(&left, &right);
    left.x=left.y+computol;
    right.x=right.y-computol;
    left.y=right.y=m;
  } else {
    if (left.x>right.x)
      conx_swapAB(&left, &right);
    left.x+=computol;
    right.x-=computol;
    left.y=m*left.x+b;
    right.y=m*right.x+b;
  }
  t=(right.x-left.x)/3.0;
  midl.x=left.x+t;
  midr.x=right.x-t;
  if (!vert) {
    midr.y=m*midr.x+b;  
    midl.y=m*midl.x+b;
  } else {
    midr.y = midl.y = m;
  }
  d1=conxk_distAB(left, X);
  d2=conxk_distAB(midl, X);
  d3=conxk_distAB(midr, X);
  d4=conxk_distAB(right, X);
  do {
    if ((d1<=d2)&&(d1<=d3)&&(d1<=d4)) {
      right=midl;
      d4=d2;
    } else if ((d2<=d1)&&(d2<=d3)&&(d2<=d4)) {
      right=midr;
      d4=d3;
    } else if ((d3<=d1)&&(d3<=d2)&&(d3<=d4)) {
      left=midl;
      d1=d2;
    } else  {
      left=midr;
      d1=d3;
    }
    t=(right.x-left.x)/3.0;
    midl.x=left.x+t;
    midr.x=right.x-t;
    if (!vert) {
      midr.y=m*midr.x+b;
      midl.y=m*midl.x+b;
    }
    
    d2=conxk_distAB(midl, X); 
    d3=conxk_distAB(midr, X);
  } while (myabs(t)>computol);

  A->x=(left.x+right.x)/2.0;

  if (vert) {
    A->y=A->x;
    A->x=m;
  } else {
    A->y=m*A->x+b;
  }
}

double conxk_distFrommbX(double m, double b, Pt X, double computol)
{
  Pt Near;

  conxk_getPtNearXonmb(X, m, b, &Near, computol);
  return conxk_distAB(Near, X);
}

void conxhm_getendptsc(double cx, double cy, Pt *enda, Pt *endb)
/* For the Poincare disk line defined by the circle whose center is (cx, cy),
   this finds the two points on the unit circle that intersect the given pd
   line.
*/
{
  double d, r;
  assert(enda != NULL);  assert(endb != NULL);
  r=conxpd_getr(cx, cy);
  d=1.0+sqr(r);
  if (r!=DIAMETER) {
    enda->x=(cx-r*cy)/d;
    enda->y=(cy+r*cx)/d;
    endb->x=(cx+r*cy)/d;
    endb->y=(cy-r*cx)/d;
  } else {
    if (myabs(cx)<=VERTICALNESS) {
      enda->x=0.0;
      enda->y=-1.0;
      endb->x=0.0;
      endb->y=1.0;
    } else {
      conxhm_getendptsmb(cy/cx, 0.0, enda, endb);
    }
    /*    if (logging) fprintf(stderr, "\nendpts %f %f %f %f", enda->x,
          enda->y, endb->x, endb->y); fflush(stderr); */
  }
  /* see
http://riceinfo.rice.edu/projects/NonEuclid/technical.html */
  /* Ax = (Xo - rYo) / d^2 
     Ay = (Yo + rXo) / d^2 
     Bx = (Xo + rYo) / d^2 
     By = (Yo - rXo) / d^2  
     where d is the length of line segment OQ. */
  
}

double conxpd_distFromcX(double cx, double cy, Pt X, double computol)
{
  Pt kendb;
  double m, b;
  
  
  conxhm_getmbfromc(cx, cy, &m, &b);
  conxhm_pdtokAB(X, &X);
  conxk_getPtNearXonmb(X, m, b, &kendb, computol);
  return conxk_distAB(X, kendb); 
}

void conxp_getPtNearXonar(Pt X, double a, double r, Pt *A, double computol)
{
  double m, b;

  assert(A != NULL);
  conxhm_getmbfromar(a, r, &m, &b);
  LOGGG6(LOGG_FULL, "\nm= %f b= %f from a= %f r= %f X %f %f", m, b, a, r,
         X.x, X.y);
  conxhm_ptokAB(X, &X);
  conxk_getPtNearXonmb(X, m, b, A, computol);
  LOGGG2(LOGG_FULL, "\nA %f %f", A->x, A->y);
  conxhm_ktopAB(*A, A);
}


double conxp_distFromarX(double a, double r, Pt X, double computol)
{
  Pt A; /* the point on the line nearest X */
  conxp_getPtNearXonar(X, a, r, &A, computol);
  return conxp_distAB(X,A);
}


/*********************************************************************
  These next functions convert poincare UHP coordinates to Klein
  coordinates, poincare UHP coordinates to poincare disk coordinates,
  poincare disk coordinates to Klein coordinates, and vice versa.
**********************************************************************/
void conxhm_ktop(double px, double py, double *kx, double *ky)
{
  assert(kx != NULL); assert(ky != NULL);
  *kx=px/(1.0-py);
  *ky=sqrt(1.0-sqr(px)-sqr(py))/(1.0-py);
}

void conxhm_ptok(double kx, double ky, double *px, double *py)
{
  double sumsqrs;
  
  assert(px != NULL); assert(py != NULL);
  sumsqrs=sqr(kx)+sqr(ky);
  *px=2.0*kx/(1.0+sumsqrs);
  *py=(sumsqrs-1.0)/(1.0+sumsqrs);
}

void conxhm_ktopd(double u, double v, double *x, double *y)
{
  double temp;
  assert(x != NULL); assert(y != NULL);
  temp=1.0+sqrt(myabs(1.0-sqr(u)-sqr(v)));
  *x=u/temp;
  *y=v/temp;
}

void conxhm_pdtok(double x, double y, double *u, double *v)
{
  double temp;
  
  assert(u != NULL); assert(v != NULL);
  temp=1.0+sqr(x)+sqr(y);
  *u=2.0*x/temp;
  *v=2.0*y/temp;
/*  if (loglevel > 1000) fprintf(stderr, "\n%f %f = u v %f %f = x y", x, y, *u, *v); fflush(stderr);  */
}

void conxhm_pdtop(double x, double y, double *u, double *v)
/* see millman/parker p. 304 */
{
  assert(u != NULL); assert(v != NULL);
/*  temp=sqr(1.0-y)+sqr(x);
  *u=2*x/temp;
  *v=(1.0-sqr(x)-sqr(y))/temp; */
  conxhm_pdtok(x, y, &x, &y);
  conxhm_ktop(x, y, u, v);
}

void conxhm_ptokAB(Pt P, Pt *K)
{
  assert(K != NULL);
  conxhm_ptok(P.x, P.y, &K->x, &K->y);
}

void conxhm_ktopAB(Pt K, Pt *P)
{
  assert(P != NULL);
  conxhm_ktop(K.x, K.y, &P->x, &P->y);
}

void conxhm_pdtopAB(Pt P, Pt *K)
{
  assert(K != NULL);
  conxhm_pdtop(P.x, P.y, &K->x, &K->y);
}

void conxhm_pdtokAB(Pt P, Pt *K)
{
  assert(K != NULL);
  conxhm_pdtok(P.x, P.y, &K->x, &K->y);
}

void conxhm_ktopdAB(Pt P, Pt *K)
{
  assert(K != NULL);
  conxhm_ktopd(P.x, P.y, &K->x, &K->y);
}

void conxhm_ptopd(double u, double v, double *x, double *y)
{
  assert(x != NULL); assert(y != NULL);
  conxhm_ptok(u, v, &u, &v);
  conxhm_ktopd(u, v, x, y);
}

void conxhm_ptopdAB(Pt P, Pt *K)
/* Convert the Poincare UHP coordinate @var{P} to a Poincare disk coordinate
   @var{*K}
*/
{
  assert(K != NULL);
  conxhm_ptopd(P.x, P.y, &K->x, &K->y);
}

void conxhm_getmbfromar(double a, double r, double *m, double *b)
{
  double x,y,u,v;
  
  assert(m != NULL); assert(b != NULL);
  if (r!=TYPEI) {
    conxhm_ptok(a-r, 0.0, &x, &y);
    conxhm_ptok(a+r, 0.0, &u, &v);
  } else {
    conxhm_ptok(a, 0.1, &x, &y);
    conxhm_ptok(a, 0.2, &u, &v); /* used to be &x, &y */
  }
  if (myabs(x-u)<VERTICALNESS) {
    *b=KINFINITY;
    *m=(x+u)/2.0;
  } else {
    *m=(y-v)/(x-u);
    *b=v-*m*u;
  }
}

void conxhm_getendptsmb(double m, double b, Pt *A, Pt *B)
{
  double aaa, bbb, ccc, other, b_2a;
  
  assert(A != NULL); assert(B != NULL);
  if ((b==KINFINITY)&&((m>-1.0)&&(m<1.0))) {
    A->x=B->x=m;
    A->y=sqrt(1.0-sqr(A->x));
    B->y=-A->y;
  } else {
    aaa=1.0+sqr(m);
    bbb=2.0*m*b;
    ccc=sqr(b)-1.0;
    b_2a=-0.5*bbb/aaa;

    other=sqrt(sqr(bbb)-4.0*aaa*ccc) / (2.0*aaa);
    A->x=b_2a+other;
    A->y=m*A->x+b;
    B->x=b_2a-other;
    B->y=m*B->x+b;
  }
}

void conxhm_getarfrommb(double m, double b, double *a, double *r)
{
  Pt A, B, pA, pB;
  
  assert(a != NULL); assert(r != NULL);
  conxhm_getendptsmb(m, b, &A, &B);
  conxhm_ktopAB(A, &pA);
  conxhm_ktopAB(B, &pB);
  /* pA and pB are at (a+r, 0) and (a-r, 0) */
  *r=myabs(pA.x-pB.x)/2.0;
  *a=(pA.x+pB.x)/2.0;
}

void conxhm_getcfrommb(double m, double b, double *cx, double *cy)
{
  Pt A, B;
  
  assert(cx != NULL); assert(cy != NULL);
  conxhm_getendptsmb(m, b, &A, &B);
  conxhm_ktopdAB(A, &A);
  conxhm_ktopdAB(B, &B);
  conxpd_getCenterAndRadius(A.x, A.y, B.x, B.y, cx, cy, &m);
}

void conxhm_getcfromar(double a, double r, double *cx, double *cy)
{
  
  Pt A, B;
  
  assert(cx != NULL); assert(cy != NULL);
  if (r!=TYPEI) {
    A.x=a-r;
    B.x=a+r;
    A.y=B.y=0.0;
  } else {
    A.x=B.x=a;
    A.y=0.1;
    B.y=0.2; 
  }
  conxhm_ptopdAB(A, &A);
  conxhm_ptopdAB(B, &B);
  conxpd_getCenterAndRadius(A.x, A.y, B.x, B.y, cx, cy, &a);
}

void conxhm_getmbfromc(double cx, double cy, double *m, double *b)
{  
  Pt A, B, pA, pB;
  
  assert(m != NULL); assert(b != NULL);
  conxhm_getendptsc(cx, cy, &A, &B);
  conxhm_pdtokAB(A, &pA);
  conxhm_pdtokAB(B, &pB);
  if (myabs(pA.x-pB.x)<VERTICALNESS) {
    *b=KINFINITY;
    *m=(pA.x+pB.x)/2.0;
  } else {
    *m=(pA.y-pB.y)/(pA.x-pB.x);
    *b=pA.y-(*m)*pA.x;
  }
  
}

void conxhm_getarfromc(double cx, double cy, double *a, double *r)
{
  Pt A, B, pA, pB;
  
  assert(a != NULL); assert(r != NULL);
  conxhm_getendptsc(cx, cy, &A, &B);
  conxhm_pdtopAB(A, &pA);
  conxhm_pdtopAB(B, &pB);
  /* pA and pB are at (a+r, 0) and (a-r, 0) */
  *r=myabs(pA.x-pB.x)/2;
  if (2*(*r)<VERTICALNESS)
    *r=TYPEI;
  *a=(pA.x+pB.x)/2;
}

void conxhm_getmbandAB(double a, double r, double *m, double *b, Pt *A, Pt *B)
{
  assert(A != NULL); assert(B != NULL);
  assert(m != NULL); assert(b != NULL);

  conxhm_getmbfromar(a, r, m, b);
  /* y=mx+e */ /* x^2+(mx)^2+2mxe+e^2=1 */
  /* (1.0+m^2)x^2+(2me)x+(e^2-1.0)=0 */
  conxhm_getendptsmb(*m, *b, A, B);
}

void conxpd_getCenterAndRadiusABC(Pt A, Pt B, Pt *C, double *r)
{
  conxpd_getCenterAndRadius(A.x, A.y, B.x, B.y, &(C->x), &(C->y), r);
}

void conxpd_getCenterAndRadius(double x1, double y1, double x2, double y2,
                               double *cx, double *cy, double *rad)
/* gets cx and cy.  If it's a diameter, rad will show that and (cx, cy) will
   be inside the unit circle. then, if it's vertical cy will show THAT and cx
   will otherwise have the slope
*/
{
  double d,e,f,g,h,i, denom, m;

  assert(cx != NULL); assert(cy != NULL); assert(rad != NULL);
  d=1.0+sqr(y1)+sqr(x1);
  e=1.0+sqr(y2)+sqr(x2);
  g=2.0*x1;
  h=2.0*y1;
  f=2.0*x2;
  i=2.0*y2;
  denom=g*i-f*h;
  if (myabs(denom)<=VERTICALNESS) { 
    *rad=DIAMETER; 
    if (myabs(x2-x1)>VERTICALNESS) {
      m=(y2-y1)/(x2-x1);
      *cy=0.6*m;
      if (myabs(*cy)>=0.8) {     /* if (.6, y) not inside the unit circle */
	*cx=0.79/m;
	*cy=0.79;  /* x,y still on the diameter, but inside the unit circle */
       }
    }
    else {
      *cx=0.0;
      *cy=0.79;
    }
  }
  else 
    {
    *cx=(d*i-e*h)/denom;
    *cy=(g*e-d*f)/denom;
    *rad=conxpd_getr(*cx, *cy); 
  }
}

#if 0 /* DLC */ 
int negproduct (double x, double y)
{
/*   return (((x > 0.0) && (y < 0.0)) || ((x < 0.0) && (y > 0.0))); DLC better. */
  if (x>=0.0) {
    if (y<=0.0)
      return 1;
    else
      return 0;
  } else {
    if (y<=0.0)
      return 0;
    else
      return 1;
  }
}
#endif

void conxhm_getCircleIntersectionnotvert(double a, double r, double c,
                                         double d, double s, Pt *A, Pt *B)
/* Intersection between a Poincare UHP ``line'' and a Euclidean circle,
   i.e. s is the Euclidean radius, not a Poincare UHP distance, and (c, d)
   is the Euclidean center, not the Poincare UHP center.
*/
/* conxhm_getCir... finds the intersection points on the circles
   (x-a)^2+y^2=r^2 (type II) &
   (x-c)^2+(y-d)^2=s^2
   */
{
  double m, b, t1, t2, t3;
  assert(A != NULL); assert(B != NULL);
  m=(c-a)/-d;  /* after reducing to a line and a circle ; see circles.mcd*/
  b=(sqr(a)-sqr(c)-sqr(d)+sqr(s)-sqr(r))/(-2.0*d);
  t1=1.0/(1.0+sqr(m));
  t2=a-m*b;
  t3=sqrt(myabs(-2.0*a*m*b-sqr(b)+sqr(r)-sqr(m*a)+sqr(m*r)));
  /* if (t4>a)
     t4=a-(t4-a);
     else
     t4=a+(a-t4); */
  A->x=t1*(t2+t3);
  B->x=t1*(t2-t3);
  A->y=yarx(a, r, A->x);
  B->y=yarx(a, r, B->x);
}

void conxhm_getCircleIntersection(double a, double r, double c, double d,
                                  double s, Pt *A, Pt *B)
{
  assert(A != NULL); assert(B != NULL);
  if (r==TYPEI) {
    A->x=B->x=a;
    A->y=d-sqrt(sqr(s)-sqr(a));
    B->y=d+sqrt(sqr(s)-sqr(a));
  } else {
    conxhm_getCircleIntersectionnotvert(a, r, c, d, s, A, B);
  }
}

void conx_screen2model(int x, int y, double xa, double xb, double yc,
                       double yd, int sh, Pt *ModelCoords)
/* Converts GL rendering context coordinates (x,y) into model coordinates
   given the rectangular bounding box of the model. */
/* width == height(i.e., sh) (aspect ratio of 1:1) is assumed. */
{
  assert(ModelCoords != NULL);
  ModelCoords->x = xa + ((double) x/(double) sh) * (xb - xa);
  ModelCoords->y = yd - ((double) y/(double) sh) * (yd - yc);
}

void conxhm_p_getEstarted(Pt *LB, Pt *RB, Pt f1, Pt f2, double cdist)
/* Get an ellipse in the Poincare UHP started by finding a point on it
   analytically.
   Set both *LB and *RB to that point.  The ellipse will have foci f1 and f2,
   and the sum of the distance between a point on the ellipse and f1 and
   the distance between that point and f2 will be cdist.
*/
/* DLC does this work with Type I lines??? Try f1=(0.1,0.3) and f2=(0.1, 0.4)
   to see.
*/
{
  double a, fr, df1f2;

  LOGGG4(80, "\npuhp getEstarted f1 (%f, %f) f2 (%f, %f)", f1.x, f1.y, f2.x, f2.y);

  /* Make f1 refer to the leftmost point. */
  if (f1.x > f2.x)
    conx_swapAB(&f1, &f2);
  
  /* Find the line between the foci. */
  conxp_geteq(f1, f2, &a, &fr);

  /* Find the distance between the foci. */
  df1f2 = conxp_distAB(f1, f2);

  /* Now find the intersection of the Poincare UHP line between the foci
     and a Poincare UHP circle (which we represent
     with Euclidean center and radius, causing messiness) centered at f1
     with a radius that will allow us to find one of the two points on the
     ellipse that is colinear with the foci.
  */
  /* SEE THE SHORT CIRCLE METHOD IN THE UHP TO UNDERSTAND THE CHOICE OF RADII */
  conxhm_getCircleIntersection(a, fr, f1.x,
                               myabs(f1.y * cosh((cdist-df1f2)/2.0)),
                               myabs(f1.y * sinh((cdist-df1f2)/2.0)), LB, RB);
  /* We now have two intersections, both colinear with the foci, of course, but
     one is between the foci, and the other, which is on the ellipse, is not.
  */
  if (!conx_pt_not_between(f1, *LB, f2))
    *LB=*RB;
  *RB=*LB;
}

void conxhm_p_getHstarted(Pt *LB, Pt *RB, Pt f1, Pt f2, double cdist)
/* In the Poincare UHP, find one point on each of the two branches of the
   hyperbola defined by f1, f2, and cdist.
   
   Does this work for Type I lines? DLC
*/
{
  double a, fr, df1f2_2;
  Pt temp;

  if (f1.x > f2.x)
    conx_swapAB(&f1, &f2); /* DLC necessary here and above???? */

  /* Find the line between the foci. */
  conxp_geteq(f1, f2, &a, &fr);

  /* Find half the distance between the foci. */
  df1f2_2 = conxp_distAB(f1, f2)/2.0;

  /* Now find the intersection of the Poincare UHP line between the foci
     and a Poincare UHP circle (which we represent
     with Euclidean center and radius, causing messiness) centered at a focus
     with a radius that will allow us to find one of the two points on the
     hyperbola that is colinear with the foci.  Do this for both branches.
  */
  /* SEE THE SHORT CIRCLE METHOD IN THE UHP TO UNDERSTAND THE CHOICE OF RADII */
  conxhm_getCircleIntersection(a, fr, f1.x,
                               myabs(f1.y*cosh(df1f2_2-cdist/2.0)),
                               myabs(f1.y*sinh(df1f2_2-cdist/2.0)), &temp, LB);
  if (conx_pt_not_between(f1, *LB, f2))
    *LB=temp;
  conxhm_getCircleIntersection(a, fr, f2.x,
                               myabs(f2.y*cosh(df1f2_2-cdist/2.0)),
                               myabs(f2.y*sinh(df1f2_2-cdist/2.0)), &temp, RB);
  if (conx_pt_not_between(f1, *RB, f2))
    *RB=temp;
}

void conxhm_p_getPstarted(Pt *LB, Pt *RB, Pt f1, double line_a, double line_r,
                          double computol)
/* Find one point on the Poincare UHP parabola, which is the set of points
   that are equidistant from the Poincare UHP line `(x-line_a)^2+y^2=line_r^2'
   and f1.
*/
{
  double fa, fr, neardist;
  Pt near;

  /* Find a point on the line that is nearest f1 */
  conxp_getPtNearXonar(f1, line_a, line_r, &near, computol);

  /* Now find the ``perpendicular'' to the line. */
  conxp_geteq(f1, near, &fa, &fr);

  /* The distance between near and f1 is exactly double the minimal distance
     from a point on the parabola and f1.  Make a circle of radius equal to the
     minimal distance, center that circle on f1, and then see where it meets
     the perpendicular.
  */
  neardist = conxp_distAB(near, f1);
  conxhm_getCircleIntersection(fa, fr, f1.x, myabs(f1.y*cosh(neardist/2)),
                               myabs(f1.y*sinh(neardist/2)), LB, RB);

  /* One of these intersections is between near and f1, and one is not.  We
     want the one that is, of course.
  */
  if (conx_pt_not_between(near, *LB, f1)) {
    if (conx_pt_not_between(near, *RB, f1)) {
      printf("Error getting parabola started for bresenham search");
    } else {
      *LB=*RB;
    }
  }
  *RB=*LB;


/*  glColor3f(0, 1, 1);
  conx_draw_circle(f1.x, f1.y*cosh(neardist/2), f1.y*sinh(neardist/2), tstp);*/
  /*conxp_graphar(fa, fr); */
/*  LB->x=f1.x-(LB->x-f.x);
  LB->y=2*(f1.y*cosh(neardist/2))-RB->x; */
  /*conx_draw_point(near); fprintf(stderr,"type number"); scanf("%f", &debug); */
  /*conx_draw_point(*LB);*/
}

void conxhm_p_getEQstarted(Pt *LB, Pt *RB, double line_a, double line_r,
                           double cdist, double ymin, double ymax)
/* Get one point on each branch of the set of points equidistant, at a distance
   cdist, from the Poincare UHP line characterized by line_a and line_r.
*/
{
  if (line_r != TYPEI) {
    /* Find the points directly above and below the crest of this semicircle.
       Use the Poincare UHP distance metric for Type I lines to solve.
    */
    LB->x=line_a;
    LB->y=line_r*exp(cdist);
    RB->x=line_a;
    RB->y=line_r/exp(cdist);
  } else {
    /* We have Type I lines on either side, and the
       horizontal distance apart can be calculated by drawing a circle on our
       Type I line of the appropriate radius.
    */
    /* DLC test this */
    double tmp;
    LB->y=(ymin+ymax)/2.0; /* We need it to be onscreen for the Bresenham
                              method to work. */
    tmp=myabs(LB->y*sinh(cdist));
    LB->x=line_a-tmp;
    RB->x=line_a+tmp;
    RB->y=(ymin+ymax)/2.0;
  }
}

