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
  Tests the parser and scanner.
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <strstream.h>

#include "parsearg.h"
#include "parser.h"
#include "cparse.hh"
#include "clsmgr.hh"
#include "stcommon.hh"
#include "stconx.hh"
#include "cmparse.hh"
#include "sth_var.hh"
#include "tester.hh"

static int tall(void);
static int tldict(void);

// Global variables:
int conx_count_line = 0;

extern "C" {
extern int yydebug;
extern int yylex (void);
extern int yyparse (void);
extern void yyrestart ( FILE *input_file );
}

static void printTokens(FILE *inputf, FILE *f);

static void yprint(FILE *f, int r, void *v)
{
  (void) fprintf(f, "token:");
  conxP_yyprint(f, r, v);
  (void) fprintf(f, "\n");
}

void printTokens(FILE *inputf, FILE *f)
{
  int r;
  //  YYSTYPE my_yylval;
  
  yyrestart(inputf);

  while ((r = yylex()) != 0) {
    yprint(f, r, &yylval);
  }
  (void) fprintf(f, "Flex is done -- out of tokens.\n");
}

int tall(void)
{
  CClsFloat f;
  RET1(f.isType(f.CLS_FLOAT));
  RET1(f.isType(f.CLS_OBJECT_BASE));
  RET1(!f.isType(f.CLS_STRING));
  RET1(CClsBoolean().isType(f.CLS_BOOLEAN));
  RET1(CClsSymbol().isType(f.CLS_SYMBOL));
  RET1(!CClsBase().isType(f.CLS_SYMBOL));
  {
    ostrstream ostr;
    ostr << f << ends;
    const char *ostrs = ostr.str();
    RET1(CConxString("<CClsFloat class instance value:37>") == ostrs);
    delete [] ostrs;
  }
  CConxClsVariable a("fRef", &f);
  RET1(a.getPointee() == &f);
  RET1(a.getPointee() != NULL);
  return 0;
}

int tldict(void)
{
  LDictionary<CConxString, int> dict;
  CConxString A = "A";
  dict.set(A, 3);
  RET1(dict.hasKey(CConxString("A")));
  RET1(dict.get(CConxString("A")) == 3);
  RET1(dict.hasKey(A));
  A = "B";
  RET1(dict.hasKey(CConxString("A")));
  RET1(dict.get(CConxString("A")) == 3);
  RET1(!dict.hasKey(A));
  dict.set(A, 4);
  RET1(dict.hasKey(A));
  RET1(dict.hasKey(CConxString("B")));
  RET1(dict.get(A) == 4);
  if (LLL_ON()) dict.debug();
  return 0;
}

int main(int argc, char **argv)
{

try {
  Boole weAreDebugging = (argc == 7);
  if (!weAreDebugging) {
    SET_LOG_LEVEL(0);
  } else {
    SET_LOG_LEVEL(LOGG_CXX_LOC+1);
  }
  /* Try --help to learn how to work this ugly beast. */

  if (argc == 1) {
    TEST(tall() == 0);
    TEST(tldict() == 0);
  } else if (argc == 2) {
    if (CConxString("--help") == argv[1]) {
      cout << ""<< argv[0] << " causes the usual test.\n"
           << argv[0] << " arg!=--help causes the tokenizer to start up on stdin/stdout.\n"
           << argv[0] << " arg1 arg2 causes the parser to start up on stdin/stdout in debug mode.\n"
           << argv[0] << " arg1 arg2 arg3 [...] causes the parser to start up normally on stdin/stdout.\n";
      return 0;
    }

    printf("Type something to see how it's tokenized:\n");
    printTokens(stdin, stdout);
  } else {
    if (argc == 3) yydebug = 1;
    cout << "Type something to be parsed, or `System helpMe' for on-line help:\n";
    conxP_set_interactive(TRUE);
    CConxClsManager cm;
    conxP_change_manager(&cm);
    conxP_initialize();
    yyparse();
    conxP_change_manager(NULL);
  }
  if (weAreDebugging)
    cout  << "\n\nconx_count_line is " << conx_count_line << endl;

  return 0;
} catch (const char *errs) {
  cerr << "\nGPLconx tparser had a const char * exception thrown:\n`"
       << errs << "'\nAborting.\n";
  return 1;
} catch (...) {
  cerr << "\nGPLconx tparser had some exception thrown.\n\nAborting.\n";
  return 1;
}
} // int main(int, char **)
