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
  C++ wrapper around point.h and viewer.h with some overloaded << operators:
*/

#ifndef GPLCONX_STPOINTHH_CXX_H
#define GPLCONX_STPOINTHH_CXX_H 1

#include <iostream.h>

#include "point.h"
#include "viewer.h"

ostream &operator<<(ostream &o, ConxModlType m);
ostream &operator<<(ostream &o, ConxMenuChoice m);

inline
ostream &operator<<(ostream &o, ConxModlType m)
{
  o << conx_modelenum2string(m);
  return o;
}

inline
ostream &operator<<(ostream &o, ConxMenuChoice m)
{
  o << conx_menu_choice2string(m);
  return o;
}

#endif // GPLCONX_STPOINTHH_CXX_H
