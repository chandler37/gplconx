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
  Implementation of C++ classes in `sthelper.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sthelper.hh"
#include "stcommon.hh"


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
int /*CClsBase::ErrType*/
CConxClsAnsMach::answer(CClsBase *receiver, CClsBase **result,
                        CConxClsMessage &o) throw(const char *)
{
  if (receiver == NULL) throw "receiver cannot be NULL";
  if (result == NULL) throw "result cannot be NULL";
  if (!o.isSet()) throw "the message must be set";
  if (ansMach == NULL) throw "the Answerer has not yet been set.";
  assert(receiver->isClassInstance() == isClassMessage());
  assert(getMessageName().getLength() > 0);
  assert(o.isMessageNamed(getMessageName()));
  return (*ansMach)(receiver, result, o);
}

NF_INLINE
void CConxClsKeywordMessage::getBoundObjects(CClsBase *argv[])
  throw (const char *)
{
  for (size_t i = 0; i < getNumberArgs(); i++) {
    argv[i] = getNthObject(i);
    assert(argv[i] != NULL);
    // we initialize variables to `nil', so this should be true.
  }
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

CConxString getCompleteHelpMessage(const CClsBase &c, const Answerers &a)
{
  CConxString e = getClassHelpMessage(c);
  e += "\n\n" + getClassInstanceHelpMessage(a);
  e += "\n\n" + getObjectInstanceHelpMessage(a);
  return e;
}

CConxString getClassHelpMessage(const CClsBase &c)
{
  CConxString e;
  CClsBase::ClsType superType = c.getSuperClassType();
  if (superType == c.CLS_INVALID)
    e = "nil";
  else {
    e = "#";
    e += CClsBase::getClsNameByType(superType);
  }
  e += " subclass:  #";
  e += c.getClsName();
  e += "\n\nObject comment:\n'";
  e += c.getClsComment();
  e += "'";
  return e;
}

CConxString getObjectInstanceHelpMessage(const Answerers &a)
{
  CConxString s = "Instance methods:\n";
  for (size_t i = 0; i < a.size(); i++) {
    const CConxClsAnsMach &r = a.get(i);
    if (!r.isClassMessage())
      s += "\t" + r.getMessageName() + " -- " + r.getHelpTip() + "\n";
  }
  return s;
}

CConxString getClassInstanceHelpMessage(const Answerers &a)
{
  CConxString s = "Class methods:\n\n";
  for (size_t i = 0; i < a.size(); i++) {
    const CConxClsAnsMach &r = a.get(i);
    if (r.isClassMessage())
      s += "\t" + r.getMessageName() + " -- " + r.getHelpTip() + "\n";
  }
  return s;
}

OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsAnsMach)
OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsKeyedArg)
OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsAssignmentList)
