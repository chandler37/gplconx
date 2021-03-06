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
  C++ Smalltalkish Symbol (#id323ae) class.
*/

#ifndef GPLCONX_STSYMBOL_CXX_H
#define GPLCONX_STSYMBOL_CXX_H 1

#include "stcharar.hh"

// DLC test all stclasses with a message that they don't respond to just
// to be sure that we ask the superclass to respond.

//////////////////////////////////////////////////////////////////////////////
// Our Symbol class.
// `#uhp'
// `Point new x: 0.3 y: 0.2 model: #klein'
class CClsSymbol : VIRT public CClsCharacterArray {
  CCONX_CLASSNAME("CClsSymbol")
  CLSNAME("Symbol", "I am a special kind of string of characters that starts with an alphabetic character or colon and contains only alphanumeric characters and colons")
  CLSTYPE(CClsCharacterArray, CLS_SYMBOL)
  DEFAULT_SEND_MESSAGE(CClsCharacterArray)
  ANSMACH_ANSWERS(CClsCharacterArray)
  STCLONE2(CClsSymbol)
  DEFAULT_ST_EQUALS(CClsCharacterArray, CClsSymbol)
public:
  CClsSymbol() { }
  CClsSymbol(const CConxString &s) : CClsCharacterArray(s) { }
  CClsSymbol(const CClsSymbol &o) : CClsCharacterArray(o) { }
  CClsSymbol &operator=(const CClsSymbol &o);

  // use CClsBase::printOn
  CConxString printString() const;
  size_t numberOfKeywords() const { return getValue().numberOf(':'); }

  // For `#a:b:', nthKeyword(0) returns "a" and nthKeyword(1) returns "b"
  CConxString nthKeyword(size_t n) const throw(const char *);

  int operator==(const CClsSymbol &o) const {
    return CClsCharacterArray::operator==(o);
  }
  int operator!=(const CClsSymbol &o) const { return !operator==(o); }

protected:
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsSymbol, oiAnswererNumberKeywords,
                                 oiActionNumberKeywords, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsSymbol, oiAnswererNthKeyword,
                                 oiActionNthKeyword, const);
private:
  static void initializeAnsweringMachines();

private:
  static Answerers *ansMachs;
}; // class CClsSymbol


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STSYMBOL_CXX_H
