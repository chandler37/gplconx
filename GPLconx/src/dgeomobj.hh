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
  that can draw themselves on a CConxCanvas.  These could be classes
  that extend those in `h_all.hh' if multiple inheritance were not evil.
  Since it is, we have a class that has a geometric object member.

  Classes here do not share stored drawings.  Let's say you try:
  {
    CConxDwGeomObj b(new CConxPoint());
    // initialize b
    b.drawOn(glcanvas);
    c=b;
    c.drawOn(glcanvas); // This works fine.
    b.setGeomObj(new CConxPoint(.1, .2, CONX_KLEIN_DISK));
         // now the display list is deleted.
    c.drawOn(glcanvas); // This won't work.
  }
  To fix it, we don't share stored drawings.
*/

#ifndef GPLCONX_DGEOMOBJ_CXX_H
#define GPLCONX_DGEOMOBJ_CXX_H 1

#include <iostream.h>
#include <assert.h>

#include "h_all.hh"
#include "color.hh"

typedef unsigned long SDID;

class CConxCanvas;

//////////////////////////////////////////////////////////////////////////////
// An abstract class that has a virtual
// `void drawOn(CConxCanvas &) const' method.
//
// (A Java ``Interface'' would be more appropriate.)
class /* interface */ CConxDrawable {
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
  virtual CConxArtist *aClone() const
  {
    CConxArtist *j = new CConxArtist(*this);
    if (j == NULL) OOM();
    return j;
  }
  CConxArtist() { }
  CConxArtist(const CConxArtist &o) : CConxObject(o) { }
  CConxArtist &operator=(const CConxArtist &o);
  ~CConxArtist() { MMM("destructor"); LLL("CConxArtist::Deleting " << this); }
  int operator==(const CConxArtist &o) const { return 1; }
  int operator!=(const CConxArtist &o) const { return !operator==(o); }

  // Make sure we can instantiate this fellow, although we won't, so
  // that we can have an array of CConxArtist objects and just ask
  // each of them to draw themselves to implement the master rendering
  // loop.
  ostream &printOn(ostream &o, ConxModlType m) const { return printOn(o); }
  void drawOn(class CConxCanvas &o) const;
}; // class CConxArtist


//////////////////////////////////////////////////////////////////////////////
// CConxDwGeomObj is a container that draws its CConxSimpleArtist contents,
// which it owns memory-management-wise.
//
// ``Validity'' here means ``lack of change since last drawing'', which
// becomes FALSE when something changes.  We keep track so that
// we do the LONGWAY method only once and then ``replay it'' via
// a stored drawing.  If the object becomes ``invalid'', then we must,
// of course, recompute the stored drawing. DLC NOT IMPLEMENTED
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
  CConxArtist *aClone() const
  {
    CConxArtist *j = new CConxDwGeomObj(*this);
    if (j == NULL) OOM();
    return j;
  }
  CConxDwGeomObj(CConxSimpleArtist *p);
  CConxDwGeomObj(const CConxSimpleArtist &p);
  CConxDwGeomObj() { init(); }
  CConxDwGeomObj(const CConxDwGeomObj &o);
  int operator==(const CConxDwGeomObj &o) const;
  int operator!=(const CConxDwGeomObj &o) const { return !operator==(o); }
  CConxDwGeomObj &operator=(const CConxDwGeomObj &o);
  ~CConxDwGeomObj();

  virtual const CConxNamedColor &getColor() const { return color; }
  virtual void setColor(const CConxColor &c);
  virtual void setColor(CConxNamedColor::ColorName cn);
  virtual double getLongwayTolerance() const { return lwtol; }
  virtual void setLongwayTolerance(double t) { setValidity(FALSE); lwtol = t; }
  virtual double getThickness() const { return thickness; }
  virtual void setThickness(double t) { setValidity(FALSE); thickness = t; }
  virtual Boole getGarnishing() const { return withGarnish; }
  virtual void setGarnishing(Boole g);
  virtual DrawingMethod getDrawingMethod() const { return dm; }
  virtual void setDrawingMethod(DrawingMethod m);
  ostream &printOn(ostream &o) const;
  ostream &printOn(ostream &o, ConxModlType m) const { return printOn(o); }
  static const char *drawingMethodToString(DrawingMethod m);

  // DLC invalSD???

  // These are invalid once you call setGeomObj or destroy this instance:
  const CConxSimpleArtist *getGeomObj() const { return P; }
  CConxSimpleArtist *getGeomObj();

  // After this, we own n and will be sure to delete it.  It should not be
  // scheduled for destruction.
  void setGeomObj(CConxSimpleArtist *n);

  void drawOn(CConxCanvas &cv) const throw(int);

  // DLC avoid run-time type identification by providing a `virtual TypeIdEnum whoAmI()' method
  // DLC add CConxString identifier

protected:
  virtual void setValidity(Boole v) { isValid = v; }
  virtual Boole hasValidity() const { return isValid; }
  CConxCanvas *getStoredCanvas() const { return sc; }
  void storeCanvas(CConxCanvas *c) const { sc = c; }
  void removeStoredCanvas() const { sc = NULL; }
  void drawLongway(CConxCanvas &cv, const CConxSimpleArtist &o) const;

  // We need this to send to the longway method, but it must use stored
  // information.
  static double longwayMetric(Pt x, void *t);

  void saveLongwayModel(ConxModlType m) const { sModel = m; }
  ConxModlType getLongwaySavedModel() const { return sModel; }

private: // operations
  static void longwayDrawVertex(double a, double b, void *t);
  void clear();
  void init();
  void uninitializedCopy(const CConxDwGeomObj &o);

private: // attributes
  CConxSimpleArtist *P;
  mutable CConxCanvas *sc;
  CConxNamedColor color;
  Boole isValid;
  Boole withGarnish;
  DrawingMethod dm;
  double thickness, lwtol;
  mutable ConxModlType sModel;
}; // class CConxDwGeomObj


#endif // GPLCONX_DGEOMOBJ_CXX_H

