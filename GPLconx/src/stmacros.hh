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
  Macros used to implement the Smalltalkish C++ classes.
*/

#ifndef GPLCONX_STMACROS_CXX_H
#define GPLCONX_STMACROS_CXX_H 1

#include <ctype.h>
#include <strstream.h>

#include "sthelper.hh"

//////////////////////////////////////////////////////////////////////////////
// Helper macros:
#define SET_RESULT(result, obj) if ((result) != NULL) *(result) = obj

#define RETURN_NEW_RESULT(result, newObj) \
   do { \
     SET_RESULT(result, newObj); return OK_NEW_THING; /* DLC check oom GC */ \
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

class CClsBase;

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

class CClsError; // : public CClsBase;

#define SET_ERROR_RESULT(result, specificError) \
        SET_RESULT(result, new CClsError(specificError)) /* DLC GC this will cause a throw in removeUser */

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

#define ENSURE_ALL_N_ARE_OF_TYPE(N, typ, Targv, o) \
    CClsBase *tmpArgv[N]; typ *(Targv)[N]; (o).getBoundObjects(tmpArgv); \
    for (int tmpI = 0; tmpI < N; tmpI++) { \
      ENSURE_KEYWD_TYPE(result, (o), tmpArgv, tmpI, typ::sGetType(), TRUE); \
      (Targv)[tmpI] = (typ *)tmpArgv[tmpI]; \
    }

#define ENSURE_SINGLE_ARG_IS_OF_TYPE(typ, Targv, o) \
             ENSURE_ALL_N_ARE_OF_TYPE(1, typ, Targv, o)

#define RETURN_EXISTING(result, obj) \
   do { SET_RESULT(result, obj); return OK; } while (0)

#define ANS_GETTER(obj, fn, attribute) \
  ErrType oiAction ## fn(CClsBase **result, CConxClsMessage &o) \
  /* This is not const because you can modify the returned object */ \
  { \
    RETURN_EXISTING(result, attribute); \
  } \
  ANSWERER(obj, oiAnswerer ## fn, oiAction ## fn)

#define CHECK_READ_ONLYNESS(result) \
  do { \
    if (isReadOnly()) RETURN_ERROR_RESULT(result, "receiver is read-only"); \
  } while(0)

#define RETURN_THIS(result) RETURN_EXISTING(result, this)

#define REMOVE_A_USER(obj) \
  do { \
    if ((obj)->decrementUsers() == 0) delete (obj); \
    (obj) = NULL; \
  } while (0)

#define MAYBE_REMOVE_A_USER(obj) \
  do { \
    if ((obj) != NULL) REMOVE_A_USER(obj); \
  } while (0)

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

#define IS_ID_FIRST_CHAR(c) (isupper(c) || islower(c))
#define IS_ID_CHAR(c) (IS_ID_FIRST_CHAR(c) || isdigit(c))

#define PRINTSTRING "printString" /* standard Smalltalk unary method. */

#define DEFAULT_PRINTSTRING(d) \
      ostrstream ostr; \
      ostr << d << ends; \
      const char *val = ostr.str(); \
      CConxString sval = val; \
      delete [] val; \
      return sval

class CClsBoolean;

#define RETURN_BOOLE(val, result) \
   do { \
     SET_RESULT(result, new CClsBoolean((val) ? TRUE : FALSE));  /* DLC GC this will cause a throw in removeUsers */ \
     return CClsBase::OK_NEW_THING; \
   } while (0)

class CClsFloat;

#define RETURN_FLOAT(result, fl) \
   do { \
     SET_RESULT(result, new CClsFloat(fl));  /* DLC GC this will cause a throw in removeUsers */ \
     return CClsBase::OK_NEW_THING; \
   } while (0)

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

class CClsNumber;

#define NEED_N_FLOATS(N, fargv, o, result) \
  CClsBase *tMpArgv[N]; double (fargv)[N]; (o).getBoundObjects(tMpArgv); \
  for (int ai = 0; ai < N; ai++) { \
    ENSURE_KEYWD_TYPE(result, (o), tMpArgv, ai, CLS_NUMBER, TRUE); \
    (fargv)[ai] = ((CClsNumber *)tMpArgv[ai])->getFloatValue(); \
  }

#define ST_METHOD(am, stMethodName, classOrObject, functionName, comment) \
        (am)->append(CConxClsAnsMach(stMethodName, \
                                     CConxClsAnsMach:: ## classOrObject, \
                                     functionName, \
                                     comment))

#define ADD_ANS_GETTER(mname, fn) \
   ST_METHOD(ansMachs, mname, OBJECT, oiAnswerer ## fn, "Returns the " mname)

#define ADD_ANS_SETTER(mname, fn) \
   ST_METHOD(ansMachs, mname, OBJECT, oiAnswerer ## fn, \
             "Sets the `" mname "' field; returns the receiver")

// The following makes implementing actions easy, but may be hard to
// unnecessarily hard to understand: DLC
#define ACTION_WITH_NO_ARGS_IMPL(cls, constness, func, innerAction) \
NF_INLINE CClsBase::ErrType \
cls ## :: ## func(CClsBase **result, CConxClsMessage &o) constness \
{ innerAction }

// The following macros assume that the result pointer is named `result':
#define RETURN_FLOAT_R(m) RETURN_FLOAT(result, m)
#define RETURN_BOOLE_R(m) RETURN_BOOLE(result, m)


#endif // GPLCONX_STMACROS_CXX_H
