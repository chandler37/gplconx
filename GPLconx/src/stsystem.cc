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
  Implementation of C++ classes in `stsystem.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ldict.h>

#include "stsystem.hh"
#include "sterror.hh"
#include "stsymbol.hh"
#include "ststring.hh"
#include "clsmgr.hh"

CConxOwnerArray<CConxClsAnsMach> *CClsSystem::ansMachs = NULL;


// DLC Linux Class Library ForEach Documentation is weak.
// This helps us access the variable dictionany in CConxClsManager:
class CConxForEachGetVars : public LForEach {
public:
  CConxForEachGetVars(CClsArray *m) { assert(m != NULL); arr = m; }
  ~CConxForEachGetVars() { }
  void process(void *value)
  {
    CConxClsManager::VarDictElem *v = (CConxClsManager::VarDictElem *)value;
    assert(arr != NULL);
    arr->append(new CClsSymbol(v->key)); // Entering GC
  }

private:
  CClsArray *arr;
}; // class CConxForEachGetVars

NF_INLINE
CClsBase::ErrType CClsSystem::iActionVars(CClsBase **result, CConxClsMessage &o)
{
  CClsArray *va = new CClsArray();
  if (va == NULL) OOM();
  if (getClsMgr() == NULL) throw "Bad init";
  CConxClsManager::VarDict &dict = getClsMgr()->getVarDict();
  dict.forEachItem(new CConxForEachGetVars(va));
  RETURN_NEW_RESULT(result, va);
}

NF_INLINE
CClsBase::ErrType CClsSystem::iActionAt(CClsBase **result, CConxClsMessage &o)
{
  ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsSymbol, argv, o);
  CConxClsManager *m = getClsMgr();
  if (m == NULL) throw "Bad init 0113";
  CClsBase *pointee = m->getModifiableNamedThing(argv[0]->getValue());
  if (pointee == NULL) {
    RETURN_ERROR_RESULT(result, "That potential variable has not yet been defined (by simply assigning to it)");
  } else {
    RETURN_EXISTING(result, pointee);
  }
}

NF_INLINE
void CClsSystem::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();
    // There is no "new", we don't allow object instances.
    ST_METHOD(ansMachs, "exit", CLASS, iAnswererExit,
              "Exits the system");
    ST_METHOD(ansMachs, "helpMe", CLASS, iAnswererHelpMe,
              "Gets high-level help");
    ST_METHOD(ansMachs, "vars", CLASS, iAnswererVars,
              "Returns an Array of Symbols that are defined variables");
    ST_METHOD(ansMachs, "at:", CLASS, iAnswererAt,
              "Returns that to which the variable whose name is given as a Symbol argument points");
    // DLC "at:put:" "unset:"
  }
}

NF_INLINE
CClsBase::ErrType 
CClsSystem::iActionExit(CClsBase **result, CConxClsMessage &o)
{
  // DLC do final cleanup to find bugs.
  exit(0);
}

NF_INLINE
CClsBase::ErrType 
CClsSystem::iActionHelpMe(CClsBase **result, CConxClsMessage &o)
{
  RETURN_NEW_RESULT(result, new CClsStringLiteral("This is the Smalltalk-80-ish system that " PACKAGE " uses to give you incredible expressive power (um, one day...).  The system's documentation is in the Texinfo document that you'll find bundled with the source, which is freely available since " PACKAGE " is GPL'ed.  To get up to speed more quickly, download and check out GNU Smalltalk, which is full-featured and better-documented.\nAnother good source of help is by typing `Object help', or `System help', or `2 help', or `0.3 help', e.g.  This gives you information about the class of the receiver, such as which methods work.  There are no binary methods in this system, and you cannot extend it unless you alter the C++ source code.")); // DLC point to sourceforge.

  // DLC print strings with formatting! Let tab cause tabbing that far and then another tab for indenting.
}

NF_INLINE
CConxString CClsSystem::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    return CConxString("an ") + getClsName();
  }
}

NF_INLINE
CClsSystem &CClsSystem::operator=(const CClsSystem &o)
{
  (void) CClsBase::operator=(o);
  cm = o.cm;
  return *this;
}

