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

#ifndef CONXV_VIEWER_H
#define CONXV_VIEWER_H 1

#include <stdlib.h>
#include <math.h>

#include "point.h"
#include "globals.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433
#endif

/* From GNU <math.h>: */
#ifndef M_SQRT1_2
#define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */
#endif

#define UNKNOWN_STR "Unkn0wn" /* Easy grep use */
#define VERTICALNESS .000001
#define WIDTH 300
#define HEIGHT 300

#define TYPEI -1 /* A vertical (type I) line; such a thing is purely a UHP concept. */
/* DLC should be a floating point number, right? */

#define KINFINITY 1024
#define POINTSIZE 6.0
#define DIAMETER -1
#define ARBITRARILYSMALL .0001

void conx_display(ConxModlType modl);
void conx_gl_init(ConxModlType, int w, int h);
void conx_gl_first_init(void);
#ifdef DLC
void mkptAll(void);
#endif

size_t conx_query(ConxModlType typ, ConxMenuChoice mc,
                  double *d, size_t d_size);
extern inline
void conx_reshape(ConxModlType modl, int w, int h);
ConxMenuChoice conx_string2menu_choice(const char *s);
const char *conx_menu_choice2string(ConxMenuChoice mc);
const char *conx_modelenum2string(ConxModlType m);
const char *conx_modelenum2short_string(ConxModlType m);
extern inline
void conx_add_new_disp_command(ConxModlType mdl, ConxMenuChoice mc);
extern inline
void conx_add_datum(double d, ConxModlType mdl);
int conx_mouse(ConxModlType mdl, ConxMouseChoice cmc, int x, int y,
               int win_height, ConxMenuChoice button_function);
int conx_menufunc(ConxMenuChoice a, ConxModlType modl, int win_height);
extern inline
int conx_num_inputs(ConxMenuChoice a);
void conxpd_getCenterAndRadiusABC(Pt A, Pt B, Pt *C, double *r);
void conxk_graphsegmentAB(Pt A, Pt B);
void conxp_getPtNearXonar(Pt X, double a, double r, Pt *A, double computol);

#define sqr(x) ((x) * (x))
#define myabs(x) fabs(x)
#define myequals(a, b, tol) (myabs((a)-(b)) <= (tol))
#define swap(a, b) { \
                      double swap_temp = *(a); \
                      *(a) = *(b); \
                      *(b) = swap_temp; \
                   }
#define greater(x, y) (((x) > (y)) ? ((x)) : ((y)))
#define lesser(x, y) (((x) > (y)) ? ((y)) : ((x)))

extern inline
int conx_pt_not_between(Pt A, Pt B, Pt C);
double conxp_distABar(Pt A, Pt B, double a, double r);
void conxp_geteq(Pt A, Pt B, double *a, double *r);
double conxp_distAB(Pt A, Pt B);
void conxhm_pdtokAB(Pt, Pt *);
void conxhm_ktopAB(Pt, Pt *);
void conxhm_ptokAB(Pt, Pt *);
double conxk_distAB(Pt, Pt);
double conxk_dist(double x, double y, double xx, double yy);
double conxpd_distAB(Pt A, Pt B);
void conxk_getPtNearXonmb(Pt X, double m, double b, Pt *A, double computol);
double conxk_distFrommbX(double m, double b, Pt X, double computol);
/*void getendptsCr(Pt C, double r, Pt *enda, Pt *endb); doesn't work DLC */
double conxpd_distFromcX(double cx, double cy, Pt X, double computol);
double conxp_distFromarX(double a, double r, Pt X, double computol);
void conxhm_ktop(double px, double py, double *kx, double *ky);
void conxhm_ptok(double kx, double ky, double *px, double *py);
void conxhm_ktopd(double u, double v, double *x, double *y);
void conxhm_pdtok(double x, double y, double *u, double *v);
void conxhm_pdtop(double x, double y, double *u, double *v);
void conxhm_pdtopAB(Pt P, Pt *K);
void conxhm_ktopdAB(Pt P, Pt *K);
void conxhm_ptopd(double u, double v, double *x, double *y);
void conxhm_ptopdAB(Pt P, Pt *K);
void conxhm_getarfromc(double cx, double cy, double *a, double *r);
void conxhm_getmbfromar(double a, double r, double *m, double *b);
void conxhm_getcfrommb(double m, double b, double *cx, double *cy);
void conxhm_getcfromar(double a, double r, double *cx, double *cy);
void conxhm_getmbfromc(double cx, double cy, double *m, double *b);
void conxhm_getendptsmb(double m, double b, Pt *A, Pt *B);
void conxhm_getendptsc(double, double, Pt *, Pt *); /* not complete as of July 31 DLC */
void conxhm_getarfrommb(double m, double b, double *a, double *r);
void conxhm_getmbandAB(double a, double r, double *m, double *b, Pt *A, Pt *B);
void conxpd_getCenterAndRadius(double x1, double y1, double x2, double y2,
                               double *cx, double *cy, double *rad);
void conx_quickdraw_circle(Pt A, double r, ConxModlType typ, double tstp);
extern inline
void conx_screen2model(int x, int y, double xa, double xb, double yc,
                       double yd, int sh, Pt *ModelCoords);
void conxhm_p_getEstarted(Pt *LB, Pt *RB, Pt f1, Pt f2, double cdist);
void conxhm_p_getHstarted(Pt *LB, Pt *RB, Pt f1, Pt f2, double cdist);
void conxhm_p_getPstarted(Pt *LB, Pt *RB, Pt f1, double line_a, double line_r,
                          double computol);
void conxhm_p_getEQstarted(Pt *LB, Pt *RB, double line_a, double line_r,
                           double cdist, double ymin, double ymax);
double conxpd_getr(double cx, double cy);
/* end of hypmath.c */
void conx_bres_trace(Pt middle, ConxDirection last, double dw, double dh,
                     ConxMetric *func, void *fArg, ConxContinueFunc *keepgoing,
                     ConxPointFunc *pfunc, void *pArg);
void conx_bresenham(ConxBresenhamStarter *getB, ConxMetric *func,
                    void *fArg, double delta_x, double delta_y,
                    ConxContinueFunc *keepgoing,
                    ConxBresTraceFunc *bres_trace);
/* end of bres2.c */
void conx_longway(ConxMetric *test, void *fArg, ConxModlType modl,
                  double tlrance, double delta_x, double delta_y,
                  double x_min, double x_max, double y_min, double y_max,
                  ConxPointFunc *pfunc, void *pArg);
/* end of longways.c */


void conxk_graphmb(double m, double b);
void conxk_graphlineAB(Pt A, Pt B);
void conxp_graphsegment(Pt A, Pt B, double tstp);
void conxp_graphar(double a, double r, double tstp, double y_min,
                   double y_max);
void conxp_graphlineAB(Pt A, Pt B, double tstp, double y_min, double y_max);
void conxpd_graphlineAB(Pt f1, Pt f2, double tstp);
void conxpd_graphsegment(Pt A, Pt B, double tstp);
void conxpd_graphc(double cx, double cy, double tstp);
void conx_draw_arc(double x, double y, double r, double theta_1,
                   double theta_2, double tstp,
                   ConxPoint2DConverterFunc *converter,
                   ConxPointFunc *pfunc, void *pArg);
/* end of lines.c */
#define FLUSH()  /* do nothing, wait until display(model) */

#endif /* CONXV_VIEWER_H */
