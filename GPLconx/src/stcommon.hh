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
  C++ classes that represent Smalltalk classes and objects that are
  not specific to GPLconx; they may be reused more broadly.
*/

#ifndef GPLCONX_STCOMMON_CXX_H
#define GPLCONX_STCOMMON_CXX_H 1

#include <iostream.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <strstream.h>

#include "CString.hh"
#include "CArray.hh"
#include "Starray.hh"
#include "sthelper.hh"
#include "decls.hh"


//////////////////////////////////////////////////////////////////////////////
// Macros:
#define IS_ID_FIRST_CHAR(c) (isupper(c) || islower(c))
#define IS_ID_CHAR(c) (IS_ID_FIRST_CHAR(c) || isdigit(c))
#define IS_KEYWORD_CHAR(c) \
         (((c) != ':') && !isspace(c) && (isalnum(c) || ispunct(c)))

// Helper macros:
#define MSG_WANTED(arity, msgArity) (((arity) == 0) || ((arity) == (msgArity)))
#define GET_MESSAGE(nmsg, msgs, helptips, msgName, helpTip) \
      ++(nmsg); \
      (msgs).append((msgName)); \
      if ((helptips) != NULL) \
        (helptips)->append((helpTip)) \

#define PRINTSTRING "printString" // standard Smalltalk unary method.

#define DEFAULT_PRINTSTRING(d) \
      ostrstream ostr; \
      ostr << d << ends; \
      const char *val = ostr.str(); \
      CConxString sval = val; \
      delete [] val; \
      return sval

#define SET_RESULT(result, obj) if ((result) != NULL) *(result) = obj

#define RETURN_EXISTING(result, obj) \
   do { SET_RESULT(result, obj); return OK; } while (0)

#define RETURN_THIS(result) RETURN_EXISTING(result, this)

#define SET_ERROR_RESULT(result, specificError) \
        SET_RESULT(result, new CClsError(specificError)) // DLC GC thiswill cause a throw in removeUsers

#define RETURN_BOOLE(val, result) \
   do { \
     SET_RESULT(result, new CClsBoolean((val) ? TRUE : FALSE));  /* DLC GC thiswill cause a throw in removeUsers */ \
     return CClsBase::OK_NEW_THING; \
   } while (0)

#define RETURN_NEW_RESULT(result, newObj) \
     SET_RESULT(result, newObj); return OK_NEW_THING // DLC check oom GC

#define RETURN_ERROR_RESULT(result, specificError) \
       do { \
         SET_ERROR_RESULT(result, specificError); \
         return CClsBase::NO_SUCH_MESSAGE; \
       } while (0)

#define RET_KEYWD_TYPE_ERROR(result, o, keywordNumber, typ, objInst) \
          RETURN_ERROR_RESULT(result, CConxString("`") \
                              + o.getNthArg(keywordNumber).getKeyword() \
                              + ":', keyword number " \
                              + CConxString((long)keywordNumber+1) \
                              + ", requires an argument of type ``" + typ \
                              + ((objInst) ? " object instance" \
                                           : " class instance") \
                              + "''")


#define ENSURE_KEYWD_TYPE(result, o, argv, keywordNumber, clsType, objInst) \
     do { \
       if (!argv[keywordNumber]->isType(clsType)) \
         RET_KEYWD_TYPE_ERROR(result, o, keywordNumber, \
                              CClsBase::getClsNameByType(clsType), objInst); \
     } while (0)

#define TRY_TO_ANSWER_BY_MACHINE() \
    initializeAnsweringMachines(); \
    size_t ksz = ansMachs->size(); \
    for (size_t k = 0; k < ksz; k++) { \
      CConxClsAnsMach &machine = ansMachs->get(k); \
      if (machine.isClassMessage() == isClassInstance() \
          && o.isMessageNamed(machine.getMessageName())) { \
        return (ErrType) machine.answer(this, result, o); \
      } \
    }

#define DEFAULT_SEND_MESSAGE(superClass) \
public: \
  Answerers *getAnswerers() { return ansMachs; } \
  ErrType sendMessage(CClsBase **result, CConxClsMessage &o) \
  { \
    TRY_TO_ANSWER_BY_MACHINE() \
    return superClass::sendMessage(result, o); \
  } \
private:

// DLC AC_CXX_STRINGIFY needs to be used, if only it existed...
// When you want to define a function for an answering machine
// (CConxClsAnsMach) to call, use this macro.
#define ANSWERER(classType, staticFunctionName, correspondingMethodName) \
  static int staticFunctionName(CClsBase *receiver, \
                                CClsBase **result, \
                                CConxClsMessage &o) \
  { \
    LLL(staticClassName() << "::" << "static ErrType " \
        << #staticFunctionName \
        << "(CClsBase *receiver, CClsBase **result, CConxClsMessage &o)"); \
    assert(receiver != NULL); assert(result != NULL); assert(o.isSet()); \
    assert(receiver->isType(sGetType())); \
    return (int) ((classType *)receiver)->correspondingMethodName(result, o); \
  }
// DLC check OOM above rather than in each *iAction*()

#define ANSWERER_FOR_ACTION_DEFN_BELOW(c, s, d, e) \
           ANSWERER(c, s, d) \
           ErrType d(CClsBase **result, CConxClsMessage &o) e

#define ANSWERER_DECL(classType, staticFunctionName, correspondingMethodName) \
  static int staticFunctionName(CClsBase *receiver, \
                                CClsBase **result, \
                                CConxClsMessage &o)
#define ANSWERER_AND_ACTION_DECL(c, s, d, e) \
           ANSWERER_DECL(c, s, d); \
           ErrType d(CClsBase **result, CConxClsMessage &o) e

#define ANSWERER_IMPL(classType, staticFunctionName, correspondingMethodName) \
inline \
int classType::staticFunctionName(CClsBase *receiver, \
                                  CClsBase **result, \
                                  CConxClsMessage &o) \
{ \
  assert(receiver != NULL); assert(result != NULL); assert(o.isSet()); \
  assert(receiver->isType(sGetType())); \
  return (int) ((classType *)receiver)->correspondingMethodName(result, o); \
}

#define ANSMACH_ANSWERS_INNER() \
    initializeAnsweringMachines(); \
    size_t ksz = ansMachs->size(); \
    for (size_t k = 0; k < ksz; k++) { \
      CConxClsAnsMach &machine = ansMachs->get(k); \
      if (machine.isClassMessage() == isClassInstance() \
          && msg == machine.getMessageName()) { \
        return TRUE; \
      } \
    }

#define ANSMACH_ANSWERS(superClass) \
public: \
  Boole answers(const CConxString &msg) const \
  { \
    ANSMACH_ANSWERS_INNER() \
    return superClass::answers(msg); \
  } \
private:

// Relies on the copy constructor and assignment operator:
#define STCLONE(cls) \
public: \
  CClsBase *stClone() const { return new cls(*this); } \
  void setToCopyOf(const CClsBase &o) throw(const char *) \
  { \
    if (!o.isType(sGetType())) \
      throw "cannot set to a copy of an object of that type"; \
    (void) operator=((const cls &)o); \
  } \
private:

// Relies on the copy constructor and assignment operator:
#define STCLONE2(cls) \
  STCLONE(cls) \
public: \
  CClsBase *stCloneDeep() const { return stClone(); } \
private:


#define REMOVE_A_USER(obj) \
  do { \
    if ((obj)->decrementUsers() == 0) delete (obj); \
    (obj) = NULL; \
  } while (0)

#define MAYBE_REMOVE_A_USER(obj) \
  do { \
    if ((obj) != NULL) REMOVE_A_USER(obj); \
  } while (0)

#define CHECK_READ_ONLYNESS(result) \
  do { \
    if (isReadOnly()) RETURN_ERROR_RESULT(result, "receiver is read-only"); \
  } while(0)

#define NEW_OI_ANSWERER(cls) \
  ErrType ciActionNew(CClsBase **result, CConxClsMessage &o) const \
  { \
    RETURN_NEW_RESULT(result, new cls()); \
  } \
  ANSWERER(cls, ciAnswererNew, ciActionNew)

#ifndef NDEBUG
#define INVARIANTS() invariants()
#else
#define INVARIANTS() // do nothing
#endif

//////////////////////////////////////////////////////////////////////////////
// An object with a reference count, useful for garbage collection (GC)
// routines.  There is no need to subclass this correctly, i.e. to call its
// copy constructor and assignment operator, because a copy's use count should
// begin at zero.  You have to increment the copied object's reference count
// if you want that to happen.
class CConxGCObject : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxGCObject")
public:
  CConxGCObject() { numUsers = 0; }
  CConxGCObject(size_t initialNumUsers) { numUsers = initialNumUsers; }
  CConxGCObject(const CConxGCObject &o) : CConxObject(o) { numUsers = 0; }
  CConxGCObject &operator=(const CConxGCObject &o) {
    (void) CConxObject::operator=(o);
    // the users point to this object, so they remain the same number.
    return *this;
  }

  // virtual so that CConxClsManager::removeUser will work without
  // a memory leak, e.g.:
  ~CConxGCObject() { }

  ostream &printOn(ostream &o) const
  {
    o << "<CConxGCObject with " << getNumUsers() << " references to it>";
    return o;
  }
  size_t decrementUsers() throw(int)
  {
    MMM("size_t decrementUsers() throw(int)");
    LLL("decrementing from " << getNumUsers());
    if (getNumUsers() == 0) throw 0;
    LLL("before setNumUsers");
    setNumUsers(getNumUsers() - 1);
    return getNumUsers();
  }
  void incrementUsers() { MMM("void incrementUsers()"); setNumUsers(1+getNumUsers()); }
  void setNumUsers(size_t sz) { numUsers = sz; }
  size_t getNumUsers() const { return numUsers; }

private: // attributes
  size_t numUsers;
}; // class CConxGCObject


class CConxClsAnsMach;
class CConxClsMessage;


//////////////////////////////////////////////////////////////////////////////
// The abstract base class for all of our Smalltalk-like classes.
class CClsBase : VIRT public CConxGCObject {
  CCONX_CLASSNAME("CClsBase")
public: // types
  enum ErrType {
    OK,
    OK_NEW_THING,// ``thing'' instead of ``class instance or object instance'' 
    NO_SUCH_MESSAGE
  };
  enum ClsType {
// When adding to this, update `static CConxString clsTypeToString(ClsType c)'
// and `static const char *CClsBase::getClsNameByType(ClsType c)
// and CConxClsManager's initialization and the test cases.
    CLS_OBJECT_BASE,
    CLS_UNDEFINED_OBJECT,   /* nil class */
    CLS_BOOLEAN,            /* true false */
    CLS_FLOAT,              /* 3.3 */
    CLS_MODELIDENTIFIER,    /* #klein */
    CLS_COLOR,
    CLS_CHARACTERARRAY,
    CLS_ARRAY,
    CLS_SMALLINT,
    CLS_DRAWABLE,
    CLS_NUMBER,
    CLS_SYSTEM,
    CLS_CANVAS,
    CLS_LINE,
    CLS_CIRCLE,
    // DLC NEWSTCLASS
    CLS_ERROR,
    CLS_SYMBOL,             /* #symbol */
    CLS_STRING,             /* 'string literal' */
    CLS_VARIABLE,           /* a variable that is bound immediately. */

    /* Geometric objects: */
    CLS_POINT,

    CLS_INVALID             /* This must be the last value. */
  };

public:
  CClsBase()
  {
    // No, this is not arbitrary.  Number one, it is more common, but more
    // importantly, this means that in
    // CClsFloat::CClsFloat(double) we don't have to say
    // `isObjectInstance = TRUE;'.  There are no constructors that make
    // class instances (yet).
    isObjectInstance = isModifiable = TRUE;
    INVARIANTS();
  }
  CClsBase(const CClsBase &o) : CConxGCObject(o)
  {
    isObjectInstance = o.isObjectInstance;
    isModifiable = TRUE; // a copy is not read-only.
    INVARIANTS();
  }
  CClsBase &operator=(const CClsBase &o)
  {
    (void) CConxGCObject::operator=(o);
    isObjectInstance = o.isObjectInstance;
    isModifiable = TRUE; // a copy is read-only.
    INVARIANTS();
    return *this;
  }

  ostream &printOn(ostream &o) const
  {
    INVARIANTS();
    o << "<" << className() << " "
      << ((isClassInstance()) ? "object" : "class")
      << " instance value:" << printString() << ">";
    return o;
  }
  int operator==(const CClsBase &o)
  {
    INVARIANTS();
    return (isClassInstance() == o.isClassInstance()
            && isReadOnly() == o.isReadOnly());
    // read-onlyness does not affect Smalltalk equality but does affect C++
    // equality.
  }
  int operator!=(const CClsBase &o) { return !operator==(o); }

  // In Smalltalk, we have class instances and object instances.  When you
  // call "String new", you get an object instance "''", but when you call
  // "'hi' new", you get a parse error.
  void setClassInstance(Boole isClass)
  {
    INVARIANTS();
    isObjectInstance = !isClass;
  }
  Boole isClassInstance() const
  {
    INVARIANTS();
    return !isObjectInstance;
  }

  virtual void setReadOnly(Boole readOnly)
  {
    INVARIANTS();
    isModifiable = !readOnly;
  }
  Boole isReadOnly() const
  {
    INVARIANTS();
    return !isModifiable;
  }

public: // virtual functions

  // For all below, the messages are interpreted differently depending on
  // isClassInstance().

  // Also, we define arity in a funny way.  The arity of a unary message
  // is 1; the arity of a binary message (of which we have none) is 2,
  // so the arity of a keyword message that has n keywords is 2+n.
  // Hence, the arity of "new:" is 3, the arity of "new" is 1, the arity of
  // "x:y:model:" is 5, etc.


  virtual Boole answers(const CConxString &msg) const;

  virtual Answerers *getAnswerers()
  {
    INVARIANTS();
    return ansMachs;
  }
  virtual void setToCopyOf(const CClsBase &o) throw(const char *)
  {
    INVARIANTS();
    (void) operator=(o);
  }

  // Processes a unary or keyword message o.  o will not be unset.
  // Returns NO_SUCH_MESSAGE if the message is not
  // understood and sets *result to a CClsError object instance.
  //
  // Returns OK and sets result to a preexisting object instance or class
  // instance or returns OK_NEW_THING and sets result to a newly allocated
  // CClsBase instance (i.e., class instance or object instance).
  //
  // Answers for the specialized class and then passes it on to its
  // superclass (except in CClsBase itself).
  virtual ErrType sendMessage(CClsBase **result, CConxClsMessage &o);

  // Return the type of the object.
  virtual ClsType getType() const
  {
    INVARIANTS();
    return CLS_OBJECT_BASE;
  }
  static ClsType sGetType() { return CLS_OBJECT_BASE; }
  virtual Boole isType(ClsType m) const
  {
    INVARIANTS();
    return m == CLS_OBJECT_BASE;
  }
  static ClsType sGetSuperClassType() { return CLS_INVALID; }
  virtual ClsType getSuperClassType() const { return CLS_INVALID; }
#define CLSTYPE(SUPERCLASS, x) \
                   public: \
                     ClsType getType() const { return x; } \
                     static ClsType sGetSuperClassType() \
                     { \
                       return SUPERCLASS::sGetType(); \
                     } \
                     ClsType getSuperClassType() const \
                     { \
                       return SUPERCLASS::sGetType(); \
                     } \
                     Boole isType(ClsType m) const \
                     { \
                        if (m == sGetType()) return TRUE; \
                        return SUPERCLASS::isType(m); \
                     } \
                     static ClsType sGetType() { return x; } \
                   private:

  // This is associated with the CLSNAME macro, and is used to implement
  // printString, e.g.
  virtual const char *getClsComment() const
  {
    INVARIANTS();
    return "I am the root of the " PACKAGE " class hierarchy.  All other classes are derived from me.";
  }
  virtual const char *getClsName() const { return "Object"; }
  static const char *sGetClsName() { return "Object"; }
#define CLSNAME(x, cc) \
                   public: \
                     const char *getClsName() const { return x; } \
                     static const char *sGetClsName() { return x; } \
                     const char *getClsComment() const { return cc; } \
                     static const char *sGetClsComment() { return cc; } \
                   private:

  // Like Smalltalk's 'Object new printString'
  virtual CConxString printString() const
  {
    INVARIANTS();
    if (isClassInstance())
      return getClsName();
    else
      return CConxString("an ") + getClsName();
  }

  // Cloning preserves the receiver and creates a new instance identical to
  // the receiver.  A pointer to a newly allocated instance is returned.
  virtual CClsBase *stClone() const
  {
    INVARIANTS();
    return new CClsBase(*this);
  }
  // stClone() makes a shallow copy, i.e. a copy in which all CClsBase
  // objects are shared, i.e. both the original and the copy point to
  // the same things.  This only matters for Drawables, Arrays, and other
  // containers.
  virtual CClsBase *stCloneDeep() const
  {
    LLL("CClsBase *CClsBase::stCloneDeep()");
    INVARIANTS();
    return stClone();
  }
  // stCloneDeep() makes a deep copy, i.e. a copy in which all CClsBase
  // objects are copied.  This means that a container will have new, copied
  // contents.
  virtual void makeReadOnly()
  {
    INVARIANTS();
    setReadOnly(TRUE);
  }


public: // static functions
  // Returns 0 if msg could not be a message name, e.g. "" or ":" or
  // "have: not:"; returns the arity of the message (1 is unary) if
  // it is a valid message.
  static size_t getMessageArity(const CConxString &msg);
  static const char *getClsNameByType(ClsType c);

  static CConxString clsTypeToString(ClsType c);

protected:
  ANSWERER_AND_ACTION_DECL(CClsBase, iAnswererHelp, iActionHelp,
                           /* const, actually */);
  ANSWERER_AND_ACTION_DECL(CClsBase, oiAnswererSet, oiActionSet,
                           /* non-const */);
  ANSWERER_AND_ACTION_DECL(CClsBase, ciAnswererClass, ciActionClass, const);
  ANSWERER_AND_ACTION_DECL(CClsBase, oiAnswererClass, oiActionClass, const);
  ANSWERER_AND_ACTION_DECL(CClsBase, ciAnswererSuperClass, ciActionSuperClass,
                           const);
  ANSWERER_AND_ACTION_DECL(CClsBase, oiAnswererMakeReadOnly,
                           oiActionMakeReadOnly, /* non-const */);
  ANSWERER_AND_ACTION_DECL(CClsBase, oiAnswererIsReadOnly,
                           oiActionIsReadOnly, const);
  ANSWERER_AND_ACTION_DECL(CClsBase, oiAnswererSuperClass, oiActionSuperClass,
                           const);
  ANSWERER_AND_ACTION_DECL(CClsBase, iAnswererPrintString, iActionPrintString,
                           const);
  ANSWERER_AND_ACTION_DECL(CClsBase, iAnswererIsClass, iActionIsClass, const);
  ANSWERER_AND_ACTION_DECL(CClsBase, iAnswererClone, iActionClone, const);
  ANSWERER_AND_ACTION_DECL(CClsBase, oiAnswererCloneDeep, oiActionCloneDeep,
                           const);
  ANSWERER_AND_ACTION_DECL(CClsBase, oiAnswererExactlyEquals,
                           oiActionExactlyEquals, const);

#define TEST_TYPE_IMPL(action, clsType) \
  inline \
  CClsBase::ErrType CClsBase::action(CClsBase **result, CConxClsMessage &o) \
  { \
    INVARIANTS(); \
    RETURN_BOOLE(isType(clsType) && !isClassInstance(), result); \
  }

#define TEST_NOT_TYPE_IMPL(action, clsType) \
  inline \
  CClsBase::ErrType CClsBase::action(CClsBase **result, CConxClsMessage &o) \
  { \
    RETURN_BOOLE(!(isType(clsType) && !isClassInstance()), result); \
  }

#define TEST_TYPE_DECL(answerer, action, clsType) \
  ANSWERER_DECL(CClsBase, answerer, action); \
  ErrType action(CClsBase **result, CConxClsMessage &o)


#define TEST_NOT_TYPE_DECL(answerer, action, clsType) \
  ANSWERER_DECL(CClsBase, answerer, action); \
  ErrType action(CClsBase **result, CConxClsMessage &o)


#define TYPE_TESTS_DECLS(cn, clsType) \
  TEST_TYPE_DECL(iAnswererIs ## cn, iActionIs ## cn, clsType); \
  TEST_NOT_TYPE_DECL(iAnswererNot ## cn, iActionNot ## cn, clsType)

  TYPE_TESTS_DECLS(Nil, CLS_UNDEFINED_OBJECT);
  TYPE_TESTS_DECLS(Float, CLS_FLOAT);
  TYPE_TESTS_DECLS(Symbol, CLS_SYMBOL);
  TYPE_TESTS_DECLS(String, CLS_STRING);
  TYPE_TESTS_DECLS(ParseError, CLS_ERROR);
  TYPE_TESTS_DECLS(ModelIdentifier, CLS_MODELIDENTIFIER);
  TYPE_TESTS_DECLS(Color, CLS_COLOR);
  TYPE_TESTS_DECLS(CharacterArray, CLS_CHARACTERARRAY);
  TYPE_TESTS_DECLS(Array, CLS_ARRAY);
  TYPE_TESTS_DECLS(SmallInt, CLS_SMALLINT);
  TYPE_TESTS_DECLS(Drawable, CLS_DRAWABLE);
  TYPE_TESTS_DECLS(Number, CLS_NUMBER);
  TYPE_TESTS_DECLS(System, CLS_SYSTEM);
  TYPE_TESTS_DECLS(Canvas, CLS_CANVAS);
  TYPE_TESTS_DECLS(Line, CLS_LINE);
  TYPE_TESTS_DECLS(Circle, CLS_CIRCLE);
// DLC NEWSTCLASS

private:
#ifndef NDEBUG
  void invariants() const
  // This makes sure that things that are always true are always true.
  {
    assert(IS_A_BOOLE(isObjectInstance));
    assert(IS_A_BOOLE(isModifiable));
    // Yes, this caught a bug once, when 1344131342 was stored in one of them.
  }
#endif
  static void initializeAnsweringMachines();

private:
  Boole isObjectInstance, isModifiable;
  static Answerers *ansMachs;
}; // class CClsBase


//////////////////////////////////////////////////////////////////////////////
// CClsError represents an error, such as the result of a bad message.
class CClsError : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsError")
  CLSNAME("ParseError",
          "I am an error, like those you get when you type something in wrong.")
  CLSTYPE(CClsBase, CLS_ERROR)
  STCLONE2(CClsError)
public:
  CClsError() : errstr("unspecified error") { }
  CClsError(const CConxString &s)
  {
    MMM("CClsError(const CConxString &s)");
    LLL("Error is " << s);
    setValue(s);
  }
  CClsError(const CClsError &o) : CClsBase(o), errstr(o.errstr) { }
  CClsError &operator=(const CClsError &o)
  {
    (void) CClsBase::operator=(o);
    errstr = o.errstr;
    return *this;
  }

  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      return CConxString(getClsName()) + ": " + errstr;
    }
  }
  void setValue(const CConxString &s) { errstr = s; }
  const CConxString &getValue() const { return errstr; }
  int operator==(const CClsError &o) { return errstr == o.errstr; }
  int operator!=(const CClsError &o) { return !operator==(o); }

private: // attributes
  CConxString errstr;
}; // class CClsError


//////////////////////////////////////////////////////////////////////////////
// Our String container is the base class for both the #Symbol
// and 'StringLiteral' classes.
class CClsCharacterArray : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsCharacterArray")
  CLSNAME("CharacterArray", "I hold characters.")
  CLSTYPE(CClsBase, CLS_CHARACTERARRAY)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsCharacterArray)
public:
  CClsCharacterArray() : string() { }
  CClsCharacterArray(const CConxString &s) : string(s) { }
  CClsCharacterArray(const CClsCharacterArray &o)
    : CClsBase(o), string(o.string) { }
  CClsCharacterArray &operator=(const CClsCharacterArray &o)
  {
    (void) CClsBase::operator=(o);
    string = o.string;
    return *this;
  }

  virtual void setValue(const CConxString &str) { string = str; }
  const CConxString &getValue() const { return string; }
  int operator==(const CClsCharacterArray &o) { return string == o.string; }
  int operator!=(const CClsCharacterArray &o) { return !operator==(o); }

protected:
  ANSWERER(CClsCharacterArray, oiAnswererSet, oiActionSet);
  ErrType oiActionSet(CClsBase **result, CConxClsMessage &o)
  {
    ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsCharacterArray, argv, o);
    CHECK_READ_ONLYNESS(result);
    if (getType() == CLS_SYMBOL && !argv[0]->isType(CLS_SYMBOL))
      RETURN_ERROR_RESULT(result, "You can set a String to a Symbol, but not vice versa");
    setValue(argv[0]->getValue());
    RETURN_THIS(result);
  }
  ANSWERER_FOR_ACTION_DEFN_BELOW(CClsCharacterArray, oiAnswererLength,
                                 oiActionLength, const);
private:
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
      // DLC "length", e.g.
      ansMachs->append(CConxClsAnsMach("set:",
                                       CConxClsAnsMach::OBJECT,
                                       oiAnswererSet,
                                       "Sets the receiver's value to that of the object passed in if their types agree"));
      ansMachs->append(CConxClsAnsMach("length",
                                       CConxClsAnsMach::OBJECT,
                                       oiAnswererLength,
                                       "Returns the receiver's length, which does not include the '#' character"));

    }
  }

private: // attributes
  CConxString string;
  static Answerers *ansMachs;
}; // class CClsCharacterArray


//////////////////////////////////////////////////////////////////////////////
// Our String class.
// `'hi there''
// `Point new x: 'this is an illegal argument' y: 0.2'
class CClsStringLiteral : VIRT public CClsCharacterArray {
  CCONX_CLASSNAME("CClsStringLiteral")
  CLSNAME("String", "I am a string of characters.")
  CLSTYPE(CClsCharacterArray, CLS_STRING)
  STCLONE2(CClsStringLiteral)
public:
  CClsStringLiteral() { }
  CClsStringLiteral(const CConxString &s) : CClsCharacterArray(s) { }
  CClsStringLiteral(const CClsStringLiteral &o)
    : CClsCharacterArray(o) { }
  CClsStringLiteral &operator=(const CClsStringLiteral &o)
  {
    (void) CClsCharacterArray::operator=(o);
    return *this;
  }
  // test that CClsCharacterArray::sendMessage() is called.
  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      return CConxString("'") + getValue() + "'";
    }
  }

  int operator==(const CClsStringLiteral &o) {
    return CClsCharacterArray::operator==(o);
  }
  int operator!=(const CClsStringLiteral &o) { return !operator==(o); }
}; // class CClsStringLiteral


//////////////////////////////////////////////////////////////////////////////
// Our equivalent to Smalltalk's Boolean class.  Don't expect all the usual
// methods, though.
class CClsBoolean : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsBoolean")
  CLSNAME("Boolean", "I am a Boolean value, i.e. I am either true or false.")
  CLSTYPE(CClsBase, CLS_BOOLEAN)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsBoolean)
public:
  CClsBoolean() { d = TRUE; }
  CClsBoolean(Boole od) { setValue(od); }
  CClsBoolean(const CClsBoolean &o)
    : CClsBase(o) { d = o.d; }
  CClsBoolean &operator=(const CClsBoolean &o)
  {
    (void) CClsBase::operator=(o);
    d = o.d;
    return *this;
  }

  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      return CConxString(d ? "true" : "false");
    }
  }
  void setValue(Boole od) { d = od; }
  Boole getValue() const { return d; }
  // DLC operator== should be true only for the same instance type {class, object} fix this for all ClsCls
  int operator==(const CClsBoolean &o)
  {
    if (isClassInstance() != o.isClassInstance()) return 0;
    return d == o.d;
  }
  int operator!=(const CClsBoolean &o) { return !operator==(o); }

protected:
  ANSWERER(CClsBoolean, ciAnswererTrue, ciActionTrue);
  ErrType ciActionTrue(CClsBase **result, CConxClsMessage &o) const
  {
    RETURN_NEW_RESULT(result, new CClsBoolean(TRUE));
  }
  ANSWERER(CClsBoolean, ciAnswererFalse, ciActionFalse);
  ErrType ciActionFalse(CClsBase **result, CConxClsMessage &o) const
  {
    RETURN_NEW_RESULT(result, new CClsBoolean(FALSE));
  }
  ANSWERER(CClsBoolean, oiAnswererNot, oiActionNot);
  ErrType oiActionNot(CClsBase **result, CConxClsMessage &o) const
  {
    RETURN_NEW_RESULT(result, new CClsBoolean(!getValue()));
  }
  ANSWERER(CClsBoolean, oiAnswererAnd, oiActionAnd);
  ErrType oiActionAnd(CClsBase **result, CConxClsMessage &o) const
  {
    CClsBase *argv[1]; o.getBoundObjects(argv);
    ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_BOOLEAN, TRUE);
    RETURN_NEW_RESULT(result,
                      new CClsBoolean(getValue() \
                                   && ((CClsBoolean *)argv[0])->getValue()));
  }
  ANSWERER(CClsBoolean, oiAnswererOr, oiActionOr);
  ErrType oiActionOr(CClsBase **result, CConxClsMessage &o) const
  {
    CClsBase *argv[1]; o.getBoundObjects(argv);
    ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_BOOLEAN, TRUE);
    RETURN_NEW_RESULT(result,
                      new CClsBoolean(getValue() \
                                   || ((CClsBoolean *)argv[0])->getValue()));
  }
  ANSWERER(CClsBoolean, oiAnswererXOr, oiActionXOr);
  ErrType oiActionXOr(CClsBase **result, CConxClsMessage &o) const
  {
    CClsBase *argv[1]; o.getBoundObjects(argv);
    ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_BOOLEAN, TRUE);
    Boole oval = ((CClsBoolean *)argv[0])->getValue();
    RETURN_NEW_RESULT(result, new CClsBoolean((getValue() && !oval)
                                              || (!getValue() && oval)));
  }
  ANSWERER(CClsBoolean, ciAnswererHelp, ciActionHelp);
  ErrType ciActionHelp(CClsBase **result, CConxClsMessage &o) const
  {
    RETURN_NEW_RESULT(result,
            new CClsStringLiteral(getCompleteHelpMessage(*this, *ansMachs)));
  }
private:
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();

      // DLC for efficiency, don't allow `Boolean true' and `Boolean false',
      // force copying `true' and `false'.
      ansMachs->append(CConxClsAnsMach("help", CConxClsAnsMach::CLASS,
                                       ciAnswererHelp,
                                       "returns help on this class's methods"));
      ansMachs->append(CConxClsAnsMach("true", CConxClsAnsMach::CLASS,
                                       ciAnswererTrue,
                                       "returns a new object instance that represents truth.  The system predefines such an instance and assigns it to the variable `true'; you will benefit from increased efficiency if you just use `true'."));
      ansMachs->append(CConxClsAnsMach("false", CConxClsAnsMach::CLASS,
                                       ciAnswererFalse,
                                       "returns a new object instance that represents falsehood.  The system predefines such an instance and assigns it to the variable `false'; you will benefit from increased efficiency if you just use `false'"));

      // object instance methods:
      ansMachs->append(CConxClsAnsMach("not", CConxClsAnsMach::OBJECT,
                                       oiAnswererNot,
                                       "Returns a Boolean object instance whose truth value is the opposite of the receiver"));
      ansMachs->append(CConxClsAnsMach("and:", CConxClsAnsMach::OBJECT,
                                       oiAnswererAnd,
                                       "Returns true iff the reciever and the Boolean argument are both true"));
      ansMachs->append(CConxClsAnsMach("or:", CConxClsAnsMach::OBJECT,
                                       oiAnswererOr,
                                       "Returns true iff either the reciever or the Boolean argument is true"));
      ansMachs->append(CConxClsAnsMach("xor:", CConxClsAnsMach::OBJECT,
                                       oiAnswererXOr,
                                       "Returns true iff either the reciever or the Boolean argument is true but both are not"));
    }
  }


private: // attributes
  Boole d;
  static Answerers *ansMachs;
}; // class CClsBoolean


//////////////////////////////////////////////////////////////////////////////
// Our equivalent to Smalltalk's UndefinedObject class, nil's class.
class CClsUndefinedObject : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsUndefinedObject")
  CLSNAME("UndefinedObject", "I have a very popular instance, `nil', that you will encounter when you have a variable that is not set.")
  CLSTYPE(CClsBase, CLS_UNDEFINED_OBJECT)
  STCLONE2(CClsUndefinedObject)
public:
  CClsUndefinedObject() { MMM("CClsUndefinedObject"); }
  CClsUndefinedObject(const CClsUndefinedObject &o)
    : CClsBase(o) { MMM("copy constructor"); }
  CClsUndefinedObject &operator=(const CClsUndefinedObject &o)
  {
    MMM("assignment operator");
    (void) CClsBase::operator=(o);
    return *this;
  }
  ~CClsUndefinedObject() { MMM("destructor"); /* cerr << "DLC destroying " << this << endl; */}

  // Uses CClsBase::sendMessage
  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      return CConxString("nil");
    }
  }
  int operator==(const CClsUndefinedObject &o)
  {
    return (isClassInstance() == o.isClassInstance());
  }
  int operator!=(const CClsUndefinedObject &o) { return !operator==(o); }
}; // class CClsUndefinedObject


//////////////////////////////////////////////////////////////////////////////
// Our equivalent to Smalltalk's Number class.
class CClsNumber : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsNumber")
  CLSNAME("Number", "I am a number.")
  CLSTYPE(CClsBase, CLS_NUMBER)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsNumber)
public:
  CClsNumber() { }
  CClsNumber(double od) { }
  CClsNumber(const CClsNumber &o) : CClsBase(o) { }
  CClsNumber &operator=(const CClsNumber &o)
  {
    (void) CClsBase::operator=(o);
    return *this;
  }

  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      return CConxString("an ") + getClsName();
    }
  }
  int operator==(const CClsNumber &o) { return 0; }
  int operator!=(const CClsNumber &o) { return !operator==(o); }
  
  virtual double getFloatValue() const
  {
    abort(); // we can't get an object instance
    return 0.0;
  }

private:
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
    }
  }

private: // attributes
  static Answerers *ansMachs;
}; // class CClsNumber


//////////////////////////////////////////////////////////////////////////////
// Our equivalent to Smalltalk's Float class.  Don't expect all the usual
// methods, though.
class CClsFloat : VIRT public CClsNumber {
  CCONX_CLASSNAME("CClsFloat")
  CLSNAME("Float", "I am a floating-point number.")
  CLSTYPE(CClsNumber, CLS_FLOAT)
  DEFAULT_SEND_MESSAGE(CClsNumber)
  ANSMACH_ANSWERS(CClsNumber)
  STCLONE2(CClsFloat)
public:
  CClsFloat() { d = 37.0; }
  CClsFloat(double od) { setValue(od); }
  CClsFloat(const CClsFloat &o) : CClsNumber(o) { d = o.d; }
  CClsFloat &operator=(const CClsFloat &o)
  {
    (void) CClsNumber::operator=(o);
    d = o.d;
    return *this;
  }

  double getFloatValue() const { return getValue(); }
  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      char s[250];
      (void) sprintf(s, "%g", getValue());
      return CConxString(s);
    }
  }
  void setValue(double od)
  {
    if (isClassInstance()) throw "set not for `Float'";
    d = od;
  }
  double getValue() const
  {
    if (isClassInstance()) throw "get not for `Float'";
    return d;
  }
  int operator==(const CClsFloat &o) { return isClassInstance() == o.isClassInstance() && d == o.d; }
  int operator!=(const CClsFloat &o) { return !operator==(o); }

protected:
  ANSWERER(CClsFloat, oiAnswererSet, oiActionSet); // DLC Is the semicolon OK?
  ErrType oiActionSet(CClsBase **result, CConxClsMessage &o)
  {
    CHECK_READ_ONLYNESS(result);
    NEED_N_FLOATS(1, fargv, o, result);
    setValue(fargv[0]);
    RETURN_THIS(result);
  }
  ANSWERER(CClsFloat, oiAnswererBetweenAnd, oiActionBetweenAnd); // DLC Is the semicolon OK?
  ErrType oiActionBetweenAnd(CClsBase **result, CConxClsMessage &o) const
  {
    NEED_N_FLOATS(2, fargv, o, result);
    RETURN_BOOLE(this->getValue() >= fargv[0] && this->getValue() <= fargv[1],
                 result);
  }
private:
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
      ansMachs->append(CConxClsAnsMach("between:and:",
                                       CConxClsAnsMach::OBJECT,
                                       oiAnswererBetweenAnd,
                                       "Returns true iff the inclusive range "
                                       "from the first argument to the second"
                                       " argument contains the receiver"));
      ansMachs->append(CConxClsAnsMach("set:",
                                       CConxClsAnsMach::OBJECT,
                                       oiAnswererSet,
                                       "Sets the receiver's value to the argument's floating-point value, coercing if possible (from a SmallInt, e.g.)"));

    }
  }

private: // attributes
  double d;
  static Answerers *ansMachs;
}; // class CClsFloat


//////////////////////////////////////////////////////////////////////////////
// Our small integer class.
class CClsSmallInt : VIRT public CClsNumber {
  CCONX_CLASSNAME("CClsSmallInt")
  CLSNAME("SmallInt", "I am a small signed integer; I fit inside a C long.")
  CLSTYPE(CClsNumber, CLS_SMALLINT)
  DEFAULT_SEND_MESSAGE(CClsNumber)
  ANSMACH_ANSWERS(CClsNumber)
  STCLONE2(CClsSmallInt)
public:
  CClsSmallInt() { d = 37; }
  CClsSmallInt(long od) { setValue(od); }
  CClsSmallInt(const CClsSmallInt &o) : CClsNumber(o) { d = o.d; }
  CClsSmallInt &operator=(const CClsSmallInt &o)
  {
    (void) CClsNumber::operator=(o);
    d = o.d;
    return *this;
  }

  double getFloatValue() const { return (double) getValue(); }
  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      char s[250];
      (void) sprintf(s, "%ld", getValue());
      return CConxString(s);
    }
  }
  void setValue(long od)
  {
    if (isClassInstance()) throw "set not for `SmallInt'";
    d = od;
  }
  long getValue() const
  {
    if (isClassInstance()) throw "get not for `SmallInt'";
    return d;
  }
  int operator==(const CClsSmallInt &o) { return isClassInstance() == o.isClassInstance() && d == o.d; }
  int operator!=(const CClsSmallInt &o) { return !operator==(o); }

protected:
  ANSWERER(CClsSmallInt, oiAnswererBetweenAnd, oiActionBetweenAnd); // DLC Is the semicolon OK?
  ErrType oiActionBetweenAnd(CClsBase **result, CConxClsMessage &o) const
  {
    assert(result != NULL); CClsBase *argv[2]; o.getBoundObjects(argv);
    for (int i = 0; i < 2; i++)
      ENSURE_KEYWD_TYPE(result, o, argv, i, CLS_SMALLINT, TRUE);
    RETURN_BOOLE(this->getValue() >= ((CClsSmallInt *)argv[0])->getValue()
                 && this->getValue() <= ((CClsSmallInt *)argv[1])->getValue(),
                 result);
  }
  NEW_OI_ANSWERER(CClsSmallInt);
private:
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
      ansMachs->append(CConxClsAnsMach("between:and:",
                                       CConxClsAnsMach::OBJECT,
                                       oiAnswererBetweenAnd,
                                       "Returns true iff the inclusive range "
                                       "from the first argument to the second"
                                       " argument contains the receiver"));
      ansMachs->append(CConxClsAnsMach("new",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererNew,
                                       "Returns a new SmallInt object instance"));
    }
  }

private: // attributes
  long d;
  static Answerers *ansMachs;
}; // class CClsSmallInt


//////////////////////////////////////////////////////////////////////////////
// Our Symbol class.
// `#uhp'
// `Point new x: 0.3 y: 0.2 model: #klein'
class CClsSymbol : VIRT public CClsCharacterArray {
  CCONX_CLASSNAME("CClsSymbol")
  CLSNAME("Symbol", "I am a special kind of string of characters that starts with an alphabetic character and contains only alphanumeric characters")
  CLSTYPE(CClsCharacterArray, CLS_SYMBOL)
  STCLONE2(CClsSymbol)
public:
  CClsSymbol() { }
  CClsSymbol(const CConxString &s) : CClsCharacterArray(s) { }
  CClsSymbol(const CClsSymbol &o)
    : CClsCharacterArray(o) { }
  CClsSymbol &operator=(const CClsSymbol &o)
  {
    (void) CClsCharacterArray::operator=(o);
    return *this;
  }

  // use CClsBase::printOn
  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      return CConxString("#") + getValue();
    }
  }

  int operator==(const CClsSymbol &o) {
    return CClsCharacterArray::operator==(o);
  }
  int operator!=(const CClsSymbol &o) { return !operator==(o); }
}; // class CClsSymbol


//////////////////////////////////////////////////////////////////////////////
// Our Array.
class CClsArray : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsArray")
  CLSNAME("Array", "I am an array that grows and holds objects of any type.")
  CLSTYPE(CClsBase, CLS_ARRAY)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE(CClsArray)
public:
  CClsArray() : contents() { init(); }
  CClsArray(const CClsArray &o)
    : CClsBase(o), contents(o.contents)
  {
    incrementAllUsersCounts();
    requiredType = o.requiredType;
  }
  CClsArray &operator=(const CClsArray &o)
  {
    (void) CClsBase::operator=(o);
    clear();
    contents = o.contents;
    incrementAllUsersCounts();
    requiredType = o.requiredType;
    return *this;
  }

  CClsBase *stCloneDeep() const
  {
    CClsArray *v = new CClsArray();
    v->requiredType = requiredType;
    if (v == NULL) OOM();
    for (size_t i = 0; i < contents.size(); i++) {
      v->contents.append((contents.get(i))->stCloneDeep());
    }
    v->incrementAllUsersCounts();
    return v;
  }

protected: // because checking for NULL pointers would be required.
  void incrementAllUsersCounts()
  {
    for (size_t i = 0; i < contents.size(); i++) {
      (contents.get(i))->incrementUsers();
    }
  }
  CClsArray(CClsBase *o1) : contents(o1) { init(); }
  CClsArray(CClsBase *o1, CClsBase *o2) : contents(o1, o2) { init(); }
  CClsArray(CClsBase *o1, CClsBase *o2, CClsBase *o3)
    : contents(o1, o2, o3) { init(); }
  CClsArray(CClsBase *o1, CClsBase *o2, CClsBase *o3, CClsBase *o4)
    : contents(o1, o2, o3, o4) { init(); }
  CClsArray(const CConxSimpleArray<CClsBase *> &s) : contents(s) { init(); }

public:
  ~CClsArray()
  {
    clear();
  }

  int operator==(const CClsArray &o) { return contents == o.contents
                                         && requiredType == o.requiredType; }
  int operator!=(const CClsArray &o) { return !operator==(o); }

  size_t numElements() const { return contents.size(); }
  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      CConxString s("#( ");
      for (size_t i = 0; i < contents.size(); i++)
        s += contents.get(i)->printString() + " ";
      return s + ")";
    }
  }

protected:
  void setRequiredType(ClsType t) { requiredType = t; }
  ClsType getRequiredType() { return requiredType; }

  CConxSimpleArray<CClsBase *> &getSimpleArray() { return contents; }
  const CConxSimpleArray<CClsBase *> &getSimpleArray() const { return contents; }

private:
  void init()
  {
    requiredType = CLS_OBJECT_BASE;
  }
  void clear()
  {
    for (size_t i = 0; i < contents.size(); i++) {
      CClsBase *r = contents.get(i);
      MAYBE_REMOVE_A_USER(r);
    }
  }

public:
  void makeReadOnly()
  {
    if (!isReadOnly()) {
      CClsBase::makeReadOnly();
      for (size_t i = 0; i < contents.size(); i++) {
        CClsBase *r = contents.get(i);
        assert(r != NULL);
        r->makeReadOnly();
      }
    }
  }
protected:
  ANSWERER(CClsArray, oiAnswererAt, oiActionAt);
  ErrType oiActionAt(CClsBase **result, CConxClsMessage &o) const
  {
    MMM("ErrType oiActionAt(CClsBase **result, CConxClsMessage &o) const");
    ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsSmallInt, argv, o);
    if (contents.size() == 0)
      RETURN_ERROR_RESULT(result, "this array is empty");
    long x = argv[0]->getValue();
    
    if (x > (long)contents.size()) x = contents.size();
    if (x < 1) x = 1;
    RETURN_EXISTING(result, contents.get(x - 1));
  }
  ANSWERER(CClsArray, oiAnswererYourself, oiActionYourself);
  ErrType oiActionYourself(CClsBase **result, CConxClsMessage &o)
  {
    MMM("ErrType oiActionYourself(CClsBase **result, CConxClsMessage &o) const");
    RETURN_THIS(result);
  }
  ANSWERER(CClsArray, oiAnswererAtPut, oiActionAtPut);
  ErrType oiActionAtPut(CClsBase **result, CConxClsMessage &o)
  {
    MMM("ErrType oiActionAtPut(CClsBase **result, CConxClsMessage &o)");
    CHECK_READ_ONLYNESS(result);
    assert(result != NULL); CClsBase *argv[2]; o.getBoundObjects(argv);
    ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_SMALLINT, TRUE);
    if (getRequiredType() != CLS_OBJECT_BASE) {
      ENSURE_KEYWD_TYPE(result, o, argv, 1, getRequiredType(), TRUE);
    }

    long x = ((CClsSmallInt *)argv[0])->getValue();
    
    if (x < 1) x = 1;

    if (x <= (long) contents.size()) {
      CClsBase *r = contents.get(x - 1);
      MAYBE_REMOVE_A_USER(r);
      argv[1]->incrementUsers();
      contents.atPut(x - 1, argv[1]);
    } else {
      if (x == (long) contents.size() + 1) {
        argv[1]->incrementUsers();
        contents.append(argv[1]);
      } else {
        RETURN_ERROR_RESULT(result, "not yet implemented, should grow I guess -- I need an integer class first.");
        // grow the array
        // DLC implement, fill with `nil's
      }
    }
    RETURN_EXISTING(result, argv[1]);
  }
  ANSWERER(CClsArray, oiAnswererAddFirst, oiActionAddFirst);
  ErrType oiActionAddFirst(CClsBase **result, CConxClsMessage &o)
  {
    MMM("ErrType oiActionAddFirst(CClsBase **result, CConxClsMessage &o)");
    CHECK_READ_ONLYNESS(result);
    assert(result != NULL); CClsBase *argv[1]; o.getBoundObjects(argv);
    if (getRequiredType() != CLS_OBJECT_BASE) {
      ENSURE_KEYWD_TYPE(result, o, argv, 0, getRequiredType(), TRUE);
    }

    argv[0]->incrementUsers();
    contents.prepend(argv[0]);
    RETURN_EXISTING(result, argv[0]);
  }
  ANSWERER(CClsArray, oiAnswererAddLast, oiActionAddLast);
  ErrType oiActionAddLast(CClsBase **result, CConxClsMessage &o)
  {
    MMM("ErrType oiActionAddLast(CClsBase **result, CConxClsMessage &o)");
    CHECK_READ_ONLYNESS(result);
    assert(result != NULL); CClsBase *argv[1]; o.getBoundObjects(argv);
    if (getRequiredType() != CLS_OBJECT_BASE) {
      ENSURE_KEYWD_TYPE(result, o, argv, 0, getRequiredType(), TRUE);
    }

    argv[0]->incrementUsers();
    contents.append(argv[0]);
    RETURN_EXISTING(result, argv[0]);
  }
  ANSWERER(CClsArray, oiAnswererSize, oiActionSize);
  ErrType oiActionSize(CClsBase **result, CConxClsMessage &o) const
  {
    RETURN_NEW_RESULT(result, new CClsSmallInt(contents.size()));
  }
  ANSWERER(CClsArray, ciAnswererWithWithWithWith, ciActionWithWithWithWith);
  ErrType ciActionWithWithWithWith(CClsBase **result, CConxClsMessage &o) const
  {
    CClsBase *argv[4]; o.getBoundObjects(argv);
    RETURN_NEW_RESULT(result,
                      new CClsArray(argv[0], argv[1], argv[2], argv[3]));
  }
  ANSWERER(CClsArray, ciAnswererWithWithWith, ciActionWithWithWith);
  ErrType ciActionWithWithWith(CClsBase **result, CConxClsMessage &o) const
  {
    CClsBase *argv[3]; o.getBoundObjects(argv);
    RETURN_NEW_RESULT(result, new CClsArray(argv[0], argv[1], argv[2]));
  }
  ANSWERER(CClsArray, ciAnswererWithWith, ciActionWithWith);
  ErrType ciActionWithWith(CClsBase **result, CConxClsMessage &o) const
  {
    CClsBase *argv[2]; o.getBoundObjects(argv);
    RETURN_NEW_RESULT(result, new CClsArray(argv[0], argv[1]));
  }
  ANSWERER(CClsArray, ciAnswererWith, ciActionWith);
  ErrType ciActionWith(CClsBase **result, CConxClsMessage &o) const
  {
    CClsBase *argv[1]; o.getBoundObjects(argv);
    RETURN_NEW_RESULT(result, new CClsArray(argv[0]));
  }
  NEW_OI_ANSWERER(CClsArray);
private:
  static void initializeAnsweringMachines();

private: // attributes
  ClsType requiredType;
  CConxSimpleArray<CClsBase *> contents;
  static Answerers *ansMachs;
}; // class CClsArray


//////////////////////////////////////////////////////////////////////////////
// Our interface to things that we just shouldn't have access to, like exiting
// and reserved words and ... // DLC do reserved words.
class CClsSystem : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsSystem")
  CLSNAME("System", "I am the interface to things like exiting the program.")
  CLSTYPE(CClsBase, CLS_SYSTEM)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsSystem)
public:
  CClsSystem() { }
  CClsSystem(const CClsSystem &o) : CClsBase(o) { }
  CClsSystem &operator=(const CClsSystem &o)
  {
    (void) CClsBase::operator=(o);
    return *this;
  }

  CConxString printString() const
  {
    if (isClassInstance())
      return getClsName();
    else {
      return CConxString("an ") + getClsName();
    }
  }
  int operator==(const CClsSystem &o) { return 0; }
  int operator!=(const CClsSystem &o) { return !operator==(o); }

protected:
  ANSWERER(CClsSystem, iAnswererExit, iActionExit);
  ErrType iActionExit(CClsBase **result, CConxClsMessage &o)
  {
    // DLC do final cleanup to find bugs.
    exit(0);
  }
  ANSWERER(CClsSystem, iAnswererHelpMe, iActionHelpMe);
  ErrType iActionHelpMe(CClsBase **result, CConxClsMessage &o)
  {
    RETURN_NEW_RESULT(result, new CClsStringLiteral("This is the Smalltalk-80-ish system that " PACKAGE " uses to give you incredible expressive power (um, one day...).  The system's documentation is in the Texinfo document that you'll find bundled with the source, which is freely available since " PACKAGE " is GPL'ed.  To get up to speed more quickly, download and check out GNU Smalltalk, which is full-featured and better-documented.\nAnother good source of help is by typing `Object help', or `System help', or `2 help', or `0.3 help', e.g.  This gives you information about the class of the receiver, such as which methods work.  There are no binary methods in this system, and you cannot extend it unless you alter the C++ source code.")); // DLC point to sourceforge.

    // DLC print strings with formatting! Let tab cause tabbing that far and then another tab for indenting.
  }
private:
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
      ansMachs->append(CConxClsAnsMach("exit",
                                       CConxClsAnsMach::CLASS,
                                       iAnswererExit,
                                       "Exits the system"));
      ansMachs->append(CConxClsAnsMach("helpMe",
                                       CConxClsAnsMach::CLASS,
                                       iAnswererHelpMe,
                                       "Gets high-level help"));
    }
  }

private: // attributes
  static Answerers *ansMachs;
}; // class CClsSystem


// Any derived class is also covered by this:
OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CClsBase);


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////

inline
Boole CClsBase::answers(const CConxString &msg) const
  // Returns TRUE if the message msg is answered by this Smalltalk class or
  // any of its superclasses.  msg should not lead with a hash mark.
  // CClsFloat answers "between:and:", e.g., as well as "class"
{
  ANSMACH_ANSWERS_INNER();
  // DLC we need lots of test cases for is..., not..., printString, class, ...!
  return FALSE;
  // DLC update when we add more methods.
}

inline CClsBase::ErrType
CClsBase::sendMessage(CClsBase **result, CConxClsMessage &o)
{
  TRY_TO_ANSWER_BY_MACHINE();

  if (o.isKeywordMessage()) {
    RETURN_ERROR_RESULT(result, "unknown keyword message");
  } else {
    assert(o.isUnaryMessage());
    RETURN_ERROR_RESULT(result, "unknown unary message");
  }
}

inline CClsBase::ErrType
CClsBase::iActionHelp(CClsBase **result, CConxClsMessage &o)
{
  // DLC test case.
  Answerers *a = getAnswerers();
  if (a == NULL) {
    RETURN_NEW_RESULT(result,
                      new CClsStringLiteral(getClassHelpMessage(*this)));
  } else {
    RETURN_NEW_RESULT(result,
                      new CClsStringLiteral(getCompleteHelpMessage(*this, *a)));
  }
}

inline CClsBase::ErrType
CClsBase::ciActionClass(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  RETURN_NEW_RESULT(result,
                    new CClsStringLiteral(CConxString(getClsName())
                                          + " class"));
}

inline CClsBase::ErrType
CClsBase::oiActionClass(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.

  /* DLC Class Metaclass ?? */
  RETURN_NEW_RESULT(result, new CClsStringLiteral(getClsName()));
}

inline CClsBase::ErrType
CClsBase::oiActionSet(CClsBase **result, CConxClsMessage &o)
{
  CHECK_READ_ONLYNESS(result);
  CClsBase *argv[1]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, getType(), TRUE);
  try {
    setToCopyOf(*argv[0]);
  } catch (const char *s) {
    RETURN_ERROR_RESULT(result, s);
  }
  RETURN_THIS(result);
}

inline CClsBase::ErrType
CClsBase::ciActionSuperClass(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  if (getType() == CLS_OBJECT_BASE) {
    RETURN_NEW_RESULT(result, new CClsError("There is no super-class; this is the root of the class hierarchy"));
  }
  RETURN_NEW_RESULT(result,
     new CClsStringLiteral(CConxString(getClsNameByType(getSuperClassType()))
                           + " class"));
}

inline CClsBase::ErrType
CClsBase::oiActionSuperClass(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  if (getType() == CLS_OBJECT_BASE) {
    RETURN_NEW_RESULT(result, new CClsError("There is no super-class; this is the root of the class hierarchy"));
  }
  RETURN_NEW_RESULT(result,
    new CClsStringLiteral(CConxString(getClsNameByType(getSuperClassType()))
                          + " class"));
}

inline CClsBase::ErrType
CClsBase::oiActionMakeReadOnly(CClsBase **result, CConxClsMessage &o)
{
  CHECK_READ_ONLYNESS(result);
  makeReadOnly(); // this is virtual.
  RETURN_THIS(result);
}

inline CClsBase::ErrType
CClsBase::oiActionIsReadOnly(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result, new CClsBoolean(isReadOnly()));
}

inline CClsBase::ErrType
CClsBase::iActionPrintString(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  // DLC add equals methods to most or all Smalltalk classes.

  RETURN_NEW_RESULT(result, new CClsStringLiteral(printString()));
}

inline CClsBase::ErrType
CClsBase::iActionIsClass(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  RETURN_BOOLE(isClassInstance(), result);
}

inline CClsBase::ErrType
CClsBase::iActionClone(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  RETURN_NEW_RESULT(result, stClone());
}

inline CClsBase::ErrType
CClsBase::oiActionCloneDeep(CClsBase **result, CConxClsMessage &o) const
{
  MMM("ErrType oiActionCloneDeep(CClsBase **result, CConxClsMessage &o) const");
  INVARIANTS();
  // DLC test case.
  RETURN_NEW_RESULT(result, stCloneDeep());
}

inline CClsBase::ErrType
CClsBase::oiActionExactlyEquals(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  CClsBase *argv[1]; o.getBoundObjects(argv);
  RETURN_BOOLE(argv[0] == this, result);
}

inline void CClsArray::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ansMachs->append(CConxClsAnsMach("with:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererWith,
                                     "Returns a new Array object instance with one value"));
    ansMachs->append(CConxClsAnsMach("with:with:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererWithWith,
                                     "Returns a new Array object instance with two values"));
    ansMachs->append(CConxClsAnsMach("with:with:with:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererWithWithWith,
                                     "Returns a new Array object instance with three values"));
    ansMachs->append(CConxClsAnsMach("with:with:with:with:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererWithWithWithWith,
                                     "Returns a new Array object instance with four values"));
    ansMachs->append(CConxClsAnsMach("new",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererNew,
                                     "Returns a new Array object instance (that grows)"));
    ansMachs->append(CConxClsAnsMach("size",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererSize,
                                     "Returns the size of the receiver"));
// DLC good help tip:    ansMachs->append(CConxClsAnsMach("set:",
//                                      CConxClsAnsMach::OBJECT,
//                                      oiAnswererSet,
//                                      "Sets the receiver to point to the same things to which the Array object instance argument points"));
    ansMachs->append(CConxClsAnsMach("at:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererAt,
                                     "Returns the element at the position specified by the strictly positive integer argument"));
    ansMachs->append(CConxClsAnsMach("at:put:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererAtPut,
                                     "Sets the element at the position specified by the strictly positive integer argument to `at:' to the object that is the `put:' argument"));
    ansMachs->append(CConxClsAnsMach("addFirst:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererAddFirst,
                                     "Adds the argument to the front of the array, growing automatically"));
    ansMachs->append(CConxClsAnsMach("addLast:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererAddLast,
                                     "Adds the argument to the end of the array, growing automatically"));
    ansMachs->append(CConxClsAnsMach("yourself",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererYourself,
                                     "Returns the array itself (i.e., the receiver), useful because #at:put: and #addFirst: return their arguments, not their receivers"));

  }
}

inline size_t CClsBase::getMessageArity(const CConxString &msg)
{
  size_t arity = 1;
  const char *val = msg.getString();
  if (*val == '\0') return 0;
  if (!IS_ID_FIRST_CHAR(*val)) return 0;
  while (*(++val) != '\0') {
    if (*val == ':') {
      if (*(val+1) == ':')
        return 0;
      ++arity;
    } else if (!IS_ID_CHAR(*val)) {
      return 0;
    }
  }
  if (arity > 1 && *(val - 1) != ':') // "hi:there" is not valid.
    return 0;
  return arity;
}

inline CConxString CClsBase::clsTypeToString(ClsType c)
{
  switch (c) {
  case CLS_OBJECT_BASE:
    return CConxString("CLS_OBJECT_BASE");
  case CLS_UNDEFINED_OBJECT:
    return CConxString("CLS_UNDEFINED_OBJECT");
  case CLS_BOOLEAN:
    return CConxString("CLS_BOOLEAN");
  case CLS_ERROR:
    return CConxString("CLS_ERROR");
  case CLS_FLOAT:
    return CConxString("CLS_FLOAT");
  case CLS_MODELIDENTIFIER:
    return CConxString("CLS_MODELIDENTIFIER");
  case CLS_COLOR:
    return CConxString("CLS_COLOR");
  case CLS_CHARACTERARRAY:
    return CConxString("CLS_CHARACTERARRAY");
  case CLS_ARRAY:
    return CConxString("CLS_ARRAY");
  case CLS_SMALLINT:
    return CConxString("CLS_SMALLINT");
  case CLS_DRAWABLE:
    return CConxString("CLS_DRAWABLE");
  case CLS_NUMBER:
    return CConxString("CLS_NUMBER");
  case CLS_SYSTEM:
    return CConxString("CLS_SYSTEM");
  case CLS_CANVAS:
    return CConxString("CLS_CANVAS");
  case CLS_LINE:
    return CConxString("CLS_LINE");
  case CLS_CIRCLE:
    return CConxString("CLS_CIRCLE");
    // DLC NEWSTCLASS
  case CLS_SYMBOL:
    return CConxString("CLS_SYMBOL");
  case CLS_STRING:
    return CConxString("CLS_STRING");
  case CLS_VARIABLE:
    return CConxString("CLS_VARIABLE");
  case CLS_POINT:
    return CConxString("CLS_POINT");
  case CLS_INVALID:
    return CConxString("CLS_INVALID");
  default:
    abort(); // DLC
    return CConxString("implementation bug j02");
  }
}

inline CClsBase::ErrType
CClsCharacterArray::oiActionLength(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result,
                    new CClsSmallInt((long)(getValue().getLength())));
}

ANSWERER_IMPL(CClsBase, iAnswererHelp, iActionHelp);
ANSWERER_IMPL(CClsBase, ciAnswererClass, ciActionClass);
ANSWERER_IMPL(CClsBase, oiAnswererClass, oiActionClass);
ANSWERER_IMPL(CClsBase, oiAnswererSet, oiActionSet);
ANSWERER_IMPL(CClsBase, ciAnswererSuperClass, ciActionSuperClass);
ANSWERER_IMPL(CClsBase, oiAnswererSuperClass, oiActionSuperClass);
ANSWERER_IMPL(CClsBase, oiAnswererMakeReadOnly, oiActionMakeReadOnly);
ANSWERER_IMPL(CClsBase, oiAnswererIsReadOnly, oiActionIsReadOnly);
ANSWERER_IMPL(CClsBase, iAnswererPrintString, iActionPrintString);
ANSWERER_IMPL(CClsBase, iAnswererIsClass, iActionIsClass);
ANSWERER_IMPL(CClsBase, iAnswererClone, iActionClone);
ANSWERER_IMPL(CClsBase, oiAnswererCloneDeep, oiActionCloneDeep);
ANSWERER_IMPL(CClsBase, oiAnswererExactlyEquals, oiActionExactlyEquals);


#define TYPE_TESTS_IMPLS(cn, clsType) \
   TEST_TYPE_IMPL(iActionIs ## cn, clsType); \
   TEST_NOT_TYPE_IMPL(iActionNot ## cn, clsType); \
   ANSWERER_IMPL(CClsBase, iAnswererIs ## cn, iActionIs ## cn); \
   ANSWERER_IMPL(CClsBase, iAnswererNot ## cn, iActionNot ## cn)

TYPE_TESTS_IMPLS(Nil, CLS_UNDEFINED_OBJECT);
TYPE_TESTS_IMPLS(Float, CLS_FLOAT);
TYPE_TESTS_IMPLS(Symbol, CLS_SYMBOL);
TYPE_TESTS_IMPLS(String, CLS_STRING);
TYPE_TESTS_IMPLS(ParseError, CLS_ERROR);
TYPE_TESTS_IMPLS(ModelIdentifier, CLS_MODELIDENTIFIER);
TYPE_TESTS_IMPLS(Color, CLS_COLOR);
TYPE_TESTS_IMPLS(CharacterArray, CLS_CHARACTERARRAY);
TYPE_TESTS_IMPLS(Array, CLS_ARRAY);
TYPE_TESTS_IMPLS(SmallInt, CLS_SMALLINT);
TYPE_TESTS_IMPLS(Drawable, CLS_DRAWABLE);
TYPE_TESTS_IMPLS(Number, CLS_NUMBER);
TYPE_TESTS_IMPLS(System, CLS_SYSTEM);
TYPE_TESTS_IMPLS(Canvas, CLS_CANVAS);
TYPE_TESTS_IMPLS(Line, CLS_LINE);
TYPE_TESTS_IMPLS(Circle, CLS_CIRCLE);
// DLC NEWSTCLASS


#endif // GPLCONX_STCOMMON_CXX_H
