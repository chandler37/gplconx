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
  C++ class that owns an array of objects that are copyable,
  equality testable, assignable, etc.
*/

#ifndef GPLCONX_COARRAY_CXX_H
#define GPLCONX_COARRAY_CXX_H 1

#include <assert.h>

#include "CObject.hh"

template <class Type>
class CConxOwnerArray : VIRT public CConxObject {
  CCONX_CLASSNAME("CConxOwnerArray")
public:
#define CCOA_INIT() ownedObjects = NULL; alloced_sz = sz = 0

  CConxOwnerArray() { CCOA_INIT(); }
  CConxOwnerArray(const Type &v1, const Type &v2,
                  const Type &v3, const Type &v4) throw(const char *)
  // This is a shortcut that initializes the array to have four entries,
  // copies of v1 through v4.
  {
    CCOA_INIT(); append(v1); append(v2); append(v3); append(v4);
  }
  CConxOwnerArray(const Type &v1, const Type &v2,
                  const Type &v3) throw(const char *)
  // This is a shortcut that initializes the array to have three entries,
  // copies of v1 through v3.
  {
    CCOA_INIT(); append(v1); append(v2); append(v3);
  }
  CConxOwnerArray(const Type &v1, const Type &v2) throw(const char *)
  // This is a shortcut that initializes the array to have two entries,
  // copies of v1 and v2.
  {
    CCOA_INIT(); append(v1); append(v2);
  }
  CConxOwnerArray(const Type &v1) throw(const char *)
  // This is a shortcut that initializes the array to have one entries,
  // a copy of v1.
  {
    CCOA_INIT(); append(v1);
  }
  CConxOwnerArray(Type *v1, Type *v2 = NULL,
                  Type *v3 = NULL, Type *v4 = NULL) throw(const char *)
  // This is a shortcut that initializes the array to have between zero and
  // four entries, the non-NULL members of [v1, v2, v3, v4].  They are owned
  // if an out of memory throw does not occur; do not touch them again.
  {
    CCOA_INIT();
    if (v1 != NULL) append(v1); if (v2 != NULL) append(v2);
    if (v3 != NULL) append(v3); if (v4 != NULL) append(v4);
  }
  ~CConxOwnerArray()
  {
    clear();
  }
  CConxOwnerArray(const CConxOwnerArray &o) throw(const char *)
    : CConxObject(o)
  {
    alloced_sz = sz = 0;
    ownedObjects = NULL;
    initializedCopy(o);
  }
  CConxOwnerArray &operator=(const CConxOwnerArray &o)
  {
    (void) CConxObject::operator=(o);
    clear();
    initializedCopy(o);
    return *this;
  }

  int operator==(const CConxOwnerArray &o) const
  {
    if (sz != o.sz) return 0;
    for (size_t i = 0; i < sz; i++) {
      if (*ownedObjects[i] != *o.ownedObjects[i]) return 0;
    }
    return 1;
  }
  int operator!=(const CConxOwnerArray &o) const { return !operator==(o); }

  size_t size() const { return sz; }
  void clear()
  {
    if (ownedObjects != NULL) {
      assert(alloced_sz > 0);
      for (size_t i = 0; i < sz; i++)
        delete ownedObjects[i];
      delete [] ownedObjects;
      ownedObjects = NULL;
      alloced_sz = sz = 0;
    } else {
      assert(sz == 0 && alloced_sz == 0);
    }
  }
  const Type &get(size_t n) const throw(const char *)
  {
    if (n >= sz)
      throw "index out of bounds in const Type &CConxOwnerArray::get(size_t n) const";
    return *ownedObjects[n];
  }
  Type &get(size_t n) throw(const char *)
  {
    if (n >= sz)
      throw "index out of bounds in Type &CConxOwnerArray::get(size_t n)";
    return *ownedObjects[n];
  }
  void append(Type **a, size_t n) throw(const char *)
  // Append n elements from the array a.  Note that a is a const array that
  // points to non-const pointers who are our property after success and
  // are possibly our property after an out of memory throw.
  {
    if (a == NULL && n > 0) throw "a cannot be NULL.";
    for (size_t i = 0; i < n; i++) {
      append(a[i]);
    }
  }
  void append(Type *a) throw(const char *)
  // This array takes ownership of *a and will delete it when clear()
  // is called or this object is destroyed.  If out of memory is thrown,
  // then a is still your property.
  {
    if (a == NULL)
      throw "illegal NULL a in CConxOwnerArray::append(Type *a)";
    if (sz + 1 >= alloced_sz) {
      // yes >= is necessary due to initial sz=alloced_sz=0

#define ONE_HALF_INITIAL_SIZE 2
      grow(2*((alloced_sz < ONE_HALF_INITIAL_SIZE)
              ? ONE_HALF_INITIAL_SIZE : alloced_sz));
    }
    ownedObjects[sz++] = a;
  }
  void append(const Type *a) throw(const char *)
  // This array does not take ownership of *a, it copies *a.
  {
    Type *n = new Type(*a);
    append(n);
  }
  void append(const Type &s) throw(const char *)
  // A copy of s is put at the end of the array.
  {
    Type *n = new Type(s);
    append(n);
  }
  void prepend(Type *a) throw(const char *)
  // This array takes ownership of *a, which is put at the head of the
  // array (index 0) and will delete it when clear()
  // is called or this object is destroyed.  If out of memory is thrown,
  // then a is still your property.
  {
    if (a == NULL)
      throw "illegal NULL a in CConxOwnerArray::append(Type *a)";
    if (sz + 1 >= alloced_sz) {
      // yes >= is necessary due to initial sz=alloced_sz=0

#define ONE_HALF_INITIAL_SIZE 2
      grow(2*((alloced_sz < ONE_HALF_INITIAL_SIZE)
              ? ONE_HALF_INITIAL_SIZE : alloced_sz));
    }
    for (size_t i = sz; i > 0; i--) {
      ownedObjects[i] = ownedObjects[i-1];
    }
    ++sz;
    ownedObjects[0] = a;
  }
  void prepend(const Type &s) throw(const char *)
  // A copy of s is put at the beginning of the array.
  {
    Type *n = new Type(s);
    prepend(n);
  }

  // DLC is there a need to replace elements rather than change elements in place?

protected:
  void grow(size_t newSize) throw(const char *)
  {
    if (newSize <= alloced_sz) return;
    Type **newownedObjects = new (Type *)[newSize];
    if (newownedObjects == NULL)
      throw "out of memory in CConxOwnerArray::grow(ssize_t newSize)";
    for (size_t i = 0; i < sz; i++)
      newownedObjects[i] = ownedObjects[i];
    for (size_t j = sz; j < newSize; j++)
      newownedObjects[j] = NULL;
    if (ownedObjects != NULL) delete [] ownedObjects;
    ownedObjects = newownedObjects;
    alloced_sz = newSize;
  }
  void initializedCopy(const CConxOwnerArray &o)
  {
    grow(o.sz);
    assert(alloced_sz == o.sz);
    for (size_t i = 0; i < o.sz; i++) {
      ownedObjects[i] = new Type(*o.ownedObjects[i]);
    }
    sz = o.sz;
  }

private:
  size_t sz;
  size_t alloced_sz;
  Type **ownedObjects;
}; // class CConxOwnerArray

#endif // GPLCONX_COARRAY_CXX_H

