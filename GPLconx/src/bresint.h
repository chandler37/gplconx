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
  Internal header for the Bresenham method.
 */

#ifndef CONX_BRESINT_H
#define CONX_BRESINT_H 1

#include "util.h"

/* Here are two ways to move a point in one of eight directions: */

/* void MOVE_POINT(Pt *current_location, ConxDirection dir,
                   double dw, double dh)
   moves *current_location in the direction of dir, where dw controls
   horizontal movement and dh controls vertical movement. */


/* DLC figure out which is faster. */
#define MOVE_POINT(current_location, dir, dw, dh) \
  switch (dir) { \
  case CXD_NW: (current_location)->x-=(dw); \
  case CXD_N: (current_location)->y+=(dh); \
    break; \
  case CXD_SE: (current_location)->x+=(dw); \
  case CXD_S: (current_location)->y-=(dh); \
    break; \
  case CXD_NE: (current_location)->y+=(dh); \
  case CXD_E: (current_location)->x+=(dw); \
    break; \
  case CXD_SW: (current_location)->y-=(dh); \
  case CXD_W: (current_location)->x-=(dw); \
    break; \
  default: FATAL("ack"); \
  }

#ifdef DLC
#define MM_HORIZONTAL(dir) \
   ( ((dir) == CXD_NW || (dir) == CXD_W || (dir) == CXD_SW) \
       ? -1.0 \
       : (((dir) != CXD_N && (dir) != CXD_S) ? 1.0 : 0.0) )

#define MM_VERTICAL(dir) \
   ( ((dir) == CXD_N || (dir) == CXD_NW || (dir) == CXD_NE) \
       ? 1.0 \
       : (((dir) != CXD_W && (dir) != CXD_E) ? -1.0 : 0.0) )
#define ALT_MOVE_POINT(current_location, dir, dw, dh) \
  (current_location)->x += MM_HORIZONTAL((dir)) * (dw); \
  (current_location)->y += MM_VERTICAL((dir)) * (dh)
#endif

#endif /* CONX_BRESINT_H */
