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
  C++ Smalltalkish Hyperbola/Ellipse class.
*/

#ifndef GPLCONX_STHYPELL_CXX_H
#define GPLCONX_STHYPELL_CXX_H 1

#include "stdrawbl.hh"
#include "stpoint.hh"
#include "stnumber.hh"

//////////////////////////////////////////////////////////////////////////////
// Our Hyperbola/Ellipse class.
class CClsHypEllipse : VIRT public CClsDrawable {
  CCONX_CLASSNAME("CClsHypEllipse")
  CLSNAME("HypEllipse", "I am a hyperbola or an ellipse in hyperbolic geometry.  I know how to draw myself.")
  CLSTYPE(CClsDrawable, CLS_HYPELLIPSE)
  DEFAULT_SEND_MESSAGE(CClsDrawable)
  ANSMACH_ANSWERS(CClsDrawable)
  STCLONE(CClsHypEllipse)
  DEFAULT_ST_EQUALS(CClsDrawable, CClsHypEllipse)
public:
  CClsHypEllipse() { init(); ensureValidity(); }
  CClsHypEllipse(CClsPoint *Focus1, CClsPoint *Focus2, CClsNumber *Dist);
  // DLC put   CClsHypEllipse(const CConxDwGeomObj &od), both setValue()'s,
  // DLC getValue and getDwValue() in CClsDrawable using CConxSimpleArtist
  // DLC rather than CConxPoint, CConxHypEllipse, etc.

  CClsHypEllipse(const CClsDrawable &od);
  CClsHypEllipse(const CConxDwGeomObj &od);
  CClsHypEllipse(const CConxHypEllipse &od) { init(); setValue(od); }
  ~CClsHypEllipse() { MMM("destructor"); clear(); invalidateSavedArtist(); }
  CClsHypEllipse(const CClsHypEllipse &o);
  CClsHypEllipse &operator=(const CClsHypEllipse &o);
  CClsBase *stCloneDeep() const;

  CConxString printString() const;
  void setValue(const CConxHypEllipse &o);
  void setValue(const CConxDwGeomObj &o);
  CConxHypEllipse getValue() const throw(CClsError *);
  const CConxArtist *getArtist() const;
  CConxDwGeomObj getDwValue() const throw(CClsError *);
  int operator==(const CClsHypEllipse &o) const;
  int operator!=(const CClsHypEllipse &o) const { return !operator==(o); }
  void clear();
  void makeReadOnly();

protected:
  // We can make a Parabola more like a Circle with the following:
  size_t numMembers() const;
  CClsBase *getNthMember(size_t i);
  const CClsBase *getNthMember(size_t i) const;
  void setNthMemberPointer(size_t i, CClsBase *mm);

  void invalidateSavedArtist() const;

private:
  void ensureValidity();
  void init();
  void uninitializedCopy(const CClsHypEllipse &o);
  void incAllUserCounts();

protected:
  ANS_GETTER(CClsHypEllipse, Focus1, focus1);
  ANS_GETTER(CClsHypEllipse, Focus2, focus2);
  ANS_GETTER(CClsHypEllipse, K, conicDistance);
  ANS_SETTER(CClsHypEllipse, CClsPoint, Focus1Colon, focus1);
  ANS_SETTER(CClsHypEllipse, CClsPoint, Focus2Colon, focus2);
  ANS_SETTER(CClsHypEllipse, CClsNumber, KColon, conicDistance);
  NEW_OI_ANSWERER(CClsHypEllipse);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsHypEllipse, ciAnswererF1F2K,
                                 ciActionF1F2K, const);
private:
  static void initializeAnsweringMachines();

protected:
  Boole dependsOn(const CClsBase *p) const;


private: // attributes
  CClsPoint *focus1, *focus2;
  CClsNumber *conicDistance;
  mutable CConxDwGeomObj *savedArtist;
  static Answerers *ansMachs;
}; // class CClsHypEllipse


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STHYPELL_CXX_H
