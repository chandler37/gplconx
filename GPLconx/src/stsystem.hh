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
  C++ Smalltalkish System class.
*/

#ifndef GPLCONX_STSYSTEM_CXX_H
#define GPLCONX_STSYSTEM_CXX_H 1

#include "stobject.hh"


//////////////////////////////////////////////////////////////////////////////
// Our interface to things that we just shouldn't have access to, like exiting
// and reserved words and ... // DLC do reserved words.
class CConxClsManager;
class CClsSystem : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsSystem")
  CLSNAME("System", "I am the interface to things like exiting the program.")
  CLSTYPE(CClsBase, CLS_SYSTEM)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsSystem)
public:
  CClsSystem() { cm = NULL; }
  CClsSystem(CConxClsManager *CM) { cm = NULL; setClsMgr(CM); }
  CClsSystem(const CClsSystem &o) : CClsBase(o) { cm = o.cm; }
  CClsSystem &operator=(const CClsSystem &o);

  CConxString printString() const;
  int operator==(const CClsSystem &o) { return 0; }
  int operator!=(const CClsSystem &o) { return !operator==(o); }

  // DLC Nothing prevents us from setting a point's value to something out-of-range and then drawing it in the Klein disk.

  void setClsMgr(CConxClsManager *CM) { cm = CM; }

protected:
  CConxClsManager *getClsMgr() { return cm; }
  const CConxClsManager *getClsMgr() const { return cm; }

  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsSystem, iAnswererExit,
                                 iActionExit, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsSystem, iAnswererHelpMe,
                                 iActionHelpMe, /* non-const */);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsSystem, iAnswererAt, iActionAt,
                                 /* non-const*/);
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsSystem, iAnswererVars, iActionVars,
                                 /* non-const */);

private:
  static void initializeAnsweringMachines();

private: // attributes
  CConxClsManager *cm;
  static Answerers *ansMachs;
}; // class CClsSystem


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STSYSTEM_CXX_H
