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
  Implementation of C++ classes in `stcharar.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stcharar.hh"
#include "stsmalli.hh"
#include "sterror.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsCharacterArray::ansMachs = NULL;
// DLC NEWSTCLASS


NF_INLINE CClsBase::ErrType
CClsCharacterArray::oiActionLength(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result,
                    new CClsSmallInt((long)(getValue().getLength())));
}

NF_INLINE
void CClsCharacterArray::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    // DLC "length", e.g.
    ansMachs->append(CConxClsAnsMach("set:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererSet,
                                     "Sets the receiver's value to that of the object passed in if their types agree"));
    ansMachs->append(CConxClsAnsMach("length",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererLength,
                                     "Returns the receiver's length, which does not include the '#' character"));

  }
}

NF_INLINE
CClsCharacterArray &CClsCharacterArray::operator=(const CClsCharacterArray &o)
{
  (void) CClsBase::operator=(o);
  string = o.string;
  return *this;
}

NF_INLINE
CClsBase::ErrType CClsCharacterArray::oiActionSet(CClsBase **result,
                                                  CConxClsMessage &o)
{
  ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsCharacterArray, argv, o);
  CHECK_READ_ONLYNESS(result);
  if (getType() == CLS_SYMBOL && !argv[0]->isType(CLS_SYMBOL))
    RETURN_ERROR_RESULT(result,
                        "You can set a String to a Symbol, but not vice versa");
  setValue(argv[0]->getValue());
  RETURN_THIS(result);
}
