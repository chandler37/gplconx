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

#ifndef GPLCONX_CSTRING_CXX_H
#define GPLCONX_CSTRING_CXX_H 1

#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "printon.hh"
#include "decls.hh"

//////////////////////////////////////////////////////////////////////////////
// A general replacement for char *:
class CConxString
  : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxString")
public:
  CConxString();

  CConxString(long l) throw(const char *);
  // l == 3 will cause this string to be "3", e.g.

  CConxString(double f, char conversion = 'g', int digits = -1)
    throw(const char *);
  // f == 3.0 will cause this string to be "3.0", e.g.
  // Throws if there is not enough memory or if conversion is not in [eEfFgG]

  CConxString(const char *s) throw(const char *);
  CConxString(const CConxString &s) throw(const char *);
  ~CConxString();

  // If s is NULL, then the string is cleared.  Otherwise, a copy
  // of s becomes this object's value.  This will throw a const char *
  // exception if memory for the copy could not be allocated.

  void setString(const char *s) throw(const char *);

  void setString(const CConxString &s) throw(const char *);
  const char *getString() const;

  // Remember to delete [] it.
  char *getStringAsNewArray() const;

  ostream &printOn(ostream &o) const;
  Boole equals(const char *s, Boole case_sensitive = TRUE) const;
  Boole equals(const CConxString &s, Boole case_sensitive = TRUE) const;
  Boole hasPrefix(const char *s, size_t n,
                  Boole case_sensitive = TRUE) const;
  Boole hasPrefix(const CConxString &s, size_t n,
                  Boole case_sensitive = TRUE) const;
  Boole isPrefixOf(const char *s, size_t n,
                   Boole case_sensitive = TRUE) const;
  Boole isPrefixOf(const CConxString &s, size_t n,
                   Boole case_sensitive = TRUE) const;

  // Returns zero for an unset object or an object set to "";
  // use getString() to distinguish between the two if necessary.
  size_t getLength() const { return ((dsize == 0) ? 0 : (dsize - 1)); }

  // Returns the nth character, which is '\0' if the character
  // is past the end of the string.  n == 0 is the first character.
  char getNthChar(size_t n) const;

  // Returns the number of times c occurs in the string.  '\0' is contained
  // 1 time for a set string and zero times for an unset string.
  size_t numberOf(char c) const;

  // Returns the substring that begins with the character at index start
  // (start == 0 for the first character) and continues for length characters.
  // If length is zero, then you get an unset object.
  // If length is not zero but start is past the end of the string, then you
  // get an object set to "".
  // If length is longer than is possible, you get from start to the end of
  // the string.
  CConxString substring(size_t start, size_t length) const throw(const char *);

  void append(const char *s) throw(const char *);

  // Returns a new object that is equal to this object repeated n times.
  CConxString repetition(size_t n) const throw(const char *);

  CConxString &operator=(const CConxString &o) throw(const char *);
  int operator==(const char *s) const;
  int operator!=(const char *s) const;
  int operator==(const CConxString &s) const { return operator==(s.getString()); }
  int operator!=(const CConxString &s) const { return operator!=(s.getString()); }

  // `+' concatenates.
  CConxString operator+(const char *s) const throw(const char *);
  CConxString operator+(const CConxString &s) const throw(const char *);
  CConxString &operator+=(const char *s) throw(const char *);
  CConxString &operator*=(size_t n) throw(const char *);
  CConxString operator*(size_t n) const throw(const char *);
  operator const char*(void) const;


// Thanks to the (const char *) casting operator, we don't need:
//
//   void append(const CConxString &s) throw(const char *)
//   {
//     append(s.getString());
//   }

protected:
  void replaceString(char *s, size_t sz);

private:
  void Clear();

private:
  mutable char *data; // mutable due to an efficient substring() implementation
  size_t dsize;
}; // class CConxString

int operator==(const char *s, const CConxString &a);
int operator!=(const char *s, const CConxString &a);
CConxString operator+(const char *s, const CConxString &a);
CConxString operator*(size_t n, const CConxString &a);

OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxString);

#endif // GPLCONX_CSTRING_CXX_H
