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
  C++ Smalltalkish Drawable class.
*/

#ifndef GPLCONX_STDRAWBL_CXX_H
#define GPLCONX_STDRAWBL_CXX_H 1

#include "stobject.hh"
#include "stcolor.hh"
#include "stboole.hh"
#include "stnumber.hh"
#include "sterror.hh"
#include "stsymbol.hh"
#include "dgeomobj.hh"

// DLC change all ST_METHOD to ST_CMETHOD or even better allow `x: aNumberX y: aNumberY m: aModelIdentifier'
// DLC put the anonymous FTP space under CVS control or move the READMEs to
// the website.
// DLC move CArray.hh's classes into one class per header file
#define DRAWABLE_STR " that can draw itself on a Canvas"
//////////////////////////////////////////////////////////////////////////////
// Our CConxDwGeomObj wrapper.
class CClsDrawable : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsDrawable")
  CLSNAME("Drawable", "I am a thing that knows how to draw itself for your viewing enjoyment.")
  CLSTYPE(CClsBase, CLS_DRAWABLE)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE(CClsDrawable)
  DEFAULT_ST_EQUALS(CClsBase, CClsDrawable)
public:
  CClsDrawable();
  CClsDrawable(const CConxDwGeomObj &od);
  ~CClsDrawable() { MMM("destructor"); clear(); }
  CClsDrawable(const CClsDrawable &o);
  CClsDrawable &operator=(const CClsDrawable &o);
  CClsBase *stCloneDeep() const;
  void setValue(const CConxDwGeomObj &od);
  CConxDwGeomObj getValue() const;
  void affectValue(CConxDwGeomObj &r) const;
  CConxString printString() const;
  int operator==(const CClsDrawable &o) const;
  int operator!=(const CClsDrawable &o) const { return !operator==(o); }
  void clear();

  virtual const CConxArtist *getArtist() const;
  // We define this only so that Drawable is not an abstract class.  It could
  // be, but we'd like to test it alone from any of its derivations.

  void makeReadOnly();

protected:
  NEW_OI_ANSWERER(CClsDrawable);
  ANS_GETTER(CClsDrawable, Color, color);
  ANS_GETTER(CClsDrawable, DrawWithGarnish, withGarnish);
  ANS_GETTER(CClsDrawable, Thickness, thickness);
  ANS_GETTER(CClsDrawable, LongwayMethodTolerance, lwtol);
  ANS_GETTER(CClsDrawable, DrawingMethod, drawingMethod);
  ANS_SETTER(CClsDrawable, CClsColor, ColorColon, color);
  ANS_SETTER(CClsDrawable, CClsBoolean, DrawWithGarnishColon, withGarnish);
  ANS_SETTER(CClsDrawable, CClsNumber, ThicknessColon, thickness);
  ANS_SETTER(CClsDrawable, CClsNumber, LongwayMethodToleranceColon, lwtol);
  ANS_SETTER(CClsDrawable, CClsSymbol, DrawingMethodColon, drawingMethod);
private:
  static void initializeAnsweringMachines();

protected:
  Boole dependsOn(const CClsBase *p) const;

private: // operations
  void init();
  void uninitializedCopy(const CClsDrawable &o);


private: // attributes
  CClsColor *color;
  CClsBoolean *withGarnish;
  CClsSymbol *drawingMethod;
  CClsNumber *thickness, *lwtol;
  static Answerers *ansMachs;
}; // class CClsDrawable


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STDRAWBL_CXX_H
