#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "zeroin.h"

static double dabs(double a)
{
  double t;
  t = (a < 0) ? -a : a;
  return t;
}

static double dsign(double dx, double dy)
{
  double r = dabs(dx);
  double t = (dy < 0 ) ? -r : r;
  return t;
}

double zeroin(double ax, double bx, double (*f)(double) , double tol)
{
  /*
     c
     c      a zero of the function  f(x)  is computed in the interval ax,bx .
     c
     c  input..
     c
     c  ax     left endpoint of initial interval
     c  bx     right endpoint of initial interval
     c  f      function subprogram which evaluates f(x) for any x in
     c         the interval  ax,bx
     c  tol    desired length of the interval of uncertainty of the
     c         final result ( .ge. 0.0d0)
     c
     c
     c  output..
     c
     c  zeroin abcissa approximating a zero of  f  in the interval ax,bx
     c
     c
     c      it is assumed  that   f(ax)   and   f(bx)   have  opposite  signs
     c  without  a  check.  zeroin  returns a zero  x  in the given interval
     c  ax,bx  to within a tolerance  4*macheps*abs(x) + tol, where macheps
     c  is the relative machine precision.
     c      this function subprogram is a slightly  modified  translation  of
     c  the algol 60 procedure  zero  given in  richard brent, algorithms for
     c  minimization without derivatives, prentice - hall, inc. (1973).
     c
     c
     */
  double a,b,c,d,e,eps,fa,fb,fc,tol1,xm,p,q,r,s;

  /*   compute eps, the relative machine precision */

  eps = 1.0;
 L10: eps = eps/2.0;
  tol1 = 1.0 + eps;
  if (tol1 > 1.0) goto L10;

  /*  initialization */

  a = ax;
  b = bx;
  fa = f(a);
  fb = f(b);

  /* begin step */

 L20:
  c = a;
  fc = fa;
  d = b - a;
  e = d;
 L30:
  if (dabs(fc) >= dabs(fb)) goto L40;
  a = b;
  b = c;
  c = a;
  fa = fb;
  fb = fc;
  fc = fa;

  /* convergence test */

 L40:
  tol1 = 2.0*eps*dabs(b) + 0.5*tol;
  xm = .5*(c - b);
  if (dabs(xm) <= tol1) goto L90;
  if (fb == 0.0) goto L90;

  /* is bisection necessary */

  if (dabs(e) < tol1) goto L70;
  if (dabs(fa) <= dabs(fb)) goto L70;

  /* is quadratic interpolation possible */

  if (a != c) goto L50;

  /*  linear interpolation */

  s = fb/fa;
  p = 2.0*xm*s;
  q = 1.0 - s;
  goto L60;

  /* inverse quadratic interpolation */

 L50:
  q = fa/fc;
  r = fb/fc;
  s = fb/fa;
  p = s*(2.0*xm*q*(q - r) - (b - a)*(r - 1.0));
  q = (q - 1.0)*(r - 1.0)*(s - 1.0);

  /* adjust signs */

 L60: if (p > 0.0) q = -q;
  p = dabs(p);

  /* is interpolation acceptable */

  if ((2.0*p) >= (3.0*xm*q - dabs(tol1*q))) goto L70;
  if (p >= dabs(0.5*e*q)) goto L70;
  e = d;
  d = p/q;
  goto L80;

  /*     bisection */

 L70:
  d = xm;
  e = d;

  /* complete step */

 L80: a = b;
  fa = fb;
  if (dabs(d) > tol1) b = b + d;
  if (dabs(d) <= tol1) b = b + dsign(tol1, xm);
  fb = f(b);
  if ((fb*(fc/dabs(fc))) > 0.0) goto L20;
  goto L30;

  /* done */

 L90: return b;

}



