## Before installing, edit this file.
## This is included by Makefile.am, so change it before you run `configure'

lcl_PATH = /home/davidc/installed/linux_class_library/
lcl_LDFLAGS = -L$(lcl_PATH)lib
lcl_LIBS = -llcl -lpthread
lcl_CPPFLAGS = -I$(lcl_PATH)include

glut_LDFLAGS = -L/usr/X11R6/lib
glut_LIBS = -lglut
glut_CPPFLAGS =

glu_LDFLAGS =
glu_LIBS = -lGLU
glu_CPPFLAGS =

gl_LDFLAGS =
gl_LIBS = -lGL
gl_CPPFLAGS =

tk_LDFLAGS =
tk_LIBS = -ltk
tk_CPPFLAGS =

tcl_LDFLAGS =
tcl_LIBS = -ltcl
tcl_CPPFLAGS =
