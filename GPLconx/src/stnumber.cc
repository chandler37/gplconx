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
  Implementation of C++ classes in `stnumber.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stnumber.hh"
#include "sterror.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsNumber::ansMachs = NULL;

NF_INLINE
void CClsNumber::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ST_METHOD(ansMachs, "asFloat", OBJECT,
              oiAnswererAsFloat,
              "Returns a Float object instance version of the receiver");
  }
}

NF_INLINE
CClsBase::ErrType 
CClsNumber::oiActionAsFloat(CClsBase **result, CConxClsMessage &o)
{
  return asFloat(result);
}

NF_INLINE
double CClsNumber::getFloatValue() const
{
  abort(); // we can't get an object instance
  return 0.0;
}

NF_INLINE
CClsNumber &CClsNumber::operator=(const CClsNumber &o)
{
  (void) CClsBase::operator=(o);
  return *this;
}

NF_INLINE
CConxString CClsNumber::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    return CConxString("an ") + getClsName();
  }
}

NF_INLINE
CClsBase::ErrType CClsNumber::asFloat(CClsBase **result)
{
  REQUIRE_METHOD_IN_SUBCLASS();
}
