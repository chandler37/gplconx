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
  C++ classes that facilitate parsing the Smalltalk-like language.
*/

#ifndef GPLCONX_MPARSE_CXX_H
#define GPLCONX_MPARSE_CXX_H 1

#include <iostream.h>
#include <assert.h>

#include "clsmgr.hh"


//////////////////////////////////////////////////////////////////////////////
// This class allows us to parse from different sources every once in a while
// and have the changes accumulate nicely.
class CConxMetaParser : VIRT public CConxObject {
public:
  DEFAULT_PRINTON()
public:
  CConxMetaParser() { dbg = FALSE; }
  ~CConxMetaParser() { }
  CConxMetaParser(const CConxMetaParser &o) : CConxObject(o) { dbg = o.dbg; }
  CConxMetaParser &operator=(const CConxMetaParser &o)
  {
    (void) CConxObject::operator=(o);
    dbg = o.dbg;
    return *this;
  }

  // parser debugging does not matter for equality.
  int operator==(const CConxMetaParser &o) const { return m == o.m; }
  int operator!=(const CConxMetaParser &o) const { return !operator==(o); }

  CConxString getLastParseResult() const;
  int parse(const char *s);
  int parse(FILE *f);
  void restoreStartupObjectWorld(); // Forget all actions from parses.
  void debugParse(Boole s) { dbg = s; }
  Boole isDebuggingParse() const { return dbg; }

protected: // virtual functions:
  virtual void initializeClsMgr(CConxClsManager *m)
  {
    assert(m != NULL);
    m->firstInit();  // it doesn't hurt to call firstInit() repeatedly.
  }

private: // operations
  int innerParse();
  CConxClsManager &getClsMgr() { return m; }

private: // attributes
  CConxClsManager m;
  Boole dbg;
}; // class CConxMetaParser


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxMetaParser);

#endif // GPLCONX_MPARSE_CXX_H

