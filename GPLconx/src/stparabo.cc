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
  Implementation of C++ classes in `stparabo.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stparabo.hh"
#include "sterror.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsParabola::ansMachs = NULL;

CF_INLINE
CClsParabola::CClsParabola(CClsPoint *Focus, CClsLine *Line)
{
  init();
  line = Line;
  focus = Focus;
  if (line != NULL) line->incrementUsers();
  if (focus != NULL) focus->incrementUsers();
  ensureValidity();
}

CF_INLINE
CClsParabola::CClsParabola(const CClsDrawable &od)
  : CClsDrawable(od)
{
  init();
  ensureValidity();
}

CF_INLINE
CClsParabola::CClsParabola(const CConxDwGeomObj &od)
  : CClsDrawable(od)
{
  init();
  setValue(od);
}

CF_INLINE
CClsParabola::CClsParabola(const CClsParabola &o)
  : CClsDrawable(o)
{
  MMM("copy constructor");
  init(); uninitializedCopy(o);
}

NF_INLINE
CClsParabola &CClsParabola::operator=(const CClsParabola &o)
{
  (void) CClsDrawable::operator=(o);
  clear();
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
CClsBase *CClsParabola::stCloneDeep() const
{
  CClsDrawable *j = (CClsDrawable *) CClsDrawable::stCloneDeep();
  if (j == NULL) OOM();
  CClsParabola *v = new CClsParabola(*j);
  delete j;
  if (v == NULL) OOM();
  v->clear();
  v->line = (CClsLine *) line->stCloneDeep();
  v->focus = (CClsPoint *) focus->stCloneDeep();
  v->line->incrementUsers();
  v->focus->incrementUsers();
  return v;
}

NF_INLINE
CConxString CClsParabola::printString() const
{
  if (isClassInstance()) {
    return getClsName();
  } else {
    assert(line != NULL); assert(focus != NULL);
    CConxString s = getClsName();
    s += "(Focus: " + focus->printString() + ", line: " + line->printString() + ")";
    s += " -- " + CClsDrawable::printString();
    return s;
  }
}

NF_INLINE
void CClsParabola::setValue(const CConxParabola &o)
{
  clear();
  line = new CClsLine(o.getLine());
  focus = new CClsPoint(o.getFocus());
  if (line == NULL || focus == NULL) OOM();
  line->incrementUsers();
  focus->incrementUsers();
  invalidateSavedArtist();
}

NF_INLINE
void CClsParabola::setValue(const CConxDwGeomObj &o)
{
  CClsDrawable::setValue(o);
  const CConxSimpleArtist *oPt = o.getGeomObj();
  if (oPt == NULL || oPt->getSAType() != CConxSimpleArtist::SA_PARABOLA)
    throw "That's just nasty.";
  setValue(*oPt);
}

NF_INLINE
CConxParabola CClsParabola::getValue() const throw(CClsError *)
{
  assert(line != NULL && focus != NULL);
  // This may throw if the focus is invalid:
  return CConxParabola(focus->getValue(), line->getValue());
}

NF_INLINE
const CConxArtist *CClsParabola::getArtist() const
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
CConxDwGeomObj CClsParabola::getDwValue() const throw(CClsError *)
{
  CConxDwGeomObj p(getValue()); // may throw
  affectValue(p);
  return p;
}

NF_INLINE
int CClsParabola::operator==(const CClsParabola &o)
{
  return (getValue() == o.getValue() && getDwValue() == o.getDwValue());
}

NF_INLINE
void CClsParabola::clear()
{
  MAYBE_REMOVE_A_USER(line);
  MAYBE_REMOVE_A_USER(focus);
  invalidateSavedArtist();
}

NF_INLINE
void CClsParabola::makeReadOnly()
{
  CClsDrawable::makeReadOnly();
  assert(focus != NULL); assert(line != NULL);
  line->makeReadOnly(); focus->makeReadOnly();
}

NF_INLINE
void CClsParabola::invalidateSavedArtist() const
{
  if (savedArtist != NULL)
    delete savedArtist;
  savedArtist = NULL;
}

NF_INLINE
void CClsParabola::ensureValidity()
{
  if (line == NULL) {
    line = new CClsLine();
    line->incrementUsers();
  }
  if (focus == NULL) {
    focus = new CClsPoint();
    focus->incrementUsers();
  }
}

NF_INLINE
void CClsParabola::uninitializedCopy(const CClsParabola &o)
{
  // We are a container of one Line and one Point, so
  // we will both point to these contents.

  invalidateSavedArtist();

  assert(o.focus != NULL); assert(o.line != NULL);
  line = o.line;
  focus = o.focus;

  line->incrementUsers();
  focus->incrementUsers();
}

NF_INLINE CClsBase::ErrType
CClsParabola::ciActionFocusLine(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[2]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_POINT, TRUE);
  ENSURE_KEYWD_TYPE(result, o, argv, 1, CLS_LINE, TRUE);
  RETURN_NEW_RESULT(result, new CClsParabola((CClsPoint *)argv[0], (CClsLine *)argv[1]));
}

NF_INLINE
void CClsParabola::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ST_METHOD(ansMachs, "new", CLASS, ciAnswererNew,
              "Returns a new object instance of a parabola in Hyperbolic geometry" DRAWABLE_STR);
    ST_METHOD(ansMachs, "focus:line:", CLASS, ciAnswererFocusLine,
              "Returns a new, set object instance of a parabola in Hyperbolic geometry" DRAWABLE_STR);
    ADD_ANS_GETTER("focus", Focus);
    ADD_ANS_GETTER("line", Line);
    ADD_ANS_SETTER("focus:", FocusColon);
    ADD_ANS_SETTER("line:", LineColon);
  }
}

