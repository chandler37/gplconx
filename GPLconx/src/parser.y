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

/* DLC you can say Real := Float for a class alias. */

/* This is a parser for a little language that will be used to
   provide a user experience somewhat like Smalltalk, but without
   the enormous class library.  Interaction will be like this:

> P1 := Point new
> // Variables have no types, P1 could next be set to ``point to'' a new
> // instance of class Line.  Yes, `Point' is a class, and `new' is a
> // message passed to it.
> 
> P2 := Point x: 0.34 y: 1 model: #uhp.  P3 := P1 clone // P3 points to a
> // new instance that is exactly like the instance that P1 ``points to''.
> L1 := Line segment: True A: (P4 := Point x: 0.33 y: 0.24) B: Point new
> L2 := Line A: Point x: 0.11 y: 0.12 model:#klein B: \
>    (P5 := Point x: 0.33 y: 0.24)
> 
> Klein draw: #L1 draw: L2
> // draw:, draw:draw:, draw:draw:draw:, draw:draw:draw:draw: are provided
> // as shortcuts.
> // When we actually redraw the Klein disk, the value of L1 at the time
> // will be used.
> System references
>>> There are 7 references: L2 P5 P1 L1 P3 P2 P1
> System undef: #L1
> Klein redisplay
>>> Error in Klein display list -- L1 is not defined, but current value
>>>         is desired.
> L1 := L2
> Klein redisplay
> // That went fine.  The Klein disk, by the way, probably is not going to
> // draw L2 only once, though it would be a nice optimization in this simple
> // case.  There is reason to redraw when another object is in the middle --
> // you get a red circle over a yellow ellipse but under a cyan parabola.
> 
> Circle radius: 1e-3 center: Point new
> // The garbage collector just threw a new Point instance, a new Circle
> // instance, and a new Float instance away.
> 
> AllModels draw: Circle radius: 1e-3 center: Point new
> // The garbage collector won't touch these three instances.
> // Also, note that the unary message is executed before the
> // keyword message.  The assignment operator `:=' has the
> // least precedence.
> 
> // In fact, we can recover the last Circle instance created:
> circ := Klein drawingAt: -1
> 
> Float help
>>> Class `Float' answers the following messages:
>>>   help
>>>   reciprocal
>>>   new:
>>>   between:and:
> System reset
> System exit

   To allow neat tricks like saying `Circle := Float' and then _not_ losing
   access to the class Circle until the next restart, you may not
   redefine `System'.  `Circle := System new: #Circle' will get you back on
   track without a restart.

   Since this is a Smalltalk-like language, see Patrick Henry Winston's
   _ON TO SMALLTALK_ for more.  Chunk 45 describes precedence rules, e.g.

   We cause parsing to produce a result at each newline (press of the enter
   key).  If this is not what you want to happen, change TOK_STMT_END in
   yylex().

   The last statement on a line is the one that is printed.

DLC eliminate shift/reduce conflicts.  Make sure that you get:
<STARTUP>
> L M N
>>> L does not accept the #M message
> P Q: R S T: #U
>>> R does not accept the #S message

 // because we want left-to-right of unary.

> (Point new) huh
>>> Point does not have an instance #huh method
> Point new class
>>> Point
> Point class
>>> Point class

consider allowing <error '.'>

DLC let symbols hold keyword selectors, like #at:put:

DLC See ``And now->The Syntax'' in GNU Smalltalk manual for more.


'65 between: 64 and: (#( 65 66 ) at: 2)' is 'true', but
'65 between: 64 and: #( 65 66 ) at: 2' is a parse error.  This explains
how a keymsg should work.

Note that 'L := M := N' is ok, but 'L := M:=N' is bad because `M:' is the
token taken, not `:='.

We don't do binary messages, and arithmetic could just use `+:' if we
did not want to implement it.

Use class UndefinedObject's instance nil and give all objects the
notNil predicate.
*/


%{
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cparse.hh"

extern int conx_count_line; /* DLC */

#define ERROR_HEADER ""
/* #define ERROR_HEADER "Syntax error: " */

static void yyerror ( const char *error ) ;
extern int yylex (void) ;

/* YYPRINT prints a token so that `bison -t' has human-readable output,
   so long as the human has the `.output' from `bison -v' beside her.
*/
#define YYPRINT(file, toktype, value)   local_yyprint (file, toktype, value)
#define YYDEBUG 1
#define YYERROR_VERBOSE 1
%}

%union {
#define MAX_ID_LENGTH 512
char id[MAX_ID_LENGTH];
double r;
long l;
SimpleClsRef otherthing;
CascadedMsg cascmsg;
KeywordMessage kmsg;
AssnList assnlist;
}


%{
  /* DLC this needs to show dd's message */
#define HANDLE_ERROR(dd) \
                  if (conxP_is_parse_error(dd)) { \
                    yyerror(conxP_get_specific_parse_error(dd)); \
                    YYERROR; \
                  }

#define HANDLE_KEYWORD_MESSAGE(resultptr, receiver, keymsg) \
   conxP_send_and_delete_keyword_message(resultptr, receiver, keymsg)

/*
  Pass on the unary message msg to recv. If the message is not answered,
  give an error.  If the message is answered, recv will be affected by it.
  result becomes the resulting recv.
*/
#define TRY_UNARY_MESSAGE(resultPtr, recv, msg) \
             conxP_send_unary_message(resultPtr, recv, (msg))


static void local_yyprint(FILE *f, int toktype, YYSTYPE v);
%}

%token <id> TOK_SYMBOL TOK_ID TOK_ID_COLON TOK_STRING_LITERAL TOK_ERROR
%token <id> TOK_BINARY_OP
%token TOK_STMT_END
%token <r> TOK_REAL
%token <l> TOK_LONG
%right TOK_ASSIGN
%left TOK_BINARY_OP
%type <otherthing> keywordarg obj simpleobj fancyobj msgobj cascrecv binaryobj
%type <cascmsg> carriedobj
%type <otherthing> partofline line
%type <kmsg> keymsg
%type <assnlist> assignments

   /*  %type  <str>     exp_str
       %type  <bool>    exp_yesno */


%%

/* TOK_ERROR is a scanner error. */


input:	/* empty */ {
                        /* we could initialize here. */
                    }
	| input line
                    {
                            /* DLC print what line represents. */
                            /* DLC Then do GC unless the line's value is
                               assigned to an implicit variable (neat feature)
                               like `ans' which might just solve the GC problem!
                            */




                      if (!conxP_is_empty($2)) {
                        /* conxP_define_new_variable(&$$, "ans");*/
                        /* First undefine the variable?? DLC Try `2.2 ans' */
                        AssnList al; long code;
                        conxP_new_assignment_list(&al);
                        conxP_add_to_assignment_list(&al, "ans");
                        code = conxP_perform_and_delete_assignments(&al, $2);
                        assert(code != -1);
                        if (code != 0) {
                          assert(code > 0);
                          /* Cannot reassign a reserved word variable */
                        }
                      }
                      conxP_print_line_of_output(&$2);
                    }
;

   /* partofline is the first part of a line of input.  An obj followed
      by a newline can come after it to make a complete line.
      partofline represents the rightmost obj.
    */
partofline:	/* empty */ { conxP_set_to_empty(&$$); }
		| partofline '.' { $$ = $1; }
		| partofline obj '.' { $$ = $2; }/*
		| partofline TOK_ERROR '.'
                        {
                          char s[MAX_ID_LENGTH + 80];
                          (void) sprintf(s, "%s%s", "Scanner error -- ", $2);
                          conxP_set_to_new_err(&$$, s);
                          YYERROR;
                        }
		| partofline error '.'
                        {
                          conxP_handle_yyerror(&$$, ERROR_HEADER, ";;");
                          } DLC */
;

line:	partofline TOK_STMT_END { $$ = $1; }
	| partofline obj TOK_STMT_END {
                              if (!conxP_is_parse_error($1))
                                $$ = $2;
                              else
                                $$ = $1;
        }
	| partofline TOK_ERROR
                 {
                   char s[MAX_ID_LENGTH + 80];
                   (void) sprintf(s, "%s%s", "Scanner error -- ", $2);
                   yyerror(s);
                   /* DLC delete                   conxP_set_to_new_err(&$$, s); */
                   YYERROR;
                 }
	| partofline error TOK_STMT_END
                 {
                   conxP_handle_yyerror(&$$, ERROR_HEADER, ";;");
                   /* DLC decide:                  yyerrok; */
                 }
;

/* The 'x: 3 y: 4' from 'P := Point x: 3 y: 4' is a keyword message,
   an keymsg:
*/
keymsg:	TOK_ID_COLON keywordarg
               {
                 conxP_new_keyword_message(&$$);
                 conxP_add_to_keyword_message(&$$, $1, $2);
               }
	| keymsg TOK_ID_COLON keywordarg
               {
                 conxP_add_to_keyword_message(&$1, $2, $3);
                 $$ = $1;
               }
;

keywordarg:	binaryobj
                     {
                       $$ = $1; /* No, it can't be `ID keymsg', e.g. --
                                   that makes 'a b: c d: e' ambiguous.   */
                     }
;

assignments:	TOK_ID TOK_ASSIGN
                        {
                          conxP_new_assignment_list(&$$);
                          conxP_add_to_assignment_list(&$$, $1);
                          /* DLC if we get a parse error, who deletes the
                             assignment list????  
                             Should we have 'assignments error { delete $1 }' ?
                          */
                        }
	        | assignments TOK_ID TOK_ASSIGN
                        { $$ = $1; conxP_add_to_assignment_list(&$$, $2); }
;

obj:	fancyobj
;

fancyobj:	msgobj
		| carriedobj
			{ $$ = $1.result; /* GC receiver? */ }

;

obj:	assignments fancyobj
		{
                  /* DLC should this be assignments obj ???? */
                  /* I removed the parens DLC  DLC does 'ID [:= ID]* := obj' */
                  long code = conxP_perform_and_delete_assignments(&$1, $2);
                  assert(code != -1);
                  if (code != 0) {
                    assert(code > 0);
                    conxP_set_to_new_err(&$$, "Cannot reassign that variable, "
                                         "it is reserved.");
                    /* DLC `System getReservedWords' */
                  } else {
                    $$ = $2;
                  }
                }
;

msgobj:	binaryobj
	| binaryobj keymsg /* 'between:and:'-style message */
		{
                  HANDLE_KEYWORD_MESSAGE(&$$, $1, &$2);
                  HANDLE_ERROR($$);
                }
;

cascrecv:	binaryobj
		| binaryobj keymsg /* 'between:and:'-style message */
			{ /* A cascaded message goes to $1, not the result of
                             applying the keyword message. */
	                  HANDLE_KEYWORD_MESSAGE(&$$, $1, &$2);
                          HANDLE_ERROR($$);
                          $$ = $1;
	                }
;


/* Cascading ';' support: */
carriedobj:	cascrecv ';' TOK_ID /* unary message */
	              {
                        $$.receiver = $1;
	                TRY_UNARY_MESSAGE(&($$.result), $1, $3);
                        HANDLE_ERROR($$.result);
	              }
		| cascrecv ';' keymsg
	              {
                        $$.receiver = $1;
	                HANDLE_KEYWORD_MESSAGE(&($$.result), $1, &$3);
                        HANDLE_ERROR($$.result);
	              }
		| carriedobj ';' keymsg
	              {
                        $$.receiver = $1.receiver;
	                HANDLE_KEYWORD_MESSAGE(&($$.result), $1.receiver,
                                               &$3);
                        HANDLE_ERROR($$.result);
	              }
		| carriedobj ';' TOK_ID /* unary message */
	              {
                        $$.receiver = $1.receiver;
	                TRY_UNARY_MESSAGE(&($$.result), $1.receiver, $3);
                        HANDLE_ERROR($$.result);
	              }
;

simpleobj:	TOK_REAL
{
 /* DLC */
        conxP_new_Float_instance(&$$, $1);
}
;

simpleobj:	TOK_LONG
{
 /* DLC */
        conxP_new_SmallInt_instance(&$$, $1);
}
;

simpleobj:	TOK_ID
{
  conxP_get_variable(&$$, $1); /* Creates if it does not exist. */
}
;

simpleobj:	TOK_SYMBOL { conxP_new_Symbol_instance(&$$, $1); }
;

simpleobj:	TOK_STRING_LITERAL { conxP_new_String_instance(&$$, $1); }
;

simpleobj:	simpleobj TOK_ID /* unary message */
{
        /* `id unarymessage unarymessage', e.g., runs through this twice. */
        TRY_UNARY_MESSAGE(&$$, $1, $2);
        HANDLE_ERROR($$);
}
;

simpleobj:	'(' obj ')' /* to override the usual unary-first precedence */
                        { $$ = $2; }
;

binaryobj:	simpleobj
		| binaryobj TOK_BINARY_OP simpleobj {
                     KeywordMessage tmp;
                     conxP_new_keyword_message(&tmp);
                     conxP_add_to_keyword_message(&tmp, $2, $3);
                     conxP_send_and_delete_keyword_message(&$$, $1, &tmp);
                  }
;

/* DLC TODO a help method for each object and class. */

/* DLC allow nil? */
/* DLC how does P := Q work? */

%%

/* Our code goes here. */
void yyerror(const char *s)
{
  conxP_yyerror(s);
}

void local_yyprint(FILE *f, int toktype, YYSTYPE v)
{
  conxP_yyprint(f, toktype, &v);
}

void conxP_yyprint (FILE *f, int r, void *v)
{
  YYSTYPE *value = ((YYSTYPE *)v);
  if (r == TOK_ID) {
    (void) fprintf(f, " %s", value->id);
  } else if (r == TOK_SYMBOL) {
    (void) fprintf(f, " #%s", value->id);
  } else if (r == TOK_ID_COLON) {
    (void) fprintf(f, " %s:", value->id);
  } else if (r == TOK_STRING_LITERAL) {
    (void) fprintf(f, " '%s'", value->id);
  } else if (r == TOK_REAL) {
    (void) fprintf(f, " %e", value->r);
  } else if (r == TOK_ASSIGN) {
    (void) fprintf(f, " :=");
  } else if (r == TOK_BINARY_OP) {
    (void) fprintf(f, " %s", value->id);
  } else if (r == TOK_LONG) {
    (void) fprintf(f, " %ld", value->l);
  } else if (r == TOK_ERROR) {
    (void) fprintf(f, " <Flex encountered an error, `%s'.>", value->id);
  } else {
    if (r <= 255) {
      if (r == '\n')
        (void) fprintf(f, " \\n");
      else
        (void) fprintf(f, " '%c'", r);
    } else {
      if (r == TOK_STMT_END)
        (void) fprintf(f, " \\n");
      else {
        (void) fprintf(f, " %d", r);
        /* Did you add a new token and not update this function? */
      }
    }
  }
}
