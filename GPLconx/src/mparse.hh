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
  CConxMetaParser() { dbg = FALSE; lastParseWasError = FALSE; }
  ~CConxMetaParser() { }
  CConxMetaParser(const CConxMetaParser &o);
  CConxMetaParser &operator=(const CConxMetaParser &o);

  // parser debugging does not matter for equality.
  int operator==(const CConxMetaParser &o) const { return m == o.m; }
  int operator!=(const CConxMetaParser &o) const { return !operator==(o); }

  CConxString getLastParseResult() const;

  // These return -1 for a fatal error, which I think is impossible,
  // or the number of non-fatal errors otherwise.
  long parse(const char *s);
  long parse(FILE *f);

  void restoreStartupObjectWorld(); // Forget all actions from parses.
  void debugParse(Boole s) { dbg = s; }
  Boole isDebuggingParse() const { return dbg; }
  Boole lastParseHadNonFatalErrors() const { return lastParseWasError; }

protected: // virtual functions:
  virtual void initializeClsMgr(CConxClsManager *m);

private: // operations

  long innerParse();
  // returns -1 for a fatal error, which I think is impossible, or the number of non-fatal errors otherwise.

  CConxClsManager &getClsMgr() { return m; }

private: // attributes
  Boole lastParseWasError;
  CConxClsManager m;
  Boole dbg;
}; // class CConxMetaParser


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxMetaParser);

#endif // GPLCONX_MPARSE_CXX_H

