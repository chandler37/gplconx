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
  Implementation of C++ classes in `mparse.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mparse.hh"
#include "cmparse.hh"

extern "C" {
// DLC delete extern FILE* yyin;
extern int yydebug;
extern int yyparse(void);
extern void yyrestart(FILE *);
extern void *yy_scan_string(const char *);
extern void yy_delete_buffer(void *);
}

// Global variables:
int conx_count_line;

NF_INLINE
CConxString CConxMetaParser::getLastParseResult() const
// Returns the string that represents the result of the last parse statement,
// which may have occured on the 7th previous invocation of parse, not
// necessarily the last, since parse("") will not cause a result.
{
  const CClsBase *lr = m.getLastResult();
  assert(lr != NULL);
  return lr->printString();
}

NF_INLINE
long CConxMetaParser::parse(const char *s)
// s == NULL causes a nop
{
  void *scanHandle = NULL;
  if (s != NULL) {
    conxP_set_interactive(FALSE);
    scanHandle = yy_scan_string(s);
    if (scanHandle == NULL)
      throw "scanHandle cannot be NULL -- out of memory?";
    initializeClsMgr(&m);
    long retval = innerParse();
    yy_delete_buffer(scanHandle);
    return retval;
  }
  return -20;
}

NF_INLINE
long CConxMetaParser::innerParse()
{
  size_t numErrBefore = m.getNumParsingErrors();
  int rc;
  yydebug = dbg;
  conxP_change_manager(&m);
  conx_count_line = 0;
  rc = yyparse();
  if (rc) {
    cerr << "DLC No, it is not impossible to have a fatal parse error.  Alert maintainer.\n";
    return -1; // fatal error, isn't this impossible.
  }
  size_t nen = m.getNumParsingErrors();
  assert(nen >= numErrBefore);
  lastParseWasError = (nen > numErrBefore);
  return (long) (nen - numErrBefore);
}

NF_INLINE
long CConxMetaParser::parse(FILE *f)
{
  if (f != NULL) {
    yyrestart(f);
  }
  return innerParse();
}

NF_INLINE
void CConxMetaParser::restoreStartupObjectWorld()
{
  m.clear();
}

CF_INLINE
CConxMetaParser::CConxMetaParser(const CConxMetaParser &o)
  : CConxObject(o)
{
  dbg = o.dbg;
  lastParseWasError = o.lastParseWasError;
  // DLC m = o.m
}

NF_INLINE
CConxMetaParser &CConxMetaParser::operator=(const CConxMetaParser &o)
{
  (void) CConxObject::operator=(o);
  dbg = o.dbg;
  lastParseWasError = o.lastParseWasError;
  // DLC m = o.m
  return *this;
}

NF_INLINE
void CConxMetaParser::initializeClsMgr(CConxClsManager *m)
{
  assert(m != NULL);
  m->firstInit();  // it doesn't hurt to call firstInit() repeatedly.
}
