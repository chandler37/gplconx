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
  STCLONE(CClsModelIdentifier)
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


private:
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
  static void initializeAnsweringMachines();

private:
  mutable Model mdl;
  static Answerers *ansMachs;
}; // class CClsModelIdentifier


//////////////////////////////////////////////////////////////////////////////
// Our Point class.
// `P := Point x: 3.0 y: 0.2 model: (M := #uhp)'
// `P x: 4.0 model: #uhp'
// `P printString'
class CClsPoint : VIRT public CClsBase {
  CCONX_CLASSNAME("CClsPoint")
  CLSNAME("Point", "I am a point in hyperbolic geometry.")
  CLSTYPE(CClsBase, CLS_POINT)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE(CClsPoint)
public:
  CClsPoint() { xCoord = yCoord = NULL; model = NULL; ensureValidity(); }
  CClsPoint(CClsNumber *x, CClsNumber *y, CClsModelIdentifier *m)
  {
    xCoord = x;
    yCoord = y;
    model = m;
    if (xCoord != NULL) xCoord->incrementUsers();
    if (yCoord != NULL) yCoord->incrementUsers();
    if (model != NULL) model->incrementUsers();
    ensureValidity();
  }
  CClsPoint(const CConxPoint &od)
  {
    ConxModlType m = CONX_POINCARE_UHP;
    xCoord = new CClsFloat(od.getX(m));
    yCoord = new CClsFloat(od.getY(m));
    model = new CClsModelIdentifier(m);
    if (xCoord == NULL || yCoord == NULL || model == NULL) OOM();
    xCoord->incrementUsers();
    yCoord->incrementUsers();
    model->incrementUsers();
  }
  ~CClsPoint() { clear(); }
  CClsPoint(const CClsPoint &o)
    : CClsBase(o)
  {
    MMM("copy constructor");
    uninitializedCopy(o);
  }
  CClsPoint &operator=(const CClsPoint &o)
  {
    (void) CClsBase::operator=(o);
    clear();
    uninitializedCopy(o);
    return *this;
  }
  CConxString printString() const; // DLC where??
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
  int operator==(const CClsPoint &o) { return getValue() == o.getValue(); }
  int operator!=(const CClsPoint &o) { return !operator==(o); }
  void clear()
  {
    MAYBE_REMOVE_A_USER(xCoord);
    MAYBE_REMOVE_A_USER(yCoord);
    MAYBE_REMOVE_A_USER(model);
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
  void uninitializedCopy(const CClsPoint &o)
  {
    xCoord = o.xCoord;
    yCoord = o.yCoord;
    model = o.model;
    if (xCoord != NULL) xCoord->incrementUsers();
    if (yCoord != NULL) yCoord->incrementUsers();
    if (model != NULL) model->incrementUsers();
  }

private:
  ANS_GETTER(CClsPoint, X, xCoord);
  ANS_GETTER(CClsPoint, Y, yCoord);
  ANS_GETTER(CClsPoint, Model, model);
  ANSWERER(CClsPoint, ciAnswererNew, ciActionNew);
  ErrType ciActionNew(CClsBase **result, CConxClsMessage &o) const
  {
    RETURN_NEW_RESULT(result, new CClsPoint());
  }
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

  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
      ansMachs->append(CConxClsAnsMach("new",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererNew,
                                       "Returns a new object instance of a 2-D point in Hyperbolic geometry"));
      ansMachs->append(CConxClsAnsMach("x:y:model:",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererXYModel,
                                       "Returns a new, set object instance of a 2-D point in Hyperbolic geometry"));
      ansMachs->append(CConxClsAnsMach("x:y:m:",
                                       CConxClsAnsMach::CLASS,
                                       ciAnswererXYModel,
                                       "Returns a new, set object instance of a 2-D point in Hyperbolic geometry"));
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
    }
    // DLC add setters, add CClsBase set: CClsBase general assignment operator
    // invoker.
  }


private: // attributes
  CClsNumber *xCoord, *yCoord;
  CClsModelIdentifier *model;
  static Answerers *ansMachs;
}; // class CClsPoint


//////////////////////////////////////////////////////////////////////////////
// Our Color class.
class CClsColor : VIRT public CClsBase {
public:
  CCONX_CLASSNAME("CClsColor")
  CLSNAME("Color", "I am a color, represented by an HSV triplet or an RGB triplet.")
  CLSTYPE(CClsBase, CLS_COLOR)
  DEFAULT_SEND_MESSAGE(CClsBase)
  ANSMACH_ANSWERS(CClsBase)
  STCLONE(CClsColor)
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

private:
  ANSWERER(CClsColor, ciAnswererNew, ciActionNew);
  ErrType ciActionNew(CClsBase **result, CConxClsMessage &o) const
  {
    RETURN_NEW_RESULT(result, new CClsColor());
  }
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
  ~CClsDrawable()
  {
    clear();
  }
  CClsDrawable(const CClsDrawable &o)
    : CClsBase(o)
  {
    MMM("CClsDrawable(const CClsDrawable &o)");
    uninitializedCopy(o);
  }
  CClsDrawable &operator=(const CClsDrawable &o)
  {
    (void) CClsBase::operator=(o);
    clear();
    uninitializedCopy(o);
    return *this;
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
    r.setColor(color->getColor());
    if (drawingMethod->getValue() == "longway") { // DLC #longway
      r.setDrawingMethod(r.LONGWAY);
    } else {
      r.setDrawingMethod(r.BRESENHAM);
    }
    r.setThickness(thickness->getValue());
    r.setLongwayTolerance(lwtol->getValue());
    r.setGarnishing(withGarnish->getValue());
    return r;
  }
  CConxString printString() const
  {
    if (isClassInstance()) {
      return getClsName();
    } else {
      CConxString s = "Drawable -- ";
      s += color->printString() + ", garnishing "
        + (withGarnish->getValue() ? "on" : "off") + ", thickness "
        + thickness->getValue() + ", slow drawing method tolerance "
        + lwtol->getValue() + ", drawing method "
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
  {
    return NULL;
  }

private:
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

private:
  ANSWERER(CClsDrawable, ciAnswererNew, ciActionNew);
  ErrType ciActionNew(CClsBase **result, CConxClsMessage &o) const
  {
    RETURN_NEW_RESULT(result, new CClsDrawable());
  }

  ANS_GETTER(CClsDrawable, Color, color);
  ANS_GETTER(CClsDrawable, DrawWithGarnish, withGarnish);
  ANS_GETTER(CClsDrawable, Thickness, thickness);
  ANS_GETTER(CClsDrawable, LongwayMethodTolerance, lwtol);
  ANS_GETTER(CClsDrawable, DrawingMethod, drawingMethod);

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
    }
  }


private: // attributes
  CClsColor *color;
  CClsBoolean *withGarnish;
  CClsSymbol *drawingMethod;
  CClsFloat *thickness, *lwtol;
  static Answerers *ansMachs;
}; // class CClsDrawable


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
  ~CClsCanvas() { } // clear() does not delete or decrementUsers, so forget it.
  CClsCanvas(const CClsCanvas &o)
    : CClsArray(o)
  {
    MMM("CClsCanvas(const CClsCanvas &o)");
    uninitializedCopy(o);
  }
  CClsCanvas &operator=(const CClsCanvas &o)
  {
    (void) CClsArray::operator=(o);
    // Forget the old canvas completely.
    init();
    uninitializedCopy(o);
    return *this;
  }
  const CConxCanvas *getValue() const { return cv; }
  CConxCanvas *getValue() { return cv; }
  void setValue(CConxCanvas *v) { cv = v; } // Forget the old one completely.
  CConxString printString() const
  {
    if (isClassInstance()) {
      return getClsName();
    } else {
      return CConxString("Canvas with ")
        + CConxString((long) ((cv != NULL) ? cv->numArtists() : 0))
        + " Drawable object instances";
    }
  }
  int operator==(const CClsCanvas &o) { return 0; } // DLC
  int operator!=(const CClsCanvas &o) { return !operator==(o); }
  void clear() { cv->clear(); }
  

private:
  void init() { cv = NULL; }
  void uninitializedCopy(const CClsCanvas &o)
  {
    cv = o.cv; // Yes, neither owns the canvas -- they share it.
  }

private:
  ANSWERER(CClsCanvas, oiAnswererSync, oiActionSync);
  ErrType oiActionSync(CClsBase **result, CConxClsMessage &o)
  {
    if (cv == NULL) {
      RETURN_ERROR_RESULT(result,
                          "This object instance is not tied to any canvas.");
    }
    CConxSimpleArray<CClsBase *> &a = getSimpleArray();
    cv->clearDrawables();
    for (size_t i = 0; i < a.size(); i++) {
      const CConxArtist *artist = ((CClsDrawable *)a.get(i))->getArtist();
      if (artist != NULL) cv->append(artist);
    }
    RETURN_THIS(result); // DLC return this???
  }// DLC test cascading.
  ANSWERER(CClsCanvas, oiAnswererAtPut, oiActionAtPut);
  ErrType oiActionAtPut(CClsBase **result, CConxClsMessage &o)
  {
    assert(result != NULL); CClsBase *argv[2]; o.getBoundObjects(argv);
    ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_SMALLINT, TRUE);
    ENSURE_KEYWD_TYPE(result, o, argv, 1, CLS_DRAWABLE, TRUE);
    return CClsArray::oiActionAtPut(result, o);
  }
  static void initializeAnsweringMachines()
  {
    if (ansMachs == NULL) {
      ansMachs = new Answerers();
      if (ansMachs == NULL) OOM();
      ansMachs->append(CConxClsAnsMach("at:put:",
                                       CConxClsAnsMach::OBJECT,
                                       oiAnswererAtPut,
                                       "Sets the element at the position specified by the strictly positive integer argument to `at:' to the Drawable object instance that is the `put:' argument"));
      ansMachs->append(CConxClsAnsMach("sync",
                                       CConxClsAnsMach::OBJECT,
                                       oiAnswererSync,
                                       "Makes the elements on screen correspond to the contents of this array"));
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
