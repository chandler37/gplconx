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
  Implementation of C++ classes in `stcolor.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stcolor.hh"
#include "stfloat.hh"
#include "sterror.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsColor::ansMachs = NULL;

NF_INLINE
void CClsColor::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ansMachs->append(CConxClsAnsMach("new",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererNew,
                                     "Returns a new Color object instance"));
    ansMachs->append(CConxClsAnsMach("R:G:B:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererRGB,
                                     "Returns a new Color object instance set to a (red in [0.0, 1.0], green in [0.0, 1.0], blue in [0.0, 1.0]) RGB value"));
    ansMachs->append(CConxClsAnsMach("H:S:V:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererHSV,
                                     "Returns a new Color object instance set to a (hue in [0.0, 360.0), saturation in [0.0, 1.0], value in [0.0, 1.0]) HSV value"));

    ansMachs->append(CConxClsAnsMach("R:G:B:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererRGB,
                                     "Sets receiver to the given RGB value."));
    ansMachs->append(CConxClsAnsMach("H:S:V:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererHSV,
                                     "Sets receiver to the given HSV value."));

    ansMachs->append(CConxClsAnsMach("H",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererH,
                                     "Returns the hue"));
    ansMachs->append(CConxClsAnsMach("S", CConxClsAnsMach::OBJECT,
                                     oiAnswererS,
                                     "Returns the saturation"));
    ansMachs->append(CConxClsAnsMach("V", CConxClsAnsMach::OBJECT,
                                     oiAnswererV,
                                     "Returns the HSV Value"));
    ansMachs->append(CConxClsAnsMach("R", CConxClsAnsMach::OBJECT,
                                     oiAnswererR,
                                     "Returns the red component"));
    ansMachs->append(CConxClsAnsMach("G", CConxClsAnsMach::OBJECT,
                                     oiAnswererG,
                                     "Returns the green component"));
    ansMachs->append(CConxClsAnsMach("B", CConxClsAnsMach::OBJECT,
                                     oiAnswererB,
                                     "Returns the blue component"));
  }
}

NF_INLINE
CClsColor &CClsColor::operator=(const CClsColor &o)
{
  MMM("operator=");
  (void) CClsBase::operator=(o);
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
CConxString CClsColor::printString() const
{
  if (isClassInstance()) {
    return getClsName();
  } else {
    return CConxString("Color(RGB=[") + getColor().getR() + ", "
      + getColor().getG() + ", " + getColor().getB() + "])";
  }
}

NF_INLINE
CClsBase::ErrType 
CClsColor::oiActionRGB(CClsBase **result, CConxClsMessage &o)
{
  CHECK_READ_ONLYNESS(result);
  NEED_N_FLOATS(3, fargv, o, result);
  color.setRGB(fargv[0], fargv[1], fargv[2]);
  RETURN_THIS(result);
}

NF_INLINE
CClsBase::ErrType 
CClsColor::oiActionHSV(CClsBase **result, CConxClsMessage &o)
{
  CHECK_READ_ONLYNESS(result);
  NEED_N_FLOATS(3, fargv, o, result);
  color.setHSV(fargv[0], fargv[1], fargv[2]);
  RETURN_THIS(result);
}

NF_INLINE
CClsBase::ErrType 
CClsColor::ciActionRGB(CClsBase **result, CConxClsMessage &o) const
{
  return ciMetaActionNewSet(result, o, CConxColor::RGB);
}

NF_INLINE
CClsBase::ErrType 
CClsColor::ciActionHSV(CClsBase **result, CConxClsMessage &o) const
{
  return ciMetaActionNewSet(result, o, CConxColor::HSV);
}

NF_INLINE
CClsBase::ErrType
CClsColor::ciMetaActionNewSet(CClsBase **result, CConxClsMessage &o,
                              CConxColor::ColorType cType) const
{
  NEED_N_FLOATS(3, fargv, o, result);
  RETURN_NEW_RESULT(result, new CClsColor(CConxColor(fargv[0], fargv[1],
                                                     fargv[2], cType)));
  // DLC clamp in this constructor
}

CF_INLINE
CClsColor::CClsColor(const CClsColor &o)
  : CClsBase(o)
{
  MMM("copy constructor");
  uninitializedCopy(o);
}

NF_INLINE
CClsBase::ErrType
CClsColor::ciMetaActionGet(CClsBase **result, CConxClsMessage &o,
                           CConxColor::Component comp) const
  // DLC make all actions (that should be) const
{
  RETURN_NEW_RESULT(result, new CClsFloat(getColor().get(comp)));
}

