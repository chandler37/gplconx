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
  Togl-related C++ Class implementation
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

extern "C" {
#include "viewer.h"
#include "util.h"
#include "point.h"
#include "tclprocs.h"
#include "gl.h"
#include "conxtcl.h"
}

#include "glcanvas.hh"
#include "toglobj.hh"
#include "stmparse.hh"

// DLC make it so that we don't have to link with lines.c and conxv.c

Boole CConxToglObj::debugMode = FALSE;

static CConxGLCanvas pdCanvas, puhpCanvas, kdCanvas;
static CConxClsMetaParser mp(&kdCanvas, &pdCanvas, &puhpCanvas);

static CConxGLCanvas *getCanvasByType(ConxModlType m);
static void initCanvases();

void initCanvases()
{
  pdCanvas.setModel(CONX_POINCARE_DISK);
  kdCanvas.setModel(CONX_KLEIN_DISK);
  puhpCanvas.setModel(CONX_POINCARE_UHP);
  puhpCanvas.setViewingRectangle(-1.0, 1.0, 0.0, 2.0);
  pdCanvas.setViewingRectangle(-1.03, 1.03, -1.03, 1.03);
  kdCanvas.setViewingRectangle(-1.03, 1.03, -1.03, 1.03);
  
  CConxPoint focus1(0.5, 0.75, CONX_POINCARE_UHP);
  CConxPoint focus2(0.2, 0.75, CONX_POINCARE_UHP);
  CConxLine l1(focus1, focus2);
  CConxCircle c1(CConxPoint(-0.3, 0.1, CONX_POINCARE_UHP), 0.7);
#ifdef DLC
  CConxDwCircle dwc1(c1);
  CConxDwCircle dwc2(c1);
  dwc1.setDrawingMethod(dwc1.LONGWAY);
  dwc1.setLongwayTolerance(0.005);
  dwc2.setDrawingMethod(dwc1.BEST);

  // We can't allow the things we append to be destroyed.
  static CConxDwPoint d01(focus1);
  puhpCanvas.append(&d01);
  static CConxDwPoint d02(focus2);
  puhpCanvas.append(&d02);
  static CConxDwPoint d03(focus1);
  kdCanvas.append(&d03);
  static CConxDwPoint d04(focus2);
  kdCanvas.append(&d04);
  static CConxDwPoint d05(focus1);
  pdCanvas.append(&d05);
  static CConxDwPoint d06(focus2);
  pdCanvas.append(&d06);
  static CConxDwLine d07(l1, FALSE);
  puhpCanvas.append(&d07);
  static CConxDwLine d08(l1, FALSE);
  kdCanvas.append(&d08);
  static CConxDwLine d09(l1, TRUE);
  pdCanvas.append(&d09);

  static CConxDwCircle d10(dwc2);
  puhpCanvas.append(&d10);
  static CConxDwCircle d11(dwc1);
  pdCanvas.append(&d11);
  static CConxDwCircle d12(dwc2);
  kdCanvas.append(&d12);
#endif
}

CConxGLCanvas *getCanvasByType(ConxModlType m)
{
  switch (m) {
  case CONX_POINCARE_DISK: return &pdCanvas;
  case CONX_POINCARE_UHP: return &puhpCanvas;
  default: assert(m == CONX_KLEIN_DISK); return &kdCanvas;
  }
}

void CConxToglObj::printLongHelp(void)
{
  cout << "DLC not just yet.\n";
}

void CConxToglObj::createCallBack(struct Togl *togl)
/* Togl widget create callback.  This is called by Tcl/Tk when the widget has
 * been realized.  Here's where one may do some one-time context setup or
 * initializations.
 */
{
  /* GL calls only work within a context, which has just been created. */
  LOGGG1(LOGG_TEXINFO, "\n@create_callback{%s}\n", Togl_Ident(togl));

  CConxGLCanvas *cnvs = getCanvasByType(tconx_togl_id2model(togl));
  cnvs->setSize(Togl_Width(togl), Togl_Height(togl));
  cnvs->initDraw();
}

void CConxToglObj::reshapeCallBack(struct Togl *togl)
/* Togl widget reshape callback.  This is called by Tcl/Tk when the widget
 * has been resized.  Typically, we call glViewport and perhaps setup the
 * projection matrix.
 */
{
  LOGGG1(LOGG_FULL, "\n@reshape_callback %s\n", Togl_Ident(togl));
  
  CConxGLCanvas *cnvs = getCanvasByType(tconx_togl_id2model(togl));
  cnvs->setSize(Togl_Width(togl), Togl_Height(togl));

  LOGGG0(LOGG_FULL, "\n@end reshape_callback\n");
}


void CConxToglObj::displayCallBack(struct Togl *togl)
/* Togl widget display callback.  This is called by Tcl/Tk when the
 * display has to be redrawn.
 */
{
  ConxModlType mt = tconx_togl_id2model(togl);
  LOGGG1(LOGG_FULL, "\n@display_callback %s\n", Togl_Ident(togl));

  CConxGLCanvas *cnvs = getCanvasByType(mt);
  cnvs->masterDraw();

#ifdef TCONX_DOUBLE_BUFFER
  Togl_SwapBuffers(togl);
#endif

  LOGGG0(LOGG_FULL, "\n@end display_callback\n");
}

int CConxToglObj::firstInit(Tcl_Interp *interp)
/* Called by Tk_Main() to let me initialize the modules (Togl) I will need. */
{
  initCanvases();

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
  Togl_CreateFunc(CConxToglObj::createCallBack);
  Togl_DisplayFunc(CConxToglObj::displayCallBack);
  Togl_ReshapeFunc(CConxToglObj::reshapeCallBack);

#ifdef DLC
  {
    char s1[] = "SAMPLE_GLOBAL_VARIABLE";
    const char *dd = "SAMPLE VALUE";
    char *unconst_dd;
    CONST_CAST(char *, unconst_dd, dd);
    Tcl_SetVar2(interp, arrayname, s1, unconst_dd, mode);
  }
#endif

  {
    char s1[] = "DebuggingOutput";
    const char *dd;
    if (isDebugMode())
      dd = "1";
    else
      dd = "0";
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
    char s1[] = "CXXCONX";
    char s2[] = "just existing is enough";
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
    char mutabl[] = "mouse2model";    /* To avoid warnings */
    Togl_CreateCommand(mutabl, CConxToglObj::mouseHandler);
  }
  {
    char mutabl[] = "data_entry";    /* To avoid warnings */
    Togl_CreateCommand(mutabl, tconx_tcl_data_entry);
  }
  {
    char mutabl[] = "query";    /* To avoid warnings */
    Togl_CreateCommand(mutabl, tconx_tcl_query);
  }
  {
    char mutabl[] = "yap";    /* To avoid warnings */
    Togl_CreateCommand(mutabl, CConxToglObj::parseString);
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

int CConxToglObj::mouseHandler(struct Togl *togl, int argc, char *argv[])
// Evaluate `.modelpd.togl_wig mouse2model 40 55', e.g., to get pd coordinates
// for this location.
{
  Tcl_Interp *interp = Togl_Interp(togl);
  char help_message[] = "Illegal usage.  Try `.modelpd.togl_wig "
    "mouse2model 40 55', e.g., to get Poincare Disk coordinates "
    "corresponding to mouse coordinates (40, 55).";


  argc -= 2; argv += 2;

  if (argc != 2) TCONX_TCL_BAD_ARGS(interp);

  long x, y;
  if (conx_str2l(argv[0], &x) || conx_str2l(argv[1], &y))
    TCONX_TCL_BAD_ARGS(interp);

  CConxDumbCanvas *cnvs = getCanvasByType(tconx_togl_id2model(togl));
  assert((uint) Togl_Height(togl) == cnvs->getHeight());
  assert((uint) Togl_Width(togl) == cnvs->getWidth());
  
  Pt m;
  cnvs->screenCoordinatesToModelCoordinates(x, y, m);
  char s[250];
  (void) sprintf(s, "%.18e %.18e", m.x, m.y); // a list of two elements.
  TCONX_TCL_OK_ARRAY(s);
}

int CConxToglObj::parseString(struct Togl *togl, int argc, char *argv[])
{
  char help_message[] = "Illegal usage.  Try `.modelpd.togl_wig yap "
    "{2.2 between: 1.1 and: (y := 3.3)}' or `yap --debug {}";
  Tcl_Interp *interp = Togl_Interp(togl);
  argc -= 2; argv += 2;
  mp.debugParse(FALSE);
  if (argc == 2) {
    if (CConxString(argv[1]) != "--debug") TCONX_TCL_BAD_ARGS(interp);
    --argc; ++argv;
    mp.debugParse(TRUE);
  }
  if (argc != 1) TCONX_TCL_BAD_ARGS(interp);
  long reslt = mp.parse(argv[0]);
  int retval = TCL_OK;
  if (reslt < 0) {
    // DLC how can this happen??? We have `input: error' in the grammar!
    TCONX_TCL_ERROR("General Fatal Parse error");
  } else if (reslt > 0) {
    // We have reslt non-fatal errors.
    retval = TCL_ERROR;
  }

  // DLC rename ttalk-make-current-good-sh to a <=14-letter name

  // Return the value of the implicit variable `ans'.
  CConxString css = mp.getLastParseResult().getString();
  char *nonconst = css.getStringAsNewArray();
  Tcl_SetResult(interp, nonconst, TCL_VOLATILE);
  delete [] nonconst;
  return retval;
}

