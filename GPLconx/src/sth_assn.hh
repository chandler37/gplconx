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
  C++ Smalltalkish back-end CConxClsAssignmentList class.
*/

#ifndef GPLCONX_STH_ASSN_CXX_H
#define GPLCONX_STH_ASSN_CXX_H 1

#include "Starray.hh"
#include "CString.hh"

//////////////////////////////////////////////////////////////////////////////
// A class that represents a list of assignment statements.  This is useful
// for implementing `L := M := N := 3.3'
class CConxClsAssignmentList : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsAssignmentList")
public:
  CConxClsAssignmentList() { }
  CConxClsAssignmentList(const CConxClsAssignmentList &o);
  CConxClsAssignmentList &operator=(const CConxClsAssignmentList &o);
  ~CConxClsAssignmentList() { }
  // DLC operator== and !=
  size_t getNumberVars() const { return vars.size(); }
  void appendVar(const CConxString &v) { vars.append(v); }
  const CConxString &getNthVar(size_t n) const throw(const char *);
  ostream &printOn(ostream &o) const;


private:
  CConxStringArray vars;
}; // class CConxClsAssignmentList

#endif // GPLCONX_STH_ASSN_CXX_H
