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
  C++ Smalltalkish SmallInt  class.
*/

#ifndef GPLCONX_STSMALLINT_CXX_H
#define GPLCONX_STSMALLINT_CXX_H 1

#include "stnumber.hh"


//////////////////////////////////////////////////////////////////////////////
// Our small integer class.
class CClsSmallInt : VIRT public CClsNumber {
  CCONX_CLASSNAME("CClsSmallInt")
  CLSNAME("SmallInt", "I am a small signed integer; I fit inside a C long.")
  CLSTYPE(CClsNumber, CLS_SMALLINT)
  DEFAULT_SEND_MESSAGE(CClsNumber)
  ANSMACH_ANSWERS(CClsNumber)
  STCLONE2(CClsSmallInt)
public:
  CClsSmallInt() { d = 37; }
  CClsSmallInt(long od) { setValue(od); }
  CClsSmallInt(const CClsSmallInt &o) : CClsNumber(o) { d = o.d; }
  CClsSmallInt &operator=(const CClsSmallInt &o);

  double getFloatValue() const { return (double) getValue(); }
  CConxString printString() const;
  void setValue(long od);
  long getValue() const;
  int operator==(const CClsSmallInt &o) { return isClassInstance() == o.isClassInstance() && d == o.d; }
  int operator!=(const CClsSmallInt &o) { return !operator==(o); }

protected:
  virtual ErrType asFloat(CClsBase **result);

protected:
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsSmallInt, oiAnswererBetweenAnd,
                                 oiActionBetweenAnd, const);
  NEW_OI_ANSWERER(CClsSmallInt);
private:
  static void initializeAnsweringMachines();

private: // attributes
  long d;
  static Answerers *ansMachs;
}; // class CClsSmallInt


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STSMALLINT_CXX_H
