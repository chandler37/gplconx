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
  C++ Smalltalkish back-end CConxClsAnsMach class.
*/

#ifndef GPLCONX_STH_ANSW_CXX_H
#define GPLCONX_STH_ANSW_CXX_H 1

#include "CString.hh"
#include "CArray.hh"


class CClsBase;
class CConxClsMessage;
//////////////////////////////////////////////////////////////////////////////
// An answering machine just accepts messages.  In this case, the messages
// are CConxClsMessage instances.
class CConxClsAnsMach : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsAnsMach")
  DEFAULT_PRINTON()
public: // types
  enum MethodType { OBJECT = FALSE, CLASS = TRUE };
  typedef int /*CClsBase::ErrType*/ (Answerer) (CClsBase *receiver,
                                                CClsBase **result,
                                                CConxClsMessage &o);
public: // methods
  CConxClsAnsMach() { ansMach = NULL; classMessage = FALSE; }
  CConxClsAnsMach(const CConxString &name, MethodType clsMessage,
                  Answerer *a, const CConxString &help);
  CConxClsAnsMach(const CConxString &name, MethodType clsMessage,
                  Answerer *a, const CConxString &category,
                  const CConxString &help);
  CConxClsAnsMach(const CConxClsAnsMach &o);
  CConxClsAnsMach &operator=(const CConxClsAnsMach &o);
  ~CConxClsAnsMach() { }
  
  int operator==(const CConxClsAnsMach &o) const;
  int operator!=(const CConxClsAnsMach &o) const { return !operator==(o); }
  
  Boole isClassMessage() const { return classMessage; }
  const CConxString &getMessageName() const { return msgName; }
  const CConxString &getHelpTip() const { return helpTip; }
  void setCategory(const CConxString &m) { category = m; }
  const CConxString &getCategory() const { return category; }


  // DLC not checking integrity!  Be careful with nm
  void setMessageName(const CConxString &nm, const CConxString &help,
                      MethodType clsMessage);
  void setMessageName(const CConxString &nm, const CConxString &category,
                      const CConxString &help, MethodType clsMessage);

  void setAnswerer(Answerer *o) { ansMach = o; }
  int /*CClsBase::ErrType*/ answer(CClsBase *receiver,
                                   CClsBase **result,
                                   CConxClsMessage &o) throw(const char *);

private:
  void uninitializedCopy(const CConxClsAnsMach &o);

private:
  Answerer *ansMach;
  CConxString msgName, helpTip, category;
  Boole classMessage;
}; // class CConxClsAnsMach


typedef CConxPrintableOwnerArray<CConxClsAnsMach> Answerers;

CConxString getClassInstanceHelpMessage(const Answerers &a);
CConxString getObjectInstanceHelpMessage(const Answerers &a);
CConxString getClassHelpMessage(const CClsBase &c);
CConxString getCompleteHelpMessage(const CClsBase &c, const Answerers &a);
Boole AnswerersRespondsTo(const Answerers &a, const CConxString &method,
                          Boole forClassInstances);

//////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////


#endif // GPLCONX_STH_ANSW_CXX_H
