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
  C++ Smalltalkish String class.
*/

#ifndef GPLCONX_STSTRING_CXX_H
#define GPLCONX_STSTRING_CXX_H 1

#include "stcharar.hh"


//////////////////////////////////////////////////////////////////////////////
// Our String class.
// `'hi there''
// `Point new x: 'this is an illegal argument' y: 0.2'
class CClsStringLiteral : VIRT public CClsCharacterArray {
  CCONX_CLASSNAME("CClsStringLiteral")
  CLSNAME("String", "I am a string of characters.")
  CLSTYPE(CClsCharacterArray, CLS_STRING)
  STCLONE2(CClsStringLiteral)
public:
  CClsStringLiteral() { }
  CClsStringLiteral(const CConxString &s) : CClsCharacterArray(s) { }
  CClsStringLiteral(const CClsStringLiteral &o) : CClsCharacterArray(o) { }
  CClsStringLiteral &operator=(const CClsStringLiteral &o);
  // test that CClsCharacterArray::sendMessage() is called.
  CConxString printString() const;

  int operator==(const CClsStringLiteral &o)
  {
    return CClsCharacterArray::operator==(o);
  }
  int operator!=(const CClsStringLiteral &o) { return !operator==(o); }
}; // class CClsStringLiteral


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STSTRING_CXX_H
