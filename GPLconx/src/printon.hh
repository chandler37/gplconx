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
  C++ Printable and PrintableByModel classes (Java ``Interfaces'' would be
  more appropriate) and supporting operators and manipulators.
*/

#ifndef GPLCONX_PRINTON_CXX_H
#define GPLCONX_PRINTON_CXX_H 1

#include <assert.h>
#include <iostream.h>

#include "point.hh"
#include "CObject.hh"

//////////////////////////////////////////////////////////////////////////////
// This class would be a Java Interface; any class that subclasses this
// class must fill in the printOn method.
class /* interface */ CConxPrintable {
public: // pure virtual functions
  virtual ostream &printOn(ostream &o) const
  {
    o << "<object that is a subclass of CConxPrintable>";
    return o;
  }
}; // class CConxPrintable


//////////////////////////////////////////////////////////////////////////////
// If an object is associated with the Poincare Disk, Poincare UHP, and
// Klein Disk models of hyperbolic geometry, then implement this
// as well as using printOn(ostream &) to output for all models:
class /* interface */ CConxPrintableByModel : public CConxPrintable {
public: // types
  enum PrintHow { PP_PD, PP_KD, PP_PUHP, PP_ALL };
public: // static members
  static PrintHow getDesiredFormat() { return phow; }
  static void setDesiredFormat(PrintHow a) { phow = a; }
  // DLC delete  template <class T> static ostream &printByModel(ostream &o, const T &a)
  static ostream &printByModel(ostream &o, const CConxPrintableByModel &a)
  {
    PrintHow p = getDesiredFormat();
    switch (p) {
    case PP_ALL: return a.printOn(o);
    case PP_KD: return a.printOn(o, CONX_KLEIN_DISK);
    case PP_PD: return a.printOn(o, CONX_POINCARE_DISK);
    default: assert(p == PP_PUHP); return a.printOn(o, CONX_POINCARE_UHP);
    }
  }

public: // pure virtual functions
  virtual ostream &printOn(ostream &o, ConxModlType mt) const
  {
    return printOn(o);
  }

  virtual ostream &printOn(ostream &o) const
  {
    o << "<CConxPrintableByModel default `virtual ostream &printOn(ostream &o) const'>";
    return o;
  }

private:
  static PrintHow phow;
}; // class CConxPrintableByModel


/*
  The code:
  {
     CConxPoint c(0.3, 0.4, CONX_KLEIN_DISK);
     cout << "The point is " << c << endl;
  }
  will print out c's coordinates in all models since we overload
  `ostream &operator<<(ostream &o, const CConxPrintable &)'.

  If you want to print out c's coordinates only in the Klein disk,
  then you must say the lengthier:
  {
     CConxPoint c(0.3, 0.4, CONX_KLEIN_DISK);
     cout << "The point is ";
     c.printOn(cout, CONX_KLEIN_DISK);
     cout << endl;
  }
  
  To make the second form look nicer, we define manipulators pd, kd, puhp,
  and all.  (all is the default.)  Now we can say:
  {
     CConxPoint c(0.3, 0.4, CONX_KLEIN_DISK);
     cout << "The point is " << kd << c << all << endl;
  }
 */
ostream &pd(ostream &);
ostream &kd(ostream &);
ostream &puhp(ostream &);
ostream &all(ostream &);

inline
ostream &operator<<(ostream &o, const CConxPrintable &s)
{
  return s.printOn(o);
}

inline
ostream &operator<<(ostream &o, const CConxPrintableByModel &s)
{
  return CConxPrintableByModel::printByModel(o, s);
}

#define DEFAULT_PRINTON() \
public: \
ostream &printOn(ostream &o) const \
{ \
  o << "<" << className() << " object>"; \
  return o; \
} \
private:

#endif // GPLCONX_PRINTON_CXX_H
