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
  Implementation of C++ classes in `stsmalli.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stsmalli.hh"
#include "sterror.hh"
#include "stboole.hh"
#include "stfloat.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsSmallInt::ansMachs = NULL;

NF_INLINE
void CClsSmallInt::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ST_METHOD(ansMachs, "between:and:", OBJECT, oiAnswererBetweenAnd,
              "Returns true iff the inclusive range "
                                     "from the first argument to the second"
                                     " argument contains the receiver");
    ST_METHOD(ansMachs, "new", CLASS, ciAnswererNew,
              "Returns a new SmallInt object instance");
  }
}

NF_INLINE
CClsBase::ErrType 
CClsSmallInt::oiActionBetweenAnd(CClsBase **result, CConxClsMessage &o) const
{
  assert(result != NULL); CClsBase *argv[2]; o.getBoundObjects(argv);
  for (int i = 0; i < 2; i++)
    ENSURE_KEYWD_TYPE(result, o, argv, i, CLS_SMALLINT, TRUE);
  RETURN_BOOLE(this->getValue() >= ((CClsSmallInt *)argv[0])->getValue()
               && this->getValue() <= ((CClsSmallInt *)argv[1])->getValue(),
               result);
}

NF_INLINE
CConxString CClsSmallInt::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    char s[250];
    (void) sprintf(s, "%ld", getValue());
    return CConxString(s);
  }
}

NF_INLINE
CClsSmallInt &CClsSmallInt::operator=(const CClsSmallInt &o)
{
  (void) CClsNumber::operator=(o);
  d = o.d;
  return *this;
}

NF_INLINE
void CClsSmallInt::setValue(long od)
{
  if (isClassInstance()) throw "set not for `SmallInt'";
  d = od;
}

NF_INLINE
long CClsSmallInt::getValue() const
{
  if (isClassInstance()) throw "get not for `SmallInt'";
  return d;
}

NF_INLINE
CClsBase::ErrType CClsSmallInt::asFloat(CClsBase **result)
{
  RETURN_FLOAT_R(getValue());
}
