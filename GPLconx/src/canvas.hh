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
  Abstract C++ Drawing Canvas classes.
*/

#ifndef GPLCONX_HGMCANVAS_CXX_H
#define GPLCONX_HGMCANVAS_CXX_H 1

#include "point.h"

#include "printon.hh"
#include "CArray.hh"
#include "dgeomobj.hh"
#include "decls.hh"
#include "color.hh"

class CConxDumbCanvas
  : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxDumbCanvas")
public:
  CConxDumbCanvas();
  CConxDumbCanvas(const CConxDumbCanvas &o) { uninitializedCopy(o); }
  CConxDumbCanvas &operator=(const CConxDumbCanvas &o)
  {
    uninitializedCopy(o);
    return *this;
  }
  // The default operator== and != will work.

  void setSize(uint w, uint h) throw(int);
  uint getWidth() const { return width; }
  uint getHeight() const { return height; }
  void setViewingRectangle(double xmin, double xmax,
                           double ymin, double ymax) throw(int);
  double getXmin() const { return xmin; }
  double getXmax() const { return xmax; }
  double getYmin() const { return ymin; }
  double getYmax() const { return ymax; }
  double getPixelWidth() const;
  double getPixelHeight() const;
  ostream &printOn(ostream &o) const;

private: // operations
  void uninitializedCopy(const CConxDumbCanvas &o)
  {
    width = o.width;
    height = o.height;
    xmin = o.xmin;
    xmax = o.xmax;
    ymin = o.ymin;
    ymax = o.ymax;
  }

private: // attributes
  uint width, height; // In pixels.  Always strictly positive.
  double xmin, xmax, ymin, ymax;
  // model coordinates.  Always xmin <= xmax and ymin <= ymax
}; // class CConxDumbCanvas


//////////////////////////////////////////////////////////////////////////////
// A canvas that you can draw on.  This class is abstract.
// An OpenGL implementation of this class would subclass this, e.g.
class CConxDrawCanvas : VIRT public CConxDumbCanvas {
  CCONX_CLASSNAME("CConxDrawCanvas")
  DEFAULT_PRINTON()
public:
  enum DrawingType { POINTS, LINES, LINE_STRIP };

public:
  CConxDrawCanvas() { }
  CConxDrawCanvas(const CConxDrawCanvas &o) : CConxDumbCanvas(o) { }
  CConxDrawCanvas &operator=(const CConxDrawCanvas &o)
  {
    (void) CConxDumbCanvas::operator=(o);
    return *this;
  }
  ~CConxDrawCanvas() { }

  // SD means ``stored drawing'', like an OpenGL display list.
  virtual SDID startSD() throw(int) = 0;
  virtual void stopSD() = 0;
  virtual void deleteSD(SDID id) = 0;
  virtual void deleteAllSD() = 0;
  virtual void executeSD(SDID id) = 0;

  virtual void beginDraw(DrawingType dt) = 0;
  virtual void endDraw() = 0;
  virtual void drawVertex(double x, double y) = 0;
  virtual void drawVertex(const Pt &p)
  {
    drawVertex(p.x, p.y);
  }
  virtual void drawCircle(double x, double y, double r) = 0;
  virtual void drawCircle(Pt p, double r) { drawCircle(p.x, p.y, r); }
  virtual void drawTopSemiCircle(double x, double y, double r) = 0;
  virtual void drawTopSemiCircle(Pt p, double r)
  {
    drawTopSemiCircle(p.x, p.y, r);
  }
  virtual void drawArc(double x, double y, double r, double t0, double t1) = 0;
  virtual void drawArc(Pt center, double r, double t0, double t1)
  {
    drawArc(center.x, center.y, r, t0, t1);
  }

  virtual void setDrawingColor(const CConxColor &C) = 0;
  virtual void setPointSize(double pSize) = 0;
  virtual void flush() = 0;
  virtual void clear() = 0;

  // you must call this before drawing and after setting the size.
  virtual void initDraw() = 0;
}; // class CConxDrawCanvas


//////////////////////////////////////////////////////////////////////////////
// Abstract -- you must subclass and implement the drawing operations.
// A canvas that you can draw on that knows what model it represents.
class CConxCanvas : VIRT public CConxDrawCanvas {
  CCONX_CLASSNAME("CConxCanvas")
public:
  CConxCanvas() : modl(CONX_KLEIN_DISK) { }
  CConxCanvas(const CConxCanvas &o) : CConxDrawCanvas(o)
  {
    uninitializedCopy(o);
  }
  CConxCanvas &operator=(const CConxCanvas &o)
  {
    (void) CConxDrawCanvas::operator=(o);
    uninitializedCopy(o);
    return *this;
  }
  int operator==(const CConxCanvas &o) const
  {
    return (artists == o.artists);
  }
  int operator!=(const CConxCanvas &o) const { return !operator==(o); }

  ConxModlType getModel() const { return modl; }
  void setModel(ConxModlType modl);
  ostream &printOn(ostream &o) const;
  virtual void masterDraw(); // non-const because there are side effects.
  void append(const CConxArtist *m) throw(const char *); // you still own m
  void clearDrawables();
  size_t numArtists() const { return artists.size(); }

protected:
  static const char *modelToString(ConxModlType modl);


private: // operations
  void uninitializedCopy(const CConxCanvas &o)
  {
    artists = o.artists;
  }

private: // attributes
  ConxModlType modl;
  CConxSimpleArray<const CConxArtist *> artists;
}; // class CConxCanvas


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxDumbCanvas);
OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxCanvas);
OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxDrawCanvas);

#endif // GPLCONX_HGMCANVAS_CXX_H
