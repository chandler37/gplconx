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
  Tests the C++ string and array classes.
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <string.h>

#include "CString.hh"
#include "CArray.hh"
#include "Starray.hh"
#include "tester.hh"

static int tcstring(void);
static int tstringarray(void); // DLC test prepend()
static int tsimplearray(void); // DLC test prepend()

int tsimplearray(void)
// Returns zero if the CConxSimpleArray class passes.
{
  CConxSimpleArray<int> g1, g2, g3;
  RET1(g1 == g2);
  g1.append(3);
  g1.append(4);
  g1.append(5);
  RET1(g1.get(1) == 4);
  RET1(g1.get(0) == 3);
  RET1(g1.get(2) == 5);
  RET1(g1 != g2);
  g2.append(3);
  g2.append(4);
  g2.append(5);
  RET1(g1 == g2);
  RET1(g1 != g3);
  g2 = g3;
  RET1(g1 != g3);
  RET1(g1 != g2);
  RET1(g3 == g2);
  RET1(g2 == g3);
  CConxSimpleArray<int> g4 = g1;
  RET1(g4.get(1) == 4);

  return 0;
}

int tstringarray(void)
// Returns zero if the CConxStringArray class passes.
{
  THERE_ARE_ZERO_OBJECTS();
  OUT("\ntstringarray top\n");
  {
    CConxStringArray f1, f2; TAG(f1, -101); TAG(f2, -102);
    OUT("f2 is " << f2 << endl);
    RET1(f1 == f2);
    RET1(f1.size() == 0);
    f1.append("1");
    RET1(f1 != f2);
    f1.append("2");
    RET1(f1.get(0) == "1");
    RET1(f1.get(1) == "2");
    RET1(f1.size() == 2);
    CConxString *newS = new CConxString("newS"); TAG(*newS, -1000);
    f2.append(newS);
    f2 = f1; // newS is destroyed
    RET1(f2 == f1);
    RET1(f2.get(0) == "1");
    RET1(f2.get(1) == "2");
    RET1(f1.get(0) == "1");
    RET1(f1.get(1) == "2");

    OUT("f2 is " << f2 << endl);
  }
  THERE_ARE_ZERO_OBJECTS();


  {
    // DLC test the array append;
    CConxString *strings0[]
      = { new CConxString("first"), new CConxString("worst") };
    CConxString *strings1[]
      = { new CConxString("first"), new CConxString("worst") };
    TAG(*strings0[0], -103); TAG(*strings0[1], -104);
    TAG(*strings1[0], -120); TAG(*strings1[1], -121);
    CConxStringArray f12, f13, f14; TAG(f12, -105); TAG(f13, -106); TAG(f14, -107);
    f12.append(strings0, 0);
    f13.append(strings0, 1);
    delete strings0[1];
    f14.append(strings1, 2);
    ostrstream ostr0;
    ostr0 << f12 << endl;
    ostr0 << f13 << endl;
    ostr0 << f14 << endl;
    ostr0 << ends;
    const char *theStr0 = ostr0.str();
    OUT("We want `" << theStr0 << "'\n");
    delete [] theStr0;
  }
  THERE_ARE_ZERO_OBJECTS();


  {
    // Now test the constructors that let us initialize up to four elements.
    CConxStringArray f3("Obsessive ", CConxString("Compulsive "),
                        "Disorder ", "DSM IV "); // We're explicit once.
    CConxStringArray f4("Obsessive ", "Compulsive ", "Disorder ");
    CConxStringArray f5("Obsessive ", "Compulsive ");
    CConxStringArray f6("Obsessive ");
    CConxStringArray f7(new CConxString("Obsessive "),
                        new CConxString("Compulsive "),
                        new CConxString("Disorder "),
                        new CConxString("DSM IV "));
    CConxStringArray f8(new CConxString("Obsessive "),
                        new CConxString("Compulsive "),
                        new CConxString("Disorder "));
    CConxStringArray f9(new CConxString("Obsessive "),
                        new CConxString("Compulsive "));
    CConxStringArray f10(new CConxString("Obsessive "));
    CConxStringArray f11(NULL); // same as the default constructor.
    TAG(f3, -108); TAG(f4, -109); TAG(f5, -110); TAG(f6, -111); TAG(f7, -112);
    TAG(f8, -113); TAG(f9, -114); TAG(f10, -115); TAG(f11, -116);

    ostrstream ostr;
    ostr << f3 << endl;
    ostr << f4 << endl;
    ostr << f5 << endl;
    ostr << f6 << endl;
    ostr << f7 << endl;
    ostr << f8 << endl;
    ostr << f9 << endl;
    ostr << f10 << endl;
    ostr << f11 << endl;
    ostr << ends;
    char *theStr = ostr.str();
    OUT("We want `" << theStr << "'\n"); // DLC delete
    CConxString F00("<CConxPrintableOwnerArray size = 4 \"Obsessive \" \"Compulsive \" \"Disorder \" \"DSM IV \">\n"
                    "<CConxPrintableOwnerArray size = 3 \"Obsessive \" \"Compulsive \" \"Disorder \">\n"
                    "<CConxPrintableOwnerArray size = 2 \"Obsessive \" \"Compulsive \">\n"
                    "<CConxPrintableOwnerArray size = 1 \"Obsessive \">\n");
    F00 = 2 * F00 + "<CConxPrintableOwnerArray size = 0>\n";
    TAG(F00, -117);

    OUT ("We got `" << F00 << "'\n");
    RET1(F00 == theStr);
    delete [] theStr;
  }
  THERE_ARE_ZERO_OBJECTS();

  OUT("\ntstringarray bottom\n");
  return 0;
}

int tcstring(void)
// Returns zero if the CConxString class passes.
{
  OUT("\ntcstring top\n");
  CConxString v1; TAG(v1, -118);
  RET1(v1 + "hi" == "hi");
  CConxString c, e = "dull";
  if (VERBOSE()) cout << "c is `" << c << "'\n";
  if (VERBOSE()) cout << "e is `" << e << "'\n";
  RET1(e == CConxString(e.getString()));
  RET1(e == (const char *)CConxString(e.getString()));
  RET1(e == e.getString());
  RET1(e != CConxString("shiny"));
  RET1(e != (const char *)CConxString("shiny"));
  RET1(e != "shiny");
  RET1(e != CConxString("Dull"));
  RET1(e.equals(CConxString("Dull"), FALSE));
  CConxString dullshiny = e;
  if (VERBOSE())
    cout << "dullshiny before the append is `" << dullshiny << "'\n";
  dullshiny.append(" shin");
  dullshiny.append(CConxString("y"));
  if (VERBOSE())
    cout << "dullshiny after the append is `" << dullshiny << "'\n";
  RET1(dullshiny == CConxString("dull shiny"));
  RET1(e + " shiny" == CConxString("dull shiny"));
  RET1(e + CConxString(" shiny") == CConxString("dull shiny"));
  RET1(e + " shiny" + CConxString(" nickle") == "dull shiny nickle");
  RET1("dull shiny nickle" == e + " shiny" + CConxString(" nickle"));
  RET1(!strcmp("dull shiny nickle", e + " shiny" + CConxString(" nickle")));
  RET1(" shiny" + e == CConxString(" shinydull"));
  RET1(CConxString("0123").getLength() == 4);
  CConxString n23 = CConxString("23");
  CConxString n012345 = CConxString("01") + (n23 += "45");
  if (VERBOSE())
    cout << "n23 is `" << n23 << "'\n";
  RET1(n23 == "2345");
  RET1(n012345 == "012345");
  CConxString old_n012345 = n012345;
  n012345 += n012345;
  RET1(n012345 == "012345012345");
  if (VERBOSE())
    cout << "old_n012345.repetition(2) is `" << old_n012345.repetition(2)
         << "'\n";
  RET1(old_n012345.repetition(2) == "012345012345");
  RET1(old_n012345*2 == "012345012345");
  RET1(2*old_n012345 == "012345012345");
  RET1(old_n012345*4 == "012345012345""012345012345");
  RET1(old_n012345*0 == "");
  old_n012345*=4;
  RET1(old_n012345*1 == "012345012345""012345012345");
  RET1(CConxString(NULL)*7 == NULL);

  // -1 will be cast to size_t, so it will be ~0, a very, very large
  // positive integer.
  if (VERBOSE())
    cout << "CConxString(\"\")*" << (size_t) -1
         << " is \"" << CConxString("")*-1 << "\"\n";
  Boole caught = FALSE;
  // This should cause us to run out of memory, which means that a
  // const char * exception will be thrown.
  try {
    CConxString joeneg1 = CConxString("joe")*-1;
    if (VERBOSE()) cout << "CConxString(\"joe\")*" << (size_t) -1
         << " is `" << joeneg1 << "'\n";
  } catch (const char *oome) {
    caught = TRUE;
    if (VERBOSE()) cout << oome << ", which is as it should be.\n";
  }
  RET1(caught);

  // Test substring()
  CConxString v2 = "abc"; TAG(v2, -119);
  RET1(v2.substring(0, 8) == v2);
  RET1(v2.substring(0, 4) == v2);
  RET1(v2.substring(0, 3) == v2);
  RET1(v2.substring(1, 8) == "bc");
  RET1(v2.substring(1, 2) == "bc");
  RET1(v2.substring(1, 3) == "bc");
  RET1(v2.substring(2, 3) == "c");
  RET1(v2.substring(2, 2) == "c");
  RET1(v2.substring(2, 1) == "c");
  RET1(v2.substring(2, 0) == CConxString()); // unset
  RET1(v2.substring(3, 0) == CConxString()); // unset
  RET1(v2.substring(3, 1) == CConxString("")); // set to ""
  RET1(v2.substring(0, 2) == "ab");
  RET1(v2.substring(0, 2).getLength() == 2);

  // test numberOf()
  RET1(CConxString("cccc").numberOf('c') == 4);
  RET1(CConxString("cccc").numberOf('\0') == 1);
  RET1(CConxString("cccc").numberOf('b') == 0);
  RET1(CConxString("bcbc").numberOf('b') == 2);
  RET1(CConxString("bcbc").numberOf('C') == 0);
  RET1(CConxString("bcbc").numberOf('c') == 2);
  RET1(CConxString("").numberOf('\0') == 1);
  RET1(CConxString().numberOf('\0') == 0);

  int i;
  for (i = 0; i < 4; i++) {
    RET1(CConxString("duck").hasPrefix("duc", i, TRUE));
    RET1(CConxString("Duck").hasPrefix("duc", i, FALSE));
    RET1(!CConxString("Duck").hasPrefix("duc", i, TRUE));
  }
  RET1(!CConxString("Duck").hasPrefix("duc", 4, TRUE));

  for (i = 0; i < 4; i++) {
    RET1(CConxString("duc").isPrefixOf("duck", i, TRUE));
    RET1(CConxString("Duc").isPrefixOf("duck", i, FALSE));
    RET1(!CConxString("Duc").isPrefixOf("duck", i, TRUE));
  }
  RET1(!CConxString("Duc").isPrefixOf("duck", 4, TRUE));

  RET1(CConxString("duck").isPrefixOf("duck", 10));
  RET1(!CConxString("ducky").isPrefixOf("duck", 10));
  RET1(!CConxString("duc").isPrefixOf("duck", 4));

  RET1(!CConxString().isPrefixOf("du", 4));
  RET1(!CConxString("foo").isPrefixOf(CConxString(), 4));
  RET1(!CConxString().hasPrefix("du", 4));
  RET1(!CConxString("foo").hasPrefix(CConxString(), 4));

  // Test the long and double constructors.
  RET1(CConxString((long)3) == "3");
  RET1(CConxString((long)0017) == "15");
  RET1(CConxString((long)-303) == "-303");
  RET1(CConxString((long)-0) == "0");
  RET1(CConxString((long)-1) == "-1");

  RET1(CConxString(-303.0, 'g') == "-303");

  {
    struct tml {
      const char *n;
      long correct;
    };
    struct tml ls[] = {
      { "   +34", 34}, { "+34", 34},
      { "34", 34}, { " 34", 34},
      { " -99", -99}, { "-99", -99}, { "-99", -99},
      { "99", 99}, { " 99", 99}, { " 99", 99}, { "99", 99},
      { NULL, -1}
    };
    for (int i = 0; ls[i].n != NULL; i++) {
      long l;
      RET1(conx_str2l(ls[i].n, &l) == 0);
      RET1(l == ls[i].correct);
    }
  }

  OUT("\ntcstring bottom\n");
  return 0;
}


int main(int argc, char **argv)
{
  HANDLE_ARGS(argc, argv);

  // DLC finish this.
  
  // DLC The following helped me discover a memory allocation bug:
  // CConxObject::cerr_from_destructor = TRUE;
  
  THERE_ARE_ZERO_OBJECTS();
  TEST(tcstring() == 0);
  THERE_ARE_ZERO_OBJECTS();
  TEST(tsimplearray() == 0);
  THERE_ARE_ZERO_OBJECTS();
  TEST(tstringarray() == 0);
  THERE_ARE_ZERO_OBJECTS();
  return GOOD_TEST_EXIT_CODE;
}
