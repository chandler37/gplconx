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
  Implementation of C++ classes in `gcobject.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gcobject.hh"


NF_INLINE
CConxGCObject &CConxGCObject::operator=(const CConxGCObject &o)
{
  (void) CConxObject::operator=(o);
  // the users point to this object, so they remain the same number.
  return *this;
}

NF_INLINE
ostream &CConxGCObject::printOn(ostream &o) const
{
  o << "<CConxGCObject with " << getNumUsers() << " references to it>";
  return o;
}

NF_INLINE
size_t CConxGCObject::decrementUsers() throw(int)
{
  MMM("size_t decrementUsers() throw(int)");
  LLL("decrementing from " << getNumUsers());
  if (getNumUsers() == 0) throw 0;
  LLL("before setNumUsers");
  setNumUsers(getNumUsers() - 1);
  return getNumUsers();
}

NF_INLINE
void CConxGCObject::incrementUsers()
{
  MMM("void incrementUsers()");
  setNumUsers(1+getNumUsers());
}
