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
  Implementation of C++ classes in `sth_kmsg.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sth_kmsg.hh"
#include "stobject.hh"

NF_INLINE
void CConxClsKeywordMessage::getBoundObjects(CClsBase *argv[])
  throw (const char *)
{
  for (size_t i = 0; i < getNumberArgs(); i++) {
    argv[i] = getNthObject(i);
    assert(argv[i] != NULL);
    // we initialize variables to `nil', so this should be true.
  }
}

NF_INLINE
CConxClsKeywordMessage &CConxClsKeywordMessage::operator=(const CConxClsKeywordMessage &o)
{
  (void) CConxObject::operator=(o);
  parts = o.parts;
  return *this;
}

CF_INLINE
CConxClsKeywordMessage::~CConxClsKeywordMessage()
{
  /* parts's destruction triggers CConxClsKeyedArg's destructor,
     which does the work of GC.
  */
}

NF_INLINE
int CConxClsKeywordMessage::operator==(const CConxClsKeywordMessage &o) const
{
  return (parts == o.parts);
}

NF_INLINE
int CConxClsKeywordMessage::operator!=(const CConxClsKeywordMessage &o) const
{
  return !operator==(o);
}

NF_INLINE
void CConxClsKeywordMessage::appendKeyedArg(const CConxClsKeyedArg &a)
{
  appendKeyedArg(new CConxClsKeyedArg(a)); // DLC check oom
}

NF_INLINE
void CConxClsKeywordMessage::appendKeyedArg(CConxClsKeyedArg *a)
  throw(const char *)
{
  if (a == NULL) {
    throw "needed non-NULL pointer in "
      "CConxClsKeywordMessage::appendKeyedArg(CConxClsKeyedArg *a)";
  }
  //    cout << "\nDLC keymsg appending " << a << endl;
  parts.append(a);
  assert(&parts.get(parts.size() - 1) == a);
}

NF_INLINE
void CConxClsKeywordMessage::getObjects(CClsBase *argv[])
  // This fills argv with getNumberArgs() pointers to Smalltalk-like
  // instances.  Some may be NULL.
{
  // DLC move to .cc
  for (size_t i = 0; i < getNumberArgs(); i++) {
    argv[i] = getNthObject(i);
    assert(argv[i] != NULL); // we initialize variables to `nil'
  }
}

NF_INLINE
CClsBase *CConxClsKeywordMessage::getNthObject(size_t n) throw(const char *)
{
  return getNthArg(n).getArg();
}

NF_INLINE
CConxClsKeyedArg &CConxClsKeywordMessage::getNthArg(size_t n) throw(const char *)
{
  return parts.get(n); // may throw
}

NF_INLINE
const CConxClsKeyedArg &CConxClsKeywordMessage::getNthArg(size_t n) const throw(const char *)
{
  return parts.get(n); // may throw
}

NF_INLINE
CConxString CConxClsKeywordMessage::getKeywordMessageName() const
  // Returns `between:and:', e.g. (without the leading hash mark).
{
  CConxString name;
  for (size_t i = 0; i < getNumberArgs(); i++) {
    name += getNthArg(i).getKeyword() + ":";
  }
  assert(name.numberOf(':') == getNumberArgs());
  return name;
}

NF_INLINE
Boole CConxClsKeywordMessage::isMessageNamed(const CConxString &s) const
{
  return (getKeywordMessageName() == s);
}

NF_INLINE
ostream &CConxClsKeywordMessage::printOn(ostream &o) const
{
  o << "<" << className() << " object with child ";
  (void) parts.printOn(o);
  o << ">";
  return o;
}
