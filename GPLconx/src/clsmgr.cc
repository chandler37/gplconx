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
  Implementation of C++ classes in `clsmgr.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "clsmgr.hh"

NF_INLINE
long CConxClsManager::makeAssignments(const CConxClsAssignmentList &al,
                                      CClsBase *assignTo)
  // Returns -1 if assignTo is not a valid instance.
  // Returns the number of failures due to attempting to assign to classes,
  // or zero on success.
{
  MMM("long makeAssignments(const CConxClsAssignmentList &al, CClsBase *assignTo)");
  if (assignTo == NULL) return -1;
  long retval = 0;
  //  cout << "DLC alist is " << flush << al << endl;
  for (size_t i = 0; i < al.getNumberVars(); i++) {
    if (makeAssignment(al.getNthVar(i), assignTo))
      ++retval;
  }
  return retval;
}

NF_INLINE
Boole CConxClsManager::makeAssignment(const CConxString &id,
                                      CClsBase *v) throw(const char *)
// Call to implement `F := G' with id == "F" and
// v == the thing G points to, e.g.
// If v is NULL, a const char * is thrown.
// Returns TRUE on failure, if you try to assign `Circle := 3.3', a reserved word.
// Returns FALSE on success, e.g. `C := Circle' or `C := 3.3'
  
// DLC we should allow reassignment like `C := Circle. C := 3.3'
{
  MMM("Boole makeAssignment(const CConxString &id, CClsBase *v) throw(const char *)");

  LLL("assigning " << id << " to " << v << " which prints as:" << *v);

  if (v == NULL) throw ("never");

  if (isReservedWord(id.getString())) {
    return TRUE; // Do not allow `Float := 3.3', e.g.
  }

  // If we are reassigning, then do garbage collection.

  // Be careful not to delete what we are about to point to.
  LLL("makeAssignment is about to void incrementUsers()");
  v->incrementUsers();

  CClsBase *ref;
  CConxClsVariable *varPtr;
  if (isVariableName(id)) {
    ref = getModifiableNamedThing(id);
    removeUser(ref); // GC
    varPtr = getVariable(id);
  } else {
    varPtr = newVariable(FALSE, id);
  }
  assert(varPtr != NULL);
  varPtr->setPointee(v);
  return FALSE;
}

CF_INLINE
CConxClsManager::~CConxClsManager()
{
  MMM("~CConxClsManager()");
  // EEEk DLC DLC DLC This is not trivial.

  // Do garbage collection.

  // Undo the effects of all the addInstance() calls and the firstInit()
  // call.

  unsigned long asz = array.size();
  for (unsigned long i = 0; i < asz; i++) {
    CClsBase *bptr = array.get(i);
    if (bptr != NULL) {
      // Relies on the virtual destructor provided by CClsBase:
      LLL("~CConxClsManager 650 " << i << " about to delete "
          << bptr << " which is " << "of type "
          << CClsBase::clsTypeToString(bptr->getType())
          << " which prints as " << *bptr << endl);
      delete bptr;
    }
  }

  // DLC kill dict so that the variables's pointees have their GC counts
  // decremented.
}


OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsManager)

