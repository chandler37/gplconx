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
  Implementation of C++ classes in `sthypell.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sthypell.hh"
#include "stfloat.hh"
#include "sterror.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsHypEllipse::ansMachs = NULL;

#define FOREACH_MEMBER_G(obj, var) \
  for (size_t var = 0; var < (obj).numMembers(); var++)

#define FOREACH_MEMBER(var) FOREACH_MEMBER_G(*this, var)

#ifndef NDEBUG
#define ENSURE_CONTENTS() \
  FOREACH_MEMBER(asTmpI) assert(getNthMember(asTmpI) != NULL)
#else
#define ENSURE_CONTENTS() /* do nothing */
#endif


CF_INLINE
CClsHypEllipse::CClsHypEllipse(CClsPoint *Focus1, CClsPoint *Focus2,
                               CClsNumber *Dist)
{
  init();
  conicDistance = Dist;
  focus1 = Focus1;
  focus2 = Focus2;
  incAllUserCounts();
  ensureValidity();
}

CF_INLINE
CClsHypEllipse::CClsHypEllipse(const CClsDrawable &od)
  : CClsDrawable(od)
{
  init();
  ensureValidity();
}

CF_INLINE
CClsHypEllipse::CClsHypEllipse(const CConxDwGeomObj &od)
  : CClsDrawable(od)
{
  init();
  setValue(od);
}

CF_INLINE
CClsHypEllipse::CClsHypEllipse(const CClsHypEllipse &o)
  : CClsDrawable(o)
{
  MMM("copy constructor");
  init(); uninitializedCopy(o);
}

NF_INLINE
void CClsHypEllipse::init()
{
  FOREACH_MEMBER(i) setNthMemberPointer(i, NULL);
  savedArtist = NULL;
}

NF_INLINE
void CClsHypEllipse::incAllUserCounts()
{
  FOREACH_MEMBER(i) {
    CClsBase *j = getNthMember(i);
    if (j != NULL) j->incrementUsers();
  }
}

NF_INLINE
CClsHypEllipse &CClsHypEllipse::operator=(const CClsHypEllipse &o)
{
  (void) CClsDrawable::operator=(o);
  clear();
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
CClsBase *CClsHypEllipse::stCloneDeep() const
{
  CClsDrawable *j = (CClsDrawable *) CClsDrawable::stCloneDeep();
  if (j == NULL) OOM();
  CClsHypEllipse *v = new CClsHypEllipse(*j);
  delete j;
  if (v == NULL) OOM();
  v->clear();
  v->conicDistance = (CClsNumber *) conicDistance->stCloneDeep();
  v->focus1 = (CClsPoint *) focus1->stCloneDeep();
  v->focus2 = (CClsPoint *) focus2->stCloneDeep();
  v->incAllUserCounts();
  return v;
}

NF_INLINE
CConxString CClsHypEllipse::printString() const
{
  if (isClassInstance()) {
    return getClsName();
  } else {
    ENSURE_CONTENTS();
    CConxString s = getClsName();
    s += "(Focus1: " + focus1->printString() +
      ", Focus2: " + focus2->printString() +
      ", K: " + conicDistance->printString() + ")";
    s += " -- " + CClsDrawable::printString();
    return s;
  }
}

NF_INLINE
void CClsHypEllipse::setValue(const CConxHypEllipse &o)
{
  clear();
  conicDistance = new CClsFloat(o.getScalar());
  focus1 = new CClsPoint(o.getFocus1());
  focus2 = new CClsPoint(o.getFocus2());
  FOREACH_MEMBER(i) {
    if (getNthMember(i) == NULL) OOM();
  }
  incAllUserCounts();
  invalidateSavedArtist();
}

NF_INLINE
void CClsHypEllipse::setValue(const CConxDwGeomObj &o)
{
  CClsDrawable::setValue(o);
  const CConxSimpleArtist *oPt = o.getGeomObj();
  if (oPt == NULL || oPt->getSAType() != CConxSimpleArtist::SA_HYPELLIPSE)
    throw "That's just nasty.";
  setValue(*oPt);
}

NF_INLINE
CConxHypEllipse CClsHypEllipse::getValue() const throw(CClsError *)
{
  ENSURE_CONTENTS();
  // This may throw if the foci are invalid:
  return CConxHypEllipse(focus1->getValue(), focus2->getValue(),
                         conicDistance->getFloatValue());
}

NF_INLINE
const CConxArtist *CClsHypEllipse::getArtist() const
{
  MMM("virtual const CConxArtist *getArtist() const");

  // See comment in CClsPoint::getArtist for more:
  invalidateSavedArtist();
  if (savedArtist == NULL) {
    savedArtist = new CConxDwGeomObj(getDwValue());
    if (savedArtist == NULL) OOM();
  }
  return savedArtist;
}

NF_INLINE
CConxDwGeomObj CClsHypEllipse::getDwValue() const throw(CClsError *)
{
  CConxDwGeomObj p(getValue()); // may throw
  affectValue(p);
  return p;
}

NF_INLINE
int CClsHypEllipse::operator==(const CClsHypEllipse &o)
{
  return (getValue() == o.getValue() && getDwValue() == o.getDwValue());
}

NF_INLINE
void CClsHypEllipse::clear()
{
  FOREACH_MEMBER(i) {
    CClsBase *j = getNthMember(i);
    MAYBE_REMOVE_A_USER(j);
  }
  invalidateSavedArtist();
}

NF_INLINE
void CClsHypEllipse::makeReadOnly()
{
  ENSURE_CONTENTS();
  CClsDrawable::makeReadOnly();
  FOREACH_MEMBER(i) {
    getNthMember(i)->makeReadOnly();
  }
}

NF_INLINE
void CClsHypEllipse::invalidateSavedArtist() const
{
  if (savedArtist != NULL)
    delete savedArtist;
  savedArtist = NULL;
}

NF_INLINE
void CClsHypEllipse::ensureValidity()
{
  if (conicDistance == NULL) {
    conicDistance = new CClsFloat(1.0);
    conicDistance->incrementUsers();
  }
  if (focus1 == NULL) {
    focus1 = new CClsPoint();
    focus1->incrementUsers();
  }
  if (focus2 == NULL) {
    focus2 = new CClsPoint();
    focus2->incrementUsers();
  }
  FOREACH_MEMBER(i) if (getNthMember(i) == NULL) OOM();
  ENSURE_CONTENTS();
}

NF_INLINE
void CClsHypEllipse::uninitializedCopy(const CClsHypEllipse &o)
{
  // We are a container of one Number and two Points, so
  // we will both point to these contents.

  invalidateSavedArtist();

  FOREACH_MEMBER_G(o, i) {
    assert(o.getNthMember(i) != NULL);
    const CClsBase *jC = o.getNthMember(i);
    CClsBase *j;
    CONST_CAST(CClsBase *, j, jC);
    setNthMemberPointer(i, j);
  }

  incAllUserCounts();
}

NF_INLINE CClsBase::ErrType
CClsHypEllipse::ciActionF1F2K(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[3]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_POINT, TRUE);
  ENSURE_KEYWD_TYPE(result, o, argv, 1, CLS_POINT, TRUE);
  ENSURE_KEYWD_TYPE(result, o, argv, 2, CLS_NUMBER, TRUE);
  RETURN_NEW_RESULT(result, new CClsHypEllipse((CClsPoint *)argv[0],
                                               (CClsPoint *)argv[1],
                                               (CClsNumber *)argv[2]));
}

NF_INLINE
void CClsHypEllipse::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ST_METHOD(ansMachs, "new", CLASS, ciAnswererNew,
              "Returns a new object instance of a hyperbola/ellipse in Hyperbolic geometry" DRAWABLE_STR);
    ST_METHOD(ansMachs, "f1:f2:K:", CLASS, ciAnswererF1F2K,
              "Returns a new, set object instance of a hyperbola/ellipse in Hyperbolic geometry" DRAWABLE_STR);
    ADD_ANS_GETTER("f1", Focus1);
    ADD_ANS_GETTER("f2", Focus2);
    ADD_ANS_GETTER("K", K);
    ADD_ANS_GETTER("f1:", Focus1Colon);
    ADD_ANS_GETTER("f2:", Focus2Colon);
    ADD_ANS_GETTER("K:", KColon);
  }
}

NF_INLINE
size_t CClsHypEllipse::numMembers() const
{
  return 3;
}

NF_INLINE
CClsBase *CClsHypEllipse::getNthMember(size_t i)
{
  assert(focus1 != NULL);
  assert(focus2 != NULL);
  assert(conicDistance != NULL);

  switch (i) {
  case 0: return focus1;
  case 1: return focus2;
  case 2: return conicDistance;
  default: throw "never!!!!";
  }
}

NF_INLINE
const CClsBase *CClsHypEllipse::getNthMember(size_t i) const
{
  const CClsHypEllipse *tC = this;
  CClsHypEllipse *t;
  CONST_CAST(CClsHypEllipse *, t, tC);
  return t->getNthMember(i);
}

NF_INLINE
void CClsHypEllipse::setNthMemberPointer(size_t i, CClsBase *mm)
{
  switch (i) {
  case 0: assert(mm == NULL || mm->isType(CLS_POINT));
    focus1 = (CClsPoint *) mm; break;
  case 1: assert(mm == NULL || mm->isType(CLS_POINT));
    focus2 = (CClsPoint *) mm; break;
  case 2: assert(mm == NULL || mm->isType(CLS_NUMBER));
    conicDistance = (CClsNumber *) mm; break;
  default: throw "never!!!!";
  }
}
