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
  Implementation of C++ classes in `stcommon.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stobject.hh"
#include "clsmgr.hh"
#include "stcommon.hh"
#include "stconx.hh"

Answerers *CClsBase::ansMachs = NULL;
// DLC NEWSTCLASS in the appropriate `.cc' file

NF_INLINE
const char *CClsBase::getClsNameByType(ClsType c)
{
  switch (c) {
  case CLS_OBJECT_BASE:
    return CClsBase::sGetClsName();
  case CLS_UNDEFINED_OBJECT:
    return CClsUndefinedObject::sGetClsName();
  case CLS_BOOLEAN:
    return CClsBoolean::sGetClsName();
  case CLS_ERROR:
    return CClsError::sGetClsName();
  case CLS_FLOAT:
    return CClsFloat::sGetClsName();
  case CLS_MODELIDENTIFIER:
    return CClsModelIdentifier::sGetClsName();
  case CLS_COLOR:
    return CClsColor::sGetClsName();
  case CLS_CHARACTERARRAY:
    return CClsCharacterArray::sGetClsName();
  case CLS_ARRAY:
    return CClsArray::sGetClsName();
  case CLS_SMALLINT:
    return CClsSmallInt::sGetClsName();
  case CLS_DRAWABLE:
    return CClsDrawable::sGetClsName();
  case CLS_NUMBER:
    return CClsNumber::sGetClsName();
  case CLS_SYSTEM:
    return CClsSystem::sGetClsName();
  case CLS_CANVAS:
    return CClsCanvas::sGetClsName();
  case CLS_LINE:
    return CClsLine::sGetClsName();
  case CLS_CIRCLE:
    return CClsCircle::sGetClsName();
  case CLS_PARABOLA:
    return CClsParabola::sGetClsName();
  case CLS_HYPELLIPSE:
    return CClsHypEllipse::sGetClsName();
  case CLS_EQDISTCURVE:
    return CClsEqDistCurve::sGetClsName();
// DLC NEWSTCLASS
  case CLS_SYMBOL:
    return CClsSymbol::sGetClsName();
  case CLS_STRING:
    return CClsStringLiteral::sGetClsName();
  case CLS_POINT:
    return CClsPoint::sGetClsName();
  case CLS_VARIABLE:
  default:
    CXXFATAL("never should this happen.");
  }
}

NF_INLINE
void CClsBase::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    // DLC check oom

    // DLC for efficiency, don't allow `Boolean true' and `Boolean false',
    // force copying `true' and `false'.
    ST_CMETHOD(ansMachs, "makeReadOnly", "setting",
               OBJECT, oiAnswererMakeReadOnly,
               "Makes the object instance read-only; if the object is a container, such as Array, Canvas, Drawable, Point, Line, etc., then the contents become read-only also.  This is irreversible and comes with some performance advantages.  Clone a read-only object to get a read-write equivalent");
    ST_CMETHOD(ansMachs, "dependsOn:", "Private",
               OBJECT, oiAnswererDependsOn,
               "Returns true if the receiver is a container that contains the argument or an object that contains the argument");
    ST_CMETHOD_BOTH(ansMachs, "respondsTo:", "testing functionality",
                    iAnswererRespondsTo,
                    "Returns true if the receiver responds to the message, which is represented by a #symbol or #symbol:like:this:");
    ST_CMETHOD_BOTH(ansMachs, "invoke:", "method invocation",
                    iAnswererInvoke,
                    "Invokes on behalf of the receiver the unary method that has that name.  This indirection may prove useful in writing scripts.");
    ST_CMETHOD_BOTH(ansMachs, "invoke:passing:", "method invocation",
                    iAnswererInvokeA,
                    "Invokes on behalf of the receiver the method that has that name.  The arguments are in the Array object instance that is the argument to #passing; its size and the number of keywords in the method name must match.  This indirection may prove useful in writing scripts.");
    ST_CMETHOD(ansMachs, "isReadOnly", "setting",
               OBJECT, oiAnswererIsReadOnly,
               "Returns a Boolean to indicate the receiver's read-only status");
    ST_CMETHOD_BOTH(ansMachs, "superClass", "metaclasses",
                    oiAnswererSuperClass,
                    "Returns the class from which the receiver is derived");
    ST_CMETHOD(ansMachs, "class", "metaclasses",
               CLASS, ciAnswererClass,
               "Returns the class of which the receiver is a class instance");
    ST_CMETHOD(ansMachs, "class", "metaclasses",
               OBJECT, oiAnswererClass,
               "Returns the class of which the receiver is an object instance");
    ST_CMETHOD_BOTH(ansMachs, PRINTSTRING, "printing", iAnswererPrintString,
                    "Returns a human-readable String object instance representation");
    ST_CMETHOD_BOTH(ansMachs, "isClass", "testing functionality",
                    iAnswererIsClass,
                    "Returns true if the receiver is a class instance rather than an object instance");
    ST_CMETHOD(ansMachs, "cloneDeep", "instance creation",
               OBJECT, oiAnswererCloneDeep,
               "Returns a copy of the receiver with, in the case or containers, all contents replaced by cloneDeep copies");
    ST_CMETHOD_BOTH(ansMachs, "clone", "instance creation", iAnswererClone,
                    "Returns an identical copy of the receiver (not the receiver itself) that shares any contents if the receiver is a container");
    ST_CMETHOD_BOTH(ansMachs, "=:", "comparison", iAnswererEquals,
                    "Returns true iff the receiver is functionally the same as the argument.  This can be a complex test.");
    ST_CMETHOD_BOTH(ansMachs, "~=:", "comparison", iAnswererNotEquals,
                    "Returns true iff the receiver is not functionally the same as the argument.  This can be a complex test.");
    ST_CMETHOD(ansMachs, "==:", "comparison",
               OBJECT, iAnswererExactlyEquals,
               "Returns true iff the receiver is precisely the same object instance as the argument (iff the pointers are equal, for you C folk) -- this is the identity relationship, and testing it is very quick.");
    ST_CMETHOD(ansMachs, "~~:", "comparison",
               OBJECT, iAnswererNotExactlyEquals,
               "Returns true iff the receiver is not precisely the same object instance as the argument (iff the pointers are inequal, for you C folk) -- this is the negation of identity relationship, and testing it is very quick.");
    ST_CMETHOD(ansMachs, "set:", "setting",
               OBJECT, oiAnswererSet,
               "Sets the receiver's value to a copy of the argument's value");
    ST_CMETHOD(ansMachs, "help", "help",
               OBJECT, iAnswererHelp,
               "Returns help on using the receiver");
    ST_CMETHOD(ansMachs, "help", "help",
               CLASS, iAnswererHelp,
               "Returns help on using the receiver");
    // DLC add respondsTo: aSymbol

// Handle `nil isNil' vs. `UndefinedObject isNil' with isClassInstance() test
#define TEST_TYPE(msgString, clsType, answerer) \
 ST_CMETHOD(ansMachs, msgString, "testing functionality", \
            OBJECT, answerer, \
            "Returns true if the receiver " msgString); \
 ST_CMETHOD(ansMachs, msgString, "testing functionality", \
            CLASS, answerer, \
            "Returns true if the receiver " msgString)

#define TEST_NOT_TYPE(msgString, clsType, answerer) \
 ST_CMETHOD(ansMachs, msgString, "testing functionality", \
            OBJECT, answerer, \
            "Returns true if the receiver is " msgString); \
 ST_CMETHOD(ansMachs, msgString, "testing functionality", \
            CLASS, answerer, \
            "Returns true if the receiver is " msgString)

#define HANDLE_CLASS_TYPE_TESTS(s, clsTyp, fn) \
    TEST_TYPE("is" s, clsTyp, iAnswererIs ## fn); \
    TEST_NOT_TYPE("not" s, clsTyp, iAnswererNot ## fn)


    HANDLE_CLASS_TYPE_TESTS("Nil", CLS_UNDEFINED_OBJECT, Nil);
    // same as:
    // TEST_TYPE("isNil", CLS_UNDEFINED_OBJECT, iAnswererIsNil);
    // TEST_NOT_TYPE("notNil", CLS_UNDEFINED_OBJECT, iAnswererNotNil);

    HANDLE_CLASS_TYPE_TESTS("Float", CLS_FLOAT, Float);
    HANDLE_CLASS_TYPE_TESTS("Symbol", CLS_SYMBOL, Symbol);
    HANDLE_CLASS_TYPE_TESTS("String", CLS_STRING, String);
    HANDLE_CLASS_TYPE_TESTS("ParseError", CLS_ERROR, ParseError);
    HANDLE_CLASS_TYPE_TESTS("ModelIdentifier", CLS_MODELIDENTIFIER, ModelIdentifier);
    HANDLE_CLASS_TYPE_TESTS("Color", CLS_COLOR, Color);
    HANDLE_CLASS_TYPE_TESTS("CharacterArray", CLS_CHARACTERARRAY, CharacterArray);
    HANDLE_CLASS_TYPE_TESTS("Array", CLS_ARRAY, Array);
    HANDLE_CLASS_TYPE_TESTS("SmallInt", CLS_SMALLINT, SmallInt);
    HANDLE_CLASS_TYPE_TESTS("Drawable", CLS_DRAWABLE, Drawable);
    HANDLE_CLASS_TYPE_TESTS("Number", CLS_NUMBER, Number);
    HANDLE_CLASS_TYPE_TESTS("System", CLS_SYSTEM, System);
    HANDLE_CLASS_TYPE_TESTS("Canvas", CLS_CANVAS, Canvas);
    HANDLE_CLASS_TYPE_TESTS("Line", CLS_LINE, Line);
    HANDLE_CLASS_TYPE_TESTS("Circle", CLS_CIRCLE, Circle);
    HANDLE_CLASS_TYPE_TESTS("Point", CLS_POINT, Point);
    HANDLE_CLASS_TYPE_TESTS("Parabola", CLS_PARABOLA, Parabola);
    HANDLE_CLASS_TYPE_TESTS("HypEllipse", CLS_HYPELLIPSE, HypEllipse);
    HANDLE_CLASS_TYPE_TESTS("EqDistCurve", CLS_EQDISTCURVE, EqDistCurve);
// DLC NEWSTCLASS
  }
}

// DLC Linux Class Library ForEach Documentation is weak.
// This helps us access the variable dictionany in CConxClsManager:
class CConxForEachGetVars : public LForEach {
public:
  CConxForEachGetVars(CClsArray *m) { assert(m != NULL); arr = m; }
  ~CConxForEachGetVars() { }
  void process(void *value)
  {
    CConxClsManager::VarDictElem *v = (CConxClsManager::VarDictElem *)value;
    assert(arr != NULL);
    arr->append(new CClsSymbol(v->key)); // Entering GC
  }

private:
  CClsArray *arr;
}; // class CConxForEachGetVars

NF_INLINE
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

NF_INLINE CClsBase::ErrType
CClsBase::sendMessage(CClsBase **result, CConxClsMessage &o)
{
  TRY_TO_ANSWER_BY_MACHINE();
  RETURN_ERROR_RESULT(result, o.getUnknownMessageComplaint());
}

NF_INLINE CClsBase::ErrType
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

NF_INLINE CClsBase::ErrType
CClsBase::ciActionClass(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  RETURN_NEW_RESULT(result,
                    new CClsStringLiteral(CConxString(getClsName())
                                          + " class"));
}

NF_INLINE CClsBase::ErrType
CClsBase::oiActionClass(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.

  /* DLC Class Metaclass ?? */
  RETURN_NEW_RESULT(result, new CClsStringLiteral(getClsName()));
}

NF_INLINE CClsBase::ErrType
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

NF_INLINE CClsBase::ErrType
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

NF_INLINE CClsBase::ErrType
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

NF_INLINE CClsBase::ErrType
CClsBase::oiActionMakeReadOnly(CClsBase **result, CConxClsMessage &o)
{
  CHECK_READ_ONLYNESS(result);
  makeReadOnly(); // this is virtual.
  RETURN_THIS(result);
}

NF_INLINE CClsBase::ErrType
CClsBase::oiActionIsReadOnly(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result, new CClsBoolean(isReadOnly()));
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionPrintString(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  // DLC add equals methods to most or all Smalltalk classes.

  RETURN_NEW_RESULT(result, new CClsStringLiteral(printString()));
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionIsClass(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  RETURN_BOOLE_R(isClassInstance());
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionClone(CClsBase **result, CConxClsMessage &o) const
{
  // DLC test case.
  RETURN_NEW_RESULT(result, stClone());
}

NF_INLINE CClsBase::ErrType
CClsBase::oiActionCloneDeep(CClsBase **result, CConxClsMessage &o) const
{
  MMM("ErrType oiActionCloneDeep(CClsBase **result, CConxClsMessage &o) const");
  INVARIANTS();
  // DLC test case.
  RETURN_NEW_RESULT(result, stCloneDeep());
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionEquals(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[1]; o.getBoundObjects(argv);
  RETURN_BOOLE(equals(argv[0]), result);
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionNotEquals(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[1]; o.getBoundObjects(argv);
  RETURN_BOOLE(!equals(argv[0]), result);
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionExactlyEquals(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[1]; o.getBoundObjects(argv);
  RETURN_BOOLE(argv[0] == this, result);
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionNotExactlyEquals(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[1]; o.getBoundObjects(argv);
  RETURN_BOOLE(argv[0] != this, result);
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionRespondsTo(CClsBase **result, CConxClsMessage &o) const
{
  ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsSymbol, argv, o);
  RETURN_BOOLE(answers(argv[0]->getValue()), result);
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionInvoke(CClsBase **result, CConxClsMessage &o)
{
  // Do not CHECK_READ_ONLYNESS here.
  ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsSymbol, argv, o);
  if (argv[0]->getValue() == "invoke") {
    RETURN_ERROR_RESULT(result, "#invoke: cannot invoke itself; picture a dragon eating its own tail");
  } else {
    CConxClsMessage msg(argv[0]->getValue());
    return sendMessage(result, msg);
  }
}

NF_INLINE CClsBase::ErrType
CClsBase::oiActionDependsOn(CClsBase **result, CConxClsMessage &o) const
{
  ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsBase, argv, o);
  RETURN_BOOLE_R(dependsOn(argv[0]));
}

NF_INLINE CClsBase::ErrType
CClsBase::iActionInvokeA(CClsBase **result, CConxClsMessage &o)
{
  // Do not CHECK_READ_ONLYNESS here.
  CClsBase *argv[2]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_SYMBOL, TRUE);
  ENSURE_KEYWD_TYPE(result, o, argv, 1, CLS_ARRAY, TRUE);
  CClsSymbol *methodSym = (CClsSymbol *)argv[0];
  CClsArray *args = (CClsArray *)argv[1];
  const CConxString &method = methodSym->getValue();
  if (method == "invoke:passing:") {
    RETURN_ERROR_RESULT(result, "#invoke:passing: cannot invoke itself; picture a dragon eating its own tail");
  } else if (args->numElements() != methodSym->numberOfKeywords()) {
    RETURN_ERROR_RESULT(result, "#invoke:passing: requires an Array with as many element as the method name has colons");
  } else {
    size_t ne = args->numElements();
    CConxClsMessage msg;
    if (ne == 0) {
      msg.setToUnaryMessage(method);
    } else {
      CConxClsKeywordMessage kmsg;
      for (size_t i = 0; i < ne; i++) {
        kmsg.appendKeyedArg(CConxClsKeyedArg(methodSym->nthKeyword(i),
                                             args->get(i)));
      }
      msg.setToKeywordMessage(kmsg);
    }
    return sendMessage(result, msg);
  }
}

NF_INLINE
size_t CClsBase::getMessageArity(const CConxString &msg)
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

NF_INLINE CConxString CClsBase::clsTypeToString(ClsType c)
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
  case CLS_PARABOLA:
    return CConxString("CLS_PARABOLA");
  case CLS_HYPELLIPSE:
    return CConxString("CLS_HYPELLIPSE");
  case CLS_EQDISTCURVE:
    return CConxString("CLS_EQDISTCURVE");
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

ANSWERER_IMPL(CClsBase, iAnswererEquals, iActionEquals);
ANSWERER_IMPL(CClsBase, iAnswererNotEquals, iActionNotEquals);
ANSWERER_IMPL(CClsBase, oiAnswererDependsOn, oiActionDependsOn);
ANSWERER_IMPL(CClsBase, iAnswererRespondsTo, iActionRespondsTo);
ANSWERER_IMPL(CClsBase, iAnswererInvoke, iActionInvoke);
ANSWERER_IMPL(CClsBase, iAnswererInvokeA, iActionInvokeA);
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
ANSWERER_IMPL(CClsBase, iAnswererExactlyEquals, iActionExactlyEquals);
ANSWERER_IMPL(CClsBase, iAnswererNotExactlyEquals, iActionNotExactlyEquals);


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
TYPE_TESTS_IMPLS(Point, CLS_POINT);
TYPE_TESTS_IMPLS(Parabola, CLS_PARABOLA);
TYPE_TESTS_IMPLS(HypEllipse, CLS_HYPELLIPSE);
TYPE_TESTS_IMPLS(EqDistCurve, CLS_EQDISTCURVE);
// DLC NEWSTCLASS


CF_INLINE CClsBase::CClsBase()
{
  // No, this is not arbitrary.  Number one, it is more common, but more
  // importantly, this means that in
  // CClsFloat::CClsFloat(double) we don't have to say
  // `isObjectInstance = TRUE;'.  There are no constructors that make
  // class instances (yet).
  isObjectInstance = isModifiable = TRUE;
  INVARIANTS();
}

CF_INLINE
CClsBase::CClsBase(const CClsBase &o)
  : CConxGCObject(o)
{
  isObjectInstance = o.isObjectInstance;
  isModifiable = TRUE; // a copy is not read-only.
  INVARIANTS();
}

NF_INLINE
CClsBase &CClsBase::operator=(const CClsBase &o)
{
  (void) CConxGCObject::operator=(o);
  isObjectInstance = o.isObjectInstance;
  isModifiable = TRUE; // a copy is read-only.
  INVARIANTS();
  return *this;
}

NF_INLINE
ostream &CClsBase::printOn(ostream &o) const
{
  INVARIANTS();
  o << "<" << className() << " "
    << ((isClassInstance()) ? "object" : "class")
    << " instance value:" << printString() << ">";
  return o;
}

NF_INLINE
int CClsBase::operator==(const CClsBase &o) const
{
  INVARIANTS();
  return (isClassInstance() == o.isClassInstance()
          && isReadOnly() == o.isReadOnly());
  // read-onlyness does not affect Smalltalk equality but does affect C++
  // equality.
}

NF_INLINE
void CClsBase::setClassInstance(Boole isClass)
{
  INVARIANTS();
  isObjectInstance = !isClass;
}

NF_INLINE
Boole CClsBase::isClassInstance() const
{
  INVARIANTS();
  return !isObjectInstance;
}

NF_INLINE
void CClsBase::setReadOnly(Boole readOnly)
{
  INVARIANTS();
  isModifiable = !readOnly;
}

NF_INLINE
Boole CClsBase::isReadOnly() const
{
  INVARIANTS();
  return !isModifiable;
}

NF_INLINE
Answerers *CClsBase::getAnswerers()
{
  INVARIANTS();
  return ansMachs;
}

NF_INLINE
void CClsBase::setToCopyOf(const CClsBase &o) throw(const char *)
{
  INVARIANTS();
  (void) operator=(o);
}

NF_INLINE
CClsBase::ClsType CClsBase::getType() const
{
  INVARIANTS();
  return CLS_OBJECT_BASE;
}

NF_INLINE
Boole CClsBase::isType(ClsType m) const
{
  INVARIANTS();
  return m == CLS_OBJECT_BASE;
}

NF_INLINE
const char *CClsBase::getClsComment() const
{
  INVARIANTS();
  return "I am the root of the " PACKAGE " class hierarchy.  All other classes are derived from me.";
}

NF_INLINE
CConxString CClsBase::printString() const
{
  INVARIANTS();
  if (isClassInstance())
    return getClsName();
  else
    return CConxString("an ") + getClsName();
}

NF_INLINE
CClsBase *CClsBase::stClone() const
{
  INVARIANTS();
  return new CClsBase(*this);
}

NF_INLINE
CClsBase *CClsBase::stCloneDeep() const
{
  LLL("CClsBase *CClsBase::stCloneDeep()");
  INVARIANTS();
  return stClone();
}

NF_INLINE
void CClsBase::makeReadOnly()
{
  INVARIANTS();
  setReadOnly(TRUE);
}
