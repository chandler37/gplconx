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
  CConxClsVariable(const CConxString &s, CClsBase *p) : varName(s)
  {
    pointee = p;
  }
  CConxClsVariable(const CConxClsVariable &v)
    : CConxObject(v), varName(v.varName)
  {
    pointee = v.pointee;
    // Yes, two variables can refer to the same thing safely.
  }
  CConxClsVariable &operator=(const CConxClsVariable &v)
  {
    (void) CConxObject::operator=(v);
    varName = v.varName;
    pointee = v.pointee;
    return *this;
  }
  ~CConxClsVariable() { } // we point to it, we don't own it.  DLC decrement user's count here for GC?
  
  void setVariableName(const CConxString &s) { varName = s; }
  const CConxString &getVariableName() const { return varName; }

  void setPointee(CClsBase *p) { pointee = p; }
  CClsBase *getPointee() const { return pointee; }

  int operator==(const CConxClsVariable &o) const
  {
    return (varName == o.varName && pointee == o.pointee);
  }
  int operator!=(const CConxClsVariable &o) const { return !operator==(o); }

  ostream &printOn(ostream &o) const
  {
    o << "<CConxClsVariable " << getVariableName() << ">";
    return o;
  }

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
  CConxClsKeyedArg(const CConxClsKeyedArg &o)
    : CConxObject(o)
  {
    uninitializedCopy(o);
  }
  CConxClsKeyedArg &operator=(const CConxClsKeyedArg &o)
  {
    (void) CConxObject::operator=(o);
    uninitializedCopy(o);
    return *this;
  }
  ~CConxClsKeyedArg();
  // DLC operator== and !=

  int operator==(const CConxClsKeyedArg &o) const
  {
    return keyword == o.keyword && obj == o.obj;
  }
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
  CConxClsKeywordMessage &operator=(const CConxClsKeywordMessage &o)
  {
    (void) CConxObject::operator=(o);
    parts = o.parts;
    return *this;
  }
  ~CConxClsKeywordMessage()
  {
    /* parts's destruction triggers CConxClsKeyedArg's destructor,
       which does the work of GC.
    */
  }
  int operator==(const CConxClsKeywordMessage &o) const
  {
    return (parts == o.parts);
  }
  int operator!=(const CConxClsKeywordMessage &o) const { return !operator==(o); }
  size_t getNumberArgs() const { return parts.size(); }
  // The nth argument is the argument to the nth keyword; n >= 0
  void appendKeyedArg(const CConxClsKeyedArg &a)
  {
    appendKeyedArg(new CConxClsKeyedArg(a)); // DLC check oom
  }
  void appendKeyedArg(CConxClsKeyedArg *a) throw(const char *)
  {
    if (a == NULL) {
      throw "needed non-NULL pointer in "
        "CConxClsKeywordMessage::appendKeyedArg(CConxClsKeyedArg *a)";
    }
    //    cout << "\nDLC keymsg appending " << a << endl;
    parts.append(a);
    assert(&parts.get(parts.size() - 1) == a);
  }
  void getObjects(CClsBase *argv[])
  // This fills argv with getNumberArgs() pointers to Smalltalk-like
  // instances.  Some may be NULL.
  {
    // DLC move to .cc
    for (size_t i = 0; i < getNumberArgs(); i++) {
      argv[i] = getNthObject(i);
      assert(argv[i] != NULL); // we initialize variables to `nil'
    }
  }

  void getBoundObjects(CClsBase *argv[]) throw (const char *);
  // Fills argv with getNumberArgs() pointers to Smalltalk-like
  // object and class instances.  All will be bound right now -- none
  // will be CClsLBVariable instances.  Some may be NULL.

  CClsBase *getNthObject(size_t n) throw(const char *)
  {
    return getNthArg(n).getArg();
  }
  CConxClsKeyedArg &getNthArg(size_t n) throw(const char *)
  {
    return parts.get(n); // may throw
  }
  const CConxClsKeyedArg &getNthArg(size_t n) const throw(const char *)
  {
    return parts.get(n); // may throw
  }
  CConxString getKeywordMessageName() const
  // Returns `between:and:', e.g. (without the leading hash mark).
  {
    CConxString name;
    for (size_t i = 0; i < getNumberArgs(); i++) {
      name += getNthArg(i).getKeyword() + ":";
    }
    assert(name.numberOf(':') == getNumberArgs());
    return name;
  }
  Boole isMessageNamed(const CConxString &s) const
  {
    return (getKeywordMessageName() == s);
  }
  ostream &printOn(ostream &o) const
  {
    o << "<" << className() << " object with child ";
    (void) parts.printOn(o);
    o << ">";
    return o;
  }
  void clear() { parts.clear(); }

private:
  CConxPrintableOwnerArray<CConxClsKeyedArg> parts;
}; // class CConxClsKeywordMessage


//////////////////////////////////////////////////////////////////////////////
// A general message, i.e. a unary or keyword message.
class CConxClsMessage : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsMessage")
public:
  CConxClsMessage(const CConxString &unaryMessage)
  {
    msgType = UNARY;
    unaryMsg = unaryMessage;
  }
  CConxClsMessage(const CConxClsKeywordMessage &keyMessage)
  {
    msgType = KEYWORD;
    keyMsg = keyMessage;
  }
  CConxClsMessage() { msgType = NONE; }
  CConxClsMessage(const CConxClsMessage &o) : CConxObject(o)
  {
    uninitializedCopy(o);
  }
  CConxClsMessage &operator=(const CConxClsMessage &o)
  {
    (void) CConxObject::operator=(o);
    uninitializedCopy(o);
    return *this;
  }

  int operator==(const CConxClsMessage &o) const
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
  int operator!=(const CConxClsMessage &o) const { return !operator==(o); }
  ostream &printOn(ostream &o) const
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

  Boole isUnaryMessage() const { return msgType == UNARY; }
  Boole isKeywordMessage() const { return msgType == KEYWORD; }
  Boole isSet() const { return msgType != NONE; }
  void clear()
  {
    unaryMsg.setString(NULL);
    msgType = NONE;
    keyMsg.clear();
  }
  void setToUnaryMessage(const CConxString &m)
  // DLC does not check m to see if it is valid.
  {
    clear();
    msgType = UNARY;
    unaryMsg = m;
  }
  void setToKeywordMessage(const CConxClsKeywordMessage &m)
  {
    clear();
    msgType = KEYWORD;
    keyMsg = m;
  }
  const CConxClsKeywordMessage &getKeywordMessage() const throw(const char *)
  {
    if (!isKeywordMessage()) throw "this is not a keyword message";
    return keyMsg;
  }
  const CConxString &getUnaryMessage() const throw(const char *)
  {
    if (!isUnaryMessage()) throw "this is not a unary message";
    return unaryMsg;
  }
  void getBoundObjects(CClsBase *argv[]) throw (const char *)
  {
    if (msgType != KEYWORD) throw "this is not a Keyword message";
    keyMsg.getBoundObjects(argv);
  }
  CConxClsKeyedArg &getNthArg(size_t n) throw(const char *)
  {
    if (msgType != KEYWORD) throw "this is not a KeYword message";
    return keyMsg.getNthArg(n); // may throw
  }
  const CConxClsKeyedArg &getNthArg(size_t n) const throw(const char *)
  {
    if (msgType != KEYWORD) throw "this is not a KeyWord message";
    return keyMsg.getNthArg(n); // may throw
  }
  Boole isMessageNamed(const CConxString &s) const
  {
    if (msgType == UNARY)
      return (unaryMsg == s);
    if (msgType == KEYWORD)
      return keyMsg.isMessageNamed(s);
    return FALSE;
  }

private: // operations
  void uninitializedCopy(const CConxClsMessage &o)
  {
    msgType = o.msgType;
    keyMsg = o.keyMsg;
    unaryMsg = o.unaryMsg;
  }

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
  CConxClsAssignmentList(const CConxClsAssignmentList &o)
    : CConxObject(o)
  {
    // DLC finish this!
    abort();
  }
  CConxClsAssignmentList &operator=(const CConxClsAssignmentList &o)
  {
    (void) CConxObject::operator=(o);

    // DLC finish this!
    abort();
    return *this;
  }
  ~CConxClsAssignmentList()
  {
  }
  // DLC operator== and !=
  size_t getNumberVars() const { return vars.size(); }
  void appendVar(const CConxString &v)
  {
    vars.append(v);
  }
  const CConxString &getNthVar(size_t n) const throw(const char *)
  {
    if (n >= getNumberVars()) {
      throw "Cannot get Nth var CConxClsAssignmentList";
    }
    return vars.get(n);
  }
  ostream &printOn(ostream &o) const
  {
    o << "<CConxClsAssignmentList";
    for (size_t i = 0; i < getNumberVars(); i++) {
      o << " " << getNthVar(i);
    }
    o << ">";
    return o;
  }


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
                  Answerer *a, const CConxString &help)
  {
    helpTip = help;
    ansMach = a;
    msgName = name;
    classMessage = (clsMessage == CLASS);
  }
  CConxClsAnsMach(const CConxClsAnsMach &o)
    : CConxObject(o)
  {
    uninitializedCopy(o);
  }
  CConxClsAnsMach &operator=(const CConxClsAnsMach &o)
  {
    (void) CConxObject::operator=(o);
    uninitializedCopy(o);
    return *this;
  }
  ~CConxClsAnsMach() { }
  
  int operator==(const CConxClsAnsMach &o) const
  {
    return (ansMach == o.ansMach
            && msgName == o.msgName
            && helpTip == o.helpTip
            && classMessage == o.classMessage);
  }
  int operator!=(const CConxClsAnsMach &o) const { return !operator==(o); }
  
  Boole isClassMessage() const { return classMessage; }
  const CConxString &getMessageName() const { return msgName; }
  const CConxString &getHelpTip() const { return helpTip; }

  // DLC not checking integrity!
  void setMessageName(const CConxString &nm, const CConxString &help,
                      MethodType clsMessage)
  {
    msgName = nm;
    helpTip = help;
    classMessage = (clsMessage == CLASS);
  }

  void setAnswerer(Answerer *o) { ansMach = o; }
  int /*CClsBase::ErrType*/ answer(CClsBase *receiver,
                                   CClsBase **result,
                                   CConxClsMessage &o) throw(const char *);

private:
  void uninitializedCopy(const CConxClsAnsMach &o)
  {
    msgName = o.msgName;
    ansMach = o.ansMach;
    helpTip = o.helpTip;
    classMessage = o.classMessage;
  }

private:
  Answerer *ansMach;
  CConxString msgName, helpTip;
  Boole classMessage;
}; // class CConxClsAnsMach


#define NEED_N_FLOATS(N, fargv, o, result) \
  CClsBase *tMpArgv[N]; double (fargv)[N]; (o).getBoundObjects(tMpArgv); \
  for (int ai = 0; ai < N; ai++) { \
    ENSURE_KEYWD_TYPE(result, (o), tMpArgv, ai, CLS_NUMBER, TRUE); \
    (fargv)[ai] = ((CClsNumber *)tMpArgv[ai])->getFloatValue(); \
  }

#define ENSURE_ALL_N_ARE_OF_TYPE(N, typ, Targv, o) \
    CClsBase *tmpArgv[N]; typ *(Targv)[N]; (o).getBoundObjects(tmpArgv); \
    for (int tmpI = 0; tmpI < N; tmpI++) { \
      ENSURE_KEYWD_TYPE(result, (o), tmpArgv, tmpI, typ::sGetType(), TRUE); \
      (Targv)[tmpI] = (typ *)tmpArgv[tmpI]; \
    }

#define ENSURE_SINGLE_ARG_IS_OF_TYPE(typ, Targv, o) \
             ENSURE_ALL_N_ARE_OF_TYPE(1, typ, Targv, o)

#define ADD_ANS_GETTER(mname, fn) \
      ansMachs->append(CConxClsAnsMach(mname, \
                                       CConxClsAnsMach::OBJECT, \
                                       oiAnswerer ## fn, \
                                       "Returns the " mname))

#define ADD_ANS_SETTER(mname, fn) \
      ansMachs->append(CConxClsAnsMach(mname, \
                                       CConxClsAnsMach::OBJECT, \
                                       oiAnswerer ## fn, \
                                       "Sets the `" mname "' field; returns the receiver"))


#define ANS_GETTER(obj, fn, attribute) \
  ErrType oiAction ## fn(CClsBase **result, CConxClsMessage &o) \
  /* This is not const because you can modify the returned object */ \
  { \
    RETURN_EXISTING(result, attribute); \
  } \
  ANSWERER(obj, oiAnswerer ## fn, oiAction ## fn)

#define ANS_SETTER(cls, fieldCls, ansName, field) \
  ErrType oiAction ## ansName(CClsBase **result, CConxClsMessage &o) \
  { \
    CHECK_READ_ONLYNESS(result); \
    ENSURE_SINGLE_ARG_IS_OF_TYPE(fieldCls, argv, o); \
    MAYBE_REMOVE_A_USER(field); \
    (field) = argv[0]; \
    (field)->incrementUsers(); \
    RETURN_THIS(result); \
  } \
  ANSWERER(cls, oiAnswerer ## ansName, oiAction ## ansName)


typedef CConxOwnerArray<CConxClsAnsMach> Answerers;

CConxString getClassInstanceHelpMessage(const Answerers &a);
CConxString getObjectInstanceHelpMessage(const Answerers &a);
CConxString getClassHelpMessage(const CClsBase &c);
CConxString getCompleteHelpMessage(const CClsBase &c, const Answerers &a);


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxClsAnsMach);
OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxClsKeyedArg);
OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxClsAssignmentList);

#endif // GPLCONX_STHELPER_CXX_H

