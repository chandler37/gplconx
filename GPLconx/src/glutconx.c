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

/* GLUT user interface for GPLconx. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "point.h"
#include "viewer.h"
#include "util.h"
#include "gl.h"

static void pdisplay(void);
static void kdisplay(void);
static void pddisplay(void);
static void conxv_do_prompt(ConxMenuChoice cmc, ConxModlType modl);
static void DisplayAll(int a);
static void myReshape(int w, int h);
static void setupmousemenu(void);
static void mouse(int iknowitstheleftbutton, int state, int x, int y);
static void mousemenu(int a);
static void setupmenu(int modd);
static void ad(const char* a, int b);
static void menufunc(int a);
static void indata(ConxModlType modd);


static int leftfunc=CONXCMD_CONX_NOT_A_CHOICE_2;
/* DLC better initial value */
static int sub1, sub2, sub3, sub4, sub5, sub7;


void setupmenu(int modd)
{
  sub1= glutCreateMenu(menufunc);
  ad("Circle", CONXCMD_BCIRCLE);
  ad("Ellipse/Hyperbola", CONXCMD_BELLIPSE);
  ad("Parabola", CONXCMD_BPARABOLA);
  ad("Equidistant curve", CONXCMD_BEQDIST);
  sub2= glutCreateMenu(menufunc);
  ad("Circle", CONXCMD_CIRCLE);
  ad("Ellipse/Hyperbola", CONXCMD_ELLIPSE);
  ad("Parabola", CONXCMD_PARABOLA);
  ad("Equidistant curve", CONXCMD_EQDIST);
  sub3= glutCreateMenu(menufunc);
  ad("Tolerance for long ways", CONXCMD_GETTOL);
  ad("Computational tolerance (expert option)", CONXCMD_GETCOMPTOL);
  ad("Step size for Circles", CONXCMD_GETTSTEP);
  ad("Model dimensions (xmin, etc.)", CONXCMD_GETXXYY);
  ad("Focus 1", CONXCMD_GETF1);
  ad("Focus 2", CONXCMD_GETF2);
  ad("Magic distance", CONXCMD_GETCD);
  ad("Log level", CONXCMD_GETLOGLEVEL);
  if (modd==CONX_KLEIN_DISK) {
    ad("Slope of line", CONXCMD_GETA);
    ad("Y-intercept (b) of line", CONXCMD_GETR);
  } else {
    ad("X of Center of line", CONXCMD_GETA);
    if (modd==CONX_POINCARE_DISK) {
      ad("Y of Center of line", CONXCMD_GETR);
    } else {
      ad("Radius of line", CONXCMD_GETR);
    }
  }

  ad("Show variables", CONXCMD_OUTPUT);
  sub4 = glutCreateMenu(menufunc);
  if (modd!=CONX_POINCARE_UHP)
    ad("Automatic circular borders", CONXCMD_BORDERSON);
  ad("Garnish", CONXCMD_GARNISHON);
  ad("Automatic Foci Drawing", CONXCMD_AUTOFOCION);
  sub5= glutCreateMenu(menufunc);
  ad("Line thru foci", CONXCMD_LINEF);
  ad("Alternate line (e.g., defined by a & r)", CONXCMD_ALTLINE);
  ad("Focus 1", CONXCMD_FOCUS);
  ad("Foci 1 & 2", CONXCMD_FOCI);
  ad("Segment between foci", CONXCMD_SEGMENT);
  sub7=glutCreateMenu(menufunc);
  ad("REALLY, TRULY EXIT", CONXCMD_EXIT);
  glutCreateMenu(menufunc);
  glutAddSubMenu("Conics", sub1);
  glutAddSubMenu("Draw", sub5);
  glutAddSubMenu("Toggle", sub4);
  glutAddSubMenu("Input", sub3);
  glutAddSubMenu("SLOW (yet reliable!) conics", sub2);
  glutAddSubMenu("Exit", sub7);
  ad("Clear window, retain changes", CONXCMD_RESETTHISONE);
  ad("Reset program>>>>", CONXCMD_CLEAR);
}

void mousemenu(int a)
{
  leftfunc=a;
}

void mouse(int iknowitstheleftbutton, int state, int x, int y)
{
  int modl= glutGetWindow() - 1;
  if (conx_mouse(modl, (state == GLUT_DOWN) ? CONXMOUSE_DOWN : CONXMOUSE_UP,
                 x, y, glutGet(GLUT_WINDOW_HEIGHT), leftfunc)) {
    glutPostRedisplay();
  }
}


void setupmousemenu(void) {
  glutCreateMenu(mousemenu);
  ad("Define left mouse button function", CONXCMD_CONX_NOT_A_CHOICE_3);
  ad("---------------------------", CONXCMD_CONX_NOT_A_CHOICE_2);
  ad("Get Foci", CONXCMD_MGETFS);
  ad("Get Focus 1", CONXCMD_MGETF1);
  ad("Get Focus 2", CONXCMD_MGETF2);
  ad("Get viewing rectangle", CONXCMD_MZOOM);
  ad("Get magic distance", CONXCMD_MGETCD);
  ad("Get alternate line", CONXCMD_MGETALT);
  glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

void menufunc(int a)
{
  if (a != CONXCMD_CONX_NOT_A_CHOICE_2 && a != CONXCMD_CONX_NOT_A_CHOICE_3) {
    int mf, i, num_inputs;
    ConxModlType modl;
    num_inputs = conx_num_inputs((ConxMenuChoice) a);
    modl = (ConxModlType) glutGetWindow() - 1;

    conxv_do_prompt((ConxMenuChoice)a, modl); /* DLC only if num_inputs is positive */
    for (i = 0; i < num_inputs; i++) {
      indata(modl);
    }
    mf = conx_menufunc((ConxMenuChoice)a, modl, glutGet(GLUT_WINDOW_HEIGHT));
#ifdef DLC_1
    if (mf & CONXMF_DISPLAY) {
      glutPostRedisplay();
    }
#else
    glutPostRedisplay(); /* DLC do this all the time, so that drawing
                            an ellipse works instead of just draws half
                            until you trigger a redisplay with window
                            system events.
                         */
#endif
    if (mf & CONXMF_DISPLAY_ALL) {
      DisplayAll(modl+1);
    }
  }
}

void conxv_do_prompt(ConxMenuChoice cmc, ConxModlType modl)
{
  switch (cmc) {
  case CONXCMD_GETCD:
    PROMPT("\nInput the magical distance associated with the conic "
           "sections: ");
    break;
  case CONXCMD_GETLOGLEVEL:
    PROMPT("\nInput the log level in [0.0, 32000.0]: ");
    break;
  case CONXCMD_GETTOL:
    PROMPT("\nInput tolerance: ");
    break;
  case CONXCMD_GETCOMPTOL:
    PROMPT("\nInput tolerance for computations such as finding "
           "the distance from a line: ");
    break;
  case CONXCMD_GETXXYY:
    PROMPT("\nInput xmin xmax ymin ymax: ");
    break;
  case CONXCMD_GETTSTEP:
    PROMPT("Input the step size for arcs.  E.g., .01 makes about "
           "314 points on a circle. : ");
    break;
  case CONXCMD_GETF1:
    PROMPT("Input Focus 1 (x y): ");
    break;
  case CONXCMD_GETF2:
    PROMPT("Input Focus 2 (x y): ");
    break;
  case CONXCMD_GETA:
    if (modl==CONX_POINCARE_DISK) {
      PROMPT("Input the x coordinate of the center of the line "
             "(orthogonal circle): ");
    } else if (modl==CONX_POINCARE_UHP) {
      PROMPT("Input a, the x coordinate of the center of a line: ");
    } else {
      PROMPT("Input the slope of the line (give the x coordinate for "
             "a vertical line): ");
    }
    break;
  case CONXCMD_GETR:
    if (modl==CONX_POINCARE_DISK) {
      PROMPT("Input the y coordinate of the center of the line "
             "(orthogonal circle): ");
    } else if (modl==CONX_POINCARE_UHP) {
      PROMPT("Input r, the radius of the line (-1 for a vertical ray): ");
    } else {
      PROMPT("Input b, the y intercept (y=mx+b)[or give 1024 if you wish "
             "for a vertical line]: ");
    }
    break;
  default:
    /* no prompt */
    break;
  }
}

void indata(ConxModlType modd)
{
  double d;
  scanf("%lf", &d);
  conx_add_datum(d, modd);
}

void ad(const char *a, int b)
{
  glutAddMenuEntry(a, b);
}

void DisplayAll(int a)
/* the window */
{
  int ct=-1;
  for (ct=1; ct<4; ct++) {
    glutSetWindow(ct);
    glutPostRedisplay();
  }
  glutSetWindow(a);
}

#ifdef DLC
void mkptAll(void)
/* the window */
{
  int ct=-1, a;
  a=glutGetWindow();
  
  for (ct=1; ct<4; ct++) {
    glutSetWindow(ct);
    conx_draw_point(focus1[ct-1]);
    conx_draw_point(focus2[ct-1]);
  }
  glutSetWindow(a);
}
#endif

void myReshape(int w, int h)
{
  ConxModlType a = (ConxModlType) (glutGetWindow()-1);
  LOGGG3(LOGG_TEXINFO, "\n@myReshape{window %s, width %d, height %d",
         conx_modelenum2short_string(a), w, h);
  if (w < h) h = w; else w = h;
  glutReshapeWindow(w, h);
  conx_reshape(a, w, h);
  LOGGG0(LOGG_TEXINFO, "}");
}

void pddisplay(void)
{
  conx_display(CONX_POINCARE_DISK);
  glFlush();
}

void kdisplay(void)
{
  conx_display(CONX_KLEIN_DISK);
  glFlush();
}

void pdisplay(void)
{
  conx_display(CONX_POINCARE_UHP);
  glFlush();
}

int main(int argc, char **argv)
{
  printf("%s", PACKAGE "'s gconx version " VERSION
         ", Copyright (C) 1996-2001 David L. Chandler\n" PACKAGE
         " comes with ABSOLUTELY NO WARRANTY\n"
         "This is free software, and you are welcome to redistribute it\n"
         "under certain conditions; read the COPYING file for details.\n\n");
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(0,0);

  glutCreateWindow("Poincare Upper Half Plane");
  conx_gl_first_init();
  glutMouseFunc(mouse);
  glutReshapeFunc(myReshape);

  conx_gl_init(CONX_POINCARE_UHP, WIDTH, HEIGHT);

  glutDisplayFunc(pdisplay);
  glutCreateMenu(menufunc);
  setupmousemenu();
  setupmenu(CONX_POINCARE_UHP);
  glutAttachMenu(GLUT_RIGHT_BUTTON);





  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(WIDTH+25, 0);
  glutCreateWindow("Beltrami-Klein Disk");
  glutMouseFunc(mouse);
  glutReshapeFunc(myReshape);

  conx_gl_init(CONX_KLEIN_DISK, WIDTH, HEIGHT);

  glutDisplayFunc(kdisplay);
  glutCreateMenu(menufunc);
  setupmousemenu();
  setupmenu(CONX_KLEIN_DISK);
  glutAttachMenu(GLUT_RIGHT_BUTTON);





  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(WIDTH*2+40, 0);
  glutCreateWindow("Poincare Disk");
  glutReshapeFunc(myReshape);

  conx_gl_init(CONX_POINCARE_DISK, WIDTH, HEIGHT);

  glutDisplayFunc(pddisplay);
  glutCreateMenu(menufunc);
  setupmousemenu();
  setupmenu(CONX_POINCARE_DISK);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  glutMouseFunc(mouse);

  green();
  FLUSH();

  glutMainLoop();

  /* NOTREACHED */
  return 0;
}
