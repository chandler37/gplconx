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
  Implementation of C++ classes in `stfloat.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#else
extern "C" {
clock(void);
}
#endif

#include "stfloat.hh"
#include "sterror.hh"
#include "stboole.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsFloat::ansMachs = NULL;

Boole CClsFloat::rngIsSeeded = FALSE;

NF_INLINE
CClsBase::ErrType CClsFloat::ciActionRandomBetweenAnd(CClsBase **result,
                                                      CConxClsMessage &o) const
{
  NEED_N_FLOATS(2, fargv, o, result);
  RETURN_NEW_RESULT(result, new CClsFloat(getRandom(fargv[0], fargv[1])));
}

NF_INLINE
void CClsFloat::seedRNG()
{
  if (!rngIsSeeded) {
    srand48((long)clock());
    rngIsSeeded = TRUE;
  }
}

NF_INLINE
double CClsFloat::getRandom(double low, double high)
// Returns a random number in the interval [low, high), seeding first
// if necessary.
{
  if (low == high) return high; // not very random...
  if (low > high) {
    double t = low;
    low = high;
    high = t;
  }
  seedRNG();
  return (low + ((high - low) * drand48()));
}

NF_INLINE
void CClsFloat::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ansMachs->append(CConxClsAnsMach("between:and:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererBetweenAnd,
                                     "Returns true iff the inclusive range "
                                     "from the first argument to the second"
                                     " argument contains the receiver"));
    ansMachs->append(CConxClsAnsMach("set:",
                                     CConxClsAnsMach::OBJECT,
                                     oiAnswererSet,
                                     "Sets the receiver's value to the argument's floating-point value, coercing if possible (from a SmallInt, e.g.)"));
    ansMachs->append(CConxClsAnsMach("randomBetween:and:",
                                     CConxClsAnsMach::CLASS,
                                     ciAnswererRandomBetweenAnd,
                                     "Returns a random number in the given interval"));

  }
}

NF_INLINE
CClsBase::ErrType 
CClsFloat::oiActionSet(CClsBase **result, CConxClsMessage &o)
{
  CHECK_READ_ONLYNESS(result);
  NEED_N_FLOATS(1, fargv, o, result);
  setValue(fargv[0]);
  RETURN_THIS(result);
}

NF_INLINE
CClsBase::ErrType 
CClsFloat::oiActionBetweenAnd(CClsBase **result, CConxClsMessage &o) const
{
  NEED_N_FLOATS(2, fargv, o, result);
  RETURN_BOOLE(this->getValue() >= fargv[0] && this->getValue() <= fargv[1],
               result);
}

NF_INLINE
CConxString CClsFloat::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    char s[250];
    (void) sprintf(s, "%g", getValue());
    return CConxString(s);
  }
}

NF_INLINE
void CClsFloat::setValue(double od)
{
  if (isClassInstance()) throw "set not for `Float'";
  d = od;
}

NF_INLINE
double CClsFloat::getValue() const
{
  if (isClassInstance()) throw "get not for `Float'";
  return d;
}

NF_INLINE
CClsFloat &CClsFloat::operator=(const CClsFloat &o)
{
  (void) CClsNumber::operator=(o);
  d = o.d;
  return *this;
}

