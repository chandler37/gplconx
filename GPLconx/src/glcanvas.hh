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
  C++ OpenGL Drawing Canvas classes.
*/

#ifndef GPLCONX_GLCANVAS_CXX_H
#define GPLCONX_GLCANVAS_CXX_H 1

#include "canvas.hh"

//////////////////////////////////////////////////////////////////////////////
// An OpenGL canvas that you can draw on.
//
// Please note that this implementation should be all static functions,
// i.e. you can call executeSD() in any instance and achieve the same result.
// Some other implementation might not have this property, so the functions
// are not static...
//
// DLC hold up... are we using different GL contexts for each instance
// by design???   Even then, a wrapper around a state machine makes copying
// semantics a bit esoteric.
class CConxGLCanvas : VIRT public CConxCanvas {
  CCONX_CLASSNAME("CConxGLCanvas")
  DEFAULT_PRINTON()
public:
  CConxGLCanvas() : lowestSD(3), highestSD(2) { }
  CConxGLCanvas(const CConxGLCanvas &o);
  CConxGLCanvas &operator=(const CConxGLCanvas &o);
  ~CConxGLCanvas();

  SDID startSD() throw(int);
  void stopSD();
  void deleteSD(SDID id);
  void deleteAllSD();
  void executeSD(SDID id);

  void drawArc(double x, double y, double r, double t0, double t1);
  void drawCircle(double x, double y, double r);
  void drawTopSemiCircle(double x, double y, double r);
  void beginDraw(DrawingType dt);
  void endDraw();
  void drawVertex(double x, double y);
  void setDrawingColor(const CConxColor &C);
  void setPointSize(double pSize);
  void flushQueue();
  void clear();
  void initDraw();
  void drawByBresenham(const CConxPoint &lb, const CConxPoint &rb,
                       DFN *f, const CConxSimpleArtist *sa);


private: // operations
  void uninitializedCopy(const CConxGLCanvas &o);
  static void bresVertex2(double a, double b, void *kArg);
  static double bresMetric(Pt middle, void *t);
  static void bresTrace(Pt middle, ConxDirection last,
                        double dw, double dh,
                        ConxMetric *func, void *fArg,
                        ConxContinueFunc *keepgoing, void *kArg);

  // The Bresenham method requires this to know when to stop.
  static int bresKeepGoing(Pt middle, Pt oldmiddle, void *t);


private: // attributes
  // These are invalid if and only if highestSD < lowestSD.
  // They allow deleteAllSD() to work.
  SDID lowestSD;
  SDID highestSD;

  DFN *savedFoo;
  const CConxSimpleArtist *savedFooArg;

  static Boole isInitialized;
}; // class CConxGLCanvas


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxGLCanvas);

#endif // GPLCONX_GLCANVAS_CXX_H
