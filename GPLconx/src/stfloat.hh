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
  C++ Smalltalkish Float class.
*/

#ifndef GPLCONX_STFLOAT_CXX_H
#define GPLCONX_STFLOAT_CXX_H 1

#include "stnumber.hh"


//////////////////////////////////////////////////////////////////////////////
// Our equivalent to Smalltalk's Float class.  Don't expect all the usual
// methods, though.
class CClsFloat : VIRT public CClsNumber {
  CCONX_CLASSNAME("CClsFloat")
  CLSNAME("Float", "I am a floating-point number.")
  CLSTYPE(CClsNumber, CLS_FLOAT)
  DEFAULT_SEND_MESSAGE(CClsNumber)
  ANSMACH_ANSWERS(CClsNumber)
  STCLONE2(CClsFloat)
public:
  CClsFloat() { d = 37.0; }
  CClsFloat(double od) { setValue(od); }
  CClsFloat(const CClsFloat &o) : CClsNumber(o) { d = o.d; }
  CClsFloat &operator=(const CClsFloat &o);

  double getFloatValue() const { return getValue(); }
  CConxString printString() const;
  void setValue(double od);
  double getValue() const;
  int operator==(const CClsFloat &o) { return isClassInstance() == o.isClassInstance() && d == o.d; }
  int operator!=(const CClsFloat &o) { return !operator==(o); }

  static double getRandom(double low, double high);

protected:
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererSet,
                                 oiActionSet, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererBetweenAnd,
                                 oiActionBetweenAnd, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, ciAnswererRandomBetweenAnd,
                                 ciActionRandomBetweenAnd, const);
private:
  static void initializeAnsweringMachines();

private: // operations
  static Boole randomIsSeeded() { return rngIsSeeded; }
  static void seedRNG();

private: // attributes
  double d;
  static Boole rngIsSeeded;
  static Answerers *ansMachs;
}; // class CClsFloat


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STFLOAT_CXX_H
