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
  Implementation of C++ classes in `stboole.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stboole.hh"
#include "ststring.hh"
#include "sterror.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsBoolean::ansMachs = NULL;

NF_INLINE
void CClsBoolean::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();

    // DLC for efficiency, don't allow `Boolean true' and `Boolean false',
    // force copying `true' and `false'.
    ansMachs->append(CConxClsAnsMach("help", CConxClsAnsMach::CLASS,
                                     ciAnswererHelp,
                                     "Returns help on this class's methods"));
    ansMachs->append(CConxClsAnsMach("true", CConxClsAnsMach::CLASS,
                                     ciAnswererTrue,
                                     "Returns a new object instance that represents truth.  The system predefines such an instance and assigns it to the variable `true'; you will benefit from increased efficiency if you just use `true'."));
    ansMachs->append(CConxClsAnsMach("false", CConxClsAnsMach::CLASS,
                                     ciAnswererFalse,
                                     "Returns a new object instance that represents falsehood.  The system predefines such an instance and assigns it to the variable `false'; you will benefit from increased efficiency if you just use `false'"));

    // object instance methods:
    ansMachs->append(CConxClsAnsMach("not", CConxClsAnsMach::OBJECT,
                                     oiAnswererNot,
                                     "Returns a Boolean object instance whose truth value is the opposite of the receiver"));
    ansMachs->append(CConxClsAnsMach("and:", CConxClsAnsMach::OBJECT,
                                     oiAnswererAnd,
                                     "Returns true iff the reciever and the Boolean argument are both true"));
    ansMachs->append(CConxClsAnsMach("or:", CConxClsAnsMach::OBJECT,
                                     oiAnswererOr,
                                     "Returns true iff either the reciever or the Boolean argument is true"));
    ansMachs->append(CConxClsAnsMach("xor:", CConxClsAnsMach::OBJECT,
                                     oiAnswererXOr,
                                     "Returns true iff either the reciever or the Boolean argument is true but both are not"));
  }
}

NF_INLINE
CClsBase::ErrType 
CClsBoolean::oiActionAnd(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[1]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_BOOLEAN, TRUE);
  RETURN_NEW_RESULT(result,
                    new CClsBoolean(getValue() \
                                    && ((CClsBoolean *)argv[0])->getValue()));
}

NF_INLINE
CClsBase::ErrType 
CClsBoolean::oiActionOr(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[1]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_BOOLEAN, TRUE);
  RETURN_NEW_RESULT(result,
                    new CClsBoolean(getValue()
                                    || ((CClsBoolean *)argv[0])->getValue()));
}

NF_INLINE
CClsBase::ErrType 
CClsBoolean::oiActionXOr(CClsBase **result, CConxClsMessage &o) const
{
  CClsBase *argv[1]; o.getBoundObjects(argv);
  ENSURE_KEYWD_TYPE(result, o, argv, 0, CLS_BOOLEAN, TRUE);
  Boole oval = ((CClsBoolean *)argv[0])->getValue();
  RETURN_NEW_RESULT(result, new CClsBoolean((getValue() && !oval)
                                            || (!getValue() && oval)));
}

NF_INLINE
CConxString CClsBoolean::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    return CConxString(d ? "true" : "false");
  }
}

NF_INLINE
CClsBoolean &CClsBoolean::operator=(const CClsBoolean &o)
{
  (void) CClsBase::operator=(o);
  d = o.d;
  return *this;
}

NF_INLINE
int CClsBoolean::operator==(const CClsBoolean &o)
{
  if (isClassInstance() != o.isClassInstance()) return 0;
  return d == o.d;
}

NF_INLINE
CClsBase::ErrType 
CClsBoolean::ciActionTrue(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result, new CClsBoolean(TRUE));
}

NF_INLINE
CClsBase::ErrType 
CClsBoolean::ciActionFalse(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result, new CClsBoolean(FALSE));
}

NF_INLINE
CClsBase::ErrType 
CClsBoolean::oiActionNot(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result, new CClsBoolean(!getValue()));
}

NF_INLINE
CClsBase::ErrType 
CClsBoolean::ciActionHelp(CClsBase **result, CConxClsMessage &o) const
{
  RETURN_NEW_RESULT(result,
                    new CClsStringLiteral(getCompleteHelpMessage(*this,
                                                                 *ansMachs)));
}

