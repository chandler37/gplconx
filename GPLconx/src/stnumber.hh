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
  C++ Smalltalkish Number class.
*/

#ifndef GPLCONX_STNUMBER_CXX_H
#define GPLCONX_STNUMBER_CXX_H 1

#include "stobject.hh"

class CClsFloat;
//////////////////////////////////////////////////////////////////////////////
// Our equivalent to Smalltalk's Number class.
class CClsNumber : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsNumber")
  CLSNAME("Number", "I am a number.")
  CLSTYPE(CClsBase, CLS_NUMBER)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsNumber)
public:
  CClsNumber() { }
  CClsNumber(double od) { }
  CClsNumber(const CClsNumber &o) : CClsBase(o) { }
  CClsNumber &operator=(const CClsNumber &o);

  CConxString printString() const;
  int operator==(const CClsNumber &o) { return 0; }
  int operator!=(const CClsNumber &o) { return !operator==(o); }
  
  virtual double getFloatValue() const;
  virtual ErrType asFloat(CClsBase **result);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsNumber, oiAnswererAsFloat,
                                 oiActionAsFloat, /* non-const, could modify
                                                     if we're already a
                                                     CClsFloat */);

private:
  static void initializeAnsweringMachines();

private: // attributes
  static Answerers *ansMachs;
}; // class CClsNumber


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STNUMBER_CXX_H
