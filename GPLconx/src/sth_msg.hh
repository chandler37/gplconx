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
  C++ Smalltalkish back-end CConxClsMessage class.
*/

#ifndef GPLCONX_STH_MSG_CXX_H
#define GPLCONX_STH_MSG_CXX_H 1

#include "sth_kmsg.hh"

class CClsBase;
//////////////////////////////////////////////////////////////////////////////
// A general message, i.e. a unary or keyword message.
class CConxClsMessage : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsMessage")
public:
  CConxClsMessage(const CConxString &unaryMessage);
  CConxClsMessage(const CConxClsKeywordMessage &keyMessage);
  CConxClsMessage() { msgType = NONE; }
  CConxClsMessage(const CConxClsMessage &o);
  CConxClsMessage &operator=(const CConxClsMessage &o);

  int operator==(const CConxClsMessage &o) const;
  int operator!=(const CConxClsMessage &o) const { return !operator==(o); }
  ostream &printOn(ostream &o) const;

  Boole isUnaryMessage() const { return msgType == UNARY; }
  Boole isKeywordMessage() const { return msgType == KEYWORD; }
  Boole isSet() const { return msgType != NONE; }
  void clear();
  void setToUnaryMessage(const CConxString &m);
  void setToKeywordMessage(const CConxClsKeywordMessage &m);
  const CConxClsKeywordMessage &getKeywordMessage() const throw(const char *);
  const CConxString &getUnaryMessage() const throw(const char *);
  void getBoundObjects(CClsBase *argv[]) throw (const char *);
  CConxClsKeyedArg &getNthArg(size_t n) throw(const char *);
  const CConxClsKeyedArg &getNthArg(size_t n) const throw(const char *);
  Boole isMessageNamed(const CConxString &s) const;
  CConxString getUnknownMessageComplaint() const;

private: // operations
  void uninitializedCopy(const CConxClsMessage &o);

private: // attributes
  CConxClsKeywordMessage keyMsg;
  CConxString unaryMsg;
  enum MsgType { UNARY, KEYWORD, NONE };
  MsgType msgType;
}; // class CConxClsMessage


#endif // GPLCONX_STH_MSG_CXX_H
