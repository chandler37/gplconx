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
  if (m == NULL) throw "why a NULL arg?";
  artists.append(m);
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
  size_t i, sz = artists.size();
  for (i = 0; i < sz; i++) {
    const CConxArtist *a = artists.get(i);
    if (LLL_ON()) {
      cout << "Now rendering ";
      a->printOn(cout);
      cout << endl;
    }
    a->drawOn(*this);
  }
  flush();
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
    << " model, currently with " << artists.size() << " Drawables in it.>";
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


OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxCanvas)
OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxDumbCanvas)
OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxDrawCanvas)
