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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>

#include "point.h"
#include "viewer.h"
#include "util.h"
#include "bresint.h"

static ConxDirection conx_compass_opposite(ConxDirection d);
static const char *conx_direction2string(ConxDirection a);
static
void conx_bres_tracebranch(ConxDirection last, Pt middle, double dw,
                           double dh, ConxMetric *func, void *fArg,
                           ConxContinueFunc *keepgoing,
                           ConxBresTraceFunc *bres_trace);
static
ConxDirection conx_startpoint(Pt LB, double dw, double dh, ConxMetric *func,
                              void *fArg);
static inline
double func_in_direction(Pt current_location, ConxDirection testdir,
                         double dw, double dh, ConxMetric *func, void *fArg);



double func_in_direction(Pt current_location, ConxDirection testdir,
                         double dw, double dh, ConxMetric *func, void *fArg)
/* Returns the value of the metric for the point adjacent to current_location
   in the testdir direction.
*/
{
  MOVE_POINT(&current_location, testdir, dw, dh);
  return func(current_location, fArg);
}

const char *conx_direction2string(ConxDirection a)
{
  switch (a) {
  case CXD_SE: return "CXD_SE";
  case CXD_S:  return"CXD_S";
  case CXD_NW: return "CXD_NW";
  case CXD_N:  return"CXD_N";
  case CXD_SW: return "CXD_SW";
  case CXD_W:  return"CXD_W";
  case CXD_NE: return "CXD_NE";
  case CXD_E: return "CXD_E";
  default: return NULL;
  }
}

ConxDirection conx_compass_opposite(ConxDirection d)
/*
  North is opposite of South, so conx_compass_opposite(CXD_N) == S
*/
{
  switch (d) {
  case CXD_SE: return CXD_NW;
  case CXD_S:  return CXD_N;
  case CXD_NW: return CXD_SE;
  case CXD_N:  return CXD_S;
  case CXD_SW: return CXD_NE;
  case CXD_W:  return CXD_E;
  case CXD_NE: return CXD_SW;
  default: assert(d == CXD_E); return CXD_W;
  }
}

ConxDirection conx_startpoint(Pt LB, double dw, double dh, ConxMetric *func,
                              void *fArg)
/* Here we find the best direction to travel from LB so that we minimize
   the absolute value of
   func(the point adjacent to LB in the direction returned).
*/
{

#define HOLD_MINIMUM(current_min, attempt, tagholder, tag) \
  { \
    double thistry = (attempt); \
    LOGGG2(LOGG_BRES2, "\nTry is " DOF " in direction %s\n", thistry, \
           conx_direction2string(tag)); \
    if (thistry < *(current_min)) { \
      *(current_min) = thistry; \
      *(tagholder) = (tag); \
    } \
  }


  double leastdistance;
  ConxDirection dir, bestdir;

  assert(func != NULL);

  /* We have to have a king to dethrone.  We will arbitrarily choose
     0 (CXD_NW, but that's unimportant) to begin.
  */
  dir = 0;
  leastdistance = myabs(func_in_direction(LB, dir, dw, dh, func, fArg));
  bestdir = dir;

#ifdef DLC
  /* DLC debug only */
  printf("func==%p;\nfunc(" DOF ", " DOF ")=" DOF "\n", (void *)func, LB.x,
         LB.y, myabs(func_in_direction(LB, dir, dw, dh, func, fArg)));

  for (dir = 0; dir <= 7; dir++) {
    printf("func(LB+%s)=" DOF "\n", conx_direction2string(dir),
           myabs(func_in_direction(LB, dir, dw, dh, func, fArg)));
  }
#endif

  for (dir = 1; dir <= 7; dir++) {
    HOLD_MINIMUM(&leastdistance,
                 myabs(func_in_direction(LB, dir, dw, dh, func, fArg)),
                 &bestdir, dir);
  }
  return bestdir;
}

void conx_bres_trace(Pt middle, ConxDirection last, double dw, double dh,
                     ConxMetric *func, void *fArg, ConxContinueFunc *keepgoing,
                     ConxPointFunc *pfunc, void *pArg)
/* We trace a curve from a point going in one direction until we fall off
   the edge of the visualized world.  We are following local minima rather
   than finding zeroes by scan lines because we have well-behaved curves.
   Thus, if you just went north, then the next move will be either north,
   northwest, or northeast.  (We definitely do not want to retrace our last
   move, but checking five points rather than three might be prudent. -- DLC)

   When a point on the curve is found, (*pfunc)(x, y) is called.
*/
{
/* Set *mindirptr to x, if array[x] is the minimum among
    { array[dir1], array[dir2], array[dir3] }
*/
#define GET_MINDIR3(mdp, array, dir1, dir2, dir3) \
   if ((array)[dir1] < (array)[dir2]) { \
     /* not dir2 */ \
     if ((array)[dir1] < (array)[dir3]) *(mdp)=dir1; else *(mdp)=dir3; \
   } else { \
     /* not dir1 */ \
     if ((array)[dir3] < (array)[dir2]) *(mdp)=dir3; else *(mdp)=dir2; \
   }

/* Set *mindirptr to x, if array[x] is the minimum among
   { array[direcarray[0]], array[direcarray[1]], array[direcarray[2]] }
*/
#define ALT_GET_MINDIR3(mindirptr, array, direcarray) \
  GET_MINDIR3(mindirptr, array, \
              (direcarray)[0], (direcarray)[1], (direcarray)[2])

#define FILL_DIRECTIONS3(lastdir, direcarray) \
  { \
    (direcarray)[0] = (lastdir); \
    switch ((lastdir)) { \
    case CXD_SE: (direcarray)[1] = CXD_S;  (direcarray)[2] = CXD_E; break; \
    case CXD_S:  (direcarray)[1] = CXD_SE; (direcarray)[2] = CXD_SW; break; \
    case CXD_NW: (direcarray)[1] = CXD_N;  (direcarray)[2] = CXD_W; break; \
    case CXD_N:  (direcarray)[1] = CXD_NW; (direcarray)[2] = CXD_NE; break; \
    case CXD_SW: (direcarray)[1] = CXD_S;  (direcarray)[2] = CXD_W; break; \
    case CXD_W:  (direcarray)[1] = CXD_NW; (direcarray)[2] = CXD_SW; break; \
    case CXD_NE: (direcarray)[1] = CXD_E;  (direcarray)[2] = CXD_N; break; \
    default: assert((lastdir) == CXD_E); \
                 (direcarray)[1] = CXD_NE; (direcarray)[2] = CXD_SE; break; \
    } \
  }


  double next[NUM_DIRECS];
  Pt oldmiddle;
  int count=0, i;

#define NUM_ADJ_POINTS 3
  ConxDirection directions[NUM_ADJ_POINTS];
    /* We check three adjacent points currently. */

  oldmiddle=middle;

  (*pfunc)(middle.x, middle.y, pArg);

  do {
    /* We are at middle.x; see where we should go next.
       For example, if we just went north (i.e., last==CXD_N), then
       see if we should next go CXD_N, CXD_NW, or CXD_NE.
    */
    FILL_DIRECTIONS3(last, directions);

    for (i = 0; i < NUM_ADJ_POINTS; i++) {
      next[directions[i]]
        = myabs(func_in_direction(middle, directions[i], dw, dh, func, fArg));
    }

    ALT_GET_MINDIR3(&last, next, directions);

    LOGGG5(LOGG_BRES2, "\nMoving from (" DOF ", " DOF ") in direction %s "
           "since F(%s)\n    = " DOF ", ", middle.x, middle.y,
           conx_direction2string(last),
           conx_direction2string(directions[0]), next[directions[0]]);
    LOGGG5(LOGG_BRES2, "F(%s)= " DOF ", and F(%s)= " DOF " (min " DOF ")",
           conx_direction2string(directions[1]), next[directions[1]],
           conx_direction2string(directions[2]), next[directions[2]],
           next[last]);

    MOVE_POINT(&middle, last, dw, dh);

    (*pfunc)(middle.x, middle.y, pArg);
 } while ((*keepgoing)(middle, oldmiddle) && (++count<=15000));
}

void conx_bres_tracebranch(ConxDirection last, Pt middle, double dw,
                           double dh, ConxMetric *func, void *fArg,
                           ConxContinueFunc *keepgoing,
                           ConxBresTraceFunc *bres_trace)
{
  LOGGG0(LOGG_BRES2, "\nAbout to trace one branch of a conic section using "
         "the Bresenham method.\n");

  MOVE_POINT(&middle, last, dw, dh);
  (*bres_trace)(middle, last, dw, dh, func, fArg, keepgoing);

  last = conx_compass_opposite(last);

  /* middle is the original middle moved in the opposite direction, so we will
     move it back to the original value of middle and trace.
     DLC shouldn't we move it once more in this same direction,
     so that the second trace is working like the first is?? BUG!?
  */

  LOGGG0(LOGG_BRES2, "\n\nAbout to trace the other branch of a conic section "
         "using the Bresenham method.\n");

  MOVE_POINT(&middle, last, dw, dh);
  (*bres_trace)(middle, last, dw, dh, func, fArg, keepgoing);
  /* now the other direction around, (DLC but from what point??) */

  LOGGG0(LOGG_BRES2, "\nDone tracing both branches of the conic section "
         "using the Bresenham method.\n");
}

void conx_bresenham(ConxBresenhamStarter *getB, ConxMetric *func,
                    void *fArg, double delta_x, double delta_y,
                    ConxContinueFunc *keepgoing,
                    ConxBresTraceFunc *bres_trace)
{
  Pt LB, RB;
  ConxDirection last;

  assert(delta_x != 0.0); assert(delta_y != 0.0);
  assert(keepgoing != NULL); assert(getB != NULL); assert(func != NULL);
  LOGGG0(LOGG_TEXINFO, "\n@conx_bresenham\n");

  /* Get points on the left and right branches. */
  getB(&LB, &RB);

  LOGGG4(LOGG_BRES2, "The two points are: (" DOF ", " DOF ") and (" DOF ", "
         DOF ")\n", LB.x, LB.y, RB.x, RB.y);

  /* Find an initial direction; trace the branch in one direction. */
  last = conx_startpoint(LB, delta_x, delta_y, func, fArg);
  LOGGG3(LOGG_BRES2, "Start point: (" DOF ", " DOF ") will move in %s "
         "direction", LB.x, LB.y, conx_direction2string(last));
  conx_bres_tracebranch(last, LB, delta_x, delta_y, func, fArg, keepgoing,
                        bres_trace);

  /* If we have two distinct branches, then trace the other also. */
  if ((RB.x != LB.x) || (RB.y != LB.y)) {
    LOGGG0(LOGG_BRES2, "\nTracing distinct second branch of the conic; still "
           "in Bresenham.\n");
    last = conx_startpoint(RB, delta_x, delta_y, func, fArg);
    LOGGG3(LOGG_BRES2, "Start point: (" DOF ", " DOF ") will move in %s "
           "direction", RB.x, RB.y, conx_direction2string(last));
    conx_bres_tracebranch(last, RB, delta_x, delta_y, func, fArg, keepgoing,
                          bres_trace);
  }
  LOGGG0(LOGG_TEXINFO, "\n@end conx_bresenham\n");
}

