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
  Implementation of C++ classes in `stgarray.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stgarray.hh"
#include "sterror.hh"
#include "stsmalli.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsArray::ansMachs = NULL;

NF_INLINE
void CClsArray::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ST_METHOD(ansMachs, "with:", CLASS, ciAnswererWith,
              "Returns a new Array object instance with one value");
    ST_METHOD(ansMachs, "with:with:", CLASS, ciAnswererWithWith,
              "Returns a new Array object instance with two values");
    ST_METHOD(ansMachs, "with:with:with:", CLASS, ciAnswererWithWithWith,
              "Returns a new Array object instance with three values");
    ST_METHOD(ansMachs, "with:with:with:with:", CLASS, ciAnswererWithWithWithWith,
              "Returns a new Array object instance with four values");
    ST_METHOD(ansMachs, "new", CLASS, ciAnswererNew,
              "Returns a new Array object instance (that grows)");
    ST_METHOD(ansMachs, "size", OBJECT, oiAnswererSize,
              "Returns the size of the receiver");
// DLC good help tip:    ST_METHOD(ansMachs, "set:",
//                                      OBJECT,
//                                      oiAnswererSet,
// "Sets the receiver to point to the same things to which the Array object instance argument points");
    ST_METHOD(ansMachs, "at:", OBJECT, oiAnswererAt,
              "Returns the element at the position specified by the strictly positive integer argument");
    ST_METHOD(ansMachs, "at:put:", OBJECT, oiAnswererAtPut,
              "Sets the element at the position specified by the strictly positive integer argument to `at:' to the object that is the `put:' argument");
    ST_METHOD(ansMachs, "addFirst:", OBJECT, oiAnswererAddFirst,
              "Adds the argument to the front of the array, growing automatically");
    ST_METHOD(ansMachs, "addLast:", OBJECT, oiAnswererAddLast,
              "Adds the argument to the end of the array, growing automatically");
    ST_METHOD(ansMachs, "yourself", OBJECT, oiAnswererYourself,
              "Returns the array itself (i.e., the receiver), useful because #at:put: and #addFirst: return their arguments, not their receivers");

  }
}

NF_INLINE
CClsBase::ErrType 
CClsArray::oiActionAddLast(CClsBase **result, CConxClsMessage &o)
{
  MMM("ErrType oiActionAddLast(CClsBase **result, CConxClsMessage &o)");
  CHECK_READ_ONLYNESS(result);
  assert(result != NULL); CClsBase *argv[1]; o.getBoundObjects(argv);
  if (getRequiredType() != CLS_OBJECT_BASE) {
    ENSURE_KEYWD_TYPE(result, o, argv, 0, getRequiredType(), TRUE);
  }

  append(argv[0]);
  RETURN_EXISTING(result, argv[0]);
}

NF_INLINE
CClsBase::ErrType 
CClsArray::oiActionYourself(CClsBase **result, CConxClsMessage &o)
{
  MMM("ErrType oiActionYourself(CClsBase **result, CConxClsMessage &o) const");
  RETURN_THIS(result);
}

NF_INLINE
CClsBase::ErrType 
CClsArray::oiActionAt(CClsBase **result, CConxClsMessage &o)
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

NF_INLINE
CClsBase::ErrType 
CClsArray::oiActionAtPut(CClsBase **result, CConxClsMessage &o)
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
      append(argv[1]);
    } else {
      RETURN_ERROR_RESULT(result, "not yet implemented, should grow I guess -- I need an integer class first.");
      // grow the array
      // DLC implement, fill with `nil's
    }
  }
  RETURN_EXISTING(result, argv[1]);
}

NF_INLINE
CClsBase::ErrType 
CClsArray::oiActionAddFirst(CClsBase **result, CConxClsMessage &o)
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

NF_INLINE
CClsBase::ErrType 
CClsArray::oiActionSize(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result, new CClsSmallInt(contents.size()));
}

NF_INLINE
CClsBase::ErrType 
CClsArray::ciActionWithWithWithWith(CClsBase **result,
                                    CConxClsMessage &o) const
{
  CClsBase *argv[4]; o.getBoundObjects(argv);
  RETURN_NEW_RESULT(result,
                    new CClsArray(argv[0], argv[1], argv[2], argv[3]));
}

NF_INLINE
CClsBase::ErrType 
CClsArray::ciActionWithWithWith(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[3]; o.getBoundObjects(argv);
  RETURN_NEW_RESULT(result, new CClsArray(argv[0], argv[1], argv[2]));
}

NF_INLINE
CClsBase::ErrType 
CClsArray::ciActionWithWith(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[2]; o.getBoundObjects(argv);
  RETURN_NEW_RESULT(result, new CClsArray(argv[0], argv[1]));
}

NF_INLINE
CClsBase::ErrType 
CClsArray::ciActionWith(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[1]; o.getBoundObjects(argv);
  RETURN_NEW_RESULT(result, new CClsArray(argv[0]));
}

NF_INLINE
void CClsArray::makeReadOnly()
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

NF_INLINE
void CClsArray::init()
{
  requiredType = CLS_OBJECT_BASE;
}

NF_INLINE
void CClsArray::clear()
{
  for (size_t i = 0; i < contents.size(); i++) {
    CClsBase *r = contents.get(i);
    MAYBE_REMOVE_A_USER(r);
  }
}

CF_INLINE
CClsArray::CClsArray(const CClsArray &o)
  : CClsBase(o), contents(o.contents)
{
  incrementAllUsersCounts();
  requiredType = o.requiredType;
}

NF_INLINE
CClsArray &CClsArray::operator=(const CClsArray &o)
{
  (void) CClsBase::operator=(o);
  clear();
  contents = o.contents;
  incrementAllUsersCounts();
  requiredType = o.requiredType;
  return *this;
}

NF_INLINE
void CClsArray::append(CClsBase *n)
{
  if (n == NULL)
    throw "NULL arg is not okay, most likely we are out of memory";
  n->incrementUsers();
  contents.append(n);
}

NF_INLINE
void CClsArray::incrementAllUsersCounts()
{
  for (size_t i = 0; i < contents.size(); i++) {
    (contents.get(i))->incrementUsers();
  }
}

NF_INLINE
CConxString CClsArray::printString() const
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

NF_INLINE
CClsBase *CClsArray::stCloneDeep() const
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
