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

#include "stcommon.hh"
#include "sthelper.hh"
#include "clsmgr.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsBase::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsFloat::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsBoolean::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsCharacterArray::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsArray::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsSmallInt::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsNumber::ansMachs = NULL;
CConxOwnerArray<CConxClsAnsMach> *CClsSystem::ansMachs = NULL;
// DLC NEWSTCLASS

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
    ansMachs = new CConxOwnerArray<CConxClsAnsMach>();
    // DLC check oom

    // DLC for efficiency, don't allow `Boolean true' and `Boolean false',
    // force copying `true' and `false'.
    ansMachs->append(CConxClsAnsMach("makeReadOnly", CConxClsAnsMach::OBJECT,
                                     oiAnswererMakeReadOnly,
                                     "Makes the object instance read-only; if the object is a container, such as Array, Canvas, Drawable, Point, Line, etc., then the contents become read-only also.  This is irreversible and comes with some performance advantages.  Clone a read-only object to get a read-write equivalent"));
    ansMachs->append(CConxClsAnsMach("isReadOnly", CConxClsAnsMach::OBJECT,
                                     oiAnswererIsReadOnly,
                                     "Returns a Boolean to indicate the receiver's read-only status"));
    ansMachs->append(CConxClsAnsMach("superClass", CConxClsAnsMach::CLASS,
                                     ciAnswererSuperClass,
                                     "Returns the class from which the receiver is derived"));
    ansMachs->append(CConxClsAnsMach("superClass", CConxClsAnsMach::OBJECT,
                                     oiAnswererSuperClass,
                                     "Returns the class from which the receiver is derived"));
    ansMachs->append(CConxClsAnsMach("class", CConxClsAnsMach::CLASS,
                                     ciAnswererClass,
                                     "Returns the class of which the receiver is a class instance"));
    ansMachs->append(CConxClsAnsMach("class", CConxClsAnsMach::OBJECT,
                                     oiAnswererClass,
                                     "Returns the class of which the receiver is an object instance"));
    ansMachs->append(CConxClsAnsMach(PRINTSTRING, CConxClsAnsMach::OBJECT,
                                     iAnswererPrintString,
                                     "Returns a human-readable String object instance representation"));
    ansMachs->append(CConxClsAnsMach(PRINTSTRING, CConxClsAnsMach::CLASS,
                                     iAnswererPrintString,
                                     "Returns a human-readable String object instance representation"));
    ansMachs->append(CConxClsAnsMach("isClass", CConxClsAnsMach::CLASS,
                                     iAnswererIsClass,
                                     "Returns true if the receiver is a class instance rather than an object instance"));
    ansMachs->append(CConxClsAnsMach("isClass", CConxClsAnsMach::OBJECT,
                                     iAnswererIsClass,
                                     "Returns true if the receiver is a class instance rather than an object instance"));
    ansMachs->append(CConxClsAnsMach("cloneDeep", CConxClsAnsMach::OBJECT,
                                     oiAnswererCloneDeep,
                                     "Returns a copy of the receiver with, in the case or containers, all contents replaced by cloneDeep copies"));
    ansMachs->append(CConxClsAnsMach("clone", CConxClsAnsMach::CLASS,
                                     iAnswererClone,
                                     "Returns an identical copy of the receiver (not the receiver itself) that shares any contents if the receiver is a container"));
    ansMachs->append(CConxClsAnsMach("clone", CConxClsAnsMach::OBJECT,
                                     iAnswererClone,
                                     "Returns an identical copy of the receiver (not the receiver itself) that shares any contents if the receiver is a container"));
    ansMachs->append(CConxClsAnsMach("exactlyEquals:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererExactlyEquals,
                                     "Returns true iff the receiver is precisely the same object instance as the argument (iff the pointers are equal, for you C folk)"));
    ansMachs->append(CConxClsAnsMach("set:", CConxClsAnsMach::OBJECT,
                                     oiAnswererSet,
                                     "Sets the receiver's value to a copy of the argument's value"));
    ansMachs->append(CConxClsAnsMach("help", CConxClsAnsMach::OBJECT,
                                     iAnswererHelp,
                                     "Returns help on using the receiver"));
    ansMachs->append(CConxClsAnsMach("help", CConxClsAnsMach::CLASS,
                                     iAnswererHelp,
                                     "Returns help on using the receiver"));


// Handle `nil isNil' vs. `UndefinedObject isNil' with isClassInstance() test
#define TEST_TYPE(msgString, clsType, answerer) \
 ansMachs->append(CConxClsAnsMach(msgString, CConxClsAnsMach::OBJECT, \
                                  answerer, \
                             "Returns true if the receiver " msgString)); \
 ansMachs->append(CConxClsAnsMach(msgString, CConxClsAnsMach::CLASS, \
                                  answerer, \
                                  "Returns true if the receiver " msgString))

#define TEST_NOT_TYPE(msgString, clsType, answerer) \
 ansMachs->append(CConxClsAnsMach(msgString, CConxClsAnsMach::OBJECT, \
                                  answerer, \
                            "Returns true if the receiver is " msgString)); \
 ansMachs->append(CConxClsAnsMach(msgString, CConxClsAnsMach::CLASS, \
                                  answerer, \
                              "Returns true if the receiver is " msgString))

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
// DLC NEWSTCLASS
  }
}

OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CClsBase)
