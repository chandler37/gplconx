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
  C++ classes that represent Smalltalk-related things like messages and
  the things that respond to messages.
*/

#ifndef GPLCONX_STHELPER_CXX_H
#define GPLCONX_STHELPER_CXX_H 1

#include <iostream.h>
#include <assert.h>

#include "Starray.hh"
#include "decls.hh"

class CClsBase;

//////////////////////////////////////////////////////////////////////////////
// A class that represents a Smalltalk variable.  It has a name and it points
// to a CClsBase instance, the ``pointee''
class CConxClsVariable : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsVariable")
public:
  CConxClsVariable() : varName() { pointee = NULL; }
  CConxClsVariable(const CConxString &s) : varName(s) { pointee = NULL; }
  CConxClsVariable(const CConxString &s, CClsBase *p);
  CConxClsVariable(const CConxClsVariable &v);
  CConxClsVariable &operator=(const CConxClsVariable &v);

  // we point to it, we don't own it.  DLC decrement user's count here for GC?
  ~CConxClsVariable() { }
  
  void setVariableName(const CConxString &s) { varName = s; }
  const CConxString &getVariableName() const { return varName; }

  void setPointee(CClsBase *p) { pointee = p; }
  CClsBase *getPointee() const { return pointee; }

  int operator==(const CConxClsVariable &o) const;
  int operator!=(const CConxClsVariable &o) const { return !operator==(o); }

  ostream &printOn(ostream &o) const;

private: // attributes
  CConxString varName;
  CClsBase *pointee;
}; // class CConxClsVariable


//////////////////////////////////////////////////////////////////////////////
// A class that represents a keyword and its accompanying argument.
// This does not own the argument, so copies will refer to the same exact
// place in memory.  (GC will take care of the argument's ownership.)
class CConxClsKeyedArg : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsKeyedArg")
  DEFAULT_PRINTON()
public:
  CConxClsKeyedArg() { }
  CConxClsKeyedArg(const CConxString &key, CClsBase *arg);
  CConxClsKeyedArg(const CConxClsKeyedArg &o);
  CConxClsKeyedArg &operator=(const CConxClsKeyedArg &o);
  ~CConxClsKeyedArg();
  // DLC operator== and !=

  int operator==(const CConxClsKeyedArg &o) const;
  int operator!=(const CConxClsKeyedArg &o) const { return !operator==(o); }
  
  const CConxString &getKeyword() const { return keyword; }
  void setKeyword(const CConxString &s) throw(const char *);
  CClsBase *getArg() { return obj; }
  void setArg(CClsBase *s);

protected:
  void uninitializedCopy(const CConxClsKeyedArg &o);

private:
  CConxString keyword;
  CClsBase *obj;
}; // class CConxClsKeyedArg


//////////////////////////////////////////////////////////////////////////////
// A class that represents a keyword message, such as class Magnitude's
// #between:and: message, with arguments.  Thus, sending an instance of this
// class to a Smalltalk-like class instance or object instance will allow
// execution of the keyword message.
// DLC garbage collection????  We decrement users when this is destructed,
// right??  What about parse errors???  Make sure we destruct as many
// instances as we create!
class CConxClsKeywordMessage : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsKeywordMessage")
public:
  CConxClsKeywordMessage() { }
  CConxClsKeywordMessage(const CConxClsKeywordMessage &o)
    : CConxObject(o), parts(o.parts) { }
  CConxClsKeywordMessage &operator=(const CConxClsKeywordMessage &o);
  ~CConxClsKeywordMessage();
  int operator==(const CConxClsKeywordMessage &o) const;
  int operator!=(const CConxClsKeywordMessage &o) const;
  size_t getNumberArgs() const { return parts.size(); }
  // The nth argument is the argument to the nth keyword; n >= 0
  void appendKeyedArg(const CConxClsKeyedArg &a);
  void appendKeyedArg(CConxClsKeyedArg *a) throw(const char *);
  void getObjects(CClsBase *argv[]);

  void getBoundObjects(CClsBase *argv[]) throw (const char *);
  // Fills argv with getNumberArgs() pointers to Smalltalk-like
  // object and class instances.  All will be bound right now -- none
  // will be CClsLBVariable instances.  Some may be NULL.

  CClsBase *getNthObject(size_t n) throw(const char *);
  CConxClsKeyedArg &getNthArg(size_t n) throw(const char *);
  const CConxClsKeyedArg &getNthArg(size_t n) const throw(const char *);
  CConxString getKeywordMessageName() const;
  Boole isMessageNamed(const CConxString &s) const;
  ostream &printOn(ostream &o) const;
  void clear() { parts.clear(); }

private:
  CConxPrintableOwnerArray<CConxClsKeyedArg> parts;
}; // class CConxClsKeywordMessage


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

private: // operations
  void uninitializedCopy(const CConxClsMessage &o);

private: // attributes
  CConxClsKeywordMessage keyMsg;
  CConxString unaryMsg;
  enum MsgType { UNARY, KEYWORD, NONE };
  MsgType msgType;
}; // class CConxClsMessage


//////////////////////////////////////////////////////////////////////////////
// A class that represents a list of assignment statements.  This is useful
// for implementing `L := M := N := 3.3'
class CConxClsAssignmentList : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsAssignmentList")
public:
  CConxClsAssignmentList() { }
  CConxClsAssignmentList(const CConxClsAssignmentList &o);
  CConxClsAssignmentList &operator=(const CConxClsAssignmentList &o);
  ~CConxClsAssignmentList() { }
  // DLC operator== and !=
  size_t getNumberVars() const { return vars.size(); }
  void appendVar(const CConxString &v) { vars.append(v); }
  const CConxString &getNthVar(size_t n) const throw(const char *);
  ostream &printOn(ostream &o) const;


private:
  CConxStringArray vars;
}; // class CConxClsAssignmentList


//////////////////////////////////////////////////////////////////////////////
// An answering machine just accepts messages.  In this case, the messages
// are CConxClsMessage instances.
class CConxClsAnsMach : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsAnsMach")
  DEFAULT_PRINTON()
public: // types
  enum MethodType { OBJECT = FALSE, CLASS = TRUE };
  typedef int /*CClsBase::ErrType*/ (Answerer) (CClsBase *receiver,
                                                CClsBase **result,
                                                CConxClsMessage &o);
public: // methods
  CConxClsAnsMach() { ansMach = NULL; classMessage = FALSE; }
  CConxClsAnsMach(const CConxString &name, MethodType clsMessage,
                  Answerer *a, const CConxString &help);
  CConxClsAnsMach(const CConxClsAnsMach &o);
  CConxClsAnsMach &operator=(const CConxClsAnsMach &o);
  ~CConxClsAnsMach() { }
  
  int operator==(const CConxClsAnsMach &o) const;
  int operator!=(const CConxClsAnsMach &o) const { return !operator==(o); }
  
  Boole isClassMessage() const { return classMessage; }
  const CConxString &getMessageName() const { return msgName; }
  const CConxString &getHelpTip() const { return helpTip; }

  // DLC not checking integrity!
  void setMessageName(const CConxString &nm, const CConxString &help,
                      MethodType clsMessage);

  void setAnswerer(Answerer *o) { ansMach = o; }
  int /*CClsBase::ErrType*/ answer(CClsBase *receiver,
                                   CClsBase **result,
                                   CConxClsMessage &o) throw(const char *);

private:
  void uninitializedCopy(const CConxClsAnsMach &o);

private:
  Answerer *ansMach;
  CConxString msgName, helpTip;
  Boole classMessage;
}; // class CConxClsAnsMach



typedef CConxOwnerArray<CConxClsAnsMach> Answerers;

CConxString getClassInstanceHelpMessage(const Answerers &a);
CConxString getObjectInstanceHelpMessage(const Answerers &a);
CConxString getClassHelpMessage(const CClsBase &c);
CConxString getCompleteHelpMessage(const CClsBase &c, const Answerers &a);


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxClsAnsMach);
OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxClsKeyedArg);
OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxClsAssignmentList);

#endif // GPLCONX_STHELPER_CXX_H

