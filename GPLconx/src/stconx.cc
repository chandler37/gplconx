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
  Implementation of C++ classes in `stconx.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stconx.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsPoint::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsLine::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsModelIdentifier::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsColor::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsDrawable::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsCanvas::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsCircle::ansMachs = NULL;
// DLC NEWSTCLASS


NF_INLINE
CConxString CClsPoint::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    assert(xCoord != NULL); assert(yCoord != NULL); assert(model != NULL);
    CConxString s = getClsName();
    s += "(x: " + xCoord->printString() + ", y: " + yCoord->printString()
      + ", model: " + model->printShortString() + ")";
    s += " -- " + CClsDrawable::printString();
    return s;
    // DLC this may show `Point(x: 0.36, y: -0.1, model: uhp)' when
    // -0.1 will be clamped to [0.0, inf)

  }
}
