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
   Functions to isolate the drawing functions from the rest of the code,
   so that OpenGL is not the only game in town.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <assert.h>

#include <GL/gl.h>

#include "viewer.h"
#include "util.h"
#include "point.h"
#include "globals.h"
#include "gl.h"

/* glIsList(a) must return 0 when and only when
   we have not yet called glNewList(a, ...) or we have deleted display list
   a.
*/
#define CONX_BEGIN_DISP_LIST(dl) \
          if (dl >= 0) { \
            if (glIsList(dl)) { \
              glCallList(dl); \
              return; \
            } else { \
              glNewList(dl, GL_COMPILE_AND_EXECUTE); \
            } \
          }

#define CONX_END_DISP_LIST(dl) \
          if (dl >= 0) { \
            glEndList(); \
          }

static const
GLfloat conx_colors[][4] = {
  { 0.0, 1.0, 0.3, 0.0 }, /* CONXCLR_GREEN */
  { 1.0, 1.0, 1.0, 0.0 }, /* CONXCLR_WHITE */
  { 0.0, 0.0, 0.0, 0.0 }, /* CONXCLR_BLACK */
  { 0.5, 0.5, 1.0, 0.0 }, /* CONXCLR_BLUE */
  { 1.0, 0.0, 0.0, 0.0 }, /* CONXCLR_RED */
  { 0.0, 1.0, 1.0, 0.0 }, /* CONXCLR_PARABOLA */
  { 1.0, 1.0, 0.0, 0.0 }, /* CONXCLR_ELLIPSE */
  { 1.0, 0.0, 1.0, 0.0 }  /* CONXCLR_CIRCLE */
/* { 0.2, 0.7, 0.3, 0.0 }; */
};


inline static
void conx_gl_vertex2(double a, double b, void *ignored);
inline static
void conx_gl_bres_trace(Pt middle, ConxDirection last, double dw, double dh,
                        ConxMetric *func, void *fArg,
                        ConxContinueFunc *keepgoing, void *kArg);


void conx_set_drawing_color(ConxColorEnum color)
{
  glColor4fv(conx_colors[color]);
}

void conx_draw_point(Pt A)
{
  glPointSize(POINTSIZE);
  red();
  glBegin(GL_POINTS);
  glVertex2f(A.x, A.y);
  glEnd();
  glPointSize(1.0);
  green();
  FLUSH();
}

void conx_gl_vertex2(double a, double b, void *ignored)
{
  glVertex2f((GLfloat)a, (GLfloat)b);
}

void conx_gl_longway(ConxMetric *test, ConxModlType modl, double tlrance,
                     double delta_x, double delta_y,
                     double x_min, double x_max, double y_min, double y_max,
                     ConxDispList dl)
{
  CONX_BEGIN_DISP_LIST(dl);
  glBegin(GL_POINTS);
  conx_longway(test, NULL, modl, tlrance, delta_x, delta_y, x_min, x_max,
               y_min, y_max, conx_gl_vertex2, NULL);
  glEnd();
  FLUSH();
  CONX_END_DISP_LIST(dl);
}

void conx_gl_bres_trace(Pt middle, ConxDirection last, double dw, double dh,
                        ConxMetric *func, void *fArg,
                        ConxContinueFunc *keepgoing, void *kArg)
{
  glBegin(GL_POINTS);
  conx_bres_trace(middle, last, dw, dh, func, fArg, keepgoing, kArg,
                  conx_gl_vertex2, NULL);
  glEnd();
  FLUSH();
}

void conx_gl_bresenham(ConxBresenhamStarter *getB, ConxMetric *func,
                       void *fArg, double delta_x, double delta_y,
                       ConxContinueFunc *keepgoing, ConxDispList dl)
{
  Pt LB, RB;
  CONX_BEGIN_DISP_LIST(dl);
  assert(getB != NULL);
  (*getB)(&LB, &RB);
  conx_bresenham(LB, RB, func, fArg,
                 delta_x, delta_y, keepgoing, NULL, conx_gl_bres_trace);
  green();
  CONX_END_DISP_LIST(dl);
}

void conx_gl_clear(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
}

void conx_gl_draw_lineAB(Pt A, Pt B)
/* DLC doc and use this */
{
  conx_gl_draw_line(A.x, A.y, B.x, B.y);
}

void conx_gl_draw_line(double Ax, double Ay, double Bx, double By)
{
  glBegin(GL_LINES);
  glVertex2f(Ax,Ay);
  glVertex2f(Bx,By);
  glEnd();
  FLUSH();
}

void conx_gl_draw_arc(double x, double y, double r, double theta_1,
                      double theta_2, double tstp,
                      ConxPoint2DConverterFunc *converter, ConxDispList dl)
{
  CONX_BEGIN_DISP_LIST(dl);
  glBegin(GL_LINE_STRIP);
  conx_draw_arc(x, y, r, theta_1, theta_2, tstp, converter, conx_gl_vertex2,
                NULL);
  glEnd();
  FLUSH();
  CONX_END_DISP_LIST(dl);
}

void conx_gl_kill_lists(ConxDispList first, ConxDispList num)
/* Any display lists in [first, first+num) are deleted. */
{
  glDeleteLists(first, num);
}

void conx_gl_flush(void)
{
  glFlush();
}

