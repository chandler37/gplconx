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

#ifndef GPLCONX_TCLPROCS_H
#define GPLCONX_TCLPROCS_H 1

#include "point.h"

struct Togl;

ConxModlType tconx_togl_id2model(struct Togl *);
int tconx_tcl_mouse(struct Togl *togl, int argc, char *argv[]);
int tconx_tcl_data_entry(struct Togl *togl, int argc, char *argv[]);
int tconx_tcl_query(struct Togl *togl, int argc, char *argv[]);

#endif /* GPLCONX_TCLPROCS_H */
