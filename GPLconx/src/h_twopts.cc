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
  Implementation of C++ classes in `h_twopts.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hypmath.hh"
#include "h_twopts.hh"
#include "cassert.h"
#include "canvas.hh"

CF_INLINE
CConxTwoPts::CConxTwoPts()
  : A(0.31, 0.32, CONX_KLEIN_DISK), B(0.33, 0.34, CONX_KLEIN_DISK)
       // These values may aid in debugging.
{
  MMM("CConxTwoPts()");
}

CF_INLINE
CConxTwoPts::CConxTwoPts(const CConxPoint &A, const CConxPoint &B)
  : A(), B()
{
  MMM("CConxTwoPts(const CConxPoint &_A, const CConxPoint &_B)");
  this->A = A; this->B = B;
}

CF_INLINE
CConxTwoPts::CConxTwoPts(const CConxTwoPts &m)
  : CConxObject(m), A(), B()
{
  MMM("CConxTwoPts(const CConxTwoPts &m)");
  A = m.getA(); B = m.getB();
}

NF_INLINE
CConxTwoPts &CConxTwoPts::operator=(const CConxTwoPts &o)
{
  MMM("CConxTwoPts &operator=(const CConxTwoPts &o)");
  (void) CConxObject::operator=(o);
  setA(o.getA()); setB(o.getB());
  return *this;
}

NF_INLINE
int CConxTwoPts::operator==(const CConxTwoPts &o) const
{
  return (getA() == o.getA() && getB() == o.getB());
}

NF_INLINE
int CConxTwoPts::operator!=(const CConxTwoPts &o) const
{
  return !operator==(o);
}

