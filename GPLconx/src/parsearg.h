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
  Data types for passing data to and from the parser in `parser.y'
*/

#ifndef GPLCONX_PARSEARG_CXX_H
#define GPLCONX_PARSEARG_CXX_H 1

#ifdef __cplusplus
extern "C" {
#endif

typedef void *CLS_ptr;
typedef void *ConstCLS_ptr;

typedef struct SimpleClsRef {
  CLS_ptr ptr;
  int ptr_type; // enum ClsType
} SimpleClsRef;

typedef struct ConstSimpleClsRef {
  ConstCLS_ptr ptr;
  int ptr_type; // enum ClsType
} ConstSimpleClsRef;

typedef void * KeywordMessage;
typedef void * AssnList;

void llextern_set_interactive(int isInteractive);

// From a SimpleClsRef *, get a CClsBase * or CConxClsVariable *
// that may or may not be NULL.
#define REF2OBJ(bp, simpleclsrefptr) \
  do { \
    assert((simpleclsrefptr)->ptr != NULL); \
    if ((simpleclsrefptr)->ptr_type == CClsBase::CLS_VARIABLE) { \
      /* Get the variable's pointee. */ \
      (bp) = ((CConxClsVariable *)(simpleclsrefptr)->ptr)->getPointee(); \
    } else { \
      (bp) = (CClsBase *) (simpleclsrefptr)->ptr; \
      /* assert((bp)->getType() == (simpleclsrefptr)->ptr_type);*/ \
    } \
  } while (0)

typedef struct CascadedMsg {
  SimpleClsRef receiver;
  SimpleClsRef result;
} CascadedMsg;



#ifdef __cplusplus
}
#endif

#endif // GPLCONX_PARSEARG_CXX_H
