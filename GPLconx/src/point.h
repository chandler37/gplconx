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

#ifndef CONXV_POINT_H
#define CONXV_POINT_H 1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Pt {
  double x, y;
} Pt;

/* These must be 0, 1, and 2: */
typedef enum modeltype {
  CONX_POINCARE_UHP, CONX_KLEIN_DISK, CONX_POINCARE_DISK
} ConxModlType;
/* poincare Upper Half plane, Beltrami-Klein disk, or poincare disk */

#define CONX_NUM_MODELS 3

/* Add to this and to conxcln.c's array at the same time. */
typedef enum menuchoices {
CONXCMD_ALTLINE=                   135,
CONXCMD_LINEF,
CONXCMD_FOCI,
CONXCMD_FOCUS,
CONXCMD_SEGMENT,
CONXCMD_ELLIPSE,
CONXCMD_CIRCLE,
CONXCMD_PARABOLA,
CONXCMD_BELLIPSE,
CONXCMD_BCIRCLE,
CONXCMD_BPARABOLA,
CONXCMD_EQDIST,
CONXCMD_BEQDIST,
CONXCMD_SHOWTOL,
CONXCMD_SHOWCOMPTOL,
CONXCMD_SHOWA,
CONXCMD_SHOWR,
CONXCMD_SHOWCD,
CONXCMD_SHOWLOGLEVEL,
CONXCMD_SHOWF1,
CONXCMD_SHOWF2,
CONXCMD_SHOWTSTEP,
CONXCMD_SHOWXXYY,
CONXCMD_GETTOL,
CONXCMD_GETCOMPTOL,
CONXCMD_GETA,
CONXCMD_GETR,
CONXCMD_GETCD,
CONXCMD_GETLOGLEVEL,
CONXCMD_GETF1,
CONXCMD_GETF2,
CONXCMD_GETTSTEP,
CONXCMD_GETXXYY,
CONXCMD_CLEAR,
CONXCMD_BORDERSON,
CONXCMD_GARNISHON,
CONXCMD_AUTOFOCION,
CONXCMD_EXIT,
CONXCMD_OUTPUT,
CONXCMD_RESETTHISONE,
CONXCMD_MGETFS,
CONXCMD_MGETF1,
CONXCMD_MGETF2,
CONXCMD_MZOOM,
CONXCMD_MGETCD,
CONXCMD_MGETALT,
CONXCMD_CONX_NOT_A_CHOICE,
CONXCMD_CONX_NOT_A_CHOICE_2,
CONXCMD_CONX_NOT_A_CHOICE_3
} ConxMenuChoice;

/* If a ConxMenuChoice can be "played back", then CONX_IS_PLAYBACK_CMD(a)
   should return non-zero.  Mouse choices MZOOM and friends need not
   be handled.  EXIT need not be handled.
*/
#define REQUIRES_PLAYBACK(a) \
  (((a)!=CONXCMD_CLEAR) \
    && ((a)!=CONXCMD_RESETTHISONE) \
    && ((a)!=CONXCMD_OUTPUT) \
    && ((a)!=CONXCMD_SHOWTOL) \
    && ((a)!=CONXCMD_SHOWCOMPTOL) \
    && ((a)!=CONXCMD_SHOWA) \
    && ((a)!=CONXCMD_SHOWR) \
    && ((a)!=CONXCMD_SHOWCD) \
    && ((a)!=CONXCMD_SHOWLOGLEVEL) \
    && ((a)!=CONXCMD_SHOWF1) \
    && ((a)!=CONXCMD_SHOWF2) \
    && ((a)!=CONXCMD_SHOWTSTEP) \
    && ((a)!=CONXCMD_SHOWXXYY) \
    && ((a)!=CONXCMD_GETLOGLEVEL)) /* DLC GETXXYY. */

/* If a ConxMenuChoice causes a visualization to occur, rather than
   changes a state variable that affects viz, then VIZ_COMMAND_P should
   return non-zero for that choice.
*/
#define VIZ_COMMAND_P(a) (   ((a) == CONXCMD_ALTLINE) \
                          || ((a) == CONXCMD_LINEF) \
                          || ((a) == CONXCMD_FOCI) \
                          || ((a) == CONXCMD_FOCUS) \
                          || ((a) == CONXCMD_SEGMENT) \
                          || ((a) == CONXCMD_ELLIPSE) \
                          || ((a) == CONXCMD_CIRCLE) \
                          || ((a) == CONXCMD_PARABOLA) \
                          || ((a) == CONXCMD_BELLIPSE) \
                          || ((a) == CONXCMD_BCIRCLE) \
                          || ((a) == CONXCMD_BPARABOLA) \
                          || ((a) == CONXCMD_EQDIST) \
                          || ((a) == CONXCMD_BEQDIST))

typedef enum ConxMouseChoice {
  CONXMOUSE_DOWN,
  CONXMOUSE_UP
} ConxMouseChoice;

enum {
  CONXMF_DISPLAY = 2,
  CONXMF_DISPLAY_ALL = 1,
  CONXMF_DO_NOTHING = 0
};

/* We use directions to refer to adjacent pixels.

     NW         N         NE




      W                    E
 



     SW         S         SE
*/

/* `for' loops and array storage sizes depend on these having the
   values 0 through 7, inclusive, but the order is immaterial.
*/
typedef enum ConxDirection {
  CXD_NW=0,
  CXD_N,
  CXD_NE,
  CXD_W,
  CXD_E,
  CXD_SW,
  CXD_S,
  CXD_SE
} ConxDirection;


#define NUM_DIRECS 8

typedef enum ConxColorEnum {
  CONXCLR_GREEN,
  CONXCLR_WHITE,
  CONXCLR_BLACK,
  CONXCLR_BLUE,
  CONXCLR_RED,
  CONXCLR_PARABOLA,
  CONXCLR_ELLIPSE,
  CONXCLR_CIRCLE
} ConxColorEnum;

typedef void (ConxBresenhamStarter) (Pt *, Pt *);
typedef double (ConxMetric) (Pt, void *);
typedef int (ConxContinueFunc) (Pt, Pt);

/* A function that converts (a, b) to (c, d), e.g. ptopd */
typedef void (ConxPoint2DConverterFunc) (double, double, double *, double *);
typedef void (ConxPointFunc) (double, double, void *);
typedef void (ConxBresTraceFunc) (Pt middle, ConxDirection last, double dw, \
                                  double dh, ConxMetric *func, void *fArg, \
                                  ConxContinueFunc *keepgoing);

#ifdef __cplusplus
}
#endif

#endif /* CONXV_POINT_H */
