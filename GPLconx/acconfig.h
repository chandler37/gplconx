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

#ifndef GPLCONX_CFG_H
#define GPLCONX_CFG_H 1

/* Yes, I know I should test for this. DLC TODO
   If you have the const_cast<> operator, define the following:
 */
#define HAVE_CONST_CAST

/* Comment this out to get a single-buffered tconx: */
#define TCONX_DOUBLE_BUFFER 1

/* If your C++ compiler supports the `using' keyword and not the
   deprecated method of granting access to inherited members, then
   define CONX_USING to be `using'.  Otherwise, leave it blank.
*/
#define CONX_USING /* Nothing */

/* If `make check' fails, then you should define this so that
   differences in outputting and inputting floating-point numbers
   will not cause a failure.
*/
/* #define NO_IFFY_TESTS */

@TOP@
@BOTTOM@
#endif /* GPLCONX_CFG_H */
