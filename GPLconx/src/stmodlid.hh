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
  C++ Smalltalkish Hyperbolic Geometry ModelIdentifier class.
*/

#ifndef GPLCONX_STMODLID_CXX_H
#define GPLCONX_STMODLID_CXX_H 1

#include "stcharar.hh"


// DLC We want to be able to do `pdc set: kdc' without the associated CConxCanvas changing.

//////////////////////////////////////////////////////////////////////////////
// Our ModelIdentifier class, so that 'Point x: 0.3 y: 0.3 model: #klein'
// will work.  We allow #klein, #Klein, #KLEIN, #k, #K, #kl, etc. to mean
// the Beltrami-klein disk.
class CClsModelIdentifier : VIRT public CClsCharacterArray {
  CCONX_CLASSNAME("CClsModelIdentifier")
  CLSNAME("ModelIdentifier", "I distinguish between three models of hyperbolic geometry, the Poincare Disk, the Beltrami-Klein Disk, and the Poincare Upper Half Plane.")
  CLSTYPE(CClsCharacterArray, CLS_MODELIDENTIFIER)
  DEFAULT_SEND_MESSAGE(CClsCharacterArray)
  ANSMACH_ANSWERS(CClsCharacterArray)
  STCLONE2(CClsModelIdentifier)
public:
  enum Model { KD, UHP, PD, NONE, NIL };
  // NIL when we haven't calculated it yet, #hello is NONE, #klein is KD, etc.

  CClsModelIdentifier() { mdl = NIL; }
  CClsModelIdentifier(ConxModlType m);
  CClsModelIdentifier(const CConxString &s)
    : CClsCharacterArray(s) { mdl = NIL; }
  CClsModelIdentifier(const CClsModelIdentifier &o)
    : CClsCharacterArray(o) { mdl = o.mdl; }
  CClsModelIdentifier &operator=(const CClsModelIdentifier &o);
  CConxString printString() const;
  CConxString printShortString() const;
  int operator==(const CClsModelIdentifier &o) const {
    return model() == o.model();
  }
  int operator!=(const CClsModelIdentifier &o) const { return !operator==(o); }
  void setValue(const CConxString &str);
  ConxModlType cModel() const throw(int);
  Model model() const;
  // protected: CClsCharacterArray::setValue; DLC if paranoia strikes...


protected:
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsModelIdentifier, ciAnswererNew,
                                 ciActionNew, const);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsModelIdentifier, ciAnswererSet,
                                 ciActionSet, /* non-const */);
private:
  static void initializeAnsweringMachines();

private:
  mutable Model mdl;
  static Answerers *ansMachs;
}; // class CClsModelIdentifier


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STMODLID_CXX_H
