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
  C++ RGB/HSV Color class.  Some code is from The GIMP.
*/

#ifndef GPLCONX_COLOR_CXX_H
#define GPLCONX_COLOR_CXX_H 1

#include <assert.h>
#include <math.h>

#include "printon.hh"
#include "decls.hh"

// For myequals():
extern "C" {
#include "viewer.h"
}

//////////////////////////////////////////////////////////////////////////////
// An RGB (red, green, blue) or HSV (hue, saturation, value) color.
class CConxColor : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxColor")
public:
  enum ColorType { HSV, RGB };
  enum Component { H, S, V, R, G, B };

public:
  CConxColor();
  CConxColor(double t1, double t2, double t3, ColorType ct = RGB);
  CConxColor(const CConxColor &o)
    : CConxObject(o)
  {
    uninitializedCopy(o);
  }
  CConxColor &operator=(const CConxColor &o)
  {
    (void) CConxObject::operator=(o);
    uninitializedCopy(o);
    return *this;
  }
  
  void setRGB(double R, double G, double B);
  void setHSV(double H, double S, double V);
  double getR() const;
  double getG() const;
  double getB() const;
  double getH() const;
  double getS() const;
  double getV() const;
  double get(Component c) const
  {
    switch (c) {
    case H: return getH();
    case S: return getS();
    case V: return getV();
    case R: return getR();
    case G: return getG();
    default: assert(B == c); return getB();
    }
  }
  Boole equals(const CConxColor &c, double tol = EQUALITY_TOL) const;
  ostream &printOn(ostream &o) const;

protected:
  // HSV support from The GIMP:
  static void calc_rgb_to_hsv(double *r, double *g, double *b);
  static void calc_hsv_to_rgb(double *h, double *s, double *v);


  static void clamp01(double &cc);
  static void clampHue(double &h);


private:
  void convertToRGB() const;
  void convertToHSV() const;
  void uninitializedCopy(const CConxColor &o)
  {
    r = o.r;
    g = o.g;
    b = o.b;
    rgbIsValid = o.rgbIsValid;
    h = o.h;
    s = o.s;
    v = o.v;
    hsvIsValid = o.hsvIsValid;
  }

private:
  mutable double r, g, b;
  mutable Boole rgbIsValid;
  mutable double h, s, v;
  mutable Boole hsvIsValid;
}; // class CConxColor


//////////////////////////////////////////////////////////////////////////////
// A color that can take on any value you like or one of a predefined set
// of values:
class CConxNamedColor : VIRT public CConxColor {
  CCONX_CLASSNAME("CConxNamedColor")
public:
  enum ColorName { 
    GREEN,
    WHITE,
    BLACK,
    BLUE,
    RED,
    PARABOLA,
    ELLIPSE,
    CIRCLE,
    CYAN,
    MAGENTA,
    YELLOW,
    POINT,
    LINE,
    LINESEG,
    EQDISTCURVE
  };

public:
  CConxNamedColor() { }
  CConxNamedColor(const CConxNamedColor &o) : CConxColor(o) { }
  CConxNamedColor(ColorName cn) { setColorByName(cn); }
  CConxNamedColor(double t1, double t2, double t3, ColorType ct = RGB)
    : CConxColor(t1, t2, t3, ct) { }
  CConxNamedColor &operator=(const CConxColor &c)
  {
    (void) CConxColor::operator=(c);
    return *this;
  }

  Boole isNamed(ColorName cn) const
  {
    CConxNamedColor isItMe(cn);
    return (equals(isItMe, 0.0)); // Do we need to account for a tiny error? DLC test, think and see.
  }

  void setColorByName(ColorName cn)
  {
    switch (cn) {
    case EQDISTCURVE:
    case GREEN:
      setRGB(0.0, 1.0, 0.3); break;
    case BLACK:
      setRGB(0.0, 0.0, 0.0); break;
    case LINE:
    case LINESEG:
    case BLUE:
      setRGB(0.5, 0.5, 1.0); break;
    case POINT:
    case RED:
      setRGB(1.0, 0.0, 0.0); break;
    case PARABOLA:
    case CYAN:
      setRGB(0.0, 1.0, 1.0); break;
    case ELLIPSE:
    case YELLOW:
      setRGB(1.0, 1.0, 0.0); break;
    case CIRCLE:
    case MAGENTA:
      setRGB(1.0, 0.0, 1.0); break;
    default:
      assert(cn == WHITE);
      setRGB(1.0, 1.0, 1.0); break;
    }
  }
}; // class CConxNamedColor


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxColor);

#endif // GPLCONX_COLOR_CXX_H
