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
  C++ classes to model computational hyperbolic geometry in three models.
*/

#ifndef GPLCONX_HYPMATH_CXX_H
#define GPLCONX_HYPMATH_CXX_H 1

#include <assert.h>

#include "point.hh"
extern "C" {
#include "viewer.h"
}

#include "decls.hh"

// class CConxPoint;
// class CConxLine;
// class CConxCircle;
// class CConxParabola;
// class CConxHypEllipse;
// class CConxEqDistCurve;

/* DLC delete this junk!
//////////////////////////////////////////////////////////////////////////////
// Abstract class:
class CConxGeomModel {
public:
  static double dist(const CConxPoint& A, const CConxPoint& B);
  static virtual double dist(double x1, double y1, double x2, double y2)=0;
  static virtual void convert(double x, double y, double &cx, double &cy,
                              ConxModlType modl)=0;


protected:
  static virtual double dist(double x1, double y1, double x2, double y2)=0;
};

// The Poincare Upper Half Plane:
class CConxPUHP : VIRT public CConxGeomModel {
};

// The Klein Disk (Beltrami-Klein Disk):
class CConxKD : VIRT public CConxGeomModel {
};

// The Poincare Disk:
class CConxPD : VIRT public CConxGeomModel {
};

inline CConxGeomModel::dist(const CConxPoint& A, const CConxPoint& B)
{
  return dist(A.getX(), A.getY(), B.getX(), B.getY());
}
*//* DLC */


//////////////////////////////////////////////////////////////////////////////
// A class with static functions that simplify coding for multiple models
// of hyperbolic geometry.  There is some overhead involved, of course.
class CConxMultiModel : VIRT public CConxObject {
  CCONX_CLASSNAME("CConxMultiModel")
public:
  static void mmconvert(double xFrom, double yFrom, ConxModlType mFrom,
                        double *xTo, double *yTo, ConxModlType mTo);
};


#endif // GPLCONX_HYPMATH_CXX_H
