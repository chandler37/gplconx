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

/* Support for parsing with yyparse(). */

#ifndef GPLCONX_CPARSE_CXX_H
#define GPLCONX_CPARSE_CXX_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "parsearg.h"

void conxP_yyerror(const char *s);
const char *conxP_get_accumulated_pyyerror(const char *breaker);
void conxP_handle_yyerror(SimpleClsRef *result, const char *first,
                          const char *breaker);
void conxP_get_variable(SimpleClsRef *r, const char *id);
void conxP_new_Float_instance(SimpleClsRef *result, double f);
void conxP_new_SmallInt_instance(SimpleClsRef *result, long l);
void conxP_new_Symbol_instance(SimpleClsRef *result, const char *s);
void conxP_new_String_instance(SimpleClsRef *result, const char *s);
void conxP_send_unary_message(SimpleClsRef *result, SimpleClsRef receiver,
                              const char *unaryMessage);
void conxP_initialize(void);
void conxP_final_cleanup(void);
void conxP_reinitialize(void);
void conxP_set_to_empty(SimpleClsRef *r);
int conxP_is_empty(SimpleClsRef r);
void conxP_set_to_new_err(SimpleClsRef *resultPtr, const char *errString);
void conxP_define_new_variable(SimpleClsRef *result, const char *varName);
void conxP_print_line_of_output(SimpleClsRef *result);
void conxP_new_assignment_list(AssnList *a);
void conxP_add_to_assignment_list(AssnList *a, const char *id);
long conxP_perform_and_delete_assignments(AssnList *a, SimpleClsRef assignTo);

void conxP_send_and_delete_keyword_message(SimpleClsRef *result,
                                           SimpleClsRef receiver,
                                           KeywordMessage *km);
void conxP_new_keyword_message(KeywordMessage *km);
void conxP_add_to_keyword_message(KeywordMessage *km, const char *keyword,
                                  SimpleClsRef m);


#include <stdio.h>

void conxP_yyprint (FILE *file, int type, void *value);

#ifdef __cplusplus
}
#endif

#endif // GPLCONX_CPARSE_CXX_H 1
