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
  C++ Smalltalkish RGB/HSV Color class.
*/

#ifndef GPLCONX_STCOLOR_CXX_H
#define GPLCONX_STCOLOR_CXX_H 1

#include "stobject.hh"
#include "color.hh"


//////////////////////////////////////////////////////////////////////////////
// Our Color class.
class CClsColor : VIRT public CClsBase {
public:
  CCONX_CLASSNAME("CClsColor")
  CLSNAME("Color", "I am a color, represented by an HSV triplet or an RGB triplet.")
  CLSTYPE(CClsBase, CLS_COLOR)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsColor)
  DEFAULT_ST_EQUALS(CClsBase, CClsColor)
public:
  CClsColor() { }
  CClsColor(const CConxColor &c) { color = c; }
  CClsColor(const CClsColor &o);
  ~CClsColor() { MMM("destructor"); }
  CClsColor &operator=(const CClsColor &o);
  CConxString printString() const;
  int operator==(const CClsColor &o) const
  {
    return (isClassInstance() == o.isClassInstance())
      && (isClassInstance() || getColor().equals(o.getColor()));
  }
  int operator!=(const CClsColor &o) const { return !operator==(o); }
  const CConxNamedColor &getColor() const { return color; }
  void setColor(const CConxColor &o) { color = o; }

private:
  void uninitializedCopy(const CClsColor &o) { color = o.color; }

protected:
  NEW_OI_ANSWERER(CClsColor);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsColor, ciAnswererRGB, ciActionRGB, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsColor, ciAnswererHSV, ciActionHSV, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsColor, oiAnswererRGB,
                                 oiActionRGB, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsColor, oiAnswererHSV,
                                 oiActionHSV, /* non-const */);
  ErrType ciMetaActionNewSet(CClsBase **result, CConxClsMessage &o,
                             CConxColor::ColorType cType) const;

#define CCLSCOLOR_GETTER(component) \
  ErrType oiAction ## component(CClsBase **result, \
                                CConxClsMessage &o) const \
  { \
    return ciMetaActionGet(result, o, CConxColor:: ## component); \
  } \
  ANSWERER(CClsColor, oiAnswerer ## component, oiAction ## component)

  CCLSCOLOR_GETTER(H);
  CCLSCOLOR_GETTER(S);
  CCLSCOLOR_GETTER(V);
  CCLSCOLOR_GETTER(R);
  CCLSCOLOR_GETTER(G);
  CCLSCOLOR_GETTER(B);
  ErrType ciMetaActionGet(CClsBase **result, CConxClsMessage &o,
                          CConxColor::Component comp) const;
private:
  static void initializeAnsweringMachines();


private: // attributes
  CConxNamedColor color;
  static Answerers *ansMachs;
}; // class CClsColor


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STCOLOR_CXX_H
