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
  C++ classes that facilitate parsing the Smalltalk-like language
  when using canvases for their side effects (i.e., the viz)
*/

#ifndef GPLCONX_STMPARSE_CXX_H
#define GPLCONX_STMPARSE_CXX_H 1

#include <assert.h>

#include "mparse.hh"
#include "canvas.hh"


//////////////////////////////////////////////////////////////////////////////
// This class allows us to parse from different sources every once in a while
// and have the changes accumulate nicely.
class CConxClsMetaParser : VIRT public CConxMetaParser {
public:
  DEFAULT_PRINTON()
public:
  CConxClsMetaParser() { init(); }
  CConxClsMetaParser(CConxCanvas *kdCanvas, CConxCanvas *pdCanvas,
                     CConxCanvas *uhpCanvas)
  {
    init();
    this->kdCanvas = kdCanvas;
    this->pdCanvas = pdCanvas;
    this->uhpCanvas = uhpCanvas;
  }
  ~CConxClsMetaParser() { }
  CConxClsMetaParser(const CConxClsMetaParser &o)
    : CConxMetaParser(o) { uninitializedCopy(o); }
  CConxClsMetaParser &operator=(const CConxClsMetaParser &o)
  {
    (void) CConxMetaParser::operator=(o);
    uninitializedCopy(o);
    return *this;
  }

  // parser debugging does not matter for equality.
  int operator==(const CConxClsMetaParser &o) const { return 0; } // DLC
  int operator!=(const CConxClsMetaParser &o) const { return !operator==(o); }

protected: // operations
  virtual void initializeClsMgr(CConxClsManager *m)
  {
    assert(m != NULL);
    m->firstInit(pdCanvas, kdCanvas, uhpCanvas);
    // it doesn't hurt to call it repeatedly.
  }

private: // operations
  void init() { pdCanvas = kdCanvas = uhpCanvas = NULL; }
  void uninitializedCopy(const CConxClsMetaParser &o)
  {
    pdCanvas = o.pdCanvas;
    kdCanvas = o.kdCanvas;
    uhpCanvas = o.uhpCanvas;
  }

private: // attributes
  // We do not own these, and we'll never make a copy of them:
  CConxCanvas *pdCanvas, *kdCanvas, *uhpCanvas;
}; // class CConxClsMetaParser


#endif // GPLCONX_STMPARSE_CXX_H

