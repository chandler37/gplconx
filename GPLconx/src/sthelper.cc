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

#include <ctype.h>

#include "sthelper.hh"
#include "stcommon.hh"

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

CF_INLINE
CConxClsVariable::CConxClsVariable(const CConxString &s, CClsBase *p)
  : varName(s)
{
  pointee = p;
}

CF_INLINE
CConxClsVariable::CConxClsVariable(const CConxClsVariable &v)
  : CConxObject(v), varName(v.varName)
{
  pointee = v.pointee;
  // Yes, two variables can refer to the same thing safely.
}

NF_INLINE
CConxClsVariable &CConxClsVariable::operator=(const CConxClsVariable &v)
{
  (void) CConxObject::operator=(v);
  varName = v.varName;
  pointee = v.pointee;
  return *this;
}

NF_INLINE
int CConxClsVariable::operator==(const CConxClsVariable &o) const
{
  return (varName == o.varName && pointee == o.pointee);
}

NF_INLINE
ostream &CConxClsVariable::printOn(ostream &o) const
{
  o << "<CConxClsVariable " << getVariableName() << ">";
  return o;
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

NF_INLINE
CConxClsKeywordMessage &CConxClsKeywordMessage::operator=(const CConxClsKeywordMessage &o)
{
  (void) CConxObject::operator=(o);
  parts = o.parts;
  return *this;
}

CF_INLINE
CConxClsKeywordMessage::~CConxClsKeywordMessage()
{
  /* parts's destruction triggers CConxClsKeyedArg's destructor,
     which does the work of GC.
  */
}

NF_INLINE
int CConxClsKeywordMessage::operator==(const CConxClsKeywordMessage &o) const
{
  return (parts == o.parts);
}

NF_INLINE
int CConxClsKeywordMessage::operator!=(const CConxClsKeywordMessage &o) const
{
  return !operator==(o);
}

NF_INLINE
void CConxClsKeywordMessage::appendKeyedArg(const CConxClsKeyedArg &a)
{
  appendKeyedArg(new CConxClsKeyedArg(a)); // DLC check oom
}

NF_INLINE
void CConxClsKeywordMessage::appendKeyedArg(CConxClsKeyedArg *a) throw(const char *)
{
  if (a == NULL) {
    throw "needed non-NULL pointer in "
      "CConxClsKeywordMessage::appendKeyedArg(CConxClsKeyedArg *a)";
  }
  //    cout << "\nDLC keymsg appending " << a << endl;
  parts.append(a);
  assert(&parts.get(parts.size() - 1) == a);
}

NF_INLINE
void CConxClsKeywordMessage::getObjects(CClsBase *argv[])
  // This fills argv with getNumberArgs() pointers to Smalltalk-like
  // instances.  Some may be NULL.
{
  // DLC move to .cc
  for (size_t i = 0; i < getNumberArgs(); i++) {
    argv[i] = getNthObject(i);
    assert(argv[i] != NULL); // we initialize variables to `nil'
  }
}

NF_INLINE
CClsBase *CConxClsKeywordMessage::getNthObject(size_t n) throw(const char *)
{
  return getNthArg(n).getArg();
}

NF_INLINE
CConxClsKeyedArg &CConxClsKeywordMessage::getNthArg(size_t n) throw(const char *)
{
  return parts.get(n); // may throw
}

NF_INLINE
const CConxClsKeyedArg &CConxClsKeywordMessage::getNthArg(size_t n) const throw(const char *)
{
  return parts.get(n); // may throw
}

NF_INLINE
CConxString CConxClsKeywordMessage::getKeywordMessageName() const
  // Returns `between:and:', e.g. (without the leading hash mark).
{
  CConxString name;
  for (size_t i = 0; i < getNumberArgs(); i++) {
    name += getNthArg(i).getKeyword() + ":";
  }
  assert(name.numberOf(':') == getNumberArgs());
  return name;
}

NF_INLINE
Boole CConxClsKeywordMessage::isMessageNamed(const CConxString &s) const
{
  return (getKeywordMessageName() == s);
}

NF_INLINE
ostream &CConxClsKeywordMessage::printOn(ostream &o) const
{
  o << "<" << className() << " object with child ";
  (void) parts.printOn(o);
  o << ">";
  return o;
}

CF_INLINE
CConxClsMessage::CConxClsMessage(const CConxString &unaryMessage)
{
  msgType = UNARY;
  unaryMsg = unaryMessage;
}

CF_INLINE
CConxClsMessage::CConxClsMessage(const CConxClsKeywordMessage &keyMessage)
{
  msgType = KEYWORD;
  keyMsg = keyMessage;
}

CF_INLINE
CConxClsMessage::CConxClsMessage(const CConxClsMessage &o)
  : CConxObject(o)
{
  uninitializedCopy(o);
}

NF_INLINE
CConxClsMessage &CConxClsMessage::operator=(const CConxClsMessage &o)
{
  (void) CConxObject::operator=(o);
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
int CConxClsMessage::operator==(const CConxClsMessage &o) const
{
  if (msgType == o.msgType) {
    if (msgType == UNARY) {
      if (unaryMsg != o.unaryMsg) return 0;
    } else if (msgType == KEYWORD) {
      if (keyMsg != o.keyMsg) return 0;
    }
    return 1;
  }
  return 0;
}

NF_INLINE
ostream &CConxClsMessage::printOn(ostream &o) const
{
  o << "<" << className() << " object";
  switch (msgType) {
  case UNARY:
    o << ", a unary message `" << unaryMsg << "'"; break;
  case KEYWORD:
    o << ", a keyword message `";
    (void) keyMsg.printOn(o);
    o << "'"; break;
  default:
    o << ", unset";
  }
  return o;
}

NF_INLINE
void CConxClsMessage::clear()
{
  unaryMsg.setString(NULL);
  msgType = NONE;
  keyMsg.clear();
}

NF_INLINE
void CConxClsMessage::setToUnaryMessage(const CConxString &m)
  // DLC does not check m to see if it is valid.
{
  clear();
  msgType = UNARY;
  unaryMsg = m;
}

NF_INLINE
void CConxClsMessage::setToKeywordMessage(const CConxClsKeywordMessage &m)
{
  clear();
  msgType = KEYWORD;
  keyMsg = m;
}

NF_INLINE
const CConxClsKeywordMessage &CConxClsMessage::getKeywordMessage() const throw(const char *)
{
  if (!isKeywordMessage()) throw "this is not a keyword message";
  return keyMsg;
}

NF_INLINE
const CConxString &CConxClsMessage::getUnaryMessage() const throw(const char *)
{
  if (!isUnaryMessage()) throw "this is not a unary message";
  return unaryMsg;
}

NF_INLINE
void CConxClsMessage::getBoundObjects(CClsBase *argv[]) throw (const char *)
{
  if (msgType != KEYWORD) throw "this is not a Keyword message";
  keyMsg.getBoundObjects(argv);
}

NF_INLINE
CConxClsKeyedArg &CConxClsMessage::getNthArg(size_t n) throw(const char *)
{
  if (msgType != KEYWORD) throw "this is not a KeYword message";
  return keyMsg.getNthArg(n); // may throw
}

NF_INLINE
const CConxClsKeyedArg &CConxClsMessage::getNthArg(size_t n) const throw(const char *)
{
  if (msgType != KEYWORD) throw "this is not a KeyWord message";
  return keyMsg.getNthArg(n); // may throw
}

NF_INLINE
Boole CConxClsMessage::isMessageNamed(const CConxString &s) const
{
  if (msgType == UNARY)
    return (unaryMsg == s);
  if (msgType == KEYWORD)
    return keyMsg.isMessageNamed(s);
  return FALSE;
}

NF_INLINE
void CConxClsMessage::uninitializedCopy(const CConxClsMessage &o)
{
  msgType = o.msgType;
  keyMsg = o.keyMsg;
  unaryMsg = o.unaryMsg;
}

CF_INLINE
CConxClsAssignmentList::CConxClsAssignmentList(const CConxClsAssignmentList &o)
  : CConxObject(o)
{
  // DLC finish this!
  abort();
}

NF_INLINE
CConxClsAssignmentList &CConxClsAssignmentList::operator=(const CConxClsAssignmentList &o)
{
  (void) CConxObject::operator=(o);

  // DLC finish this!
  abort();
  return *this;
}

NF_INLINE
const CConxString &CConxClsAssignmentList::getNthVar(size_t n) const throw(const char *)
{
  if (n >= getNumberVars()) {
    throw "Cannot get Nth var CConxClsAssignmentList";
  }
  return vars.get(n);
}

NF_INLINE
ostream &CConxClsAssignmentList::printOn(ostream &o) const
{
  o << "<CConxClsAssignmentList";
  for (size_t i = 0; i < getNumberVars(); i++) {
    o << " " << getNthVar(i);
  }
  o << ">";
  return o;
}

CF_INLINE
CConxClsAnsMach::CConxClsAnsMach(const CConxString &name,
                                 MethodType clsMessage, Answerer *a,
                                 const CConxString &help)
{
  helpTip = help;
  ansMach = a;
  msgName = name;
  classMessage = (clsMessage == CLASS);
}

CF_INLINE
CConxClsAnsMach::CConxClsAnsMach(const CConxClsAnsMach &o)
  : CConxObject(o)
{
  uninitializedCopy(o);
}

NF_INLINE
CConxClsAnsMach &CConxClsAnsMach::operator=(const CConxClsAnsMach &o)
{
  (void) CConxObject::operator=(o);
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
int CConxClsAnsMach::operator==(const CConxClsAnsMach &o) const
{
  return (ansMach == o.ansMach
          && msgName == o.msgName
          && helpTip == o.helpTip
          && classMessage == o.classMessage);
}

NF_INLINE
void CConxClsAnsMach::setMessageName(const CConxString &nm, const CConxString &help, MethodType clsMessage)
{
  msgName = nm;
  helpTip = help;
  classMessage = (clsMessage == CLASS);
}

NF_INLINE
void CConxClsAnsMach::uninitializedCopy(const CConxClsAnsMach &o)
{
  msgName = o.msgName;
  ansMach = o.ansMach;
  helpTip = o.helpTip;
  classMessage = o.classMessage;
  }

OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsAnsMach)
OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsKeyedArg)
OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsAssignmentList)
