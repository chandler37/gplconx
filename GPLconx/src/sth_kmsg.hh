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
  C++ Smalltalkish back-end CConxClsKeywordMessage class.
*/

#ifndef GPLCONX_STH_KMSG_CXX_H
#define GPLCONX_STH_KMSG_CXX_H 1

#include "CString.hh"
#include "CArray.hh"
#include "sth_karg.hh"

class CClsBase;
//////////////////////////////////////////////////////////////////////////////
// A class that represents a keyword message, such as class Magnitude's
// #between:and: message, with arguments.  Thus, sending an instance of this
// class to a Smalltalk-like class instance or object instance will allow
// execution of the keyword message.
// DLC garbage collection????  We decrement users when this is destructed,
// right??  What about parse errors???  Make sure we destruct as many
// instances as we create!
class CConxClsKeywordMessage : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsKeywordMessage")
public:
  CConxClsKeywordMessage() { }
  CConxClsKeywordMessage(const CConxClsKeywordMessage &o)
    : CConxObject(o), parts(o.parts) { }
  CConxClsKeywordMessage &operator=(const CConxClsKeywordMessage &o);
  ~CConxClsKeywordMessage();
  int operator==(const CConxClsKeywordMessage &o) const;
  int operator!=(const CConxClsKeywordMessage &o) const;
  size_t getNumberArgs() const { return parts.size(); }
  // The nth argument is the argument to the nth keyword; n >= 0
  void appendKeyedArg(const CConxClsKeyedArg &a);
  void appendKeyedArg(CConxClsKeyedArg *a) throw(const char *);
  void getObjects(CClsBase *argv[]);

  void getBoundObjects(CClsBase *argv[]) throw (const char *);
  // Fills argv with getNumberArgs() pointers to Smalltalk-like
  // object and class instances.  All will be bound right now -- none
  // will be CClsLBVariable instances.  Some may be NULL.

  CClsBase *getNthObject(size_t n) throw(const char *);
  CConxClsKeyedArg &getNthArg(size_t n) throw(const char *);
  const CConxClsKeyedArg &getNthArg(size_t n) const throw(const char *);
  CConxString getKeywordMessageName() const;
  Boole isMessageNamed(const CConxString &s) const;
  ostream &printOn(ostream &o) const;
  void clear() { parts.clear(); }

private:
  CConxPrintableOwnerArray<CConxClsKeyedArg> parts;
}; // class CConxClsKeywordMessage

#endif // GPLCONX_STH_KMSG_CXX_H
