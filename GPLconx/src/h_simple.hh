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
  C++ Hyperbolic Geometry Simple class
*/

#ifndef GPLCONX_H_SIMPLE_CXX_H
#define GPLCONX_H_SIMPLE_CXX_H 1

#include "printon.hh"

class CConxCanvas;
class CConxPoint;

//////////////////////////////////////////////////////////////////////////////
// A class that implements this interface draws itself without regard to
// color, line size, etc.
class /* interface */ CConxSimpleArtist : public CConxPrintableByModel {
public:
  virtual ~CConxSimpleArtist();
  /* So that we don't need run-type type identification (rtti): */
  enum SAType { SA_POINT, SA_LINE, SA_CIRCLE, SA_HYPELLIPSE, SA_EQDISTCURVE,
                SA_PARABOLA
  };
  static const char *humanSAType(SAType s);
  virtual SAType getSAType() const = 0;

  // DLC use rtti cast:
#define SA_TYPE(m, Me) private: SAType getSAType() const { return m; } \
           public: int eql(const CConxSimpleArtist &o) const \
                   { \
                     return (getSAType() == o.getSAType() \
                             && operator==((const Me &)o)); \
                   }
                   
  


  // You must overload the assignment operator, copy constructor,
  // and equality operator.

  // There are many ways to draw yourself.  These may all do the same
  // thing for some objects.
  
  // If a display list (stored drawing) might be a good idea, override
  // this to return TRUE:
  virtual Boole requiresHeavyComputation() const { return FALSE; }

  // If you are a line, draw your two defining points, e.g.:
  virtual void drawGarnishOn(CConxCanvas &cv) const = 0;

  // Bresenham means ``Either by the Bresenham method, or the best, fastest,
  // way you know how''.
  virtual void drawBresenhamOn(CConxCanvas &cv) const = 0;
   
  // This function returns zero if and only if X is on the object.
  // Most of the time, nearly zero means nearly on the object.
  virtual double definingFunction(const CConxPoint &X) const = 0;
#define SA_DEFFN() \
 private: \
   static double definingFunctionWrapper(const CConxSimpleArtist *sa, \
                                         const CConxPoint &X) \
   { \
     assert(sa != NULL); \
     return sa->definingFunction(X); \
   }

  // A virtual copy constructor, if you will.  This is needed
  // because CConxDwGeomObj is a container, but copying it is a deep copy,
  // not a shallow one.
  virtual CConxSimpleArtist *clone() const = 0;

// Relies on the copy constructor:
#define SA_CLONE(Me) private: \
          CConxSimpleArtist *clone() const { return new Me(*this); }

  // Virtual operator==, if you will:
  virtual int eql(const CConxSimpleArtist &o) const = 0;


#define SA_IMP(m, Me) \
 SA_TYPE(m, Me) \
 SA_CLONE(Me) \
 SA_DEFFN()


}; // class CConxSimpleArtist


#define DRAW_SINGLE_LINE(cv, A, B) \
   do { \
           cv.beginDraw(CConxCanvas::LINES); \
           cv.drawVertex(A); \
           cv.drawVertex(B); \
           cv.endDraw(); \
   } while (0)


// The rest of the code doesn't want +INF or NaN, it wants a large double.
// This is within an order of magnitude of the largest double, which could
// be found by enquire or paranoia.  <float.h> would have this as DBL_MAX.
// We don't bother with all that since it is not standard and since
// this is plenty big:
#define CCONX_INFINITY 1.8e+307
// On x86, I think 1.7976931348623157e+308 is the correct value.


// So that the bit-flags 0, a=CONX_MODEL2BIT(CONX_POINCARE_DISK),
// b=CONX_MODEL2BIT(CONX_KLEIN_DISK),
// c=CONX_MODEL2BIT(CONX_POINCARE_UHP),
// a|b, a|c, b|c, and a|b|c are all distinct, we define:
#define CONX_MODEL2BIT(modl) (1 << modl)


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_H_SIMPLE_CXX_H
