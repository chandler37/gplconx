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
  C++ Printable classes (Java ``Interfaces'' would be more appropriate).
*/

#ifndef GPLCONX_PRINTON_CXX_H
#define GPLCONX_PRINTON_CXX_H 1

#include <assert.h>

#include "point.hh"
#include "decls.hh"

class ostream;

//////////////////////////////////////////////////////////////////////////////
// This class would be a Java Interface; any class that subclasses this
// class must fill in the printOn method.
class /* interface */ CConxPrintable {
public: // pure virtual functions
  virtual ostream &printOn(ostream &) const = 0;
};

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
  template <class T> static ostream &printByModel(ostream &o, const T &a)
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
  virtual ostream &printOn(ostream &, ConxModlType) const = 0;


private:
  static PrintHow phow;
};


/*
  The code:
  {
     CConxPoint c(0.3, 0.4, CONX_KLEIN_DISK);
     cout << "The point is " << c << endl;
  }
  will print out c's coordinates in all models since we use
    ostream &operator<<(ostream &o, const CConxPoint &a)
    {
      return a.printOn(o);
    }
  as a shortcut.

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
  so long as we define
    ostream &operator<<(ostream &o, const CConxPoint &a)
    {
      return CConxPrintableByModel::printByModel(o, a);
    }
  as our shortcut.

  It's a timesaver; see how it's used in `tgeomobj.cc'.
 */
ostream &pd(ostream &);
ostream &kd(ostream &);
ostream &puhp(ostream &);
ostream &all(ostream &);

// See `geomobj.cc' and `geomobj.hh' to see how these are used:
#define PBM_STREAM_OUTPUT_SHORTCUT_DECL(subclass) \
ostream &operator<<(ostream &, const subclass &)

#define PBM_STREAM_OUTPUT_SHORTCUT(subclass) \
ostream &operator<<(ostream &o, const subclass &a) \
{ \
  return CConxPrintableByModel::printByModel(o, a); \
}

// For subclasses of Printable
#define P_STREAM_OUTPUT_SHORTCUT_DECL(subclass) \
ostream &operator<<(ostream &, const subclass &)

#define P_STREAM_OUTPUT_SHORTCUT(subclass) \
ostream &operator<<(ostream &o, const subclass &a) \
{ \
  return a.printOn(o); \
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
