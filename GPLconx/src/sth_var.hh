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
  C++ Smalltalkish back-end CConxClsVariable class.
*/

#ifndef GPLCONX_STH_VAR_CXX_H
#define GPLCONX_STH_VAR_CXX_H 1

#include "CString.hh"


class CClsBase;
//////////////////////////////////////////////////////////////////////////////
// A class that represents a Smalltalk variable.  It has a name and it points
// to a CClsBase instance, the ``pointee''
class CConxClsVariable : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsVariable")
public:
  CConxClsVariable() : varName() { pointee = NULL; }
  CConxClsVariable(const CConxString &s) : varName(s) { pointee = NULL; }
  CConxClsVariable(const CConxString &s, CClsBase *p);
  CConxClsVariable(const CConxClsVariable &v);
  CConxClsVariable &operator=(const CConxClsVariable &v);

  // We point to it, we don't own it.  DLC decrement user's count here for GC?
  ~CConxClsVariable() { }
  
  void setVariableName(const CConxString &s) { varName = s; }
  const CConxString &getVariableName() const { return varName; }

  void setPointee(CClsBase *p) { pointee = p; }
  CClsBase *getPointee() const { return pointee; }

  int operator==(const CConxClsVariable &o) const;
  int operator!=(const CConxClsVariable &o) const { return !operator==(o); }

  ostream &printOn(ostream &o) const;

private: // attributes
  CConxString varName;
  CClsBase *pointee;
}; // class CConxClsVariable

OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxClsVariable);

//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STH_VAR_CXX_H
