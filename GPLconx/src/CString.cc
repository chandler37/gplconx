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
  C++ string class.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <stdio.h>

extern "C" {
#include "util.h"
}

#include "CString.hh"

CF_INLINE
CConxString::CConxString()
  : data(NULL), dsize(0)
{
  MMM1("CConxString()");
}

CF_INLINE
CConxString::CConxString(long l) throw(const char *)
  : data(NULL), dsize(0)
{
  MMM1("CConxString(long l)");
  char s[80];
  (void) sprintf(s, "%ld", l);
  setString(s);
}

CF_INLINE
CConxString::CConxString(double f, char conversion,
                         int digits) throw(const char *)
  : data(NULL), dsize(0)
{
  MMM1("CConxString(double f, char conversion, int digits)");
  if (conversion != 'e' && conversion != 'E'
      && conversion != 'f' && conversion != 'F'
      && conversion != 'g' && conversion != 'G')
    throw "bad conversion, not in [eEfFgG]";
  char s[80], formatString[80];
  if (digits > 0 && digits < 30) {
    (void) sprintf(formatString, "%%.%d%c", digits, conversion);
  } else {
    (void) sprintf(formatString, "%%%c", conversion);
  }
  (void) sprintf(s, formatString, f);
  setString(s);
}

CF_INLINE
CConxString::CConxString(const char *s) throw(const char *)
 : data(NULL), dsize(0)
{
  MMM1("CConxString(const char *s)");
  setString(s);
}

CF_INLINE
CConxString::CConxString(const CConxString &s) throw(const char *)
  : CConxObject(s), data(NULL), dsize(0) // DLC do CConxObject copy constructor for all children.
{
  MMM1("CConxString(const CConxString &s)");
  setString(s.getString());
}

CF_INLINE
CConxString::~CConxString()
{
  MMM1("~CConxString()");
  Clear();
}

NF_INLINE
void CConxString::setString(const char *s) throw(const char *)
{
  // If s is NULL, then the string is cleared.  Otherwise, a copy
  // of s becomes this object's value.  This will throw a const char *
  // exception if memory for the copy could not be allocated.

  MMM1("setString(const char *s) throw(const char *)");
  Clear();
  if (s != NULL) {
    dsize = 1+strlen(s);
    Boole caught = FALSE;
    try {
      data = new char[dsize];
    } catch (...) {
      caught = TRUE;
    }
    if (caught || data == NULL) {
      dsize = 0;
      throw "Out of memory in CConxString::setString";
    }
    strcpy(data, s);
  }
}

NF_INLINE
void CConxString::setString(const CConxString &s) throw(const char *)
{
  setString(s.getString());
}

NF_INLINE
const char *CConxString::getString() const
{
  // Returns the current value of this object, which could change
  // if you subsequently call a non-const method.

    // May return NULL if this object's value is not set, which could
    // be the result of setString(NULL).
  assert(data == NULL || dsize >= 1);
  assert(data != NULL || dsize == 0);
  assert(data == NULL || *(data+dsize-1) == '\0');
  return data;
}

PF_INLINE
ostream &CConxString::printOn(ostream &o) const
{
  const char *s = getString();
  if (s == NULL) {
    o << "<Blank CConxString Object>";
  } else {
    o << s;
  }
  return o;
}

NF_INLINE
Boole CConxString::equals(const char *s, Boole case_sensitive = TRUE) const
{
  const char *ts = getString();
  if (s == NULL) return (ts == NULL);
  return ((case_sensitive) ? (!strcmp(s, ts)) : (!strcasecmp(s, ts)));
}

NF_INLINE
Boole CConxString::equals(const CConxString &s,
                          Boole case_sensitive = TRUE) const
{
  return equals(s.getString(), case_sensitive);
}

NF_INLINE
Boole CConxString::hasPrefix(const char *s, size_t n,
                             Boole case_sensitive = TRUE) const
// If s agrees in the first n characters in a case_sensitive fashion,
// then TRUE is returned.  (i.e., this is the complete string).
{
  const char *ts = getString();
  if (s == NULL) return (ts == NULL);
  if (ts == NULL) return FALSE;
  return conx_prefix_streq(ts, s, n, case_sensitive);
}

NF_INLINE
Boole CConxString::hasPrefix(const CConxString &s, size_t n,
                             Boole case_sensitive = TRUE) const
{
  return hasPrefix(s.getString(), case_sensitive, n);
}

NF_INLINE
Boole CConxString::isPrefixOf(const char *s, size_t n,
                              Boole case_sensitive = TRUE) const
// If this string agrees with s in the first n characters in a
// case_sensitive fashion, then TRUE is returned.
{
  const char *ts = getString();
  if (s == NULL) return (ts == NULL);
  if (ts == NULL) return FALSE;
  return conx_prefix_streq(s, ts, n, case_sensitive);
}

NF_INLINE
Boole CConxString::isPrefixOf(const CConxString &s, size_t n,
                              Boole case_sensitive = TRUE) const
{
  return isPrefixOf(s.getString(), case_sensitive, n);
}

NF_INLINE
void CConxString::append(const char *s) throw(const char *)
{
  // If s == NULL or s == "", this object does not change.
  // Otherwise, s is tacked on to the end of this string.
  if (s != NULL && *s != '\0') {
    size_t new_size = strlen(s)+((dsize == 0)?1:dsize);
    // realloc() would be nice, but we're using new and delete.
    char *newdata = NULL; // to silence -W
    Boole caught = FALSE;
    try {
      newdata = new char[new_size];
    } catch (...) { // catch all
      caught = TRUE;
    }
    if (caught || newdata == NULL) throw "Out of memory in CConxString::append";
    (void) sprintf(newdata, "%s%s", (data != NULL) ? data : "", s);
    replaceString(newdata, new_size);
  }
}

NF_INLINE
CConxString CConxString::repetition(size_t n) const throw(const char *)
{

  CConxString temp;
  if (data == NULL) return temp;
  if (*data == '\0' || n == 0) {
    temp.setString("");
  } else {
    assert(dsize > 1);
    char *newdata = NULL; // to silence -W
    size_t new_size = 1+(dsize-1)*n;
    Boole caught = FALSE;
    try {
      newdata = new char[new_size];
    } catch (...) { // catch all
      caught = TRUE;
    }
    if (caught || newdata == NULL)
      throw "Out of memory in CConxString::repetition";

    char *working_newdata = newdata;
    for (size_t i = 0; i < n; i++) {
      strncpy(working_newdata, data, dsize-1);
      working_newdata += dsize-1;
    }
    *working_newdata = '\0';
    temp.replaceString(newdata, new_size);
  }
  return temp;
}

NF_INLINE
CConxString &CConxString::operator=(const CConxString &o) throw(const char *)
{
  MMM1("CConxString &operator=(const CConxString &o)");
  (void) CConxObject::operator=(o); // DLC do this for all children of Object
  setString(o.getString());
  return *this;
}

NF_INLINE
int CConxString::operator==(const char *s) const
{
  MMM1("operator==(const char *s)");
  return equals(s);
}

NF_INLINE
int CConxString::operator!=(const char *s) const
{
  return !operator==(s);
}

NF_INLINE
CConxString CConxString::operator+(const char *s) const throw(const char *)
{
  CConxString cat(*this);
  cat.append(s);
  return cat;
}

NF_INLINE
CConxString CConxString::operator+(const CConxString &s) const throw(const char *)
{
  return operator+(s.getString());
}

NF_INLINE
CConxString &CConxString::operator+=(const char *s) throw(const char *)
{
  append(s);
  // This better stay equivalent to `*this = operator+(s);'
  return *this;
}

NF_INLINE
CConxString &CConxString::operator*=(size_t n) throw(const char *)
{
  (void) operator=(operator*(n));
  return *this;
}

NF_INLINE
CConxString CConxString::operator*(size_t n) const throw(const char *)
{
  return repetition(n);
}

NF_INLINE
CConxString::operator const char*(void) const
{
  // This means that you can use `(const char *)CConxString("t123")',
  // but be careful about subsequently modifying the object.
  //
  // This is especially handy for eliminating overloaded functions
  // that take a `const CConxString &' argument and just use its
  // getString() method.
  return getString();
}

NF_INLINE
void CConxString::replaceString(char *s, size_t sz)
{
  // Steals s, which must have been allocated with new [].
  Clear();
  data = s;
  dsize = sz;
}

NF_INLINE
void CConxString::Clear()
{
  if (data != NULL) {
    delete [] data; // DLC compat delete [dsize] data
    data = NULL;
    dsize = 0;
  } else {
    assert(dsize == 0);
  }
}

int operator==(const char *s, const CConxString &a)
{
  return a.operator==(s);
}

int operator!=(const char *s, const CConxString &a)
{
  return !operator==(s, a);
}

CConxString operator+(const char *s, const CConxString &a)
{
  LLL1("CConxString operator+(const char *s, const CConxString &a)");
  return CConxString(s).operator+(a);
}

CConxString operator*(size_t n, const CConxString &a)
{
  LLL1("CConxString operator*(size_t n, const CConxString &a)");
  return a.repetition(n);
}


OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxString)
