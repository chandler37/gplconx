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
  C++ Smalltalkish Parabola class.
*/

#ifndef GPLCONX_STPARABO_CXX_H
#define GPLCONX_STPARABO_CXX_H 1

#include "stdrawbl.hh"
#include "stpoint.hh"
#include "stline.hh"

//////////////////////////////////////////////////////////////////////////////
// Our Parabola class.
class CClsParabola : VIRT public CClsDrawable {
  CCONX_CLASSNAME("CClsParabola")
  CLSNAME("Parabola", "I am a parabola in hyperbolic geometry.  I know how to draw myself.")
  CLSTYPE(CClsDrawable, CLS_PARABOLA)
  DEFAULT_SEND_MESSAGE(CClsDrawable)
  ANSMACH_ANSWERS(CClsDrawable)
  STCLONE(CClsParabola)
public:
  CClsParabola() { init(); ensureValidity(); }
  CClsParabola(CClsPoint *Focus, CClsLine *Line);
  // DLC put   CClsParabola(const CConxDwGeomObj &od), both setValue()'s,
  // DLC getValue and getDwValue() in CClsDrawable using CConxSimpleArtist
  // DLC rather than CConxPoint, CConxParabola, etc.

  CClsParabola(const CClsDrawable &od);
  CClsParabola(const CConxDwGeomObj &od);
  CClsParabola(const CConxParabola &od) { init(); setValue(od); }
  ~CClsParabola() { MMM("destructor"); clear(); invalidateSavedArtist(); }
  CClsParabola(const CClsParabola &o);
  CClsParabola &operator=(const CClsParabola &o);
  CClsBase *stCloneDeep() const;

  CConxString printString() const;
  void setValue(const CConxParabola &o);
  void setValue(const CConxDwGeomObj &o);
  CConxParabola getValue() const throw(CClsError *);
  const CConxArtist *getArtist() const;
  CConxDwGeomObj getDwValue() const throw(CClsError *);
  int operator==(const CClsParabola &o);
  int operator!=(const CClsParabola &o) { return !operator==(o); }
  void clear();
  void makeReadOnly();

protected:
  void invalidateSavedArtist() const;

private:
  void ensureValidity();
  void init() { line = NULL; focus = NULL; savedArtist = NULL; }
  void uninitializedCopy(const CClsParabola &o);

protected:
  ANS_GETTER(CClsParabola, Focus, focus);
  ANS_GETTER(CClsParabola, Line, line);
  ANS_SETTER(CClsParabola, CClsPoint, FocusColon, focus);
  ANS_SETTER(CClsParabola, CClsLine, LineColon, line);
  NEW_OI_ANSWERER(CClsParabola);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsParabola, ciAnswererFocusLine,
                                 ciActionFocusLine, const);
private:
  static void initializeAnsweringMachines();


private: // attributes
  CClsPoint *focus;
  CClsLine *line;
  mutable CConxDwGeomObj *savedArtist;
  static Answerers *ansMachs;
}; // class CClsParabola


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STPARABO_CXX_H
