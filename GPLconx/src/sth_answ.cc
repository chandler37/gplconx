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
  Implementation of C++ classes in `sth_answ.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sth_answ.hh"
#include "sth_msg.hh"
#include "stobject.hh"

NF_INLINE
int /*CClsBase::ErrType*/
CConxClsAnsMach::answer(CClsBase *receiver, CClsBase **result,
                        CConxClsMessage &o) throw(const char *)
{
  if (receiver == NULL) throw "receiver cannot be NULL";
  if (result == NULL) throw "result cannot be NULL";
  if (!o.isSet()) throw "the message must be set";
  if (ansMach == NULL) throw "the Answerer has not yet been set.";
  assert(receiver->isClassInstance() == isClassMessage());
  assert(getMessageName().getLength() > 0);
  assert(o.isMessageNamed(getMessageName()));
  return (*ansMach)(receiver, result, o);
}

Boole AnswerersRespondsTo(const Answerers &a, const CConxString &method,
                          Boole forClassInstances)
// Returns TRUE if a answers method forClassInstances or for object instances
{
  for (size_t i = 0; i < a.size(); i++) {
    const CConxClsAnsMach &answ = a.get(i);
    if (answ.isClassMessage() == forClassInstances
        && answ.getMessageName() == method)
      return TRUE;
  }
  return FALSE;
}

CConxString getCompleteHelpMessage(const CClsBase &c, const Answerers &a)
{
  CConxString e = getClassHelpMessage(c);
  e += "\n\n" + getClassInstanceHelpMessage(a);
  e += "\n\n" + getObjectInstanceHelpMessage(a);
  return e;
}

CConxString getClassHelpMessage(const CClsBase &c)
{
  CConxString e;
  CClsBase::ClsType superType = c.getSuperClassType();
  if (superType == c.CLS_INVALID)
    e = "nil";
  else {
    e = "#";
    e += CClsBase::getClsNameByType(superType);
  }
  e += " subclass:  #";
  e += c.getClsName();
  e += "\n\nObject comment:\n'";
  e += c.getClsComment();
  e += "'";
  return e;
}

CConxString getObjectInstanceHelpMessage(const Answerers &a)
{
  CConxString s = "Instance methods:\n";
  for (size_t i = 0; i < a.size(); i++) {
    const CConxClsAnsMach &r = a.get(i);
    if (!r.isClassMessage())
      s += "\t" + r.getMessageName() + " (" + r.getCategory() + ") -- " + r.getHelpTip() + "\n";
  }
  return s;
}

CConxString getClassInstanceHelpMessage(const Answerers &a)
{
  CConxString s = "Class methods:\n\n";
  for (size_t i = 0; i < a.size(); i++) {
    const CConxClsAnsMach &r = a.get(i);
    if (r.isClassMessage())
      s += "\t" + r.getMessageName() + " (" + r.getCategory() + ") -- " + r.getHelpTip() + "\n";
  }
  return s;
}

CF_INLINE
CConxClsAnsMach::CConxClsAnsMach(const CConxString &name,
                                 MethodType clsMessage, Answerer *a,
                                 const CConxString &help)
  : category("miscellaneous")
{
  setMessageName(name, help, clsMessage);
  ansMach = a;
}

CF_INLINE
CConxClsAnsMach::CConxClsAnsMach(const CConxString &name,
                                 MethodType clsMessage, Answerer *a,
                                 const CConxString &category,
                                 const CConxString &help)
{
  setMessageName(name, help, clsMessage);
  this->category = category;
  ansMach = a;
}

CF_INLINE
CConxClsAnsMach::CConxClsAnsMach(const CConxClsAnsMach &o)
  : CConxObject(o)
{
  uninitializedCopy(o);
}

NF_INLINE
CConxClsAnsMach &CConxClsAnsMach::operator=(const CConxClsAnsMach &o)
{
  (void) CConxObject::operator=(o);
  uninitializedCopy(o);
  return *this;
}

NF_INLINE
int CConxClsAnsMach::operator==(const CConxClsAnsMach &o) const
{
  return (ansMach == o.ansMach
          && msgName == o.msgName
          && helpTip == o.helpTip
          && category == o.category
          && classMessage == o.classMessage);
}

NF_INLINE
void CConxClsAnsMach::setMessageName(const CConxString &nm,
                                     const CConxString &help,
                                     MethodType clsMessage)
{
  msgName = nm;
  helpTip = help;
  classMessage = (clsMessage == CLASS);
}

NF_INLINE
void CConxClsAnsMach::setMessageName(const CConxString &nm,
                                     const CConxString &category,
                                     const CConxString &help,
                                     MethodType clsMessage)
{
  setCategory(category);
  msgName = nm;
  helpTip = help;
  classMessage = (clsMessage == CLASS);
}

NF_INLINE
void CConxClsAnsMach::uninitializedCopy(const CConxClsAnsMach &o)
{
  msgName = o.msgName;
  ansMach = o.ansMach;
  helpTip = o.helpTip;
  category = o.category;
  classMessage = o.classMessage;
}
