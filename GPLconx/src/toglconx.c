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
  Tcl/Tk+Togl+OpenGL version of GPLconx
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <tcl.h>
#include <tk.h>
/* DLC */
#include <Togl/togl.h>

#include "viewer.h"
#include "util.h"
#include "point.h"
#include "tclprocs.h"
#include "gl.h"

#include "tconxopt.h"

enum {
  TCONX_EXIT_OK = 0,
  TCONX_EXIT_ARGS = 1,
  TCONX_EXIT_LONG_HELP = 2
};

/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl. (or so Togl-1.5's examples say).
 */
extern int matherr();
int *tclDummyMathPtr = (int *) matherr;

static int first_init(Tcl_Interp *);
static void display_callback(struct Togl *); /* display callback */
static void reshape_callback(struct Togl *); /* reshape callback */
static void create_callback(struct Togl *);  /* widget creation callback */
static void tconx_print_long_help(void);


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

void tconx_print_long_help(void)
{
  printf("DLC not just yet.\n");
}

void create_callback(struct Togl *togl)
/* Togl widget create callback.  This is called by Tcl/Tk when the widget has
 * been realized.  Here's where one may do some one-time context setup or
 * initializations.
 */
{
  /* GL calls only work within a context, which has just been created. */
  static int first = 1;
  LOGGG1(LOGG_TEXINFO, "\n@create_callback{%s}\n", Togl_Ident(togl));

  if (first) {
    conx_gl_first_init();
    first = 0;
  }

  conx_gl_init(tconx_togl_id2model(togl),
               Togl_Width(togl), Togl_Height(togl));
}

void reshape_callback(struct Togl *togl)
/* Togl widget reshape callback.  This is called by Tcl/Tk when the widget
 * has been resized.  Typically, we call glViewport and perhaps setup the
 * projection matrix.
 */
{
  LOGGG1(LOGG_FULL, "\n@reshape_callback %s\n", Togl_Ident(togl));
  conx_reshape(tconx_togl_id2model(togl), /* DLC was 0 */
               Togl_Width(togl), Togl_Height(togl));
  LOGGG0(LOGG_FULL, "\n@end reshape_callback\n");
}

void display_callback(struct Togl *togl)
/* Togl widget display callback.  This is called by Tcl/Tk when the
 * display has to be redrawn.
 */
{
  ConxModlType mt = tconx_togl_id2model(togl);
  LOGGG1(LOGG_FULL, "\n@display_callback %s\n", Togl_Ident(togl));

  conx_display(mt);
  conx_gl_flush();
#ifdef TCONX_DOUBLE_BUFFER
  Togl_SwapBuffers(togl);
#endif

  LOGGG0(LOGG_FULL, "\n@end display_callback\n");
}

int first_init(Tcl_Interp *interp)
/* Called by Tk_Main() to let me initialize the modules (Togl) I will need. */
{
  int mode = TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG;
  char arrayname[] = "tconx_globls";

  /* DLC use conx_file_exists and the is_accessible() functions. */
  char tcl_file_name[] = "Tcl/tconx.tcl";
  /*
   * Specify a user-specific startup file to invoke if the application
   * is run interactively.  Typically the startup file is "~/.apprc"
   * where "app" is the name of the application.  If this line is deleted
   * then no user-specific startup file will be run under any conditions.
   */



  LOGGG1(LOGG_FULL, "%s", "    Togl's first init in " __FILE__ "\n");

  /*
   * Initialize Tcl, Tk, and the Togl widget module.
   */
  if (Tcl_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }
  if (Tk_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }
  if (Togl_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }

  /*
   * Specify the C callback functions for widget creation, display,
   * and reshape.
   */
  Togl_CreateFunc(create_callback);
  Togl_DisplayFunc(display_callback);
  Togl_ReshapeFunc(reshape_callback);

  {
    char s1[] = "SAMPLE_GLOBAL_VARIABLE";
    const char *dd = "SAMPLE VALUE";
    char *unconst_dd;
    CONST_CAST(char *, unconst_dd, dd);
    Tcl_SetVar2(interp, arrayname, s1, unconst_dd, mode);
  }

  {
    char s1[] = "Package_Version";
    char s2[] = VERSION;
    /* To avoid warnings, these are defined as non-const char arrays */
    Tcl_SetVar2(interp, arrayname, s1, s2, mode);
  }

  {
    char s1[] = "Package";
    char s2[] = PACKAGE;
    /* To avoid warnings, these are defined as non-const char arrays */
    Tcl_SetVar2(interp, arrayname, s1, s2, mode);
  }

  {
    char s1[] = "double_buffered";
#ifdef TCONX_DOUBLE_BUFFER
    char s2[] = "true";
#else
    char s2[] = "false";
#endif
    /* To avoid warnings, these are defined as non-const char arrays */
    Tcl_SetVar2(interp, arrayname, s1, s2, mode);
  }

  /*
   * Call Tcl_CreateCommand for application-specific commands, if
   * they weren't already created by the init procedures called above.
   */

  {
    char mutable[] = "mouse";    /* To avoid warnings */
    Togl_CreateCommand(mutable, tconx_tcl_mouse);
  }
  {
    char mutable[] = "data_entry";    /* To avoid warnings */
    Togl_CreateCommand(mutable, tconx_tcl_data_entry);
  }
  {
    char mutable[] = "query";    /* To avoid warnings */
    Togl_CreateCommand(mutable, tconx_tcl_query);
  }

  /*
    We're using our Tcl code like a user's RC file, so we
    don't get nice error checking. DLC
  */
#if (TCL_MAJOR_VERSION * 100 + TCL_MINOR_VERSION) >= 705
  {
    char s1[] = "tcl_rcFileName";
    Tcl_SetVar(interp, s1, tcl_file_name, mode);
  }
#else
  strcpy(tcl_RcFileName, tcl_file_name);
#endif
  return TCL_OK;
}

/* DLC Can I use the Togl example code in this GPL'ed source file?? */
int main(int argc, char **argv)
{
  struct gengetopt_args_info garg;

  printf("%s", PACKAGE "'s tconx version " VERSION
         ", Copyright (C) 1996-2001 David L. Chandler\n" PACKAGE
         " comes with ABSOLUTELY NO WARRANTY\n"
         "This is free software, and you are welcome to redistribute it\n"
         "under certain conditions; read the COPYING file for details.\n\n");

  /* Process command-line arguments */
  if (cmdline_parser(argc, argv, &garg)) {
    exit(TCONX_EXIT_ARGS);
  }
  if (garg.Tcl_dir_given) {
    /* DLC */
    printf("not yet DLC\n");
    exit(2);
  }

  if (garg.long_help_given) {
    tconx_print_long_help();
    exit(TCONX_EXIT_LONG_HELP);
  }

  if (garg.inputs_num > 0) {
    printf("There %s %d extra argument%s on the command line.\n",
           (garg.inputs_num == 1) ? "is" : "are",
           garg.inputs_num, (garg.inputs_num == 1) ? "" : "s");
    exit(TCONX_EXIT_ARGS);
  }

  Tk_Main(1, argv, first_init); /* Pretend we don't have any arguments
                                   except the program name */
  /* NOTREACHED */
  return 0;
}

