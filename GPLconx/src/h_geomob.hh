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
  C++ container of two Hyperbolic Geometry Points and a HG Line.
*/

#ifndef GPLCONX_H_GEOMOB_CXX_H
#define GPLCONX_H_GEOMOB_CXX_H 1

#include "h_line.hh"

//////////////////////////////////////////////////////////////////////////////
// An abstract class that has a superset of the required data for any
// other geometric object that we handle.  Subclasses may wish to change
// the names of some fields.
class CConxGeomObj : VIRT public CConxTwoPts {
  CCONX_CLASSNAME("CConxGeomObj")
  DEFAULT_PRINTON()
public:
  CConxGeomObj();
  CConxGeomObj(const CConxGeomObj &a);
  CConxGeomObj &operator=(const CConxGeomObj &o);
  ~CConxGeomObj() { MMM("destructor"); }

  void setLine(const CConxLine &L) { this->L = L; }
  const CConxLine &getLine() const { return L; }
  double getScalar() const { return scalar; }
  void setScalar(double x) { scalar = x; }
  int operator==(const CConxGeomObj &o) const;
  int operator!=(const CConxGeomObj &o) const { return !operator==(o); }
  ostream &printOn(ostream &o, ConxModlType m) const { return printOn(o); }


private:
  CConxLine L;
  double scalar;
}; // class CConxGeomObj
// DLC TODO int CConxLine:isParallelTo(const CConxLine& o);



//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_H_GEOMOB_CXX_H
