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
  C++ Smalltalkish Point class.
*/

#ifndef GPLCONX_STPOINT_CXX_H
#define GPLCONX_STPOINT_CXX_H 1

#include "stdrawbl.hh"
#include "stmodlid.hh"

//////////////////////////////////////////////////////////////////////////////
// Our Point class.
// `P := Point x: 3.0 y: 0.2 model: (M := #uhp)'
// `P x: 4.0 model: #uhp'
// `P printString'
class CClsPoint : VIRT public CClsDrawable {
  CCONX_CLASSNAME("CClsPoint")
  CLSNAME("Point", "I am a point in hyperbolic geometry.  I know how to draw myself.")
  CLSTYPE(CClsDrawable, CLS_POINT)
  DEFAULT_SEND_MESSAGE(CClsDrawable)
  ANSMACH_ANSWERS(CClsDrawable)
  STCLONE(CClsPoint)
  DEFAULT_ST_EQUALS(CClsDrawable, CClsPoint)
public:
  CClsPoint() { init(); ensureValidity(); }
  CClsPoint(CClsNumber *x, CClsNumber *y, CClsModelIdentifier *m);
  CClsPoint(const CClsDrawable &od)
    : CClsDrawable(od) { init(); ensureValidity(); }
  CClsPoint(const CConxDwGeomObj &od)
    : CClsDrawable(od) { init(); setValue(od); }
  CClsPoint(const CConxPoint &od, ConxModlType m = CONX_POINCARE_UHP);

  CClsPoint(double euclideanRadius);
  /*
     This constructs a new point randomly distributed in the Klein disk's
     Euclidean counterpart, the unit circle, with Euclidean distance from
     (0, 0) less than euclideanRadius (maximum of 1.0 since the unit disk
     is at infinity) -- this has a random Euclidean radius less than or
     equal to euclideanRadius, but the hyperbolic distance from hyperbolic
     (0, 0) is more likely to be small, i.e. it is not a random hyperbolic
     distance from (0, 0), because that would skew the points toward the
     edge of the disk, which is not a great idea for visualization.
   */

  ~CClsPoint() { MMM("destructor"); clear(); invalidateSavedArtist(); }
  CClsPoint(const CClsPoint &o);
  CClsPoint &operator=(const CClsPoint &o);
  CClsBase *stCloneDeep() const;
  CConxString printString() const;
  void setValue(const CConxPoint &o, ConxModlType m = CONX_POINCARE_UHP);
  void setValue(const CConxDwGeomObj &o, ConxModlType m = CONX_POINCARE_UHP);
  CConxPoint getValue() const throw(CClsError *);
  void setReadOnly(Boole readOnly);
  const CConxArtist *getArtist() const;
  CConxDwGeomObj getDwValue() const throw(CClsError *);
  int operator==(const CClsPoint &o) const;
  int operator!=(const CClsPoint &o) const { return !operator==(o); }
  void clear();
  void makeReadOnly();


protected:
  void invalidateSavedArtist() const;

private:
  void ensureValidity();
  void init() { xCoord = yCoord = NULL; model = NULL; savedArtist = NULL; }
  void uninitializedCopy(const CClsPoint &o);

protected:
  NEW_OI_ANSWERER(CClsPoint);
  ANS_GETTER(CClsPoint, X, xCoord);
  ANS_GETTER(CClsPoint, Y, yCoord);
  ANS_GETTER(CClsPoint, Model, model);
  ANS_SETTER(CClsPoint, CClsNumber, XColon, xCoord);
  ANS_SETTER(CClsPoint, CClsNumber, YColon, yCoord);
  ANS_SETTER(CClsPoint, CClsModelIdentifier, ModelColon, model);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsPoint, ciAnswererXYModel,
                                 ciActionXYModel, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsPoint, ciAnswererRandom1,
                                 ciActionRandom1, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsPoint, ciAnswererRandom,
                                 ciActionRandom, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsPoint, oiAnswererDistanceFromPoint,
                                 oiActionDistanceFromPoint, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsPoint, oiAnswererBetweenAnd,
                                 oiActionBetweenAnd, const);
private:
  static void initializeAnsweringMachines();

protected:
  Boole dependsOn(const CClsBase *p) const;


private: // attributes
  CClsNumber *xCoord, *yCoord;
  CClsModelIdentifier *model;
  mutable CConxDwGeomObj *savedArtist;
  static Answerers *ansMachs;
}; // class CClsPoint


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STPOINT_CXX_H
