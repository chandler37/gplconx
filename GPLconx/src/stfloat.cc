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

#include <math.h>

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
  RETURN_FLOAT(result, getRandom(fargv[0], fargv[1]));
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
    ST_METHOD(ansMachs, "between:and:", OBJECT,
              oiAnswererBetweenAnd,
              "Returns true iff the inclusive range from the first argument to the second argument contains the receiver");
    ST_METHOD(ansMachs, "set:", OBJECT,
              oiAnswererSet,
              "Sets the receiver's value to the argument's floating-point value, coercing if possible (from a SmallInt, e.g.)");
    ST_METHOD(ansMachs, "randomBetween:and:", CLASS,
              ciAnswererRandomBetweenAnd,
              "Returns a random number in the given interval");
    ST_METHOD(ansMachs, "exp", OBJECT, oiAnswererExp,
              "Returns e to the receiver's power, the anti-natural-logarithm");
    ST_METHOD(ansMachs, "ln", OBJECT, oiAnswererLn,
              "Returns the natural logarithm of the receiver");
    ST_METHOD(ansMachs, "log10", OBJECT, oiAnswererLog10,
              "Returns the common (base 10) logarithm of the receiver");
    ST_METHOD(ansMachs, "Pi", CLASS, ciAnswererPi,
              "Returns a new object instance, Pi, the circumference of a circle with radius 1");
    ST_METHOD(ansMachs, "e", CLASS, ciAnswererE,
              "Returns a new object instance, e, the base of the natural logarithm");
    ST_METHOD(ansMachs, "sin", OBJECT, oiAnswererSine,
              "Returns the sine of the receiver");
    ST_METHOD(ansMachs, "cos", OBJECT, oiAnswererCosine,
              "Returns the cosine of the receiver");
    ST_METHOD(ansMachs, "tan", OBJECT, oiAnswererTangent,
              "Returns the tangent of the receiver");
    ST_METHOD(ansMachs, "plus:", OBJECT, oiAnswererPlus,
              "Returns the sum of the receiver and argument");
    ST_METHOD(ansMachs, "minus:", OBJECT, oiAnswererMinus,
              "Returns difference between the receiver and argument");
    ST_METHOD(ansMachs, "times:", OBJECT, oiAnswererTimes,
              "Returns the product of the receiver and argument");
    ST_METHOD(ansMachs, "dividedBy:", OBJECT, oiAnswererDividedBy,
              "Returns the receiver divided by the argument");
    ST_METHOD(ansMachs, "power:", OBJECT, oiAnswererPower,
              "Returns the receiver to the power of the argument");
    ST_METHOD(ansMachs, "negation", OBJECT, oiAnswererNegation,
              "Returns the negation of the receiver");
    ST_METHOD(ansMachs, "reciprocal", OBJECT, oiAnswererReciprocal,
              "Returns the reciprocal of the receiver");
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
CClsBase::ErrType 
CClsFloat::oiActionPlus(CClsBase **result, CConxClsMessage &o) const
{
  NEED_N_FLOATS(1, fargv, o, result);
  RETURN_FLOAT(result, getValue() + fargv[0]);
}

NF_INLINE
CClsBase::ErrType 
CClsFloat::oiActionNegation(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_FLOAT(result, -1.0 * getValue());
}

NF_INLINE
CClsBase::ErrType 
CClsFloat::oiActionReciprocal(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_FLOAT(result, 1.0 / getValue());
}

NF_INLINE
CClsBase::ErrType 
CClsFloat::oiActionMinus(CClsBase **result, CConxClsMessage &o) const
{
  NEED_N_FLOATS(1, fargv, o, result);
  RETURN_FLOAT(result, getValue() - fargv[0]);
}

NF_INLINE
CClsBase::ErrType 
CClsFloat::oiActionTimes(CClsBase **result, CConxClsMessage &o) const
{
  NEED_N_FLOATS(1, fargv, o, result);
  RETURN_FLOAT(result, getValue() * fargv[0]);
}

NF_INLINE
CClsBase::ErrType 
CClsFloat::oiActionDividedBy(CClsBase **result, CConxClsMessage &o) const
{
  NEED_N_FLOATS(1, fargv, o, result);
  RETURN_FLOAT(result, getValue() / fargv[0]);
}

NF_INLINE
CClsBase::ErrType 
CClsFloat::oiActionPower(CClsBase **result, CConxClsMessage &o) const
{
  NEED_N_FLOATS(1, fargv, o, result);
  RETURN_FLOAT(result, pow(getValue(), fargv[0]));
}

ACTION_WITH_NO_ARGS_IMPL(CClsFloat, const, ciActionPi,
                         RETURN_FLOAT_R(M_PI););
ACTION_WITH_NO_ARGS_IMPL(CClsFloat, const, oiActionExp,
                         RETURN_FLOAT_R(exp(getValue())););
ACTION_WITH_NO_ARGS_IMPL(CClsFloat, const, ciActionE,
                         RETURN_FLOAT_R(M_E););
ACTION_WITH_NO_ARGS_IMPL(CClsFloat, const, oiActionLn,
                         RETURN_FLOAT_R(log(getValue())););
ACTION_WITH_NO_ARGS_IMPL(CClsFloat, const, oiActionLog10,
                         RETURN_FLOAT_R(log10(getValue())););
ACTION_WITH_NO_ARGS_IMPL(CClsFloat, const, oiActionCosine,
                         RETURN_FLOAT_R(cos(getValue())););
ACTION_WITH_NO_ARGS_IMPL(CClsFloat, const, oiActionSine,
                         RETURN_FLOAT_R(sin(getValue())););
ACTION_WITH_NO_ARGS_IMPL(CClsFloat, const, oiActionTangent,
                         RETURN_FLOAT_R(tan(getValue())););

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
  if (isClassInstance()) throw "get not for `Float' class, only `Float' objects";
  return d;
}

NF_INLINE
CClsFloat &CClsFloat::operator=(const CClsFloat &o)
{
  (void) CClsNumber::operator=(o);
  d = o.d;
  return *this;
}

