## Before installing, edit this file.
## This is included by Makefile.am, so change it before you run `configure'

## _CPPFLAGS has the -I directives.
## _LDFLAGS has the -L directives
## _LIBS has the `-lfoo' or `libfoo.a' statement.

## specify glut_LDFLAGS with --with-glut-libdir
## specify glut_CPPFLAGS with --with-glut-incdir
glut_LIBS = -lglut

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
