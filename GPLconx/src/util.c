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
  Utility functions.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"

int conx_str2ul(const char *ulrep, unsigned long *a)
/* Returns zero and sets *a to the unsigned integer that ulrep represents
   if ulrep is a string of the digits '0' through '9'.  No
   plus or minus signs or whitespace is allowed.  Otherwise a positive number
   is returned.  The integer that ulrep represents must also fit inside an
   unsigned long, or a negative number is returned.
   Otherwise, non-zero is returned.
*/
{
  unsigned long ul, i = 0;
  char *badplace;

  if (a == NULL || ulrep == NULL) return 2;
  if (ulrep[0] == '\0') return 1;
  while (ulrep[i] != '\0') {
    if (!isdigit((int) ulrep[i])) return 1;
    ++i; /* isdigit might be a macro */
  }

  errno = !ERANGE;
  ul = strtoul(ulrep, &badplace, 10);
  if (*badplace == '\0') {
    if (errno == ERANGE)
      return -1;
    *a = ul;
    return 0;
  } else
    return 1;
}

int conx_str2l(const char *lrep, long *a)
/* Returns zero and sets *a to the possibly signed integer that lrep
   represents if lrep is a string of the digits '0' through '9'.
   Whitespace is allowed at the start of lrep only.
   Otherwise a positive number is returned.  The integer that lrep
   represents must also fit inside a long, or a negative number is returned.
   Otherwise, non-zero is returned.
*/
{
  long l;
  char *badplace;

  if (a == NULL || lrep == NULL) return 2;
  if (lrep[0] == '\0') return 1;

  errno = !ERANGE;
  l = strtol(lrep, &badplace, 10);
  if (*badplace == '\0') {
    if (errno == ERANGE)
      return -1;
    *a = l;
    return 0;
  } else
    return 1;
}

int conx_str2d(const char *realrep, double *a)
/* Returns zero and sets *a to point to the real number represented by the
   string realrep if realrep is a string of the digits '0' through '9'
   optionally followed by an 'e' or 'E', then a '+' or '-', then another
   string of digits.
   No leading plus or minus sign or leading whitespace is allowed.
   Otherwise a positive number is returned.  The number that realrep
   represents must also fit inside double, or a negative number is returned.
*/
{
  char *badplace;
  double real;

  if (a == NULL || realrep == NULL) return 2;
  if (realrep[0] == '\0') return 1;

  errno = !ERANGE;

  real = strtod(realrep, &badplace);

  if (*badplace == '\0') {
    if (errno == ERANGE) return -1;
    *a = real;
    return 0;
  } else {
    return 1;
  }
}

int conx_strcmp(const char *a, const char *b, int caseful)
/* Returns strcmp(a, b) if caseful is non-zero, returns
   strcasecmp(a, b) otherwise.
*/
{
  if (a == NULL || b == NULL) return 1;
  return ((caseful) ? strcmp(a, b) : strcasecmp(a, b));
}

int conx_prefix_streq(const char *complete, const char *attempt, size_t n,
                      int caseful)
/* Returns non-zero if attempt matches complete entirely or in at least the
   first n characters; if attempt is longer than complete, there is no match.
*/
{
  size_t i = 0;

  assert(complete != NULL && attempt != NULL);

  if (caseful) {
    while (complete[i] == attempt[i])
      if (complete[i++] == '\0') return 1;
  } else {
    while (tolower(complete[i]) == tolower(attempt[i]))
      if (complete[i++] == '\0') return 1;
  }
  return (i >= n && attempt[i] == '\0');
}

int conx_file_exists(const char *fn)
/* Returns non-zero if the regular file @var{fn} exists;
   returns zero if it does not.
*/
{
  int rc;
  struct stat s;

  rc = stat(fn, &s);
  if (rc == -1) {
    /* errno will be ENOTDIR, ENOENT, EACCES, etc. */
    return 0;
  }
  return S_ISREG(s.st_mode);
  /* This can return false positives, use AC_HEADER_STAT
     if you can think of a reason not to pass them on.
  */
}

