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
  C++ Smalltalkish Equidistant curve class.
*/

#ifndef GPLCONX_STEQDIST_CXX_H
#define GPLCONX_STEQDIST_CXX_H 1

#include "stdrawbl.hh"
#include "stline.hh"
#include "stnumber.hh"

//////////////////////////////////////////////////////////////////////////////
// Our equidistance curve class
class CClsEqDistCurve : VIRT public CClsDrawable {
  CCONX_CLASSNAME("CClsEqDistCurve")
  CLSNAME("EqDistCurve", "I am a curve that is a set distance from a line in hyperbolic geometry.  I know how to draw myself.")
  CLSTYPE(CClsDrawable, CLS_EQDISTCURVE)
  DEFAULT_SEND_MESSAGE(CClsDrawable)
  ANSMACH_ANSWERS(CClsDrawable)
  STCLONE(CClsEqDistCurve)
  DEFAULT_ST_EQUALS(CClsDrawable, CClsEqDistCurve)
public:
  CClsEqDistCurve() { init(); ensureValidity(); }
  CClsEqDistCurve(CClsLine *l, CClsNumber *Dist);
  // DLC put   CClsEqDistCurve(const CConxDwGeomObj &od), both setValue()'s,
  // DLC getValue and getDwValue() in CClsDrawable using CConxSimpleArtist
  // DLC rather than CConxPoint, CConxEqDistCurve, etc.

  CClsEqDistCurve(const CClsDrawable &od);
  CClsEqDistCurve(const CConxDwGeomObj &od);
  CClsEqDistCurve(const CConxEqDistCurve &od) { init(); setValue(od); }
  ~CClsEqDistCurve() { MMM("destructor"); clear(); invalidateSavedArtist(); }
  CClsEqDistCurve(const CClsEqDistCurve &o);
  CClsEqDistCurve &operator=(const CClsEqDistCurve &o);
  CClsBase *stCloneDeep() const;

  CConxString printString() const;
  void setValue(const CConxEqDistCurve &o);
  void setValue(const CConxDwGeomObj &o);
  CConxEqDistCurve getValue() const throw(CClsError *);
  const CConxArtist *getArtist() const;
  CConxDwGeomObj getDwValue() const throw(CClsError *);
  int operator==(const CClsEqDistCurve &o) const;
  int operator!=(const CClsEqDistCurve &o) const { return !operator==(o); }
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
  void uninitializedCopy(const CClsEqDistCurve &o);
  void incAllUserCounts();

protected:
  ANS_GETTER(CClsEqDistCurve, Line, line);
  ANS_GETTER(CClsEqDistCurve, K, conicDistance);
  ANS_SETTER(CClsEqDistCurve, CClsLine, LineColon, line);
  ANS_SETTER(CClsEqDistCurve, CClsNumber, KColon, conicDistance);
  NEW_OI_ANSWERER(CClsEqDistCurve);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsEqDistCurve, ciAnswererLineK,
                                 ciActionLineK, const);
private:
  static void initializeAnsweringMachines();

protected:
  Boole dependsOn(const CClsBase *p) const;


private: // attributes
  CClsLine *line;
  CClsNumber *conicDistance;
  mutable CConxDwGeomObj *savedArtist;
  static Answerers *ansMachs;
}; // class CClsEqDistCurve


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STEQDIST_CXX_H
