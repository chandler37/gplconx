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
  C++ Smalltalkish Line class.
*/

#ifndef GPLCONX_STLINE_CXX_H
#define GPLCONX_STLINE_CXX_H 1

#include "stdrawbl.hh"
#include "stpoint.hh"

//////////////////////////////////////////////////////////////////////////////
// Our Line class.
class CClsLine : VIRT public CClsDrawable {
  CCONX_CLASSNAME("CClsLine")
  CLSNAME("Line", "I am a line in hyperbolic geometry.  I know how to draw myself.")
  CLSTYPE(CClsDrawable, CLS_LINE)
  DEFAULT_SEND_MESSAGE(CClsDrawable)
  ANSMACH_ANSWERS(CClsDrawable)
  STCLONE(CClsLine)
public:
  CClsLine() { init(); ensureValidity(); }
  CClsLine(CClsPoint *A, CClsPoint *B, CClsBoolean *seg);
  CClsLine(const CClsDrawable &od)
    : CClsDrawable(od) { init(); ensureValidity(); }
  CClsLine(const CConxDwGeomObj &od)
    : CClsDrawable(od) { init(); setValue(od); }
  CClsLine(const CConxLine &od) { init(); setValue(od); }
  ~CClsLine() { MMM("destructor"); clear(); invalidateSavedArtist(); }
  CClsLine(const CClsLine &o);
  CClsLine &operator=(const CClsLine &o);
  CClsBase *stCloneDeep() const;
  
  CConxString printString() const;
  void setValue(const CConxLine &o);
  void setValue(const CConxDwGeomObj &o)
  {
    CClsDrawable::setValue(o);
    const CConxSimpleArtist *oPt = o.getGeomObj();
    if (oPt == NULL || oPt->getSAType() != CConxSimpleArtist::SA_LINE)
      throw "That's just nasty.";
    setValue(*oPt);
  }
  CConxLine getValue() const throw(CClsError *);
  const CConxArtist *getArtist() const;
  CConxDwGeomObj getDwValue() const throw(CClsError *);
  int operator==(const CClsLine &o);
  int operator!=(const CClsLine &o) { return !operator==(o); }
  void clear();
  void makeReadOnly();
  
protected:
  void invalidateSavedArtist() const;
  
private:
  void ensureValidity();
  void init() { P[0] = P[1] = NULL; isSeg = NULL; savedArtist = NULL; }
  void uninitializedCopy(const CClsLine &o);
  
protected:
  ANS_GETTER(CClsLine, A, P[0]);
  ANS_GETTER(CClsLine, B, P[1]);
  ANS_GETTER(CClsLine, IsSegment, isSeg);
  ANS_SETTER(CClsLine, CClsPoint, AColon, P[0]);
  ANS_SETTER(CClsLine, CClsPoint, BColon, P[1]);
  ANS_SETTER(CClsLine, CClsBoolean, IsSegmentColon, isSeg);
  NEW_OI_ANSWERER(CClsLine);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsLine, ciAnswererABIsSegment,
                                 ciActionABIsSegment, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsLine, ciAnswererRandom,
                                 ciActionRandom, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsLine, ciAnswererRandom1,
                                 ciActionRandom1, const);
private:
  static void initializeAnsweringMachines();
  

private: // attributes
  CClsPoint *P[2];
  CClsBoolean *isSeg;
  mutable CConxDwGeomObj *savedArtist;
  static Answerers *ansMachs;
}; // class CClsLine


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STLINE_CXX_H
