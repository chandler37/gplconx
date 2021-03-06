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
/* DLC delete static int llerror(const char *errmsg); */
#define RETURN_BINARYOP(k) \
    yylval.id[0] = k; yylval.id[1] = '\0'; return TOK_BINARY_OP
#define RETURN_YYTEXT_BINARYOP() \
    (void) strcpy(yylval.id, yytext); yylval.id[yyleng] = '\0'; \
    return TOK_BINARY_OP
#define LLERROR(msg) assert(strlen(msg) < MAX_ID_LENGTH); strcpy(yylval.id, (msg)); return TOK_ERROR
int yylex (void) ;
%}


/* DLC change the comment cuz smalltalk uses '//'
 */

ID ([[:alpha:]][[:alnum:]]*)
BINARYOP ([\+\-*/\\%@,&|<>=~])

%%

{ID}:		{ if (yyleng >= MAX_ID_LENGTH) {
                    /* error */
                    LLERROR("Message with colon has a name that is too long");
                  }
                  (void) strcpy(yylval.id, yytext);
                  yylval.id[yyleng-1] = '\0';
                  return TOK_ID_COLON;
                }

#({ID}|:|_|$|{BINARYOP})+	{
                  if (yyleng-1 >= MAX_ID_LENGTH) {
                    /* error */
                    LLERROR("#Literal is too long.");
                  }
                  (void) strcpy(yylval.id, yytext+1);
                  return TOK_SYMBOL;
                }

{ID}		{ if (yyleng >= MAX_ID_LENGTH) {
                    /* error */
                    LLERROR("ID is too long.");
                  }
                  (void) strcpy(yylval.id, yytext);
                  return TOK_ID;
                }

'[^'\n]*'	{ if (yyleng - 2 >= MAX_ID_LENGTH) {
                    /* error */
                    LLERROR("'String literal' is too long.");
                  }
                  (void) strncpy(yylval.id, yytext+1, yyleng - 2); /* DLC - 2??? */
                  yylval.id[yyleng - 2] = '\0';
                  return TOK_STRING_LITERAL;
                }

[\+-]?(([0-9]+\.[0-9]+([eE][\+-]?[0-9]+)?)|([0-9]+[eE][\+-]?[0-9]+)) {
  /* 3.1 3.1e-13 0.1e+10 4e9 +1.3e+009 -3.3 -2e+013
        but not .3 or 3. or +.3 or +3. since that would make '.' a poor
        statement separator.
   */
  int err = conx_str2d(yytext, &yylval.r);
  if (err) {
    assert(err < 0);
    LLERROR("The floating-point value won't fit.");
  }
  return TOK_REAL;
}

[\+-]?([0-9]+) {
                 int err = conx_str2l(yytext, &yylval.l);
                 if (err) {
                   assert(err < 0);
                   LLERROR("The integer value won't fit.");
                 }
                 return TOK_LONG;
               }

:=		{ return TOK_ASSIGN; }
((==)|(>=)|(<=)|(~=)|(~~))	{ RETURN_YYTEXT_BINARYOP(); }
{BINARYOP}	{ RETURN_BINARYOP(yytext[0]); }
\(		{ return '('; }
\)		{ return ')'; }
\.		{ return '.'; }
\;		{ return ';'; }
[ \t\r]+				/* eat spaces and allow MS-DOS text */
\/\/[^\n]*				/* comments begin with // in any place and continue until the end of the line. */
\\\n				conx_count_line++; /* continue onto next line */
\n				conx_count_line++; return TOK_STMT_END;

.		{ LLERROR("Unrecognized character"); }

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
/* DLC delete int llerror(const char *errmsg)
/  * DLC pass on errmsg in a const char * field in YYSTYPE *   /
{
  return TOK_ERROR;
}
*/
