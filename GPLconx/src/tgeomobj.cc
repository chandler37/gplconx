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
  Tests the C++ classes in `geomobj.hh'.
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>

#include "geomobj.hh"
#include "color.hh"
#include "CString.hh"

#include "tester.hh"

static int tcolor(void);
static int tcircle(void);
static int thypellipse(void);
static int teqdistcurve(void);
static int tparabola(void);
static int tline(void);
static int tlineseg(void);
static int tpoint(void);

int tcolor(void)
{
  CConxColor c(721.0, 0.5, 0.0, CConxColor::HSV);
  if (VERBOSE())
    cout << "CConxColor c(721.0, 0.5, 0.0, CConxColor::HSV) is `" << c
         << "'\n";
  //  cout << "CConxColor c(721.0, 0.5, 0.0, CConxColor::HSV).getH() is `" << c.getH() << "'\n";
  RET1(myequals(1.0, c.getH(), EQUALITY_TOL));
  RET1(myequals(0.0, c.getR(), EQUALITY_TOL));
  RET1(myequals(0.0, c.getG(), EQUALITY_TOL));
  RET1(myequals(0.0, c.getB(), EQUALITY_TOL));
  CConxColor d(-721.0, 0.5, 0.70, CConxColor::HSV);
  if (VERBOSE()) cout << "d is `" << d << "'\n";
  d.setRGB(0.1, 0.2, 0.3);
  if (VERBOSE()) cout << "d is `" << d << "'\n";
  d.setHSV(-721.0, 0.5, 0.70);
  RET1(myequals(359.0, d.getH(), EQUALITY_TOL));
  d.setRGB(0.7, 0.35, 0.355833);
  if (VERBOSE()) cout << "now d(0.7, 0.35, 0.355833) is `" << d << "'\n";

  CConxColor D(210, 2.0/3.0, 0.3, CConxColor::HSV);
  if (VERBOSE()) cout << "rgb(.1,.2,.3)->" << D << "\n";


  return 0;
}

int tcircle(void)
// Returns zero if the CConxCircle class passes.
{
  CConxCircle c;
  CConxCircle e;
  if (VERBOSE()) {
    c.printOn(cout);
    cout << endl;
  }
  c.setCenter(CConxPoint(0.1, 0.2, CONX_KLEIN_DISK));
  c.setRadius(1.0); // IEEE makes 1.0 act nice.
  if (VERBOSE()) cout << "\nNow we have: " << c << endl;
  RET1(c.getRadius() == 1.0);
  CConxCircle d(c);
  RET1(c == d);
  if (VERBOSE()) cout << "And e is " << e << endl;
  RET1(c != e);
  e = c;
  if (VERBOSE())
    cout << "After `e = c;', e is `" << e << "' and c is `" << c << "'\n";

  RET1(c == e);
  CConxCircle c1;
  return 0;
}

int thypellipse(void)
{
  CConxHypEllipse d;
  d.setScalar(1.0);
  RET1(d.getScalar() == 1.0);
  // DLC fill this in.
  return 0;
}

int teqdistcurve(void)
{
  CConxEqDistCurve d;
  d.setDistance(1.0);
  RET1(d.getDistance() == 1.0);
  CConxLine l(CConxPoint(-.25, .25, CONX_POINCARE_DISK),
              CConxPoint(0.5, 0.0, CONX_KLEIN_DISK));
  CConxLine l_2(CConxPoint(-.20, .25, CONX_POINCARE_DISK),
                CConxPoint(0.5, 0.0, CONX_KLEIN_DISK));
  RET1(!l.isLineSegment());
  l.setSegment(TRUE);
  RET1(l.isLineSegment());
  l.setSegment(FALSE);
  RET1(!l.isLineSegment());
  d.setLine(l);
  RET1(l == d.getLine());
  RET1(l_2 != d.getLine());
  if (VERBOSE()) {
    cout << "The line for the equidistant curve is `" << d.getLine() << "'\n";
    cout << "In the Poincare disk, that's `";
    d.getLine().printOn(cout, CONX_POINCARE_DISK);
    cout << "'\n";
  }
  

  // DLC fill this in.
  return 0;
}

int tparabola(void)
{
  CConxPoint pt1(0.3, 0.4, CONX_KLEIN_DISK), pt2(-0.4, -0.5, CONX_KLEIN_DISK);
  CConxPoint pt3(-0.4, 0.3, CONX_KLEIN_DISK);
  CConxLine L1(pt1, pt2);
  CConxParabola p1(pt3, L1), p2, p3;
  if (VERBOSE()) {
    cout << "Parabolas:\n" << kd << p1 << "\n\tand\n" << p2
         << "\nare NOT equal.\n";
  }
  RET1(p1 != p2);
  p2 = p1;
  if (VERBOSE()) {
    cout << "Parabolas:\n" << kd << p1 << "\n\tand\n" << p2
         << "\nare equal.\n";
  }
  RET1(p1 == p2);
  CConxPoint pt_on_p1;
  p1.getPointOn(&pt_on_p1);
  if (VERBOSE()) {
    cout << "The point " << kd << pt_on_p1 << " is on " << p1 << "\n";
  }
  // DLC fill this in.
  return 0;
}

int tline(void)
{
  CConxPoint ptE1, ptE2, pt1(0.7, 0.1, CONX_KLEIN_DISK);
  CConxPoint pt2(-0.7, -0.1, CONX_KLEIN_DISK);
  CConxLine L1(pt1, pt2);
  if (VERBOSE()) {
    cout << kd << "tline::\nThe Klein disk points " << pt1 << " and "
         << pt2 << " are on L1,\n\tand "
         << (ptE1 = CConxPoint(L1.getKleinEndPoint1(), CONX_KLEIN_DISK))
         << " and "
         << (ptE2 = CConxPoint(L1.getKleinEndPoint2(), CONX_KLEIN_DISK))
         << " are the end points.\n";
    cout << ptE1 << " is"
         << (ptE1.isAtInfinity(0.0) ? "" : " NOT")
         << " at infinity (tol=0.0) and is"
         << (ptE1.isAtInfinity(EQUALITY_TOL) ? "" : " NOT")
         << " at infinity (tol=EQUALITY_TOL).\n";
  }
  // DLC fill this in.
  return 0;
}

int tlineseg(void)
{
  // DLC fill this in.
  return 0;
}

int tpoint(void)
{
  CConxPoint p1(0.3, 0.4, CONX_KLEIN_DISK);
  ostrstream ostr;
  ostr << all << "first all " << p1 << " then kd " << kd << p1 << pd
       << " and now pd " << p1 << puhp << " and now puhp " << p1
       << all << " and now all again " << p1 << endl << ends;
  char *s = ostr.str();
#ifndef NO_IFFY_TESTS
  RET1(CConxString("first all [puhp(0.5, 1.44338), kd(0.3, 0.4), pd(0.16077, 0.214359)] then kd kd(0.3, 0.4) and now pd pd(0.16077, 0.214359) and now puhp puhp(0.5, 1.44338) and now all again [puhp(0.5, 1.44338), kd(0.3, 0.4), pd(0.16077, 0.214359)]\n") == s);
#endif
  if (VERBOSE()) {
    cout << s;
  }
  delete [] s;
  
  // DLC fill this in.
  return 0;
}


int main(int argc, char **argv)
{
  HANDLE_ARGS(argc, argv);

  // DLC finish this.
  THERE_ARE_ZERO_OBJECTS();
  TEST(tcolor() == 0);
  THERE_ARE_ZERO_OBJECTS();
  TEST(tcircle() == 0);
  THERE_ARE_ZERO_OBJECTS();
  TEST(thypellipse() == 0);
  THERE_ARE_ZERO_OBJECTS();
  TEST(teqdistcurve() == 0);
  THERE_ARE_ZERO_OBJECTS();
  TEST(tparabola() == 0);
  THERE_ARE_ZERO_OBJECTS();
  TEST(tline() == 0);
  THERE_ARE_ZERO_OBJECTS();
  TEST(tlineseg() == 0);
  THERE_ARE_ZERO_OBJECTS();
  TEST(tpoint() == 0);
  THERE_ARE_ZERO_OBJECTS();
  return GOOD_TEST_EXIT_CODE;
}
