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
  Implementation of C++ classes in `sth_assn.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sth_assn.hh"

CF_INLINE
CConxClsAssignmentList::CConxClsAssignmentList(const CConxClsAssignmentList &o)
  : CConxObject(o)
{
  // DLC finish this!
  abort();
}

NF_INLINE
CConxClsAssignmentList &
CConxClsAssignmentList::operator=(const CConxClsAssignmentList &o)
{
  (void) CConxObject::operator=(o);

  // DLC finish this!
  abort();
  return *this;
}

NF_INLINE
const CConxString &CConxClsAssignmentList::getNthVar(size_t n) const
  throw(const char *)
{
  if (n >= getNumberVars()) {
    throw "Cannot get Nth var CConxClsAssignmentList";
  }
  return vars.get(n);
}

NF_INLINE
ostream &CConxClsAssignmentList::printOn(ostream &o) const
{
  o << "<CConxClsAssignmentList";
  for (size_t i = 0; i < getNumberVars(); i++) {
    o << " " << getNthVar(i);
  }
  o << ">";
  return o;
}

OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsAssignmentList)
