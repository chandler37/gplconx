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
  C++ classes that model elements of computational hyperbolic geometry
  that can draw themselves on a CConxCanvas.  These classes extend those
  in `geomobj.hh'.

  Classes here do not share stored drawings.  Let's say you try:
  {
    CConxDwPoint b;
    // initialize b
    b.drawOn(glcanvas);
    c=b;
    c.drawOn(glcanvas); // This works fine.
    b.setPoint(.1, .2, CONX_KLEIN_DISK); // now the display list is deleted.
    c.drawOn(glcanvas); // This won't work.
  }
  To fix it, we don't share stored drawings.
*/

#ifndef GPLCONX_DGEOMOBJ_CXX_H
#define GPLCONX_DGEOMOBJ_CXX_H 1

#include <iostream.h>
#include <assert.h>

#include "geomobj.hh"
#include "color.hh"

typedef unsigned long SDID;

class CConxCanvas;

//////////////////////////////////////////////////////////////////////////////
// An abstract class that has a virtual
// `void drawOn(CConxCanvas &) const' method.
//
// (A Java ``Interface'' would be more appropriate.)
class CConxDrawable {
public: // pure virtual functions
  virtual void drawOn(CConxCanvas &) const throw(int) = 0;
}; // class CConxDrawable


//////////////////////////////////////////////////////////////////////////////
// The base class for drawable objects, like a circle that knows how to draw
// itself.
class CConxArtist : VIRT public CConxObject,
                    public CConxPrintableByModel,
                    public CConxDrawable          {
public:
  DEFAULT_PRINTON()
public:
  CConxArtist() { }
  CConxArtist(const CConxArtist &o) : CConxObject(o) { }
  CConxArtist &operator=(const CConxArtist &o)
  {
    (void) CConxObject::operator=(o);
    return *this;
  }
  ~CConxArtist() { }
  int operator==(const CConxArtist &o) { return 1; }
  int operator!=(const CConxArtist &o) { return !operator==(o); }

  // Make sure we can instantiate this fellow, although we won't, so
  // that we can have an array of CConxArtist objects and just ask
  // each of them to draw themselves to implement the master rendering
  // loop.
  ostream &printOn(ostream &o, ConxModlType m) const { return printOn(o); }
  void drawOn(class CConxCanvas &o) const
  {
    MMM("void drawOn(class CConxCanvas &o) const");
  }

}; // class CConxArtist


//////////////////////////////////////////////////////////////////////////////
// CConxDwGeomObj is the base class for DLC ...
// ``Validity'' here means ``lack of change since last drawing'', which
// becomes FALSE when something changes.  We keep track so that
// we do the LONGWAY method only once and then ``replay it'' via
// a stored drawing.  If the object becomes ``invalid'', then we must,
// of course, recompute the stored drawing.
//
// The default copy constructor, operator=, and equality operators would
// work, but subclasses would get compiler warnings about removed
// constness when they called the compiler-provided versions.
//
// Usually a subclass's versions of the copy constructor
// and the operator= will usually call CConxDwGeomObj::setValidity(FALSE)
// since stored drawings are kind of like pointers.
class CConxDwGeomObj : VIRT public CConxArtist {
  CCONX_CLASSNAME("CConxDwGeomObj")
public: // types
  enum DrawingMethod { SAFEST, BRESENHAM, LONGWAY, BEST };
public:
  CConxDwGeomObj()
    : sc(NULL), color(CConxNamedColor::GREEN), isValid(FALSE),
      withGarnish(TRUE), dm(BEST), thickness(1.0), lwtol(.0015),
      sModel(CONX_KLEIN_DISK) { }
  CConxDwGeomObj(const CConxDwGeomObj &o)
    : CConxArtist(o)
  {
    uninitializedCopy(o);
  }
  int operator==(const CConxDwGeomObj &o) const
  {
    return (getColor().equals(o.getColor())
            && getGarnishing() == o.getGarnishing()
            && getThickness() == o.getThickness()
            && getLongwayTolerance() == o.getLongwayTolerance()
            && getDrawingMethod() == o.getDrawingMethod());
  }
  int operator!=(const CConxDwGeomObj &o) const { return !operator==(o); }
  CConxDwGeomObj &operator=(const CConxDwGeomObj &o)
  {
    (void) CConxArtist::operator=(o);
    uninitializedCopy(o);
    return *this;
  }
  virtual ~CConxDwGeomObj() { /* No, don't delete the stored canvas. */ }

  // since drawOn is not defined, test that we cannot instantiate.  DLC

  virtual const CConxNamedColor &getColor() const { return color; }
  virtual void setColor(const CConxColor &c)
  {
    if (!color.equals(c)) {
      setValidity(FALSE);
      color = c;
    }
  }
  virtual void setColor(CConxNamedColor::ColorName cn)
  {
    if (!color.isNamed(cn)) {
      setValidity(FALSE);
      color.setColorByName(cn);
    }
  }
  virtual double getLongwayTolerance() const { return lwtol; }
  virtual void setLongwayTolerance(double t) { setValidity(FALSE); lwtol = t; }
  virtual double getThickness() const { return thickness; }
  virtual void setThickness(double t) { setValidity(FALSE); thickness = t; }
  virtual Boole getGarnishing() const { return withGarnish; }
  virtual void setGarnishing(Boole g)
  {
    if (withGarnish != g) {
      setValidity(FALSE);
      withGarnish = g;
    }
  }
  virtual DrawingMethod getDrawingMethod() const { return dm; }
  virtual void setDrawingMethod(DrawingMethod m)
  {
    if (dm != m) {
      setValidity(FALSE);
      dm = m;
    }
  }
  ostream &printOn(ostream &o) const
  {
    o << "<CConxDwGeomObj object color=";
    color.printOn(o);
    o << ", isValid=" << BOOLE2STRING(isValid) << ", withGarnish="
      << BOOLE2STRING(withGarnish) << ", drawingMethod="
      << drawingMethodToString(dm) << ", thickness=" << thickness
      << ", lwtol=" << lwtol
      << ">";
    return o;
  }
  ostream &printOn(ostream &o, ConxModlType m) const { return printOn(o); }
  static const char *drawingMethodToString(DrawingMethod m)
  {
    switch (m) {
    case SAFEST: return "SAFEST";
    case BRESENHAM: return "BRESENHAM";
    case LONGWAY: return "LONGWAY";
    default: assert(m == BEST); return "BEST";
    }
  }

  // DLC avoid run-time type identification by providing a `virtual TypeIdEnum whoAmI()' method
  // DLC add CString identifier

protected:
  virtual void setValidity(Boole v) { isValid = v; }
  virtual Boole hasValidity() const { return isValid; }
  CConxCanvas *getStoredCanvas() const { return sc; }
  void storeCanvas(CConxCanvas *c) const { sc = c; }
  void removeStoredCanvas() const { sc = NULL; }
  void drawLongway(CConxCanvas &cv, ConxMetric *test) const;
  void saveLongwayModel(ConxModlType m) const { sModel = m; }
  ConxModlType getLongwaySavedModel() const { return sModel; }

private: // operations
  static void longwayDrawVertex(double a, double b, void *t);
  void uninitializedCopy(const CConxDwGeomObj &o)
  {
    color = o.color;
    isValid = o.isValid;
    withGarnish = o.withGarnish;
    dm = o.dm;
    thickness = o.thickness;
    lwtol = o.lwtol;
    sModel = o.sModel;

    sc = NULL;
    // Why share this too? We only keep this during a call to drawOn!
  }

private: // attributes
  mutable CConxCanvas *sc;
  CConxNamedColor color;
  Boole isValid;
  Boole withGarnish;
  DrawingMethod dm;
  double thickness, lwtol;
  mutable ConxModlType sModel;
}; // class CConxDwGeomObj


//////////////////////////////////////////////////////////////////////////////
class CConxDwPoint : VIRT public CConxDwGeomObj {
  CCONX_CLASSNAME("CConxDwPoint")
public:
#define DWPOINT_INIT() setColor(CConxNamedColor::POINT); setThickness(6.0)

  CConxDwPoint() : P(), sdid(0), sdidIsValid(FALSE) { DWPOINT_INIT(); }
  CConxDwPoint(double x, double y, ConxModlType modl)
    : P(x, y, modl), sdid(0), sdidIsValid(FALSE) { DWPOINT_INIT(); }
  CConxDwPoint(const CConxPoint &p) : P(p), sdid(0), sdidIsValid(FALSE)
  {
    DWPOINT_INIT();
  }
  CConxDwPoint(const CConxDwPoint &o)
    : CConxDwGeomObj(o), P(o.P), sdid(0), sdidIsValid(FALSE)
  {
    setValidity(FALSE);
  }
  CConxDwPoint &operator=(const CConxDwPoint &o)
  {
    (void) CConxDwGeomObj::operator=(o);
    setValidity(FALSE);
    invalSD();
    P = o.P;
    return *this;
  }

  void drawOn(CConxCanvas &) const throw(int);
  const CConxPoint &getPoint() const { return P; }
  void setPoint(const CConxPoint &p)
  {
    // DLC test for equality for max efficiency.
    P = p;
    setValidity(FALSE);
  }

  ostream &printOn(ostream &o) const
  {
    o << "CConxDwPoint<[";
    CConxDwGeomObj::printOn(o);
    o << ", ";
    P.printOn(o);
    o << "]>";
    return o;
  }
  ostream &printOn(ostream &o, ConxModlType modl) const
  {
    o << "CConxDwPoint<[";
    CConxDwGeomObj::printOn(o, modl);
    o << ", ";
    P.printOn(o);
    o << "]>";
    return o;
  }


private: // operations
  void invalSD()
  // Call when the display list becomes invalid.
  {
    // We need to know who the canvas is!  The canvas needs to store display lists??? DLC
    if (sdidIsValid) {
      // DLC delete the sd.
      sdidIsValid = FALSE;
    }
  }

private: // attributes
  CConxPoint P;
  mutable SDID sdid;
  mutable Boole sdidIsValid;
}; // class CConxDwPoint


//////////////////////////////////////////////////////////////////////////////
// CConxLine's drawable cousin
class CConxDwLine : VIRT public CConxDwGeomObj {
  CCONX_CLASSNAME("CConxDwLine")
public:
#define DWLINE_INIT() setColor(CConxNamedColor::LINE)
  CConxDwLine() : L(), sdid(0), sdidIsValid(FALSE) { DWLINE_INIT(); }
  CConxDwLine(const CConxLine &l)
    : L(l), sdid(0), sdidIsValid(FALSE) { DWLINE_INIT(); }
  CConxDwLine(const CConxLine &l, Boole ls)
    : L(l), sdid(0), sdidIsValid(FALSE) { DWLINE_INIT(); L.setSegment(ls); }
  CConxDwLine(const CConxPoint &A, const CConxPoint &B)
    : L(A, B), sdid(0), sdidIsValid(FALSE) { DWLINE_INIT(); }
  CConxDwLine(const CConxDwLine &o)
    : CConxDwGeomObj(o), L(o.L), sdid(0), sdidIsValid(FALSE)
  {
    MMM("CConxDwLine(const CConxDwLine &)");
    setValidity(FALSE);
  }
  CConxDwLine &operator=(const CConxDwLine &o)
  {
    MMM("CConxDwLine &operator=(const CConxDwLine &)");
    (void) CConxDwGeomObj::operator=(o);
    L = o.L;
    setValidity(FALSE);
    sdidIsValid = FALSE;
    return *this;
  }

  void setLine(const CConxLine &l) { setValidity(FALSE); invalSD(); L = l; }
  const CConxLine &getLine() const { return L; }
  void setA(const CConxPoint &A) { setValidity(FALSE); invalSD(); L.setA(A); }
  void setB(const CConxPoint &B) { setValidity(FALSE); invalSD(); L.setB(B); }
  void drawOn(CConxCanvas &) const throw(int);
  ostream &printOn(ostream &o) const
  {
    o << "CConxDwLine<[";
    CConxDwGeomObj::printOn(o);
    o << ", ";
    L.printOn(o);
    o << "]>";
    return o;
  }
  ostream &printOn(ostream &o, ConxModlType modl) const
  {
    o << "CConxDwLine<[";
    CConxDwGeomObj::printOn(o, modl);
    o << ", ";
    L.printOn(o, modl);
    o << "]>";
    return o;
  }


private: // operations
  void invalSD()
  // Call when the display list becomes invalid.
  {
    // We need to know who the canvas is!  The canvas needs to store display lists??? DLC
    if (sdidIsValid) {
      // DLC delete the sd.
      sdidIsValid = FALSE;
    }
  }

private: // attributes
  CConxLine L;
  mutable SDID sdid;
  mutable Boole sdidIsValid;
}; // class CConxDwLine


//////////////////////////////////////////////////////////////////////////////
// CConxCircle's drawable cousin
class CConxDwCircle : VIRT public CConxDwGeomObj {
  CCONX_CLASSNAME("CConxDwCircle")
public:
#define DWCIRCLE_INIT() setColor(CConxNamedColor::CIRCLE)
  CConxDwCircle() : L(), sdid(0), sdidIsValid(FALSE) { DWCIRCLE_INIT(); }
  CConxDwCircle(const CConxCircle &l)
    : L(l), sdid(0), sdidIsValid(FALSE) { DWCIRCLE_INIT(); }
  CConxDwCircle(const CConxPoint &C, double radius)
    : L(C, radius), sdid(0), sdidIsValid(FALSE) { DWCIRCLE_INIT(); }
  CConxDwCircle(const CConxDwCircle &o)
    : CConxDwGeomObj(o), L(o.L), sdid(0), sdidIsValid(FALSE)
  {
    MMM("CConxDwCircle(const CConxDwCircle &)");
    setValidity(FALSE);
  }
  CConxDwCircle &operator=(const CConxDwCircle &o)
  {
    MMM("CConxDwCircle &operator=(const CConxDwCircle &)");
    (void) CConxDwGeomObj::operator=(o);
    L = o.L;
    setValidity(FALSE);
    sdidIsValid = FALSE;
    return *this;
  }

  void setCircle(const CConxCircle &l) { setValidity(FALSE); invalSD(); L = l; }
  const CConxCircle &getCircle() const { return L; }
  void setRadius(double r) { setValidity(FALSE); invalSD(); L.setRadius(r); }
  void setCenter(const CConxPoint &C) { setValidity(FALSE); invalSD(); L.setCenter(C); }
  void drawOn(CConxCanvas &) const throw(int);
  ostream &printOn(ostream &o) const
  {
    o << className() << "<[";
    CConxDwGeomObj::printOn(o);
    o << ", ";
    L.printOn(o);
    o << "]>";
    return o;
  }
  ostream &printOn(ostream &o, ConxModlType modl) const
  {
    o << className() << "<[";
    CConxDwGeomObj::printOn(o, modl);
    o << ", ";
    L.printOn(o, modl);
    o << "]>";
    return o;
  }


private: // operations
  static double longwayMetric(Pt x, void *t);
  void longwayDrawOn(CConxCanvas &cvs) const throw(int);
  void smartDrawOn(CConxCanvas &cvs) const throw(int);
  void invalSD()
  // Call when the display list becomes invalid.
  {
    // We need to know who the canvas is!  The canvas needs to store display lists??? DLC
    if (sdidIsValid) {
      // DLC delete the sd.
      sdidIsValid = FALSE;
    }
  }

private: // attributes
  CConxCircle L;
  mutable SDID sdid;
  mutable Boole sdidIsValid;
}; // class CConxDwCircle


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxDwGeomObj);
OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxArtist);


#endif // GPLCONX_DGEOMOBJ_CXX_H

