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
  Implementation of C++ classes in `hypmath.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hypmath.hh"
#include "viewer.h"

void CConxMultiModel::mmconvert(double xFrom, double yFrom, ConxModlType mFrom,
                                double *xTo, double *yTo, ConxModlType mTo)
// Convert from (xFrom, yFrom) in the mFrom model to
// (*xTo, *yTo) in the mTo model.
{
  if (mFrom == CONX_POINCARE_UHP) {
    if (mTo == CONX_POINCARE_UHP) {
      *xTo = xFrom;
      *yTo = yFrom;
    } else if (mTo == CONX_POINCARE_DISK) {
      conxhm_ptopd(xFrom, yFrom, xTo, yTo);
    } else {
      assert(mTo == CONX_KLEIN_DISK);
      conxhm_ptok(xFrom, yFrom, xTo, yTo);
    }
  } else if (mFrom == CONX_POINCARE_DISK) {
    if (mTo == CONX_POINCARE_UHP) {
      conxhm_pdtop(xFrom, yFrom, xTo, yTo);
    } else if (mTo == CONX_POINCARE_DISK) {
      *xTo = xFrom;
      *yTo = yFrom;
    } else {
      assert(mTo == CONX_KLEIN_DISK);
      conxhm_pdtok(xFrom, yFrom, xTo, yTo);
    }
  } else {
    assert(mFrom == CONX_KLEIN_DISK);
    if (mTo == CONX_POINCARE_UHP) {
      conxhm_ktop(xFrom, yFrom, xTo, yTo);
    } else if (mTo == CONX_POINCARE_DISK) {
      conxhm_ktopd(xFrom, yFrom, xTo, yTo);
    } else {
      assert(mTo == CONX_KLEIN_DISK);
      *xTo = xFrom;
      *yTo = yFrom;
    }
  }
}
