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
  C++ Smalltalkish Boolean (true/false) class.
*/

#ifndef GPLCONX_STBOOLEAN_CXX_H
#define GPLCONX_STBOOLEAN_CXX_H 1

#include "stobject.hh"


//////////////////////////////////////////////////////////////////////////////
// Our equivalent to Smalltalk's Boolean class.  Don't expect all the usual
// methods, though.
class CClsBoolean : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsBoolean")
  CLSNAME("Boolean", "I am a Boolean value, i.e. I am either true or false.")
  CLSTYPE(CClsBase, CLS_BOOLEAN)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsBoolean)
  DEFAULT_ST_EQUALS(CClsBase, CClsBoolean)
public:
  CClsBoolean() { d = TRUE; }
  CClsBoolean(Boole od) { setValue(od); }
  CClsBoolean(const CClsBoolean &o) : CClsBase(o) { d = o.d; }
  CClsBoolean &operator=(const CClsBoolean &o);

  CConxString printString() const;
  void setValue(Boole od) { d = od; }
  Boole getValue() const { return d; }
  // DLC operator== should be true only for the same instance type {class, object} fix this for all ClsCls
  int operator==(const CClsBoolean &o) const;
  int operator!=(const CClsBoolean &o) const { return !operator==(o); }

protected:
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsBoolean, ciAnswererTrue,
                                 ciActionTrue, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsBoolean, ciAnswererFalse,
                                 ciActionFalse, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsBoolean, oiAnswererNot,
                                 oiActionNot, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsBoolean, oiAnswererAnd,
                                 oiActionAnd, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsBoolean, oiAnswererOr,
                                 oiActionOr, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsBoolean, oiAnswererXOr,
                                 oiActionXOr, const);
private:
  static void initializeAnsweringMachines();

private: // attributes
  Boole d;
  static Answerers *ansMachs;
}; // class CClsBoolean


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STBOOLEAN_CXX_H
