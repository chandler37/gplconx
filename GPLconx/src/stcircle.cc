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
  Implementation of C++ classes in `stcircle.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stcircle.hh"
#include "stfloat.hh"
#include "sterror.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsCircle::ansMachs = NULL;

CF_INLINE
CClsCircle::CClsCircle(CClsPoint *Center, CClsNumber *Radius)
{
  init();
  radius = Radius;
  center = Center;
  if (radius != NULL) radius->incrementUsers();
  if (center != NULL) center->incrementUsers();
  ensureValidity();
}

CF_INLINE
CClsCircle::CClsCircle(const CClsCircle &o)
  : CClsDrawable(o)
{
  MMM("copy constructor");
  init(); uninitializedCopy(o);
}

NF_INLINE
CClsCircle &CClsCircle::operator=(const CClsCircle &o)
{
  (void) CClsDrawable::operator=(o);
  clear();
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
CClsBase *CClsCircle::stCloneDeep() const
{
  CClsDrawable *j = (CClsDrawable *) CClsDrawable::stCloneDeep();
  if (j == NULL) OOM();
  CClsCircle *v = new CClsCircle(*j);
  delete j;
  if (v == NULL) OOM();
  v->clear();
  v->radius = (CClsNumber *) radius->stCloneDeep();
  v->center = (CClsPoint *) center->stCloneDeep();
  v->radius->incrementUsers();
  v->center->incrementUsers();
  return v;
}

NF_INLINE
CConxString CClsCircle::printString() const
{
  if (isClassInstance()) {
    return getClsName();
  } else {
    assert(radius != NULL); assert(center != NULL);
    CConxString s = getClsName();
    s += "(Center: " + center->printString() + ", radius: " + radius->printString() + ")";
    s += " -- " + CClsDrawable::printString();
    return s;
  }
}

NF_INLINE
void CClsCircle::setValue(const CConxCircle &o)
{
  clear();
  radius = new CClsFloat(o.getRadius());
  center = new CClsPoint(o.getCenter());
  if (radius == NULL || center == NULL) OOM();
  radius->incrementUsers();
  center->incrementUsers();
  invalidateSavedArtist();
}

NF_INLINE
void CClsCircle::setValue(const CConxDwGeomObj &o)
{
  CClsDrawable::setValue(o);
  const CConxSimpleArtist *oPt = o.getGeomObj();
  if (oPt == NULL || oPt->getSAType() != CConxSimpleArtist::SA_CIRCLE)
    throw "That's just nasty.";
  setValue(*oPt);
}

NF_INLINE
CConxCircle CClsCircle::getValue() const throw(CClsError *)
{
  assert(radius != NULL && center != NULL);
  // This may throw if the center is invalid:
  return CConxCircle(center->getValue(), radius->getFloatValue());
}

NF_INLINE
const CConxArtist *CClsCircle::getArtist() const
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
CConxDwGeomObj CClsCircle::getDwValue() const throw(CClsError *)
{
  CConxDwGeomObj p(getValue()); // may throw
  affectValue(p);
  return p;
}

NF_INLINE
int CClsCircle::operator==(const CClsCircle &o)
{
  return (getValue() == o.getValue() && getDwValue() == o.getDwValue());
}

NF_INLINE
void CClsCircle::clear()
{
  MAYBE_REMOVE_A_USER(radius);
  MAYBE_REMOVE_A_USER(center);
  invalidateSavedArtist();
}

NF_INLINE
void CClsCircle::makeReadOnly()
{
  CClsDrawable::makeReadOnly();
  assert(center != NULL); assert(radius != NULL);
  radius->makeReadOnly(); center->makeReadOnly();
}

NF_INLINE
void CClsCircle::invalidateSavedArtist() const
{
  if (savedArtist != NULL)
    delete savedArtist;
  savedArtist = NULL;
}

NF_INLINE
void CClsCircle::ensureValidity()
{
  if (radius == NULL) {
    radius = new CClsFloat(1.0);
    radius->incrementUsers();
  }
  if (center == NULL) {
    center = new CClsPoint();
    center->incrementUsers();
  }
}

NF_INLINE
void CClsCircle::uninitializedCopy(const CClsCircle &o)
{
  // We are a container of one Numbers and one Point, so
  // we will both point to these contents.

  invalidateSavedArtist();

  assert(o.center != NULL); assert(o.radius != NULL);
  radius = o.radius;
  center = o.center;

  radius->incrementUsers();
  center->incrementUsers();
}

NF_INLINE CClsBase::ErrType
CClsCircle::ciActionCenterRadius(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[2]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_POINT, TRUE);
  ENSURE_KEYWD_TYPE(result, o, argv, 1, CLS_NUMBER, TRUE);
  RETURN_NEW_RESULT(result, new CClsCircle((CClsPoint *)argv[0], (CClsNumber *)argv[1]));
}

NF_INLINE
void CClsCircle::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ansMachs->append(CConxClsAnsMach("new",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererNew,
                                     "Returns a new object instance of a circle in Hyperbolic geometry" DRAWABLE_STR));
    ansMachs->append(CConxClsAnsMach("center:radius:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererCenterRadius,
                                     "Returns a new, set object instance of a circle in Hyperbolic geometry" DRAWABLE_STR));
    ADD_ANS_GETTER("center", Center);
    ADD_ANS_GETTER("radius", Radius);
    ADD_ANS_SETTER("center:", CenterColon);
    ADD_ANS_SETTER("radius:", RadiusColon);
  }
}

