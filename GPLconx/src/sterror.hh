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
  C++ Smalltalkish Error class.
*/

#ifndef GPLCONX_STERROR_CXX_H
#define GPLCONX_STERROR_CXX_H 1

#include "stobject.hh"


//////////////////////////////////////////////////////////////////////////////
// CClsError represents an error, such as the result of a bad message.
class CClsError : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsError")
  CLSNAME("ParseError",
          "I am an error, like those you get when you type something in wrong.")
  CLSTYPE(CClsBase, CLS_ERROR)
  STCLONE2(CClsError)
  DEFAULT_ST_EQUALS(CClsBase, CClsError)
public:
  CClsError() : errstr("unspecified error") { }
  CClsError(const CConxString &s);
  CClsError(const CClsError &o) : CClsBase(o), errstr(o.errstr) { }
  CClsError &operator=(const CClsError &o);

  CConxString printString() const;
  void setValue(const CConxString &s) { errstr = s; }
  const CConxString &getValue() const { return errstr; }
  int operator==(const CClsError &o) const
  {
    return (isClassInstance() == o.isClassInstance())
      && (isClassInstance() || (errstr == o.errstr));
  }
  int operator!=(const CClsError &o) const { return !operator==(o); }

private: // attributes
  CConxString errstr;
}; // class CClsError


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STERROR_CXX_H
