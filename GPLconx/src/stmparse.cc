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
  Implementation of C++ classes in `stmparse.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stmparse.hh"

CF_INLINE
CConxClsMetaParser::CConxClsMetaParser(CConxCanvas *kdCanvas,
                                       CConxCanvas *pdCanvas,
                                       CConxCanvas *uhpCanvas)
{
  init();
  this->kdCanvas = kdCanvas;
  this->pdCanvas = pdCanvas;
  this->uhpCanvas = uhpCanvas;
}

CF_INLINE
CConxClsMetaParser::CConxClsMetaParser(const CConxClsMetaParser &o)
  : CConxMetaParser(o)
{
  uninitializedCopy(o);
}

NF_INLINE
CConxClsMetaParser &CConxClsMetaParser::operator=(const CConxClsMetaParser &o)
{
  (void) CConxMetaParser::operator=(o);
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
void CConxClsMetaParser::initializeClsMgr(CConxClsManager *m)
{
  assert(m != NULL);
  m->firstInit(pdCanvas, kdCanvas, uhpCanvas);
  // it doesn't hurt to call it repeatedly.
}

NF_INLINE
void CConxClsMetaParser::uninitializedCopy(const CConxClsMetaParser &o)
{
  pdCanvas = o.pdCanvas;
  kdCanvas = o.kdCanvas;
  uhpCanvas = o.uhpCanvas;
}

