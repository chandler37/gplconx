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
  C++ Smalltalkish Object class -- the base class for all Smalltalkish
  classes.

//  DLC consider an operator ::= that does a set, rather than changing
    the pointer.  This makes `r := 0.4 ... r set: 0.3' become the
    easier `r := 0.4 ... r ::= 0.3'
*/

#ifndef GPLCONX_STOBJECT_CXX_H
#define GPLCONX_STOBJECT_CXX_H 1

#include "gcobject.hh"
#include "stmacros.hh"
#include "sth_answ.hh"
#include "CString.hh"

class CConxClsAnsMach;
class CConxClsMessage;

// Declaring these in advance is unnecessary but makes compiler messages
// more meaningful.
class CClsUndefinedObject;
class CClsBoolean;
class CClsFloat;
class CClsModelIdentifier;
class CClsColor;
class CClsCharacterArray;
class CClsArray;
class CClsSmallInt;
class CClsDrawable;
class CClsNumber;
class CClsSystem;
class CClsCanvas;
class CClsLine;
class CClsCircle;
class CClsParabola;
class CClsHypEllipse;
class CClsError;
class CClsSymbol;
class CClsString;
class CClsPoint;
// DLC NEWSTCLASS



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
    CLS_PARABOLA,
    CLS_HYPELLIPSE,
    // DLC NEWSTCLASS
    CLS_ERROR,
    CLS_SYMBOL,             /* #symbol */
    CLS_STRING,             /* 'string literal' */
    CLS_VARIABLE,           /* a variable that is bound immediately.
                               This is not really a Smalltalk class.
                            */
    CLS_POINT,

    CLS_INVALID             /* This must be the last value. */
  };

public:
  CClsBase();
  CClsBase(const CClsBase &o);
  CClsBase &operator=(const CClsBase &o);

  ostream &printOn(ostream &o) const;
  int operator==(const CClsBase &o);
  int operator!=(const CClsBase &o) { return !operator==(o); }

  // In Smalltalk, we have class instances and object instances.  When you
  // call "String new", you get an object instance "''", but when you call
  // "'hi' new", you get a parse error.
  void setClassInstance(Boole isClass);
  Boole isClassInstance() const;
  virtual void setReadOnly(Boole readOnly);
  Boole isReadOnly() const;

public: // virtual functions

  // For all below, the messages are interpreted differently depending on
  // isClassInstance().

  // Also, we define arity in a funny way.  The arity of a unary message
  // is 1; the arity of a binary message (of which we have none) is 2,
  // so the arity of a keyword message that has n keywords is 2+n.
  // Hence, the arity of "new:" is 3, the arity of "new" is 1, the arity of
  // "x:y:model:" is 5, etc.


  virtual Boole answers(const CConxString &msg) const;

  virtual Answerers *getAnswerers();
  virtual void setToCopyOf(const CClsBase &o) throw(const char *);

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
  virtual ClsType getType() const;
  static ClsType sGetType() { return CLS_OBJECT_BASE; }
  virtual Boole isType(ClsType m) const;
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
  virtual const char *getClsComment() const;
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
  virtual CConxString printString() const;

  // Cloning preserves the receiver and creates a new instance identical to
  // the receiver.  A pointer to a newly allocated instance is returned.
  virtual CClsBase *stClone() const;

  // stClone() makes a shallow copy, i.e. a copy in which all CClsBase
  // objects are shared, i.e. both the original and the copy point to
  // the same things.  This only matters for Drawables, Arrays, and other
  // containers.
  virtual CClsBase *stCloneDeep() const;
  // stCloneDeep() makes a deep copy, i.e. a copy in which all CClsBase
  // objects are copied.  This means that a container will have new, copied
  // contents.

  virtual void makeReadOnly();


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
  TYPE_TESTS_DECLS(Point, CLS_POINT);
  TYPE_TESTS_DECLS(Parabola, CLS_PARABOLA);
  TYPE_TESTS_DECLS(HypEllipse, CLS_HYPELLIPSE);
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


// Any derived class is also covered by this:
OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CClsBase);

#endif // GPLCONX_STOBJECT_CXX_H
