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
  //  equals() in Number will do very nicely.
public:
  CClsFloat() { d = 37.0; }
  CClsFloat(double od) { setValue(od); }
  CClsFloat(const CClsFloat &o) : CClsNumber(o) { d = o.d; }
  CClsFloat &operator=(const CClsFloat &o);

  double getFloatValue() const { return getValue(); }
  CConxString printString() const;
  void setValue(double od);
  double getValue() const;
  int operator==(const CClsFloat &o) const
  {
    return isClassInstance() == o.isClassInstance() && d == o.d;
  }
  int operator!=(const CClsFloat &o) const { return !operator==(o); }

  static double getRandom(double low, double high);

protected:
  virtual ErrType asFloat(CClsBase **result);

protected:
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererSet,
                                 oiActionSet, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererBetweenAnd,
                                 oiActionBetweenAnd, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, ciAnswererRandomBetweenAnd,
                                 ciActionRandomBetweenAnd, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererPlus,
                                 oiActionPlus, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererMinus,
                                 oiActionMinus, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererNegation,
                                 oiActionNegation, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererReciprocal,
                                 oiActionReciprocal, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererSine,
                                 oiActionSine, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererCosine,
                                 oiActionCosine, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererTangent,
                                 oiActionTangent, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererTimes,
                                 oiActionTimes, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererDividedBy,
                                 oiActionDividedBy, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, ciAnswererPi,
                                 ciActionPi, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, ciAnswererE,
                                 ciActionE, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererExp,
                                 oiActionExp, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererLog10,
                                 oiActionLog10, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererLn,
                                 oiActionLn, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererRaisedTo,
                                 oiActionRaisedTo, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererGT,
                                 oiActionGT, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererLT,
                                 oiActionLT, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererGTE,
                                 oiActionGTE, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererLTE,
                                 oiActionLTE, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsFloat, oiAnswererSqrt,
                                 oiActionSqrt, const);
  // DLC put these in number!

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
