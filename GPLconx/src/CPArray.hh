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
  C++ class that owns an array of objects that are printable, copyable,
  equality testable, assignable, etc.  -- thus the whole array is printable.
*/

#ifndef GPLCONX_CPARRAY_CXX_H
#define GPLCONX_CPARRAY_CXX_H 1

#include <assert.h>
#include <iostream.h>

#include "COArray.hh"
#include "printon.hh"

//////////////////////////////////////////////////////////////////////////////
template <class Type>
class CConxPrintableOwnerArray
  : VIRT public CConxOwnerArray<Type>, public CConxPrintable {
  CCONX_CLASSNAME("CConxPrintableOwnerArray")
public:
  CConxPrintableOwnerArray(const CConxPrintableOwnerArray &o)
    : CConxOwnerArray<Type>(o) { }
  CConxPrintableOwnerArray() { }
  
  // For easy initialization of up to four entries:
  CConxPrintableOwnerArray(const Type &v1, const Type &v2,
                           const Type &v3, const Type &v4) throw(const char *)
    : CConxOwnerArray<Type>(v1, v2, v3, v4) { }
  CConxPrintableOwnerArray(const Type &v1, const Type &v2,
                           const Type &v3) throw(const char *)
    : CConxOwnerArray<Type>(v1, v2, v3) { }
  CConxPrintableOwnerArray(const Type &v1, const Type &v2) throw(const char *)
    : CConxOwnerArray<Type>(v1, v2) { }
  CConxPrintableOwnerArray(const Type &v1) throw(const char *)
    : CConxOwnerArray<Type>(v1) { }
  CConxPrintableOwnerArray(Type *v1, Type *v2 = NULL, Type *v3 = NULL,
                           Type *v4 = NULL) throw(const char *)
    : CConxOwnerArray<Type>(v1, v2, v3, v4) { }

  CConxPrintableOwnerArray &operator=(const CConxPrintableOwnerArray &o)
  {
    (void) CConxOwnerArray<Type>::operator=(o);
    return *this;
  }

  ostream &printOn(ostream &o) const
  {
    // DLC move to .cc
    o << "<" << className() << " size = " << size();
    for (size_t i = 0; i < size(); i++) {
      o << " \"";
      get(i).printOn(o);
      o << "\"";
    }
    o << ">";
    return o;
  }
}; // class CConxPrintableOwnerArray

#endif // GPLCONX_CPARRAY_CXX_H

