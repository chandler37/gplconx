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
  C++ class that owns an array of simple objects like pointers or ints.
*/

#ifndef GPLCONX_CSARRAY_CXX_H
#define GPLCONX_CSARRAY_CXX_H 1

#include <assert.h>

#include "CObject.hh"

//////////////////////////////////////////////////////////////////////////////
// This holds items that do not need to be deleted, e.g. pointers, ints,
// or doubles.
template <class Type>
class CConxSimpleArray : VIRT public CConxObject {
  CCONX_CLASSNAME("CConxSimpleArray")
#define CSA_INIT() simpleObjects = NULL; alloced_sz = sz = 0

public:
  CConxSimpleArray()
  {
    CSA_INIT();
  }
  CConxSimpleArray(const Type &t1) { CSA_INIT(); append(t1); }
  CConxSimpleArray(const Type &t1, const Type &t2) { CSA_INIT(); append(t1); append(t2); }
  CConxSimpleArray(const Type &t1, const Type &t2, const Type &t3)
  { CSA_INIT(); append(t1); append(t2); append(t3); }
  CConxSimpleArray(const Type &t1, const Type &t2, const Type &t3, const Type &t4)
  { CSA_INIT(); append(t1); append(t2); append(t3); append(t4); }
  ~CConxSimpleArray()
  {
    clear();
  }
  CConxSimpleArray(const CConxSimpleArray &o) throw(const char *)
    : CConxObject(o)
  {
    CSA_INIT();
    initializedCopy(o);
  }
  CConxSimpleArray &operator=(const CConxSimpleArray &o)
  {
    (void) CConxObject::operator=(o);
    clear();
    initializedCopy(o);
    return *this;
  }

  int operator==(const CConxSimpleArray &o) const
  {
    if (sz != o.sz) return 0;
    for (size_t i = 0; i < sz; i++) {
      if (simpleObjects[i] != o.simpleObjects[i]) return 0;
    }
    return 1;
  }
  int operator!=(const CConxSimpleArray &o) const { return !operator==(o); }

  size_t size() const { return sz; }
  void clear()
  {
    if (simpleObjects) {
      assert(alloced_sz > 0);
      delete [] simpleObjects;
      simpleObjects = NULL;
      alloced_sz = sz = 0;
    } else {
      assert(sz == 0 && alloced_sz == 0);
    }
  }
  Type get(size_t n) const throw(const char *)
  {
    if (n >= sz)
      throw "index out of bounds in Type &CConxSimpleArray::get(size_t n) const";
    return simpleObjects[n];
  }
  void append(const Type &a) throw(const char *)
  // Puts a at the end of the array.
  {
    if (sz + 1 >= alloced_sz) {
      // yes >= is necessary due to initial sz=alloced_sz=0

#define ONE_HALF_INITIAL_SIZE 2
      grow(2*((alloced_sz < ONE_HALF_INITIAL_SIZE)
              ? ONE_HALF_INITIAL_SIZE : alloced_sz));
    }
    simpleObjects[sz++] = a;
  }
  void prepend(const Type &a) throw(const char *)
  // Puts a at the beginning of the array.
  {
    if (sz + 1 >= alloced_sz) {
      // yes >= is necessary due to initial sz=alloced_sz=0

#define ONE_HALF_INITIAL_SIZE 2
      grow(2*((alloced_sz < ONE_HALF_INITIAL_SIZE)
              ? ONE_HALF_INITIAL_SIZE : alloced_sz));
    }
    for (size_t i = sz; i > 0; i--) {
      simpleObjects[i] = simpleObjects[i-1];
    }
    ++sz;
    simpleObjects[0] = a;
  }
  // DLC is there a need to replace elements rather than change elements in place?

  void atPut(size_t n, const Type &t) throw(const char *)
  {
    if (n >= size()) throw("that position does not yet exist");
    simpleObjects[n] = t;
  }
  void deleteEntry(size_t n) throw(const char *)
  // n == 0 for the first.
  {
    if (!(n < sz)) throw "bad index";
    for (size_t k = n; k < sz; k++) {
      simpleObjects[k] = simpleObjects[k+1];
    }
    --sz;
  }

protected:
  void grow(size_t newSize) throw(const char *)
  {
    if (newSize <= alloced_sz) return;
    Type *newsimpleObjects = new Type[newSize];
    if (newsimpleObjects == NULL)
      throw "out of memory in CConxSimpleArray::grow(ssize_t newSize)";
    for (size_t i = 0; i < sz; i++)
      newsimpleObjects[i] = simpleObjects[i];
    if (simpleObjects) delete [] simpleObjects;
    simpleObjects = newsimpleObjects;
    alloced_sz = newSize;
  }
  void initializedCopy(const CConxSimpleArray &o)
  {
    grow(o.sz);
    assert(alloced_sz == o.sz);
    for (size_t i = 0; i < o.sz; i++) {
      simpleObjects[i] = o.simpleObjects[i];
    }
    sz = o.sz;
  }

private:
  size_t sz;
  size_t alloced_sz;
  Type *simpleObjects;
}; // class CConxSimpleArray


#endif // GPLCONX_CSARRAY_CXX_H
