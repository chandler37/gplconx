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
  C++ classes that represent Smalltalk classes and objects that are specific
  to GPLconx.
*/

#ifndef GPLCONX_STCONX_CXX_H
#define GPLCONX_STCONX_CXX_H 1

#include <iostream.h>
#include <assert.h>

#include "stcommon.hh"
#include "dgeomobj.hh"
#include "canvas.hh"


#define DRAWABLE_STR " that can draw itself on a Canvas"

//////////////////////////////////////////////////////////////////////////////
// Our ModelIdentifier class, so that 'Point x: 0.3 y: 0.3 model: #klein'
// will work.  We allow #klein, #Klein, #KLEIN, #k, #K, #kl, etc. to mean
// the Beltrami-klein disk.
class CClsModelIdentifier : VIRT public CClsCharacterArray {
  CCONX_CLASSNAME("CClsModelIdentifier")
  CLSNAME("ModelIdentifier", "I distinguish between three models of hyperbolic geometry, the Poincare Disk, the Beltrami-Klein Disk, and the Poincare Upper Half Plane.")
  CLSTYPE(CClsCharacterArray, CLS_MODELIDENTIFIER)
  DEFAULT_SEND_MESSAGE(CClsCharacterArray)
  ANSMACH_ANSWERS(CClsCharacterArray)
  STCLONE2(CClsModelIdentifier)
public:
  enum Model { KD, UHP, PD, NONE, NIL };
  // NIL when we haven't calculated it yet, #hello is NONE, #klein is KD, etc.

  CClsModelIdentifier() { mdl = NIL; }
  CClsModelIdentifier(ConxModlType m);
  CClsModelIdentifier(const CConxString &s)
    : CClsCharacterArray(s) { mdl = NIL; }
  CClsModelIdentifier(const CClsModelIdentifier &o)
    : CClsCharacterArray(o) { mdl = o.mdl; }
  CClsModelIdentifier &operator=(const CClsModelIdentifier &o);
  CConxString printString() const;
  CConxString printShortString() const
  {
    Model m = model();
    switch (m) {
    case KD: return "kd";
    case PD: return "pd";
    case UHP: return "uhp";
    default: return "unknown";
    }
  }
  int operator==(const CClsModelIdentifier &o) const {
    return model() == o.model();
  }
  int operator!=(const CClsModelIdentifier &o) const { return !operator==(o); }
  void setValue(const CConxString &str)
  {
    CClsCharacterArray::setValue(str);
    mdl = NIL;
  }
  ConxModlType cModel() const throw(int);
  Model model() const;
  // protected: CClsCharacterArray::setValue; DLC if paranoia strikes...


protected:
  ANSWERER(CClsModelIdentifier, ciAnswererNew, ciActionNew);
  ErrType ciActionNew(CClsBase **result, CConxClsMessage &o) const
  {
    /*
      DLC test
$ M1 := ModelIdentifier new: #p
>>> Model Identifier #p which refers to no valid model
$ M2 := ModelIdentifier new: #poincare-disk
Parser Error: <parse error, expecting `TOK_STMT_END' or `'.''>

GPLconx: parser got error

GPLconx: parser got error
>>> nil
$ M2 := ModelIdentifier new: 'poincare-disk'
>>> Model Identifier #poincare-disk which refers to the Poincare Disk
$ M3 := ModelIdentifier new: 'poincare upper half'
>>> Model Identifier #poincare upper half which refers to the Poincare Upper Half Plane
     */
    ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsCharacterArray, argv, o);
    RETURN_NEW_RESULT(result, new CClsModelIdentifier(argv[0]->getValue()));
  }
  ANSWERER(CClsModelIdentifier, ciAnswererSet, ciActionSet);
  ErrType ciActionSet(CClsBase **result, CConxClsMessage &o)
  {
    ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsCharacterArray, argv, o);
    CHECK_READ_ONLYNESS(result);
    setValue(argv[0]->getValue());
    RETURN_THIS(result);
  }
private:
  static void initializeAnsweringMachines();

private:
  mutable Model mdl;
  static Answerers *ansMachs;
}; // class CClsModelIdentifier


//////////////////////////////////////////////////////////////////////////////
// Our Color class.
class CClsColor : VIRT public CClsBase {
public:
  CCONX_CLASSNAME("CClsColor")
  CLSNAME("Color", "I am a color, represented by an HSV triplet or an RGB triplet.")
  CLSTYPE(CClsBase, CLS_COLOR)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE2(CClsColor)
public:
  CClsColor() { }
  CClsColor(const CConxColor &c)
  {
    color = c;
  }
  CClsColor(const CClsColor &o)
    : CClsBase(o)
  {
    MMM("copy constructor");
    uninitializedCopy(o);
  }
  ~CClsColor() { MMM("destructor"); }
  CClsColor &operator=(const CClsColor &o)
  {
    MMM("operator=");
    (void) CClsBase::operator=(o);
    uninitializedCopy(o);
    return *this;
  }
  CConxString printString() const
  {
    if (isClassInstance()) {
      return getClsName();
    } else {
      return CConxString("Color(RGB=[") + getColor().getR() + ", "
        + getColor().getG() + ", " + getColor().getB() + "])";
    }
  }
  int operator==(const CClsColor &o) { return getColor().equals(o.getColor()); }
  int operator!=(const CClsColor &o) { return !operator==(o); }
  const CConxNamedColor &getColor() const { return color; }
  void setColor(const CConxColor &o) { color = o; }

private:
  void uninitializedCopy(const CClsColor &o)
  {
    color = o.color;
  }

protected:
  NEW_OI_ANSWERER(CClsColor);
  ANSWERER(CClsColor, ciAnswererRGB, ciActionRGB);
  ErrType ciActionRGB(CClsBase **result, CConxClsMessage &o) const
  {
    return ciMetaActionNewSet(result, o, CConxColor::RGB);
  }
  ANSWERER(CClsColor, ciAnswererHSV, ciActionHSV);
  ErrType ciActionHSV(CClsBase **result, CConxClsMessage &o) const
  {
    return ciMetaActionNewSet(result, o, CConxColor::HSV);
  }
  ANSWERER(CClsColor, oiAnswererRGB, oiActionRGB);
  ErrType oiActionRGB(CClsBase **result, CConxClsMessage &o)
  {
    CHECK_READ_ONLYNESS(result);
    NEED_N_FLOATS(3, fargv, o, result);
    color.setRGB(fargv[0], fargv[1], fargv[2]);
    RETURN_THIS(result);
  }
  ANSWERER(CClsColor, oiAnswererHSV, oiActionHSV);
  ErrType oiActionHSV(CClsBase **result, CConxClsMessage &o)
  {
    CHECK_READ_ONLYNESS(result);
    NEED_N_FLOATS(3, fargv, o, result);
    color.setHSV(fargv[0], fargv[1], fargv[2]);
    RETURN_THIS(result);
  }

  ErrType ciMetaActionNewSet(CClsBase **result, CConxClsMessage &o,
                             CConxColor::ColorType cType) const
  {
    NEED_N_FLOATS(3, fargv, o, result);
    RETURN_NEW_RESULT(result, new CClsColor(CConxColor(fargv[0], fargv[1],
                                                       fargv[2], cType)));
    // DLC clamp in this constructor
  }

#define CCLSCOLOR_GETTER(component) \
  ErrType oiAction ## component(CClsBase **result, \
                                CConxClsMessage &o) const \
  { \
    return ciMetaActionGet(result, o, CConxColor:: ## component); \
  } \
  ANSWERER(CClsColor, oiAnswerer ## component, oiAction ## component)

  CCLSCOLOR_GETTER(H);
  CCLSCOLOR_GETTER(S);
  CCLSCOLOR_GETTER(V);
  CCLSCOLOR_GETTER(R);
  CCLSCOLOR_GETTER(G);
  CCLSCOLOR_GETTER(B);
  ErrType ciMetaActionGet(CClsBase **result, CConxClsMessage &o,
                          CConxColor::Component comp) const
    // DLC make all actions (that should be) const
  {
    RETURN_NEW_RESULT(result, new CClsFloat(getColor().get(comp)));
  }
private:
  static void initializeAnsweringMachines();


private: // attributes
  CConxNamedColor color;
  static Answerers *ansMachs;
}; // class CClsColor


//////////////////////////////////////////////////////////////////////////////
// Our CConxDwGeomObj wrapper.
class CClsDrawable : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsDrawable")
  CLSNAME("Drawable", "I am a thing that knows how to draw itself for your viewing enjoyment.")
  CLSTYPE(CClsBase, CLS_DRAWABLE)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE(CClsDrawable)
public:
  CClsDrawable()
  {
    // initialize so that getValue will work.
    CConxDwGeomObj r;
    r.setColor(CConxNamedColor(CConxNamedColor::GREEN));
    r.setDrawingMethod(r.BRESENHAM);
    init(); setValue(r);
  }
  CClsDrawable(const CConxDwGeomObj &od)
  {
    init();
    setValue(od);
  }
  ~CClsDrawable() { MMM("destructor"); clear(); }
  CClsDrawable(const CClsDrawable &o)
    : CClsBase(o)
  {
    MMM("copy constructor");
    uninitializedCopy(o);
  }
  CClsDrawable &operator=(const CClsDrawable &o)
  {
    (void) CClsBase::operator=(o);
    clear();
    uninitializedCopy(o);
    return *this;
  }
  CClsBase *stCloneDeep() const
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

  void setValue(const CConxDwGeomObj &od)
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
  CConxDwGeomObj getValue() const
  // DLC optimize, track changes and only make a new one when changes occur.
  {
    assert(color != NULL && withGarnish != NULL && lwtol != NULL
           && thickness != NULL && drawingMethod != NULL);
    CConxDwGeomObj r;
    affectValue(r);
    return r;
  }
  void affectValue(CConxDwGeomObj &r) const
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
  CConxString printString() const
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
  int operator==(const CClsDrawable &o) { return getValue() == o.getValue(); }
  int operator!=(const CClsDrawable &o) { return !operator==(o); }
  void clear()
  {
    MAYBE_REMOVE_A_USER(color);
    MAYBE_REMOVE_A_USER(thickness);
    MAYBE_REMOVE_A_USER(lwtol);
    MAYBE_REMOVE_A_USER(withGarnish);
    MAYBE_REMOVE_A_USER(drawingMethod);
  }
  virtual const CConxArtist *getArtist() const
  // We define this only so that Drawable is not an abstract class.  It could
  // be, but we'd like to test it alone from any of its derivations.
  {
    MMM("virtual const CConxArtist *getArtist() const");
    return NULL;
  }
  void makeReadOnly()
  {
    CClsBase::makeReadOnly();

    assert(color != NULL); assert(withGarnish != NULL);
    assert(drawingMethod != NULL); assert(thickness != NULL);
    assert(lwtol != NULL);
    color->makeReadOnly(); withGarnish->makeReadOnly();
    drawingMethod->makeReadOnly(); thickness->makeReadOnly();
    lwtol->makeReadOnly();
  }

protected:
  NEW_OI_ANSWERER(CClsDrawable);
  ANS_GETTER(CClsDrawable, Color, color);
  ANS_GETTER(CClsDrawable, DrawWithGarnish, withGarnish);
  ANS_GETTER(CClsDrawable, Thickness, thickness);
  ANS_GETTER(CClsDrawable, LongwayMethodTolerance, lwtol);
  ANS_GETTER(CClsDrawable, DrawingMethod, drawingMethod);
  ANS_SETTER(CClsDrawable, CClsColor, ColorColon, color);
  ANS_SETTER(CClsDrawable, CClsBoolean, DrawWithGarnishColon, withGarnish);
  ANS_SETTER(CClsDrawable, CClsNumber, ThicknessColon, thickness);
  ANS_SETTER(CClsDrawable, CClsNumber, LongwayMethodToleranceColon, lwtol);
  ANS_SETTER(CClsDrawable, CClsSymbol, DrawingMethodColon, drawingMethod);
private:
  static void initializeAnsweringMachines()
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

private: // operations
  void init()
  {
    color = NULL; withGarnish = NULL; drawingMethod = NULL; thickness = NULL;
    lwtol = NULL;
  }
  void uninitializedCopy(const CClsDrawable &o)
  {
    init();
    setValue(o.getValue());
  }


private: // attributes
  CClsColor *color;
  CClsBoolean *withGarnish;
  CClsSymbol *drawingMethod;
  CClsNumber *thickness, *lwtol;
  static Answerers *ansMachs;
}; // class CClsDrawable


//////////////////////////////////////////////////////////////////////////////
// Our Point class.
// `P := Point x: 3.0 y: 0.2 model: (M := #uhp)'
// `P x: 4.0 model: #uhp'
// `P printString'
class CClsPoint : VIRT public CClsDrawable {
  CCONX_CLASSNAME("CClsPoint")
  CLSNAME("Point", "I am a point in hyperbolic geometry.  I know how to draw myself.")
  CLSTYPE(CClsDrawable, CLS_POINT)
  DEFAULT_SEND_MESSAGE(CClsDrawable)
  ANSMACH_ANSWERS(CClsDrawable)
  STCLONE(CClsPoint)
public:
  CClsPoint() { init(); ensureValidity(); }
  CClsPoint(CClsNumber *x, CClsNumber *y, CClsModelIdentifier *m)
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
  CClsPoint(const CClsDrawable &od)
    : CClsDrawable(od) { init(); ensureValidity(); }
  CClsPoint(const CConxDwPoint &od)
    : CClsDrawable(od) { init(); setValue(od); }
  CClsPoint(const CConxPoint &od) { init(); setValue(od); }
  ~CClsPoint() { MMM("destructor"); clear(); invalidateSavedArtist(); }
  CClsPoint(const CClsPoint &o)
    : CClsDrawable(o)
  {
    MMM("copy constructor");
    init(); uninitializedCopy(o);
  }
  CClsPoint &operator=(const CClsPoint &o)
  {
    (void) CClsDrawable::operator=(o);
    clear();
    uninitializedCopy(o);
    return *this;
  }
  CClsBase *stCloneDeep() const
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

  CConxString printString() const;
  void setValue(const CConxPoint &o, ConxModlType m = CONX_POINCARE_UHP)
  {
    clear();
    xCoord = new CClsFloat(o.getX(m));
    yCoord = new CClsFloat(o.getY(m));
    model = new CClsModelIdentifier(m);
    if (xCoord == NULL || yCoord == NULL || model == NULL) OOM();
    xCoord->incrementUsers();
    yCoord->incrementUsers();
    model->incrementUsers();
    invalidateSavedArtist();
  }
  void setValue(const CConxDwPoint &o, ConxModlType m = CONX_POINCARE_UHP)
  {
    CClsDrawable::setValue(o);
    setValue(o.getPoint(), m);
  }
  CConxPoint getValue() const throw(CClsError *)
  {
    assert(xCoord != NULL && yCoord != NULL && model != NULL);
    if (model->model() == CClsModelIdentifier::NONE) {
      CClsError *ne = new CClsError("the point's model is indecipherable");
      throw ne; // DLC check oom
    }
    return CConxPoint(xCoord->getFloatValue(), yCoord->getFloatValue(),
                      model->cModel());
  }
  void setReadOnly(Boole readOnly)
  {
    // From here on out, the artist will be saved.
    CClsDrawable::setReadOnly(readOnly);
    invalidateSavedArtist();
  }
  const CConxArtist *getArtist() const
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
      savedArtist = new CConxDwPoint(getDwValue());
      if (savedArtist == NULL) OOM();
    }
    return savedArtist;
  }
  CConxDwPoint getDwValue() const throw(CClsError *)
  {
    CConxDwPoint p(getValue()); // may throw
    affectValue(p);
    return p;
  }
  int operator==(const CClsPoint &o) { return getValue() == o.getValue()
                                         && getDwValue() == getDwValue(); }
  int operator!=(const CClsPoint &o) { return !operator==(o); }
  void clear()
  {
    MAYBE_REMOVE_A_USER(xCoord);
    MAYBE_REMOVE_A_USER(yCoord);
    MAYBE_REMOVE_A_USER(model);
    invalidateSavedArtist();
  }
  void makeReadOnly()
  {
    CClsDrawable::makeReadOnly();
    assert(model != NULL); assert(xCoord != NULL); assert(yCoord != NULL);
    xCoord->makeReadOnly();
    yCoord->makeReadOnly();
    model->makeReadOnly();
  }


protected:
  void invalidateSavedArtist() const
  {
    if (savedArtist != NULL)
      delete savedArtist;
    savedArtist = NULL;
  }

private:
  void ensureValidity()
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
  void init() { xCoord = yCoord = NULL; model = NULL; savedArtist = NULL; }
  void uninitializedCopy(const CClsPoint &o)
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

protected:
  NEW_OI_ANSWERER(CClsPoint);
  ANS_GETTER(CClsPoint, X, xCoord);
  ANS_GETTER(CClsPoint, Y, yCoord);
  ANS_GETTER(CClsPoint, Model, model);
  ANS_SETTER(CClsPoint, CClsNumber, XColon, xCoord);
  ANS_SETTER(CClsPoint, CClsNumber, YColon, yCoord);
  ANS_SETTER(CClsPoint, CClsModelIdentifier, ModelColon, model);
  ANSWERER(CClsPoint, ciAnswererXYModel, ciActionXYModel);
  ErrType ciActionXYModel(CClsBase **result, CConxClsMessage &o) const
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
  ANSWERER(CClsPoint, oiAnswererDistanceFromPoint, oiActionDistanceFromPoint);
  ErrType oiActionDistanceFromPoint(CClsBase **result, CConxClsMessage &o) const
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
  ANSWERER(CClsPoint, oiAnswererBetweenAnd, oiActionBetweenAnd);
  ErrType oiActionBetweenAnd(CClsBase **result, CConxClsMessage &o) const
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
private:
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
      ansMachs->append(CConxClsAnsMach("new",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererNew,
                                       "Returns a new object instance of a 2-D point in Hyperbolic geometry" DRAWABLE_STR));
      ansMachs->append(CConxClsAnsMach("x:y:model:",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererXYModel,
                                       "Returns a new, set object instance of a 2-D point in Hyperbolic geometry" DRAWABLE_STR));
      ansMachs->append(CConxClsAnsMach("x:y:m:",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererXYModel,
                                       "Returns a new, set object instance of a 2-D point in Hyperbolic geometry" DRAWABLE_STR));
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


private: // attributes
  CClsNumber *xCoord, *yCoord;
  CClsModelIdentifier *model;
  mutable CConxDwPoint *savedArtist;
  static Answerers *ansMachs;
}; // class CClsPoint


//////////////////////////////////////////////////////////////////////////////
// Our Line class.
class CClsLine : VIRT public CClsDrawable {
  CCONX_CLASSNAME("CClsLine")
  CLSNAME("Line", "I am a line in hyperbolic geometry.  I know how to draw myself.")
  CLSTYPE(CClsDrawable, CLS_LINE)
  DEFAULT_SEND_MESSAGE(CClsDrawable)
  ANSMACH_ANSWERS(CClsDrawable)
  STCLONE(CClsLine)
public:
  CClsLine() { init(); ensureValidity(); }
  CClsLine(CClsPoint *A, CClsPoint *B, CClsBoolean *seg)
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
  CClsLine(const CClsDrawable &od)
    : CClsDrawable(od) { init(); ensureValidity(); }
  CClsLine(const CConxDwLine &od)
    : CClsDrawable(od) { init(); setValue(od); }
  CClsLine(const CConxLine &od) { init(); setValue(od); }
  ~CClsLine() { MMM("destructor"); clear(); invalidateSavedArtist(); }
  CClsLine(const CClsLine &o)
    : CClsDrawable(o)
  {
    MMM("copy constructor");
    init(); uninitializedCopy(o);
  }
  CClsLine &operator=(const CClsLine &o)
  {
    (void) CClsDrawable::operator=(o);
    clear();
    uninitializedCopy(o);
    return *this;
  }
  CClsBase *stCloneDeep() const
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

  CConxString printString() const
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
  void setValue(const CConxLine &o)
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
  void setValue(const CConxDwLine &o)
  {
    CClsDrawable::setValue(o);
    setValue(o.getLine());
  }
  CConxLine getValue() const throw(CClsError *)
  {
    assert(P[0] != NULL && P[1] != NULL && isSeg != NULL);
    // This may throw if one of the points is invalid:
    return CConxLine(P[0]->getValue(), P[1]->getValue(), isSeg->getValue());
  }
  const CConxArtist *getArtist() const
  {
    MMM("virtual const CConxArtist *getArtist() const");

    // See comment in CClsPoint::getArtist for more:
    invalidateSavedArtist();
    if (savedArtist == NULL) {
      savedArtist = new CConxDwLine(getDwValue());
      if (savedArtist == NULL) OOM();
    }
    return savedArtist;
  }
  CConxDwLine getDwValue() const throw(CClsError *)
  {
    CConxDwLine p(getValue()); // may throw
    affectValue(p);
    return p;
  }
  int operator==(const CClsLine &o) { return getValue() == o.getValue()
                                        && getDwValue() == o.getDwValue(); }
  int operator!=(const CClsLine &o) { return !operator==(o); }
  void clear()
  {
    MAYBE_REMOVE_A_USER(P[0]);
    MAYBE_REMOVE_A_USER(P[1]);
    MAYBE_REMOVE_A_USER(isSeg);
    invalidateSavedArtist();
  }
  void makeReadOnly()
  {
    CClsDrawable::makeReadOnly();
    assert(isSeg != NULL); assert(P[0] != NULL); assert(P[1] != NULL);
    P[0]->makeReadOnly(); P[1]->makeReadOnly(); isSeg->makeReadOnly();
  }

protected:
  void invalidateSavedArtist() const
  {
    if (savedArtist != NULL)
      delete savedArtist;
    savedArtist = NULL;
  }

private:
  void ensureValidity()
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
  void init() { P[0] = P[1] = NULL; isSeg = NULL; savedArtist = NULL; }
  void uninitializedCopy(const CClsLine &o)
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

protected:
  ANS_GETTER(CClsLine, A, P[0]);
  ANS_GETTER(CClsLine, B, P[1]);
  ANS_GETTER(CClsLine, IsSegment, isSeg);
  ANS_SETTER(CClsLine, CClsPoint, AColon, P[0]);
  ANS_SETTER(CClsLine, CClsPoint, BColon, P[1]);
  ANS_SETTER(CClsLine, CClsBoolean, IsSegmentColon, isSeg);
  NEW_OI_ANSWERER(CClsLine);
  ANSWERER(CClsLine, ciAnswererABIsSegment, ciActionABIsSegment);
  ErrType ciActionABIsSegment(CClsBase **result, CConxClsMessage &o) const
  {
    CClsBase *argv[3]; o.getBoundObjects(argv);
    for (int ai = 0; ai < 2; ai++)
      ENSURE_KEYWD_TYPE(result, o, argv, ai, CLS_POINT, TRUE);
    ENSURE_KEYWD_TYPE(result, o, argv, 2, CLS_BOOLEAN, TRUE);
    CClsPoint *A = (CClsPoint *)argv[0], *B = (CClsPoint *)argv[1];
    CClsBoolean *isSegment = (CClsBoolean *) argv[2];
    RETURN_NEW_RESULT(result, new CClsLine(A, B, isSegment));
  }
private:
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
      ansMachs->append(CConxClsAnsMach("new",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererNew,
                                       "Returns a new object instance of a line in Hyperbolic geometry" DRAWABLE_STR));
      ansMachs->append(CConxClsAnsMach("A:B:isSegment:",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererABIsSegment,
                                       "Returns a new, set object instance of a 2-D line in Hyperbolic geometry" DRAWABLE_STR));
      ADD_ANS_GETTER("A", A);
      ADD_ANS_GETTER("B", B);
      ADD_ANS_GETTER("isSegment", IsSegment);
      ADD_ANS_SETTER("A:", AColon);
      ADD_ANS_SETTER("B:", BColon);
      ADD_ANS_SETTER("isSegment:", IsSegmentColon);
    }
  }


private: // attributes
  CClsPoint *P[2];
  CClsBoolean *isSeg;
  mutable CConxDwLine *savedArtist;
  static Answerers *ansMachs;
}; // class CClsLine


//////////////////////////////////////////////////////////////////////////////
// Our Circle class.
class CClsCircle : VIRT public CClsDrawable {
  CCONX_CLASSNAME("CClsCircle")
  CLSNAME("Circle", "I am a circle in hyperbolic geometry.  I know how to draw myself.")
  CLSTYPE(CClsDrawable, CLS_CIRCLE)
  DEFAULT_SEND_MESSAGE(CClsDrawable)
  ANSMACH_ANSWERS(CClsDrawable)
  STCLONE(CClsCircle)
public:
  CClsCircle() { init(); ensureValidity(); }
  CClsCircle(CClsPoint *Center, CClsNumber *Radius)
  {
    init();
    radius = Radius;
    center = Center;
    if (radius != NULL) radius->incrementUsers();
    if (center != NULL) center->incrementUsers();
    ensureValidity();
  }
  CClsCircle(const CClsDrawable &od)
    : CClsDrawable(od) { init(); ensureValidity(); }
  CClsCircle(const CConxDwCircle &od)
    : CClsDrawable(od) { init(); setValue(od); }
  CClsCircle(const CConxCircle &od) { init(); setValue(od); }
  ~CClsCircle() { MMM("destructor"); clear(); invalidateSavedArtist(); }
  CClsCircle(const CClsCircle &o)
    : CClsDrawable(o)
  {
    MMM("copy constructor");
    init(); uninitializedCopy(o);
  }
  CClsCircle &operator=(const CClsCircle &o)
  {
    (void) CClsDrawable::operator=(o);
    clear();
    uninitializedCopy(o);
    return *this;
  }
  CClsBase *stCloneDeep() const
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

  CConxString printString() const
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
  void setValue(const CConxCircle &o)
  {
    clear();
    radius = new CClsFloat(o.getRadius());
    center = new CClsPoint(o.getCenter());
    if (radius == NULL || center == NULL) OOM();
    radius->incrementUsers();
    center->incrementUsers();
    invalidateSavedArtist();
  }
  void setValue(const CConxDwCircle &o)
  {
    CClsDrawable::setValue(o);
    setValue(o.getCircle());
  }
  CConxCircle getValue() const throw(CClsError *)
  {
    assert(radius != NULL && center != NULL);
    // This may throw if the center is invalid:
    return CConxCircle(center->getValue(), radius->getFloatValue());
  }
  const CConxArtist *getArtist() const
  {
    MMM("virtual const CConxArtist *getArtist() const");

    // See comment in CClsPoint::getArtist for more:
    invalidateSavedArtist();
    if (savedArtist == NULL) {
      savedArtist = new CConxDwCircle(getDwValue());
      if (savedArtist == NULL) OOM();
    }
    return savedArtist;
  }
  CConxDwCircle getDwValue() const throw(CClsError *)
  {
    CConxDwCircle p(getValue()); // may throw
    affectValue(p);
    return p;
  }
  int operator==(const CClsCircle &o) { return getValue() == o.getValue()
                                        && getDwValue() == o.getDwValue(); }
  int operator!=(const CClsCircle &o) { return !operator==(o); }
  void clear()
  {
    MAYBE_REMOVE_A_USER(radius);
    MAYBE_REMOVE_A_USER(center);
    invalidateSavedArtist();
  }
  void makeReadOnly()
  {
    CClsDrawable::makeReadOnly();
    assert(center != NULL); assert(radius != NULL);
    radius->makeReadOnly(); center->makeReadOnly();
  }

protected:
  void invalidateSavedArtist() const
  {
    if (savedArtist != NULL)
      delete savedArtist;
    savedArtist = NULL;
  }

private:
  void ensureValidity()
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
  void init() { radius = NULL; center = NULL; savedArtist = NULL; }
  void uninitializedCopy(const CClsCircle &o)
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

protected:
  ANS_GETTER(CClsCircle, Center, center);
  ANS_GETTER(CClsCircle, Radius, radius);
  ANS_SETTER(CClsCircle, CClsPoint, CenterColon, center);
  ANS_SETTER(CClsCircle, CClsNumber, RadiusColon, radius);
  NEW_OI_ANSWERER(CClsCircle);
  ANSWERER(CClsCircle, ciAnswererCenterRadius, ciActionCenterRadius);
  ErrType ciActionCenterRadius(CClsBase **result, CConxClsMessage &o) const
  {
    CClsBase *argv[2]; o.getBoundObjects(argv);
    ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_POINT, TRUE);
    ENSURE_KEYWD_TYPE(result, o, argv, 1, CLS_NUMBER, TRUE);
    RETURN_NEW_RESULT(result, new CClsCircle((CClsPoint *)argv[0], (CClsNumber *)argv[1]));
  }
private:
  static void initializeAnsweringMachines()
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


private: // attributes
  CClsPoint *center;
  CClsNumber *radius;
  mutable CConxDwCircle *savedArtist;
  static Answerers *ansMachs;
}; // class CClsCircle


//////////////////////////////////////////////////////////////////////////////
// Our CConxCanvas wrapper
class CClsCanvas : VIRT public CClsArray {
  CCONX_CLASSNAME("CClsCanvas")
  CLSNAME("Canvas", "I am a canvas upon which you can draw Drawable object instances.  I hold onto my objects like an Array.")
  CLSTYPE(CClsArray, CLS_CANVAS)
  DEFAULT_SEND_MESSAGE(CClsArray)
  ANSMACH_ANSWERS(CClsArray)
  STCLONE(CClsCanvas)
public:
  CClsCanvas() { init(); }
  CClsCanvas(CConxCanvas *f) { init(); setValue(f); }
  ~CClsCanvas() { MMM("destructor"); } // clear() does not delete or decrementUsers, so forget it.
  CClsCanvas(const CClsCanvas &o)
    : CClsArray(o)
  {
    MMM("copy constructor");
    uninitializedCopy(o);
    assert(getRequiredType() == CLS_DRAWABLE);
  }
  CClsCanvas(const CClsArray &o)
    : CClsArray(o) { init(); }

  CClsCanvas &operator=(const CClsCanvas &o)
  {
    (void) CClsArray::operator=(o);
    // Forget the old canvas completely.
    init();
    uninitializedCopy(o);
    assert(getRequiredType() == CLS_DRAWABLE);
    return *this;
  }
  CClsBase *stCloneDeep() const
  {
    CClsArray *j = (CClsArray *) CClsArray::stCloneDeep();
    CClsCanvas *v = new CClsCanvas(*j);
    delete j;
    if (v == NULL) OOM();
    v->uninitializedCopy(*this);
    return v;
  }

  const CConxCanvas *getValue() const { return cv; }
  CConxCanvas *getValue() { return cv; }
  void setValue(CConxCanvas *v) { cv = v; } // Forget the old one completely.
  CConxString printString() const
  {
    if (isClassInstance()) {
      return getClsName();
    } else {
      return CConxString("Canvas(")
        + CConxString((long) ((cv != NULL) ? cv->numArtists() : 0))
        + " Drawable object instances, "
        + CConxString((long)numElements())
        + " entries)";
    }
  }
  int operator==(const CClsCanvas &o) { return 0; } // DLC
  int operator!=(const CClsCanvas &o) { return !operator==(o); }
  void clear() { cv->clear(); }
  

private:
  void init() { cv = NULL; setRequiredType(CLS_DRAWABLE); }
  void uninitializedCopy(const CClsCanvas &o)
  {
    cv = o.cv; // Yes, neither owns the canvas -- they share it.
  }

protected:
  NEW_OI_ANSWERER(CClsCanvas); // otherwise `Canvas new' will act as `Array new'
  ANSWERER(CClsCanvas, oiAnswererSync, oiActionSync);
  ErrType oiActionSync(CClsBase **result, CConxClsMessage &o)
  {
    if (cv == NULL) {
      RETURN_ERROR_RESULT(result,
                          "This object instance is not tied to any canvas.");
    }
    CConxSimpleArray<CClsBase *> &a = getSimpleArray();
    cv->clearDrawables();
    LLL("While syncing, we will try " << a.size() << " different possible artists");
    for (size_t i = 0; i < a.size(); i++) {

      assert(a.get(i) != NULL && a.get(i)->isType(CLS_DRAWABLE));
      // We restrict access to read-write functions by implementing a
      // version of each in this class.

      const CConxArtist *artist = ((CClsDrawable *)a.get(i))->getArtist();
      if (artist != NULL) {
        LLL(cout << "newly added artist is " << artist);
        cv->append(artist);
      }
    }
    RETURN_THIS(result); // DLC return this???
  }// DLC test cascading.
private:
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
      ansMachs->append(CConxClsAnsMach("sync",
                                       CConxClsAnsMach::OBJECT,
                                       oiAnswererSync,
                                       "Makes the elements on screen correspond to the contents of this array"));
      ansMachs->append(CConxClsAnsMach("new",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererNew,
                                       "Returns a new canvas that is not connected to a display"));
    }
  }


private: // attributes
  CConxCanvas *cv;
  static Answerers *ansMachs;
}; // class CClsCanvas


//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////
inline CClsModelIdentifier::Model CClsModelIdentifier::model() const
// Will not return NIL, of course.
{
  // DLC use gperf for greater speed.
  if (mdl == NIL) {
    const CConxString &s = getValue();
    // We include dashes and spaces here but the scanner may or may not allow them.
    // 'poincare-disk' is ok but #poincare-disk is illegal.
    if (s.isPrefixOf("kleindisk", 1, FALSE)
        || s.isPrefixOf("kd", 2, FALSE)
        || s.isPrefixOf("klein-disk", 1, FALSE)
        || s.isPrefixOf("beltrami-klein", 1, FALSE)
        || s.isPrefixOf("klein_disk", 1, FALSE)
        || s.isPrefixOf("beltrami_klein", 1, FALSE)
        || s.isPrefixOf("klein disk", 1, FALSE)
        || s.isPrefixOf("beltrami klein", 1, FALSE)
        || s.isPrefixOf("beltramiklein", 1, FALSE)) {
      mdl = KD;
    } else if (s.isPrefixOf("uhp", 1, FALSE)
               || s.isPrefixOf("puhp", CConxString("pu").getLength(), FALSE)
               || s.isPrefixOf("upperhalfplane", 1, FALSE)
               || s.isPrefixOf("poincareuhp",
                               CConxString("poincareu").getLength(), FALSE)
               || s.isPrefixOf("poincare-upper-half-plane",
                               CConxString("poincare-u").getLength(), FALSE)
               || s.isPrefixOf("upper-half-plane", 1, FALSE) // DLC add 'poincare uhp
               || s.isPrefixOf("poincare_upper_half_plane",
                               CConxString("poincare_u").getLength(), FALSE)
               || s.isPrefixOf("upper_half_plane", 1, FALSE)
               || s.isPrefixOf("poincare upper half plane",
                               CConxString("poincare u").getLength(), FALSE)
               || s.isPrefixOf("upper half plane", 1, FALSE)) {
      mdl = UHP;
    } else if (s.isPrefixOf("poincaredisk",
                            CConxString("poincared").getLength(), FALSE)
               || s.isPrefixOf("poincare-disk",
                               CConxString("poincare-d").getLength(), FALSE)
               || s.isPrefixOf("poincare_disk",
                               CConxString("poincare_d").getLength(), FALSE)
               || s.isPrefixOf("poincare disk",
                               CConxString("poincare d").getLength(), FALSE)
               || s.isPrefixOf("pd", 100, FALSE)) {
      mdl = PD;
    } else {
      //      if (s.prefixEquals("klein" || s == 
      mdl = NONE; // DLC finish this.
    }
  }
  assert(mdl == NONE || mdl == PD || mdl == KD || mdl == UHP);
  return mdl;
}

inline CConxString CClsModelIdentifier::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    const char *ms;
    if (mdl == NIL)
      (void) model(); // calculate it.
    assert(mdl != NIL);
    switch(mdl) {
    case NONE: ms = "no valid model"; break;
    case PD: ms = "the Poincare Disk"; break;
    case UHP: ms = "the Poincare Upper Half Plane"; break;
    default: assert(mdl == KD); ms = "the Beltrami-Klein Disk"; break;
    }
    return CConxString("Model Identifier '") + getValue()
      + "' which refers to " + ms;
  }
}

inline CClsModelIdentifier::CClsModelIdentifier(ConxModlType m)
{
  if (m == CONX_POINCARE_UHP) {
    mdl = UHP;
    setValue("uhp");
  } else if (m == CONX_POINCARE_DISK) {
    mdl = PD;
    setValue("pd");
  } else {
    assert(m == CONX_KLEIN_DISK);
    mdl = KD;
    setValue("kd");
  }
}

inline ConxModlType CClsModelIdentifier::cModel() const throw(int)
// Throws if model() == NONE
{
  if (mdl == NIL)
    (void) model(); // calculate it
  if (mdl == NONE) throw(1);
  else if (mdl == PD) return CONX_POINCARE_DISK;
  else if (mdl == KD) return CONX_KLEIN_DISK;
  else return CONX_POINCARE_UHP;
}

inline CClsModelIdentifier &
CClsModelIdentifier::operator=(const CClsModelIdentifier &o)
{
  (void) CClsCharacterArray::operator=(o);
  mdl = o.mdl;
  return *this;
}

inline void CClsModelIdentifier::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();

    // DLC for efficiency, don't allow `Boolean true' and `Boolean false',
    // force copying `true' and `false'.
    ansMachs->append(CConxClsAnsMach("new:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererNew,
                                     "Returns a new object instance that represents the Beltrami-Klein disk, Poincare Disk, Poincare Upper Halp Plane, or an unknown model of hyperbolic geometry, depending on the String or Symbol object instance argument.  Note that `kd', `pd', and `uhp' are reserved variables that are already provided for you, and it is more efficient to use them"));
    ansMachs->append(CConxClsAnsMach("set:",
                                     CConxClsAnsMach::OBJECT,
                                     ciAnswererSet,
                                     "Sets the value of the receiver to the Symbol or String argument"));
  }
}

inline void CClsColor::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ansMachs->append(CConxClsAnsMach("new",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererNew,
                                     "Returns a new Color object instance"));
    ansMachs->append(CConxClsAnsMach("R:G:B:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererRGB,
                                     "Returns a new Color object instance set to a (red in [0.0, 1.0], green in [0.0, 1.0], blue in [0.0, 1.0]) RGB value"));
    ansMachs->append(CConxClsAnsMach("H:S:V:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererHSV,
                                     "Returns a new Color object instance set to a (hue in [0.0, 360.0), saturation in [0.0, 1.0], value in [0.0, 1.0]) HSV value"));

    ansMachs->append(CConxClsAnsMach("R:G:B:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererRGB,
                                     "Sets receiver to the given RGB value."));
    ansMachs->append(CConxClsAnsMach("H:S:V:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererHSV,
                                     "Sets receiver to the given HSV value."));

    ansMachs->append(CConxClsAnsMach("H",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererH,
                                     "Returns the hue"));
    ansMachs->append(CConxClsAnsMach("S", CConxClsAnsMach::OBJECT,
                                     oiAnswererS,
                                     "Returns the saturation"));
    ansMachs->append(CConxClsAnsMach("V", CConxClsAnsMach::OBJECT,
                                     oiAnswererV,
                                     "Returns the HSV Value"));
    ansMachs->append(CConxClsAnsMach("R", CConxClsAnsMach::OBJECT,
                                     oiAnswererR,
                                     "Returns the red component"));
    ansMachs->append(CConxClsAnsMach("G", CConxClsAnsMach::OBJECT,
                                     oiAnswererG,
                                     "Returns the green component"));
    ansMachs->append(CConxClsAnsMach("B", CConxClsAnsMach::OBJECT,
                                     oiAnswererB,
                                     "Returns the blue component"));
  }
}

#endif // GPLCONX_STCONX_CXX_H
