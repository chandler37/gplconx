/* -*- fundamental -*- */
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

/* This is a flex (lex) scanner for GPLconx. */

%option nounput

%{
/* DLC use HAVE_CONFIG_H ??? */
#include <string.h>
#include <assert.h>
#include "parsearg.h"
#include "parser.h"
#include "util.h"
extern int conx_count_line;
static int llerror(const char *errmsg);
int yylex (void) ;
%}


/* DLC change the comment cuz smalltalk uses '//'
 */

ID ([[:alpha:]][[:alnum:]]*)

%%

{ID}:		{ if (yyleng >= MAX_ID_LENGTH) {
                    /* error */
                    return llerror("Message with colon has a name that is too long");
                  }
                  (void) strcpy(yylval.id, yytext);
                  yylval.id[yyleng-1] = '\0';
                  return TOK_ID_COLON;
                }

#{ID}		{ if (yyleng-1 >= MAX_ID_LENGTH) {
                    /* error */
                    return llerror("#Literal is too long.");
                  }
                  (void) strcpy(yylval.id, yytext+1);
                  return TOK_SYMBOL;
                }

{ID}		{ if (yyleng >= MAX_ID_LENGTH) {
                    /* error */
                    return llerror("ID is too long.");
                  }
                  (void) strcpy(yylval.id, yytext);
                  return TOK_ID;
                }

'[^'\n]*'	{ if (yyleng - 2 >= MAX_ID_LENGTH) {
                    /* error */
                    return llerror("'String literal' is too long.");
                  }
                  (void) strncpy(yylval.id, yytext+1, yyleng - 2); /* DLC - 2??? */
                  yylval.id[yyleng - 2] = '\0';
                  return TOK_STRING_LITERAL;
                }

[\+-]?([0-9]+) {
                 int err = conx_str2l(yytext, &yylval.l);
                 if (err) {
                   assert(err < 0);
                   return llerror("The integer value won't fit.");
                 }
                 return TOK_LONG;
               }

[\+-]?(([0-9]*\.[0-9]+([eE][\+-]?[0-9]+)?)|([0-9]+((\.[0-9]+)|(\.[0-9]*([eE][\+-]?[0-9]+)?)))) {
  /* 3.1 3.1e-13 .1e+10 1e9 1.3 4 .4 */
  int err = conx_str2d(yytext, &yylval.r);
  if (err) {
    assert(err < 0);
    return llerror("The floating-point value won't fit.");
  }
  return TOK_REAL;
}


:=		{ return TOK_ASSIGN; }
,		{ return ','; }
\(		{ return '('; }
\)		{ return ')'; }
\.		{ return '.'; }
\;		{ return ';'; }
[ \t\r]+				/* eat spaces and allow MS-DOS text */
\/\/[^\n]*				/* comments begin with // in any place and continue until the end of the line. */
\\\n				conx_count_line++; /* continue onto next line */
\n				conx_count_line++; return TOK_STMT_END;

.		{ return llerror("Unrecognized char"); }

<<EOF>> { yyterminate(); }

%%

/* Linking with -lfl is unnecessary if we have yywrap: */
int yywrap(void) {
  return 1; /* This means that there is nothing left to parse. */
}

void llextern_set_interactive(int isInteractive)
{
/* DLC this causes a core dump, how do you use it? It's just faster, btw.
  yy_set_interactive(isInteractive);
*/
}

/* DLC make `Point new' initialize to new Floats */
int llerror(const char *errmsg)
/* DLC pass on errmsg in a const char * field in YYSTYPE */
{
  return TOK_ERROR;
}

