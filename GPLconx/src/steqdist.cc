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
  Implementation of C++ classes in `steqdist.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "steqdist.hh"
#include "stfloat.hh"
#include "sterror.hh"

Answerers *CClsEqDistCurve::ansMachs = NULL;

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
CClsEqDistCurve::CClsEqDistCurve(CClsLine *l, CClsNumber *Dist)
{
  init();
  conicDistance = Dist;
  line = l;
  incAllUserCounts();
  ensureValidity();
}

CF_INLINE
CClsEqDistCurve::CClsEqDistCurve(const CClsDrawable &od)
  : CClsDrawable(od)
{
  init();
  ensureValidity();
}

CF_INLINE
CClsEqDistCurve::CClsEqDistCurve(const CConxDwGeomObj &od)
  : CClsDrawable(od)
{
  init();
  setValue(od);
}

CF_INLINE
CClsEqDistCurve::CClsEqDistCurve(const CClsEqDistCurve &o)
  : CClsDrawable(o)
{
  MMM("copy constructor");
  init(); uninitializedCopy(o);
}

NF_INLINE
void CClsEqDistCurve::init()
{
  FOREACH_MEMBER(i) setNthMemberPointer(i, NULL);
  savedArtist = NULL;
}

NF_INLINE
void CClsEqDistCurve::incAllUserCounts()
{
  FOREACH_MEMBER(i) {
    CClsBase *j = getNthMember(i);
    if (j != NULL) j->incrementUsers();
  }
}

NF_INLINE
CClsEqDistCurve &CClsEqDistCurve::operator=(const CClsEqDistCurve &o)
{
  (void) CClsDrawable::operator=(o);
  clear();
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
CClsBase *CClsEqDistCurve::stCloneDeep() const
{
  CClsDrawable *j = (CClsDrawable *) CClsDrawable::stCloneDeep();
  if (j == NULL) OOM();
  CClsEqDistCurve *v = new CClsEqDistCurve(*j);
  delete j;
  if (v == NULL) OOM();
  v->clear();
  v->conicDistance = (CClsNumber *) conicDistance->stCloneDeep();
  v->line = (CClsLine *) line->stCloneDeep();
  v->incAllUserCounts();
  return v;
}

NF_INLINE
CConxString CClsEqDistCurve::printString() const
{
  if (isClassInstance()) {
    return getClsName();
  } else {
    ENSURE_CONTENTS();
    CConxString s = getClsName();
    s += "(Line: " + line->printString() +
      ", K: " + conicDistance->printString() + ")";
    s += " -- " + CClsDrawable::printString();
    return s;
  }
}

NF_INLINE
void CClsEqDistCurve::setValue(const CConxEqDistCurve &o)
{
  clear();
  conicDistance = new CClsFloat(o.getDistance());
  line = new CClsLine(o.getLine());
  FOREACH_MEMBER(i) {
    if (getNthMember(i) == NULL) OOM();
  }
  incAllUserCounts();
  invalidateSavedArtist();
}

NF_INLINE
void CClsEqDistCurve::setValue(const CConxDwGeomObj &o)
{
  CClsDrawable::setValue(o);
  const CConxSimpleArtist *oPt = o.getGeomObj();
  if (oPt == NULL || oPt->getSAType() != CConxSimpleArtist::SA_EQDISTCURVE)
    throw "That's just nasty.i.";
  setValue(*oPt);
}

NF_INLINE
CConxEqDistCurve CClsEqDistCurve::getValue() const throw(CClsError *)
{
  ENSURE_CONTENTS();
  // This may throw if the line is invalid:
  return CConxEqDistCurve(line->getValue(), conicDistance->getFloatValue());
}

// DLC the number `K' can only be positive.  How is that handled?

NF_INLINE
const CConxArtist *CClsEqDistCurve::getArtist() const
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
CConxDwGeomObj CClsEqDistCurve::getDwValue() const throw(CClsError *)
{
  CConxDwGeomObj p(getValue()); // may throw
  affectValue(p);
  return p;
}

NF_INLINE
int CClsEqDistCurve::operator==(const CClsEqDistCurve &o) const
{
  return (isClassInstance() == o.isClassInstance())
    && (isClassInstance()
        || (getValue() == o.getValue() && getDwValue() == o.getDwValue()));
}

NF_INLINE
void CClsEqDistCurve::clear()
{
  FOREACH_MEMBER(i) {
    CClsBase *j = getNthMember(i);
    MAYBE_REMOVE_A_USER(j);
  }
  invalidateSavedArtist();
}

NF_INLINE
void CClsEqDistCurve::makeReadOnly()
{
  ENSURE_CONTENTS();
  CClsDrawable::makeReadOnly();
  FOREACH_MEMBER(i) {
    getNthMember(i)->makeReadOnly();
  }
}

NF_INLINE
void CClsEqDistCurve::invalidateSavedArtist() const
{
  if (savedArtist != NULL)
    delete savedArtist;
  savedArtist = NULL;
}

NF_INLINE
void CClsEqDistCurve::ensureValidity()
{
  if (conicDistance == NULL) {
    conicDistance = new CClsFloat(1.0);
    conicDistance->incrementUsers();
  }
  if (line == NULL) {
    line = new CClsLine();
    line->incrementUsers();
  }
  FOREACH_MEMBER(i) if (getNthMember(i) == NULL) OOM();
  ENSURE_CONTENTS();
}

NF_INLINE
void CClsEqDistCurve::uninitializedCopy(const CClsEqDistCurve &o)
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
CClsEqDistCurve::ciActionLineK(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[2]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_LINE, TRUE);
  ENSURE_KEYWD_TYPE(result, o, argv, 1, CLS_NUMBER, TRUE);
  RETURN_NEW_RESULT(result, new CClsEqDistCurve((CClsLine *)argv[0],
                                                (CClsNumber *)argv[1]));
}

NF_INLINE
void CClsEqDistCurve::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ST_CMETHOD(ansMachs, "new", "instance creation",
               CLASS, ciAnswererNew,
               "Returns a new object instance of a curve at a set distance from a line in Hyperbolic geometry" DRAWABLE_STR);
    ST_CMETHOD(ansMachs, "line:K:", "instance creation",
               CLASS, ciAnswererLineK,
               "Returns a new, set object instance of a curve at a set distance from a line in Hyperbolic geometry" DRAWABLE_STR);
    ADD_ANS_GETTER("line", Line);
    ADD_ANS_GETTER("K", K);
    ADD_ANS_GETTER("line:", LineColon);
    ADD_ANS_GETTER("K:", KColon);
  }
}

NF_INLINE
size_t CClsEqDistCurve::numMembers() const
{
  return 2;
}

NF_INLINE
CClsBase *CClsEqDistCurve::getNthMember(size_t i)
{
  assert(line != NULL);
  assert(conicDistance != NULL);

  switch (i) {
  case 0: return line;
  case 1: return conicDistance;
  default: throw "never!!!!";
  }
}

NF_INLINE
const CClsBase *CClsEqDistCurve::getNthMember(size_t i) const
{
  const CClsEqDistCurve *tC = this;
  CClsEqDistCurve *t;
  CONST_CAST(CClsEqDistCurve *, t, tC);
  return t->getNthMember(i);
}

NF_INLINE
void CClsEqDistCurve::setNthMemberPointer(size_t i, CClsBase *mm)
{
  switch (i) {
  case 0: assert(mm == NULL || mm->isType(CLS_LINE));
    line = (CClsLine *) mm; break;
  case 1: assert(mm == NULL || mm->isType(CLS_NUMBER));
    conicDistance = (CClsNumber *) mm; break;
  default: throw "never!!!!";
  }
}

NF_INLINE
Boole CClsEqDistCurve::dependsOn(const CClsBase *p) const
{
  if (p == line || p == conicDistance)
    return TRUE;
  return CClsDrawable::dependsOn(p);
}

