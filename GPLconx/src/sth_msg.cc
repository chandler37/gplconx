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
  Implementation of C++ classes in `sth_msg.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sth_msg.hh"
#include "stobject.hh"

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

NF_INLINE
CConxString CConxClsMessage::getUnknownMessageComplaint() const
{
  CConxString msg = "unknown ";
  if (isKeywordMessage()) {
    msg += "keyword message #";
    msg += getKeywordMessage().getKeywordMessageName();
  } else {
    assert(isUnaryMessage());
    msg += "unary message #";
    msg += getUnaryMessage();
  }
  return msg;
}

