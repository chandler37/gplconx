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
  C++ container of two Hyperbolic Geometry Points.
*/

#ifndef GPLCONX_H_TWOPTS_CXX_H
#define GPLCONX_H_TWOPTS_CXX_H 1

#include "h_point.hh"

//////////////////////////////////////////////////////////////////////////////
// A class that contains two points, A and B.
class CConxTwoPts : VIRT public CConxObject/* DLC delete   , public CConxPrintableByModel */{
  CCONX_CLASSNAME("CConxTwoPts")
    // DLC delete  DEFAULT_PRINTON()
public:
  CConxTwoPts();
  CConxTwoPts(const CConxPoint &A, const CConxPoint &B);
  CConxTwoPts(const CConxTwoPts &m);
  CConxTwoPts &operator=(const CConxTwoPts &o);
  ~CConxTwoPts() { MMM("destructor"); }

  void setA(const CConxPoint &A) { this->A = A; }
  void setB(const CConxPoint &B) { this->B = B; }
  const CConxPoint &getA() const { return A; }
  const CConxPoint &getB() const { return B; }
  virtual int operator==(const CConxTwoPts &o) const;
  virtual int operator!=(const CConxTwoPts &o) const;
  // DLC delete  ostream &printOn(ostream &o, ConxModlType m) const { return printOn(o); }

private:
  CConxPoint A, B;
}; // class CConxTwoPts


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_H_TWOPTS_CXX_H
