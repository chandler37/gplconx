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
  C++ Printable classes (Java ``Interfaces'' would be more appropriate).
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <assert.h>

#include "printon.hh"

CConxPrintableByModel::PrintHow CConxPrintableByModel::phow
   = CConxPrintableByModel::PP_ALL;

ostream &pd(ostream &o)
{
  CConxPrintableByModel::setDesiredFormat(CConxPrintableByModel::PP_PD);
  return o;
}

ostream &kd(ostream &o)
{
  CConxPrintableByModel::setDesiredFormat(CConxPrintableByModel::PP_KD);
  return o;
}

ostream &puhp(ostream &o)
{
  CConxPrintableByModel::setDesiredFormat(CConxPrintableByModel::PP_PUHP);
  return o;
}

ostream &all(ostream &o)
{
  CConxPrintableByModel::setDesiredFormat(CConxPrintableByModel::PP_ALL);
  return o;
}
