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
  The root of the GPLconx C++ class hierarchy, CConxObject.
*/

#ifndef GPLCONX_COBJECT_CXX_H
#define GPLCONX_COBJECT_CXX_H 1

#include <assert.h>

#include "decls.hh"

//////////////////////////////////////////////////////////////////////////////
// The base class of all base classes.
// The counting is unnecessary overhead but helpful for debugging.
class CConxObject {
  CCONX_CLASSNAME("CConxObject")
public:
  CConxObject()
  {
#ifndef NDEBUG
    debuggingTag =
#endif
      ++numCreated;
  }
  CConxObject(const CConxObject &o)
  {
#ifndef NDEBUG
    debuggingTag =
#endif
      ++numCreated;
  }
  CConxObject &operator=(const CConxObject &o)
  {
    // do not transfer the debugging tag.
    return *this;
  }

  // When wouldn't you want a virtual destructor???
  virtual ~CConxObject()
  {
    ++numDestroyed;

    assert(this != NULL);
#ifndef NDEBUG
    if (cerr_from_destructor) {
      cerr << "\nDestroying " << className() << " instance with debugging tag `" << flush;
      cerr << debuggingTag << "'" << endl << flush;
    }
#endif

    if (LLL_ON() && numDestroyed >= numCreated) {
      char hm[256];
      (void) sprintf(hm, "<<<<< CConxObject::~CConxObject()\nThere are now, after this destruction, %lu objects"
                     " in existence.\n>>>>>", (unsigned long) numberExisting());
      LLL(hm);
    }
  }

  static size_t numberExisting() { return numCreated - numDestroyed; }

  // When you get a core dump in a destructor, you'd like to know which
  // object gave it to you.  If you use unique debugging tags, you can
  // tell easily. NDEBUG makes these functions useless; a set is ignored
  // and a get always returns 0.
#ifndef NDEBUG
  void setDebuggingTag(long l)
  {
    debuggingTag = l;
  }
#else
#define setDebuggingTag(l) /* ignore */
#endif

#ifndef NDEBUG
  long getDebuggingTag() const
  {
    return debuggingTag;
  }
#else
#define getDebuggingTag() 0L
#endif

#ifndef NDEBUG
#define TAG(object, tag) (object).setDebuggingTag(tag)
#else
#define TAG(object, tag)
#endif


#ifndef NDEBUG
public:
  static Boole cerr_from_destructor;
#endif

private:
#ifndef NDEBUG
  long debuggingTag;
#endif
  static size_t numCreated, numDestroyed;
};

#endif // GPLCONX_COBJECT_CXX_H
