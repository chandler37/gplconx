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
  Assertions.
*/

#ifndef GPLCONX_CASSERT_H
#define GPLCONX_CASSERT_H 1

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/* When an object has a certain constraint on it from birth until death,
   that constraint is called an invariant.
*/
#define CONX_INVARIANT(condition) assert(condition)

#ifdef __cplusplus
}
#endif

#endif /* GPLCONX_CASSERT_H */
