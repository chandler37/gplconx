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
  Implementation of C++ classes in `stcanvas.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stcanvas.hh"
#include "stdrawbl.hh"
#include "sterror.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsCanvas::ansMachs = NULL;

NF_INLINE
void CClsCanvas::uninitializedCopy(const CClsCanvas &o)
{
  cv = o.cv; // Yes, neither owns the canvas -- they share it.
}

NF_INLINE CClsBase::ErrType
CClsCanvas::oiActionSync(CClsBase **result, CConxClsMessage &o)
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
      LLL("newly added artist is " << artist);
      cv->append(artist);
    }
  }
  RETURN_THIS(result); // DLC return this???
}

NF_INLINE
void CClsCanvas::initializeAnsweringMachines()
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

CF_INLINE
CClsCanvas::CClsCanvas(const CClsCanvas &o)
  : CClsArray(o)
{
  MMM("copy constructor");
  uninitializedCopy(o);
  assert(getRequiredType() == CLS_DRAWABLE);
}

CF_INLINE
CClsCanvas::CClsCanvas(const CClsArray &o)
  : CClsArray(o)
{
  init();
}

NF_INLINE
CClsCanvas &CClsCanvas::operator=(const CClsCanvas &o)
{
  (void) CClsArray::operator=(o);
  // Forget the old canvas completely.
  init();
  uninitializedCopy(o);
  assert(getRequiredType() == CLS_DRAWABLE);
  return *this;
}

NF_INLINE
CClsBase *CClsCanvas::stCloneDeep() const
{
  CClsArray *j = (CClsArray *) CClsArray::stCloneDeep();
  CClsCanvas *v = new CClsCanvas(*j);
  delete j;
  if (v == NULL) OOM();
  v->uninitializedCopy(*this);
  return v;
}

NF_INLINE
CConxString CClsCanvas::printString() const
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

