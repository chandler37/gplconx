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
  Declarations that aid C++ classes in general.
*/

#ifndef GPLCONX_DECLS_CXX_H
#define GPLCONX_DECLS_CXX_H 1

typedef unsigned int uint;

// bool, true, and false are not supported by all compilers.
typedef int Boole;
#define BOOLE2STRING(b) ((b) ? "TRUE" : "FALSE")
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define BOOLE_CAST(x) ((x) ? TRUE : FALSE)
#define IS_A_BOOLE(x) ((x) == TRUE || (x) == FALSE)

// `Bitflag b = A|B|C;' support:
typedef unsigned int Bitflag;


// The stdout debugging support [ that defines great code 8) ]:
extern "C" {
#include "util.h"
}
#include <iostream.h>
#define LLL_ON() (loglevel > LOGG_CXX_LOC)
#define LLL1_ON() (loglevel > LOGG_CXX_LOC1)
#define LLL(tmpSMsg) if (LLL_ON()) cout << tmpSMsg << endl << flush

#include <stdlib.h>
#define CXXFATAL(s) do { cerr << PACKAGE " fatal error:\n" << s; exit(1); } while(0)

// When are two ellipses equal??  If you put 0.0, then only when they
// are bitwise equal.  If you put a larger value, then theorems that
// say, ``When you compute an ellipse like so and so, then it is the same
// ellipse as if you had...'' will work for most cases.
//
// No value for EQUALITY_TOL will cause silly things like saved queries
// to make any difference.
//
// This is closely related to the question, when are two doubles equal?
//
// Must be nonnegative.
#define EQUALITY_TOL 1.0e-8


// The following are included so that a compiler that is smarter than
// egcs-2.91.66 can be used to increase execution speed.  Define as
// many as possible of the following to `inline':
#define NF_INLINE /* DLC there is a linker error! inline */ // do inline normal methods
#define BUGGY_INLINE // DLC what don't I understand?? You should be able to replace this with inline, but just try it!
#define CF_INLINE // don't inline constructors and destructors.
#define PF_INLINE // don't inline printOn implementations.

// VIRT should be `virtual', because C++'s idea of a virtual base class is
// my idea of a base class.  However, it is not needed because we don't
// inherit twice, so we won't make the compiler figure out that we don't
// actually need it.  (Embedded C++ does not even support it.)
//
// The real reason it is still in the code is that I had to learn a lesson
// about virtual base classes the hard way when two books on C++ both
// failed to mention that constructors are affected.  See `tderive.cc'
// for more.
// `#define VIRT virtual' will cause a ton of bugs unless you fix all
// the copy constructors.
//
// Even worse, it you try to use them, then
// ((CConxClsBase *)(<CConxClsFloat *>))->getType() will
// cause a weird abort in stl_alloc with egcs-2.91.66 on Linux 2.2.17 #5
// i686-pc-linux-gnu.  That's ugly!
#define VIRT /* nothing */


#define CCONX_CLASSNAME(x) \
public: \
  static const char *staticClassName() {return x;} \
  virtual const char * className() const {return x;} \
private:

// CCONX_METHOD_STRING won't work very well for class CConxString, of course!
// Use like `LLL(CCONX_COUT_METHOD("CConxString()");', as MMM("CConxString()")
// does.
#define CCONX_METHOD_STRING(tmpMMsg) (CConxString(this->className()) + "::" + (tmpMMsg))
#define CCONX_COUT_METHOD(tmpMMsg) this->className() << "::" << (tmpMMsg)
#define MMM(tmpMMsg) LLL(CCONX_COUT_METHOD(tmpMMsg))

// For bug-free code that is used often, use these so debugging output is
// not so cluttered:
#define LLL1(tmpSMsg) if (LLL1_ON()) cout << tmpSMsg << endl << flush
#define MMM1(tmpMMsg) LLL1(CCONX_COUT_METHOD(tmpMMsg))

#define OOM() CXXFATAL("OUT OF MEMORY\n")

#endif // GPLCONX_DECLS_CXX_H
