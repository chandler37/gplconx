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

#ifndef GPLCONX_GL_H
#define GPLCONX_GL_H 1

#include "point.h"

typedef long ConxDispList; /* Negative values mean "no display list" */

extern inline
void conx_gl_clear(void);
extern inline
void conx_set_drawing_color(ConxColorEnum color);

#define green() conx_set_drawing_color(CONXCLR_GREEN)
#define white() conx_set_drawing_color(CONXCLR_WHITE)
#define blue() conx_set_drawing_color(CONXCLR_BLUE)
#define red() conx_set_drawing_color(CONXCLR_RED)
#define pcol() conx_set_drawing_color(CONXCLR_PARABOLA)
#define ecol() conx_set_drawing_color(CONXCLR_ELLIPSE)
#define ccol() conx_set_drawing_color(CONXCLR_CIRCLE)

extern inline
void conx_draw_point(Pt A);
extern inline
void conx_gl_longway(ConxMetric *test, ConxModlType modl, double tlrance,
                     double delta_x, double delta_y,
                     double x_min, double x_max, double y_min, double y_max,
                     ConxDispList dl);
extern inline
void conx_gl_bresenham(ConxBresenhamStarter *getB, ConxMetric *func,
                       void *fArg, double delta_x, double delta_y,
                       ConxContinueFunc *keepgoing, ConxDispList dl);
extern inline
void conx_gl_kill_lists(ConxDispList first, ConxDispList num);
extern inline
void conx_gl_flush(void);
extern inline
void conx_gl_draw_lineAB(Pt A, Pt B);
extern inline
void conx_gl_draw_line(double Ax, double Ay, double Bx, double By);
void conx_gl_draw_arc(double x, double y, double r, double theta_1,
                      double theta_2, double tstp,
                      ConxPoint2DConverterFunc *converter, ConxDispList dl);

/* Draws a complete circle. */
#define conx_gl_draw_circle(x, y, r, tstp, dl) \
    conx_gl_draw_arc((x), (y), (r), 0.0, 2.0*M_PI, (tstp), NULL, (dl))

#endif /* GPLCONX_GL_H */
