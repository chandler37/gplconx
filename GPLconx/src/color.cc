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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>

#include "viewer.h"
#include "canvas.hh"


CConxColor::CConxColor()
  : r(0.0), g(0.0), b(0.0), rgbIsValid(TRUE), h(0.0), s(0.0), v(0.0),
    hsvIsValid(FALSE)
{
}

CConxColor::CConxColor(double t1, double t2, double t3, ColorType ct = RGB)
{
  r = g = b = h = s = v = 0.0;
  rgbIsValid = (ct == RGB);
  hsvIsValid = !rgbIsValid;
  if (ct == RGB) {
    setRGB(t1, t2, t3);
  } else {
    setHSV(t1, t2, t3);
  }
}
  
void CConxColor::setRGB(double R, double G, double B)
{
  // Clamps all values to [0.0, 1.0] and makes this RGB value the object's
  // value.

  hsvIsValid = FALSE;
  rgbIsValid = TRUE;
  clamp01(R);
  clamp01(G);
  clamp01(B);
  r = R;
  g = G;
  b = B;
}

void CConxColor::setHSV(double H, double S, double V)
{
  // Clamps S and V to [0.0, 1.0], clamps H to [0.0, 360.0) by
  // adding k*360.0 for some integer k (as you would an angle before doing
  // a trig operation), and makes this HSV value the object's value.

  hsvIsValid = TRUE;
  rgbIsValid = FALSE;
  clampHue(H);
  clamp01(S);
  clamp01(V);
  h = H;
  s = S;
  v = V;
}
double CConxColor::getR() const
{
  if (!rgbIsValid) convertToRGB();
  return r; 
}

double CConxColor::getG() const
{
  if (!rgbIsValid) convertToRGB();
  return g;
}

double CConxColor::getB() const
{
  if (!rgbIsValid) convertToRGB();
  return b;
}

double CConxColor::getH() const
{
  if (!hsvIsValid) convertToHSV();
  return h; 
}

double CConxColor::getS() const 
{
  if (!hsvIsValid) convertToHSV();
  return s;
}

double CConxColor::getV() const
{
  if (!hsvIsValid) convertToHSV();
  return v;
}

Boole CConxColor::equals(const CConxColor &c, double tol = EQUALITY_TOL) const
{
  if (rgbIsValid) {
    return (myequals(c.getR(), getR(), tol)
            && myequals(c.getG(), getG(), tol)
            && myequals(c.getB(), getB(), tol));
  } else {
    return (myequals(c.getH(), getH(), tol)
            && myequals(c.getS(), getS(), tol)
            && myequals(c.getV(), getV(), tol));
  }
}

ostream &CConxColor::printOn(ostream &o) const
{
  o << "CConxColor[RGB=(" << getR() << ", " << getG() << ", " << getB()
    << "), HSV=(" << getH() << ", " << getS() << ", " << getV() << ")]";
  return o;
}

void CConxColor::clamp01(double &cc)
{
  // clamp this RGB color component to the range [0.0, 1.0]
  if (cc < 0.0) cc = 0.0;
  if (cc > 1.0) cc = 1.0;
}

void CConxColor::clampHue(double &h)
{
  // clamp to [0.0, 360.0) by adding k*360.0 for the appropriate integer
  // k.
  if (h < 0.0)
    h += 360.0 * (1.0+floor(-h/360.0));
  else if (h >= 360.0) {
    h -= 360.0 * floor(h/360.0);
  }
  assert(h >= 0.0-EQUALITY_TOL);
  assert(h < 360.0+EQUALITY_TOL);
}

void CConxColor::convertToRGB() const
{
  assert(!rgbIsValid); assert(hsvIsValid);
  r = h;
  //  cout << "\nr is " << r << "; should be equal to h(" << h << ")\n";
  g = s;
  //  cout << "g is " << g << "; should be equal to s(" << s << ")\n";
  b = v;
  //  cout << "b is " << b << "; should be equal to v(" << v << ")\n";
  calc_hsv_to_rgb(&r, &g, &b);
  //  cout << "  r, g, b is " << r << ", " << g << ", " << b << "\n";
  rgbIsValid = TRUE;
}

void CConxColor::convertToHSV() const
{
  assert(rgbIsValid); assert(!hsvIsValid);
  h = r;
  //  cout << "\nh is " << h << "; should be equal to r(" << r << ")\n";
  s = g;
  //  cout << "s is " << s << "; should be equal to g(" << g << ")\n";
  v = b;
  //  cout << "v is " << v << "; should be equal to b(" << b << ")\n";
  calc_rgb_to_hsv(&h, &s, &v);
  //  cout << "  h, s, v is " << h << ", " << s << ", " << v << "\n";
  hsvIsValid = TRUE;
}


/* calc_rgb_to_hsv and calc_hsv_to_rgb are borrowed from a great GPL'ed
 * project, The GIMP.  This is from `gimp-1.0.4/app/blend.c'.
 * The change made is that they are now static member functions of
 * class CConxColor, and that a couple of lines have been adjusted to
 * map the hue component to [0.0, 360.0) rather than [0.0, 1.0), which
 * is what the gimp uses.

 * The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 */

void CConxColor::calc_rgb_to_hsv(double *rw_r, double *rw_g, double *rw_b)
{
  double red, green, blue;
  double hsv_h, hsv_s, hsv_v;
  double min, max;
  double delta;

  red   = *rw_r;
  green = *rw_g;
  blue  = *rw_b;

  hsv_h = 0.0; /* Shut up -Wall */

  if (red > green)
    {
      if (red > blue)
	max = red;
      else
	max = blue;

      if (green < blue)
	min = green;
      else
	min = blue;
    }
  else
    {
      if (green > blue)
	max = green;
      else
	max = blue;

      if (red < blue)
	min = red;
      else
	min = blue;
    }

  hsv_v = max;

  if (max != 0.0)
    hsv_s = (max - min) / max;
  else
    hsv_s = 0.0;

  if (hsv_s == 0.0)
    hsv_h = 0.0;
  else
    {
      delta = max - min;

      if (red == max)
	hsv_h = (green - blue) / delta;
      else if (green == max)
	hsv_h = 2 + (blue - red) / delta;
      else if (blue == max)
	hsv_h = 4 + (red - green) / delta;

      hsv_h /= 6.0;

      if (hsv_h < 0.0)
	hsv_h += 1.0;
      else if (hsv_h > 1.0)
	hsv_h -= 1.0;
    }

//   *rw_r = hsv_h;
  *rw_r = hsv_h * 360.0;
  *rw_g = hsv_s;
  *rw_b = hsv_v;
}

void CConxColor::calc_hsv_to_rgb(double *rw_h, double *rw_s, double *rw_v)
{
  double hue, saturation, value;
  double f, p, q, t;

  if (*rw_s == 0.0)
    {
      *rw_h = *rw_v;
      *rw_s = *rw_v;
      *rw_v = *rw_v; /* heh */
    }
  else
    {

      // hue needs to be in [0.0, 6.0) after this:
      hue        = *rw_h / 60.0;
//       hue        = *rw_h * 6.0;

      saturation = *rw_s;
      value      = *rw_v;

      if (hue == 6.0)
	hue = 0.0;

      f = hue - (int) hue;
      p = value * (1.0 - saturation);
      q = value * (1.0 - saturation * f);
      t = value * (1.0 - saturation * (1.0 - f));

      switch ((int) hue)
	{
	case 0:
	  *rw_h = value;
	  *rw_s = t;
	  *rw_v = p;
	  break;

	case 1:
	  *rw_h = q;
	  *rw_s = value;
	  *rw_v = p;
	  break;

	case 2:
	  *rw_h = p;
	  *rw_s = value;
	  *rw_v = t;
	  break;

	case 3:
	  *rw_h = p;
	  *rw_s = q;
	  *rw_v = value;
	  break;

	case 4:
	  *rw_h = t;
	  *rw_s = p;
	  *rw_v = value;
	  break;

	case 5:
	  *rw_h = value;
	  *rw_s = p;
	  *rw_v = q;
	  break;
        default:
          cout << "\ncase is not handled!!\n"; exit(1);
	}
    }
}

NF_INLINE
void CConxNamedColor::setColorByName(ColorName cn)
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

NF_INLINE
Boole CConxNamedColor::isNamed(ColorName cn) const
{
  CConxNamedColor isItMe(cn);
  return (equals(isItMe, 0.0)); // Do we need to account for a tiny error? DLC test, think and see.
}

NF_INLINE
CConxNamedColor &CConxNamedColor::operator=(const CConxColor &c)
{
  (void) CConxColor::operator=(c);
  return *this;
}

NF_INLINE
void CConxColor::uninitializedCopy(const CConxColor &o)
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

NF_INLINE
double CConxColor::get(Component c) const
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

NF_INLINE
CConxColor &CConxColor::operator=(const CConxColor &o)
{
  (void) CConxObject::operator=(o);
  uninitializedCopy(o);
  return *this;
}

CF_INLINE
CConxColor::CConxColor(const CConxColor &o)
  : CConxObject(o)
{
  uninitializedCopy(o);
}
