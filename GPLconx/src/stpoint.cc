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
  Implementation of C++ classes in `stpoint.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stpoint.hh"
#include "sterror.hh"
#include "stfloat.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsPoint::ansMachs = NULL;

NF_INLINE
CConxString CClsPoint::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    assert(xCoord != NULL); assert(yCoord != NULL); assert(model != NULL);
    CConxString s = getClsName();
    s += "(x: " + xCoord->printString() + ", y: " + yCoord->printString()
      + ", model: " + model->printShortString() + ")";
    s += " -- " + CClsDrawable::printString();
    return s;
    // DLC this may show `Point(x: 0.36, y: -0.1, model: uhp)' when
    // -0.1 will be clamped to [0.0, inf)

  }
}

CF_INLINE
CClsPoint::CClsPoint(CClsNumber *x, CClsNumber *y, CClsModelIdentifier *m)
{
  init();
  xCoord = x;
  yCoord = y;
  model = m;
  if (xCoord != NULL) xCoord->incrementUsers();
  if (yCoord != NULL) yCoord->incrementUsers();
  if (model != NULL) model->incrementUsers();
  ensureValidity();
}

CF_INLINE
CClsPoint::CClsPoint(double euclideanRadius)
{
  init();

  // Clamp the Euclidean radius of the circle with center (0, 0) that
  // we will be evenly distributing points into.
  if (euclideanRadius < 0.0) euclideanRadius = 0.0;
  if (euclideanRadius > 1.0) euclideanRadius = 1.0;

  // Find a random direction and then go a random distance.
  double theta = CClsFloat::getRandom(0.0, 2.0 * M_PI);
  double r = CClsFloat::getRandom(0.0, euclideanRadius);
  // This is the polar description of the Euclidean point, give a Cartesian:
  double x = r * cos(theta);
  double y = r * sin(theta);
  setValue(CConxPoint(x, y, CONX_KLEIN_DISK), CONX_KLEIN_DISK);
}

CF_INLINE
CClsPoint::CClsPoint(const CConxPoint &od, ConxModlType m = CONX_POINCARE_UHP)
{
  init(); setValue(od, m);
}

CF_INLINE
CClsPoint::CClsPoint(const CClsPoint &o)
  : CClsDrawable(o)
{
  MMM("copy constructor");
  init(); uninitializedCopy(o);
}

NF_INLINE
CClsPoint &CClsPoint::operator=(const CClsPoint &o)
{
  (void) CClsDrawable::operator=(o);
  clear();
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
CClsBase *CClsPoint::stCloneDeep() const
{
  LLL("CClsBase *CClsPoint::stCloneDeep()");
  CClsDrawable *j = (CClsDrawable *) CClsDrawable::stCloneDeep();
  if (j == NULL) OOM();
  CClsPoint *v = new CClsPoint(*j);
  delete j;
  if (v == NULL) OOM();
  v->clear();
  v->xCoord = (CClsNumber *) xCoord->stCloneDeep();
  v->yCoord = (CClsNumber *) yCoord->stCloneDeep();
  v->model = (CClsModelIdentifier *) model->stCloneDeep();
  v->xCoord->incrementUsers();
  v->yCoord->incrementUsers();
  v->model->incrementUsers();
  return v;
}

NF_INLINE
void CClsPoint::setValue(const CConxPoint &o, ConxModlType m = CONX_POINCARE_UHP)
{
  clear();
  xCoord = new CClsFloat(o.getX(m));
  yCoord = new CClsFloat(o.getY(m));
  model = new CClsModelIdentifier(m);
  // DLC CClsModelIdentifier::getObjectInstance(ConxModlType) for efficiency's sake

  if (xCoord == NULL || yCoord == NULL || model == NULL) OOM();
  xCoord->incrementUsers();
  yCoord->incrementUsers();
  model->incrementUsers();
  invalidateSavedArtist();
}

NF_INLINE
void CClsPoint::setValue(const CConxDwGeomObj &o, ConxModlType m = CONX_POINCARE_UHP)
{
  CClsDrawable::setValue(o);
  const CConxSimpleArtist *oPt = o.getGeomObj();
  if (oPt == NULL || oPt->getSAType() != CConxSimpleArtist::SA_POINT)
    throw "That's just nasty.";
  setValue(*oPt, m);
}

NF_INLINE
CConxPoint CClsPoint::getValue() const throw(CClsError *)
{
  assert(xCoord != NULL && yCoord != NULL && model != NULL);
  if (model->model() == CClsModelIdentifier::NONE) {
    CClsError *ne = new CClsError("the point's model is indecipherable");
    throw ne; // DLC check oom
  }
  return CConxPoint(xCoord->getFloatValue(), yCoord->getFloatValue(),
                    model->cModel());
}

NF_INLINE
void CClsPoint::setReadOnly(Boole readOnly)
{
  // From here on out, the artist will be saved.
  CClsDrawable::setReadOnly(readOnly);
  invalidateSavedArtist();
}

NF_INLINE
const CConxArtist *CClsPoint::getArtist() const
{
  MMM("virtual const CConxArtist *getArtist() const");

  // The saved artist is always invalid if the user count on x, y, and model
  // sums to 4 or more for some amount of time on a non-read-only Point.
  // We could have the user sync this Point, but
  // that's dumb, so just don't call getArtist() very often!  (If you
  // really wanted to, you could invalidate only when a getter is called,
  // or you could make some objects sconst, i.e. unable to be contained
  // in drawables.  Then you could redraw the whole canvas in a smart
  // way.  Most likely, though, the user will have the option of immediate
  // updates (slower) or manual updates (faster).
  // We should make `Point x: x1 clone y: y1 clone m: uhp' fast until
  // a getter is called for x: or y: (yes, the read-only m: is excluded,
  // a getter does not have a chance of changing this object))
  if (!isReadOnly())
    invalidateSavedArtist();

  if (savedArtist == NULL) {
    savedArtist = new CConxDwGeomObj(getDwValue());
    if (savedArtist == NULL) OOM();
  }
  return savedArtist;
}

NF_INLINE
CConxDwGeomObj CClsPoint::getDwValue() const throw(CClsError *)
{
  CConxDwGeomObj p(getValue()); // may throw
  affectValue(p);
  return p;
}

NF_INLINE
int CClsPoint::operator==(const CClsPoint &o) 
{
  return (getValue() == o.getValue() && getDwValue() == getDwValue());
}

NF_INLINE
void CClsPoint::clear()
{
  MAYBE_REMOVE_A_USER(xCoord);
  MAYBE_REMOVE_A_USER(yCoord);
  MAYBE_REMOVE_A_USER(model);
  invalidateSavedArtist();
}

NF_INLINE
void CClsPoint::makeReadOnly()
{
  CClsDrawable::makeReadOnly();
  assert(model != NULL); assert(xCoord != NULL); assert(yCoord != NULL);
  xCoord->makeReadOnly();
  yCoord->makeReadOnly();
  model->makeReadOnly();
}

NF_INLINE
void CClsPoint::invalidateSavedArtist() const
{
  if (savedArtist != NULL)
    delete savedArtist;
  savedArtist = NULL;
}

NF_INLINE
void CClsPoint::ensureValidity()
{
  if (xCoord == NULL) {
    xCoord = new CClsFloat(0.36);
    xCoord->incrementUsers();
  }
  if (yCoord == NULL) {
    yCoord = new CClsFloat(0.37);
    yCoord->incrementUsers();
  }
  if (model == NULL) {
    model = new CClsModelIdentifier("uhp");
    model->incrementUsers();
  }
}

NF_INLINE
void CClsPoint::uninitializedCopy(const CClsPoint &o)
{
  // We are a container of two Numbers and a ModelIdentifier, so
  // we will both point to these contents.

  invalidateSavedArtist();

  assert(o.model != NULL);
  assert(o.xCoord != NULL); assert(o.yCoord != NULL);
  xCoord = o.xCoord;
  yCoord = o.yCoord;
  model = o.model;

  xCoord->incrementUsers();
  yCoord->incrementUsers();
  model->incrementUsers();
}

NF_INLINE
CClsBase::ErrType 
CClsPoint::ciActionXYModel(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[3]; o.getBoundObjects(argv);
  for (int ai = 0; ai < 2; ai++)
    ENSURE_KEYWD_TYPE(result, o, argv, ai, CLS_NUMBER, TRUE);
  ENSURE_KEYWD_TYPE(result, o, argv, 2, CLS_MODELIDENTIFIER, TRUE);
  CClsNumber *x = (CClsNumber *)argv[0], *y = (CClsNumber *)argv[1];
  CClsModelIdentifier *mdl = (CClsModelIdentifier *) argv[2];
  if (mdl->model() == CClsModelIdentifier::NONE) {
    RETURN_ERROR_RESULT(result, "third keyword's argument must represent a model unambiguously");
  }
  RETURN_NEW_RESULT(result, new CClsPoint(x, y, mdl));
}

NF_INLINE
CClsBase::ErrType 
CClsPoint::ciActionRandom1(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result, new CClsPoint(1.0));
}

NF_INLINE
CClsBase::ErrType 
CClsPoint::ciActionRandom(CClsBase **result, CConxClsMessage &o) const
{
  NEED_N_FLOATS(1, fargv, o, result);
  RETURN_NEW_RESULT(result, new CClsPoint(fargv[0]));
}

NF_INLINE
CClsBase::ErrType 
CClsPoint::oiActionDistanceFromPoint(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[1]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_POINT, TRUE);
  try {
    const CConxPoint &pt = ((CClsPoint *)argv[0])->getValue();
    RETURN_NEW_RESULT(result, new CClsFloat(getValue().distanceFrom(pt)));
  } catch (CClsError *ne) {
    RETURN_NEW_RESULT(result, ne);
  }
}

NF_INLINE
CClsBase::ErrType 
CClsPoint::oiActionBetweenAnd(CClsBase **result, CConxClsMessage &o) const
{
  // DLC does not test for approximate colinearity!
  CClsBase *argv[2]; o.getBoundObjects(argv);
  for (int ai = 0; ai < 2; ai++)
    ENSURE_KEYWD_TYPE(result, o, argv, ai, CLS_POINT, TRUE);
  try {
    RETURN_NEW_RESULT(result,
                      new CClsBoolean(getValue().isBetween( \
                                        ((CClsPoint *)argv[0])->getValue(),
                                        ((CClsPoint *)argv[1])->getValue())));
  } catch (CClsError *ne) {
    RETURN_NEW_RESULT(result, ne);
  }
}

NF_INLINE
void CClsPoint::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ansMachs->append(CConxClsAnsMach("new",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererNew,
                                     "Returns a new object instance of a 2-D point in Hyperbolic geometry" DRAWABLE_STR));
    ansMachs->append(CConxClsAnsMach("random:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererRandom,
                                     "Returns a new point randomly distributed in the Klein disk's Euclidean counterpart with Euclidean distance from (0, 0) less than the argument (maximum of 1.0 since the unit disk is at infinity) -- this has a random Euclidean radius, but the hyperbolic distance from hyperbolic (0, 0) is more likely to be small, i.e. it is not a random hyperbolic distance from (0, 0), because that would skew the points toward the edge of the disk, which is not a great idea for visualization."));
    ansMachs->append(CConxClsAnsMach("random",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererRandom1,
                                     "Returns what `random: 1.0' does"));
    ansMachs->append(CConxClsAnsMach("x:y:model:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererXYModel,
                                     "Returns a new, set object instance of a 2-D point in Hyperbolic geometry" DRAWABLE_STR));
    ansMachs->append(CConxClsAnsMach("x:y:m:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererXYModel,
                                     "Returns a new, set object instance of a 2-D point in Hyperbolic geometry" DRAWABLE_STR));
    // DLC ParseError object instance `help' gives `Object help'
    ansMachs->append(CConxClsAnsMach("distanceFromPoint:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererDistanceFromPoint,
                                     "Returns the distance between the receiver and the Point object instance argument"));
    ansMachs->append(CConxClsAnsMach("betweenColinearPoint:andOtherColinearPoint:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererBetweenAnd,
                                     "Returns true iff the receiver lies between two Point object instance arguments on the hyperbolic geometry line upon which the three Points are assumed to be colinear"));
    ADD_ANS_GETTER("x", X);
    ADD_ANS_GETTER("y", Y);
    ADD_ANS_GETTER("model", Model);
    ADD_ANS_SETTER("x:", XColon);
    ADD_ANS_SETTER("y:", YColon);
    ADD_ANS_SETTER("model:", ModelColon);
  }
  // DLC add setters, add CClsBase set: CClsBase general assignment operator
  // invoker.
}


