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
  Implementation of C++ classes in `CArray.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "CArray.hh"

OOLTLTI_INLINE
template <class Type>
P_STREAM_OUTPUT_SHORTCUT(CConxPrintableOwnerArray<Type>)

// Implementation of function in Starray.h:
#include "CString.hh"
OOLTLTI_INLINE
ostream &operator<<(ostream &o, const CConxPrintableOwnerArray<CConxString> &a)
{
  return a.printOn(o);
}
