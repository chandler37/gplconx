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

/* C++ Header file to be included by all test programs. */

#ifndef GPLCONX_TESTER_CXX_H
#define GPLCONX_TESTER_CXX_H 1

#include <stdlib.h>
#include <assert.h>

extern "C" {
#include "util.h"
}

extern int weAreVerbose;
#define VERBOSE() weAreVerbose
#define SET_VERBOSE(m) weAreVerbose = m

#define HANDLE_ARGS(argc, argv) \
  if (argc == 1) { \
    SET_VERBOSE(0); \
    SET_LOG_LEVEL(0); \
  }

#define OUT(a) if (VERBOSE()) cout << a

#define CRASH_IF_BAD_TEST
#ifdef CRASH_IF_BAD_TEST
#define TEST(condition) assert(condition)
#define RET1(condition) assert(condition)
#else
#define TEST(condition) if (!(condition)) exit(1)
#define RET1(condition) if (!(condition)) return 1
#endif

#define SET_LOG_LEVEL(ll) loglevel = ll

#define GOOD_TEST_EXIT_CODE 0
// invalid test (but not failure) code is 77; see Automake docs.

// To ensure that you are deleting every CCOnxObject that you are creating,
// and that you are calling the superclass's operator= and copy constructor
// for all classes, call this whenever appropriate:
#define THERE_ARE_ZERO_OBJECTS() \
  RET1(CConxObject::numberExisting() == 0)
  // The presence of static CConxObjects in code linked with tCString
  // will make this false, have
  // you added any, such as a static CConxString class member?  If so,
  // you will still get 0 objects at the end of the program, which you
  // can check if LLL_ON().

#endif // GPLCONX_TESTER_CXX_H
