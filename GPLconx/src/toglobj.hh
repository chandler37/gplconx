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
  Togl-related C++ Class
*/

#ifndef GPLCONX_TOGLOBJ_H
#define GPLCONX_TOGLOBJ_H 1

#include "decls.hh"

struct Togl;
struct Tcl_Interp;

class CConxToglObj : VIRT public CConxObject {
  CCONX_CLASSNAME("CConxToglObj")
public:
  static void displayCallBack(struct Togl *);
  static void reshapeCallBack(struct Togl *);
  static void createCallBack(struct Togl *);
  static int firstInit(struct Tcl_Interp *);
  static void printLongHelp(void);

  static int parseString(struct Togl *togl, int argc, char *argv[]);
}; // class CConxToglObj

#endif // GPLCONX_TOGLOBJ_H
