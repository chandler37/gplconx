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
  Implementation of C++ classes in `stline.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stline.hh"
#include "sterror.hh"
#include "stfloat.hh"

Answerers *CClsLine::ansMachs = NULL;

NF_INLINE
void CClsLine::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ST_CMETHOD(ansMachs, "new", "instance creation",
               CLASS, ciAnswererNew,
               "Returns a new object instance of a line in Hyperbolic geometry" DRAWABLE_STR);
    ST_CMETHOD(ansMachs, "A:B:isSegment:", "instance creation",
               CLASS, ciAnswererABIsSegment,
               "Returns a new, set object instance of a 2-D line in Hyperbolic geometry" DRAWABLE_STR);
    ST_CMETHOD(ansMachs, "random:", "instance creation",
               CLASS, ciAnswererRandom,
               "Returns a new 2-D line in Hyperbolic geometry with two ``random'' Points with Euclidean Klein Disk radius less than the argument (see help for `Point random:')");
    ST_CMETHOD(ansMachs, "random", "instance creation",
               CLASS, ciAnswererRandom1,
               "Returns a new 2-D line in Hyperbolic geometry with two ``random'' Points (see help for `Point random')");
    ADD_ANS_GETTER("A", A);
    ADD_ANS_GETTER("B", B);
    ADD_ANS_GETTER("isSegment", IsSegment);
    ADD_ANS_SETTER("A:", AColon);
    ADD_ANS_SETTER("B:", BColon);
    ADD_ANS_SETTER("isSegment:", IsSegmentColon);
  }
}

NF_INLINE
void CClsLine::uninitializedCopy(const CClsLine &o)
{
  // We are a container of two Numbers and a ModelIdentifier, so
  // we will both line to these contents.

  invalidateSavedArtist();

  assert(o.isSeg != NULL); assert(o.P[0] != NULL); assert(o.P[1] != NULL);
  P[0] = o.P[0];
  P[1] = o.P[1];
  isSeg = o.isSeg;

  P[0]->incrementUsers();
  P[1]->incrementUsers();
  isSeg->incrementUsers();
}

CF_INLINE
CClsLine::CClsLine(CClsPoint *A, CClsPoint *B, CClsBoolean *seg)
{
  init();
  P[0] = A;
  P[1] = B;
  isSeg = seg;
  if (P[0] != NULL) P[0]->incrementUsers();
  if (P[1] != NULL) P[1]->incrementUsers();
  if (isSeg != NULL) isSeg->incrementUsers();
  ensureValidity();
}

CF_INLINE
CClsLine::CClsLine(const CClsLine &o)
  : CClsDrawable(o)
{
  MMM("copy constructor");
  init(); uninitializedCopy(o);
}

NF_INLINE
CClsLine &CClsLine::operator=(const CClsLine &o)
{
  (void) CClsDrawable::operator=(o);
  clear();
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
CClsBase *CClsLine::stCloneDeep() const
{
  CClsDrawable *j = (CClsDrawable *) CClsDrawable::stCloneDeep();
  if (j == NULL) OOM();
  CClsLine *v = new CClsLine(*j);
  delete j;
  if (v == NULL) OOM();
  v->clear();
  v->P[0] = (CClsPoint *) P[0]->stCloneDeep();
  v->P[1] = (CClsPoint *) P[1]->stCloneDeep();
  v->isSeg = (CClsBoolean *) isSeg->stCloneDeep();
  v->P[0]->incrementUsers();
  v->P[1]->incrementUsers();
  v->isSeg->incrementUsers();
  return v;
}

NF_INLINE
CConxString CClsLine::printString() const
{
  if (isClassInstance()) {
    return getClsName();
  } else {
    assert(P[0] != NULL); assert(P[1] != NULL); assert(isSeg != NULL);
    CConxString s = getClsName();
    if (isSeg->getValue()) {
      s += " segment";
    }
    s += "(A: " + P[0]->printString() + ", B: " + P[1]->printString() + ")";
    s += " -- " + CClsDrawable::printString();
    return s;
  }
}

NF_INLINE
void CClsLine::setValue(const CConxLine &o)
{
  clear();
  P[0] = new CClsPoint(o.getA());
  P[1] = new CClsPoint(o.getB());
  isSeg = new CClsBoolean(o.isLineSegment());
  if (P[0] == NULL || P[1] == NULL || isSeg == NULL) OOM();
  P[0]->incrementUsers();
  P[1]->incrementUsers();
  isSeg->incrementUsers();
  invalidateSavedArtist();
}

NF_INLINE
CConxLine CClsLine::getValue() const throw(CClsError *)
{
  assert(P[0] != NULL && P[1] != NULL && isSeg != NULL);
  // This may throw if one of the points is invalid:
  return CConxLine(P[0]->getValue(), P[1]->getValue(), isSeg->getValue());
}

NF_INLINE
const CConxArtist *CClsLine::getArtist() const
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
CConxDwGeomObj CClsLine::getDwValue() const throw(CClsError *)
{
  CConxDwGeomObj p(getValue()); // may throw
  affectValue(p);
  return p;
}

NF_INLINE
int CClsLine::operator==(const CClsLine &o) const
{
  return (isClassInstance() == o.isClassInstance())
    && (isClassInstance()
        || (getValue() == o.getValue() && getDwValue() == o.getDwValue()));
}

NF_INLINE
void CClsLine::clear()
{
  MAYBE_REMOVE_A_USER(P[0]);
  MAYBE_REMOVE_A_USER(P[1]);
  MAYBE_REMOVE_A_USER(isSeg);
  invalidateSavedArtist();
}

NF_INLINE
void CClsLine::makeReadOnly()
{
  CClsDrawable::makeReadOnly();
  assert(isSeg != NULL); assert(P[0] != NULL); assert(P[1] != NULL);
  P[0]->makeReadOnly(); P[1]->makeReadOnly(); isSeg->makeReadOnly();
}

NF_INLINE
void CClsLine::invalidateSavedArtist() const
{
  if (savedArtist != NULL)
    delete savedArtist;
  savedArtist = NULL;
}

NF_INLINE
void CClsLine::ensureValidity()
{
  if (P[0] == NULL) {
    P[0] = new CClsPoint();
    P[0]->incrementUsers();
  }
  if (P[1] == NULL) {
    P[1] = new CClsPoint();
    P[1]->incrementUsers();
  }
  if (isSeg == NULL) {
    isSeg = new CClsBoolean(FALSE);
    isSeg->incrementUsers();
  }
}

NF_INLINE CClsBase::ErrType
CClsLine::ciActionABIsSegment(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[3]; o.getBoundObjects(argv);
  for (int ai = 0; ai < 2; ai++)
    ENSURE_KEYWD_TYPE(result, o, argv, ai, CLS_POINT, TRUE);
  ENSURE_KEYWD_TYPE(result, o, argv, 2, CLS_BOOLEAN, TRUE);
  CClsPoint *A = (CClsPoint *)argv[0], *B = (CClsPoint *)argv[1];
  CClsBoolean *isSegment = (CClsBoolean *) argv[2];
  RETURN_NEW_RESULT(result, new CClsLine(A, B, isSegment));
}

NF_INLINE CClsBase::ErrType
CClsLine::ciActionRandom(CClsBase **result, CConxClsMessage &o) const
{
  NEED_N_FLOATS(1, fargv, o, result);
  RETURN_NEW_RESULT(result, new CClsLine(new CClsPoint(fargv[0]),
                                         new CClsPoint(fargv[0]),
                                         new CClsBoolean(FALSE)));
}

NF_INLINE CClsBase::ErrType
CClsLine::ciActionRandom1(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result, new CClsLine(new CClsPoint(1.0),
                                         new CClsPoint(1.0),
                                         new CClsBoolean(FALSE)));
}

NF_INLINE
Boole CClsLine::dependsOn(const CClsBase *p) const
{
  if (p == isSeg || p == P[0] || p == P[1])
    return TRUE;
  return CClsDrawable::dependsOn(p);
}

