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
  C++ Smalltalkish back-end CConxClsKeyedArg class.
*/

#ifndef GPLCONX_STH_KARG_CXX_H
#define GPLCONX_STH_KARG_CXX_H 1

#include "CString.hh"

class CClsBase;
//////////////////////////////////////////////////////////////////////////////
// A class that represents a keyword and its accompanying argument.
// This does not own the argument, so copies will refer to the same exact
// place in memory.  (GC will take care of the argument's ownership.)
class CConxClsKeyedArg : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsKeyedArg")
  DEFAULT_PRINTON()
public:
  CConxClsKeyedArg() { }
  CConxClsKeyedArg(const CConxString &key, CClsBase *arg);
  CConxClsKeyedArg(const CConxClsKeyedArg &o);
  CConxClsKeyedArg &operator=(const CConxClsKeyedArg &o);
  ~CConxClsKeyedArg();
  // DLC operator== and !=

  int operator==(const CConxClsKeyedArg &o) const;
  int operator!=(const CConxClsKeyedArg &o) const { return !operator==(o); }
  
  const CConxString &getKeyword() const { return keyword; }
  void setKeyword(const CConxString &s) throw(const char *);
  CClsBase *getArg() { return obj; }
  void setArg(CClsBase *s);

protected:
  void uninitializedCopy(const CConxClsKeyedArg &o);

private:
  CConxString keyword;
  CClsBase *obj;
}; // class CConxClsKeyedArg


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxClsKeyedArg);

#endif // GPLCONX_STH_KARG_CXX_H
