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
  Implementation of C++ classes in `dgeomobj.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include "dgeomobj.hh"
#include "canvas.hh"


NF_INLINE
void CConxDwGeomObj::drawOn(CConxCanvas &cv) const throw(int)
{
  MMM("void drawOn(CConxCanvas &cv) const throw(int)");
  // DLC How do you garnish a point??? `getGarnishing()' makes no difference.
  // The method makes no difference either.  If we did use a stored drawing,
  // we would inherit with protected access and then define our own
  // versions of setGarnishing and setDrawingMethod so that setValidity
  // would not be called.  (Other methods would have their access raised.)
  // This is necessary even with a virtual setGarnishing if you
  // assume the worst of the class's users, i.e. that they will say
  // ((CConxDwGeomObj *)&cconxdwpoint)->CConxDwGeomObj::setGarnishing(TRUE)
  // just to make us less efficient.

  // A stored drawing is too much overhead for as simple a thing as this.


  // DLC test requiresHeavyComputation() and make a display list (maybe not
  // in this function).
  if (P == NULL) throw 38;
  cv.setDrawingColor(getColor());
  cv.setPointSize(getThickness());
  if (getGarnishing()) {
    P->drawGarnishOn(cv);
  }
  switch (getDrawingMethod()) {
  case LONGWAY:
  case SAFEST:
    drawLongway(cv, *P);
    break;
  case BRESENHAM:
  case BEST:
    P->drawBresenhamOn(cv);
    break;
  }

// For those that use SD's, setValidity(TRUE) if startSD did not throw by now.
}

NF_INLINE
double CConxDwGeomObj::longwayMetric(Pt x, void *t)
{
  // DLC static object, affects instance count (debugging is harder)
  static CConxPoint sPoint;
  assert(t != NULL);
  assert(((const CConxDwGeomObj *)t)->P != NULL);
  sPoint.setPoint(x, ((const CConxDwGeomObj *)t)->getLongwaySavedModel());
  return (((const CConxDwGeomObj *)t)->P)->definingFunction(sPoint);
}

NF_INLINE
void CConxDwGeomObj::longwayDrawVertex(double a, double b, void *t)
{
  (((const CConxDwGeomObj *)t)->getStoredCanvas())->drawVertex(a, b);
}

NF_INLINE
void CConxDwGeomObj::drawLongway(CConxCanvas &cv,
                                 const CConxSimpleArtist &o) const
{
  saveLongwayModel(cv.getModel());
  // DLC  CONX_BEGIN_DISP_LIST(dl);
  cv.beginDraw(cv.POINTS);
  storeCanvas(&cv);
#ifdef HAVE_CONST_CAST
  CConxDwGeomObj *constlessThis = const_cast< CConxDwGeomObj * >( this );
#else
  CConxDwGeomObj *constlessThis = this;
#endif
  conx_longway(longwayMetric, constlessThis, cv.getModel(),
               getLongwayTolerance(),
               cv.getPixelWidth(), cv.getPixelHeight(),
               cv.getXmin(), cv.getXmax(), cv.getYmin(), cv.getYmax(),
               longwayDrawVertex, constlessThis);
  removeStoredCanvas();
  cv.endDraw();
  // DLC  CONX_END_DISP_LIST(dl);
}

NF_INLINE
CConxArtist &CConxArtist::operator=(const CConxArtist &o)
{
  (void) CConxObject::operator=(o);
  return *this;
}

NF_INLINE
void CConxArtist::drawOn(class CConxCanvas &o) const
{
  MMM("void drawOn(class CConxCanvas &o) const");
}

CF_INLINE
CConxDwGeomObj::CConxDwGeomObj(CConxSimpleArtist *p)
{
  init();
  setGeomObj(p);  // We own p now!
}

CF_INLINE
CConxDwGeomObj::CConxDwGeomObj(const CConxSimpleArtist &p)
{
  init();
  setGeomObj(p.clone());
}

CF_INLINE
CConxDwGeomObj::CConxDwGeomObj(const CConxDwGeomObj &o)
  : CConxArtist(o)
{
  uninitializedCopy(o);
}

NF_INLINE
int CConxDwGeomObj::operator==(const CConxDwGeomObj &o) const
{
  return (getColor().equals(o.getColor())
          && getGarnishing() == o.getGarnishing()
          && getThickness() == o.getThickness()
          && getLongwayTolerance() == o.getLongwayTolerance()
          && getDrawingMethod() == o.getDrawingMethod()
          && ((P == NULL && o.P == NULL)
              || (P != NULL && o.P != NULL && P->eql(*o.P))));
}

NF_INLINE
CConxDwGeomObj &CConxDwGeomObj::operator=(const CConxDwGeomObj &o)
{
  (void) CConxArtist::operator=(o);
  clear();
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
CConxDwGeomObj::~CConxDwGeomObj()
{
  MMM("destructor"); LLL("Deleting " << this);
  /* No, don't delete the stored canvas. */
  clear();
}

NF_INLINE
void CConxDwGeomObj::setColor(const CConxColor &c)
{
  if (!color.equals(c)) {
    setValidity(FALSE);
    color = c;
  }
}

NF_INLINE
void CConxDwGeomObj::setColor(CConxNamedColor::ColorName cn)
{
  if (!color.isNamed(cn)) {
    setValidity(FALSE);
    color.setColorByName(cn);
  }
}

NF_INLINE
void CConxDwGeomObj::setGarnishing(Boole g)
{
  if (withGarnish != g) {
    setValidity(FALSE);
    withGarnish = g;
  }
}

NF_INLINE
void CConxDwGeomObj::setDrawingMethod(DrawingMethod m)
{
  if (dm != m) {
    setValidity(FALSE);
    dm = m;
  }
}

NF_INLINE
ostream &CConxDwGeomObj::printOn(ostream &o) const
{
  o << "<CConxDwGeomObj object {";

  const CConxSimpleArtist *ggo = getGeomObj();
  if (ggo == NULL) {
    o << "Not associated with any CConxSimpleArtist";
  } else {
    o << CConxSimpleArtist::humanSAType(ggo->getSAType()) << ": ";
    ggo->printOn(o);
  }

  o << "} color=";
  color.printOn(o);
  o << ", isValid=" << BOOLE2STRING(isValid) << ", withGarnish="
    << BOOLE2STRING(withGarnish) << ", drawingMethod="
    << drawingMethodToString(dm) << ", thickness=" << thickness
    << ", lwtol=" << lwtol
    << ">";
  return o;
}

NF_INLINE
const char *CConxDwGeomObj::drawingMethodToString(DrawingMethod m)
{
  switch (m) {
  case SAFEST: return "SAFEST";
  case BRESENHAM: return "BRESENHAM";
  case LONGWAY: return "LONGWAY";
  default: assert(m == BEST); return "BEST";
  }
}

NF_INLINE
CConxSimpleArtist *CConxDwGeomObj::getGeomObj()
{
  return P;
  /* DLC setValidity(FALSE); ?? */
}

NF_INLINE
void CConxDwGeomObj::setGeomObj(CConxSimpleArtist *n)
{
  /* DLC setValidity(FALSE); ?? */
  clear();
  P = n;
}

NF_INLINE
void CConxDwGeomObj::clear()
{
  /* DLC setValidity(FALSE); ?? */
  if (P != NULL) {
    LLL("DELETING DWGEOMOBJ OWNED MEMBER");
    //      P->printOn(cerr); ostream << endl;
    delete P;
  }
  P = NULL;
}

NF_INLINE
void CConxDwGeomObj::init()
{
  sc = NULL;
  color = CConxNamedColor::GREEN;
  isValid = FALSE;
  withGarnish = TRUE;
  dm = BEST;
  thickness = 1.0;
  lwtol = .0015;
  sModel = CONX_KLEIN_DISK;
  P = NULL;
}

NF_INLINE
void CConxDwGeomObj::uninitializedCopy(const CConxDwGeomObj &o)
{
  /* DLC setValidity(FALSE); ?? */
  if (o.P != NULL) {
    P = o.P->clone(); // overload the assignment operator for CConxSimpleArtists.
  } else {
    P = NULL;
  }
  color = o.color;
  isValid = o.isValid;
  withGarnish = o.withGarnish;
  dm = o.dm;
  thickness = o.thickness;
  lwtol = o.lwtol;
  sModel = o.sModel;

  sc = NULL;
  // Why share this too? We only keep this during a call to drawOn!
}

OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxArtist)
