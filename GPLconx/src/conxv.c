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
   Functions to draw three models of hyperbolic geometry,
   plus functions that do various mathematical tricks that require
   variables that we wish to be global to only this file.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "viewer.h"
#include "util.h"
#include "point.h"
#include "gl.h"

#define RESET_DISPLAY_LISTS(first, num) \
     conx_gl_kill_lists(first, num)

#define HANDLE_BORDERS_DISPLAY(borders, modl, tstep) \
          if (borders&&(modl!=CONX_POINCARE_UHP)) { \
            white(); \
            conx_gl_draw_circle(0.0, 0.0, 1.0, tstep, 8 /* DLC dl */); \
            green(); \
          }

#define LONGWAY(func) \
         conx_gl_longway(func, modl, tol, dx[modl], dy[modl], xmin[modl], \
                         xmax[modl], ymin[modl], ymax[modl], 9 /* DLC dl */)

#define BRESENHAM(sfunc, mfunc) \
         conx_gl_bresenham(sfunc, mfunc, NULL, dx[modl], dy[modl], \
                           bres_keep_going, icount + 1 /* DLC dl */)

static double tol=0.001;
static double oldcd[CONX_NUM_MODELS], olda[CONX_NUM_MODELS];
static double oldtol[CONX_NUM_MODELS], oldcomptol[CONX_NUM_MODELS];
static double oldtstep[CONX_NUM_MODELS], oldxmin[CONX_NUM_MODELS];
static double oldxmax[CONX_NUM_MODELS], oldymin[CONX_NUM_MODELS];
static double oldymax[CONX_NUM_MODELS], oldr[CONX_NUM_MODELS];

static Pt oldf1, oldf2;
static int disp[500][CONX_NUM_MODELS];
static int top[CONX_NUM_MODELS]={-1, -1, -1};
static double data[1000][CONX_NUM_MODELS];
static int dtop[CONX_NUM_MODELS]={-1, -1, -1};

/* A conic section can have a distance associated with it, which is the radius
   of a circle, e.g.
*/
static double conicdistance=1.01;
static double comptol = .000000001;
static GLboolean borders=GL_TRUE;
static GLboolean autofoci=GL_TRUE;
static GLboolean garnish=GL_TRUE;

/* Only give the Poincare upper half plane coordinates for the foci initially.
   The coordinates in the other models are implied.
*/
static Pt focus1[CONX_NUM_MODELS]={{0.5, 0.75},{0.0, 0.0},{ 0.0, 0.0}};
static Pt focus2[CONX_NUM_MODELS]={{0.2, 0.75},{0.0, 0.0},{ 0.0, 0.0}};
static double tstep=0.005;
static double linea[CONX_NUM_MODELS]={0.0, 0.3, 1};
static double liner[CONX_NUM_MODELS]={0.5, 0.3,  0.6};
static double xmin[CONX_NUM_MODELS]={-1.0, -1.03, -1.03};
static double xmax[CONX_NUM_MODELS]={1.0, 1.03, 1.03};
static double ymin[CONX_NUM_MODELS]={0.0, -1.03, -1.03};
static double ymax[CONX_NUM_MODELS]={2.0, 1.03, 1.03};
static double dx[CONX_NUM_MODELS], dy[CONX_NUM_MODELS];
static ConxModlType saved_hgmodel;

/* Global variables: */
/* loglevel == 0 is no logging, loglevel == 16000 is all output.
   100 gives you some but not all, 101 gives you all currently.
*/
/* int loglevel = 801; to debug bres2.c */
int loglevel = 101;


typedef void (ConxFullBresStarter) (Pt *, Pt *, Pt, Pt, double);

static void conxpd_get_started(Pt *LB, Pt *RB, ConxFullBresStarter *func);
static void conxk_get_started(Pt *LB, Pt *RB, ConxFullBresStarter *func);
inline static void conx_getpf1(void);
inline static void conx_getpf2(void);
inline static void conx_getpdf1(void);
inline static void conx_getpdf2(void);
inline static void conx_disp(ConxModlType modl, int icount, int *jcount);
static void displaystore(ConxModlType modl);
static void displayrecall(ConxModlType modl);
static void displayswitch(ConxMenuChoice choice, int *jc, ConxModlType modl);
static void conxpd_disp(int icount, int *jcount, ConxModlType modl);
static void kdisp(int icount, int *jcount, ConxModlType modl);
static void pdisp(int icount, int *jcount, ConxModlType modl);
inline static int bres_keep_going(Pt middle, Pt oldmiddle, void *ignored);

/* For conx_gl_longway: */
inline static double conxp_circle(Pt X, void *ignored);
inline static double conxpd_circle(Pt X, void *ignored);
inline static double conxk_circle(Pt X, void *ignored);
inline static double conxp_ellipse(Pt X, void *ignored);
inline static double conxk_ellipse(Pt X, void *ignored);
inline static double conxpd_ellipse(Pt X, void *ignored);
inline static double conxp_hyperbola(Pt X, void *ignored);
inline static double conxk_hyperbola(Pt X, void *ignored);
inline static double conxpd_hyperbola(Pt X, void *ignored);
inline static double conxp_parabola(Pt X, void *ignored);
inline static double conxk_parabola(Pt X, void *ignored);
inline static double conxpd_parabola(Pt X, void *ignored);
inline static double conxp_eqdist(Pt X, void *ignored);
inline static double conxk_eqdist(Pt X, void *ignored);
inline static double conxpd_eqdist(Pt X, void *ignored);

/* For Bresenham: */
static void conxp_getHstarted(Pt *LB, Pt *RB);
static void conxp_getEstarted(Pt *LB, Pt *RB);
static void conxp_getEQstarted(Pt *LB, Pt *RB);
static void conxp_getPstarted(Pt *LB, Pt *RB);
static void conxk_getEstarted(Pt *LB, Pt *RB);
static void conxk_getHstarted(Pt *LB, Pt *RB);
static void conxk_getPstarted(Pt *LB, Pt *RB);
static void conxk_getEQstarted(Pt *LB, Pt *RB);
static void conxpd_getEQstarted(Pt *LB, Pt *RB);
static void conxpd_getEstarted(Pt *LB, Pt *RB);
static void conxpd_getPstarted(Pt *LB, Pt *RB);
static void conxpd_getHstarted(Pt *LB, Pt *RB);

size_t conx_query(ConxModlType typ, ConxMenuChoice mc,
                  double *d, size_t d_size)
/*
@conxdox
@deftypefn int conx_query (ConxModlType @var{typ}, ConxMenuChoice @var{mc}, double *@var{d}, size_t @var{d_size})

   Gets parameters that correspond to @var{mc} unless @var{d} @equiv{}
   @Conx_NULL{} or if @var{d_size}, the size of the array @var{d}, is not
   large enough to accommodate the parameters, or if @var{mc} is not
   recognized.  Returns the number of parameters that were stored in the
   array @var{d}, or 0 on failure.
@end deftypefn
@end conxdox
*/
{
  if (d_size < 1 || d == NULL) return 0;
  switch (mc) {
  case CONXCMD_SHOWCD:
    *d = conicdistance;
    break;
  case CONXCMD_SHOWTOL:
    *d = tol;
    break;
  case CONXCMD_SHOWCOMPTOL:
    *d = comptol;
    break;
  case CONXCMD_SHOWA:
    *d = linea[typ];
    break;
  case CONXCMD_SHOWR:
    *d = liner[typ];
    break;
  case CONXCMD_SHOWLOGLEVEL:
    *d = (double) loglevel;
    break;
  case CONXCMD_SHOWF1:
    if (d_size < 2) return 0;
    *d++ = focus1[typ].x;
    *d++ = focus1[typ].y;
    return 2;
  case CONXCMD_SHOWF2:
    if (d_size < 2) return 0;
    *d++ = focus2[typ].x;
    *d++ = focus2[typ].y;
    return 2;
  case CONXCMD_SHOWTSTEP:
    *d = tstep;
    break;
  case CONXCMD_SHOWXXYY:
    if (d_size < 4) return 0;
    *d++ = xmin[typ];
    *d++ = xmax[typ];
    *d++ = ymin[typ];
    *d++ = ymax[typ];
    return 4;
  default:
    return 0;
  }
  return 1;
}

void conx_reshape(ConxModlType modl, int w, int h)
/* Call when we reshape the window to size w x h. */
{
  /* Recalculate display lists for many items.  Note that if you reshape in the
     Poincare UHP, then an equidistant curve in the Poincare Disk, e.g., will still
     have a valid display list. DLC
  */
  /* Why not do this in GETXXYY? DLC */

  if (w < h) h = w; else w = h;

  glViewport(0, 0, w, h);

  dx[modl]=(xmax[modl]-xmin[modl])/(double)w;
  dy[modl]=(ymax[modl]-ymin[modl])/(double)h;
}

void conx_add_datum(double d, ConxModlType mdl)
/* So that data and dtop need not be global variables. */
{
  data[++dtop[mdl]][mdl] = d;
}

void conx_add_new_disp_command(ConxModlType mdl, ConxMenuChoice mc)
/* So that disp and top need not be global variables. */
{
  disp[++top[mdl]][mdl] = mc;
}

void displaystore(ConxModlType mdl)
/* store all vars for recall later */
{
  oldcd[mdl]=conicdistance;
  oldtol[mdl]=tol;
  oldcomptol[mdl]=comptol;
  oldtstep[mdl]=tstep;
  oldxmin[mdl]=xmin[mdl];
  oldxmax[mdl]=xmax[mdl];
  oldymin[mdl]=ymin[mdl];
  oldymax[mdl]=ymax[mdl];

  oldf1=focus1[CONX_KLEIN_DISK];
  oldf2=focus2[CONX_KLEIN_DISK];
  /* Yes, see how displayrecall retrieves for all other models from the
     CONX_KLEIN_DISK coordinates.
  */

  olda[mdl]=linea[mdl];
  oldr[mdl]=liner[mdl];
}

void displayrecall(ConxModlType mdl)
{
  conicdistance=oldcd[mdl];
  tol=oldtol[mdl];
  comptol=oldcomptol[mdl];
  tstep=oldtstep[mdl];
/*  xmin[mdl]=oldxmin[mdl];
  xmax[mdl]=oldxmax[mdl];
  ymin[mdl]=oldymin[mdl];
  ymax[mdl]=oldymax[mdl]; */
  focus1[CONX_KLEIN_DISK]=oldf1;
  focus2[CONX_KLEIN_DISK]=oldf2;
  conx_getpf1();
  conx_getpf2();
  conx_getpdf1();
  conx_getpdf2();
  linea[mdl]=olda[mdl];
  liner[mdl]=oldr[mdl];
}
  
void conx_getpdf1(void)
{
  conxhm_ktopdAB(focus1[CONX_KLEIN_DISK], &focus1[CONX_POINCARE_DISK]);
}

void conx_getpdf2(void)
{
  conxhm_ktopdAB(focus2[CONX_KLEIN_DISK], &focus2[CONX_POINCARE_DISK]);
}

void conx_getpf1(void)
/* Calculate the Poincare UHP coordinates of focus1 */
{
  conxhm_ktopAB(focus1[CONX_KLEIN_DISK], &focus1[CONX_POINCARE_UHP]);
}

void conx_getpf2(void)
{
  conxhm_ktopAB(focus2[CONX_KLEIN_DISK], &focus2[CONX_POINCARE_UHP]);
}

void conx_disp(ConxModlType modl, int icount, int *jcount)
/* Draw one item, e.g. a circle, based on the model. */
{
  if (modl==CONX_POINCARE_DISK) {
    conxpd_disp(icount, jcount, modl);
  } else if (modl==CONX_KLEIN_DISK) {
    kdisp(icount, jcount, modl);
  } else {
    assert(modl==CONX_POINCARE_UHP);
    pdisp(icount, jcount, modl);
  }
}

void displayswitch(ConxMenuChoice choice, int *jc, ConxModlType modl)
/* These items work the same no matter which model we are "drawing" in. */
{
  int jcount;
  jcount=*jc;
  

  switch (choice) {
  case CONXCMD_BCIRCLE:
    LOGGG4(LOGG_FULL, "\nModel %s f1 (%f, %f) conicdistance= %f\n",
           conx_modelenum2short_string(modl), focus1[modl].x,
           focus1[modl].y, conicdistance);
    conx_quickdraw_circle(focus1[modl], conicdistance, modl, tstep);
    break;
  case CONXCMD_GETCD:
    conicdistance=data[++jcount][modl];
    break;
  case CONXCMD_GETA:
    linea[modl]=data[++jcount][modl];
    if (modl==CONX_POINCARE_DISK) {
      conxhm_getmbfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                        &linea[CONX_KLEIN_DISK], &liner[CONX_KLEIN_DISK]);
      conxhm_getarfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                 &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
    }
    else if (modl==CONX_POINCARE_UHP) {
      conxhm_getmbfromar(linea[modl], liner[modl], &linea[CONX_KLEIN_DISK],
                  &liner[CONX_KLEIN_DISK]);
      conxhm_getcfromar(linea[modl], liner[modl], &linea[CONX_POINCARE_DISK],
                 &linea[CONX_POINCARE_DISK]);
    }
    else {
      conxhm_getcfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                 &linea[CONX_POINCARE_DISK], &liner[CONX_POINCARE_DISK]);
      conxhm_getarfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                  &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
    }
    break;
  case CONXCMD_GETR:
    liner[modl]=data[++jcount][modl];
    if (modl==CONX_POINCARE_DISK) {
      conxhm_getmbfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                 &linea[CONX_KLEIN_DISK], &liner[CONX_KLEIN_DISK]);
      conxhm_getarfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                 &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
    } else if (modl==CONX_POINCARE_UHP) {
      conxhm_getmbfromar(linea[modl], liner[modl], &linea[CONX_KLEIN_DISK],
                  &liner[CONX_KLEIN_DISK]);
      conxhm_getcfromar(linea[modl], liner[modl], &linea[CONX_POINCARE_DISK],
                 &linea[CONX_POINCARE_DISK]);
    } else {
      conxhm_getcfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                 &linea[CONX_POINCARE_DISK], &liner[CONX_POINCARE_DISK]);
      conxhm_getarfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                  &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]); 
    }
    break;
  case CONXCMD_FOCI: 
    conx_draw_point(focus2[modl]); /* fall through */
  case CONXCMD_FOCUS: 
    conx_draw_point(focus1[modl]);
    break;
  case CONXCMD_GETF1:
    focus1[modl].x=data[++jcount][modl];
    focus1[modl].y=data[++jcount][modl];
    if (modl==CONX_POINCARE_DISK) {
      conxhm_pdtokAB(focus1[CONX_POINCARE_DISK], &focus1[CONX_KLEIN_DISK]);
      conx_getpf1();
    } else if (modl==CONX_POINCARE_UHP) {
      conxhm_ptokAB(focus1[CONX_POINCARE_UHP], &focus1[CONX_KLEIN_DISK]);
      conx_getpdf1();
    } else {
      conx_getpf1();
      conx_getpdf1();
    }
#ifdef DLC
    mkptAll();
#else
    conx_draw_point(focus1[modl]);
    conx_draw_point(focus2[modl]);
#endif

    break;
  case CONXCMD_GETF2:
    focus2[modl].x=data[++jcount][modl];
    focus2[modl].y=data[++jcount][modl];
    if (modl==CONX_POINCARE_DISK) {
      conxhm_pdtokAB(focus2[CONX_POINCARE_DISK], &focus2[CONX_KLEIN_DISK]);
      conx_getpf2();
    }
    if (modl==CONX_POINCARE_UHP) {
      conxhm_ptokAB(focus2[CONX_POINCARE_UHP], &focus2[CONX_KLEIN_DISK]);
      conx_getpdf2();
    } else {
      conx_getpf2();
      conx_getpdf2();
    }
#ifdef DLC
    mkptAll();
#else
    conx_draw_point(focus1[modl]);
    conx_draw_point(focus2[modl]);
#endif

    break;
  case CONXCMD_GETTOL:
    tol=data[++jcount][modl];
    break;
  case CONXCMD_GETCOMPTOL:
    comptol=data[++jcount][modl];
    break;
  case CONXCMD_GETTSTEP:
    tstep=data[++jcount][modl];
    break;
/*
  case CONXCMD_GETLOGLEVEL:
          / * do nothing * / break;
  case CONXCMD_GETXXYY:

Don't do anything, this doesn't make sense to be replayed. DLC.

    xmin[modl]=data[++jcount][modl];
    xmax[modl]=data[++jcount][modl];
    ymin[modl]=data[++jcount][modl];
    ymax[modl]=data[++jcount][modl];
    h=glutGet(GLUT_WINDOW_HEIGHT);
    w=h;
    dx[modl]=(xmax[modl]-xmin[modl])/w;
    dy[modl]=(ymax[modl]-ymin[modl])/h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(xmin[modl], xmax[modl], ymin[modl], ymax[modl]);
    glutPostRedisplay();
    break; */

  default: /* do nothing */ break;
  }
  *jc=jcount;
}

void pdisp(int icount, int *jcount, ConxModlType modl)
{
  switch (icount) {
  case CONXCMD_PARABOLA: 
    pcol();
    LONGWAY(conxp_parabola);
    green();
    break;
  case CONXCMD_LINEF:
    conxp_graphlineAB(focus1[CONX_POINCARE_UHP], focus2[CONX_POINCARE_UHP],
                 tstep, ymin[modl], ymax[modl]);
    break;
  case CONXCMD_CIRCLE: 
    ccol();
    LONGWAY(conxp_circle);
    green();
    break;
  case CONXCMD_ELLIPSE:
    if (conicdistance > conxp_distAB(focus1[CONX_POINCARE_UHP],
                                     focus2[CONX_POINCARE_UHP])) {
      if (garnish)
        conxp_graphsegment(focus1[CONX_POINCARE_UHP],
                           focus2[CONX_POINCARE_UHP], tstep);
      ecol();
      LONGWAY(conxp_ellipse);
    } else {
      conxp_graphlineAB(focus1[CONX_POINCARE_UHP], focus2[CONX_POINCARE_UHP],
                        tstep, ymin[modl], ymax[modl]);
      ecol();
      LONGWAY(conxp_hyperbola);
    }
    green();
    break;
  case CONXCMD_SEGMENT:
    conxp_graphsegment(focus1[CONX_POINCARE_UHP], focus2[CONX_POINCARE_UHP],
                       tstep);
    break;
  case CONXCMD_ALTLINE:
    conxp_graphar(linea[CONX_POINCARE_UHP], liner[CONX_POINCARE_UHP], tstep,
                  ymin[modl], ymax[modl]);
    break;
  case CONXCMD_BPARABOLA:
    if (garnish)
      conxp_graphar(linea[CONX_POINCARE_UHP], liner[CONX_POINCARE_UHP], tstep,
                    ymin[modl], ymax[modl]);
    pcol();
    BRESENHAM(conxp_getPstarted, conxp_parabola);
    break;
  case CONXCMD_BELLIPSE:
    LOGGG2(LOGG_QUICK, "\nBBB f1 (%f, %f)\n", focus1[CONX_POINCARE_UHP].x,
           focus1[CONX_POINCARE_UHP].y);

    if (conicdistance > conxp_distAB(focus1[CONX_POINCARE_UHP],
                                     focus2[CONX_POINCARE_UHP])) {
      if (garnish)
        conxp_graphsegment(focus1[CONX_POINCARE_UHP],
                           focus2[CONX_POINCARE_UHP], tstep);
      LOGGG2(LOGG_QUICK, "\nDDD f1 (%f, %f)\n", focus1[CONX_POINCARE_UHP].x,
             focus1[CONX_POINCARE_UHP].y);
      ecol();
      BRESENHAM(conxp_getEstarted, conxp_ellipse);
    } else {
      conxp_graphlineAB(focus1[CONX_POINCARE_UHP], focus2[CONX_POINCARE_UHP],
                        tstep, ymin[modl], ymax[modl]);
      ecol();
      BRESENHAM(conxp_getHstarted, conxp_hyperbola);
    }
    LOGGG2(LOGG_QUICK, "\nCCC f1 (%f, %f)\n", focus1[CONX_POINCARE_UHP].x,
           focus1[CONX_POINCARE_UHP].y);
    break;
  case CONXCMD_EQDIST:
    if (garnish)
      conxp_graphar(linea[CONX_POINCARE_UHP], liner[CONX_POINCARE_UHP], tstep,
                    ymin[modl], ymax[modl]);
    LONGWAY(conxp_eqdist);
    break;
  case CONXCMD_BEQDIST:
    if (garnish)
      conxp_graphar(linea[CONX_POINCARE_UHP], liner[CONX_POINCARE_UHP], tstep,
                    ymin[modl], ymax[modl]);
    BRESENHAM(conxp_getEQstarted, conxp_eqdist);
    break;
  default:
    displayswitch((ConxMenuChoice)icount, jcount, modl);
    break;
  } /* switch */ 
}

void kdisp(int icount, int *jcount, ConxModlType modl)
{
  switch (icount) {
    case CONXCMD_PARABOLA: 
      pcol();
      LONGWAY(conxk_parabola);
      green();
      break;
    case CONXCMD_LINEF: 
      conxk_graphlineAB(focus1[CONX_KLEIN_DISK], focus2[CONX_KLEIN_DISK]);
      break;
    case CONXCMD_CIRCLE: 
      ccol();
      LONGWAY(conxk_circle);
      green();
      break;
    case CONXCMD_ELLIPSE: 
      if (garnish)
	conxk_graphlineAB(focus1[CONX_KLEIN_DISK], focus2[CONX_KLEIN_DISK]);
      ecol();
      if (conicdistance > conxk_distAB(focus1[CONX_KLEIN_DISK],
                                       focus2[CONX_KLEIN_DISK]))
	LONGWAY(conxk_ellipse);
      else
	LONGWAY(conxk_hyperbola);
      green();
      break;
    case CONXCMD_SEGMENT: 
      conxk_graphsegmentAB(focus1[CONX_KLEIN_DISK], focus2[CONX_KLEIN_DISK]);
      break;
    case CONXCMD_ALTLINE:
      conxk_graphmb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK]);
      break;
    case CONXCMD_BPARABOLA:
      if (garnish)
        conxk_graphmb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK]);
      pcol();
      BRESENHAM(conxk_getPstarted, conxk_parabola);
      break;
    case CONXCMD_BELLIPSE:
      if (garnish)
	conxk_graphsegmentAB(focus1[CONX_KLEIN_DISK], focus2[CONX_KLEIN_DISK]);
      ecol();
      if (conicdistance > conxk_distAB(focus1[CONX_KLEIN_DISK],
                                       focus2[CONX_KLEIN_DISK]))
	BRESENHAM(conxk_getEstarted, conxk_ellipse);
      else
	BRESENHAM(conxk_getHstarted, conxk_hyperbola);
      break;
    case CONXCMD_EQDIST:
      if (garnish)
        conxk_graphmb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK]);
      LONGWAY(conxk_eqdist);
      break;
    case CONXCMD_BEQDIST:
      if (garnish)
        conxk_graphmb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK]);
      BRESENHAM(conxk_getEQstarted, conxk_eqdist);
      break;
    default:
      displayswitch((ConxMenuChoice)icount, jcount, modl);
      break;
    } /* switch */ 

}

void conxpd_disp(int icount, int *jcount, ConxModlType modl)
{
  switch (icount) {
    case CONXCMD_PARABOLA: 
      pcol();
      LONGWAY(conxpd_parabola);
      green();
      break;
    case CONXCMD_LINEF: 
      conxpd_graphlineAB(focus1[CONX_POINCARE_DISK],
                         focus2[CONX_POINCARE_DISK], tstep);
      break;
    case CONXCMD_CIRCLE: 
      ccol();
      LONGWAY(conxpd_circle);
      green();
      break;
    case CONXCMD_ELLIPSE: 
      if (garnish)
	conxpd_graphsegment(focus1[CONX_POINCARE_DISK],
                            focus2[CONX_POINCARE_DISK], tstep);
      ecol();
      if (conicdistance>conxpd_distAB(focus1[CONX_POINCARE_DISK],
                                      focus2[CONX_POINCARE_DISK]))
	LONGWAY(conxpd_ellipse);
      else
	LONGWAY(conxpd_hyperbola);
      pcol();
      break;
    case CONXCMD_SEGMENT: 
      conxpd_graphsegment(focus1[CONX_POINCARE_DISK],
                          focus2[CONX_POINCARE_DISK], tstep);
      break;
    case CONXCMD_ALTLINE:
      conxpd_graphc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                    tstep);
      break;
    case CONXCMD_BPARABOLA:
      if (garnish)
	conxpd_graphc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                      tstep);
      pcol();
      BRESENHAM(conxpd_getPstarted, conxpd_parabola);
      break;
    case CONXCMD_BELLIPSE:
#ifdef DLC
      printf("\nfoci: (%f, %f) and (%f, %f)\nmagiccd: %f > %f for an ellipse\n"
             "\n",
             focus1[CONX_POINCARE_DISK].x, focus1[CONX_POINCARE_DISK].y,
             focus2[CONX_POINCARE_DISK].x, focus2[CONX_POINCARE_DISK].y,
             conicdistance,
             conxpd_distAB(focus1[CONX_POINCARE_DISK],
                           focus2[CONX_POINCARE_DISK]));
#endif
      if (garnish)
	conxpd_graphsegment(focus1[CONX_POINCARE_DISK],
                            focus2[CONX_POINCARE_DISK], tstep);
      ecol();
      if (conicdistance > conxpd_distAB(focus1[CONX_POINCARE_DISK],
                                        focus2[CONX_POINCARE_DISK]))
	BRESENHAM(conxpd_getEstarted, conxpd_ellipse);
      else
	BRESENHAM(conxpd_getHstarted, conxpd_hyperbola);
      break;
    case CONXCMD_EQDIST:
      if (garnish)
	conxpd_graphc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                      tstep);
      LONGWAY(conxpd_eqdist);
      break;
    case CONXCMD_BEQDIST:
      if (garnish)
	conxpd_graphc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                      tstep);
      BRESENHAM(conxpd_getEQstarted, conxpd_eqdist);
      break;
 
    default:
      displayswitch((ConxMenuChoice)icount, jcount, modl);
      break;
    } /* switch */ 
}

void conx_display(ConxModlType modl)
/* This draws the display for model modl.  You should call glFlush()
   after this.
*/
{
  int icount=-1, jcount=-1;

  LOGGG1(LOGG_TEXINFO, "\n@conx_display %s\n", conx_modelenum2short_string(modl));
  LOGGG4(78, "\n122 foci 1 2 (%f, %f) (%f, %f)\n",
         focus1[CONX_POINCARE_UHP].x, focus1[CONX_POINCARE_UHP].y,
         focus2[CONX_POINCARE_UHP].x, focus2[CONX_POINCARE_UHP].y);

  saved_hgmodel = modl;

/*   displayrecall(oldcd, oldtol, oldcomptol, oldtstep, oldxmin, oldxmax,
     oldymin, oldymax, oldf1, oldf2, olda, oldr); DLC */
  displayrecall(modl);
  LOGGG4(78, "\n123 foci 1 2 (%f, %f) (%f, %f)\n",
         focus1[CONX_POINCARE_UHP].x, focus1[CONX_POINCARE_UHP].y,
         focus2[CONX_POINCARE_UHP].x, focus2[CONX_POINCARE_UHP].y);

  conx_gl_clear();
  if (autofoci) {
    conx_draw_point(focus1[modl]);
    conx_draw_point(focus2[modl]);
  }
  HANDLE_BORDERS_DISPLAY(borders, modl, tstep);
  while (icount<top[modl]) {
    conx_disp(modl, disp[++icount][modl], &jcount);
    LOGGG4(LOGG_FULL, "\nicount= %d disp[icount]= %s j= %d data[j]=%f\n", icount,
           conx_menu_choice2string((ConxMenuChoice)disp[icount][modl]), jcount,
           data[jcount][modl]);
  } /* while */

  LOGGG4(LOGG_QUICK, "\nACQQ foci: 1=(%f, %f) 2=(%f, %f)\n",
         focus1[CONX_POINCARE_UHP].x, focus1[CONX_POINCARE_UHP].y,
         focus2[CONX_POINCARE_UHP].x, focus2[CONX_POINCARE_UHP].y);

  LOGGG0(LOGG_TEXINFO, "\n@end conx_display\n");
}

void conx_gl_first_init(void)
{
  glShadeModel(GL_FLAT);
  green();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  conxhm_ptokAB(focus1[CONX_POINCARE_UHP], &focus1[CONX_KLEIN_DISK]);
  conxhm_ptokAB(focus2[CONX_POINCARE_UHP], &focus2[CONX_KLEIN_DISK]);
  conx_getpdf1();
  conx_getpdf2();
  LOGGG4(LOGG_QUICK, "\n foci 1 2 (%f, %f) (%f, %f)\n", focus1[CONX_POINCARE_UHP].x,
         focus1[CONX_POINCARE_UHP].y, focus2[CONX_POINCARE_UHP].x,
         focus2[CONX_POINCARE_UHP].y);

  conxhm_getcfromar(linea[CONX_POINCARE_UHP], liner[CONX_POINCARE_UHP],
             &linea[CONX_POINCARE_DISK], &liner[CONX_POINCARE_DISK]);
  conxhm_getmbfromar(linea[CONX_POINCARE_UHP], liner[CONX_POINCARE_UHP],
              &linea[CONX_KLEIN_DISK], &liner[CONX_KLEIN_DISK]);
  LOGGG4(LOGG_FULL, "\nmb from cr: (%f, %f) \t cr from those mb? (%f, %f) \n",
         linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
         linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK]);
  /* start keeping track of variables */
  displaystore(CONX_POINCARE_UHP);
  displaystore(CONX_KLEIN_DISK);
  displaystore(CONX_POINCARE_DISK);

  LOGGG4(LOGG_QUICK, "\n 121foci 1 2 (%f, %f) (%f, %f)\n",
         focus1[CONX_POINCARE_UHP].x, focus1[CONX_POINCARE_UHP].y,
         focus2[CONX_POINCARE_UHP].x, focus2[CONX_POINCARE_UHP].y);
}

void conx_gl_init(ConxModlType mt, int w, int h)
{
  gluOrtho2D(xmin[mt], xmax[mt], ymin[mt], ymax[mt]);
  conx_reshape(mt, w, h);
}

int conx_mouse(ConxModlType mdl, ConxMouseChoice cmc, int x, int y,
               int win_height, ConxMenuChoice button_function)
/* Call this when the mouse is pressed or released in one of the three
   models' windows.
   
   Returns 0 if no redisplay is needed, 1 if a redisplay is needed. DLC ???
*/
{
  static Pt held;
  int retval = 0;
  Pt neww;
  int w, h;
  double uhh, crap;

  saved_hgmodel = mdl; /* DLC I think this is unnecessary since only the
                          Bresenham keep_going function uses it. */

  LOGGG1(LOGG_QUICK, "\nconx_mouse(): model %s\n", conx_modelenum2short_string(mdl));
  if (cmc == CONXMOUSE_DOWN) {
    conx_screen2model(x, y, xmin[mdl], xmax[mdl], ymin[mdl], ymax[mdl],
                      win_height, &held);
    switch (button_function) {
     case CONXCMD_MGETF2: 
      disp[++top[mdl]][mdl]=CONXCMD_GETF2;
      data[++dtop[mdl]][mdl]=held.x;
      data[++dtop[mdl]][mdl]=held.y;
      focus2[mdl].x=held.x;
      focus2[mdl].y=held.y;
      if (mdl==CONX_POINCARE_DISK) {
        conxhm_pdtokAB(focus2[CONX_POINCARE_DISK], &focus2[CONX_KLEIN_DISK]);
        conx_getpf2();
      }
      if (mdl==CONX_POINCARE_UHP) {
        conxhm_ptokAB(focus2[CONX_POINCARE_UHP], &focus2[CONX_KLEIN_DISK]);
        conx_getpdf2();
      }
      break;
    case CONXCMD_MGETF1:
      disp[++top[mdl]][mdl]=CONXCMD_GETF1;
      data[++dtop[mdl]][mdl]=held.x;
      data[++dtop[mdl]][mdl]=held.y;
      focus1[mdl].x=held.x;
      focus1[mdl].y=held.y;
      if (mdl==CONX_POINCARE_DISK) {
        conxhm_pdtokAB(focus1[CONX_POINCARE_DISK], &focus1[CONX_KLEIN_DISK]);
        conx_getpf1();
      }
      if (mdl==CONX_POINCARE_UHP) {
        conxhm_ptokAB(focus1[CONX_POINCARE_UHP], &focus1[CONX_KLEIN_DISK]);
        conx_getpdf1();
      }
      break;
    default: /* do nothing */
      assert(button_function == CONXCMD_MZOOM
             || button_function == CONXCMD_MGETCD
             || button_function == CONXCMD_MGETALT
             || button_function == CONXCMD_MGETFS
             || button_function == CONXCMD_CONX_NOT_A_CHOICE_2
             || button_function == CONXCMD_CONX_NOT_A_CHOICE_3);
      break;
    }
  } else {
    /* The button has been released. */
    conx_screen2model(x, y, xmin[mdl], xmax[mdl], ymin[mdl], ymax[mdl],
                      win_height, &neww);
    switch (button_function) {
    case CONXCMD_MZOOM:
      disp[++top[mdl]][mdl]=CONXCMD_GETXXYY;
      data[++dtop[mdl]][mdl]=lesser(neww.x, held.x);
      LOGGG3(LOGG_FULL, "%f %f %f 1 2 least\n", neww.x, held.x, lesser(neww.x, held.x));
      data[++dtop[mdl]][mdl]=greater(neww.x, held.x);
      data[++dtop[mdl]][mdl]=lesser(neww.y, held.y);
      data[++dtop[mdl]][mdl]=greater(neww.y, held.y);
      xmin[mdl]=data[dtop[mdl]-3][mdl];
      xmax[mdl]=data[dtop[mdl]-2][mdl];
      ymin[mdl]=data[dtop[mdl]-1][mdl];
      ymax[mdl]=data[dtop[mdl]-0][mdl];
      h=win_height;
      w=h;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(xmin[mdl], xmax[mdl], ymin[mdl], ymax[mdl]);
      dx[mdl]=(xmax[mdl]-xmin[mdl])/w;
      dy[mdl]=(ymax[mdl]-ymin[mdl])/h;
      retval = 1;
/*       glutPostRedisplay(); */
      break;
    case CONXCMD_MGETCD:
      disp[++top[mdl]][mdl]=CONXCMD_GETCD;
      if (mdl==CONX_POINCARE_DISK) {
	data[++dtop[mdl]][mdl]=conxpd_distAB(neww, held);
      } else if (mdl==CONX_KLEIN_DISK) {
	data[++dtop[mdl]][mdl]=conxk_distAB(neww, held);
      } else {
        data[++dtop[mdl]][mdl]=conxp_distAB(neww, held);
      }
      conicdistance=data[dtop[mdl]][mdl];
      break;
    case CONXCMD_MGETALT:
      disp[++top[mdl]][mdl]=CONXCMD_GETA;
      disp[++top[mdl]][mdl]=CONXCMD_GETR;
      if (mdl==CONX_POINCARE_DISK) {
	conxpd_getCenterAndRadius(held.x, held.y, neww.x, neww.y, \
          &data[++dtop[CONX_POINCARE_DISK]][CONX_POINCARE_DISK], &crap, &uhh);
	data[++dtop[CONX_POINCARE_DISK]][CONX_POINCARE_DISK]=crap;
      } else if (mdl==CONX_KLEIN_DISK) {
	if (myabs(crap=neww.x-held.x)>VERTICALNESS) {
	  data[++dtop[CONX_KLEIN_DISK]][CONX_KLEIN_DISK]=(neww.y-held.y)/crap;
	  data[++dtop[CONX_KLEIN_DISK]][CONX_KLEIN_DISK]
            = neww.y - data[dtop[CONX_KLEIN_DISK]-1][CONX_KLEIN_DISK]*neww.x;
        } else {
	  data[++dtop[CONX_KLEIN_DISK]][CONX_KLEIN_DISK]=(neww.x+held.x)/2;
	  data[++dtop[CONX_KLEIN_DISK]][CONX_KLEIN_DISK]=KINFINITY;
        }
      } else {
	conxp_geteq(neww, held, &data[++dtop[CONX_KLEIN_DISK]][CONX_KLEIN_DISK],
                    &crap);
	data[++dtop[CONX_KLEIN_DISK]][CONX_KLEIN_DISK]=crap;
      }
      linea[mdl]=data[dtop[mdl]-1][mdl];
      liner[mdl]=data[dtop[mdl]][mdl];
      if (mdl==CONX_POINCARE_DISK) {
	conxhm_getmbfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                   &linea[CONX_KLEIN_DISK], &liner[CONX_KLEIN_DISK]);
	conxhm_getarfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                   &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
      } else if (mdl==CONX_POINCARE_UHP) {
	conxhm_getmbfromar(linea[mdl], liner[mdl], &linea[CONX_KLEIN_DISK],
                    &liner[CONX_KLEIN_DISK]);
	conxhm_getcfromar(linea[mdl], liner[mdl], &linea[CONX_POINCARE_DISK],
                   &linea[CONX_POINCARE_DISK]);
      } else {
	conxhm_getcfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                   &linea[CONX_POINCARE_DISK], &liner[CONX_POINCARE_DISK]);
	conxhm_getarfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                    &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]); 
      }
      break;
    case CONXCMD_MGETFS:
      disp[++top[mdl]][mdl]=CONXCMD_GETF1;
      disp[++top[mdl]][mdl]=CONXCMD_GETF2;
      data[++dtop[mdl]][mdl]=held.x;
      data[++dtop[mdl]][mdl]=held.y;
      data[++dtop[mdl]][mdl]=neww.x;
      data[++dtop[mdl]][mdl]=neww.y;
      focus1[mdl].x=held.x;
      focus1[mdl].y=held.y;
      focus2[mdl].x=neww.x;
      focus2[mdl].y=neww.y;
      if (mdl==CONX_POINCARE_DISK) {
	conxhm_pdtokAB(focus1[CONX_POINCARE_DISK], &focus1[CONX_KLEIN_DISK]);
	conxhm_pdtokAB(focus2[CONX_POINCARE_DISK], &focus2[CONX_KLEIN_DISK]); 
	conx_getpf1();
	conx_getpf2(); 
      } else if (mdl==CONX_POINCARE_UHP) {
	conxhm_ptokAB(focus1[CONX_POINCARE_UHP], &focus1[CONX_KLEIN_DISK]);
	conxhm_ptokAB(focus2[CONX_POINCARE_UHP], &focus2[CONX_KLEIN_DISK]); 
	conx_getpdf1();
	conx_getpdf2(); 
      } else {
	conx_getpdf1();
	conx_getpdf2();
	conx_getpf1();
	conx_getpf2();
      }
      
      break;
    default:
      /* do nothing */
      assert(button_function == CONXCMD_MGETF1
             || button_function == CONXCMD_MGETF2
             || button_function == CONXCMD_CONX_NOT_A_CHOICE_2
             || button_function == CONXCMD_CONX_NOT_A_CHOICE_3);
      break;
    } /*switch */
    /*    glutPostRedisplay(); */
  } /*else*/
  LOGGG4(LOGG_QUICK, "\nheld (%f, %f) new (%f, %f)\n", held.x, held.y, neww.x, neww.y);
  if (autofoci) {
    LOGGG4(LOGG_QUICK, "\nf1 (%f, %f) f2 (%f, %f)\n", focus1[mdl].x, focus1[mdl].y,
           focus2[mdl].x, focus2[mdl].y);
#ifdef DLC
    mkptAll();
#else
    conx_draw_point(focus1[mdl]);
    conx_draw_point(focus2[mdl]);
#endif
  }

  return retval;
}

int conx_menufunc(ConxMenuChoice a, ConxModlType modl, int win_height)
/* Once the data array has been loaded, call this.
   To see what you should do, check the return code, which is either
   CONXMF_DO_NOTHING, CONXMF_DISPLAY, or CONXMF_DISPLAY_ALL.
*/
{
  int ct, temp, jct, w, h, rettval = CONXMF_DO_NOTHING;

  saved_hgmodel = modl;

  LOGGG4(LOGG_TEXINFO, "\n@conx_menufunc %s model=%s focus1 (%f, %f)\n",
         conx_menu_choice2string(a), conx_modelenum2short_string(modl),
         focus1[modl].x, focus1[modl].y);

  switch (a) {
  case CONXCMD_OUTPUT:
    LOGGG0(LOGG_TEXINFO, "\n@output");
    (void) fprintf(stdout, "\nConic parameter: %f tstep %f comptol %f"
                   "\nx[]: %f\t%f\ty[]: %f\t%f"
                   "\npd:: f1: (%f, %f)\tf2:(%f, %f)"
                   "\npuhp:: f1: (%f, %f)\tf2:(%f, %f)"
                   "\nkd:: f1: (%f, %f)\tf2:(%f, %f)"
                   "\nm: %f\tb: %f\t"
                   "\na: %f\tr: %f"
                   "\nCenter (%f, %f)"
                   "\nconxp_dist: %f\tconxk_dist: %f"
                   "\nconxpd_istfromarx: %f\tconxpd_dist: %f"
                   "\ntol: %f\tmodel %s\tloglevel %d\n",
                   conicdistance, tstep, comptol,
                   xmin[modl], xmax[modl], ymin[modl], ymax[modl],
                   focus1[CONX_POINCARE_DISK].x, focus1[CONX_POINCARE_DISK].y,
                   focus2[CONX_POINCARE_DISK].x, focus2[CONX_POINCARE_DISK].y,
                   focus1[CONX_POINCARE_UHP].x, focus1[CONX_POINCARE_UHP].y,
                   focus2[CONX_POINCARE_UHP].x, focus2[CONX_POINCARE_UHP].y,
                   focus1[CONX_KLEIN_DISK].x, focus1[CONX_KLEIN_DISK].y,
                   focus2[CONX_KLEIN_DISK].x, focus2[CONX_KLEIN_DISK].y,
                   linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                   linea[CONX_POINCARE_UHP], liner[CONX_POINCARE_UHP],
                   linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                   conxp_distAB(focus1[modl], focus2[modl]),
                   conxk_distAB(focus1[modl], focus2[modl]),
                   conxp_distFromarX(linea[CONX_POINCARE_UHP],
                                     liner[CONX_POINCARE_UHP],
                                     focus1[modl], comptol),
                   conxpd_distAB(focus1[modl], focus2[modl]), tol,
                   conx_modelenum2short_string(modl), loglevel);
    fflush(stdout);
    LOGGG0(LOGG_TEXINFO, "@end output\n");
    break;
  case CONXCMD_RESETTHISONE:
    RESET_DISPLAY_LISTS(0, top[modl]);
    jct=-1;
    for (ct=0; ct<top[modl]; ct++) {
      switch (temp=disp[ct][modl]) {
      case CONXCMD_BCIRCLE:
      case CONXCMD_FOCUS:
      case CONXCMD_FOCI:
        break;
      default:
        displayswitch((ConxMenuChoice)temp, &jct, modl);
        break;
      }
    }
    if (jct-1!=dtop[modl]) {
      LOGGG1(LOGG_QUICK, "\n Clearing the window didn't work exactly right.\n"
             "  All changes may not be saved.\n%d changes appear missing.\n\n",
             dtop[modl]-jct);
      /* DLC fix this. */
    }
    dtop[modl]=-1;
    top[modl]=-1;
    displaystore(modl);
    conx_gl_clear();
    HANDLE_BORDERS_DISPLAY(borders, modl, tstep);
    break;
      
  case CONXCMD_EXIT:
    exit(0);
    break;
      
  case CONXCMD_GETLOGLEVEL:
    LOGGG1(LOGG_TEXINFO, "\n@setloglevel{%f}\n", data[dtop[modl]][modl]);
    loglevel = (int) ceil(data[dtop[modl]][modl]);
    break;
  case CONXCMD_GETCD:
    conicdistance=data[dtop[modl]][modl];
    break;
  case CONXCMD_GETTOL:
    tol=data[dtop[modl]][modl];
    break;
  case CONXCMD_GETCOMPTOL:
    comptol=data[dtop[modl]][modl];
    break;
  case CONXCMD_GETXXYY:
    xmin[modl]=data[dtop[modl]-3][modl];
    xmax[modl]=data[dtop[modl]-2][modl];
    ymin[modl]=data[dtop[modl]-1][modl];
    ymax[modl]=data[dtop[modl]-0][modl];
    h=win_height;
    w=h;
    LOGGG5(LOGG_QUICK, "\nx[] : [%f, %f] y[] : [%f, %f] h %d\n", xmin[modl],
           xmax[modl], ymin[modl], ymax[modl], h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(xmin[modl], xmax[modl], ymin[modl], ymax[modl]);
    dx[modl]=(xmax[modl]-xmin[modl])/(double)w;
    dy[modl]=(ymax[modl]-ymin[modl])/(double)h;
    LOGGG0(LOGG_QUICK, "yep yep");
    rettval |= CONXMF_DISPLAY;
    LOGGG0(LOGG_QUICK, "reached gotxxyy in menufunc\n");
    break;
  case CONXCMD_GETTSTEP:
    tstep=data[dtop[modl]][modl];
    break;/* DLC was a bug */
  case CONXCMD_GETF1:
    focus1[modl].x=data[dtop[modl]-1][modl];
    focus1[modl].y=data[dtop[modl]][modl];
    if (modl==CONX_POINCARE_DISK) {
      conxhm_pdtokAB(focus1[CONX_POINCARE_DISK], &focus1[CONX_KLEIN_DISK]);
      conx_getpf1();
    }
    if (modl==CONX_POINCARE_UHP) {
      conxhm_ptokAB(focus1[CONX_POINCARE_UHP], &focus1[CONX_KLEIN_DISK]);
      conx_getpdf1();
    }

    break;
  case CONXCMD_GETF2:
    focus2[modl].x=data[dtop[modl]-1][modl];
    focus2[modl].y=data[dtop[modl]][modl];
    if (modl==CONX_POINCARE_DISK) {
      conxhm_pdtokAB(focus2[CONX_POINCARE_DISK], &focus2[CONX_KLEIN_DISK]);
      conx_getpf2();
    }
    if (modl==CONX_POINCARE_UHP) {
      conxhm_ptokAB(focus2[CONX_POINCARE_UHP], &focus2[CONX_KLEIN_DISK]);
      conx_getpdf2();
    }

    break;
  case CONXCMD_GETA:
    linea[modl]=data[dtop[modl]][modl];
    if (modl==CONX_POINCARE_DISK) {
      conxhm_getmbfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                 &linea[CONX_KLEIN_DISK], &liner[CONX_KLEIN_DISK]);
      conxhm_getarfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                 &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
    } else if (modl==CONX_POINCARE_UHP) {
      conxhm_getmbfromar(linea[modl], liner[modl], &linea[CONX_KLEIN_DISK],
                  &liner[CONX_KLEIN_DISK]);
      conxhm_getcfromar(linea[modl], liner[modl], &linea[CONX_POINCARE_DISK],
                 &linea[CONX_POINCARE_DISK]);
    } else {
      conxhm_getcfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                 &linea[CONX_POINCARE_DISK], &liner[CONX_POINCARE_DISK]);
      conxhm_getarfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                  &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]); 
    }

    break;
  case CONXCMD_GETR:
    liner[modl]=data[dtop[modl]][modl];
    if (modl==CONX_POINCARE_DISK) {
      conxhm_getmbfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                 &linea[CONX_KLEIN_DISK], &liner[CONX_KLEIN_DISK]);
      conxhm_getarfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
                 &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
    } else if (modl==CONX_POINCARE_UHP) {
      conxhm_getmbfromar(linea[modl], liner[modl], &linea[CONX_KLEIN_DISK],
                  &liner[CONX_KLEIN_DISK]);
      conxhm_getcfromar(linea[modl], liner[modl], &linea[CONX_POINCARE_DISK],
                 &linea[CONX_POINCARE_DISK]);
    } else {
      conxhm_getcfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                 &linea[CONX_POINCARE_DISK], &liner[CONX_POINCARE_DISK]);
      conxhm_getarfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                  &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]); 
    }

    break;
  case CONXCMD_AUTOFOCION:
    autofoci=!autofoci;
    rettval |= CONXMF_DISPLAY_ALL;
    break;
  case CONXCMD_GARNISHON:
    garnish=!garnish;
    rettval |= CONXMF_DISPLAY_ALL;
    break;
  case CONXCMD_BORDERSON:
    borders=!borders;
    rettval |= CONXMF_DISPLAY_ALL;
    break;
  case CONXCMD_CLEAR:
    RESET_DISPLAY_LISTS(0, top[modl]);
    displayrecall(modl);
    for (ct=0; ct<3; ct++) {
      top[ct]=  -1;
      dtop[ct]= -1;
    }
    rettval |= CONXMF_DISPLAY_ALL;
    break;
  default: /* do nothing */ break;
  }  /* switch */

  LOGGG2(LOGG_QUICK, "\n000 f1 (%f, %f)\n", focus1[modl].x, focus1[modl].y);
  if (REQUIRES_PLAYBACK(a))
    disp[++top[modl]][modl]=a;

  switch (a) {
  case CONXCMD_GETXXYY:
  case CONXCMD_GETF1:
  case CONXCMD_GETF2:
#ifdef DLC
  case CONXCMD_FROMK: 
  case CONXCMD_FROMP:
  case CONXCMD_FROMPD:
#endif
    if (autofoci)
      disp[++top[modl]][modl]=CONXCMD_FOCI;
#ifdef DLC
    mkptAll();
#else
    conx_draw_point(focus1[modl]);
    conx_draw_point(focus2[modl]);
#endif
    break;
  default: /* do nothing */ break;
  } /* switch */

  LOGGG2(LOGG_QUICK, "\nAAA f1 (%f, %f)\n", focus1[modl].x, focus1[modl].y);
#ifdef DLC
  if (VIZ_COMMAND_P(disp[top[modl]][modl])) {
    /* DLC just tell that we need a redisplay. */
    conx_disp(modl, disp[top[modl]][modl], &ct);
  }
#endif

  if (autofoci) {
    conx_draw_point(focus1[modl]);
    conx_draw_point(focus2[modl]);
  }

  LOGGG6(LOGG_QUICK, "\n... focus1=(%f, %f); %s;\n\t"
         "model=%s a= %d dtop= %d,", focus1[modl].x, focus1[modl].y,
         (top[modl] >= 0)
         ? conx_menu_choice2string((ConxMenuChoice)disp[top[modl]][modl])
         : "<empty command stack>",
         conx_modelenum2short_string(modl), a, dtop[modl]);
  LOGGG1(LOGG_QUICK, " top= %d\n", top[modl]);
  LOGGG0(LOGG_TEXINFO, "\n@end conx_menufunc\n");

  return rettval;
}

int bres_keep_going(Pt middle, Pt oldmiddle, void *ignored)
/* Bresenham method requires this to know when to stop. */
{
  return (((saved_hgmodel != CONX_POINCARE_UHP)
           ? (sqr(middle.x) + sqr(middle.y) < 1.0)
               /* DLC what about xmin and xmax?  We can save time but may
                  have to reenter if we treat them correctly, the same
                  troubles as in the Poincare UHP. */
            : ((middle.x < xmax[saved_hgmodel])
               && (middle.x > xmin[saved_hgmodel])
               && (middle.y < ymax[saved_hgmodel])
               && (middle.y > ymin[saved_hgmodel])))
           && (myabs(middle.x - oldmiddle.x) + myabs(middle.y - oldmiddle.y)
               > ARBITRARILYSMALL)
          );
}

/****************************************************************************
   The following determine the conics for the "longway" method.  The zeroes
   of each defines one of the conics.  If only zero were zero in machine
   arithmetic, the Bresenham method might not be preferred.
*****************************************************************************/
double conxp_circle(Pt X, void *ignored)
{
  return conxp_distAB(X, focus1[CONX_POINCARE_UHP]) - conicdistance;
}

double conxpd_circle(Pt X, void *ignored)
{
  return conxpd_distAB(X, focus1[CONX_POINCARE_DISK]) - conicdistance;
}

double conxk_circle(Pt X, void *ignored)
{
  return conxk_distAB(X, focus1[CONX_KLEIN_DISK]) - conicdistance;
}

double conxp_ellipse(Pt X, void *ignored)
{
  return conxp_distAB(focus1[CONX_POINCARE_UHP], X)
          + conxp_distAB(focus2[CONX_POINCARE_UHP], X)
          - conicdistance;
}

double conxk_ellipse(Pt X, void *ignored)
{
  return conxk_distAB(focus1[CONX_KLEIN_DISK], X)
          + conxk_distAB(focus2[CONX_KLEIN_DISK], X)
          - conicdistance;
}

double conxpd_ellipse(Pt X, void *ignored)
{
  return conxpd_distAB(focus1[CONX_POINCARE_DISK], X)
          + conxpd_distAB(focus2[CONX_POINCARE_DISK], X)
          - conicdistance;
}

double conxp_hyperbola(Pt X, void *ignored)
{
  return myabs(conxp_distAB(focus1[CONX_POINCARE_UHP], X)
                - conxp_distAB(focus2[CONX_POINCARE_UHP], X))
          - conicdistance;
}

double conxk_hyperbola(Pt X, void *ignored)
{
  return myabs(conxk_distAB(focus1[CONX_KLEIN_DISK], X)
                - conxk_distAB(focus2[CONX_KLEIN_DISK], X))
          - conicdistance;
}

double conxpd_hyperbola(Pt X, void *ignored)
{
  return myabs(conxpd_distAB(focus1[CONX_POINCARE_DISK], X)
                - conxpd_distAB(focus2[CONX_POINCARE_DISK], X))
          - conicdistance;
}

double conxp_parabola(Pt X, void *ignored)
{
  return conxp_distAB(focus1[CONX_POINCARE_UHP], X)
         - conxp_distFromarX(linea[CONX_POINCARE_UHP],
                             liner[CONX_POINCARE_UHP], X, comptol);
}

double conxk_parabola(Pt X, void *ignored)
{
  return conxk_distAB(focus1[CONX_KLEIN_DISK], X)
          - conxk_distFrommbX(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
                              X, comptol);
}

double conxpd_parabola(Pt X, void *ignored)
{
  return conxpd_distAB(focus1[CONX_POINCARE_DISK], X)
          - conxpd_distFromcX(linea[CONX_POINCARE_DISK],
                         liner[CONX_POINCARE_DISK], X, comptol);
}

double conxp_eqdist(Pt X, void *ignored)
{
  return conxp_distFromarX(linea[CONX_POINCARE_UHP], liner[CONX_POINCARE_UHP],
                           X, comptol)
          - conicdistance;
}

double conxk_eqdist(Pt X, void *ignored)
{
  return conxk_distFrommbX(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK], X,
                           comptol)
           - conicdistance;
}

double conxpd_eqdist(Pt X, void *ignored)
{
  return conxpd_distFromcX(linea[CONX_POINCARE_DISK],
                           liner[CONX_POINCARE_DISK], X, comptol)
          - conicdistance;
}

/**************************************************************************
    The following functions start off the conics for the bresenham method
    by finding a point on every branch.  If there is one branch, then
    the points are set equal.
***************************************************************************/
void conxk_get_started(Pt *LB, Pt *RB, ConxFullBresStarter *func)
/* We find a point or points on the conic section in the Poincare UHP.
   We then translate those UHP coordinates back to Klein Disk coordinates.
*/
{
  Pt puhp_f1, puhp_f2;

  conxhm_ktopAB(focus1[CONX_KLEIN_DISK], &puhp_f1);
  conxhm_ktopAB(focus2[CONX_KLEIN_DISK], &puhp_f2);
  func(LB, RB, puhp_f1, puhp_f2, conicdistance);
  conxhm_ptokAB(*LB, LB);
  conxhm_ptokAB(*RB, RB);
}

void conxpd_get_started(Pt *LB, Pt *RB, ConxFullBresStarter *func)
/* We find a point or points on the conic section in the Poincare UHP.
   We then translate those UHP coordinates back to Poincare Disk coordinates.
*/
{
  Pt puhp_f1, puhp_f2;

  conxhm_pdtopAB(focus1[CONX_POINCARE_DISK], &puhp_f1);
  conxhm_pdtopAB(focus2[CONX_POINCARE_DISK], &puhp_f2);
  func(LB, RB, puhp_f1, puhp_f2, conicdistance);
  conxhm_ptopdAB(*LB, LB);
  conxhm_ptopdAB(*RB, RB);
}


void conxpd_getEstarted(Pt *LB, Pt *RB)
{
  conxpd_get_started(LB, RB, conxhm_p_getEstarted);
}

void conxk_getEstarted(Pt *LB, Pt *RB)
{
  conxk_get_started(LB, RB, conxhm_p_getEstarted);
}

void conxp_getEstarted(Pt *LB, Pt *RB)
{
  conxhm_p_getEstarted(LB, RB, focus1[CONX_POINCARE_UHP],
                       focus2[CONX_POINCARE_UHP], conicdistance);
}

void conxpd_getHstarted(Pt *LB, Pt *RB)
{
  conxpd_get_started(LB, RB, conxhm_p_getHstarted);
}

void conxk_getHstarted(Pt *LB, Pt *RB)
{
  conxk_get_started(LB, RB, conxhm_p_getHstarted);
}

void conxp_getHstarted(Pt *LB, Pt *RB)
{
  conxhm_p_getHstarted(LB, RB, focus1[CONX_POINCARE_UHP],
                       focus2[CONX_POINCARE_UHP], conicdistance);
}

void conxp_getPstarted(Pt *LB, Pt *RB)
{
  conxhm_p_getPstarted(LB, RB, focus1[CONX_POINCARE_UHP],
                       linea[CONX_POINCARE_UHP], liner[CONX_POINCARE_UHP],
                       comptol);
}


void conxpd_getPstarted(Pt *LB, Pt *RB)
{
  Pt realf1;
  double olda, oldr;

  olda=linea[CONX_POINCARE_UHP];
  oldr=liner[CONX_POINCARE_UHP];
  realf1=focus1[CONX_POINCARE_UHP];

  conxhm_pdtopAB(focus1[CONX_POINCARE_DISK], &focus1[CONX_POINCARE_UHP]);
  conxhm_getarfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
             &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
  conxp_getPstarted(LB, RB);
  conxhm_ptopdAB(*LB, LB);
  *RB=*LB;
  focus1[CONX_POINCARE_UHP]=realf1;
  linea[CONX_POINCARE_UHP]=olda;
  liner[CONX_POINCARE_UHP]=oldr;
}

void conxk_getPstarted(Pt *LB, Pt *RB)
{
  Pt realf1;
  double olda, oldr;
  olda=linea[CONX_POINCARE_UHP];
  oldr=liner[CONX_POINCARE_UHP];
  realf1=focus1[CONX_POINCARE_UHP];
  conxhm_ktopAB(focus1[CONX_KLEIN_DISK], &focus1[CONX_POINCARE_UHP]);
  conxhm_getarfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
              &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
  conxp_getPstarted(LB, RB);
  conxhm_ptokAB(*LB, LB);
  *RB=*LB;
  focus1[CONX_POINCARE_UHP]=realf1;
  linea[CONX_POINCARE_UHP]=olda;
  liner[CONX_POINCARE_UHP]=oldr;
}

void conxp_getEQstarted(Pt *LB, Pt *RB)
{
  conxhm_p_getEQstarted(LB, RB, linea[CONX_POINCARE_UHP],
                        liner[CONX_POINCARE_UHP], conicdistance,
                        ymin[CONX_POINCARE_UHP], ymax[CONX_POINCARE_UHP]);
}

void conxpd_getEQstarted(Pt *LB, Pt *RB)
{
  double reala, realr;
  reala=linea[CONX_POINCARE_UHP];
  realr=liner[CONX_POINCARE_UHP];
  conxhm_getarfromc(linea[CONX_POINCARE_DISK], liner[CONX_POINCARE_DISK],
             &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
  conxp_getEQstarted(LB, RB);
  conxhm_ptopdAB(*LB, LB);
  conxhm_ptopdAB(*RB, RB);
  linea[CONX_POINCARE_UHP]=reala;
  liner[CONX_POINCARE_UHP]=realr;
}

void conxk_getEQstarted(Pt *LB, Pt *RB)
{
  double reala, realr;
  reala=linea[CONX_POINCARE_UHP];
  realr=liner[CONX_POINCARE_UHP];
  conxhm_getarfrommb(linea[CONX_KLEIN_DISK], liner[CONX_KLEIN_DISK],
              &linea[CONX_POINCARE_UHP], &liner[CONX_POINCARE_UHP]);
  conxp_getEQstarted(LB, RB);
  conxhm_ptokAB(*LB, LB);
  conxhm_ptokAB(*RB, RB);
  linea[CONX_POINCARE_UHP]=reala;
  liner[CONX_POINCARE_UHP]=realr;
}
