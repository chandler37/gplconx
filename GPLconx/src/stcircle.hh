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
  C++ Smalltalkish Circle class.
*/

#ifndef GPLCONX_STCIRCLE_CXX_H
#define GPLCONX_STCIRCLE_CXX_H 1

#include "stdrawbl.hh"
#include "stpoint.hh"
#include "stnumber.hh"

//////////////////////////////////////////////////////////////////////////////
// Our Circle class.
class CClsCircle : VIRT public CClsDrawable {
  CCONX_CLASSNAME("CClsCircle")
  CLSNAME("Circle", "I am a circle in hyperbolic geometry.  I know how to draw myself.")
  CLSTYPE(CClsDrawable, CLS_CIRCLE)
  DEFAULT_SEND_MESSAGE(CClsDrawable)
  ANSMACH_ANSWERS(CClsDrawable)
  STCLONE(CClsCircle)
public:
  CClsCircle() { init(); ensureValidity(); }
  CClsCircle(CClsPoint *Center, CClsNumber *Radius);
  // DLC put   CClsCircle(const CConxDwGeomObj &od), both setValue()'s,
  // DLC getValue and getDwValue() in CClsDrawable using CConxSimpleArtist
  // DLC rather than CConxPoint, CConxCircle, etc.

  CClsCircle(const CClsDrawable &od)
    : CClsDrawable(od) { init(); ensureValidity(); }
  CClsCircle(const CConxDwGeomObj &od)
    : CClsDrawable(od) { init(); setValue(od); }
  CClsCircle(const CConxCircle &od) { init(); setValue(od); }
  ~CClsCircle() { MMM("destructor"); clear(); invalidateSavedArtist(); }
  CClsCircle(const CClsCircle &o);
  CClsCircle &operator=(const CClsCircle &o);
  CClsBase *stCloneDeep() const;

  CConxString printString() const;
  void setValue(const CConxCircle &o);
  void setValue(const CConxDwGeomObj &o);
  CConxCircle getValue() const throw(CClsError *);
  const CConxArtist *getArtist() const;
  CConxDwGeomObj getDwValue() const throw(CClsError *);
  int operator==(const CClsCircle &o);
  int operator!=(const CClsCircle &o) { return !operator==(o); }
  void clear();
  void makeReadOnly();

protected:
  void invalidateSavedArtist() const;

private:
  void ensureValidity();
  void init() { radius = NULL; center = NULL; savedArtist = NULL; }
  void uninitializedCopy(const CClsCircle &o);

protected:
  ANS_GETTER(CClsCircle, Center, center);
  ANS_GETTER(CClsCircle, Radius, radius);
  ANS_SETTER(CClsCircle, CClsPoint, CenterColon, center);
  ANS_SETTER(CClsCircle, CClsNumber, RadiusColon, radius);
  NEW_OI_ANSWERER(CClsCircle);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsCircle, ciAnswererCenterRadius,
                                 ciActionCenterRadius, const);
private:
  static void initializeAnsweringMachines();


private: // attributes
  CClsPoint *center;
  CClsNumber *radius;
  mutable CConxDwGeomObj *savedArtist;
  static Answerers *ansMachs;
}; // class CClsCircle


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STCIRCLE_CXX_H
