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
  Implementation of C++ classes in `stundefo.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stundefo.hh"
#include "ststring.hh"
#include "sterror.hh"

NF_INLINE
CConxString CClsUndefinedObject::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    return CConxString("nil");
  }
}

NF_INLINE
CClsUndefinedObject &
CClsUndefinedObject::operator=(const CClsUndefinedObject &o)
{
  MMM("assignment operator");
  (void) CClsBase::operator=(o);
  return *this;
}

NF_INLINE
int CClsUndefinedObject::operator==(const CClsUndefinedObject &o)
{
  return (isClassInstance() == o.isClassInstance());
}

