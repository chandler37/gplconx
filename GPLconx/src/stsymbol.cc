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
  Implementation of C++ classes in `stsymbol.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stsymbol.hh"
#include "stsmalli.hh"
#include "sterror.hh"

Answerers *CClsSymbol::ansMachs = NULL;

NF_INLINE
CConxString CClsSymbol::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    return CConxString("#") + getValue();
  }
}

NF_INLINE
CClsSymbol &CClsSymbol::operator=(const CClsSymbol &o)
{
  (void) CClsCharacterArray::operator=(o);
  return *this;
}

NF_INLINE
CConxString CClsSymbol::nthKeyword(size_t n) const throw(const char *)
{
  if (n >= numberOfKeywords()) throw "n is too high";
  const char *s = getValue().getString();
  size_t count = 0, start = 0;
  // (n == 0) is the only time when start needs to be initialized
  // to zero, but it would take a very, very smart compiler to know that.
  for (size_t i = 0; ; i++) {
    if (s[i] == ':') {
      if (count + 1 == n) {
        start = i + 1;
      }
      if (count++ == n) {
        return getValue().substring(start, i - start);
      }
    }
  }
}

NF_INLINE
void CClsSymbol::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    ST_CMETHOD(ansMachs, "numberKeywords", "method naming", OBJECT,
               oiAnswererNumberKeywords,
               "Returns the number of keywords that the symbol represents");
    ST_CMETHOD(ansMachs, "nthKeyword:", "method naming", OBJECT,
               oiAnswererNthKeyword,
               "Returns the Nth keyword from the keyword method name that the symbol represents");
  }
}

NF_INLINE
CClsBase::ErrType 
CClsSymbol::oiActionNumberKeywords(CClsBase **result,
                                   CConxClsMessage &o) const
{
  RETURN_SMALLINT_R((long)numberOfKeywords());
}

NF_INLINE
CClsBase::ErrType 
CClsSymbol::oiActionNthKeyword(CClsBase **result, CConxClsMessage &o) const
{
  ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsSmallInt, n, o);
  if (n[0]->getValue() < 1) {
    RETURN_ERROR_R("Must be strictly positive");
  }
  if (n[0]->getValue() > (long) numberOfKeywords()) {
    RETURN_ERROR_R("Must be less than or equal to `self numberKeywords'");
  }
  RETURN_NEW_R(new CClsSymbol(nthKeyword(n[0]->getValue() - 1)));
}
