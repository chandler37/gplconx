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
  C++ class that has a reference count -- helpful for garbage collection.
*/

#ifndef GPLCONX_GC_CXX_H
#define GPLCONX_GC_CXX_H 1

#include "printon.hh"

//////////////////////////////////////////////////////////////////////////////
// An object with a reference count, useful for garbage collection (GC)
// routines.  There is no need to subclass this correctly, i.e. to call its
// copy constructor and assignment operator, because a copy's use count should
// begin at zero.  You have to increment the copied object's reference count
// if you want that to happen.
class CConxGCObject : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxGCObject")
public:
  CConxGCObject() { numUsers = 0; }
  CConxGCObject(size_t initialNumUsers) { numUsers = initialNumUsers; }
  CConxGCObject(const CConxGCObject &o) : CConxObject(o) { numUsers = 0; }
  CConxGCObject &operator=(const CConxGCObject &o);

  // virtual so that CConxClsManager::removeUser will work without
  // a memory leak, e.g.:
  ~CConxGCObject() { }

  ostream &printOn(ostream &o) const;
  size_t decrementUsers() throw(int);
  void incrementUsers();
  void setNumUsers(size_t sz) { numUsers = sz; }
  size_t getNumUsers() const { return numUsers; }

private: // attributes
  size_t numUsers;
}; // class CConxGCObject


#endif // GPLCONX_GC_CXX_H
