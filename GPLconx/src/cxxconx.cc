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
  Tcl/Tk+Togl+OpenGL+C++ version of GPLconx
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <iostream.h>

#include <string.h>

#include <tcl.h>
#include <tk.h>

#include <Togl/togl.h>

#include "viewer.h"
#include "util.h"
#include "tclprocs.h"
#include "tconxopt.h"

#include "point.hh"
#include "CString.hh"
#include "glcanvas.hh"
#include "toglobj.hh"

int loglevel = 0;

enum {
  TCONX_EXIT_OK = 0,
  TCONX_EXIT_ARGS = 1,
  TCONX_EXIT_LONG_HELP = 2
};

extern "C" {
/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl. (or so Togl-1.5's examples say).
 */
extern int matherr();
int *tclDummyMathPtr = (int *) matherr;
}

#ifdef DLC
void mkptAll(void)
{
  /* set window DLC */
  conx_draw_point(focus1[CONX_POINCARE_UHP]);
  conx_draw_point(focus2[CONX_POINCARE_UHP]);
  /* set window */
  conx_draw_point(focus1[CONX_POINCARE_DISK]);
  conx_draw_point(focus2[CONX_POINCARE_DISK]);
  /* set window DLC questionable */
  conx_draw_point(focus1[CONX_KLEIN_DISK]);
  conx_draw_point(focus2[CONX_KLEIN_DISK]);
}
#endif

/* DLC Can I use the Togl example code in this GPL'ed source file?? */
int main(int argc, char **argv)
{
  cout << PACKAGE "'s cxxconx version " VERSION
    ", Copyright (C) 1996-2001 David L. Chandler\n" PACKAGE
    " comes with ABSOLUTELY NO WARRANTY\n"
    "This is free software, and you are welcome to redistribute it\n"
    "under certain conditions; read the COPYING file for details.\n\n";

  /* Process command-line arguments */
  struct gengetopt_args_info garg;
  if (cmdline_parser(argc, argv, &garg)) {
    exit(TCONX_EXIT_ARGS);
  }
  if (garg.Tcl_dir_given) {
    /* DLC */
    cout << "not yet DLC\n";
    exit(2);
  }

  if (garg.long_help_given) {
    CConxToglObj::printLongHelp();
    exit(TCONX_EXIT_LONG_HELP);
  }

  if (garg.debug_given) {
    loglevel = 101;
    CConxToglObj::setDebugMode(TRUE);
  } else {
    CConxToglObj::setDebugMode(FALSE);
    loglevel = 0;
  }

  if (garg.inputs_num > 0) {
    cout << "There " << ((garg.inputs_num == 1) ? "is" : "are")
         << " " << garg.inputs_num << " extra argument"
         << ((garg.inputs_num == 1) ? "" : "s") << " on the command line.\n";
    exit(TCONX_EXIT_ARGS);
  }

  /* Pretend we don't have any arguments except the program name: */
  Tk_Main(1, argv, CConxToglObj::firstInit);

  /* NOTREACHED */
  return 0;
}

