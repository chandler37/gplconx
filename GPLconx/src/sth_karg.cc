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
  Implementation of C++ classes in `sth_karg.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>

#include "sth_karg.hh"
#include "stobject.hh"


#define IS_KEYWORD_CHAR(c) \
         (((c) != ':') && !isspace(c) && (isalnum(c) || ispunct(c)))


CF_INLINE
CConxClsKeyedArg::CConxClsKeyedArg(const CConxString &key, CClsBase *arg)
{
  keyword = key;
  obj = arg;
  if (obj != NULL) obj->incrementUsers();
}

NF_INLINE
void CConxClsKeyedArg::setArg(CClsBase *s)
{
  obj = s;
  if (obj != NULL) obj->incrementUsers();
}

NF_INLINE
void CConxClsKeyedArg::uninitializedCopy(const CConxClsKeyedArg &o)
{
  keyword = o.keyword;
  obj = o.obj;
  if (obj != NULL) obj->incrementUsers(); // DLC note that copying has GC implications.
}

CF_INLINE
CConxClsKeyedArg::~CConxClsKeyedArg()
{
  MAYBE_REMOVE_A_USER(obj);
}

NF_INLINE
void CConxClsKeyedArg::setKeyword(const CConxString &s) throw(const char *)
  // Keywords must have only alphanumeric or punctuation characters in them,
  // but whitespace and the colon is not allowed.
  // s's length must be at least 1.
  // "Bad keyword" is thrown if this is not followed.
{
  size_t sl = s.getLength();
  if (sl < 1) throw "Bad keyword";
  const char *m = s.getString();
  for (size_t i = 0; i < sl; i++) {
    if (!IS_KEYWORD_CHAR(m[i])) throw "Bad keyword";
  }
  keyword = s; // could throw also.
}

CF_INLINE
CConxClsKeyedArg::CConxClsKeyedArg(const CConxClsKeyedArg &o)
  : CConxObject(o)
{
  uninitializedCopy(o);
}

NF_INLINE
CConxClsKeyedArg &CConxClsKeyedArg::operator=(const CConxClsKeyedArg &o)
{
  (void) CConxObject::operator=(o);
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
int CConxClsKeyedArg::operator==(const CConxClsKeyedArg &o) const
{
  return keyword == o.keyword && obj == o.obj;
}

OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsKeyedArg)
