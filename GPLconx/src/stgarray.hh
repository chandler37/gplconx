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
  C++ Smalltalkish growable Array class.
*/

#ifndef GPLCONX_STGARRAY_CXX_H
#define GPLCONX_STGARRAY_CXX_H 1

#include "stobject.hh"


//////////////////////////////////////////////////////////////////////////////
// Our Array.
class CClsArray : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsArray")
  CLSNAME("Array", "I am an array that grows and holds objects of any type.")
  CLSTYPE(CClsBase, CLS_ARRAY)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE(CClsArray)
  DEFAULT_ST_EQUALS(CClsBase, CClsArray)
public:
  CClsArray() : contents() { init(); }
  CClsArray(const CClsArray &o);
  CClsArray &operator=(const CClsArray &o);
  CClsBase *stCloneDeep() const;

protected:
  void incrementAllUsersCounts();
protected: // because checking for NULL pointers would be required.
  CClsArray(CClsBase *o1) : contents(o1) { init(); }
  CClsArray(CClsBase *o1, CClsBase *o2) : contents(o1, o2) { init(); }
  CClsArray(CClsBase *o1, CClsBase *o2, CClsBase *o3)
    : contents(o1, o2, o3) { init(); }
  CClsArray(CClsBase *o1, CClsBase *o2, CClsBase *o3, CClsBase *o4)
    : contents(o1, o2, o3, o4) { init(); }
  CClsArray(const CConxSimpleArray<CClsBase *> &s) : contents(s) { init(); }

public:
  ~CClsArray() { clear(); }

  int operator==(const CClsArray &o) const
  {
    return (isClassInstance() == o.isClassInstance())
      && (isClassInstance()
          || (contents == o.contents && requiredType == o.requiredType));
  }
  int operator!=(const CClsArray &o) const { return !operator==(o); }

  size_t numElements() const { return contents.size(); }
  CConxString printString() const;
  void append(CClsBase *n);
  CClsBase *get(size_t n) const throw(const char *) { return getSimpleArray().get(n); }
  

protected:
  void setRequiredType(ClsType t) { requiredType = t; }
  ClsType getRequiredType() { return requiredType; }

  CConxSimpleArray<CClsBase *> &getSimpleArray() { return contents; }
  const CConxSimpleArray<CClsBase *> &getSimpleArray() const { return contents; }

private:
  void clear();
  void init();

public:
  void makeReadOnly();
protected:
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, oiAnswererAt,
                                 oiActionAt, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, oiAnswererYourself,
                                 oiActionYourself, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, oiAnswererAtPut,
                                 oiActionAtPut, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, oiAnswererAddFirst,
                                 oiActionAddFirst, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, oiAnswererAddLast,
                                 oiActionAddLast, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, oiAnswererSize,
                                 oiActionSize, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, ciAnswererWithWithWithWith,
                                 ciActionWithWithWithWith, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, ciAnswererWithWithWith,
                                 ciActionWithWithWith, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, ciAnswererWithWith,
                                 ciActionWithWith, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsArray, ciAnswererWith,
                                 ciActionWith, const);
  NEW_OI_ANSWERER(CClsArray);
private:
  static void initializeAnsweringMachines();

protected:
  Boole dependsOn(const CClsBase *p) const;

private: // attributes
  ClsType requiredType;
  CConxSimpleArray<CClsBase *> contents;
  static Answerers *ansMachs;
}; // class CClsArray


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STGARRAY_CXX_H
