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
  C++ OpenGL Drawing Canvas class.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <math.h>
#include <iostream.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "glcanvas.hh"


// DLC TODO add OpenGL error checking and throw if errors are found.

#define cxxgreen() setDrawingColor(CConxNamedColor(CConxNamedColor::GREEN))

Boole CConxGLCanvas::isInitialized = FALSE;

NF_INLINE
void CConxGLCanvas::initDraw()
{
  if (!isInitialized) {
    // from void conx_gl_first_init(void);
    glShadeModel(GL_FLAT);
    cxxgreen();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    isInitialized = TRUE;
  }

  gluOrtho2D(getXmin(), getXmax(), getYmin(), getYmax());
  glViewport(0, 0, getWidth(), getHeight());
}

NF_INLINE
SDID CConxGLCanvas::startSD() throw(int)
// DLC doc this
{
  GLuint new_disp_list = glGenLists(1);
  if (new_disp_list == 0) throw 0;
  glNewList(new_disp_list, GL_COMPILE_AND_EXECUTE);
  if (highestSD < lowestSD) {
    lowestSD = highestSD = (SDID)new_disp_list;
  } else {
    if ((SDID)new_disp_list > highestSD)
      highestSD = (SDID)new_disp_list;
    if ((SDID)new_disp_list < lowestSD)
      lowestSD = (SDID)new_disp_list;
  }
  return (SDID)new_disp_list;
}

NF_INLINE
void CConxGLCanvas::stopSD()
// Notifies this object that we are finished with the most recently called
// startSD (in any instance of class CConxGLCanvas, actually!)
{
  glEndList();
}

NF_INLINE
void CConxGLCanvas::deleteSD(SDID id)
// Deletes the stored drawing that id represents, or does nothing if
// id does not represent a stored drawing.
//
// A class (static) method in actuality, i.e. you can define an SD
// with one instance and delete it with another.
{
  if (id >= lowestSD && id <= highestSD) {
    glDeleteLists((GLuint)id, 1);
    // The following adjustments may not be entirely accurate,
    // but they will cause the sole assigned SD's deletion to
    // cause lowestSD to be greater than highestSD.
    if (id == lowestSD) ++lowestSD;
    if (id == highestSD) --highestSD;
  }
}

NF_INLINE
void CConxGLCanvas::deleteAllSD()
{
  if (lowestSD <= highestSD) {
    glDeleteLists((GLuint)lowestSD, (GLsizei)(highestSD-lowestSD+1));
    lowestSD = 3;
    highestSD = 2;
  }
}

NF_INLINE
void CConxGLCanvas::executeSD(SDID id)
// DLC doc this
{
  // DLC if (id < lowestSD || id > highestSD) throw "Can't work!!"
  // will catch some but not necessarily all errors.  glIsList gets all,
  // I think.
  glCallList((GLuint) id);
}

NF_INLINE
void CConxGLCanvas::beginDraw(DrawingType dt)
// Sets up the environment so that you can call drawVertex and have it
// make either a new point or a vertex of a line or a vertex of a line strip.
{
  assert(isInitialized);
  switch (dt) {
  case POINTS: glBegin(GL_POINTS); break;
  case LINE_STRIP: glBegin(GL_LINE_STRIP); break;
  default: assert(dt == LINES); glBegin(GL_LINES); break;
  }
}

NF_INLINE
void CConxGLCanvas::endDraw()
// Ends the most recent beginDraw (from any instance of this class!)
{
  glEnd();
}

NF_INLINE
void CConxGLCanvas::drawVertex(double x, double y)
// Draw a vertex which is interpreted via the parameter given to the
// most recent beginDraw (in any instance of this class!)
// This only works in between a beginDraw...endDraw()
{
  glVertex2d((GLdouble) x, (GLdouble) y);
}

NF_INLINE
void CConxGLCanvas::setDrawingColor(const CConxColor &C)
// Affects upcoming drawVertex calls (in any instance of this class!)
{
  glColor3d((GLdouble)C.getR(), (GLdouble)C.getG(), (GLdouble)C.getB());
}

NF_INLINE
void CConxGLCanvas::setPointSize(double pSize)
// Affects upcoming drawVertex calls (in any instance of this class!)
// when you are drawing POINTS.  Call this before beginDraw().
{
  assert(pSize > 0.0); // DLC
  if (pSize > 0.0) {
    glPointSize(pSize);
  }
}

NF_INLINE
void CConxGLCanvas::flush()
// Causes the drawing to be presented to the user.  Call after endDraw()
{
  glFlush();
}

NF_INLINE
void CConxGLCanvas::clear()
{
  setDrawingColor(CConxNamedColor(CConxNamedColor::BLACK));
  glRectd(getXmin(), getYmin(), getXmax(), getYmax());
  //  glClear(GL_COLOR_BUFFER_BIT);
}

NF_INLINE
void CConxGLCanvas::drawTopSemiCircle(double x, double y, double r)
{
  drawArc(x, y, r, 0.0, M_PI);
}

NF_INLINE
void CConxGLCanvas::drawCircle(double x, double y, double r)
{
  drawArc(x, y, r, 0.0, M_PI * 2.0);
}

NF_INLINE
void CConxGLCanvas::drawArc(double x, double y, double r, double t0, double t1)
// t0 and t1 are in radians.  Use t0 = 0, t1 = PI/2 for the quarter-circle arc
// in the first quadrant, e.g.
{
  // DLC needs to be a stored drawing, plus test t0 and t1, plus? clipping
  assert(r > 0);
  beginDraw(LINE_STRIP);
  for (double t = t0; t <= t1; t += 0.005/* DLC tstep */) {
    drawVertex(x + r*cos(t), y + r*sin(t));
  }
  endDraw();
}


OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxGLCanvas)
