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
  C++ Smalltalkish UndefinedObject class.
*/

#ifndef GPLCONX_STUNDEFOBJ_CXX_H
#define GPLCONX_STUNDEFOBJ_CXX_H 1

#include "stobject.hh"


//////////////////////////////////////////////////////////////////////////////
// Our equivalent to Smalltalk's UndefinedObject class, nil's class.
class CClsUndefinedObject : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsUndefinedObject")
  CLSNAME("UndefinedObject", "I have a very popular instance, `nil', that you will encounter when you have a variable that is not set.")
  CLSTYPE(CClsBase, CLS_UNDEFINED_OBJECT)
  STCLONE2(CClsUndefinedObject)
public:
  CClsUndefinedObject() { MMM("CClsUndefinedObject"); }
  CClsUndefinedObject(const CClsUndefinedObject &o)
    : CClsBase(o) { MMM("copy constructor"); }
  CClsUndefinedObject &operator=(const CClsUndefinedObject &o);
  ~CClsUndefinedObject() { MMM("destructor"); /* cerr << "DLC destroying " << this << endl; */}

  // Uses CClsBase::sendMessage
  CConxString printString() const;
  int operator==(const CClsUndefinedObject &o);
  int operator!=(const CClsUndefinedObject &o) { return !operator==(o); }
}; // class CClsUndefinedObject


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STUNDEFOBJ_CXX_H
