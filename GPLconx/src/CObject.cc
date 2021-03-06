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
  Implementation of C++ classes in `CObject.hh'
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include "CObject.hh"

size_t CConxObject::numCreated = 0;
size_t CConxObject::numDestroyed = 0;
#ifndef NDEBUG
Boole CConxObject::cerr_from_destructor = FALSE;
#endif

void unexpected()
{
  cerr << "\nAn unexpected C++ exception was thrown, which means that there is a bug or you are out of memory.\n";
#ifndef NDEBUG
  abort();
#else
  exit(4);
#endif
}
