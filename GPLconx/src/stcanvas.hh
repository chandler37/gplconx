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
  C++ Smalltalkish Canvas class.
*/

#ifndef GPLCONX_STCANVAS_CXX_H
#define GPLCONX_STCANVAS_CXX_H 1

#include "stgarray.hh"
#include "canvas.hh"

//////////////////////////////////////////////////////////////////////////////
// Our CConxCanvas wrapper
class CClsCanvas : VIRT public CClsArray {
  CCONX_CLASSNAME("CClsCanvas")
  CLSNAME("Canvas", "I am a canvas upon which you can draw Drawable object instances.  I hold onto my objects like an Array.")
  CLSTYPE(CClsArray, CLS_CANVAS)
  DEFAULT_SEND_MESSAGE(CClsArray)
  ANSMACH_ANSWERS(CClsArray)
  STCLONE(CClsCanvas)
public:
  CClsCanvas() { init(); }
  CClsCanvas(CConxCanvas *f) { init(); setValue(f); }
  ~CClsCanvas() { MMM("destructor"); } // clear() does not delete or decrementUsers, so forget it.
  CClsCanvas(const CClsCanvas &o);
  CClsCanvas(const CClsArray &o);

  CClsCanvas &operator=(const CClsCanvas &o);
  CClsBase *stCloneDeep() const;

  const CConxCanvas *getValue() const { return cv; }
  CConxCanvas *getValue() { return cv; }
  void setValue(CConxCanvas *v) { cv = v; } // Forget the old one completely.
  CConxString printString() const;
  int operator==(const CClsCanvas &o) { return 0; } // DLC
  int operator!=(const CClsCanvas &o) { return !operator==(o); }
  void clear() { cv->clear(); }
  

private:
  void init() { cv = NULL; setRequiredType(CLS_DRAWABLE); }
  void uninitializedCopy(const CClsCanvas &o);

protected:
  NEW_OI_ANSWERER(CClsCanvas); // otherwise `Canvas new' will act as `Array new'
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsCanvas, oiAnswererSync,
                                 oiActionSync, /* non-const */);
private:
  static void initializeAnsweringMachines();


private: // attributes
  CConxCanvas *cv;
  static Answerers *ansMachs;
}; // class CClsCanvas


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STCANVAS_CXX_H
