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
  Implementation of C++ classes in `stdrawbl.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stdrawbl.hh"
#include "sterror.hh"
#include "stfloat.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsDrawable::ansMachs = NULL;

CF_INLINE
CClsDrawable::CClsDrawable()
{
  // initialize so that getValue will work.
  CConxDwGeomObj r;
  r.setColor(CConxNamedColor(CConxNamedColor::GREEN));
  r.setDrawingMethod(r.BRESENHAM);
  init(); setValue(r);
}

CF_INLINE
CClsDrawable::CClsDrawable(const CConxDwGeomObj &od)
{
  init();
  setValue(od);
}

CF_INLINE
CClsDrawable::CClsDrawable(const CClsDrawable &o)
  : CClsBase(o)
{
  MMM("copy constructor");
  uninitializedCopy(o);
}

NF_INLINE
CClsDrawable &CClsDrawable::operator=(const CClsDrawable &o)
{
  (void) CClsBase::operator=(o);
  clear();
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
CClsBase *CClsDrawable::stCloneDeep() const
{
  CClsDrawable *v = new CClsDrawable();
  if (v == NULL) OOM();
  v->color = (CClsColor *) color->stCloneDeep(); // DLC use rtti cast
  v->withGarnish = (CClsBoolean *) withGarnish->stCloneDeep();
  v->thickness = (CClsFloat *) thickness->stCloneDeep();
  v->lwtol = (CClsFloat *) lwtol->stCloneDeep();
  v->drawingMethod = (CClsSymbol *) drawingMethod->stCloneDeep();
  v->color->incrementUsers();
  v->withGarnish->incrementUsers();
  v->thickness->incrementUsers();
  v->lwtol->incrementUsers();
  v->drawingMethod->incrementUsers();
  return v;
}

NF_INLINE
void CClsDrawable::setValue(const CConxDwGeomObj &od)
{
  clear();
  color = new CClsColor(od.getColor());
  withGarnish = new CClsBoolean(od.getGarnishing());
  thickness = new CClsFloat(od.getThickness());
  lwtol = new CClsFloat(od.getLongwayTolerance());
  drawingMethod
    = new CClsSymbol(CConxDwGeomObj::drawingMethodToString( \
                                                     od.getDrawingMethod()));
  if (color == NULL || withGarnish == NULL || thickness == NULL
      || lwtol == NULL || drawingMethod == NULL)
    OOM();

  color->incrementUsers();
  withGarnish->incrementUsers();
  thickness->incrementUsers();
  lwtol->incrementUsers();
  drawingMethod->incrementUsers();
}

NF_INLINE
CConxDwGeomObj CClsDrawable::getValue() const
  // DLC optimize, track changes and only make a new one when changes occur.
{
  assert(color != NULL && withGarnish != NULL && lwtol != NULL
         && thickness != NULL && drawingMethod != NULL);
  CConxDwGeomObj r;
  affectValue(r);
  return r;
}

NF_INLINE
void CClsDrawable::affectValue(CConxDwGeomObj &r) const
  // Sets o so that it resembles this object.
{
  assert(color != NULL && withGarnish != NULL && lwtol != NULL
         && thickness != NULL && drawingMethod != NULL);
  r.setColor(color->getColor());
  if (drawingMethod->getValue() == "longway") { // DLC #longway
    r.setDrawingMethod(r.LONGWAY);
  } else {
    r.setDrawingMethod(r.BRESENHAM);
  }
  r.setThickness(thickness->getFloatValue());
  r.setLongwayTolerance(lwtol->getFloatValue());
  r.setGarnishing(withGarnish->getValue());
}

NF_INLINE
CConxString CClsDrawable::printString() const
{
  if (isClassInstance()) {
    return getClsName();
  } else {
    CConxString s = "Drawable -- ";
    s += color->printString() + ", garnishing "
      + (withGarnish->getValue() ? "on" : "off") + ", thickness "
      + thickness->getFloatValue() + ", slow drawing method tolerance "
      + lwtol->getFloatValue() + ", drawing method "
      + drawingMethod->printString();
    return s;
  }
}

NF_INLINE
void CClsDrawable::clear()
{
  MAYBE_REMOVE_A_USER(color);
  MAYBE_REMOVE_A_USER(thickness);
  MAYBE_REMOVE_A_USER(lwtol);
  MAYBE_REMOVE_A_USER(withGarnish);
  MAYBE_REMOVE_A_USER(drawingMethod);
}

NF_INLINE
const CConxArtist *CClsDrawable::getArtist() const
{
  MMM("virtual const CConxArtist *getArtist() const");
  return NULL;
}

NF_INLINE
void CClsDrawable::makeReadOnly()
{
  CClsBase::makeReadOnly();

  assert(color != NULL); assert(withGarnish != NULL);
  assert(drawingMethod != NULL); assert(thickness != NULL);
  assert(lwtol != NULL);
  color->makeReadOnly(); withGarnish->makeReadOnly();
  drawingMethod->makeReadOnly(); thickness->makeReadOnly();
  lwtol->makeReadOnly();
}

NF_INLINE
void CClsDrawable::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ansMachs->append(CConxClsAnsMach("new",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererNew,
                                     "Returns a new object instance of a drawable object, whose subclasses include points, lines, circles, parabolas, etc."));

    ADD_ANS_GETTER("drawWithGarnish", DrawWithGarnish);
    ADD_ANS_GETTER("thickness", Thickness);
    ADD_ANS_GETTER("longwayMethodTolerance", LongwayMethodTolerance);
    ADD_ANS_GETTER("color", Color);
    ADD_ANS_GETTER("drawingMethod", DrawingMethod);
    ADD_ANS_SETTER("drawWithGarnish:", DrawWithGarnishColon);
    ADD_ANS_SETTER("thickness:", ThicknessColon);
    ADD_ANS_SETTER("longwayMethodTolerance:", LongwayMethodToleranceColon);
    ADD_ANS_SETTER("color:", ColorColon);
    ADD_ANS_SETTER("drawingMethod:", DrawingMethodColon);
  }
}

NF_INLINE
void CClsDrawable::init()
{
  color = NULL; withGarnish = NULL; drawingMethod = NULL; thickness = NULL;
  lwtol = NULL;
}

NF_INLINE
void CClsDrawable::uninitializedCopy(const CClsDrawable &o)
{
  init();
  setValue(o.getValue());
}

