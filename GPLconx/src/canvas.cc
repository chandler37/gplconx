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
  C++ Drawing Canvas class.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>

#include "canvas.hh"
#include "dgeomobj.hh"


CF_INLINE
CConxDumbCanvas::CConxDumbCanvas()
  : width(300), height(300), xmin(-1.03), xmax(1.03),
    ymin(-1.03), ymax(1.03)
{
  MMM("CConxDumbCanvas()");
}

BUGGY_INLINE
void CConxDumbCanvas::setSize(uint w, uint h) throw(int)
{
  // throws an int if w or h is 0.
  if (w == 0) throw 0;
  if (h == 0) throw 1;
  width = w; height = h;

  // Make it square. DLC
  if (width < height) width = height;
  if (width > height) height = width;
}

BUGGY_INLINE
void CConxDumbCanvas::setViewingRectangle(double xmin, double xmax,
                                          double ymin, double ymax) throw(int)
{
  // Throws an int if xmin > xmax or ymin > ymax.


  // DLC invalidate display lists?

  if (xmin > xmax) throw 1;
  if (ymin > ymax) throw 2;
  this->xmin = xmin;
  this->xmax = xmax;
  this->ymin = ymin;
  this->ymax = ymax;
}

NF_INLINE
double CConxDumbCanvas::getPixelWidth() const
{
  return (getXmax()-getXmin())/(double)getWidth();
}

NF_INLINE
double CConxDumbCanvas::getPixelHeight() const
{
  return (getYmax()-getYmin())/(double)getHeight();
}

NF_INLINE
ostream &CConxDumbCanvas::printOn(ostream &o) const
{
  o << "CConxDumbCanvas object"; // DLC width, height, xy range
  return o;
}

NF_INLINE
void CConxCanvas::clearDrawables()
{
  artists.clear();
}

NF_INLINE
void CConxCanvas::append(const CConxArtist *m) throw(const char *)
// Takes ownership of *m; our destructor will delete m.
{
  MMM("void append(const CConxArtist *m) throw(const char *)");
  if (m == NULL) throw "why a NULL arg?";
  LLL("Appending to the canvas " << (*m) << " @" << m);
  CConxArtist *nn = m->aClone();
  if (nn == NULL) OOM();
  artists.append(nn);
}

NF_INLINE
void CConxCanvas::masterDraw()
{
  clear();
  if (getModel() != CONX_POINCARE_UHP) {
    // Draw the bounding circle if it is visible
    // DLC make this optional
    setDrawingColor(CConxNamedColor(CConxNamedColor::WHITE));
    drawCircle(0.0, 0.0, 1.0);
  }
  size_t i, sz = numArtists();
  for (i = 0; i < sz; i++) {
    const CConxArtist &a = artists.get(i);
    LLL("Now rendering " << flush << a);
    a.drawOn(*this);
  }
  flushQueue();
}

NF_INLINE
void CConxCanvas::setModel(ConxModlType modl)
{
  assert(modl == CONX_KLEIN_DISK
         || modl == CONX_POINCARE_DISK
         || modl == CONX_POINCARE_UHP);
  // DLC invalidate display lists.
  this->modl = modl;
}

PF_INLINE
ostream &CConxCanvas::printOn(ostream &o) const
{
  o << "<CConxCanvas object of " << modelToString(getModel())
    << " model, currently with " << numArtists() << " Drawables in it.>";
  return o;
}

NF_INLINE
const char *CConxCanvas::modelToString(ConxModlType modl)
{
  const char *pds = "Poincare Disk";
  const char *kds = "Beltrami-Klein Disk";
  const char *puhps = "Poincare Upper Half Plane";
  const char *s = puhps;
  if (modl == CONX_KLEIN_DISK)
    s = kds;
  else if (modl == CONX_POINCARE_DISK)
    s = pds;
  return s;
}

NF_INLINE
CConxDumbCanvas &CConxDumbCanvas::operator=(const CConxDumbCanvas &o)
{
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
Pt CConxDumbCanvas::screenCoordinatesToModelCoordinates(long x, long y)
  // This has poor error checking -- x is usually less than getWidth()
  // and y is usually less than getHeight().  Both are usually nonnegative.
{
  Pt modelCoords;
  screenCoordinatesToModelCoordinates(x, y, modelCoords);
  return modelCoords;
}

NF_INLINE
void CConxDumbCanvas::screenCoordinatesToModelCoordinates(long x, long y, Pt &modelCoords)
  // This has poor error checking.
{
  modelCoords.x =
    getXmin() + ((double) x/(double) getWidth()) * (getXmax() - getXmin());
  modelCoords.y =
    getYmax() - ((double) y/(double) getHeight()) * (getYmax() - getYmin());
}

NF_INLINE
void CConxDumbCanvas::uninitializedCopy(const CConxDumbCanvas &o)
{
  width = o.width;
  height = o.height;
  xmin = o.xmin;
  xmax = o.xmax;
  ymin = o.ymin;
  ymax = o.ymax;
}

NF_INLINE
CConxDrawCanvas &CConxDrawCanvas::operator=(const CConxDrawCanvas &o)
{
  (void) CConxDumbCanvas::operator=(o);
  return *this;
}

NF_INLINE
void CConxDrawCanvas::drawTopSemiCircle(Pt p, double r)
{
  drawTopSemiCircle(p.x, p.y, r);
}

NF_INLINE
void CConxDrawCanvas::drawArc(Pt center, double r, double t0, double t1)
{
  drawArc(center.x, center.y, r, t0, t1);
}

CF_INLINE
CConxCanvas::CConxCanvas(const CConxCanvas &o)
  : CConxDrawCanvas(o)
{
  uninitializedCopy(o);
}

NF_INLINE
CConxCanvas &CConxCanvas::operator=(const CConxCanvas &o)
{
  (void) CConxDrawCanvas::operator=(o);
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
int CConxCanvas::operator==(const CConxCanvas &o) const
{
  return (artists == o.artists);
}

NF_INLINE
void CConxCanvas::uninitializedCopy(const CConxCanvas &o)
{
  artists = o.artists;
}


OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxDumbCanvas)
