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
  C++ Smalltalkish CharacterArray class.
*/

#ifndef GPLCONX_STCHARAR_CXX_H
#define GPLCONX_STCHARAR_CXX_H 1

#include "stobject.hh"


//////////////////////////////////////////////////////////////////////////////
// Our String container is the base class for both the #Symbol
// and 'StringLiteral' classes.
class CClsCharacterArray : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsCharacterArray")
  CLSNAME("CharacterArray", "I hold characters.")
  CLSTYPE(CClsBase, CLS_CHARACTERARRAY)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsCharacterArray)
  DEFAULT_ST_EQUALS(CClsBase, CClsCharacterArray)
public:
  CClsCharacterArray() : string() { }
  CClsCharacterArray(const CConxString &s) : string(s) { }
  CClsCharacterArray(const CClsCharacterArray &o)
    : CClsBase(o), string(o.string) { }
  CClsCharacterArray &operator=(const CClsCharacterArray &o);

  virtual void setValue(const CConxString &str) { string = str; }
  const CConxString &getValue() const { return string; }
  int operator==(const CClsCharacterArray &o) const
  {
    return (isClassInstance() == o.isClassInstance())
      && (isClassInstance() || (string == o.string));
  }
  int operator!=(const CClsCharacterArray &o) const { return !operator==(o); }

protected:
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsCharacterArray, oiAnswererSet,
                                 oiActionSet, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsCharacterArray, oiAnswererLength,
                                 oiActionLength, const);
private:
  static void initializeAnsweringMachines();

private: // attributes
  CConxString string;
  static Answerers *ansMachs;
}; // class CClsCharacterArray


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STCHARAR_CXX_H
