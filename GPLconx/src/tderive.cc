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
  Shows how to properly inherit in C++.
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>

#include "tester.hh"


//////////////////////////////////////////////////////////////////////////////
// This shows how you should subclass in single-inheritance C++, in general.
// There are different issues involved in using virtual base classes and
// regular base classes. It's more than ob.derived1::b and ob.derived2::b;
// when you use a virtual base class you cannot rely on the derived class's
// copy constructor to call the base class's copy constructor.  You
// must do it yourself, as with class C below.
//
// I.e., you must call the base classes' copy constructors explicitly from the
// virtually derived class's copy constructor so that the default copy
// constructor in the base class or the base class's base class
// will not be called.
//
// You must call the base class's operator=() from the derived class's
// operator=() and call the base class's copy constructor from the
// derived class's copy constructor regardless of the type of inheritance.
//
// See `README.DEV' for more.
class A {
public:
  A(int a)
  {
    OUT("in A(a)\n");
    setA(a);
  }
  A()
  {
    OUT("in A default constructor\n");
    a = 0;
  }
  A(const A &o)
  {
    OUT("in A copy constructor\n");
    a = o.a;
  }
  ~A()
  {
    OUT("in A destructor\n");
  }
  A &operator=(const A &o)
  {
    OUT("in A assignment operator\n");
    a = o.a;
    return *this;
  }
  int operator==(const A &o) const
  {
    return a == o.a;
  }
  int operator!=(const A &o) const { return !operator==(o); }
  void setA(int a) { this->a = a; }
  int getA() const { return a; }


private:
  int a;
}; // class A


//////////////////////////////////////////////////////////////////////////////
// B specializes A.
class B : virtual public A {
public:
  B(int a, int b)
  {
    OUT("in B(a, b)\n");
    setA(a);
    setB(b);
  }
  B()
  {
    OUT("in B default constructor\n");
    b = 0;
  }
  B(const B &o)
    : A(o)
  {
    OUT("in B copy constructor\n");
    b = o.b;
  }
  ~B()
  {
    OUT("in B destructor\n");
  }
  B &operator=(const B &o)
  {
    (void) A::operator=(o);
    OUT("in B assignment operator\n");
    b = o.b;
    return *this;
  }
  int operator==(const B &o) const
  {
    if (A::operator!=(o)) return 0;
    return b == o.b;
  }
  int operator!=(const B &o) const { return !operator==(o); }
  void setB(int b) { this->b = b; }
  int getB() const { return b; }


private:
  int b;
}; // class B


//////////////////////////////////////////////////////////////////////////////
// C specializes B.
class C : virtual public B {
public:
  C(int a, int b, int c)
  {
    OUT("in C(a, b, c)\n");
    setA(a);
    setB(b);
    setC(c);
  }
  C()
  {
    OUT("in C default constructor\n");
    c = 0;
  }
  C(const C &o)
    : B(o), A(o)
  {
    OUT("in C copy constructor\n");
    c = o.c;
  }
  ~C()
  {
    OUT("in C destructor\n");
  }
  C &operator=(const C &o)
  {
    (void) B::operator=(o);
    OUT("in C assignment operator\n");
    c = o.c;
    return *this;
  }
  int operator==(const C &o) const
  {
    if (B::operator!=(o)) return 0;
    return c == o.c;
  }
  int operator!=(const C &o) const { return !operator==(o); }
  void setC(int c) { this->c = c; }
  int getC() const { return c; }


private:
  int c;
}; // class C


//////////////////////////////////////////////////////////////////////////////
// Cbad specializes B.
class Cbad : virtual public B {
public:
  Cbad(int a, int b, int c)
  {
    OUT("in Cbad(a, b, c)\n");
    setA(a);
    setB(b);
    setC(c);
  }
  Cbad()
  {
    OUT("in Cbad default constructor\n");
    c = 0;
  }
  Cbad(const Cbad &o)
    : B(o) // we should call A(o) but we don't, hence the `bad' in `Cbad'.
  {
    OUT("in Cbad copy constructor\n");
    c = o.c;
  }
  ~Cbad()
  {
    OUT("in Cbad destructor\n");
  }

  // The default operator= would work, but we'd have to write our own if
  // we had a pointer attributes.  This is the wrong way to write it:
  Cbad &operator=(const Cbad &o)
  {
    // we should call (void) B::operator=(o);, but we don't.
    OUT("in Cbad assignment operator\n");
    c = o.c;
    return *this;
  }

  int operator==(const Cbad &o) const
  {
    if (B::operator!=(o)) return 0;
    return c == o.c;
  }
  int operator!=(const Cbad &o) const { return !operator==(o); }
  void setC(int c) { this->c = c; }
  int getC() const { return c; }


private:
  int c;
}; // class Cbad


//////////////////////////////////////////////////////////////////////////////
// Now we use A as a normal base class:
class Bnormal : public A {
public:
  Bnormal(int a, int b)
  {
    OUT("in Bnormal(a, b)\n");
    setA(a);
    setB(b);
  }
  Bnormal()
  {
    OUT("in Bnormal default constructor\n");
    b = 0;
  }
  Bnormal(const Bnormal &o)
    : A(o)
  {
    OUT("in Bnormal copy constructor\n");
    b = o.b;
  }
  ~Bnormal()
  {
    OUT("in Bnormal destructor\n");
  }
  Bnormal &operator=(const Bnormal &o)
  {
    (void) A::operator=(o);
    OUT("in Bnormal assignment operator\n");
    b = o.b;
    return *this;
  }
  int operator==(const Bnormal &o) const
  {
    if (A::operator!=(o)) return 0;
    return b == o.b;
  }
  int operator!=(const Bnormal &o) const { return !operator==(o); }
  void setB(int b) { this->b = b; }
  int getB() const { return b; }


private:
  int b;
}; // class Bnormal


//////////////////////////////////////////////////////////////////////////////
class Cnormal : public Bnormal {
public:
  Cnormal(int a, int b, int c)
  {
    OUT("in Cnormal(a, b, c)\n");
    setA(a);
    setB(b);
    setC(c);
  }
  Cnormal()
  {
    OUT("in Cnormal default constructor\n");
    c = 0;
  }
  Cnormal(const Cnormal &o)
    : Bnormal(o) // if A were virtually inherited, we'd need A(o)
  {
    OUT("in Cnormal copy constructor\n");
    c = o.c;
  }
  ~Cnormal()
  {
    OUT("in Cnormal destructor\n");
  }
  Cnormal &operator=(const Cnormal &o)
  {
    (void) Bnormal::operator=(o);
    OUT("in Cnormal assignment operator\n");
    c = o.c;
    return *this;
  }
  int operator==(const Cnormal &o) const
  {
    if (Bnormal::operator!=(o)) return 0;
    return c == o.c;
  }
  int operator!=(const Cnormal &o) const { return !operator==(o); }
  void setC(int c) { this->c = c; }
  int getC() const { return c; }


private:
  int c;
}; // class Cnormal


//////////////////////////////////////////////////////////////////////////////
class Cbadnormal : public Bnormal {
public:
  Cbadnormal(int a, int b, int c)
  {
    OUT("in Cbadnormal(a, b, c)\n");
    setA(a);
    setB(b);
    setC(c);
  }
  Cbadnormal()
  {
    OUT("in Cbadnormal default constructor\n");
    c = 0;
  }
  Cbadnormal(const Cbadnormal &o)
    : Bnormal(o) // we should call A(o) but we don't, hence the `bad' in `Cbadnormal'.
  {
    OUT("in Cbadnormal copy constructor\n");
    c = o.c;
  }
  ~Cbadnormal()
  {
    OUT("in Cbadnormal destructor\n");
  }

  // The default operator= will work just fine.
#if 0
  Cbadnormal &operator=(const Cbadnormal &o)
  {
    // we should call (void) Bnormal::operator=(o);, but we don't.
    OUT("in Cbadnormal assignment operator\n");
    c = o.c;
    return *this;
  }
#endif

  int operator==(const Cbadnormal &o) const
  {
    if (Bnormal::operator!=(o)) return 0;
    return c == o.c;
  }
  int operator!=(const Cbadnormal &o) const { return !operator==(o); }
  void setC(int c) { this->c = c; }
  int getC() const { return c; }


private:
  int c;
}; // class Cbadnormal

class D1 {
public:
  D1() { }
  virtual const char *shout() { return "D1 shout\n"; }

  // This is a virtual function calling a virtual function:
  virtual const char *shouter() { return shout(); }

}; // class D1

class D2 : public D1 {
public:
  D2() { }
  virtual const char *shout() { return "D2 shout\n"; }
}; // class D2

class D3 : public D2 {
public:
  D3() { }
  virtual const char *shout() { return "D3 shout\n"; }
}; // class D3

static int test_normal_base_classes(void);
static int test_virtvirt(void);
static int test_virtual_base_classes(void);

int test_virtvirt(void)
{
  class D1 *d1p;
  class D2 *d2p;
  class D3 d3;
  
  d1p = &d3;
  d2p = &d3;
  RET1(!strcmp("D3 shout\n", d1p->shout()));
  RET1(!strcmp("D3 shout\n", d1p->shouter()));
  RET1(!strcmp("D3 shout\n", d2p->shout()));
  RET1(!strcmp("D3 shout\n", d2p->shouter()));
  return 0;
}

int test_normal_base_classes(void)
// Returns zero if the CConxString class passes.
{
  OUT("Making sure that int a, int b, and int c all affect equality\n");
  Cnormal t1(133, 134, 135);
  RET1(t1 == Cnormal(133, 134, 135));
  RET1(t1 != Cnormal(-1, 134, 135));
  RET1(t1 != Cnormal(133, -1, 135));
  RET1(t1 != Cnormal(133, 134, -1));

  OUT("Before copying a Cnormal instance\n");
  Cnormal t2 = t1; // invokes the copy constructor.
  OUT("After copying a Cnormal instance\n");
  RET1(t2 == t1);

  Cbadnormal tbad00(180, 181, 182);
  Cbadnormal tbad01(183, 184, 185);
  tbad00 = tbad01; // invokes the assignment operator
  RET1(tbad00 == tbad01);

  Cbadnormal tbad1(133, 134, 135);
  RET1(t1 == Cnormal(133, 134, 135));
  RET1(t1 != Cnormal(-1, 134, 135));
  RET1(t1 != Cnormal(133, -1, 135));
  RET1(t1 != Cnormal(133, 134, -1));

  OUT("Before copying a Cbadnormal instance\n");
  Cbadnormal tbad2 = tbad1; // invokes the copy constructor.
  OUT("After copying a Cbadnormal instance\n");
  RET1(tbad2 == tbad1);

  A ta1 = t1;
  RET1(ta1 == t1);
  t1.setB(233);
  RET1(ta1 == t1);
  ta1.setA(76);
  RET1(ta1 != t1);
  return 0;
}

int test_virtual_base_classes(void)
// Returns zero if the CConxString class passes.
{
  OUT("Making sure that int a, int b, and int c all affect equality\n");
  C t1(133, 134, 135);
  RET1(t1 == C(133, 134, 135));
  RET1(t1 != C(-1, 134, 135));
  RET1(t1 != C(133, -1, 135));
  RET1(t1 != C(133, 134, -1));

  OUT("Before copying a C instance\n");
  C t2 = t1; // invokes the copy constructor.
  OUT("After copying a C instance\n");
  RET1(t2 == t1);

  Cbad tbad00(180, 181, 182);
  Cbad tbad01(183, 184, 185);
  tbad00 = tbad01; // invokes the faulty assignment operator
  RET1(tbad00 != tbad01);

  Cbad tbad1(133, 134, 135);
  RET1(t1 == C(133, 134, 135));
  RET1(t1 != C(-1, 134, 135));
  RET1(t1 != C(133, -1, 135));
  RET1(t1 != C(133, 134, -1));

  OUT("Before copying a Cbad instance\n");
  Cbad tbad2 = tbad1; // invokes the copy constructor.
  OUT("After copying a Cbad instance\n");
  RET1(tbad2 != tbad1);

  A ta1 = t1;
  RET1(ta1 == t1);
  t1.setB(233);
  RET1(ta1 == t1);
  ta1.setA(76);
  RET1(ta1 != t1);
  return 0;
}

int main(int argc, char **argv)
{
  HANDLE_ARGS(argc, argv);
  OUT("Before virtvirt test\n");
  TEST(test_virtvirt() == 0);
  OUT("Before virtual base class test\n");
  TEST(test_virtual_base_classes() == 0);
  OUT("\n\n\nBefore normal base class test\n");
  TEST(test_normal_base_classes() == 0);
  OUT("\nNormal finish.\n");
  return GOOD_TEST_EXIT_CODE;
}
